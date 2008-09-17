SEMAINE Architecture: C++ API
=============================

version 0.0.1, Marc Schroeder, September 2008

About
-----

This code provides the API needed for connecting C++ processing components of the SEMAINE
system to the JMS middleware.


Compiling
---------

1. Install ActiveMQ-CPP from http://activemq.apache.org/cms/. At least version 2.2.1 is required.

Note the preconditions as listed in the README.

On Linux/Mac OS X, APR and APR-util, automake, libtool
are usually already installed or can be installed using operating system packages (apt-get on
Ubuntu, Mac Ports on the Mac).

On Windows, you do need APR and APR-util. The easiest way to get these in binary form is to
get a binary version of the Apache2 web server, e.g. from http://www.apachelounge.com/download/.
Remember where you install it -- e.g., c:\Program Files\Apache2.

Build ActiveMQ-CPP according to the instructions.

Linux / Mac:
	./autogen.sh
	./configure (add --prefix=/path/where/activemq-cpp/should/be/installed if needed)
	make
	make install

Windows:
	There are instructions in the ActiveMQ-CPP Readme on how to set environment variables.
	The project file we need is activemq-cpp-2.2.1\vs2005-build\vs2005-activemq.vcproj.
	I had to adjust include and link settings:
	- Open the project property page in Visual C++ by right-clicking vs2005-activemq and
	  selecting "Properties".
	- In the C/C++ settings, make sure that "Additional Include settings" has an entry to the
	  include files for APR; if you installed them as part of Apache2 into
	  C:\Program Files\Apache2, then the directory to list here is C:\Program Files\Apache2\include
	- Now this should compile OK and create a static library
	  ....\activemq-cpp-2.2.1\vs2005-build\Debug\libactivemq-cppd.lib.

2. Configure and compile the SEMAINE code.

Linux / Mac:
	cd semaine/c++
	./autogen.sh
			this will generate a configure script
	./configure --with-activemqcpp=/path/to/activemq-cpp-installation
	make

This will generate the executable binary src/test.


Windows:
	The project file is semaine\c++\src\semaine.vcproj. This is a Visual C++ 2008 Express Edition
	file. 
	You can get Visual C++ 2008 Express Edition for free from http://www.microsoft.com/express/vc/.
	You will need to adapt paths to your setup as follows:
	- Open the project property page by right-clicking the project "semaine" and selecting
	  "Properties".
	- C/C++ settings, the "Additional Include Directories" must contain three entries:
	  (i) the source folder of the current project (tips of how to do this differently are welcome);
	  (ii) the APR include folder, e.g. C:\Program Files\Apache2\include
	  (iii) the Location of the ActiveMQ-CPP sources, ...activemq-cpp-2.2.1\src\main
	- Linker settings, the "Additional Library Directories" must contain
	  (i) the location where libapr-1.lib and libaprutil-1.lib can be found, 
	      e.g. C:\Program Files\Apache2\lib
	  (ii) the output folder of ActiveMQ-CPP, containing libactivemq-cppd.lib,
	       e.g. ...\activemq-cpp-2.2.1\vs2005-build\Debug.

This will generate the executable binary ...\semaine\c++\src\Debug\semaine.exe. 



Running the demo
----------------

1. Download the Java-based ActiveMQ server and set up the Java-based dummy system as
described at http://semaine.opendfki.de/wiki/DummySystem

2. Start external processes

	export ACTIVEMQDIR=/path/to/activemq-dir
	semaine/bin/start-activemq.sh
	
	semaine/bin/start-message-logger-only.sh
	
3. Start the demo system

On Linux, type in a shell:

	(it may be necessary to indicate the location of libactivemq-cpp.so :)
	export LD_LIBRARY_PATH=/path/to/activemq-dir/lib
	src/test

On Windows, type at the command prompt:

	(it may be necessary to indicate the location of libapr-1.dll :)
	set PATH=%PATH%;"C:\Program Files\Apache2\bin"
    cd ...\semaine\c++\src\Debug
    semaine

The simple demo program will try to connect to a local ActiveMQ server, register
the two dummy components "Jim" and "John", and start them when an external system
manager component gives them the green light.

The following are typical outputs:

1. "Connection refused" 
   -> no activemq server found on the local machine. Make sure to start ActiveMQ.

2. Jim and John start and report ready, but nothing happens
   -> ActiveMQ is running, but the system manager is not running. Make sure to start
      the message logger and the system manager, e.g. using
      semaine/bin/start-message-logger-only.sh.

3. Jim and John start up, and start chatting with each other. Congratulations,
   you have the system up and running! Now you can start replacing Jim and John
   (two instances of a GenericTestComponent) with your real components.