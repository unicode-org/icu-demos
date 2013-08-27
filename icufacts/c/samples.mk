#  Copyright (c) 2010-2012 IBM Corporation and Others. All Rights Reserved.
# Note: local.mk must set 'ICU=/path/to/icu'

include ../config.mk


all: $(TARGET)

check: all
	$(INVOKE) ./$(TARGET) $(ARGS)

clean:
	-rm -rf $(TARGET) *~ *.o *.ao

info:
	echo ICU version $(ICU_VER)

fixign:
	svn ps svn:ignore $(TARGET) .
