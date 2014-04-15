#  Copyright (c) 2010-2013 IBM Corporation and Others. All Rights Reserved.

# set 'RESNAME=reshello'  to build resources 'reshello'.

ifneq ($(RESNAME),)
ifeq ($(RESPAR),)
RESPAR=.
endif
RESIN=$(RESPAR)/$(RESNAME)
RESOUT=./out
EXTRA_CLEAN=$(RESOUT)
EXTRA_TARGETS=build-res
endif

ifeq ($(TARGET),)
TARGET:=$(shell basename $(shell pwd))
endif

include ../samples.mk

ifneq ($(RESNAME),)
#use this (or more v's) for verbose build
#BLDICURES_OPTS=-v

build-res:
	-rm -rf "$(RESOUT)"
	../bldicures.py $(BLDICURES_OPTS) --name "$(RESNAME)" --dest "$(RESOUT)" --from "$(RESIN)"

.PHONY: $(EXTRA_TARGETS)
endif
