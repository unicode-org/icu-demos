@echo off
if "%ICU_DATA%"=="" goto :error

cl "/I..\..\icu\source\common" /GD /c "%ICU_DATA%unames_dat.c"
echo "/out:%ICU_DATA%icudata.dll">mkdll.tmp
echo unames_dat.obj>>mkdll.tmp
type mkdll.lk>>mkdll.tmp
link @mkdll.tmp
goto :end

:error
echo This creates icudata.dll from unames_dat.c
echo ICU_DATA must be set correctly

:end
