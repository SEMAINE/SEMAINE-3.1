README
------

This directory contains an example implementation for how to use the SEMAINE API from C++.

This version is compatible with SEMAINE API version 3.1.

It implements the Hello World example from the SEMAINE API tutorial, which also exists in Java --
see http://semaine.opendfki.de/wiki/BuildingSystems


Usage:

- you need autoconf tools on Linux or Mac
- first build the SEMAINE API, cd ../..; bin/buildall.sh
- then build examples:

cd c++/examples
./autogen.sh
./configure
make

- now start the system as follows:

(in one shell:)
cd ../..; bin/semaine-message-logger-only.sh

(in another shell:)
cd c++/examples; ./hello

(now type text into "hello": it should react to the keywords "a bit", "very", "happy" and "sad"
by showing different emoticons)
