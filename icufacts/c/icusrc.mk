# Copyright (c) 2010-2011 IBM Corporation and Others. All Rights Reserved.
# for running against a non-installed ICU: set icubuild

srcdir=$(shell sed -ne "s%^srcdir='\(.*\)'%\1%gp" < "$(icubuild)/config.status" )

ifeq ($(srcdir),)
$(error could not find $$srcdir - does $$icubuild point to a built ICU?)
endif
ifeq ($(srcdir),.)
srcdir:=$(icubuild)
endif

cross_buildroot:=$(icubuild)
include $(icubuild)/config/icucross.mk

ICULIBDIR=$(icubuild)/lib
LDFLAGS+=-L$(ICULIBDIR) -licuuc -licui18n -licuio
CPPFLAGS+=-I$(srcdir)/common -I$(srcdir)/i18n -I$(srcdir)/io -I$(icubuild)/common
ICU_VER=$(CROSS_ICU_VERSION)

