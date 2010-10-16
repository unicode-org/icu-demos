#  Copyright (c) 2010 IBM Corporation and Others. All Rights Reserved.
# Note: local.mk must set 'ICU=/path/to/icu'


include ../local.mk
-include local.mk
ifneq ($(ICU_SRC),)
include ../icusrc.mk
endif

ifeq ($(ICU_VER),)
ifneq ($(shell pkg-config --silence-errors icu-io --modversion),)
include ../icupc.mk
endif
endif
ifeq ($(ICU_VER),)
include ../icuconfig.mk
endif


all: $(TARGET)

check: all
	$(INVOKE) ./$(TARGET) $(ARGS)

clean:
	-rm -rf $(TARGET) *~ *.o *.ao

info:
	echo ICU version $(ICU_VER)
