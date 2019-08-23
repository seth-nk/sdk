#ifndef _CONFIG_H
#define _CONFIG_H

namespace Config {
	extern bool modified;

	bool open(const char *file);
	bool save(const char *file);
	const char *get(const char *sec, const char *key);
	const char *get(const char *sec, const char *key, const char *def);
	long get_int(const char *sec, const char *key);
}

#endif /* _CONFIG_H */
