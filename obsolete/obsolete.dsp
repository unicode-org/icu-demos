# Microsoft Developer Studio Project File - Name="obsolete" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=obsolete - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "obsolete.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "obsolete.mak" CFG="obsolete - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "obsolete - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "obsolete - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "obsolete - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OBSOLETE_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\icu\include" /I "..\..\icu\source\common" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OBSOLETE_EXPORTS" /D "U_OBSOLETE_IMPLEMENTATION" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 icuuc.lib icuin.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:"../bin/icuob26.pdb" /machine:I386 /out:"../bin/icuob26.dll" /implib:"..\lib\icuobs.lib" /libpath:"..\..\icu\lib\\"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "obsolete - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OBSOLETE_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\icu\include" /I "..\..\icu\source\common" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OBSOLETE_EXPORTS" /D "U_OBSOLETE_IMPLEMENTATION" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 icuucd.lib icuind.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:"../bin/icuob26d.pdb" /debug /machine:I386 /out:"../bin/icuob26d.dll" /implib:"..\lib\icuobsd.lib" /pdbtype:sept /libpath:"..\..\icu\lib\\"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "obsolete - Win32 Release"
# Name "obsolete - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\bidi.cpp
# End Source File
# Begin Source File

SOURCE=.\unicode\bidi.h

!IF  "$(CFG)" == "obsolete - Win32 Release"

# Begin Custom Build
InputPath=.\unicode\bidi.h

"..\include\unicode\bidi.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy    $(InputPath)    ..\include\unicode

# End Custom Build

!ELSEIF  "$(CFG)" == "obsolete - Win32 Debug"

# Begin Custom Build
InputPath=.\unicode\bidi.h

"..\include\unicode\bidi.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy    $(InputPath)    ..\include\unicode

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\convert.cpp
# End Source File
# Begin Source File

SOURCE=.\unicode\convert.h

!IF  "$(CFG)" == "obsolete - Win32 Release"

# Begin Custom Build
InputPath=.\unicode\convert.h

"..\include\unicode\convert.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy    $(InputPath)    ..\include\unicode

# End Custom Build

!ELSEIF  "$(CFG)" == "obsolete - Win32 Debug"

# Begin Custom Build
InputPath=.\unicode\convert.h

"..\include\unicode\convert.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy    $(InputPath)    ..\include\unicode

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\unicode\obsolete.h

!IF  "$(CFG)" == "obsolete - Win32 Release"

# Begin Custom Build
InputPath=.\unicode\obsolete.h

"..\include\unicode\obsolete.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy    $(InputPath)    ..\include\unicode

# End Custom Build

!ELSEIF  "$(CFG)" == "obsolete - Win32 Debug"

# Begin Custom Build
InputPath=.\unicode\obsolete.h

"..\include\unicode\obsolete.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy    $(InputPath)    ..\include\unicode

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\uchar_ob.c
# End Source File
# Begin Source File

SOURCE=.\unicode\uchar_ob.h
# End Source File
# Begin Source File

SOURCE=.\ucol_ob.cpp
# End Source File
# Begin Source File

SOURCE=.\unicode\ucol_ob.h
# End Source File
# Begin Source File

SOURCE=.\unicode.cpp
# End Source File
# Begin Source File

SOURCE=.\unicode\unicode.h

!IF  "$(CFG)" == "obsolete - Win32 Release"

# Begin Custom Build
InputPath=.\unicode\unicode.h

"..\include\unicode\unicode.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy    $(InputPath)    ..\include\unicode

# End Custom Build

!ELSEIF  "$(CFG)" == "obsolete - Win32 Debug"

# Begin Custom Build
InputPath=.\unicode\unicode.h

"..\include\unicode\unicode.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy    $(InputPath)    ..\include\unicode

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\unistrob.cpp
# End Source File
# Begin Source File

SOURCE=.\unicode\unistrob.h
# End Source File
# Begin Source File

SOURCE=.\unicode\unorm_ob.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
