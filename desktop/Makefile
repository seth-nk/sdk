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
FLAG_LTO := -flto
FLAG_LD_LTO := --lto
FLAG_OPT := -O3

include scripts/debug.mk

COMMENFLAGS += -Wall $(FLAG_OPT) $(FLAG_LTO) $(EXTRA_FLAGS)
LDFLAGS := $(FLAG_LD_LTO) -static-libgcc -static-libstdc++ -lseth

BINSUFFIX :=
OBJS := backend.o \
	config.o

ifeq ($(OS),Windows_NT)
	include configs/windows.mk
else
	include configs/posix.mk
endif

ALL := $(LIBSETHDKT) sethdkt-cli sethdkt-gtk

include backends/backends.mk

COMMENFLAGS += -DOSNAME='"$(OSNAME)"' $(shell $(PKGCONFIG) --cflags $(PKG_DEP))
LDFLAGS += $(shell $(PKGCONFIG) --libs $(PKG_STATIC) $(PKG_DEP)) $(POST_LDFLAGS)

CFLAGS := -std=c99 $(COMMENFLAGS)
CXXFLAGS := -std=c++11 $(COMMENFLAGS)

all : $(ALL)

$(LIBSETHDKT) : sethcli_conf.h $(OBJS)
	$(CXX) -fPIC -shared $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	$(STRIP) -s $@

sethdkt-cli : frontend-cli.cc $(LIBSETHDKT) sethcli_conf.h
	$(CXX) $(CXXFLAGS) -o $@.o $(shell $(PKGCONFIG) --cflags gtk+-3.0) -c $<
	$(CXX) -no-pie $(CXXFLAGS) -o $@$(BINSUFFIX) $@.o $(LDFLAGS) -lsethdkt -L.
	$(STRIP) -s $@$(BINSUFFIX)

sethdkt-gtk : frontend-gtk.cc $(LIBSETHDKT) sethcli_conf.h
	$(CXX) $(SETHDKT_GTK_EXTRA_FLAGS) $(CXXFLAGS) -o $@.o $(shell $(PKGCONFIG) --cflags gtk+-3.0) -c $<
	$(CXX) -no-pie $(SETHDKT_GTK_EXTRA_FLAGS) $(CXXFLAGS) -o $@$(BINSUFFIX) $@.o $(LDFLAGS) -lsethdkt -L. $(shell $(PKGCONFIG) --libs gtk+-3.0)
	$(STRIP) -s $@$(BINSUFFIX)

sethcli_conf.h : sethcli.conf.in
	./scripts/mkch "$(DEF_BACKEND)" sethcli.conf.in default_sethcli_conf > sethcli_conf.h
 
%.res : %.rc
	windres $< -O coff -o $@

%.o : %.cc
	$(CXX) -fPIC $(CXXFLAGS) -o $@ -c $<

clean:
	rm -f $(ALL) $(OBJS) *.o *.res *.exe *.so *.dll sethcli_conf.h

install:
	$(INSTALL) -m755 $(LIBSETHDKT) $(PREFIX)/lib/
	$(INSTALL) -m755 sethdkt-cli $(PREFIX)/bin/
	$(INSTALL) -m755 sethdkt-gtk $(PREFIX)/bin/
	$(INSTALL) -m755 seth-pppoe $(PREFIX)/bin/
	ldconfig

uninstall:
	rm -f $(PREFIX)/lib/$(LIBSETHDKT)
	rm -f $(PREFIX)/bin/sethdkt-cli
	rm -f $(PREFIX)/bin/sethdkt-gtk
	rm -f $(PREFIX)/bin/seth-pppoe
	# old name
	rm -f $(PREFIX)/bin/sethcli
	ldconfig

.PHONY: all clean install uninstall
