rem Start all installed SEMAINE-3.1 components with one script
@echo off

rem Which ActiveMQ to connect to:

set CMS_URL=tcp://localhost:61616
echo.Components will contact ActiveMQ server at %CMS_URL%

rem Locations of installed software:
rem %~dp0 is expanded pathname of the current script
set SEMAINEDIR=%~dp0%
set JAVADIR="%SEMAINEDIR%java"
set GRETADIR="%SEMAINEDIR%Greta"
set OPENSMILEDIR="%SEMAINEDIR%Opensmile"
set IMPERIALDIR="C:\Program Files\iBUG\Semaine Video Components"

rem Standalone ACTIVEMQ: start and wait for it to be started
rem set ACTIVEMQDIR="%SEMAINEDIR%apache-activemq-5.3.0"
rem if exist %ACTIVEMQDIR% (
rem  echo.Trying to start ActiveMQ from %ACTIVEMQDIR%...
rem  set SUNJMX=-Dcom.sun.management.jmxremote.port=1099 -Dcom.sun.management.jmxremote.authenticate=false -Dcom.sun.management.jmxremote.ssl=false
rem  start "activemq" /MIN /D %ACTIVEMQDIR%\bin activemq-admin.bat start
rem  :loop
rem  %GRETADIR%\utils\WAIT.exe 1
rem  for /f "delims=" %%a in ('netstat -an ^| findstr "61616"') do @set ACTIVEMQ_READY=%%a 
rem  if "%ACTIVEMQ_READY%"=="" (
rem    echo.Waiting for ActiveMQ...
rem    goto loop
rem  ) else (
rem    echo.ActiveMQ seems to be ready.
rem  )
rem )

rem Java components (now with embedded ActiveMQ)
if exist %JAVADIR% (
  echo.Starting Java components
  start "speech2face" /MIN /D "%SEMAINEDIR%" bin\semaine-speech2face.bat
  :loop
  %GRETADIR%\utils\WAIT.exe 1
  for /f "delims=" %%a in ('netstat -an ^| findstr "61616"') do @set ACTIVEMQ_READY=%%a 
  if "%ACTIVEMQ_READY%"=="" (
    echo.Waiting for ActiveMQ...
    goto loop
  ) else (
    echo.ActiveMQ seems to be ready.
  )
)

rem GRETA components
if exist %GRETADIR% (
  echo.Starting Greta components
  start /MIN /D %GRETADIR% Active_ActionSelection.exe
  start /MIN /D %GRETADIR% Active_BehaviorPlanner.exe
  start /MIN /D %GRETADIR% Active_BehaviorPlanner.exe -p
  start /MIN /D %GRETADIR% Active_BehaviorRealizer.exe
  start /MIN /D %GRETADIR% Active_BehaviorRealizer.exe -p
  start /MIN /D %GRETADIR% Active_ListenerIntentPlanner.exe
  start /D %GRETADIR% Active_Player.exe
)

rem Start audio components if installed
if exist %OPENSMILEDIR%\start_openSMILE.bat (
  echo.Starting Opensmile audio input components
  start "OpenSMILE" /MIN /D %OPENSMILEDIR% start_openSMILE.bat
)

rem Start video components if installed
if exist %IMPERIALDIR%\VideoFeatureExtractorConsole.exe (
  echo.Starting video input components
  start /MIN /D %IMPERIALDIR% VideoFeatureExtractorConsole.exe
)

rem pause
