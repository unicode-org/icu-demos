@echo off
if "%ICU_DATA%"=="" goto :error

rem toolversion: Debug or Release
set toolversion=Release
if not "%1"=="" set toolversion=%1

echo %ICU_DATA%unames.dat>mkmap.tmp
%toolversion%\gencmn 1000000 mkmap.tmp

goto :end

:error
echo This creates icudata.dat from unames.dat
echo ICU_DATA must be set correctly
echo an optional parameter can be Debug or Release to specify the tools versions

:end
