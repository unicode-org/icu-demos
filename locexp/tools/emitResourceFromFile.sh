#!/bin/sh
##    Copyright (C) 1999-2000, International Business Machines
##    Corporation and others.  All Rights Reserved.
if [ $# -ne 3 ]; then
  echo 1>&2 "Usage: $0 resource_tag source_file encoding"
  exit 1
fi

if [ ! -f $3 ]; then
  echo 1>&2 "$0: can't open file $3"
  exit 1
fi

echo
echo "// From $3 ($2)"
echo "   $1 {"

# should be: -n or \c
echo  "       \""

uconv -f $2 -t UTF16_BigEndian $3 | escapeForBundle

# should be: -n or \c
echo  "\""

echo "   }"
echo

