@echo off
set BINDIR=%~dp0%

set JARDIR=%BINDIR%..\java\lib

if not exist %JARDIR%\semaine.jar (
  echo.No semaine.jar -- you need to do 'ant jars' in the java folder first!
  goto end
)

if %1%a==a (
  set CONFIG=%BINDIR%..\java\config\dummy-system.config
) else (
  set CONFIG=%1%
)

if not %CMS_URL%a==a (
  echo.Connecting to JMS server at %CMS_URL%
  set JMS_URL_SETTING="-Djms.url=%CMS_URL%"
)


echo.Starting SEMAINE system as: 'java %JMS_URL_SETTING% -jar %JARDIR%\semaine.jar %CONFIG%'

java %JMS_URL_SETTING% -jar %JARDIR%\semaine.jar %CONFIG%

:: goto target:

:end
set /P DUMMY=Press return to continue...
