# Copyright (c) 2010 IBM Corporation and Others. All Rights Reserved.
# pkg-config version

ICU_VER=$(shell pkg-config icu-io --modversion)
LDFLAGS+=$(shell pkg-config icu-io --libs)
CPPFLAGS+=$(shell pkg-config icu-io --cflags)
#INVOKE=$(shell icu-config --invoke)
# assume can invoke from command line
INVOKE=
