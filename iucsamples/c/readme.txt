# Copyright (C) 2011 IBM Corporation and Others. All Rights Reserved.
#

Windows Visual Studio: 

Use the project file iucsamples.sln.  
Set  variable IcuDir  to the root of your ICU build (containing bin, lib, include)
set PATH=%IcuDir%\bin;%PATH% 


UNIXes: see below.


To locate ICU for the purpose of these samples, ensure one of the following is true:

1. ICU built, not installed:
      Set the variable "icubuild" to point to a built ICU source directory (in-source or out of source)
   Example:  add to 'local.mk' this line:    "icubuild=/home/srl/icu/source" being a directory where ICU is built.
   Example:  'export icubuild=/tmp/icu-build'  

2. ICU built, installed, pkg-config is installed:
     (PKG_CONFIG_PATH or equivalent should point to icu's .pc files).

3. ICU built, installed, icu-config is on the $PATH



To build and run all samples, just run 'make check'. To build an individual sample, run 'make check' from inside that directory.
