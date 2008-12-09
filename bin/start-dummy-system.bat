@echo off
set BINDIR=%~dp0%

set JARDIR=%BINDIR%..\java\lib
echo.%JARDIR%

if not exist %JARDIR%\semaine.jar (
  echo.No semaine.jar -- you need to do 'ant jars' in the java folder first!
  goto end
)

set CONFIG=%1%
if %CONFIG%a==a (
  set CONFDIR=%BINDIR%..\java\config
  set CONFIG=%CONFDIR%\dummy-system.config
)

echo.Starting SEMAINE ComponentRunner as: 'java -jar %JARDIR%\semaine.jar %CONFIG%'

java -jar %JARDIR%\semaine.jar %CONFIG%

:: goto target:

:end
