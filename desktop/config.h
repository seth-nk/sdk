#ifndef _CONFIG_H
#define _CONFIG_H

namespace Config {
	extern bool modified;

	void add_override(const char *key, const char *value);
	bool open(const char *file);
	bool open_mem(const char *data, size_t length);
	bool save(const char *file);
	const char *__get_overided_value(const char *sec, const char *key);
	const char *get(const char *sec, const char *key);
	const char *get(const char *sec, const char *key, const char *def);
	long get_int(const char *sec, const char *key);
}

#endif /* _CONFIG_H */
