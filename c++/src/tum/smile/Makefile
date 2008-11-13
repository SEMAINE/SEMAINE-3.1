# Project: smile

CPP  = g++
CC   = gcc
AR   = ar
RANLIB = ranlib

OBJFT  = src/main.o src/input-framer.o src/featum_util.o src/wave-input.o src/wave-output.o src/pcm-data.o src/pcm-process.o src/audio-stream.o src/option-parser.o src/feature-memory.o src/LLDs.o src/LLDex.o src/LLDenergy.o src/extractors-globalconfig.o src/functionals.o src/LLDfft.o src/fftsg.o src/LLDmfcc.o src/LLDpitch.o src/deltas.o src/HTKoutput.o src/CSVoutput.o src/LLDtime.o src/ARFFoutput.o src/LLDcepstrum.o src/LLDcepstralF0.o src/LLDlpc.o src/live-input.o src/GENoutput.o src/LSVMoutput.o src/LLDspectral.o src/LLDvq.o

OBJFTcpp  = src/input-framer.o src/featum_util.o src/wave-input.o src/wave-output.o src/pcm-data.o src/pcm-process.o src/audio-stream.o src/option-parser.o src/feature-memory.o src/LLDs.o src/LLDex.o src/LLDenergy.o src/extractors-globalconfig.o src/functionals.o src/LLDfft.o src/fftsg.o src/LLDmfcc.o src/LLDpitch.o src/deltas.o src/HTKoutput.o src/CSVoutput.o src/LLDtime.o src/ARFFoutput.o src/LLDcepstrum.o src/LLDcepstralF0.o src/LLDlpc.o src/live-input.o src/GENoutput.o svm/svm.o src/SVMpredict.o src/LSVMoutput.o src/silence-detector.o src/LLDspectral.o src/LLDvq.o

OBJAREC  = src/main_recorder.o src/featum_util.o src/wave-input.o src/wave-output.o src/pcm-data.o src/audio-stream.o src/option-parser.o src/live-input.o

OBJSYNC  = src/main_sync.o src/featum_util.o src/wave-input.o src/wave-output.o src/pcm-data.o src/option-parser.o

LINKOBJFT  = $(OBJFT) 
#src/main.o src/input-framer.o src/featum_util.o src/wave-input.o  src/wave-output.o src/pcm-data.o src/pcm-process.o src/audio-stream.o src/option-parser.o src/LLDs.o src/LLDex.o src/LLDenergy.o src/feature-memory.o src/extractors-globalconfig.o src/functionals.o src/LLDfft.o src/fftsg.o src/LLDmfcc.o src/LLDpitch.o src/deltas.o src/HTKoutput.o src/CSVoutput.o src/LLDtime.o src/ARFFoutput.o src/LLDcepstrum.o src/LLDcepstralF0.o src/LLDlpc.o src/live-input.o src/GENoutput.o src/LSVMoutput.o src/LLDspectral.o src/LLDvq.o

LINKOBJFTcpp  = $(OBJFTcpp) 
#src/input-framer.o src/featum_util.o src/wave-input.o  src/wave-output.o src/pcm-data.o src/pcm-process.o src/audio-stream.o src/option-parser.o src/LLDs.o src/LLDex.o src/LLDenergy.o src/feature-memory.o src/extractors-globalconfig.o src/functionals.o src/LLDfft.o src/fftsg.o src/LLDmfcc.o src/LLDpitch.o src/deltas.o src/HTKoutput.o src/CSVoutput.o src/LLDtime.o src/ARFFoutput.o src/LLDcepstrum.o src/LLDcepstralF0.o src/LLDlpc.o src/live-input.o src/GENoutput.o svm/svm.o src/SVMpredict.o src/LSVMoutput.o src/silence-detector.o src/LLDspectral.o src/LLDvq.o
LINKOBJAREC  = src/main_recorder.o src/featum_util.o src/wave-input.o src/wave-output.o src/pcm-data.o src/audio-stream.o src/option-parser.o src/live-input.o
LINKOBJSYNC  = src/main_sync.o src/featum_util.o src/wave-input.o src/wave-output.o src/pcm-data.o src/option-parser.o


PA_INC = ../../../../thirdparty/inst/include
PA_LIB = ../../../../thirdparty/inst/lib

# use these lines for linux with portaudio installed in ./pa-inst
#LIBS =  -lc -lm -lrt -lasound -ljack -lpthread -lportaudio 
LIBS_LIN_PA =  -lc -lm -lrt -lasound -lpthread $(PA_LIB)/libportaudio.a
# 
LDFLAGS_LIN_PA = 
#-L$(PA_LIB)

