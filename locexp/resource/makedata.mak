#**********************************************************************
#* Copyright (C) 1999-2004, International Business Machines Corporation
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
RESNAME=locexp
RESDIR=.
RESFILES=resfiles.mk

DLL_OUTPUT=.

ICUTOOLS=$(ICUP)\icu\bin

# This appears in original Microsofts makefiles
!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE
NULL=nul
!ENDIF

PATH = $(ICUP)\bin;$(PATH)

# Suffixes for data files
.SUFFIXES : .ucm .cnv .dll .lib .dat .res .txt .c

# We're including a list of ucm files. There are two lists, one is essential 'ucmfiles.mk' and
# the other is optional 'ucmlocal.mk'
!IF EXISTS("$(RESFILES)")
!INCLUDE "$(RESFILES)"
!ELSE
!ERROR ERROR: cannot find "$(RESFILES)"
!ENDIF
RB_FILES = $(RESSRC:.txt=.res)

# This target should build all the data files
ALL : GODATA  root.txt "$(DLL_OUTPUT)\$(RESNAME).lib" GOBACK #$(RESNAME).dat
	@echo All targets are up to date

#invoke pkgdata
"$(DLL_OUTPUT)\$(RESNAME).lib" :  $(RB_FILES) $(RESFILES)
	@echo Building $(RESNAME)
 	@"$(ICUTOOLS)\pkgdata" -v -m static -c -p $(RESNAME) -d "$(DLL_OUTPUT)" -s "$(RESDIR)" <<pkgdatain.txt
$(RB_FILES:.res =.res
)
<<KEEP

# utility to send us to the right dir
GODATA :
#	cd "$(RESDIR)"

root.txt: root_SAMPLE.txt
	copy root_SAMPLE.txt root.txt

# utility to get us back to the right dir
GOBACK :
#	cd "$(RESDIR)\.."

# This is to remove all the data files
CLEAN :
	@cd "$(RESDIR)"
	-@erase "*.cnv"
	-@erase "*.res"
	-@erase "$(TRANS)*.res"
	-@erase "cnvalias*.*"
	-@erase "icudata.*"
	-@erase "*.obj"
	-@erase "base*.*"
	@cd "$(ICUTOOLS)"

# Inference rule for creating resource bundles
.txt.res:
	@echo Making Resource Bundle files
	"$(ICUTOOLS)\genrb" -s$(@D) -d$(@D) $(?F)



$(RESSRC) : {"$(ICUTOOLS)"}genrb.exe

