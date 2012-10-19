# requires icu-config in the path

include ../config.mk

# use a tmp dir, and verbose
PKGDATA_OPTS=-v -T tmp

all:
	-mkdir myapp
	genrb -d myapp root.txt
	genrb -d myapp es.txt
	-mkdir tmp
	$(shell which pkgdata) $(PKGDATA_OPTS) -p myapp pkgdatain.txt
clean:
	-rm -rf tmp myapp*

check:
	@echo nothing to check