# Microsoft Developer Studio Project File - Name="util" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=util - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "util.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "util.mak" CFG="util - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "util - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "util - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "util - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UTIL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\..\icu\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UTIL_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 icuuc.lib icui18n.lib /nologo /dll /machine:I386 /libpath:"..\..\..\icu\lib\release"

!ELSEIF  "$(CFG)" == "util - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "util___Win32_Debug"
# PROP BASE Intermediate_Dir "util___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\icu\lib\debug\"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UTIL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /ML /W3 /Gm /GX /ZI /Od /I "..\..\..\icu\include" /I "..\..\..\icu\source\common" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UTIL_EXPORTS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 icuuc.lib icui18n.lib /nologo /dll /debug /machine:I386 /out:"..\..\..\icu\bin\Debug/util.dll" /pdbtype:sept /libpath:"..\..\..\icu\lib\debug"

!ENDIF 

# Begin Target

# Name "util - Win32 Release"
# Name "util - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\collectcb.c
# End Source File
# Begin Source File

SOURCE=.\decompcb.c
# End Source File
# Begin Source File

SOURCE=.\devanagari.cpp
# End Source File
# Begin Source File

SOURCE=.\fontedcb.c
# End Source File
# Begin Source File

SOURCE=.\inscript.c
# End Source File
# Begin Source File

SOURCE=.\kangxi.c
# End Source File
# Begin Source File

SOURCE=.\lx_utils.c
# End Source File
# Begin Source File

SOURCE=.\syriac.cpp
# End Source File
# Begin Source File

SOURCE=.\syrinput.c
# End Source File
# Begin Source File

SOURCE=.\translitcb.c
# End Source File
# Begin Source File

SOURCE=.\ures_additions.cpp
# End Source File
# Begin Source File

SOURCE=.\utimzone.cpp
# End Source File
# Begin Source File

SOURCE=.\utrnslit.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\unicode\collectcb.h

!IF  "$(CFG)" == "util - Win32 Release"

# Begin Custom Build
InputPath=.\unicode\collectcb.h
InputName=collectcb

"..\..\..\icu\include\unicode\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy  unicode\$(InputName).h  ..\..\..\icu\include\unicode\ 

# End Custom Build

!ELSEIF  "$(CFG)" == "util - Win32 Debug"

# Begin Custom Build
InputPath=.\unicode\collectcb.h
InputName=collectcb

"..\..\..\icu\include\unicode\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy  unicode\$(InputName).h  ..\..\..\icu\include\unicode\ 

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\unicode\decompcb.h

!IF  "$(CFG)" == "util - Win32 Release"

# Begin Custom Build
InputPath=.\unicode\decompcb.h
InputName=decompcb

"..\..\..\icu\include\unicode\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy  unicode\$(InputName).h  ..\..\..\icu\include\unicode\ 

# End Custom Build

!ELSEIF  "$(CFG)" == "util - Win32 Debug"

# Begin Custom Build
InputPath=.\unicode\decompcb.h
InputName=decompcb

"..\..\..\icu\include\unicode\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy  unicode\$(InputName).h  ..\..\..\icu\include\unicode\ 

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\unicode\devtables.h

!IF  "$(CFG)" == "util - Win32 Release"

# Begin Custom Build
InputPath=.\unicode\devtables.h
InputName=devtables

"..\..\..\icu\include\unicode\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy  unicode\$(InputName).h  ..\..\..\icu\include\unicode\ 

# End Custom Build

!ELSEIF  "$(CFG)" == "util - Win32 Debug"

# Begin Custom Build
InputPath=.\unicode\devtables.h
InputName=devtables

"..\..\..\icu\include\unicode\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy  unicode\$(InputName).h  ..\..\..\icu\include\unicode\ 

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\unicode\fonted_imp.h

!IF  "$(CFG)" == "util - Win32 Release"

# Begin Custom Build
InputPath=.\unicode\fonted_imp.h
InputName=fonted_imp

"..\..\..\icu\include\unicode\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy  unicode\$(InputName).h  ..\..\..\icu\include\unicode\ 

# End Custom Build

!ELSEIF  "$(CFG)" == "util - Win32 Debug"

# Begin Custom Build
InputPath=.\unicode\fonted_imp.h
InputName=fonted_imp

"..\..\..\icu\include\unicode\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy  unicode\$(InputName).h  ..\..\..\icu\include\unicode\ 

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\unicode\fontedcb.h

!IF  "$(CFG)" == "util - Win32 Release"

# Begin Custom Build
InputPath=.\unicode\fontedcb.h
InputName=fontedcb

"..\..\..\icu\include\unicode\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy  unicode\$(InputName).h  ..\..\..\icu\include\unicode\ 

# End Custom Build

!ELSEIF  "$(CFG)" == "util - Win32 Debug"

# Begin Custom Build
InputPath=.\unicode\fontedcb.h
InputName=fontedcb

"..\..\..\icu\include\unicode\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy  unicode\$(InputName).h  ..\..\..\icu\include\unicode\ 

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\unicode\kangxi.h

