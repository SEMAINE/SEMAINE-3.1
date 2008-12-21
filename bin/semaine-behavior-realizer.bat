@echo off
set BINDIR=%~dp0%

set GRETADIR=%BINDIR%..\GretaActive

if not exist %GRETADIR% (
  echo.No Greta directory in %GRETADIR%! Cannot start.
  goto end
)


if %1%a==a (
  set EXEFILE=%GRETADIR%\bin\BehaviorRealizerDebug.exe
) else (
  set EXEFILE=%GRETADIR%\bin\%1%
)

if not exist %EXEFILE% (
  echo.No such file: %EXEFILE%. Cannot start.
  goto end
)


if not %CMS_URL%a==a (
  echo.Connecting to JMS server at %CMS_URL%
) else (
  echo.Connecting to JMS server at tcp://localhost:61616
)


%EXEFILE%

:: goto target:

:end
set /P DUMMY=Press return to continue...
