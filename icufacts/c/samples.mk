#  Copyright (c) 2010-2013 IBM Corporation and Others. All Rights Reserved.
# Note: local.mk must set 'ICU=/path/to/icu'

include ../config.mk


all: $(TARGET) $(EXTRA_TARGETS)

check: all $(EXTRA_TARGETS)
	$(INVOKE) ./$(TARGET) $(ARGS)

clean:
	-rm -rf $(TARGET) *~ *.o *.ao $(EXTRA_CLEAN)

info:
	echo ICU version $(ICU_VER)

fixign:
	svn ps svn:ignore $(TARGET) .
