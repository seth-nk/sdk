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

#include "backend.h"

std::map<std::string, SDBackend*> *SDBackendManager::backends;

void SDBackendManager::register_backend(SDBackend *backend)
{
#ifdef _DEBUG
	printf("registe backend: %s (%s)\n", backend->getid().data(), backend->getname().data());
#endif // _DEBUG
	if (backends == nullptr)
		backends = new std::map<std::string, SDBackend*>;
	backends->insert({ backend->getid(), backend });
}

SDBackend *SDBackendManager::get_backend(std::string id)
{
	auto backend = backends->find(id);
	if (backend == backends->end())
		return nullptr;
	return backend->second;
}
