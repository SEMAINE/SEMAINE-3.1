
@echo off
set SEMAINE_JAVA_DIR=..\..\SEMAINE-2.0
set MARYDIR=%SEMAINE_JAVA_DIR%\MARY
if not exist %MARYDIR% (
  echo.MARY directory not found in %MARYDIR%. Cannot start.
  goto end
)
set JARDIR=%SEMAINE_JAVA_DIR%\java\lib

set BINDIR=%~dp0%
set MYJAVADIR=%BINDIR%


if %1%a==a (
  echo.No config file given! Cannot start.
  goto end
) else (
  set CONFIG=%1%
)

if not %CMS_URL%a==a (
  echo.Connecting to JMS server at %CMS_URL%
  set JMS_URL_SETTING="-Djms.url=%CMS_URL%"
)

set ARGS=-Xmx400m %JMS_URL_SETTING% -classpath "%MYJAVADIR%\bin";"%JARDIR%\semaine.jar";"%JARDIR%\semaine-mary.jar";"%JARDIR%\semaine-dialogue.jar" -Dmary.base=%MARYDIR% eu.semaine.system.ComponentRunner %CONFIG%

echo.Starting SEMAINE system as: 'java %ARGS%'

java %ARGS%

:: goto target:

:end
set /P DUMMY=Press return to continue...
