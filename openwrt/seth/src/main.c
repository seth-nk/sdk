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
void dbglog __P((char *, ...));	/* log a debug message */
void info __P((char *, ...));	/* log an informational message */
void notice __P((char *, ...));	/* log a notice-level message */
void warn __P((char *, ...));	/* log a warning message */
void error __P((char *, ...));	/* log an error message */
void fatal __P((char *, ...));	/* log an error message and die(1) */

typedef void (*notify_func) __P((void *, int));
extern struct notifier *exitnotify;
void add_notifier __P((struct notifier **, notify_func, void *));

char pppd_version[] = _PPPD_VERSION;

static int seth_override_by_sth_file(const char *fname)
{
	char *papreal;
	int err;

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
	info("seth: trying data file '%s'", filename);

	if (access(filename, F_OK) == 0) {
		return seth_override_by_sth_file(filename);
	}

	return -1;
}

static int seth_try_override_file(const char *format)
{
	char filename[PATH_MAX];
	snprintf(filename, PATH_MAX, format, user);
	info("seth: trying override file '%s'", filename);

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
	if (seth_try_sth_file("/etc/seth/%s.sth3") == 0)
		return 0;
	if (seth_try_override_file("/tmp/nk-override/%s") == 0)
		return 0;

	return -1;
}

void on_exitnotify(void *opaque, int arg)
{
	info("seth: pppd exiting: %d", arg);
	if (arg != 5) {
		info("seth: prevent openwrt restarting pppd to protect your account");
		_exit(2);
	}
}

void plugin_init()
{
	info("seth: Seth pppd plugin 0.2.1 loaded");
	info("seth: copyright 2019-2020 glider0, The-Seth-Project");

	if (seth_override() < 0) {
		error("seth: failed: no data file or override file");
		exit(1);
	}

	add_notifier(&exitnotify, on_exitnotify, NULL);

	info("seth: overrided username: %s", user);
}
