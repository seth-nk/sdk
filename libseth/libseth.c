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

#ifdef _WIN32
#include <libloaderapi.h>
#endif // _WIN32

#include "portable.h"
#include "seth.h"
#include "nkget.h"

#define be8toh(x)	(x)
#define htobe8(x)	(x)

#ifndef TARGET_WASM
struct seth_v2_hdr *seth_hdr_new()
{
	struct seth_v2_hdr *hdr = malloc(sizeof(struct seth_v2_hdr));
	memset(hdr, 0, sizeof(struct seth_v2_hdr));
	hdr->magic0 = SETH_HDR_MAGIC0;
	hdr->magic1 = SETH_HDR_MAGIC1;
	seth_hdr_set_version(hdr, SETH_HDR_VERSION);
	return hdr;
}
#endif // TARGET_WASM

static void libseth_ver_check(const char *version)
{
	if (strcmp(version, LIBSETH_SDK_VERSION) != 0) {
		fprintf(stderr, "libseth: application's sdk version '%s' mismatches libseth's '%s', aborting.\n", version, LIBSETH_SDK_VERSION);
		abort();
	}
}

#ifndef _WIN32
void __attribute__((constructor)) libseth_auto_init()
{
	libseth_ver_check(libseth_application_sdk_version);
}
#else
void libseth_winnt_startup(const char *version)
{
	libseth_ver_check(version);
}
#endif // _WIN32

int seth_hdr_valid(struct seth_v2_hdr *hdr)
{
	if (hdr->magic0 != SETH_HDR_MAGIC0 ||
		hdr->magic1 != SETH_HDR_MAGIC1)
		return -1;
	if (seth_hdr_get_version(hdr) != SETH_HDR_VERSION)
		return -2;
	return 0;
}

#define SETH_HDR_GET_SET_UINT(field, type, size) \
type seth_hdr_get_##field(struct seth_v2_hdr *hdr) \
{ \
	return ((type) be##size##toh(hdr->field)); \
} \
void seth_hdr_set_##field(struct seth_v2_hdr *hdr, type field) \
{ \
	hdr->field = htobe##size((int##size##_t) field); \
}

SETH_HDR_GET_SET_UINT(version, uint16_t, 16)
SETH_HDR_GET_SET_UINT(life, size_t, 32)
SETH_HDR_GET_SET_UINT(timestamp, time_t, 64)
SETH_HDR_GET_SET_UINT(offset, size_t, 32)
SETH_HDR_GET_SET_UINT(length, size_t, 32)
SETH_HDR_GET_SET_UINT(isp, uint16_t, 16)
SETH_HDR_GET_SET_UINT(nkversion, uint16_t, 16)
SETH_HDR_GET_SET_UINT(nkrevision, uint16_t, 16)
SETH_HDR_GET_SET_UINT(prefix0, uint8_t, 8)
SETH_HDR_GET_SET_UINT(prefix1, uint8_t, 8)
SETH_HDR_GET_SET_UINT(flags, uint8_t, 8)

void seth_hdr_add_flag(struct seth_v2_hdr *hdr, uint8_t flag)
{
	hdr->flags |= flag;
}

off_t seth_nkpin_offset(struct seth_v2_hdr *hdr, const time_t timestamp)
{
	time_t start = seth_hdr_get_timestamp(hdr);
	size_t length = seth_hdr_get_length(hdr);
	size_t life = seth_hdr_get_life(hdr);
	off_t base_offset = seth_hdr_get_offset(hdr);

	return (base_offset +
		((off_t) timestamp - (off_t) start) / life * length);
}

#ifndef TARGET_WASM
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

int seth_nkpin_get_from_file(const char *file, const char *username,
			 time_t timestamp, char **pap_username)
{
	struct seth_v2_hdr hdr;
	int ret = NKCAT_ERROR_SUCCESS;
	int fd;
	off_t offset;
	size_t length;
	char *data;

	if ((fd = open(file, O_RDONLY | O_NOATIME)) < 0) {;
		ret = NKCAT_ERROR_OPEN_FAILD;
		goto err_open;
	}

	if (readn(fd, &hdr, sizeof(struct seth_v2_hdr)) !=
	    sizeof(struct seth_v2_hdr)) {
		ret = NKCAT_ERROR_METADATA_READ;
		goto err_read_hdr;
	}

	if (seth_hdr_valid(&hdr) < 0) {
		ret = NKCAT_ERROR_DATA_INVALID;
		goto err_hdr_invalid;
	}

	if (username == NULL) {
		if (hdr.flags & SETH_NK_FLAG_PrivacyExtensionEnabled) {
			ret = NKCAT_ERROR_NOPAPNAME;
			goto err_nopapname;
		}

		hdr.papname[sizeof(hdr.papname) - 1] = 0;
		username = hdr.papname;
	}

	length = seth_hdr_get_length(&hdr);

	if ((data = malloc(length + strlen(username) + 3)) == NULL) {
		ret = NKCAT_ERROR_NOMEM;
		goto err_malloc;
	}

	if (timestamp == 0) {
		timestamp = time(NULL);
	}
	offset = seth_nkpin_offset(&hdr, timestamp);

	if (lseek(fd, offset, SEEK_SET) < 0) {
		ret = NKCAT_ERROR_FILE_SEEK;
		goto err_lseek;
	}

	if (readn(fd, data + 2, length) != length) {
		ret = NKCAT_ERROR_DATE_READ;
		goto err_read_data;
	}

	data[0] = seth_hdr_get_prefix0(&hdr);
	data[1] = seth_hdr_get_prefix1(&hdr);
	strcpy(data + 2 + length, username);

	*pap_username = data;
	data = NULL;

err_read_data:
err_lseek:
	if (data != NULL)
		free(data);
err_malloc:
err_nopapname:
err_hdr_invalid:
err_read_hdr:
	close(fd);
err_open:
	return ret;
}
#endif // TARGET_WASM
