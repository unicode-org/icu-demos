# (c) 1999 IBM, Inc.
# <srl@monkey.sbay.org>

# this make file is responsible for creating {lib|client}/Makefile


# Copied from 'source/extra'..

## Shell to use
SHELL = /bin/sh

## Install directory information
srcdir = .
top_srcdir = ../../source
prefix = /usr/local
exec_prefix = ${prefix}

bindir = ${exec_prefix}/bin
sbindir = ${exec_prefix}/sbin
datadir = ${prefix}/share
libdir = ${exec_prefix}/lib
includedir = ${prefix}/include

pkgdatadir = $(datadir)/icu
pkglibdir = $(libdir)/icu
pkgincludedir = $(includedir)/icu

## Build directory information
top_builddir = ..
exdir= example/usort
subdir= ../$(exdir)

## Install program information
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs

INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_SCRIPT = ${INSTALL_PROGRAM}

## Package information
PACKAGE = icu
VERSION = 1.3

## Files to remove for 'make clean'
CLEANFILES = *~

# must have a trailing slash.. [will be removed by another part of the script]
SUBDIRS = lib/ client/

## List of phony targets
.PHONY : all all-local all-recursive install install-local		\
install-recursive clean clean-local clean-recursive distclean		\
distclean-local distclean-recursive dist dist-recursive dist-local	\
check check-recursive check-local

## Clear suffix list
.SUFFIXES :

## List of standard targets
all: all-recursive all-local
install: install-recursive install-local
clean: clean-recursive clean-local
distclean : distclean-recursive distclean-local
odist: dist-recursive dist-local
check: check-recursive check-local

## Subdir makefiles [srl]

SUBDIR_MAKES = $(SUBDIRS:/=/Makefile)
SUBDIR_MAKEIN = $(SUBDIRS:/=/Makefile.in)



subdirs-makefiles: $(SUBDIR_MAKES)

$(SUBDIR_MAKES): $(SUBDIR_MAKEIN)
	@list='$(SUBDIRS:/=)'; for subdir in $$list; do \
	 if test -f "$$subdir/Makefile"; then \
	   echo "Skipping extant $$subdir/Makefile"; \
	 else \
	   echo "Making (for the first time) $$subdir/Makefile"; \
	   (cd $(top_srcdir) ; CONFIG_FILES=../$(exdir)/"$$subdir"/Makefile CONFIG_HEADERS= ./config.status); \
	 fi; \
	done;

## Recursive targets
all-recursive install-recursive clean-recursive distclean-recursive dist-recursive check-recursive: subdirs-makefiles
	@dot_seen=no; \
	target=`echo $@ | sed s/-recursive//`; \
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  echo "Making $$target in $$subdir"; \
	  if test "$$subdir" = "."; then \
	    dot_seen=yes; \
	    local_target="$$target-local"; \
	  else \
	    local_target="$$target"; \
	  fi; \
	  (cd $$subdir && $(MAKE) $$local_target); \
	done; \
	if test "$$dot_seen" = "no"; then \
	  $(MAKE) "$$target-local" || exit 1; \
	fi

all-local:

install-local:

dist-local:

clean-local:
	test -z "$(CLEANFILES)" || rm -f $(CLEANFILES)

check-local:

distclean-local:
	rm -f $(SUBDIR_MAKES)

#Makefile: $(srcdir)/Makefile.in  $(top_builddir)/config.status
#	cd $(top_builddir) \
#	&& CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= $(SHELL) ./config.status


