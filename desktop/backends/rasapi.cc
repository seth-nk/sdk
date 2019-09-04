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

#ifdef _WIN32
#define WINVER 0x0601
#define _WIN32_WINNT 0x0601
#define RASNP_Ipv6 0x00000008

#include <windows.h>
#include <ras.h>
#include <raserror.h>
#include <shlwapi.h>
#endif /* _WIN32 */

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

#include "../backend.h"
#include "../version.h"

class SDBackendRasapi : public SDBackend {
private:
	std::string lasterror;
public:
	virtual std::string getid() { return "rasapi"; };
	virtual std::string getname() { return "Win32 RAS API PPPoE"; };
	virtual bool dialup(const char *usrnam, const char *passwd);
	virtual bool hangup();
	virtual bool getstat();
	virtual std::string get_last_error();
};

__attribute__((constructor)) static void backend_rasapi_main(void)
{
	SDBackendRasapi *backend = new SDBackendRasapi;
	SDBackendManager::register_backend(backend);
}

// Fuck the Woe32!
// Stinky and too long API, and lame Hungarian nomenclature !!

bool SDBackendRasapi::dialup(const char *usrnam, const char *passwd)
{
#ifdef _WIN32
	RASENTRYA pRasEntry = { 0 };
	pRasEntry.dwSize = sizeof(pRasEntry);
	pRasEntry.szLocalPhoneNumber[0] = '\0';
	strcpy(pRasEntry.szDeviceType, RASDT_PPPoE);
	pRasEntry.dwFramingProtocol = RASFP_Ppp;
	pRasEntry.dwType = RASET_Broadband;
	pRasEntry.dwfOptions = RASEO_SwCompression | RASEO_RemoteDefaultGateway | RASEO_PreviewUserPw;
	pRasEntry.dwfNetProtocols = RASNP_Ip | RASNP_Ipv6;
	pRasEntry.dwEncryptionType = ET_Optional;
	pRasEntry.dwfOptions2 = RASEO2_Internet;
	RasSetEntryPropertiesA(NULL, SETHD_PPPOE_IFNAME, &pRasEntry, sizeof(pRasEntry), NULL, 0);

	HRASCONN hRasConn = NULL;
	RASDIALPARAMSA pRasPara = { 0 };
	pRasPara.dwSize = sizeof(RASDIALPARAMSA);
	strcpy(pRasPara.szEntryName, SETHD_PPPOE_IFNAME);
	strcpy(pRasPara.szCallbackNumber, "");
	strcpy(pRasPara.szPhoneNumber, "");
	strcpy(pRasPara.szDomain, "");
	strcpy(pRasPara.szUserName, usrnam);
	strcpy(pRasPara.szPassword, passwd);
	DWORD pDialRes;
	if ((pDialRes = RasDialA(NULL, NULL, &pRasPara, 0, NULL, &hRasConn)) != ERROR_SUCCESS) {
		RasHangUpA(hRasConn);
		wchar_t szRasString[512];
		RasGetErrorStringW(pDialRes, szRasString, 512);
		size_t len = wcslen(szRasString) + sizeof(wchar_t);
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, szRasString, len, NULL, 0, NULL, NULL);
		std::string tmp(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, szRasString, len, &tmp[0], size_needed, NULL, NULL);
		tmp = std::string(tmp.data());
		lasterror = std::string("windows RAS error '") + std::to_string(pDialRes) + "' (" + tmp + ")";
		return false;
	}

	return true;
#else
	lasterror = "RAS API backend require a Windows OS.";
	return false;
#endif /* _WIN32 */
}

#ifdef _WIN32
static bool get_hrasconn(HRASCONN *hRasConn)
{
	DWORD dwRet = 0;
	LPRASCONN lpRasConn = NULL;
	LPRASCONN lpTempRasConn = NULL;
	DWORD cb = 0;
	DWORD dwConnections = 0;
	DWORD i = 0;
	BOOL fSuccess = FALSE;

	dwRet = RasEnumConnections(lpRasConn, &cb, &dwConnections);

	switch (dwRet) {
	case ERROR_BUFFER_TOO_SMALL:
		lpRasConn = (LPRASCONN)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, cb);
		if (NULL == lpRasConn)
			return 0;

		lpRasConn->dwSize = sizeof(RASCONN);
		dwRet = RasEnumConnections(lpRasConn, &cb, &dwConnections);
		if (ERROR_SUCCESS == dwRet) {
			fSuccess = TRUE;
		} else {
			goto done;
		}
		break;
	case ERROR_SUCCESS:
		fSuccess = TRUE;
		break;
	default:
		break;
	}

	if (fSuccess) {
		lpTempRasConn = lpRasConn;
		for (i = 0; i < dwConnections; i++) {
			if (strcmp(SETHD_PPPOE_IFNAME, lpTempRasConn->szEntryName) == 0) {
				*hRasConn = lpTempRasConn->hrasconn;
				HeapFree(GetProcessHeap(), 0, (LPVOID) lpRasConn);
				return true;
			}
			lpTempRasConn++;
		}
	}

done:
	HeapFree(GetProcessHeap(), 0, (LPVOID) lpRasConn);
	return false;
}
#endif /* _WIN32 */

bool SDBackendRasapi::hangup()
{
#ifdef _WIN32
	HRASCONN hrasconn;
	get_hrasconn(&hrasconn);
	RasHangUpA(hrasconn);
	return true;
#else
	return false;
#endif /* _WIN32 */
}

bool SDBackendRasapi::getstat()
{
#ifdef _WIN32
	HRASCONN hrasconn;
	return get_hrasconn(&hrasconn);
#else
	return false;
#endif /* _WIN32 */
}

std::string SDBackendRasapi::get_last_error()
{
	return this->lasterror;
}
