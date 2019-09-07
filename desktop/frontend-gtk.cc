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
#include <gtk/gtk.h>

#define LIBSETH_APPLICATION_PORTABLE_FOR_POSIX
#define LIBSETH_APPLICATION_PORTABLE_FOR_WINDOWS
#include "seth.h"
#include "nkget.h"
}

#include "config.h"
#include "backend.h"
#include "version.h"

#include "sethcli_conf.h"

LIBSETH_APPLICATION("crop.seth.dkt.gtk")

int seth_application_main(int argc, char *argv[])
{
	Config::open_mem(default_sethcli_conf, sizeof(default_sethcli_conf) - 1);

#ifdef _WIN32
	int pause = 0;

	SetConsoleOutputCP(CP_UTF8);
	HMODULE hModule = GetModuleHandleW(NULL);
	WCHAR path[MAX_PATH];
	GetModuleFileNameW(hModule, path, MAX_PATH);
	PathRemoveFileSpecW(path);
	if (SetCurrentDirectoryW(path) == 0) {
		fprintf(stderr, "SetCurrentDirectoryW failed.\n");
		exit(1);
	}

	// fixup MSYS2 and Cygwin
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
#else
	std::string confighome = g_get_user_config_dir();
	confighome += "/sethdkt/";
	mkdir(confighome.data(), 0775);
	if (chdir(confighome.data()) < 0) {
		perror("chdir");
		exit(1);
	}
#endif // _WIN32
#ifdef _DEBUG
	printf("current dir: %s\n", getcwd(NULL, 0));
#endif // _DEBUG

	gtk_init(&argc, &argv);

	gtk_main();
	exit(0);
}
