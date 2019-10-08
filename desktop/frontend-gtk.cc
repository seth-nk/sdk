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

#include <string>
#include <sstream>
#include <iostream>

#ifdef _WIN32
#define WINVER 0x0601
#define _WIN32_WINNT 0x0601

#include <windows.h>
#include <shlwapi.h>
#endif /* _WIN32 */


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>

#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>
#include <libgen.h>
#include <dirent.h>

extern "C" {
#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gprintf.h>

#define LIBSETH_APPLICATION_PORTABLE_FOR_POSIX
#define LIBSETH_APPLICATION_PORTABLE_FOR_WINDOWS
#include "seth.h"
#include "nkget.h"
}

#include "config.h"
#include "backend.h"
#include "version.h"

#include "frontend-gtk-glade.h"
#include "frontend-gtk-menu.h"

LIBSETH_APPLICATION("comp.seth.dkt.gtk")

static GtkWidget *window = NULL, *button, *label, *entry_password, *tv_adv;
static SDBackend *sdbackend;
static std::string filename("");
static bool textbuffer_adv_changed = false;

static void msgbox(const void *window, GtkMessageType type, const char* msg)
{
	GtkWidget *msgdialog = gtk_message_dialog_new(GTK_WINDOW(window),
	                                   GTK_DIALOG_DESTROY_WITH_PARENT,
	                                   type,
	                                   GTK_BUTTONS_CLOSE,
	                                   "%s", msg);

	gtk_window_set_title(GTK_WINDOW(msgdialog), libseth_application_name);

	gtk_dialog_run(GTK_DIALOG(msgdialog));
	gtk_widget_destroy(msgdialog);
}

static void errbox(const void *window, const char* msg)
{
	msgbox(window, GTK_MESSAGE_ERROR, msg);
}

static void errquit(const void *window, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	gchar *msg;
	g_vasprintf(&msg, fmt, args);
	errbox(window, msg);
	g_free(msg);
	va_end(args);
	exit(1);
}

static void gtk_utils_tv_set_text(GtkTextView *widget, const gchar *text)
{
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(widget);
	gtk_text_buffer_set_text(buffer, text, -1);
}

static gchar *gtk_utils_tv_get_text(GtkTextView *widget)
{
	GtkTextIter start, end;
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(widget);
	gtk_text_buffer_get_bounds(buffer, &start, &end);
	return gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
}

//====================================================================

static void addconfig(const char *text)
{
	std::string _adv = std::string(text);
	std::istringstream adv(_adv);
	std::string line;
	while (std::getline(adv, line)) {
		Config::add_override(line);
	}
}

extern "C" {
static void tv_adv_text_changed(GtkTextBuffer *textbuffer, gpointer user_data)
{
	textbuffer_adv_changed = true;
}
}

static void load_saved_config()
{
	gchar *data;

	if (g_file_get_contents((std::string(".") + libseth_application_name + ".password").data(), &data, nullptr, nullptr)) {
		gtk_entry_set_text(GTK_ENTRY(entry_password), data);
		g_free(data);
	}

	if (g_file_get_contents((std::string(".") + libseth_application_name + ".configs").data(), &data, nullptr, nullptr)) {
		gtk_utils_tv_set_text(GTK_TEXT_VIEW(tv_adv), data);
		addconfig(data);
		g_free(data);
	}

	GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv_adv));
	g_signal_connect(buffer, "changed", G_CALLBACK(tv_adv_text_changed), nullptr);
}

static void save_config()
{
	const gchar *password = gtk_entry_get_text(GTK_ENTRY(entry_password));
	g_file_set_contents((std::string(".") + libseth_application_name + ".password").data(), password, -1, nullptr);

	gchar *data = gtk_utils_tv_get_text(GTK_TEXT_VIEW(tv_adv));
	g_file_set_contents((std::string(".") + libseth_application_name + ".configs").data(), data, -1, nullptr);
	g_free(data);
}

static gboolean reload_stat(gpointer user_data)
{
	gtk_widget_set_sensitive(button, TRUE);
	gtk_button_set_label(GTK_BUTTON(button), sdbackend->getstat() ? "断开" : "连接");
	return G_SOURCE_REMOVE;
}

static gboolean dialup_failed_callback(gpointer user_data)
{
	errbox(window, ("后端返回如下错误：" + sdbackend->get_last_error()).data());
	return G_SOURCE_REMOVE;
}

struct dialup_auth {
	std::string username;
	std::string password;
};

static gpointer dialup(gpointer user_data)
{
	dialup_auth *auth = (dialup_auth *) user_data;
#ifdef _DEBUG
	printf("auth: %s %s\n", auth->username.data(), auth->password.data());
#endif // _DEBUG

	if (!sdbackend->dialup(auth->username.data(), auth->password.data())) {
		gdk_threads_add_idle(dialup_failed_callback, nullptr);
	}

	delete auth;
	gdk_threads_add_idle(reload_stat, nullptr);
	return nullptr;
}

//====================================================================

