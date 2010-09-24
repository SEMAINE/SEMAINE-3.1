rem Which ActiveMQ to connect to:
set CMS_URL=tcp://l92.168.1.10:61616

rem Locations of installed software:
rem %~dp0 is expanded pathname of the current script
set IMPERIALDIR="C:\Program Files\iBUG\Semaine Video Components"

rem Start video components if installed
if exist %IMPERIALDIR%\VideoFeatureExtractorConsole.exe (
  echo.Starting video input components
  start /MIN /D %IMPERIALDIR% VideoFeatureExtractorConsole.exe
)
