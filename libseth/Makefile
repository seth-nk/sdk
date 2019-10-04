# 
# copyright 2019 NSWA Maintainers
# copyright 2019 The-Seth-Project
# License: MIT
# This is free software with ABSOLUTELY NO WARRANTY.
# 

PREFIX := /usr/local

CC := gcc
AR := ar
STRIP := strip
INSTALL := install
LN := ln

CFLAGS := -pipe -flto -Wall -O3 -std=c11
LDFLAGS := --lto

OBJS := libseth.o

BINSUFFIX :=
ifeq ($(WASM),1)
	ALL := libseth.wasm
else
	ifeq ($(OS),Windows_NT)
		ALL := libseth.dll
		CFLAGS += -mconsole -D_WIN32
		LDFLAGS += -Wl,--export-all-symbols -Wl,--stack,10000000
		BINSUFFIX := .exe
	else
		LDFLAGS += -rdynamic
		ALL := libseth.so
	endif
	ALL += libseth-static.a
endif

ALL_EXECUTABLE := seth-nkgetpin seth-dumpmeta seth-genhdr seth-split seth-strip

SOMAJOR := 0
SOMINOR := 1
SONAME := libseth.so.$(SOMAJOR)

all : $(ALL) $(ALL_EXECUTABLE)

%.o : %.c
	$(CC) -c $(CFLAGS) -o $@ $<

libseth.so : $(OBJS)
	$(CC) $(CFLAGS) -shared -fPIC -o $(SONAME) $(OBJS) $(LDFLAGS) -Wl,-h$(SONAME)
	$(STRIP) -s $(SONAME)
	$(LN) -s -f $(SONAME) $@

libseth-static.a : $(OBJS)
	$(AR) rcs $@ $(OBJS)

libseth.dll : $(OBJS)
	$(CC) $(CFLAGS) -shared -fPIC -o $@ $(OBJS) $(LDFLAGS)
	$(STRIP) -s $@

libseth.wasm : libseth.c
	clang -DTARGET_WASM=1 \
		-Wall -O3 -flto -std=c11 \
		--target=wasm32 -nostdlib \
		-Wl,--no-entry -Wl,--export-all \
		-Wl,--lto-O3 \
		-o $@ $^

seth-nkgetpin : seth-nkgetpin.c
seth-dumpmeta : seth-dumpmeta.c
seth-genhdr : seth-genhdr.c
seth-split : seth-split.c
seth-strip : seth-strip.c
seth-nkgetpin seth-dumpmeta seth-genhdr seth-split seth-strip:
	$(CC) $(CFLAGS) -no-pie -fno-pie -o $@$(BINSUFFIX) $< -lseth -L. $(LDFLAGS)
	$(CC) $(CFLAGS) -no-pie -fno-pie -o $@-sls$(BINSUFFIX) $< libseth-static.a $(LDFLAGS)
	$(STRIP) -s $@$(BINSUFFIX)
	$(STRIP) -s $@-sls$(BINSUFFIX)

clean:
	rm -f $(ALL) $(ALL_EXECUTABLE) *.o *.a *.exe *.dll *.wasm *.so *.so.* seth-*-sls

install : all
	$(INSTALL) -m644 $(SONAME) $(PREFIX)/lib/libseth.so.$(SOMAJOR).$(SOMINOR)
	$(LN) -s -f libseth.so.$(SOMAJOR).$(SOMINOR) $(PREFIX)/lib/libseth.so.$(SOMAJOR)
	$(LN) -s -f libseth.so.$(SOMAJOR).$(SOMINOR) $(PREFIX)/lib/libseth.so
	$(INSTALL) -m644 nkget.h $(PREFIX)/include/
	$(INSTALL) -m644 seth.h $(PREFIX)/include/
	$(INSTALL) -m755 seth-nkgetpin $(PREFIX)/bin/
	$(INSTALL) -m755 seth-dumpmeta $(PREFIX)/bin/
	$(INSTALL) -m755 seth-genhdr $(PREFIX)/bin/
	$(INSTALL) -m755 seth-split $(PREFIX)/bin/
	$(INSTALL) -m755 seth-strip $(PREFIX)/bin/
	ldconfig

uninstall:
	rm -f $(PREFIX)/lib/libseth.so.*
	rm -f $(PREFIX)/include/seth.h
	rm -f $(PREFIX)/include/nkget.h
	rm -f $(PREFIX)/bin/seth-nkgetpin
	rm -f $(PREFIX)/bin/seth-dumpmeta
	rm -f $(PREFIX)/bin/seth-genhdr
	rm -f $(PREFIX)/bin/seth-split
	rm -f $(PREFIX)/bin/seth-strip
	ldconfig

$(ALL_EXECUTABLE) : $(ALL)
.PHONY: all clean install uninstall
