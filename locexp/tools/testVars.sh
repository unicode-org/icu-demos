#!/bin/sh
## ******************************************************************************
## *
## *   Copyright (C) 1999-2000, International Business Machines
## *   Corporation and others.  All Rights Reserved.
## *
## *******************************************************************************
#
# Useful for testing locale explorer. 
# Usage:
# 
#  . ./tools/testVars.sh
#  ./locexp                  #(HTML printed to stdout)
#             
#
export QUERY_STRING="_=fr_FR_EURO"
export SCRIPT_NAME=/cgi-bin/locexp
export SERVER_NAME=www.icu-is-cool.com
export PATH_INFO=/


