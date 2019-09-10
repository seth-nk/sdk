# 
# copyright 2019 NSWA Maintainers
# copyright 2019 The-Seth-Project
# License: MIT
# This is free software with ABSOLUTELY NO WARRANTY.
# 

PKG_DEP := glib-2.0

PREFIX := /usr/local

INSTALL := install
LN := ln
CC := gcc
CXX := g++
PKGCONFIG := pkg-config
STRIP := strip

COMMENFLAGS := -pipe
FLAG_LTO :=
FLAG_LD_LTO :=
FLAG_OPT := -O3

include scripts/debug.mk

COMMENFLAGS += -Wall $(FLAG_OPT) $(FLAG_LTO) $(EXTRA_FLAGS)
LDFLAGS := $(FLAG_LD_LTO) -static-libgcc -static-libstdc++ -lseth

BINSUFFIX :=
FRONTENDS_OBJS :=
OBJS := backend.o \
	config.o

ALL := 

ifeq ($(OS),Windows_NT)
	include configs/windows.mk
else
	include configs/posix.mk
endif

ALL += $(LIBSETHDKT) sethdkt-cli sethdkt-gtk

include backends/backends.mk

COMMENFLAGS += -DOSNAME='"$(OSNAME)"' $(shell $(PKGCONFIG) --cflags $(PKG_DEP))
LDFLAGS += $(shell $(PKGCONFIG) --libs $(PKG_STATIC) $(PKG_DEP)) $(POST_LDFLAGS)

CFLAGS := -std=c99 $(COMMENFLAGS)
CXXFLAGS := -std=c++11 $(COMMENFLAGS)

all : $(ALL)

$(LIBSETHDKT) : sethcli_conf.o $(OBJS)
	$(CXX) -fPIC -shared $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	$(STRIP) -s $@

sethdkt-cli : frontend-cli.cc $(LIBSETHDKT) $(FRONTENDS_OBJS)
	$(CXX) $(SETHDKT_CLI_EXTRA_FLAGS) $(CXXFLAGS) -o $@.o -c $<
	$(CXX) -no-pie $(SETHDKT_CLI_EXTRA_FLAGS) $(CXXFLAGS) -o $@$(BINSUFFIX) $@.o $(FRONTENDS_OBJS) $(LDFLAGS) -lsethdkt -L.
	$(STRIP) -s $@$(BINSUFFIX)

frontend-gtk-glade.h :  frontend-gtk.glade
	./scripts/bin2h frontend-gtk.glade glade > frontend-gtk-glade.h

sethdkt-gtk : frontend-gtk.cc $(LIBSETHDKT) frontend-gtk-glade.h $(FRONTENDS_OBJS)
	$(CXX) $(SETHDKT_GTK_EXTRA_FLAGS) $(CXXFLAGS) -o $@.o $(shell $(PKGCONFIG) --cflags gtk+-3.0) -c $<
	$(CXX) -no-pie $(SETHDKT_GTK_EXTRA_FLAGS) $(CXXFLAGS) -o $@$(BINSUFFIX) $@.o $(FRONTENDS_OBJS) $(LDFLAGS) -lsethdkt -L. $(shell $(PKGCONFIG) --libs gtk+-3.0)
	$(STRIP) -s $@$(BINSUFFIX)

sethdkt-gtkWL.exe : win32lancher.c $(FRONTENDS_OBJS)
	$(CC) -no-pie -mwindows $(CFLAGS) -o $@ $< $(FRONTENDS_OBJS) -static-libgcc -lShell32 -lShlwapi

sethcli_conf.c : sethcli.conf.in
	./scripts/mkch "$(DEF_BACKEND)" $< default_sethcli_conf > $@
 
%.res : %.rc
	windres $< -O coff -o $@

%.o : %.cc
	$(CXX) -fPIC $(CXXFLAGS) -o $@ -c $<

clean:
	rm -f $(ALL) $(OBJS) *.o *.res *.exe *.so *.dll sethcli_conf.c frontend-gtk-glade.h

install:
	$(INSTALL) -m755 $(LIBSETHDKT) $(PREFIX)/lib/
	$(INSTALL) -m755 sethdkt-cli $(PREFIX)/bin/
	$(INSTALL) -m755 sethdkt-gtk $(PREFIX)/bin/
	cp -r uxdist/. $(PREFIX)/
	ldconfig

uninstall:
	rm -f $(PREFIX)/lib/$(LIBSETHDKT)
	rm -f $(PREFIX)/bin/sethdkt-cli
	rm -f $(PREFIX)/bin/sethdkt-gtk
	./scripts/uxdist-uninstall $(PREFIX)
	# old name
	rm -f $(PREFIX)/bin/sethcli
	ldconfig

.PHONY: all clean install uninstall