extern "C"
void on_button_clicked(GtkButton *_button, gpointer user_data)
{
	gtk_widget_set_sensitive(button, FALSE);
	gtk_button_set_label(GTK_BUTTON(button), "请稍候");

	if (textbuffer_adv_changed) {
#ifdef _DEBUG
		printf("advconfig has been changed, re-overrided configs\n");
#endif // _DEBUG
		gchar *text = gtk_utils_tv_get_text(GTK_TEXT_VIEW(tv_adv));
		addconfig(text);
		g_free(text);
		textbuffer_adv_changed = false;
	}

	if (sdbackend->getstat()) {
		sdbackend->hangup();
		gdk_threads_add_timeout(200, reload_stat, nullptr);
	} else {
		char *papreal;
		const char *password;
		int err;
		if ((err = seth_nkpin_get_from_file(filename.data(), nullptr, 0, &papreal)) < 0) {
			errquit(window, "seth_nkpin_get_from_file() 错误, 错误代码: %d", err);
		}

		password = gtk_entry_get_text(GTK_ENTRY(entry_password));

		auto auth = new dialup_auth();
		auth->username = std::string(papreal);
		auth->password = std::string(password);

#ifdef _WIN32
		g_thread_new(nullptr, dialup, auth);
#else
		// FIXME: some backends need call GDK/GLib API. But these can only be called on main thread.
		//g_thread_new(nullptr, dialup, auth);
		dialup(auth);
#endif // _WIN32
	}
}

extern "C"
void on_window_destroy(GtkWidget *object, gpointer user_data)
{
	save_config();
	gtk_main_quit();
}

//====================================================================

static void find_first_sth_file()
{
	DIR* dir;
	struct dirent* dent;

	if ((dir = opendir(".")) == NULL)
		errquit(window, "打开当前目录失败");

	while ((dent = readdir(dir)) != NULL) {
		if ((strcmp(dent->d_name, ".") == 0) || (strcmp(dent->d_name, "..") == 0))
			continue;

		size_t length = strlen(dent->d_name);
		if (length > 5 && strcmp(dent->d_name + length - 5, ".sth3") == 0) {
			filename = std::string(dent->d_name);
			break;
		}
	}

	closedir(dir);
}

static void load_backend()
{
	const char *backend; backend = Config::get("pppoe", "backend");

	if ((sdbackend = SDBackendManager::get_backend(backend)) == nullptr) {
		errquit(nullptr, "FATAL: backend '%s' not found\n", backend);
	}
}

static void action_app_cb_open_dir(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	std::string uri = std::string("file:///") + g_get_current_dir();
	g_app_info_launch_default_for_uri(uri.data(), nullptr, nullptr);
}

static void action_app_cb_reset_config(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	GtkBuilder *builder = gtk_builder_new();
	gtk_builder_add_from_string(builder, glade, sizeof(glade) - 1, NULL);
	GtkWidget *adv = GTK_WIDGET(gtk_builder_get_object(builder, "tv_adv"));
	errquit(window, "%s\n%s", g_file_set_contents((std::string(".") + libseth_application_name + ".configs").data(),
		gtk_utils_tv_get_text(GTK_TEXT_VIEW(adv)), -1, nullptr) ? "恢复高级设置为默认成功" : "恢复高级设置为默认失败",
		"修改将在重新启动 Seth Desktop GTK 后生效");
}

#ifndef _WIN32
static bool sethdkt_gtk_set_autostart()
{
	gchar *data;
	std::string dest = std::string(g_get_user_config_dir()) + "/autostart/comp.seth.dkt.gtk.desktop";
	if (g_file_get_contents(_PREFIX "/share/applications/comp.seth.dkt.gtk.desktop", &data, nullptr, nullptr)) {
		if (!g_file_set_contents(dest.data(), data, -1, nullptr))
			goto err_get_contens;
	} else {
		goto err_get_contens;
	}

	return true;

err_set_contens:
	g_free(data);
err_get_contens:
	return false;
}
#else
static bool sethdkt_gtk_set_autostart()
{
	HKEY hKey;
	BOOL isWOW64;
	REGSAM p = KEY_WRITE;

	WCHAR path[MAX_PATH + 1];
	HMODULE hModule = GetModuleHandleW(NULL);
	GetModuleFileNameW(hModule, path, MAX_PATH);

	IsWow64Process(GetCurrentProcess(), &isWOW64);
	if (isWOW64)
		p |= KEY_WOW64_64KEY;

	if (RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, NULL, 0, p, NULL, &hKey, NULL) != ERROR_SUCCESS)
		return false;

	if (RegSetValueExW(hKey, L"seth4win", 0, REG_SZ, (BYTE*) path, sizeof(path) * sizeof(wchar_t))!= ERROR_SUCCESS){
		RegCloseKey(hKey);
		return false;
	}
	RegCloseKey(hKey);
	return true;
}
#endif // _WIN32