# use this line for windows with portaudio
#LIBS_WIN_PA = -L. -lm -lportaudio

# use this line for windows/linux without portaudio
#LIBS = -lm

INCS = 
BIN  = feaTUM feaTUMcpp SEMAINEsync TUMrec feaTUM.exe feaTUMcpp.exe SEMAINEsync.exe TUMrec.exe
# use -DSAMPLEFP_ISFLOAT option for compiling the TUMrec recorder only!
#CFLAGS = -O3 $(INCS) -DSAMPLEFP_ISFLOAT  
#CFLAGS = -O3 $(INCS)
#CFLAGS = -O3 -mtune=athlon64 -mpopcnt -m3dnow -mfpmath=sse -msse -msse2 -mmmx $(INCS)
#CFLAGS = -O3 -mtune=athlon64 -m3dnow -mfpmath=sse -msse -msse2 -mmmx $(INCS)
#CFLAGS = -O3 -mtune=pentium4 -mfpmath=sse -msse -msse2 -malign-double -mmmx $(INCS)

# use this line for linux  with portaudio installed in prefix ./pa-inst/

CFLAGS = -g 
#-O2 -mtune=i686 -mfpmath=sse -msse -msse2 -mmmx $(INCS)
CFLAGS_LIN_PA = -I$(PA_INC) $(CFLAGS)
#CFLAGS_LIN_PA = $(CFLAGS)

#CFLAGS_WIN_PA = $(CFLAGS)

# use this line for windows (have libportaudio.dll.a in the current path... and libportaudio-2.dll in system32/)
#CFLAGS = -O2 -mtune=i686 -mfpmath=sse -msse -msse2 -mmmx $(INCS)
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before smileLib feaTUMcpp all-after


