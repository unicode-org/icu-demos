# Copyright (c) 2010 IBM Corporation and Others. All Rights Reserved.
# icu-config version
ICU_VER=$(shell icu-config --version)
LDFLAGS+=$(shell icu-config --ldflags --ldflags-icuio)
CPPFLAGS+=$(shell icu-config --cppflags)
INVOKE=$(shell icu-config --invoke)
