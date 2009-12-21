:: Start all Greta components with one script

:: Update this to match the location of your GRETA installation, if needed:
:: set GRETADIR="..\GretaActive"
::set GRETADIR="..\..\private-svn\GretaXerces3"
set GRETADIR="..\..\GretaSVN\XERCES"

start /MIN /D %GRETADIR%\bin\ Active_ActionSelection.exe
%GRETADIR%\bin\utils\WAIT.exe 1

start /MIN /D %GRETADIR%\bin\ Active_BehaviorPlanner.exe
%GRETADIR%\bin\utils\WAIT.exe 1

start /MIN /D %GRETADIR%\bin\ Active_BehaviorRealizer.exe
%GRETADIR%\bin\utils\WAIT.exe 1

start /MIN /D %GRETADIR%\bin\ Active_ListenerIntentPlanner.exe
%GRETADIR%\bin\utils\WAIT.exe 1

start /D %GRETADIR%\bin\ Active_Player.exe
%GRETADIR%\bin\utils\WAIT.exe 1
