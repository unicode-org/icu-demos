#!/bin/sh
## ******************************************************************************
## *
## *   Copyright (C) 1999-2000, International Business Machines
## *   Corporation and others.  All Rights Reserved.
## *
## *******************************************************************************

## this script extracts strings of the form:
##    FSWF("myTag", "Fallback text for myTag");
##
## into the root resource for the project.
##
## NOTE:  
##   - only ONE fswf per line, please.
##
##   - make sure the whole FSWF expression (in the source) is on one line
##
##   - if you do NOT want a string emitted (for example, if you use
##      emitResourceFromFile.sh for a large string) then use this in the
##      source:
##
##	     FSWF ( /* NOEXTRACT */ "htmlHEAD",
##		    "</HEAD>\r\n<BODY BGCOLOR=\"#FFFFFF\" >\r\n");
##
##      The /*NOEXTRACT*/ between the leftparenthesis and the 
##      doublequote is sufficient to break the matching string below.
##
## BUGS
##      Does not escape characters properly!
##      Does not respect commented-out FSWF's, much less #ifdefed out ones!
##      Script NEEDS to be generalized. What about multiple input files? What if you want something other than root_head and root_tail?
##
## Note: FSWF means Fetch String with Fallback.   see locexp.c
export INVOKE_UCONV
echo "// root file. Generated from $* on `date` on `hostname`"
echo "// It's probably not a good idea to change this file."
echo "// Better to change locexp.c or the ROOT.* source files and rebuild."
echo
echo
echo "root {"
echo
fgrep -h 'FSWF("' $* | sed -e 's%.*FSWF("\([^"]*\)",[ ]*"\([^"]*\)".*%   \1  {  "\2"  }%' | sort | uniq
echo 
echo "// Special Cases"
if [ -f root_head.html ]; then
    emitResourceFromFile.sh htmlHEAD iso-8859-1 root_head.html || exit 1
fi

if [ -f root_tail.html ]; then
    emitResourceFromFile.sh htmlTAIL iso-8859-1 root_tail.html || exit 1
fi
echo
if [ -f root.txt.inc ]; then
    echo
    echo " // Imported from: root.txt.inc "
    cat root.txt.inc
fi
echo
echo "}"
echo
