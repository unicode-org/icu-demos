#  Copyright (c) 2008-2013, International Business Machines Corporation and others. All Rights Reserved.
# 
#
# Makefile for regenerating configure in the face of a bad ^M
# This should become unnecessary for autoconf past 2.63
# 
# Usage:    MAKE -f configure.mk configure

all: configure apputil/demo_config.h.in

configure:	configure.ac ./acinclude.m4
	aclocal && autoconf

apputil/demo_config.h.in: configure.ac
	autoheader configure.ac