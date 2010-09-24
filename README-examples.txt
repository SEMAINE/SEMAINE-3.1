The examples package contains a number of illustrations how the SEMAINE API
can be used to build new emotion-oriented systems.


To run the example systems, proceed as follows.

* Hello

This example should run on any system with Java.

Make sure an activemq server is running.

Then start the Hello example as follows:

cd SEMAINE-x.x.x
java -jar java/lib/semaine-examples.jar java/config/example-hello.config


* Emotion Mirror

This example requires the native component OpenSMILE.

Make sure an activemq server is running.

Then start the Emotion mirror example as follows:

cd SEMAINE-x.x.x
bin/run_components/start_component_tum.opensmile (on linux/mac)
or
Opensmile\start_openSMILE.bat (on windows)

(in a second shell)
cd SEMAINE-x.x.x
java -jar java/lib/semaine-examples.jar java/config/example-mirror.config


* Swimmer's game

Make sure an activemq server is running.

Then start the Swimmer's game example as follows:

(in a first shell)
cd SEMAINE-x.x.x
bin/run_components/start_component_tum.opensmile (on linux/mac)
or
Opensmile\start_openSMILE.bat (on windows)

(in a second shell)
cd SEMAINE-x.x.x
bin/example-swimmer.sh (on linux/mac)
bin\example-swimmer.bat (on Windows)


The source code of the example components can be found below java/src/eu/semaine/examples.
