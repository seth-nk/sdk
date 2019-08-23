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
#ifndef _WIN32
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#endif // _WIN32

extern "C" {
#include <gio/gio.h>
}

#include "../backend.h"
#include "../config.h"
#include "../version.h"

class SDBackendRpPppoe : public SDBackend {
private:
	std::string lasterror;
	bool kernel;
	bool subprocess(const char **argv);
public:
	SDBackendRpPppoe(bool k) : kernel(k) {};
	virtual std::string getid() { return (kernel ? "rp-pppoe-linux" : "rp-pppoe"); };
	virtual std::string getname() { return (kernel ? "RP-PPPoE kernel mode + kernel pppoe mod (Linux only)": "RP-PPPoE user mode (Unix/Linux)"); };
	virtual bool dialup(const char *usrnam, const char *passwd);
	virtual bool hangup();
	virtual bool getstat();
	virtual std::string get_last_error();
};

__attribute__((constructor)) static void backend_rp_pppoe_main(void)
{
	SDBackendRpPppoe *backend;
	backend = new SDBackendRpPppoe(false);
	SDBackendManager::register_backend(backend);
	backend = new SDBackendRpPppoe(true);
	SDBackendManager::register_backend(backend);
}

extern "C" {
}

bool SDBackendRpPppoe::subprocess(const char **argv)
{
	GSubprocess *subp;	
	bool ret = false;
	char *buf = nullptr;

	if ((subp = g_subprocess_newv(argv, static_cast<GSubprocessFlags>(
						G_SUBPROCESS_FLAGS_STDOUT_PIPE |
						G_SUBPROCESS_FLAGS_STDERR_MERGE),
					nullptr)) == nullptr) {
		lasterror = "start sub-process failed.";
		goto err_subp_new;
	}

	g_subprocess_communicate_utf8(subp, nullptr, nullptr, &buf, nullptr, nullptr);

	if (!g_subprocess_get_if_exited(subp)) {
		lasterror = "sub-process exited abnormal. outputs: \n" + std::string(buf);
		goto err_subp_fail;
	}

	if (g_subprocess_get_exit_status(subp)) {
		lasterror = "sub-process exited failed. outputs: \n" + std::string(buf);
		goto err_subp_fail;
	}

	lasterror = "success, outputs: \n" + std::string(buf);
	ret = true;

err_subp_fail:
	g_object_unref(subp);
err_subp_new:
	return ret;
}

bool SDBackendRpPppoe::dialup(const char *usrnam, const char *passwd)
{
	const char *kernel = this->kernel ? "1" : "0";
	const char *ifname = Config::get("pppoe", "interface", "eth0");

	const char *argv[6];
	argv[0] = "seth-pppoe";
	argv[1] = ifname;
	argv[2] = usrnam;
	argv[3] = passwd;
	argv[4] = kernel;
	argv[5] = nullptr;

	return subprocess(argv);
}

bool SDBackendRpPppoe::hangup()
{
	const char *argv[3];
	argv[0] = "seth-pppoe";
	argv[1] = "--hangup";
	argv[2] = nullptr;

	return subprocess(argv);
}

bool SDBackendRpPppoe::getstat()
{
#ifndef _WIN32
	int fd;
	struct ifreq ifr;
	char iface[] = SETHD_PPPOE_IFNAME;

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name , iface , IFNAMSIZ - 1);

	return (ioctl(fd, SIOCGIFINDEX, &ifr) == 0);
#else
	return false;
#endif // _WIN32
}

std::string SDBackendRpPppoe::get_last_error()
{
	return this->lasterror;
}
