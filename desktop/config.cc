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

#include <glib.h>

#include "config.h"

bool Config::modified = false;
namespace Config {
static GKeyFile *kfile = nullptr;
}

bool Config::open(const char *file)
{
	if (!(kfile = g_key_file_new()))
		return false;

	return (g_key_file_load_from_file(kfile, file, G_KEY_FILE_NONE, nullptr) == TRUE);
}

bool Config::save(const char *file)
{
	if (modified && kfile)
		return (g_key_file_save_to_file(kfile, file, nullptr) == TRUE);
	return true;
}

const char *Config::get(const char *sec, const char *key)
{
	return g_key_file_get_string(kfile, sec, key, nullptr);
}

const char *Config::get(const char *sec, const char *key, const char *def)
{
	const char *value;
	if ((value = get(sec, key)) != nullptr) {
		return value;
	} else {
		return def;
	}
}

long Config::get_int(const char *sec, const char *key)
{
	return g_key_file_get_integer(kfile, sec, key, nullptr);
}
