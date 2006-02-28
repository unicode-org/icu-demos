Copyright (C) 2006, International Business Machines Corporation and others.  All Rights Reserved.


This tool can create C# wrappers around ICU's C API.

This tool requires Cygwin's preprocessor tool (cpp) and Visual C# .NET.

Here is an example of running the tool:

bin\Debug\genicuwrapper.exe --classname Common --include ..\..\..\icu\source\common\unicode --suffix _3_5 --libname icuuc35.dll > Common.cs