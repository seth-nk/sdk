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

LIBSETH_APPLICATION("genhdr")

static void usage(const char *program)
{
	printf("usage: %s [options]\n\n"
		"  -l life (def: 5)\n"
		"  -t timestamp\n"
		"  -L length (def: 8)\n"
		"  -i isp (def: 0)\n"
		"  -n nkversion (def: 3)\n"
		"  -r nkrevision (def: 0)\n"
		"  -p prefix0 (def: '\\r')\n"
		"  -P prefix1 (def: '1')\n"
		"  -u username (if not spec, PrivacyExtension will be enabled)\n",
		program);
}

int seth_application_main(int argc, char *argv[])
{
	int opt, privacy = 1;
	struct seth_v2_hdr *hdr = seth_hdr_new();
	seth_hdr_set_life(hdr, 5);
	seth_hdr_set_length(hdr, 8);
	seth_hdr_set_nkversion(hdr, 3);
	seth_hdr_set_prefix0(hdr, '\r');
	seth_hdr_set_prefix1(hdr, '1');

	while ((opt = getopt(argc, argv, "l:t:L:i:n:r:p:P:u:")) != -1) {
		switch (opt) {
		case 'l':
			seth_hdr_set_life(hdr, atol(optarg));
			break;
		case 't':
			seth_hdr_set_timestamp(hdr, atol(optarg));
			break;
		case 'L':
			seth_hdr_set_length(hdr, atol(optarg));
			break;
		case 'i':
			seth_hdr_set_isp(hdr, atol(optarg));
			break;
		case 'n':
			seth_hdr_set_nkversion(hdr, atol(optarg));
			break;
		case 'r':
			seth_hdr_set_nkrevision(hdr, atol(optarg));
			break;
		case 'p':
			seth_hdr_set_prefix0(hdr, optarg[0]);
			break;
		case 'P':
			seth_hdr_set_prefix1(hdr, optarg[0]);
			break;
		case 'u':
			snprintf(hdr->papname, sizeof(hdr->papname), "%s", optarg);
			privacy = 0;
			break;
		case '?':
			usage(argv[0]);
			exit(0);
			break;
		}
	}

	if (privacy) {
		seth_hdr_add_flag(hdr, SETH_NK_FLAG_PrivacyExtensionEnabled);
	}

	seth_hdr_set_offset(hdr, sizeof(struct seth_v2_hdr));

	fwrite(hdr, sizeof(struct seth_v2_hdr), 1, stdout);

	exit(0);
}
