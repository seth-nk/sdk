# 
# copyright 2019 NSWA Maintainers
# copyright 2019 The-Seth-Project
# License: MIT
# This is free software with ABSOLUTELY NO WARRANTY.
# 

CC := gcc
STRIP := strip

CFLAGS := -pipe -flto -Wall -O3 -std=c11 -I../include -I..
LDFLAGS := --lto

ALL := seth-demo
OBJS := seth-demo.o
BINSUFFIX :=

ifeq ($(OS),Windows_NT)
	CFLAGS += -mconsole -D_WIN32
	LDFLAGS += -lws2_32 -Wl,--export-all-symbols
	BINSUFFIX := .exe
else
	LDFLAGS += -rdynamic
endif

all : $(ALL)

%.o : %.c
	$(CC) -c $(CFLAGS) -o $@ $<

seth-demo : $(OBJS)
	$(CC) $(CFLAGS) -no-pie -fno-pie -o $@$(BINSUFFIX) $< $(LDFLAGS) -lseth -L..
	$(CC) $(CFLAGS) -no-pie -fno-pie -o $@-sls$(BINSUFFIX) $< $(LDFLAGS) -lseth-static -L..
	$(STRIP) -s $@$(BINSUFFIX)
	$(STRIP) -s $@-sls$(BINSUFFIX)

clean:
	rm -f $(ALL) *.o *.a *.exe

.PHONY: all clean
