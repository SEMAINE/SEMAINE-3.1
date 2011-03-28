AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = hello
#bin_PROGRAMS += mirror
#bin_PROGRAMS += swimmer

MY_CPPFLAGS = -Isrc $(SEMAINEAPI_CPPFLAGS)
MY_LIBS = $(SEMAINEAPI_LDFLAGS)

hello_SOURCES = src/hellomain.cpp src/helloinput.cpp src/helloanalyser.cpp src/emoticonoutput.cpp
hello_CPPFLAGS = $(MY_CPPFLAGS)
hello_LDADD = $(MY_LIBS)

