rem Which ActiveMQ to connect to:
set CMS_URL=tcp://192.168.1.10:61616

rem Locations of installed software:
rem %~dp0 is expanded pathname of the current script
set SEMAINEDIR=%~dp0%
set GRETADIR="%SEMAINEDIR%Greta"

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
