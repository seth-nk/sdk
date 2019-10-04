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

LIBSETH_APPLICATION("strip")

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

static ssize_t writen(int fd, const void *ptr, size_t n)
{
	size_t nleft;
	ssize_t nwritten;

	nleft = n;
	while (nleft > 0) {
		if ((nwritten = write(fd, ptr, nleft)) < 0) {
			if (nleft == n)
				return (-1);	/* error, return -1 */
			else
				break;	/* error, return amount written so far */
		} else if (nwritten == 0) {
			break;
		}
		nleft -= nwritten;
		ptr += nwritten;
	}
	return (n - nleft);	/* return >= 0 */
}

static void usage(const char *program)
{
	printf("usage: %s [options]\n\n"
	       "    -i <input_file>\n"
	       "    -o <output_file> (stdout while not specify)\n"
	       "    -s <scale> (drop scale-1 groups which scale groups)\n\n"
	       "  which group will be kept, use only one option (default is -b 0)\n"
	       "    -b <begin_after_offset>\n"
	       "    -m <middle_after_offset>\n"
	       "    -M <middle_before_offset> (same as -m -{num})\n"
	       "    -e <end_before_offset> (same as -b -{num})\n\n", program);
}

int seth_application_main(int argc, char *argv[])
{
	int exitval = 0, opt;

	size_t scale = 0;
	ssize_t keep_offset = 0;
	int middle = 0;

	const char *ifile = NULL, *ofile = NULL;
	int ifd, ofd;

	struct seth_v2_hdr hdr;

	while ((opt = getopt(argc, argv, "i:o:s:S:e:E:l:")) != -1) {
		switch (opt) {
		case 'i':
			ifile = optarg;
			break;
		case 'o':
			ofile = optarg;
			break;
		case 's':
			scale = atol(optarg);
			break;
		case 'b':
			keep_offset = atol(optarg);
			break;
		case 'm':
			middle = 1;
			keep_offset = atol(optarg);
			break;
		case 'M':
			middle = 1;
			keep_offset = -atol(optarg);
			break;
		case 'e':
			keep_offset = -atol(optarg);
			break;
		case '?':
			usage(argv[0]);
			exit(0);
			break;
		}
	}

	if (!ifile) {
		fprintf(stderr, "stdin is not allowed\n");
		goto err_open_ifd;
	} else if ((ifd = open(ifile, O_RDONLY | O_NOATIME)) < 0) {
		exitval = 1;
		perror("input file open error");
		goto err_open_ifd;
	}

	if (!ofile) {
		ofd = STDOUT_FILENO;
	} else if ((ofd = open(ofile, O_WRONLY | O_CREAT | O_TRUNC,
		      S_IRUSR | S_IWUSR)) < 0) {
		exitval = 2;
		perror("output file open error");
		goto err_open_ofd;
	}

	if (readn(ifd, &hdr, sizeof(struct seth_v2_hdr)) !=
	    sizeof(struct seth_v2_hdr)) {
		exitval = 3;
		perror("read error");
		goto err_read_hdr;
	}

	if (seth_hdr_valid(&hdr) < 0) {
		exitval = 4;
		fprintf(stderr, "Seth data invalid\n");
		goto err_hdr_invalid;
	}

	size_t offset = seth_hdr_get_offset(&hdr);
	//time_t timestamp = seth_hdr_get_timestamp(&hdr);
	size_t life = seth_hdr_get_life(&hdr);
	size_t length = seth_hdr_get_length(&hdr);

	size_t life_new = life * scale;

	ssize_t pos = (middle ? life_new / 2 + keep_offset : keep_offset);
	if (pos < 0)
		pos = life_new + pos;

	if (pos < 0 || pos > life_new) {
		exitval = 5;
		fprintf(stderr, "pos '%zd' overflow new life '%zd'\n", pos, life_new);
		goto err_pos_overflow;
	}

	off_t fsize;
	if ((fsize = lseek(ifd, 0, SEEK_END)) == -1) {
		exitval = 6;
		perror("lseek error");
		goto err_lseek;
	}

	if (lseek(ifd, offset, SEEK_SET) == -1) {
		exitval = 6;
		perror("lseek error");
		goto err_lseek;
	}

	fsize -= offset;
	size_t count = fsize / length / scale;

	if (count == 0) {
		exitval = 7;
		fprintf(stderr, "scale too big\n");
		goto err_scale_toobig;
	}
	
	seth_hdr_set_life(&hdr, life_new);
	seth_hdr_set_offset(&hdr, sizeof(struct seth_v2_hdr));

	if (writen(ofd, &hdr, sizeof(struct seth_v2_hdr)) !=
	    sizeof(struct seth_v2_hdr)) {
		exitval = 8;
		perror("write\n");
		goto err_writemeta;
	}

	for (size_t i = 0; i < count; i++) {
		char buffer[length];
		if (lseek(ifd, length * pos, SEEK_CUR) < 0) {
			exitval = 9;
			perror("lseek error");
			goto err_sendfile;
		}

		if (readn(ifd, buffer, length) != length) {
			exitval = 9;
			perror("read error");
			goto err_sendfile;
		}

		if (writen(ofd, buffer, length) < 0) {
			exitval = 9;
			perror("write error");
			goto err_sendfile;
		}

		if (lseek(ifd, length * (scale - pos - 1), SEEK_CUR) < 0) {
			exitval = 9;
			perror("lseek error");
			goto err_sendfile;
		}
	}

	/* when origin file end with some data but less than a scaled
	 * group needed, dirty hack it. Use pos zero. To avoid some
	 * programs which called seth-strip to meet timestamp problems.
	 */
	if (fsize / length % scale) {
		char buffer[length];
		if (readn(ifd, buffer, length) != length) {
			exitval = 9;
			perror("read error");
			goto err_sendfile;
		}

		if (writen(ofd, buffer, length) < 0) {
			exitval = 9;
			perror("write error");
			goto err_sendfile;
		}
	}

err_sendfile:
err_writemeta:
err_scale_toobig:
err_lseek:
err_pos_overflow:
err_hdr_invalid:
err_read_hdr:
	close(ofd);
err_open_ofd:
	close(ifd);
err_open_ifd:
	exit(exitval);
}
