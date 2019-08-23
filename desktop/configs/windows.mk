# 
# copyright 2019 NSWA Maintainers
# copyright 2019 The-Seth-Project
# License: MIT
# This is free software with ABSOLUTELY NO WARRANTY.
# 

BINSUFFIX := 4win.exe
COMMENFLAGS += -mconsole -D_WIN32 -I../libseth
LDFLAGS += -L../libseth -lws2_32 -lRasapi32 -lShlwapi -Wl,-Bstatic
OBJS += seth4win.res

OSNAME := Windows
DEF_BACKEND := rasapi
PKG_STATIC := --static
POST_LDFLAGS += -lpthread -liconv

CONFIG_BACKEND_RASAPI := y
CONFIG_BACKEND_RANGA := y
CONFIG_BACKEND_WEBHOOK := y
CONFIG_PLUGIN_SYNC := y
