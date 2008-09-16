SEMAINE Architecture: C++ API
=============================

version 0.0.1, Marc Schroeder, September 2008

About
-----

This code provides the API needed for connecting C++ processing components of the SEMAINE
system to the JMS middleware.


Compiling
---------

1. Install ActiveMQ-CPP from http://activemq.apache.org/cms/

2. Configure and compile the code as follows:

	./autogen.sh
			this will generate a configure script
	./configure --with-activemqcpp=/path/to/activemq-cpp-installation

3. make


Running the demo
----------------

1. Download the Java-based ActiveMQ server and set up the Java-based dummy system as
described at http://semaine.opendfki.de/wiki/DummySystem

2. Start external processes

	export ACTIVEMQDIR=/path/to/activemq-dir
	semaine/bin/start-activemq.sh
	
	semaine/bin/start-message-logger-only.sh
	
3. Start the demo system

	export LD_LIBRARY_PATH=/path/to/activemq-dir/lib (on linux)
	src/test

