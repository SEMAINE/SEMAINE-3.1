rem Which ActiveMQ to connect to:
set CMS_URL=tcp://192.168.1.1:61616

rem Locations of installed software:
rem %~dp0 is expanded pathname of the current script
set SEMAINEDIR=%~dp0%
set OPENSMILEDIR="%SEMAINEDIR%Opensmile"
set IMPERIALDIR="C:\Program Files\iBUG\Semaine Video Components"


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