# Copyright (c) 2010 IBM Corporation and Others. All Rights Reserved.
# pkg-config version

ICU_VER=$(shell $(ICUPKGCONFIG) icu-io --modversion)
LDFLAGS+=$(shell $(ICUPKGCONFIG) icu-io --libs)
CPPFLAGS+=$(shell $(ICUPKGCONFIG) icu-io --cflags)
#INVOKE=$(shell icu-config --invoke)
# assume can invoke from command line
CXX=c++ -std=c++11

INVOKE=

# make sure LDFLAGS come last due to library ordering

%: %.c
	$(CC) $(CPPFLAGS) -o $@ $< $(LDFLAGS)

%: %.cc
	$(CXX) $(CPPFLAGS) -o $@ $< $(LDFLAGS)

%: %.cpp
	$(CXX) $(CPPFLAGS) -o $@ $< $(LDFLAGS)
