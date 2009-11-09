:: Start all Greta components with one script

:: Update this to match the location of your GRETA installation, if needed:
set GRETADIR="..\GretaActive"
start /MIN .\%GRETADIR%\bin\Active_ActionSelectionDebug.exe
%GRETADIR%\bin\utils\WAIT.exe 1

start /MIN .\%GRETADIR%\bin\Active_BehaviorPlannerDebug.exe
%GRETADIR%\bin\utils\WAIT.exe 1

start /MIN .\%GRETADIR%\bin\Active_BehaviorRealizerDebug.exe
%GRETADIR%\bin\utils\WAIT.exe 1

start /MIN .\%GRETADIR%\bin\Active_ListenerIntentPlannerDebug.exe
%GRETADIR%\bin\utils\WAIT.exe 1

start .\%GRETADIR%\bin\Active_PlayerDebug.exe
%GRETADIR%\bin\utils\WAIT.exe 1
