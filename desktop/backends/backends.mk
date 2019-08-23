# 
# copyright 2019 NSWA Maintainers
# copyright 2019 The-Seth-Project
# License: MIT
# This is free software with ABSOLUTELY NO WARRANTY.
# 

obj-$(CONFIG_BACKEND_RASAPI)   += backends/rasapi.o
obj-$(CONFIG_BACKEND_RP_PPPOE) += backends/rp_pppoe.o
obj-$(CONFIG_BACKEND_NMCLI)    += backends/nmcli.o

pkgdep-$(CONFIG_BACKEND_RP_PPPOE) += gio-2.0
pkgdep-$(CONFIG_BACKEND_NMCLI)    += libnm

OBJS += $(obj-y)
PKG_DEP += $(pkgdep-y)