clean: clean-custom
	${RM} $(OBJSYNC) $(OBJAREC) $(OBJFT) $(OBJFTcpp) smileLib.a src/maincpp.o $(BIN) svm/svm.o 2>/dev/null
	${RM} src/*.cpp~ src/*.c~ src/*.h~ src/*.hpp~

smileLib: smileLib.a

smileLib.a: $(OBJFTcpp)
	$(AR) -cr smileLib.a $(LINKOBJFTcpp) 
	$(RANLIB) smileLib.a

feaTUMcpp: src/maincpp.o smileLib.a
	$(CPP)  src/maincpp.o smileLib.a $(CFLAGS_LIN_PA) $(LDFLAGS_LIN_PA) -o "feaTUMcpp" $(LIBS_LIN_PA)

#feaTUMcpp: src/maincpp.o $(OBJFTcpp)
#	$(CPP)  src/maincpp.o $(LINKOBJFTcpp) $(LDFLAGS_LIN_PA) -o "feaTUMcpp" $(LIBS_LIN_PA)

feaTUM: $(OBJFT)
	$(CPP)  $(LINKOBJFT) $(LDFLAGS) -o "feaTUM" $(LIBS)

TUMrec: $(OBJAREC)
	$(CC)  $(LINKOBJAREC) $(LDFLAGS) -o "TUMrec" $(LIBS)

SEMAINEsync: $(OBJSYNC)
	$(CC)  $(LINKOBJSYNC) $(LDFLAGS) -o "SEMAINEsync" $(LIBS)

src/main.o: src/main.c
	$(CC) -c src/main.c -o src/main.o $(CFLAGS)

src/maincpp.o: src/main.cpp
	$(CPP) -c src/main.cpp -o src/maincpp.o $(CFLAGS_LIN_PA)

src/main_recorder.o: src/main_recorder.c
	$(CC) -c src/main_recorder.c -o src/main_recorder.o $(CFLAGS)

src/main_sync.o: src/main_sync.c
	$(CC) -c src/main_sync.c -o src/main_sync.o $(CFLAGS)

src/input-framer.o: src/input-framer.c
	$(CC) -c src/input-framer.c -o src/input-framer.o $(CFLAGS)

src/featum_util.o: src/featum_util.c
	$(CC) -c src/featum_util.c -o src/featum_util.o $(CFLAGS)

src/wave-input.o: src/wave-input.c
	$(CC) -c src/wave-input.c -o src/wave-input.o $(CFLAGS)

src/live-input.o: src/live-input.c
	$(CC) -c src/live-input.c $(CFLAGS_LIN_PA) -o src/live-input.o 

src/pcm-data.o: src/pcm-data.c
	$(CC) -c src/pcm-data.c -o src/pcm-data.o $(CFLAGS)

src/pcm-process.o: src/pcm-process.c
	$(CC) -c src/pcm-process.c -o src/pcm-process.o $(CFLAGS)

src/audio-stream.o: src/audio-stream.c
	$(CC) -c src/audio-stream.c -o src/audio-stream.o $(CFLAGS_LIN_PA)

src/option-parser.o: src/option-parser.c
	$(CC) -c src/option-parser.c -o src/option-parser.o $(CFLAGS)

src/feature-memory.o: src/feature-memory.c
	$(CC) -c src/feature-memory.c -o src/feature-memory.o $(CFLAGS)

src/LLDex.o: src/LLDex.c
	$(CC) -c src/LLDex.c -o src/LLDex.o $(CFLAGS)

src/LLDs.o: src/LLDs.c
	$(CC) -c src/LLDs.c -o src/LLDs.o $(CFLAGS)

src/LLDenergy.o: src/LLDenergy.c
	$(CC) -c src/LLDenergy.c -o src/LLDenergy.o $(CFLAGS)

src/extractors-globalconfig.o:src/extractors-globalconfig.c
	$(CC) -c src/extractors-globalconfig.c -o src/extractors-globalconfig.o $(CFLAGS)

src/functionals.o: src/functionals.c src/functionals_comp.h src/functionals.h
	$(CC) -c src/functionals.c -o src/functionals.o $(CFLAGS)

src/deltas.o: src/deltas.c
	$(CC) -c src/deltas.c -o src/deltas.o $(CFLAGS)

src/LLDfft.o: src/LLDfft.c
	$(CC) -c src/LLDfft.c -o src/LLDfft.o $(CFLAGS)

src/fftsg.o: src/fftsg.c
	$(CC) -c src/fftsg.c -o src/fftsg.o $(CFLAGS)

src/LLDmfcc.o: src/LLDmfcc.c
	$(CC) -c src/LLDmfcc.c -o src/LLDmfcc.o $(CFLAGS)

src/LLDpitch.o: src/LLDpitch.c
	$(CC) -c src/LLDpitch.c -o src/LLDpitch.o $(CFLAGS)

src/LLDcepstrum.o: src/LLDcepstrum.c
	$(CC) -c src/LLDcepstrum.c -o src/LLDcepstrum.o $(CFLAGS)

src/LLDcepstralF0.o: src/LLDcepstralF0.c
	$(CC) -c src/LLDcepstralF0.c -o src/LLDcepstralF0.o $(CFLAGS)

src/LLDlpc.o: src/LLDlpc.c
	$(CC) -c src/LLDlpc.c -o src/LLDlpc.o $(CFLAGS)

src/LLDspectral.o: src/LLDspectral.c
	$(CC) -c src/LLDspectral.c -o src/LLDspectral.o $(CFLAGS)

src/LLDvq.o: src/LLDvq.c
	$(CC) -c src/LLDvq.c -o src/LLDvq.o $(CFLAGS)

src/GENoutput.o: src/GENoutput.c
	$(CC) -c src/GENoutput.c -o src/GENoutput.o $(CFLAGS)

src/HTKoutput.o: src/HTKoutput.c
	$(CC) -c src/HTKoutput.c -o src/HTKoutput.o $(CFLAGS)

src/CSVoutput.o: src/CSVoutput.c
	$(CC) -c src/CSVoutput.c -o src/CSVoutput.o $(CFLAGS)

src/SVMpredict.o: src/SVMpredict.cpp
	$(CPP) -c src/SVMpredict.cpp -o src/SVMpredict.o $(CFLAGS)

src/AMQoutput.o: src/AMQoutput.cpp
	$(CPP) -c src/AMQoutput.cpp -o src/AMQoutput.o $(CFLAGS)

src/ARFFoutput.o: src/ARFFoutput.c
	$(CC) -c src/ARFFoutput.c -o src/ARFFoutput.o $(CFLAGS)

src/LSVMoutput.o: src/LSVMoutput.c
	$(CC) -c src/LSVMoutput.c -o src/LSVMoutput.o $(CFLAGS)

src/LLDtime.o: src/LLDtime.c
	$(CC) -c src/LLDtime.c -o src/LLDtime.o $(CFLAGS)

src/silence-detector.o: src/silence-detector.cpp
	$(CPP) -c src/silence-detector.cpp -o src/silence-detector.o $(CFLAGS)

svm/svm.o: svm/svm.cpp
	$(CPP) -Wall -Wconversion -fPIC -c svm/svm.cpp -o svm/svm.o $(CFLAGS)
