#!/bin/sh
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
