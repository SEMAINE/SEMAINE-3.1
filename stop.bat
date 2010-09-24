:: Stop all SEMAINE components in one script

:: Locations of installed software:
set SEMAINEDIR=%~dp0%
set ACTIVEMQDIR=%SEMAINEDIR%apache-activemq-5.3.0

FOR /F "tokens=2" %%n IN ('TASKLIST /NH /FI "WINDOWTITLE eq speech2face - bin\semaine-speech2face.bat"' ) DO SET PID=%%n
taskkill /PID %PID%

taskkill /f /im Active_ActionSelection.exe
taskkill /f /im Active_BehaviorPlanner.exe
taskkill /f /im Active_BehaviorRealizer.exe
taskkill /f /im Active_ListenerIntentPlanner.exe
taskkill /f /im Active_Player.exe

FOR /F "tokens=2" %%n IN ('TASKLIST /NH /FI "WINDOWTITLE eq OpenSMILE - start_openSMILE.bat"' ) DO SET PID=%%n
taskkill /PID %PID%

taskkill /f /im VideoFeatureExtractorConsole.exe

if exist %ACTIVEMQDIR% start /MIN /D %ACTIVEMQDIR%\bin activemq-admin.bat stop

