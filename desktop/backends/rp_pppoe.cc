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
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include "../backend.h"
#include "../config.h"
#include "../version.h"

class SDBackendRpPppoe : public SDBackend {
private:
	std::string lasterror;
	bool kernel;
public:
	SDBackendRpPppoe(bool k) : kernel(k) {};
	virtual std::string getid() { return (kernel ? "rp-pppoe-linux" : "rp-pppoe"); };
	virtual std::string getname() { return (kernel ? "RP-PPPoE kernel mode (Linux only)": "RP-PPPoE user mode (Unix/Linux)"); };
	virtual bool dialup(const char *usrnam, const char *passwd);
	virtual bool hangup();
	virtual bool getstat();
	virtual std::string get_last_error();
};

__attribute__((constructor)) static void backend_rp_pppoe_main(void)
{
	SDBackendRpPppoe *backend;
	backend = new SDBackendRpPppoe(false);
	SDBackendManager::register_backend(backend);
	backend = new SDBackendRpPppoe(true);
	SDBackendManager::register_backend(backend);
}

bool start_daemon(...)
{
	/* fixme: stub */
	return true;
}

void load_os_ppp_driver(bool km)
{
	/* fixme: stub */
}

int if_set_up(const char *ifname)
{
	/* fixme: stub */
	return 0;
}

static const char *rp_pppoe_linux_km_plugins[] = {
	"/etc/ppp/plugins/rp-pppoe.so",
	"/usr/lib/pppd/2.4.7/rp-pppoe.so",
	"/usr/lib/rp-pppoe/rp-pppoe.so",
	"rp-pppoe.so",
	nullptr
};

bool SDBackendRpPppoe::dialup(const char *usrnam, const char *passwd)
{
	const char *ifname = Config::get("pppoe", "interface", "eth0");
	const char *plugin = "";

	load_os_ppp_driver(this->kernel);
	if_set_up(ifname);

	if (this->kernel) {
		std::string nic = std::string("nic-") + ifname;

		int tmp = 0;
		while (rp_pppoe_linux_km_plugins[tmp] != nullptr) {
			if (access(rp_pppoe_linux_km_plugins[tmp], F_OK) == 0) {
				plugin = rp_pppoe_linux_km_plugins[tmp];
				break;
			}
			tmp++;
		}

		if (strcmp(plugin, "") == 0) {
			lasterror = "Kernel mode rp-pppoe not supported for your system. please try user mode\n";
			return false;
		}

		return start_daemon("/usr/sbin/pppd", "plugin", plugin, \
			nic.data(), "noipdefault", "noauth", \
			"default-asyncmap", "defaultroute", \
			"hide-password", "updetach", "usepeerdns"\
			"mtu", "1492", "mru", "1492", "noaccomp", \
			"nodeflate", "nopcomp", "novj", "novjccomp", \
			"user", usrnam, "password", passwd, \
			"lcp-echo-interval", "20", "lcp-echo-failure", "3", \
			"ifname", SETHD_PPPOE_IFNAME, nullptr);
	} else {
		return start_daemon("/usr/sbin/pppd", "pty", \
			"/usr/sbin/pppoe -I eth0 -T 80 -U -m 1412", \
			"noipdefault", "noauth", \
			"default-asyncmap", "defaultroute", \
			"hide-password", "updetach", "usepeerdns"\
			"mtu", "1492", "mru", "1492", "noaccomp", \
			"nodeflate", "nopcomp", "novj", "novjccomp", \
			"user", usrnam, "password", passwd, \
			"lcp-echo-interval", "20", "lcp-echo-failure", "3", \
			"ifname", SETHD_PPPOE_IFNAME, nullptr);
	}
}

bool SDBackendRpPppoe::hangup()
{
	/* fixme: stub */
	return true;
}

bool SDBackendRpPppoe::getstat()
{
	int fd;
	struct ifreq ifr;
	char iface[] = SETHD_PPPOE_IFNAME;

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name , iface , IFNAMSIZ - 1);

	return (ioctl(fd, SIOCGIFINDEX, &ifr) == 0);
}

std::string SDBackendRpPppoe::get_last_error()
{
	return this->lasterror;
}
