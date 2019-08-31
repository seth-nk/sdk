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
COMMENFLAGS := -Wall -O3 -flto $(EXTRA_FLAGS)
STRIP := strip
LDFLAGS := -no-pie --lto -static-libgcc -static-libstdc++ -lseth

BINSUFFIX :=
ALL := sethcli
OBJS := backend.o \
	main.o \
	config.o

ifeq ($(OS),Windows_NT)
	include configs/windows.mk
else
	include configs/posix.mk
endif

include backends/backends.mk

COMMENFLAGS += -DOSNAME='"$(OSNAME)"' $(shell pkg-config --cflags $(PKG_DEP))
LDFLAGS += $(shell pkg-config --libs $(PKG_STATIC) $(PKG_DEP)) $(POST_LDFLAGS)

CFLAGS := -std=c99 $(COMMENFLAGS)
CXXFLAGS := -std=c++11 $(COMMENFLAGS)

all : $(ALL)

sethcli : sethcli_conf.h $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@$(BINSUFFIX) $^ $(LDFLAGS)
	$(STRIP) -s $@$(BINSUFFIX)
	
sethcli_conf.h : sethcli.conf.in
	./scripts/mkch "$(DEF_BACKEND)" sethcli.conf.in default_sethcli_conf > sethcli_conf.h
 
%.res : %.rc
	windres $< -O coff -o $@

%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $<

%.o : %.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	rm -f $(ALL) $(OBJS) *.o *.res *.exe sethcli_conf.h

install:
	$(INSTALL) -m755 sethcli $(PREFIX)/bin/
	$(INSTALL) -m755 seth-pppoe $(PREFIX)/bin/

uninstall:
	rm -f $(PREFIX)/bin/sethcli

.PHONY: all clean install uninstall
