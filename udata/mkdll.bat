@echo off
if "%ICU_DATA%"=="" goto :error
cl "/I..\..\icu\source\common" /GD "/Fe%ICU_DATA%icudata.dll" "%ICU_DATA%unames_dat.c" /link /DLL /NOENTRY /base:"0x4ad00000"
goto :end

:error
echo This creates icudata.dll from unames_dat.c
echo ICU_DATA must be set correctly

:end
