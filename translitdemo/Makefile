# Copyright (c) 2000-2006 IBM, Inc. and others

# Usage:
#  - configure and build ICU [see the docs] .. use "--prefix=" something
#     (I  used --prefix=/home/srl/III )
# 
#  - do 'make install' of icu 
#
#  - do 'configure' of icuapps (../configure)
#
#  - do 'make' in this directory
#
#  - do make install.    This CGI needs to have the 'data' directory in the
#  same directory as the translit CGI binary.

# make paths in icuapps.mk work
top_srcdir=..

include ../icuapps.mk
#ICU_CONFIG=/data/icu/bin/icu-config
ICU_INC=$(shell $(ICU_CONFIG) --incfile)
BINDIR=$(shell $(ICU_CONFIG) --bindir)

# Must find uhash.h and hash.h
ICU_COMMON_HEADERS=$(ICU_SOURCE)/common
###########################################
-include Makefile.local


include $(ICU_INC)

# Name of your target
TARGET=translit

# All object files (C or C++)
#OBJECTS=TemplateCGI.o TransliteratorCGI.o getcgi.o main.o util.o \
# TextCache.o

OBJECTS +=TemplateCGI.o
OBJECTS += TransliteratorCGI.o
OBJECTS += getcgi.o
OBJECTS +=  main.o
OBJECTS += util.o 
OBJECTS +=  TextCache.o

CPPFLAGS += -I$(ICU_COMMON_HEADERS) 
CPPFLAGS += -I../apputil

ICULIBS := -L../apputil $(ICULIBS) -lapputil $(ICULIBS_ICUIO)

CLEANFILES=*~ $(TARGET).out

all: $(ICU_COMMON_HEADERS)/hash.h $(ICU_COMMON_HEADERS)/uhash.h $(TARGET)


.PHONY: all clean distclean check report

distclean clean:
	-test -z "$(CLEANFILES)" || rm -f $(CLEANFILES)
	-$(RMV) $(OBJECTS) $(TARGET)

# Can change this to LINK.c if it is a C only program
# Can add more libraries here. 
$(TARGET): $(OBJECTS)
	$(LINK.cc) -o $@ $^ $(ICULIBS)

TRANSLITD=$(bindir)/data

install: all
	-mkdir $(bindir)
	cp $(TARGET) $(bindir)
	-mkdir $(TRANSLITD)
	cp -R data/js $(TRANSLITD)/
	cp -R data/*.html $(TRANSLITD)
	-mkdir $(TRANSLITD)/cache
	-chmod a+rw $(TRANSLITD)/cache
	-find $(TRANSLITD) -type d -print | xargs chmod a+x
	-find $(TRANSLITD) -type f -print | xargs chmod a+r

$(ICU_INC):
	@echo "Please read the directions at the top of this file (Makefile)"
	@echo "Can't open $(ICU_INC)"
	@false

$(ICU_COMMON_HEADERS)/hash.h:
	@echo "Need to find hash.h, was not found in $(ICU_COMMON_HEADERS)"
	@echo "Please set 'ICU_SOURCE=/some/where/icu/source/'  in ../icuapps-mk.local"
	@false

$(ICU_COMMON_HEADERS)/uhash.h:
	@echo "Need to find uhash.h, was not found in $(ICU_COMMON_HEADERS)"
	@echo "Please set 'ICU_SOURCE=/some/where/icu/source/'  in ../icuapps-mk.local"
	@false