static void action_app_cb_autostart_config(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	msgbox(window, GTK_MESSAGE_INFO, sethdkt_gtk_set_autostart() ? "设置为自动启动成功。\n\n若你使用第三方工具禁用 Seth Desktop 的自启动后想要恢复。重复运行本工具可能无法恢复，请在第三方工具中解除禁用。" : "设置为自动启动失败。");
}

static void action_app_cb_check_updata(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	g_app_info_launch_default_for_uri("https://github.com/seth-project/sdk/releases", nullptr, nullptr);
}

static const GActionEntry app_actions[] = {
	{ "open_dir", action_app_cb_open_dir },
	{ "reset_config", action_app_cb_reset_config },
	{ "autostart_config", action_app_cb_autostart_config },
	{ "check_updata", action_app_cb_check_updata }
};


int seth_application_main(int argc, char *argv[])
{
	Config::open_mem(default_sethcli_conf, default_sethcli_conf_size);

#ifdef _WIN32
	SetConsoleOutputCP(CP_UTF8);
	HMODULE hModule = GetModuleHandleW(NULL);
	WCHAR path[MAX_PATH];
	GetModuleFileNameW(hModule, path, MAX_PATH);
	PathRemoveFileSpecW(path);
	if (SetCurrentDirectoryW(path) == 0) {
		MessageBoxA(NULL, "SetCurrentDirectoryW failed.\n", libseth_application_name, MB_OK|MB_ICONERROR|MB_SETFOREGROUND);
		exit(1);
	}
#else
	std::string confighome = g_get_user_config_dir();
	confighome += "/sethdkt/";
	mkdir(confighome.data(), 0775);
	if (chdir(confighome.data()) < 0) {
		perror("chdir");
		exit(1);
	}

	Config::add_override("pppoe.backend", "nmcli");
#endif // _WIN32
#ifdef _DEBUG
	printf("current dir: %s\n", getcwd(NULL, 0));
#endif // _DEBUG

	gtk_init(&argc, &argv);

/*
	int shift = 0;
	while (argc > shift + 1) {
		shift++;

		if (strcmp(argv[shift], "-CallBackendFunctionDialup") == 0) {
			shift += 2;
			if (argc > shift) {
				load_backend();
				if (sdbackend->dialup(argv[shift - 1], argv[shift]) == false) {
					printf("后端返回如下错误：%s\n", sdbackend->get_last_error().data());
					exit(1);
				}
				exit(0);
			}
		} else if (strcmp(argv[shift], "-") == 0) {
			break;
		}
	}
*/
	GtkBuilder *builder = gtk_builder_new();
	gtk_builder_add_from_string(builder, glade, sizeof(glade) - 1, NULL);
	gtk_builder_add_from_string(builder, menu, sizeof(menu) - 1, NULL);

	window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
	GMenuModel *menu = G_MENU_MODEL(gtk_builder_get_object(builder, "menu_main"));

	GActionGroup *action_group = (GActionGroup *) g_simple_action_group_new();
	g_action_map_add_action_entries(G_ACTION_MAP(action_group), app_actions, G_N_ELEMENTS(app_actions), window);
	gtk_widget_insert_action_group(window, "app", action_group);
	g_object_unref(action_group);

	GtkWidget *menu_button = GTK_WIDGET(gtk_builder_get_object(builder, "menu_button_main"));
	gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(menu_button), menu);

	button = GTK_WIDGET(gtk_builder_get_object(builder, "button"));
	label = GTK_WIDGET(gtk_builder_get_object(builder, "label"));
	entry_password = GTK_WIDGET(gtk_builder_get_object(builder, "entry_password"));
	tv_adv = GTK_WIDGET(gtk_builder_get_object(builder, "tv_adv"));

	gtk_builder_connect_signals(builder, NULL);

	g_object_unref(builder);

	find_first_sth_file();
	load_saved_config();
	load_backend();
	reload_stat(nullptr);

	if (filename.empty())
		errquit(window, "未在目录 '%s' 中找到 Seth 数据文件", g_get_current_dir());

	Config::add_override("seth.filename", filename.data());

	gchar *info = g_strconcat("GTK+ version seth-desktop frontend.\nVersion: ", SD_VERSION,
		" (", OSNAME, ") Locale: 中文（简体）\n",
		"build with Seth SDK (Version: ", LIBSETH_SDK_VERSION, ") which licensed by MIT\n" \
		"copyright 2019 <a href=\"https://github.com/seth-project/sdk/blob/master/CREDITS\">The-Seth-Project and contributors</a>\n\n" \
		"SDK repo: <a href=\"https://github.com/seth-project/sdk\">https://github.com/seth-project/sdk</a>\n" \
		"Website: <a href=\"https://seth-project.tk\">https://seth-project.tk</a>\n\n找到并使用数据文件：<b>",
		filename.data(), "</b>", nullptr);
	gtk_label_set_markup(GTK_LABEL(label), info);
	g_free(info);

	gtk_widget_show(window);

	if (!sdbackend->getstat() && Config::get_int("frontends.gtk", "auto.dialup") == 1) {
		g_signal_emit_by_name(button, "clicked");
	}

	gtk_main();
	exit(0);
}
