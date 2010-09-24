@echo off
set BINDIR=%~dp0%

set JARDIR="%BINDIR%..\java\lib"

if not exist %JARDIR%\semaine.jar (
  echo.No semaine.jar -- you need to do 'ant jars' in the java folder first!
  goto end
)


java -cp %JARDIR%\semaine.jar eu.semaine.util.MessageLogInspectorGUI %1%

:: goto target:

:end
set /P DUMMY=Press return to continue...
