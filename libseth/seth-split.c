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

LIBSETH_APPLICATION("split")

#define BUFF_SIZE 4194304

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
	       "    -i <input_file> (stdin while not specify)\n"
	       "    -o <output_file> (stdout while not specify)\n\n"
	       "  START (choose one and only one)\n"
	       "    -s <start_time> (unix timestamp)\n"
	       "    -S <start_number> (default 0)\n\n"
	       "  END (choose one and only one)\n"
	       "    -e <end_time> (unix timestamp)\n"
	       "    -E <end_number>\n"
	       "    -l <groups> (default 1)\n\n", program);
}

int seth_application_main(int argc, char *argv[])
{
	int exitval = 0, opt;
	const char *ifile = NULL, *ofile = NULL;
	int ifd, ofd;
	size_t start = 0, end = 0;
	size_t startn = 0, endn = 0, groups = 1;
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
			start = atol(optarg);
			break;
		case 'S':
			startn = atol(optarg);
			break;
		case 'e':
			end = atol(optarg);
			break;
		case 'E':
			endn = atol(optarg);
			break;
		case 'l':
			groups = atol(optarg);
			break;
		case '?':
			usage(argv[0]);
			exit(0);
			break;
		}
	}

	if (!ifile) {
		ifd = STDIN_FILENO;
	} else if ((ifd = open(ifile, O_RDONLY | O_NOATIME)) < 0) {
		exitval = 1;
		perror("input file open error");
		goto err_open_ifd;
	}

	if (!ofile) {
		ofd = STDOUT_FILENO;
	} else
	    if ((ofd =
		 open(ofile, O_WRONLY | O_CREAT | O_TRUNC,
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
	time_t timestamp = seth_hdr_get_timestamp(&hdr);
	size_t life = seth_hdr_get_life(&hdr);
	size_t length = seth_hdr_get_length(&hdr);

	if (start != 0) {
		startn = (start - timestamp) / life;
	}

	if (endn != 0) {
		groups = endn - startn + 1;
	} else if (end != 0) {
		endn = (end - timestamp) / life;
		groups = endn - startn + 1;
	}

	off_t readoffset = offset + startn * length;
	size_t readbytes = groups * length;

	if (lseek(ifd, readoffset, SEEK_SET) == -1) {
		exitval = 5;
		perror("lseek error");
		goto err_lseek;
	}

	seth_hdr_set_timestamp(&hdr, timestamp + startn * life);
	seth_hdr_set_offset(&hdr, sizeof(struct seth_v2_hdr));

	if (writen(ofd, &hdr, sizeof(struct seth_v2_hdr)) !=
	    sizeof(struct seth_v2_hdr)) {
		exitval = 6;
		perror("write\n");
		goto err_writemeta;
	}

	char buffer[BUFF_SIZE];
	size_t buffersize = BUFF_SIZE;
	while (readbytes != 0) {
		ssize_t n = 0;

		if (readbytes < buffersize)
			buffersize = readbytes;

		if ((n = readn(ifd, buffer, buffersize)) > 0) {
			readbytes -= n;
		} else if (n == 0) {
			fprintf(stderr, "WARNING: early EOF!\n");
			break;
		} else {
			exitval = 7;
			perror("read error");
			goto err_sendfile;
		}

		if (writen(ofd, buffer, n) < 0) {
			exitval = 7;
			perror("write error");
			goto err_sendfile;
		}
	}

err_sendfile:
err_writemeta:
err_lseek:
err_hdr_invalid:
err_read_hdr:
	close(ofd);
err_open_ofd:
	close(ifd);
err_open_ifd:
	exit(exitval);
}
