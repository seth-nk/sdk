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
#define RASNP_Ipv6 0x00000008

#include <windows.h>
#include <ras.h>
#include <raserror.h>
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

#define LIBSETH_APPLICATION_PORTABLE_FOR_POSIX
#define LIBSETH_APPLICATION_PORTABLE_FOR_WINDOWS
#include "seth.h"
#include "nkget.h"
}
#include "config.h"
#include "backend.h"
#include "version.h"

LIBSETH_APPLICATION("comp.seth.dkt.cli")

static void check_null(const void *s, const char *name)
{
	if (s == NULL) {
		fprintf(stderr, "'%s' not found, please specify it in config file\n", name);
		exit(1);
	}
}

int seth_application_main(int argc, char *argv[])
{
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
/*
	const char *cdir = dirname(argv[0]);
	std::string path_env = std::string(cdir) + ":" + getenv("PATH");
	setenv("PATH", path_env.data(), 1);
*/
#endif // _WIN32
#ifdef _DEBUG
	printf("current dir: %s\n", getcwd(NULL, 0));
#endif // _DEBUG

	int shift = 0;
	char cmd_platformdef = 0,
		cmd_nofilecreation = 0;
	
	while (argc > shift + 1) {
		shift++;

		if (strcmp(argv[shift], "-LoadPlatformDefaultConfig") == 0) {
			cmd_platformdef = 1;
		} else if (strcmp(argv[shift], "-NoConfigFileCreation") == 0) {
			cmd_nofilecreation = 1;
		} else if (strcmp(argv[shift], "-OverrideConfig") == 0) {
			shift += 2;
			if (argc > shift)
				Config::add_override(argv[shift - 1], argv[shift]);
		} else if (strcmp(argv[shift], "-") == 0) {
			break;
		}
	}

	printf("%s\n", "Seth Desktop-CLI demo for " OSNAME "\n" \
	               "Version: " SD_VERSION "\n" \
	               "build with Seth SDK which licensed by MIT\n\n" \
	               " * copyright 2019 NSWA Maintainers\n" \
	               " * copyright 2019 The-Seth-Project\n" \
		       "\n" \
		       " SDK repo: https://github.com/seth-project/sdk\n" \
		       " Website: https://tienetech.tk/seth/\n" \
	               "===============================================");

	if (!cmd_nofilecreation && access("sethcli.conf", F_OK) == -1) {
		fprintf(stderr, "generating configuration file 'sethcli.conf'...\n");
		FILE *f = fopen("sethcli.conf", "w");
		if (f == NULL) {
			fprintf(stderr, "create configuration file failed.\n");
			exit(1);
		}
		if (fwrite(default_sethcli_conf, 1, default_sethcli_conf_size, f) != default_sethcli_conf_size) {
			fprintf(stderr, "init configuration file failed.\n");
			unlink("sethcli.conf");
			exit(1);
		}
		fclose(f);
		fprintf(stderr, "Configuration file 'sethcli.conf' in directory '%s' generated. Edit it and re-run program.\n", g_get_current_dir());
#ifdef _WIN32
		pause = 1;
#endif // _WIN32
		goto end;
	}

	if (cmd_platformdef) {
		Config::open_mem(default_sethcli_conf, default_sethcli_conf_size);
	} else if (!Config::open("sethcli.conf")) {
		fprintf(stderr, "configuration file open failed.\n");
		exit(1);
	}

	const char *papuser; papuser = Config::get("pap", "username");
	const char *pappass; pappass = Config::get("pap", "password");
	const char *filename; filename = Config::get("seth", "filename");
	const char *backend; backend = Config::get("pppoe", "backend");
#ifdef _WIN32
	pause = Config::get_int("seth4win", "pause");
#endif // _WIN32
	check_null(pappass, "pap password");
	check_null(filename, "seth filename");
	check_null(backend, "pppoe backend");

	if (papuser == nullptr || strcmp(papuser, "") == 0) {
		printf("pap username in seth data file will be used\n");
		papuser = NULL;
	}

	char *papreal;
	int err;
	if ((err = seth_nkpin_get_from_file(filename, papuser, time(NULL), &papreal)) < 0) {
		fprintf(stderr, "seth_nkpin_get_from_file error, errcode: %d\n", err);
		goto end;
	}

	printf("override PAP username: ^M%s\n", papreal + 1);

	printf("dial-up started, backend '%s'...\n", backend);

#ifdef _DEBUG
	for (auto i : *SDBackendManager::backends) {
		printf("detected backend: %s, %s\n", i.first.data(), i.second->getname().data());
	}
#endif // _DEBUG

	SDBackend *sdbackend;
	if ((sdbackend = SDBackendManager::get_backend(backend)) == nullptr) {
		fprintf(stderr, "FATAL: backend '%s' not found\n", backend);
		goto end;
	}

	if (sdbackend->getstat()) {
		fprintf(stderr, "reject to dial-up on a connected session. stopping...\n");
		goto end;
	}

	if (!sdbackend->dialup(papreal, pappass)) {
		fprintf(stderr, "dial-up failed, errmsg reported by backend: %s\n", sdbackend->get_last_error().data());
	}

end:
#ifdef _WIN32
	if (pause == 1) {
		printf("Press any key to exit\n");
		getchar();
	}
#endif // _WIN32
	exit(0);
}
