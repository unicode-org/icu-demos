# Microsoft Developer Studio Project File - Name="obstest" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=obstest - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "obstest.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "obstest.mak" CFG="obstest - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "obstest - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "obstest - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "obstest - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\include" /I "..\..\..\icu\include" /I "..\..\..\icu\source\common" /I "..\..\..\icu\source\tools\toolutil" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "ICU_UBIDI_USE_DEPRECATES" /D "ICU_UNICODE_CLASS_USE_DEPRECATES" /D "ICU_UNICODECONVERTER_USE_DEPRECATES" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 icuobs.lib icuuc.lib icuin.lib icudata.lib icutu.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /libpath:"..\..\lib\\" /libpath:"..\..\..\icu\lib\\"

!ELSEIF  "$(CFG)" == "obstest - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\..\icu\include" /I "..\..\..\icu\source\common" /I "..\..\..\icu\source\tools\toolutil" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "ICU_UBIDI_USE_DEPRECATES" /D "ICU_UNICODE_CLASS_USE_DEPRECATES" /D "ICU_UNICODECONVERTER_USE_DEPRECATES" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 icuobsd.lib icuucd.lib icuind.lib icudata.lib icutud.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"..\..\lib\\" /libpath:"..\..\..\icu\lib\\"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "obstest - Win32 Release"
# Name "obstest - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\biditst.cpp
# End Source File
# Begin Source File

SOURCE=.\cppcnvt.cpp
# End Source File
# Begin Source File

SOURCE=.\intltest.cpp
# End Source File
# Begin Source File

SOURCE=.\itconv.cpp
# End Source File
# Begin Source File

SOURCE=.\itmajor.cpp
# End Source File
# Begin Source File

SOURCE=.\itutil.cpp
# End Source File
# Begin Source File

SOURCE=..\testdata\README.TXT

!IF  "$(CFG)" == "obstest - Win32 Release"

# Begin Custom Build
InputPath=..\testdata\README.TXT

"..\testdata\out\testdata.dat" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy      ..\..\..\icu\source\test\testdata\out\testdata.dat      ..\testdata\out\ 

# End Custom Build

!ELSEIF  "$(CFG)" == "obstest - Win32 Debug"

# Begin Custom Build
InputPath=..\testdata\README.TXT

"..\testdata\out\testdata.dat" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy      ..\..\..\icu\source\test\testdata\out\testdata.dat      ..\testdata\out\ 

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ucdtest.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
