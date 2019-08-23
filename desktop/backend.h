#ifndef _BACKEND_H
#define _BACKEND_H

#include <map>

class SDBackend {
private:
public:
	virtual std::string getid() = 0;
	virtual std::string getname() = 0;
	virtual bool dialup(const char *usrnam, const char *passwd) = 0;
	virtual bool hangup() = 0;
	virtual bool getstat() = 0;
	virtual std::string get_last_error() = 0;
};

namespace SDBackendManager {
	extern std::map<std::string, SDBackend*> *backends;
	void register_backend(SDBackend *backend);
	SDBackend *get_backend(std::string id);
};

#endif /* _BACKEND_H */
