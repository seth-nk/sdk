/* 
 * copyright 2019 NSWA Maintainers
 * copyright 2019 The-Seth-Project
 * License: MIT
 * This is free software with ABSOLUTELY NO WARRANTY.
 */

#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700
#define _BSD_SOURCE
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>
#include <libgen.h>

extern "C" {
#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>
#include <NetworkManager.h>
}

#include "../backend.h"
#include "../version.h"
#include "../config.h"

static const char *uuid = "ac00a48e-89a5-456d-8b95-4bec0c45c0c5";

class SDBackendNmcli : public SDBackend {
private:
	bool dialup_result;
	std::string lasterror;
	GMainLoop *loop;

	static void cb_add_connection(GObject *client, GAsyncResult *result, gpointer user_data);
	static void cb_active(GObject *client, GAsyncResult *res, gpointer user_data);
	static void activate_ac_state_changed(NMActiveConnection *active_connection, guint state, guint reason, gpointer user_data);

public:
	virtual std::string getid() { return "nmcli"; };
	virtual std::string getname() { return "NetworkManager D-Bus client"; };
	virtual bool dialup(const char *usrnam, const char *passwd);
	virtual bool hangup();
	virtual bool getstat();
	virtual std::string get_last_error();
};

__attribute__((constructor)) static void backend_nmcli_main(void)
{
	SDBackendNmcli *backend = new SDBackendNmcli;
	SDBackendManager::register_backend(backend);
}

void SDBackendNmcli::activate_ac_state_changed(NMActiveConnection *active_connection, guint state, guint reason, gpointer user_data)
{
	SDBackendNmcli *backend = (SDBackendNmcli *)user_data;
	GMainLoop *loop = backend->loop;

	if (state != NM_ACTIVE_CONNECTION_STATE_ACTIVATING &&
		state != NM_ACTIVE_CONNECTION_STATE_DEACTIVATING) {
		if (state == NM_ACTIVE_CONNECTION_STATE_ACTIVATED) {
			backend->dialup_result = true;
		}
		backend->lasterror = "error connecting to peer, please see NetworkManager log";
		g_main_loop_quit(loop);
	}
}

void SDBackendNmcli::cb_active(GObject *client, GAsyncResult *res, gpointer user_data)
{
	SDBackendNmcli *backend = (SDBackendNmcli *)user_data;
	GMainLoop *loop = backend->loop;
	GError *error = nullptr;
	
	NMActiveConnection *ac = nm_client_activate_connection_finish(NM_CLIENT(client), res, &error);
	
	if (error) {
		backend->lasterror = std::string("error activing connection, libnm reported: ") + error->message;
		g_error_free(error);
		g_main_loop_quit(loop);
	} else {
		g_signal_connect(ac, "state-changed", G_CALLBACK(&SDBackendNmcli::activate_ac_state_changed), user_data);
		
	}
}

void SDBackendNmcli::cb_add_connection(GObject *client, GAsyncResult *result, gpointer user_data)
{
	SDBackendNmcli *backend = (SDBackendNmcli *)user_data;
	GMainLoop *loop = backend->loop;
	GError *error = nullptr;

	NMRemoteConnection *conn = nm_client_add_connection_finish(NM_CLIENT(client), result, &error);

	if (error) {
		backend->lasterror = std::string("error adding nm connection, libnm reported: ") + error->message;
		g_error_free(error);
		g_main_loop_quit(loop);
	} else {
		g_object_unref(conn);
		nm_client_activate_connection_async(NM_CLIENT(client), NM_CONNECTION(conn), nullptr, nullptr, nullptr, &SDBackendNmcli::cb_active, user_data);
	}
}

