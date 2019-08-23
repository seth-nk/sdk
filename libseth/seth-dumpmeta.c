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
#include "portable.h"

LIBSETH_APPLICATION("dumpmeta")

static ssize_t readn(int fd, void *ptr, size_t n)
{
	size_t nleft;
	ssize_t nread;

	nleft = n;
	while (nleft > 0) {
		if ((nread = read(fd, ptr, nleft)) < 0) {
			if (nleft == n)
				return (-1);	/* error, return -1 */
			else
				break;	/* error, return amount read so far */
		} else if (nread == 0) {
			break;	/* EOF */
		}
		nleft -= nread;
		ptr += nread;
	}
	return (n - nleft);	/* return >= 0 */
}

static const char *get_isp_name(uint16_t ispcode)
{
	switch (ispcode) {
	case SETH_NK_ISP_UNSPEC:
		return "unspecified";
	case SETH_NK_ISP_CMCC_SD:
		return "CMCC Shandong";
	}
	return "Unknown";
}

static const char *get_netkeeper_version(uint16_t nkversion)
{
	switch (nkversion) {
	case SETH_NK_VERSION_INV:
		return "invalid";
	case SETH_NK_VERSION_2_5:
		return "2.5.0059";
	case SETH_NK_VERSION_4_9:
		return "4.9.19.699";
	case SETH_NK_VERSION_4_11:
		return "4.11.4.731";
	case SETH_NK_VERSION_5:
		return "5.x";
	}
	return "Unknown";
}

int seth_application_main(int argc, char *argv[])
{
	int exitval = 0;
	int fd, flags;
	struct seth_v2_hdr hdr;

	if (argc < 1) {
		printf("usage: %s <file>\n", argv[0]);
		exit(1);
	}

	if ((fd = open(argv[1], O_RDONLY | O_NOATIME)) < 0) {;
		exitval = 1;
		fprintf(stderr, "open error\n");
		goto err_open;
	}

	if (readn(fd, &hdr, sizeof(struct seth_v2_hdr)) !=
	    sizeof(struct seth_v2_hdr)) {
		exitval = 2;
		fprintf(stderr, "read error\n");
		goto err_read_hdr;
	}

	if (seth_hdr_valid(&hdr) < 0) {
		exitval = 3;
		fprintf(stderr, "Seth data invalid\n");
		goto err_hdr_invalid;
	}

	printf("Seth version: 0x%X\n"
		"life: %zd, length: %zd, timestamp: 0x%zX, offset: %p\n"
		"ISP: %s, nkversion: %s, nkrevision: %d\n"
		"Prefixes: 0x%02X 0x%02X\n",
		seth_hdr_get_version(&hdr),
		seth_hdr_get_life(&hdr),
		seth_hdr_get_length(&hdr),
		seth_hdr_get_timestamp(&hdr),
		(void *) seth_hdr_get_offset(&hdr),
		get_isp_name(seth_hdr_get_isp(&hdr)),
		get_netkeeper_version(seth_hdr_get_nkversion(&hdr)),
		seth_hdr_get_nkrevision(&hdr),
		seth_hdr_get_prefix0(&hdr),
		seth_hdr_get_prefix1(&hdr));

	flags = seth_hdr_get_flags(&hdr);

	printf("FLAG  .......%d  Privacy Extension Enabled\n"
		"FLAG  ......%d.  Option Fields Available\n",
		(flags & SETH_NK_FLAG_PrivacyExtensionEnabled) ? 1 : 0,
		(flags & SETH_NK_FLAG_OptionFieldsAvailable) ? 1 : 0);

	printf("PAP username: %s\n", (flags & SETH_NK_FLAG_PrivacyExtensionEnabled) ? "invalid" : hdr.papname);

err_hdr_invalid:
err_read_hdr:
	close(fd);
err_open:
	exit(exitval);
}
