@echo off
set BINDIR=%~dp0%

set JARDIR=%BINDIR%..\java\lib

if not exist %JARDIR%\semaine.jar (
  echo.No semaine.jar -- you need to do 'ant jars' in the java folder first!
  goto end
)

set CONFIG=%1%
if %CONFIG%a==a (
  set CONFDIR=%BINDIR%..\java\config
  set CONFIG=%CONFDIR%\message-logger-only.config
)

set JMS_URL_SETTING=""
if %CMS_URL%a==a (
  echo.Connecting to JMS server at %CMS_URL%
  set JMS_URL_SETTING="-Djms.url=%CMS_URL%"
fi


echo.Starting SEMAINE system as: 'java %JMS_URL_SETTING% -jar %JARDIR%\semaine.jar %CONFIG%'

java %JMS_URL_SETTING% -jar %JARDIR%\semaine.jar %CONFIG%

:: goto target:

:end
