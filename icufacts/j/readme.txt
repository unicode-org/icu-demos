# Copyright (c) 2011 IBM Corporation and Others. All Rights Reserved

** To use from the command line: (UNIX)
*  create  local.mk   with "ICU4J=/Users/srl/Downloads/icu4j.jar"   then 'make check' should run each test.

** Otherwise, you can compile as follows given some "icu4j.jar"
 javac -classpath icu4j.jar s49.java
 java -classpath icu4j.jar:. s49

** Otherwise, to use from Eclipse, import the 'j' directory as a Java project ( icu-iucsamples-j ) 
and set the ICU_UTILITIES User Class entry to contain icu4j.jar


----

Note that s58 requires the "data" directory to be in the current working path.
