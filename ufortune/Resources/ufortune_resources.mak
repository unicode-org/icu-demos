# Copyright (c) 2001-2007 IBM, Inc. and others
#
#  fortune_resources.mak
#
#      Windows nmake makefile for compiling and packaging the resources
#      for for the ICU sample program "ufortune".
#
#      This makefile is normally invoked by the pre-link step in the
#      MSVC project file for ufortune


#
#  List of txt files to be built.
#    When adding a resource source (.txt) file for a new locale, the corresponding
#    .res file must be added to this list, AND to the file res-file-list.txt
#
TXTFILES= te.txt de.txt es.txt

#
#  List of resource files to be built.
#    When adding a resource source (.txt) file for a new locale, the corresponding
#    .res file must be added to this list, AND to the file res-file-list.txt
#
RESFILES= root.res te.res de.res es.res 

#
#  ICUDIR   the location of ICU, used to locate the tools for
#           compiling and packaging resources.
#
ICUDIR=$(ICU_ROOT)

#
# CLASSPATH path to jar file containing XLIFF2ICUConverter
#
CLASSPATH=$(XLIFF_JAR_DIR)\xliff.jar

#
# JAVA path to java virtual mahine
#
JAVA=$(JAVA_HOME)\bin\java.exe

#
#
#
XLIFFCONV=com.ibm.icu.dev.tool.localeconverter.XLIFF2ICUConverter

DEBUGDIR=./debug

#
#  File name extensions for inference rule matching.
#    clear out the built-in ones (for .c and the like), and add
#    the definition for .txt to .res.
#
.SUFFIXES : .xlf .txt


#
#  Inference rule, for compiling a .xlf file into a .txt file.
#
.xlf.txt:
	$(JAVA) -cp $(CLASSPATH) $(XLIFFCONV) -d . -t $* ufortune_$*.xlf


#
#  Inference rule, for compiling a .txt file into a .res file.
#  -t fools make into thinking there are files such as es.res, etc
#
.txt.res:
	$(ICUDIR)\bin\genrb  -d . $*.txt


#
#  all - nmake starts here by default
#
all: ufortune_resources.dll

ROOT:
	@echo Creating root.txt
	$(JAVA) -cp $(CLASSPATH) $(XLIFFCONV) -d . -c root -r root.xlf

ufortune_resources.dll: ROOT $(TXTFILES) $(RESFILES)
	$(ICUDIR)\bin\pkgdata --name ufortuneRes -v --mode dll -d . res-list.txt


