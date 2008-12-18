
AUTOMAKE_OPTIONS = subdir-objects
bin_PROGRAMS = 
  
if BUILD_SMILExtract
  bin_PROGRAMS += SMILExtract

  SMILExtract_SOURCES=\
	src/SMILExtract.cpp \
	src/audio-stream.c \
	src/wave-input.c \
	src/wave-output.c \
	src/extractors-globalconfig.c \
	src/live-input.c \
	src/LLDs.c \
	src/LLDex.c \
	src/input-framer.c \
	src/functionals.c \
	src/deltas.c \
	src/LLDfft.c \
	src/LLDmfcc.c \
	src/LLDmfccz.c \
	src/LLDspectral.c \
	src/LLDpitch.c \
	src/LLDcepstrum.c \
	src/LLDcepstralF0.c \
	src/LLDvq.c \
	src/LLDtime.c \
	src/LLDenergy.c \
	src/LLDlpc.c \
	src/CSVoutput.c \
	src/GENoutput.c \
	src/feature-memory.c \
	src/featum_util.c \
	src/LSVMoutput.c \
	src/ARFFoutput.c \
	src/SVMpredict.cpp \
	src/silence-detector.cpp \
	src/HTKoutput.c \
	src/pcm-process.c \
	src/pcm-data.c \
	src/option-parser.c \
	src/fftsg.c \
	svm/svm.cpp
  SMILExtract_CPPFLAGS = $(PORTAUDIO_CPPFLAGS)
  SMILExtract_LDADD = $(PORTAUDIO_LDFLAGS) -lm
endif 

if BUILD_SEMAINExtract
  bin_PROGRAMS += SEMAINExtract

  SEMAINExtract_SOURCES = \
	src/SEMAINExtract.cpp \
        src/audio-stream.c \
        src/wave-input.c \
        src/wave-output.c \
        src/extractors-globalconfig.c \
        src/live-input.c \
        src/LLDs.c \
        src/LLDex.c \
        src/input-framer.c \
        src/functionals.c \
        src/deltas.c \
        src/LLDfft.c \
        src/LLDmfcc.c \
        src/LLDmfccz.c \
        src/LLDspectral.c \
        src/LLDpitch.c \
        src/LLDcepstrum.c \
        src/LLDcepstralF0.c \
        src/LLDvq.c \
        src/LLDtime.c \
        src/LLDenergy.c \
        src/LLDlpc.c \
        src/GENoutput.c \
        src/feature-memory.c \
        src/featum_util.c \
        src/SVMpredict.cpp \
	src/silence-detector.cpp \
        src/pcm-process.c \
        src/pcm-data.c \
        src/option-parser.c \
        src/fftsg.c \
	src/AMQoutput.cpp \
	src/TumFeatureExtractor.cpp \
	svm/svm.cpp
  SEMAINExtract_CPPFLAGS =  $(PORTAUDIO_CPPFLAGS) $(SEMAINEAPI_CPPFLAGS)
  SEMAINExtract_LDADD = $(PORTAUDIO_LDFLAGS) $(SEMAINEAPI_LDFLAGS) -lm
endif 

if BUILD_SMILErec
  bin_PROGRAMS += SMILErec

  SMILErec_SOURCES = \
	src/SMILErec.c \
	src/wave-output.c \
	src/featum_util.c \
	src/wave-input.c \
	src/pcm-data.c \
	src/fftsg.c \
	src/pcm-process.c \
	src/audio-stream.c \
	src/option-parser.c \
	src/live-input.c
  SMILErec_CPPFLAGS = $(PORTAUDIO_CPPFLAGS)
  SMILErec_LDADD = $(PORTAUDIO_LDFLAGS) -lm

endif

if BUILD_SMILEsync
  bin_PROGRAMS += SMILEsync

  SMILEsync_SOURCES = \
	src/SMILEsync.c \
	src/featum_util.c \
	src/wave-input.c \
	src/wave-output.c \
	src/pcm-data.c \
	src/option-parser.c \
	src/fftsg.c \
	src/pcm-process.c
  SMILEsync_CPPFLAGS = $(PORTAUDIO_CPPFLAGS)
  SMILEsync_LDADD = $(PORTAUDIO_LDFLAGS) -lm
endif

if BUILD_audiorecorder
  bin_PROGRAMS += audiorecorder

  audiorecorder_SOURCES = \
	src/audiorecorder.cpp \
	src/wave-output.c \
	src/wave-input.c \
	src/featum_util.c \
	src/pcm-data.c \
	src/fftsg.c \
	src/pcm-process.c \
	src/audio-stream.c \
	src/option-parser.c \
	src/live-input.c
  audiorecorder_CPPFLAGS = $(PORTAUDIO_CPPFLAGS)
  audiorecorder_LDADD = $(PORTAUDIO_LDFLAGS) -lm
endif

if BUILD_audiosplit
  bin_PROGRAMS += audiosplit

  audiosplit_SOURCES = \
	src/audiosplit.cpp \
	src/wave-output.c \
	src/featum_util.c \
	src/wave-input.c \
	src/pcm-data.c \
	src/fftsg.c \
	src/pcm-process.c \
	src/audio-stream.c \
	src/option-parser.c \
	src/live-input.c
  audiosplit_CPPFLAGS = $(PORTAUDIO_CPPFLAGS)
  audiosplit_LDADD = $(PORTAUDIO_LDFLAGS) -lm
endif


