# requires icu-config in the path

include ../config.mk

all:
	-mkdir myapp
	genrb -d myapp root.txt
	genrb -d myapp es.txt
	-mkdir tmp
	$(shell which pkgdata) -v -T tmp -p myapp pkgdatain.txt

clean:
	-rm -rf tmp myapp*