!IF  "$(CFG)" == "util - Win32 Release"

# Begin Custom Build
InputPath=.\unicode\kangxi.h
InputName=kangxi

"..\..\..\icu\include\unicode\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy  unicode\$(InputName).h  ..\..\..\icu\include\unicode\ 

# End Custom Build

!ELSEIF  "$(CFG)" == "util - Win32 Debug"

# Begin Custom Build
InputPath=.\unicode\kangxi.h
InputName=kangxi

"..\..\..\icu\include\unicode\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy  unicode\$(InputName).h  ..\..\..\icu\include\unicode\ 

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\unicode\lx_utils.h

!IF  "$(CFG)" == "util - Win32 Release"

# Begin Custom Build
InputPath=.\unicode\lx_utils.h
InputName=lx_utils

"..\..\..\icu\include\unicode\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy  unicode\$(InputName).h  ..\..\..\icu\include\unicode\ 

# End Custom Build

!ELSEIF  "$(CFG)" == "util - Win32 Debug"

# Begin Custom Build
InputPath=.\unicode\lx_utils.h
InputName=lx_utils

"..\..\..\icu\include\unicode\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy  unicode\$(InputName).h  ..\..\..\icu\include\unicode\ 

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\unicode\syrinput.h

!IF  "$(CFG)" == "util - Win32 Release"

# Begin Custom Build
InputPath=.\unicode\syrinput.h
InputName=syrinput

"..\..\..\icu\include\unicode\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy  unicode\$(InputName).h  ..\..\..\icu\include\unicode\ 

# End Custom Build

!ELSEIF  "$(CFG)" == "util - Win32 Debug"

# Begin Custom Build
InputPath=.\unicode\syrinput.h
InputName=syrinput

"..\..\..\icu\include\unicode\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy  unicode\$(InputName).h  ..\..\..\icu\include\unicode\ 

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\unicode\syrtables.h

!IF  "$(CFG)" == "util - Win32 Release"

# Begin Custom Build
InputPath=.\unicode\syrtables.h
InputName=syrtables

"..\..\..\icu\include\unicode\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy  unicode\$(InputName).h  ..\..\..\icu\include\unicode\ 

# End Custom Build

!ELSEIF  "$(CFG)" == "util - Win32 Debug"

# Begin Custom Build
InputPath=.\unicode\syrtables.h
InputName=syrtables

"..\..\..\icu\include\unicode\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy  unicode\$(InputName).h  ..\..\..\icu\include\unicode\ 

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\unicode\translitcb.h

!IF  "$(CFG)" == "util - Win32 Release"

# Begin Custom Build
InputPath=.\unicode\translitcb.h
InputName=translitcb

"..\..\..\icu\include\unicode\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy  unicode\$(InputName).h  ..\..\..\icu\include\unicode\ 

# End Custom Build

!ELSEIF  "$(CFG)" == "util - Win32 Debug"

# Begin Custom Build
InputPath=.\unicode\translitcb.h
InputName=translitcb

"..\..\..\icu\include\unicode\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy  unicode\$(InputName).h  ..\..\..\icu\include\unicode\ 

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\unicode\ures_additions.h

!IF  "$(CFG)" == "util - Win32 Release"

# Begin Custom Build
InputPath=.\unicode\ures_additions.h
InputName=ures_additions

"..\..\..\icu\include\unicode\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy  unicode\$(InputName).h  ..\..\..\icu\include\unicode\ 

# End Custom Build

!ELSEIF  "$(CFG)" == "util - Win32 Debug"

# Begin Custom Build
InputPath=.\unicode\ures_additions.h
InputName=ures_additions

"..\..\..\icu\include\unicode\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy  unicode\$(InputName).h  ..\..\..\icu\include\unicode\ 

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\unicode\utimzone.h

!IF  "$(CFG)" == "util - Win32 Release"

# Begin Custom Build
InputPath=.\unicode\utimzone.h
InputName=utimzone

"..\..\..\icu\include\unicode\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy  unicode\$(InputName).h  ..\..\..\icu\include\unicode\ 

# End Custom Build

!ELSEIF  "$(CFG)" == "util - Win32 Debug"

# Begin Custom Build
InputPath=.\unicode\utimzone.h
InputName=utimzone

"..\..\..\icu\include\unicode\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy  unicode\$(InputName).h  ..\..\..\icu\include\unicode\ 

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\unicode\utrnslit.h

!IF  "$(CFG)" == "util - Win32 Release"

# Begin Custom Build
InputPath=.\unicode\utrnslit.h
InputName=utrnslit

"..\..\..\icu\include\unicode\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy  unicode\$(InputName).h  ..\..\..\icu\include\unicode\ 

# End Custom Build

!ELSEIF  "$(CFG)" == "util - Win32 Debug"

# Begin Custom Build
InputPath=.\unicode\utrnslit.h
InputName=utrnslit

"..\..\..\icu\include\unicode\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy  unicode\$(InputName).h  ..\..\..\icu\include\unicode\ 

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
