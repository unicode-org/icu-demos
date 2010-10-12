/*
*******************************************************************************
*
*   Copyright (C) 2010, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*/
Performance test to compare C# (.NET) and ICU4C:

Author: Michael Ow

The purpose of this is to provide a framework for testing the performance of ICU4C and C# (.NET) using the 
performance test perl scripts in ICU4C.  The ICU4C C# Wrappers are used to call ICU4C functions.

Note: Designed to be used in conjunction with the ICU4C performance test perl scripts.
Note: Some path are hard coded, ensure that they are correct.

Files:
PerformanceTestFramework.cs:
Parent class of the performance tests.  Loads the test data (if specified) and parses the options given.
Runs the specified test and prints the results in a format that ICU4C performance test perl scripts understands.

PerformanceTestMain.cs:
The starting point of the performance test.

I18n.cs:
Contains the include statements for ICU4C header files in the i18n library.

Common.cs:
Contains the include statements for ICU4C header files in the common library.

CollationPerformanceTest.cs:
Contains the actual collation performance tests for ICU4C and .NET.

NormalizationPerformanceTest.cs:
Contains the actual normalization performance tests for ICU4C and .NET.

Creating a new test:
Look at NormalizationPerformanceTest.cs or CollationPerformanceTest.cs to get an idea of
how to create a new test and which functions you need to override.

Note:
To use ICU4C functions in C# you have to import the dll.
For example to create a collator object:
        [DllImport("C:\\PerformanceTest\\icu\\bin\\icuin45.dll", EntryPoint = "ucol_open_45")]
        public unsafe static extern IntPtr ucol_open(byte* locale, ref UErrorCode status);
before actually calling ucol_open.
Make sure the version number is correct.

Running the test:
Use the perl script formats in ICU4C: source/test/perf.  Create a new directory and perl scripts to run
the actual executables.

Options:
-r "type" (e.g. collperf)
-t "time" (skipped for now)
-p "passes" (number of passes to run each test)
-L "locale" (e.g. en_US)
-f "file" (filename with the data to test)
"Test"  (The actual test name)
Note: Don't use quotes for the options.

