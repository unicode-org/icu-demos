# Copyright (c) 2010 IBM Corporation and Others. All Rights Reserved.
# for running against a non-installed ICU: set ICU.
# this should work on linuxes and mac

ICULIBDIR=$(ICU_SRC)/source/lib
LDFLAGS+=-L$(ICULIBDIR) -licuuc -licui18n -licuio
CPPFLAGS+=-I$(ICU_SRC)/source/common -I$(ICU_SRC)/source/i18n -I$(ICU_SRC)/source/io
INVOKE=env LD_LIBRARY_PATH=$(ICULIBDIR):$$LD_LIBRARY_PATH DYLD_LIBRARY_PATH=$(ICULIBDIR)::$$LD_LIBRARY_PATH
ICU_VER=$(shell sed -n "s/.*U_ICU_VERSION[ 	][ 	]*[^0-9.]*\([0-9.]*\).*/\1/p" < $(ICU_SRC)/source/common/unicode/uvernum.h)
