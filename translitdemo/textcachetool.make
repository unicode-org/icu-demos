
ICU_DEFAULT_PREFIX=~/icu

ifeq ($(strip $(ICU_PREFIX)),)
  ICU_INC=$(ICU_DEFAULT_PREFIX)/share/icu/Makefile.inc
else
  ICU_INC=$(ICU_PREFIX)/share/icu/Makefile.inc
endif
ICUPATH=

###########################################
# HACK : TODO fix this
ICU_INC=/usr/local/lib/icu/Makefile.inc
###########################################

include $(ICU_INC)

# Name of your target
TARGET=tct

# All object files (C or C++)
OBJECTS=util.o uhash.o TextCache.o textcachetool.o

CLEANFILES=*~ $(TARGET).out

DEPS=$(OBJECTS:.o=.d)

all: $(TARGET)

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

ifneq ($(MAKECMDGOALS),distclean)
-include $(DEPS)
endif
