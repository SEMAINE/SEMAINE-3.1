REM @echo off
set jpath=..\..\..\..\thirdparty\build\julius-win

copy /Y win\sent\vocabulary.h %jpath%\libsent\include\sent
copy /Y win\sent\tcpip.h %jpath%\libsent\include\sent
copy /Y win\sent\adin.h %jpath%\libsent\include\sent
copy /Y win\sent\config.h %jpath%\libsent\include\sent
copy /Y win\sent\config-win.h %jpath%\libsent\include\sent
copy /Y win\sent\gzfile.c %jpath%\libsent\src\util
copy /Y win\sent\server-client.c %jpath%\libsent\src\net

copy /Y win\julius\recog.h %jpath%\libjulius\include\julius
copy /Y win\julius\jconf.h %jpath%\libjulius\include\julius
copy /Y win\julius\plugin.h %jpath%\libjulius\include\julius
copy /Y win\julius\config-win-julius.h %jpath%\libjulius\include\julius


copy /Y win\julius\version.c %jpath%\libjulius\src
copy /Y win\julius\jfunc.c %jpath%\libjulius\src
copy /Y win\julius\realtime-1stpass.c %jpath%\libjulius\src
copy /Y win\julius\recogmain.c %jpath%\libjulius\src
copy /Y win\julius\plugin.c %jpath%\libjulius\src
copy /Y win\julius\m_info.c %jpath%\libjulius\src
copy /Y win\julius\m_adin.c %jpath%\libjulius\src
copy /Y win\julius\m_jconf.c %jpath%\libjulius\src
copy /Y win\julius\wchmm_check.c %jpath%\libjulius\src

mkdir %jpath%\vs05
copy vs05 %jpath%\vs05

pause
