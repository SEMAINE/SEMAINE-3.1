
cd ..\c++\src\tum\opensmile
echo "COMPONENT START SCRIPT: starting semaine c++ component "tum.opensmile""
REM if [ -n "$CMS_URL" ] ; then 
set CMS_URL=tcp://localhost:61616
echo "Connecting to ActiveMQ server at %CMS_URL%"
REM else echo "Connecting to ActiveMQ server at tcp://localhost:61616"
REM fi
REM USE THE FOLLOWING LINE TO SEE A LIST OF AVAILABLE AUDIO DEVICES:
REM SEMAINExtract -C ..\auxiliary\opensmileSemaine.conf -listdevices 1
SEMAINExtract -C ..\auxiliary\conf\opensmileSemaine3a.conf -l 4
echo "COMPONENT START SCRIPT: component "tum.opensmile" exited"
pause

