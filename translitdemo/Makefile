# Copyright (c) 2000 IBM, Inc. and others

# Usage:
#  - configure and build ICU [see the docs] .. use "--prefix=" something
#     (I  used --prefix=/home/srl/III )
# 
#  - do 'make install' of icu 
#
#  - if "icu-config" isn't in the PATH, add it to the path.
#
#  - We expect the ICU build to be relative to this directory (ICUAPPS),
#     if NOT, change the ICU_COMMON_HEADERS variable to point to the location
#     of  icu/source/common/uhash.h
#  
#  - do 'make' in this directory
#
#  - do make install.    This CGI needs to have the 'data' directory in the
#  same directory as the translit CGI binary.

ICU_CONFIG=icu-config
ICU_INC=$(shell $(ICU_CONFIG) --incfile)
BINDIR=$(shell $(ICU_CONFIG) --bindir)

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

all: $(ICU_COMMON_HEADERS)/uhash.h $(TARGET)


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
	cp -R js $(TRANSLITD)/
	cp -R data/*.html $(TRANSLITD)
	-mkdir $(TRANSLITD)/cache
	chmod a+rw $(TRANSLITD)/cache
	find $(TRANSLITD) -type d -print | xargs chmod a+x
	find $(TRANSLITD) -type f -print | xargs chmod a+r

$(ICU_INC):
	@echo "Please read the directions at the top of this file (Makefile)"
	@echo "Can't open $(ICU_INC)"
	@false

$(ICU_COMMON_HEADERS)/uhash.h:
	@echo "Need to find uhash.h, was not found in $(ICU_COMMON_HEADERS)"
	@echo "Please change ICU_COMMON_HEADERS in this makefile."
	@false

