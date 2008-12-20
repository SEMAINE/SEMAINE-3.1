@echo off
set BINDIR=%~dp0%

set JARDIR=%BINDIR%..\java\lib

if not exist %JARDIR%\semaine.jar (
  echo.No semaine.jar -- you need to do 'ant jars' in the java folder first!
  goto end
)

set JMS_URL_SETTING=""
if %CMS_URL%a==a (
  echo.Connecting to JMS server at %CMS_URL%
  set JMS_URL_SETTING="-Djms.url=%CMS_URL%"
fi


echo.Starting SEMAINE LogReader as: 'java %JMS_URL_SETTING% -cp %JARDIR%\semaine.jar eu.semaine.jms.JMSLogReader %1% %2%'

java %JMS_URL_SETTING% -cp %JARDIR%\semaine.jar eu.semaine.jms.JMSLogReader %1% %2%

:: goto target:

:end
