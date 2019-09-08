# 
# copyright 2019 NSWA Maintainers
# copyright 2019 The-Seth-Project
# License: MIT
# This is free software with ABSOLUTELY NO WARRANTY.
# 

LIBSETHDKT := libsethdkt.dll

ALL += sethdkt-gtkWL.exe

BINSUFFIX := .exe
COMMENFLAGS += -D_WIN32 -I../libseth
LDFLAGS += -L../libseth -lws2_32 -lRasapi32 -lShlwapi -Wl,-Bstatic

FRONTENDS_OBJS += seth4win.res

OSNAME := Windows
DEF_BACKEND := rasapi
PKG_STATIC := --static
POST_LDFLAGS += -lpthread -lintl -liconv -Wl,--export-all-symbols -Wl,-Bdynamic

SETHDKT_GTK_EXTRA_FLAGS := -mwindows

CONFIG_BACKEND_RASAPI := y
CONFIG_BACKEND_RANGA := y
CONFIG_BACKEND_WEBHOOK := y
CONFIG_PLUGIN_SYNC := y
