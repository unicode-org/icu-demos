# requires icu-config in the path

include ../config.mk

# use a tmp dir, and verbose
PKGDATA_OPTS=-v -T tmp
PATH+=:$(icubuild)/bin:$(icubuild)/sbin:/usr/local/opt/icu4c/bin/:/usr/local/opt/icu4c/sbin/
GENRB=env DYLD_LIBRARY_PATH=$(icubuild)/lib  LD_LIBRARY_PATH=$(icubuild)/lib genrb
PKGDATA=env DYLD_LIBRARY_PATH=$(icubuild)/lib  LD_LIBRARY_PATH=$(icubuild)/lib pkgdata

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