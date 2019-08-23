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
#define _GNU_SOURCE

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

#define LIBSETH_APPLICATION_PORTABLE_FOR_POSIX
#define LIBSETH_APPLICATION_PORTABLE_FOR_WINDOWS
#include "seth.h"
#include "nkget.h"

LIBSETH_APPLICATION("nkgetpin")

static void usage(const char *program)
{
	printf("usage: %s -f <filename> [-u username] [-t timestamp]\n\n"
		" *    If username not specified, the username in seth data file will be used.\n"
		"      When username not specified and PrivacyExtension is alse enabled\n"
		"      only nkpin will be display.\n\n"
		" *    If timestamp not specified, the current time will be used.\n\n",
		program);
}

int seth_application_main(int argc, char *argv[])
{
	int opt;
	const char *file = NULL, *username = NULL;
	time_t timestamp = 0;

	while ((opt = getopt(argc, argv, "f:u:t:")) != -1) {
		switch (opt) {
		case 'f':
			file = optarg;
			break;
		case 'u':
			username = optarg;
			break;
		case 't':
			timestamp = atol(optarg);
			break;
		case '?':
			usage(argv[0]);
			exit(0);
			break;
		}
	}

	if (file == NULL) {
		fprintf(stderr, "filename cannot be empty\n");
		exit(1);
	}

	char *user;
	int err;
	if ((err = seth_nkpin_get_from_file(file, username, timestamp, &user)) < 0) {
		fprintf(stderr, "seth_nkpin_get_from_file error, errcode: %d\n", err);
		exit(1);
	}

	printf("%s\n", user);
	free(user);
	exit(0);
}