bool SDBackendNmcli::dialup(const char *usrnam, const char *passwd)
{
	NMClient *client;
	NMRemoteConnection *conn;
	GError *error = nullptr;

	NMConnection *connection;
	NMSettingConnection *s_con;
	NMSettingPppoe *s_pppoe;
	NMSettingIP4Config *s_ip4;
	NMSettingIP6Config *s_ip6;

	this->dialup_result = false;
	this->lasterror = "";

	const char *ifname = Config::get("pppoe", "interface", "eth0");
	int ipv6enable = Config::get_int("pppoe", "enable_ipv6");

	client = nm_client_new(nullptr, &error);
	if (!client) {
		lasterror = std::string("error connecting to NetworkManager, libnm reported: ") + error->message;
		g_error_free(error);
		goto err_client_new;
	}

	if ((conn = nm_client_get_connection_by_uuid(client, uuid)) != nullptr) {
		if (nm_remote_connection_delete(conn, nullptr, &error) == FALSE) {
			lasterror = std::string("error deleting existing connection, libnm reported: ") + error->message;
			goto err_exist_del;
		}
	}

	this->loop = g_main_loop_new(nullptr, FALSE);
	connection = nm_simple_connection_new();

	s_con = (NMSettingConnection*)nm_setting_connection_new();
	g_object_set(G_OBJECT(s_con),
			NM_SETTING_CONNECTION_UUID, uuid,
			NM_SETTING_CONNECTION_ID, SETHD_PPPOE_IFNAME "nm",
			NM_SETTING_CONNECTION_INTERFACE_NAME, ifname,
			NM_SETTING_CONNECTION_TYPE, "pppoe",
			NM_SETTING_CONNECTION_AUTOCONNECT, FALSE,
			nullptr);
	nm_connection_add_setting(connection, NM_SETTING(s_con));

	s_pppoe = (NMSettingPppoe *)nm_setting_pppoe_new();
	g_object_set(G_OBJECT(s_pppoe),
			NM_SETTING_PPPOE_USERNAME, usrnam,
			NM_SETTING_PPPOE_PASSWORD, passwd,
			nullptr);
	nm_connection_add_setting(connection, NM_SETTING(s_pppoe));

	s_ip4 = (NMSettingIP4Config *)nm_setting_ip4_config_new();
	g_object_set(G_OBJECT(s_ip4),
			NM_SETTING_IP_CONFIG_METHOD, NM_SETTING_IP4_CONFIG_METHOD_AUTO,
			nullptr);
	nm_connection_add_setting(connection, NM_SETTING(s_ip4));

	s_ip6 = (NMSettingIP6Config *)nm_setting_ip6_config_new();
	g_object_set(G_OBJECT(s_ip6),
			NM_SETTING_IP_CONFIG_METHOD,
			(ipv6enable ? NM_SETTING_IP6_CONFIG_METHOD_AUTO : NM_SETTING_IP6_CONFIG_METHOD_IGNORE),
			nullptr);
	nm_connection_add_setting(connection, NM_SETTING(s_ip6));

	nm_client_add_connection_async(client, connection, FALSE, nullptr, &SDBackendNmcli::cb_add_connection, this);
	g_main_loop_run(loop);

	g_object_unref(connection);

err_exist_del:
	g_object_unref(client);
err_client_new:
	return this->dialup_result;
}

bool SDBackendNmcli::hangup()
{
	NMClient *client;
	NMRemoteConnection *conn;
	GError *error = nullptr;
	bool ret = false;

	client = nm_client_new(nullptr, &error);
	if (!client) {
		lasterror = std::string("error connecting to NetworkManager, libnm reported: ") + error->message;
		g_error_free(error);
		goto err_client_new;
	}

	if ((conn = nm_client_get_connection_by_uuid(client, uuid)) != nullptr) {
		if (nm_remote_connection_delete(conn, nullptr, &error) == FALSE) {
			lasterror = std::string("error deleting existing connection, libnm reported: ") + error->message;
			goto err_exist_del;
		}
	}

	ret = true;
err_exist_del:
	g_object_unref(client);
err_client_new:
	return ret;
}

bool SDBackendNmcli::getstat()
{
	/* TODO: please impl this function */
	return false;
}

std::string SDBackendNmcli::get_last_error()
{
	return this->lasterror;
}
