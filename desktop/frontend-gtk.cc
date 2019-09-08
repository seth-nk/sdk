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

LIBSETH_APPLICATION("comp.seth.dkt.gtk")

GtkWidget *window = NULL, *button, *label, *entry_password, *tv_adv;
SDBackend *sdbackend;
std::string filename("");

static void errbox(const void *window, const char* msg)
{
	GtkWidget *msgdialog = gtk_message_dialog_new(GTK_WINDOW(window),
	                                   GTK_DIALOG_DESTROY_WITH_PARENT,
	                                   GTK_MESSAGE_ERROR,
	                                   GTK_BUTTONS_CLOSE,
	                                   "%s", msg);

	gtk_window_set_title(GTK_WINDOW(msgdialog), libseth_application_name);

	gtk_dialog_run(GTK_DIALOG(msgdialog));
	gtk_widget_destroy(msgdialog);
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

static void load_saved_config()
{
	gchar *data;
	if (g_file_get_contents((std::string(".") + libseth_application_name + ".password").data(), &data, nullptr, nullptr)) {
		gtk_entry_set_text(GTK_ENTRY(entry_password), data);
		g_free(data);
	}

	if (g_file_get_contents((std::string(".") + libseth_application_name + ".configs").data(), &data, nullptr, nullptr)) {
		gtk_utils_tv_set_text(GTK_TEXT_VIEW(tv_adv), data);
		g_free(data);
	}
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

	gchar *text = gtk_utils_tv_get_text(GTK_TEXT_VIEW(tv_adv));
	std::string _adv = std::string(text);
	std::istringstream adv(_adv);
	std::string line;
	while (std::getline(adv, line)) {
		Config::add_override(line);
	}
	g_free(text);

	if (sdbackend->getstat()) {
		sdbackend->hangup();
		gdk_threads_add_timeout(200, reload_stat, nullptr);
	} else {
		char *papreal;
		const char *password;
		int err;
		if ((err = seth_nkpin_get_from_file(filename.data(), nullptr, time(NULL), &papreal)) < 0) {
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
	load_backend();

	GtkBuilder *builder = gtk_builder_new();
	gtk_builder_add_from_string(builder, glade, sizeof(glade) - 1, NULL);

	window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));

	button = GTK_WIDGET(gtk_builder_get_object(builder, "button"));
	label = GTK_WIDGET(gtk_builder_get_object(builder, "label"));
	entry_password = GTK_WIDGET(gtk_builder_get_object(builder, "entry_password"));
	tv_adv = GTK_WIDGET(gtk_builder_get_object(builder, "tv_adv"));

	gtk_builder_connect_signals(builder, NULL);

	g_object_unref(builder);

	reload_stat(nullptr);
	find_first_sth_file();
	load_saved_config();

	if (filename.empty())
		errquit(window, "未在目录 '%s' 中找到 Seth 数据文件", g_get_current_dir());

	Config::add_override("seth.filename", filename.data());

	gchar *info = g_strconcat("GTK+ version seth-desktop frontend.\nVersion: ", SD_VERSION,
		" (", OSNAME, ") Locale: 中文（简体）\n",
		"build with Seth SDK which licensed by MIT\n\n" \
		" * copyright 2019 The-Seth-Project\n" \
		" * copyright 2019 NSWA Maintainers\n" \
		"\n" \
		"SDK repo: <a href=\"https://github.com/seth-project/sdk\">https://github.com/seth-project/sdk</a>\n" \
		"Website: <a href=\"https://tienetech.tk/seth/\">https://tienetech.tk/seth/</a>\n\n找到并使用数据文件：<b>",
		filename.data(), "</b>", nullptr);
	gtk_label_set_markup(GTK_LABEL(label), info);
	g_free(info);

	gtk_widget_show(window);
	gtk_main();
	exit(0);
}
