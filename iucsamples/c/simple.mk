ifeq ($(TARGET),)
TARGET:=$(shell basename $(shell pwd))
endif

include ../samples.mk

