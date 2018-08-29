# requires icu-config in the path

include ../config.mk

# use a tmp dir, and verbose
PKGDATA_OPTS=-v -T tmp
# PATH+=:$(icubuild)/bin:$(icubuild)/sbin
GENRB=env DYLD_LIBRARY_PATH=$(icubuild)/lib  LD_LIBRARY_PATH=$(icubuild)/lib $(icubuild)/bin/genrb
PKGDATA=env DYLD_LIBRARY_PATH=$(icubuild)/lib  LD_LIBRARY_PATH=$(icubuild)/lib $(icubuild)/bin/pkgdata

all:
	-mkdir myapp
	$(GENRB) -d myapp root.txt
	$(GENRB) -d myapp es.txt
	-mkdir tmp
	$(PKGDATA) $(PKGDATA_OPTS) -p myapp pkgdatain.txt
clean:
	-rm -rf tmp myapp*

check:
	@echo nothing to check