# Microsoft Developer Studio Project File - Name="locexp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=locexp - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "locexp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "locexp.mak" CFG="locexp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "locexp - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "locexp - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "locexp - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\icu\include" /I ".\util" /I "..\usort\lib" /I "..\..\icu\source\common" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D LXHOSTNAME=\"Win_NT\" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 icuuc.lib icuin.lib icuio.lib icusr.lib /nologo /subsystem:console /machine:I386 /libpath:"..\..\icu\lib"

!ELSEIF  "$(CFG)" == "locexp - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\icu\include" /I "..\..\icu\source\common" /I "." /I "util" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 icuucd.lib icuind.lib icuiod.lib icusrd.lib util.lib kernel32.lib user32.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"libcd.lib" /pdbtype:sept /libpath:"..\..\icu\lib"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "locexp - Win32 Release"
# Name "locexp - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\calexp.c
# End Source File
# Begin Source File

SOURCE=.\cgimain.c
# End Source File
# Begin Source File

SOURCE=.\util\decompcb.c
# End Source File
# Begin Source File

SOURCE=.\util\kangxi.c
# End Source File
# Begin Source File

SOURCE=.\locexp.c
# End Source File
# Begin Source File

SOURCE=.\util\lx_cpputils.cpp
# End Source File
# Begin Source File

SOURCE=.\util\lx_utils.c
# End Source File
# Begin Source File

SOURCE=.\srchexp.c
# End Source File
# Begin Source File

SOURCE=.\util\translitcb.c
# End Source File
# Begin Source File

SOURCE=.\util\utimzone.cpp
# End Source File
# Begin Source File

SOURCE=.\wfobject.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Build Scripts"

# PROP Default_Filter "mk;mak"
# Begin Source File

SOURCE=.\resource\makedata.mak

!IF  "$(CFG)" == "locexp - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\resource\makedata.mak
InputName=makedata

"$(ProjDir)\..\..\..\icu\source\data\locexp.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd $(ProjDir)\resource 
	nmake /f $(InputName).mak icup=$(ProjDir)\..\..\..\icu CFG=Release 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "locexp - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\resource\makedata.mak
InputName=makedata

"$(ProjDir)\..\..\icu\source\data\locexp.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd $(ProjDir)\resource 
	nmake /f $(InputName).mak icup=$(ProjDir)\..\..\..\icu CFG=Debug 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\resource\resfiles.mk
# End Source File
# End Group
# End Target
# End Project
