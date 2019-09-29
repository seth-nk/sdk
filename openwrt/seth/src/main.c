/* 
 * copyright 2019 glider0
 * License: MIT
 * This is free software with ABSOLUTELY NO WARRANTY.
 */

#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700
#define _BSD_SOURCE
#define _DEFAULT_SOURCE

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <limits.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define LIBSETH_APPLICATION_PORTABLE_FOR_POSIX
#include "seth.h"
#include "nkget.h"

LIBSETH_APPLICATION("seth.openwrt.pppdplugin")

/* import pppd vars */
#define MAXNAMELEN	256
extern char user[MAXNAMELEN];
typedef void (*notify_func) __P((void *, int));
typedef void (*printer_func) __P((void *, char *, ...));
extern int (*pap_check_hook) __P((void));
extern int (*chap_check_hook) __P((void));
extern void (*info) __P((char *, ...));
extern void (*error) __P((char *, ...));

char pppd_version[] = "2.4.7";

static int check()
{
	return 1;
}

static int seth_override_by_sth_file(const char *fname)
{
	char *papreal;
	if ((err = seth_nkpin_get_from_file(fname, NULL, time(NULL), &papreal)) < 0) {
		info("seth_nkpin_get_from_file error, errcode: %d\n", err);
		return -1;
	}

	strncpy(user, papreal, MAXNAMELEN);
	free(papreal);

	return 0;
}

static int seth_try_sth_file(const char *format)
{
	char filename[PATH_MAX];
	snprintf(filename, PATH_MAX, format, user);
	info("seth: trying '%s'", filename);

	if (access(filename, F_OK) == 0) {
		return seth_override_by_sth_file(filename);
	}

	return -1;
}

static int seth_try_override_file(const char *format)
{
	char filename[PATH_MAX];
	snprintf(filename, PATH_MAX, format, user);
	FILE *fp = fopen(filename, "r");
	if (!fp)
		return -1;
		
	if (fgets(user, MAXNAMELEN, fp) == NULL)
		return -1;

	fclose(fp);
	unlink(filename);
	return 0;
}

static int seth_override()
{
	if (seth_try_sth_file("/tmp/seth/%s.sth3") == 0)
		return 0;
	if (seth_try_sth_file("/var/seth/%s.sth3") == 0)
		return 0;
	if (seth_try_override_file("/tmp/nk-override/%s") == 0)
		return 0;

	return -1;
}

void plugin_init()
{

	info("seth: Seth pppd plugin 0.1 loaded");
	info("seth: copyright 2019 glider0, The-Seth-Project");

	if (seth_override() < 0) {
		error("seth: failed: no data file or override file");
		exit(1);
	}

	info("seth: overrided username: %s", user);

	pap_check_hook = check;
	chap_check_hook = check;
}
