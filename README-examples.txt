The examples package contains a number of illustrations how the SEMAINE API
can be used to build new emotion-oriented systems.


To run the example systems, proceed as follows.

* Hello

This example should run on any system with Java.

Make sure you have installed the package 
	SEMAINE-java
from http://sourceforge.net/projects/semaine
and installed it as documented at http://semaine.opendfki.de/wiki/SEMAINE-1.0.

Make sure an activemq server is running.

Then start the Hello example as follows:

cd SEMAINE-x.x.x
java -jar java/lib/semaine-examples.jar java/config/example-hello.config


* Emotion Mirror

This example requires Linux or Mac OS X.

Make sure you have installed the packages 
	SEMAINE-java
	SEMAINE-linux
from http://sourceforge.net/projects/semaine
and installed them as documented at http://semaine.opendfki.de/wiki/SEMAINE-1.0.

Make sure an activemq server is running.

Then start the Emotion mirror example as follows:

(in a first shell, on a linux or mac system)
cd SEMAINE-x.x.x
bin/run_components/start_component_tum.smile

(in a second shell)
cd SEMAINE-x.x.x
java -jar java/lib/semaine-examples.jar java/config/example-mirror.config


* Swimmer's game

This example requires Linux or Mac OS X.

Make sure you have installed the packages 
	SEMAINE-java
	SEMAINE-linux
from http://sourceforge.net/projects/semaine
and installed them as documented at http://semaine.opendfki.de/wiki/SEMAINE-1.0.

Make sure an activemq server is running.

Then start the Swimmer's game example as follows:

(in a first shell, on a linux or mac system)
cd SEMAINE-x.x.x
bin/run_components/start_component_tum.smile

(in a second shell)
cd SEMAINE-x.x.x
bin/example-swimmer.sh (on linux/mac)
bin\example-swimmer.bat (on Windows)

Note that if you run the second shell on Windows, you will need to take into account
the notes on "Running a distributed system" at 
http://semaine.opendfki.de/wiki/SEMAINE-1.0-DistributedSystem



The source code of the example components can be found below java/src/eu/semaine/examples.