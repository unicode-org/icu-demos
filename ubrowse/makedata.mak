#**********************************************************************
#* Copyright (C) 1999-2005, International Business Machines Corporation
#* and others.  All Rights Reserved.
#**********************************************************************
# nmake file for creating data files on win32
# invoke with
# nmake /f makedata.mak icup=<path_to_icu_instalation> [Debug|Release]
#
#	12/10/1999	weiv	Created

#Let's see if user has given us a path to ICU
#This could be found according to the path to makefile, but for now it is this way
!IF "$(ICUP)"==""
!ERROR Can't find path!
!ENDIF
!MESSAGE icu path is $(ICUP)
PKG=ubrowseres
ICONS=icons
RESDIR=.
RESFILES=resfiles.mk

DLL_OUTPUT=$(CFG)

ICUTOOLS=$(ICUP)\icu\bin
!MESSAGE ICUTOOLS is $(ICUTOOLS)

PATH = $(ICUP)\icu\bin;$(PATH)

# Suffixes for data files
.SUFFIXES : .ucm .cnv .dll .lib .dat .res .txt .c

# We're including a list of ucm files. There are two lists, one is essential 'ucmfiles.mk' and
# the other is optional 'ucmlocal.mk'
#!IF EXISTS("$(RESFILES)")
#!INCLUDE "$(RESFILES)"
#!ELSE
#!ERROR ERROR: cannot find "$(RESFILES)"
#!ENDIF
RESSRC=$(PKG)\icons.txt
RB_FILES = $(RESSRC:.txt=.res)

# This target should build all the data files
ALL : GODATA  $(RB_FILES) "$(DLL_OUTPUT)\$(PKG).lib" GOBACK 
	@echo All targets are up to date

#invoke pkgdata
"$(DLL_OUTPUT)\$(PKG).lib" : $(RB_FILES)
	@echo Building $(PKG)
 	@"$(ICUTOOLS)\pkgdata" -v -m static -c -p $(PKG) -d "$(DLL_OUTPUT)" -s "$(RESDIR)" <<pkgdatain.txt
$(RB_FILES:.res =.res
)
<<KEEP

# utility to send us to the right dir
GODATA :
	@if not exist "$(PKG)\$(NULL)" mkdir "$(PKG)"
#	cd "$(RESDIR)"

# utility to get us back to the right dir
GOBACK :
#	cd "$(RESDIR)\.."

# This is to remove all the data files
CLEAN :
	@cd "$(RESDIR)"
	-@erase "*.cnv"
	-@erase "*.res"
	-@erase "*.obj"
	@cd "$(ICUTOOLS)"

$(PKG)/icons.res : $(ICONS)/*.gif
	@echo creating $*.txt
	@echo icons { > $*.txt
	@for %i in ($(**B: =.gif ).gif) do @echo %i:import { "$(ICONS)/%i" } >> $*.txt
	@echo } >> $*.txt
	"$(ICUTOOLS)\genrb" -d$(@D) $*.txt



