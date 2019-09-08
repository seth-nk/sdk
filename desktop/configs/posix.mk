# 
# copyright 2019 NSWA Maintainers
# copyright 2019 The-Seth-Project
# License: MIT
# This is free software with ABSOLUTELY NO WARRANTY.
# 

LIBSETHDKT := libsethdkt.so

OSNAME := posix
DEF_BACKEND := rp-pppoe

FLAG_LTO := -flto
FLAG_LD_LTO := --lto

SETHDKT_CLI_EXTRA_FLAGS := -rdynamic
SETHDKT_GTK_EXTRA_FLAGS := -rdynamic

CONFIG_BACKEND_RP_PPPOE := y
CONFIG_BACKEND_NMCLI := y
CONFIG_BACKEND_RANGA := y
CONFIG_BACKEND_WEBHOOK := y
CONFIG_PLUGIN_SYNC := y
