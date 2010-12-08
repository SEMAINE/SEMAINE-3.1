:: Stop all SEMAINE components in one script

:: Locations of installed software:
set SEMAINEDIR=%~dp0%

taskkill /f /im Active_ActionSelection.exe
taskkill /f /im Active_BehaviorPlanner.exe
taskkill /f /im Active_BehaviorRealizer.exe
taskkill /f /im Active_ListenerIntentPlanner.exe
taskkill /f /im Active_Player.exe


taskkill /f /im SEMAINExtract.exe

taskkill /f /im VideoFeatureExtractorConsole.exe


FOR /F "tokens=2" %%n IN ('TASKLIST /NH /FI "WINDOWTITLE eq speech2face - bin\semaine-speech2face.bat"' ) DO SET PID=%%n
taskkill /PID %PID%

