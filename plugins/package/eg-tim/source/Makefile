	#!/usr/bin/make -f
# Makefile for eg-tim.lv2 #
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
build: eg-tim

# --------------------------------------------------------------
# eg-tim build rules

eg-tim: eg-tim.lv2/tim$(LIB_EXT) eg-tim.lv2/manifest.ttl

eg-tim.lv2/tim$(LIB_EXT): tim.c
	$(CC) $^ $(BUILD_C_FLAGS) $(LINK_FLAGS) -std=c99 -lm $(SHARED) -o $@

eg-tim.lv2/manifest.ttl: eg-tim.lv2/manifest.ttl.in
	sed -e "s|@LIB_EXT@|$(LIB_EXT)|" $< > $@

# --------------------------------------------------------------

clean:
	rm -f eg-tim.lv2/tim$(LIB_EXT) eg-tim.lv2/manifest.ttl

# --------------------------------------------------------------

install: build
	install -d $(DESTDIR)$(PREFIX)/lib/lv2/eg-tim.lv2

	install -m 644 eg-tim.lv2/*.so  $(DESTDIR)$(PREFIX)/lib/lv2/eg-tim.lv2/
	install -m 644 eg-tim.lv2/*.ttl $(DESTDIR)$(PREFIX)/lib/lv2/eg-tim.lv2/

# --------------------------------------------------------------
