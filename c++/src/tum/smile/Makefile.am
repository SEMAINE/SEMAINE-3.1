
AUTOMAKE_OPTIONS = subdir-objects
bin_PROGRAMS = 
  
if BUILD_featum
  bin_PROGRAMS += featum

  featum_SOURCES=\
	src/main.cpp \
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
  featum_CPPFLAGS = $(PORTAUDIO_CPPFLAGS)
  featum_LDADD = $(PORTAUDIO_LDFLAGS) -lm
endif 

if BUILD_featumamq
  bin_PROGRAMS += featumamq

  featumamq_SOURCES = \
	src/featum.cpp \
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
  featumamq_CPPFLAGS =  $(PORTAUDIO_CPPFLAGS) $(SEMAINEAPI_CPPFLAGS)
  featumamq_LDADD = $(PORTAUDIO_LDFLAGS) $(SEMAINEAPI_LDFLAGS) -lm
endif 

if BUILD_tumrec
  bin_PROGRAMS += tumrec

  tumrec_SOURCES = \
	src/main_recorder.c \
	src/wave-output.c \
	src/featum_util.c \
	src/wave-input.c \
	src/pcm-data.c \
	src/fftsg.c \
	src/pcm-process.c \
	src/audio-stream.c \
	src/option-parser.c \
	src/live-input.c
  tumrec_CPPFLAGS = $(PORTAUDIO_CPPFLAGS)
  tumrec_LDADD = $(PORTAUDIO_LDFLAGS) -lm

endif

if BUILD_tumsync
  bin_PROGRAMS += tumsync

  tumsync_SOURCES = \
	src/main_sync.c \
	src/featum_util.c \
	src/wave-input.c \
	src/wave-output.c \
	src/pcm-data.c \
	src/option-parser.c \
	src/fftsg.c \
	src/pcm-process.c
  tumsync_CPPFLAGS = $(PORTAUDIO_CPPFLAGS)
  tumsync_LDADD = $(PORTAUDIO_LDFLAGS) -lm
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


