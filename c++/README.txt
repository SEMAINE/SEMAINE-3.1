SEMAINE Architecture: C++ API
=============================

version 0.0.1, Marc Schroeder, September 2008

About
-----

This code provides the API needed for connecting C++ processing components of the SEMAINE
system to the JMS middleware.


-----------------------------------
Dependencies on installed libraries
-----------------------------------

The following third-party libraries are required.

1. ActiveMQ-CPP
===============

This is the C++ client for the Java Messaging Service server ActiveMQ.

Download ActiveMQ-CPP from http://activemq.apache.org/cms/. At least version 2.2.1 is required.

Short build instructions:

Linux / Mac:
------------

Preconditions: On Linux/Mac OS X, APR and APR-util, automake, libtool
are usually already installed or can be installed using operating system packages (apt-get on
Ubuntu, Mac Ports on the Mac).

Building:
	./autogen.sh
	./configure (add --prefix=/path/where/activemq-cpp/should/be/installed if needed)
	make
	make install


Windows:
--------

Preconditions:
- APR and APR-util. The easiest way to get these in binary form is to
get a binary version of the Apache2 web server, e.g. from http://httpd.apache.org/download.cgi.
In the installer, select "Custom install", and make sure "Build headers and libraries" is installed.
Remember where you install it -- e.g., c:\Program Files\Apache2.
- Microsoft Platform SDK. Get it from
http://www.microsoft.com/downloads/details.aspx?FamilyId=A55B6B43-E24F-4EA3-A93E-40C0EC4F68E5&displaylang=en
Again, remember where it gets installed -- e.g., C:\Program Files\Microsoft SDKs\Windows\v6.1

Building using Visual C++ 2005 Express or Visual C++ 2008 Express:
The project file we need is activemq-cpp-2.2.1\vs2005-build\vs2005-activemq.vcproj.
I had to adjust include and link settings:
	- Open the project property page in Visual C++ by right-clicking vs2005-activemq and
	  selecting "Properties".
	- In the C/C++ settings, make sure that "Additional Include Directories" has an entry to the
	  include files for APR; if you installed them as part of Apache2 into
	  C:\Program Files\Apache2, then the directory to list here is
	  C:\Program Files\Apache2\include
	- At the same place ("Additional Include Directories"), list the Platform SDK Include directory, e.g.
	  C:\Program Files\Microsoft SDKs\Windows\v6.1\Include
	- Now this should compile OK and create a static library
	  ....\activemq-cpp-2.2.1\vs2005-build\Debug\libactivemq-cppd.lib.
	- You can test that lib by trying to compile vs2005-activemq-examples.vcproj.
          Set the "Additional Include Directories" to the same value as for vs2005-activemq.vcproj,
          and the Linker->"Additional Library Directories" to, e.g.,
	  "C:\Program Files\Apache2\include";"C:\Program Files\Microsoft SDKs\Windows\v6.1\Include"
	  When you build that, it should create a file 
	  ....\activemq-cpp-2.2.1\vs2005-build\Debug\vs2005-activemq-example.exe.

2. Xerces-C
===========

This is a standards-compliant XML parser.

Install Xerces-C from http://xerces.apache.org/xerces-c/.
Binary releases exist for very many platforms.
For windows, you must use xerces-c_2_8_0-x86-windows-vc_8_0.zip.
Unzip into a folder of your choice -- we will refer to this folder as
.../xerces/ below.


-----------------------------
Compiling the SEMAINE C++ API
-----------------------------

Configure and compile the SEMAINE code as follows.

Linux / Mac:
	cd semaine/c++
	./autogen.sh
			this will generate a configure script
	./configure --with-activemqcpp=/path/to/activemq-cpp-installation --with-xerces=/path/to/xerces-folder
	make

This will generate the executable binary src/test.


Windows:
	The project file is semaine\c++\src\vs2005-semaine.vcproj. This is a Visual C++ 2005 Express Edition
	file. 
	(You can get Visual C++ 2005 Express Edition for free from http://www.microsoft.com/express/2005/)
	You will need to adapt paths to your setup as follows:
	- Open the project property page by right-clicking the project "semaine" and selecting
	  "Properties".
	- C/C++ settings, the "Additional Include Directories" must contain the following entries:
	  (i) the source folder of the current project, as "."
	  (ii) the APR include folder, e.g. C:\Program Files\Apache2\include
	  (iii) the location of the ActiveMQ-CPP sources, ...activemq-cpp-2.2.1\src\main
	  (iv) the Xerces include folder, ...\xerces\include
	  (v) the Microsoft Platform SDK include folder, e.g. 
	      C:\Program Files\Microsoft SDKs\Windows\v6.1\Include
	- Linker settings, the "Additional Library Directories" must contain
	  (i) the location where libapr-1.lib and libaprutil-1.lib can be found, 
	      e.g. C:\Program Files\Apache2\lib
	  (ii) the output folder of ActiveMQ-CPP, containing libactivemq-cppd.lib,
	       e.g. ...\activemq-cpp-2.2.1\vs2005-build\Debug
	  (iii) the Xerces lib folder, ...\xerces\lib
	  (iv) the Microsoft Platform SDK lib folder, e.g.
	       C:\Program Files\Microsoft SDKs\Windows\v6.1\Lib

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

	(it may be necessary to indicate the location of libapr-1.dll and
	 of xerces-c_2_8D.dll :)
	set PATH=%PATH%;"C:\Program Files\Apache2\bin";"...\xerces\bin"
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
      semaine/bin/start-message-logger-only.sh or simply

	cd semaine\trunk\java
	java -cp lib\semaine.jar eu.semaine.system.ComponentRunner config\message-logger-only.config
      

3. Jim and John start up, and start chatting with each other. Congratulations,
   you have the system up and running! Now you can start replacing Jim and John
   (two instances of a GenericTestComponent) with your real components.