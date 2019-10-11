#  Copyright (c) 2010-2011 IBM Corporation and Others. All Rights Reserved.

# add this so mac homebrew works. Can be overridden in local.mk

-include ../local.mk
-include local.mk
ifneq ($(icubuild),)
include ../icusrc.mk
endif

ifeq ($(ICU_VER),)
# make mac homebrew work out of the box
ICUPKGCONFIG=env PKG_CONFIG_PATH=$(PKG_CONFIG_PATH):/usr/local/opt/icu4c/lib/pkgconfig/ pkg-config
ifneq ($(shell $(ICUPKGCONFIG) --silence-errors icu-io --modversion),)
include ../icupc.mk
endif
endif
ifeq ($(ICU_VER),)
include ../icuconfig.mk
endif
