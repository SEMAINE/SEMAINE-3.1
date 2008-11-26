
AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS=asrtum

asrtum_SOURCES=\
	src/asrtum.cpp \
	src/ASR.cpp \
	src/APipeSource.cpp 
asrtum_CPPFLAGS = $(SEMAINEAPI_CPPFLAGS) $(ATKLIB_CPPFLAGS)
asrtum_LDADD = $(SEMAINEAPI_LDFLAGS) $(ATKLIB_LDFLAGS) -lm


