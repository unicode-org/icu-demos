# Copyright (c) 2000 IBM, Inc. and others

# Usage:
#  - configure and build ICU [see the docs] .. use "--prefix=" something
#     (I  used --prefix=/home/srl/III )
# 
#  - do 'make install' of icu 
#
#  - change the following line to point to the $(prefix) that 
#    was used (will look for $(prefix)/share/icu/Makefile.inc )
#      OR 
#   set the variable ICU_PREFIX to point at $(prefix)
#
#  - ICU_COMMON_HEADERS must be set to were ICU's common/unicode directory
#    is. 
#  
#  - do 'make' in this directory

ICU_DEFAULT_PREFIX=~/icu

ifeq ($(strip $(ICU_PREFIX)),)
  ICU_INC=$(ICU_DEFAULT_PREFIX)/share/icu/Makefile.inc
else
  ICU_INC=$(ICU_PREFIX)/share/icu/Makefile.inc
endif
ICUPATH=

###########################################
# HACK : TODO fix this
ICU_INC=~/II/lib/icu/Makefile.inc

# Must find uhash.h
ICU_COMMON_HEADERS=../../icu/source/common
###########################################


include $(ICU_INC)

# Name of your target
TARGET=translit

# All object files (C or C++)
#OBJECTS=TemplateCGI.o TransliteratorCGI.o getcgi.o main.o util.o \
#uhash.o TextCache.o

OBJECTS +=TemplateCGI.o
OBJECTS += TransliteratorCGI.o
OBJECTS += getcgi.o
OBJECTS +=  main.o
OBJECTS += util.o 
#OBJECTS += uhash.o
OBJECTS +=  TextCache.o

CPPFLAGS += -I$(ICU_COMMON_HEADERS)

CLEANFILES=*~ $(TARGET).out

DEPS=$(OBJECTS:.o=.d)

all: $(ICU_COMMON_HEADERS)/uhash.h $(TARGET)

.PHONY: all clean distclean check report

distclean clean:
	-test -z "$(CLEANFILES)" || rm -f $(CLEANFILES)
	-$(RMV) $(OBJECTS) $(TARGET)
	-$(RMV) $(DEPS)

# Can change this to LINK.c if it is a C only program
# Can add more libraries here. 
$(TARGET): $(OBJECTS)
	$(LINK.cc) -o $@ $^ $(ICULIBS)

$(ICU_INC):
	@echo "Please read the directions at the top of this file (Makefile)"
	@echo "Can't open $(ICU_INC)"
	@false

$(ICU_COMMON_HEADERS)/uhash.h:
	@echo "Need to find uhash.h, was not found in $(ICU_COMMON_HEADERS)"
	@echo "Please change ICU_COMMON_HEADERS in this makefile."
	@false

ifneq ($(MAKECMDGOALS),distclean)
-include $(DEPS)
endif
