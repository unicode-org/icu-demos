#  Copyright (c) 2010-2011 IBM Corporation and Others. All Rights Reserved.

-include ../local.mk
-include local.mk
ifneq ($(icubuild),)
include ../icusrc.mk
endif

ifeq ($(ICU_VER),)
ifneq ($(shell pkg-config --silence-errors icu-io --modversion),)
include ../icupc.mk
endif
endif
ifeq ($(ICU_VER),)
include ../icuconfig.mk
endif
