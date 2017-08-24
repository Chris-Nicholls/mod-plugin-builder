#!/usr/bin/make -f
# Makefile for eg-chris.lv2 #
# ----------------------- #
# Created by falkTX
#

include Makefile.mk

# --------------------------------------------------------------

PREFIX  ?= /usr/local
DESTDIR ?=

# --------------------------------------------------------------
# Default target is to build all plugins

all: build
build: eg-chris

# --------------------------------------------------------------
# eg-chris build rules

eg-chris: eg-chris.lv2/chris$(LIB_EXT) eg-chris.lv2/manifest.ttl

eg-chris.lv2/chris$(LIB_EXT): chris.c
	$(CC) $^ $(BUILD_C_FLAGS) $(LINK_FLAGS) -lm $(SHARED) -o $@

eg-chris.lv2/manifest.ttl: eg-chris.lv2/manifest.ttl.in
	sed -e "s|@LIB_EXT@|$(LIB_EXT)|" $< > $@

# --------------------------------------------------------------

clean:
	rm -f eg-chris.lv2/chris$(LIB_EXT) eg-chris.lv2/manifest.ttl

# --------------------------------------------------------------

install: build
	install -d $(DESTDIR)$(PREFIX)/lib/lv2/eg-chris.lv2

	install -m 644 eg-chris.lv2/*.so  $(DESTDIR)$(PREFIX)/lib/lv2/eg-chris.lv2/
	install -m 644 eg-chris.lv2/*.ttl $(DESTDIR)$(PREFIX)/lib/lv2/eg-chris.lv2/

# --------------------------------------------------------------
