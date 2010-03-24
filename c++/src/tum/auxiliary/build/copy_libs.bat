
REM This bat files copies libraries required by openSMILE to the openSMILE root directory

set rootpath=..\..\..\..\..
set TP=%rootpath%\thirdparty\build
set target=..\..\opensmile
REM set target=..\..\opensmile\bin
set APRlibs=C:\Program Files\Apache Software Foundation\Apache2.2


REM Release libraries:
copy /Y %TP%\portaudio-win\portaudio_x86Release.dll %target%
copy /Y %TP%\xerces-c-src\Build\Win32\VC8\Release\xerces-c_3_0.dll %target%
copy /Y %TP%\xerces-c-src\Build\Win32\VC8\Release\xerces-c_3_1.dll %target%
copy /Y %TP%\xqilla\build\windows\VC8\Win32\Release\xqilla22.dll %target%

REM Debug libraries:
 copy /Y %TP%\portaudio-win\portaudio_x86Debug.dll %target%
 copy /Y %TP%\xerces-c-src\Build\Win32\VC8\Debug\xerces-c_3_0D.dll %target%
 copy /Y %TP%\xerces-c-src\Build\Win32\VC8\Debug\xerces-c_3_1D.dll %target%
 copy /Y %TP%\xqilla\build\windows\VC8\Win32\Debug\xqilla22d.dll %target%

REM APR runtime:
copy /Y %APRlibs%\libapr-1.dll %target%
copy /Y %APRlibs%\libapriconv-1.dll %target%
copy /Y %APRlibs%\libaprutil-1.dll %target%

