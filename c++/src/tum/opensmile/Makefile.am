
# ACLOCAL_AMFLAGS = -I m4

AUTOMAKE_OPTIONS = subdir-objects
  
bin_PROGRAMS = SMILExtract 
bin_PROGRAMS += SEMAINExtract

SMILE_SOURCES=\
	src/configManager.cpp \
	src/exceptions.cpp \
	src/smileLogger.cpp \
	src/commandlineParser.cpp \
	src/smileUtil.c \
	src/smileCommon.cpp \
	src/smileComponent.cpp \
	src/dataMemory.cpp \
	src/buffer.cpp \
	src/bowProducer.cpp \
	src/signalGenerator.cpp \
	src/vecGlMean.cpp \
	src/fullinputMean.cpp \
	src/fullturnMean.cpp \
	src/vectorMVStd.cpp \
	src/vectorMVN.cpp \
	src/vectorHEQ.cpp \
	src/vectorOperation.cpp \
	src/vectorSorter.cpp \
	src/vectorTransform.cpp \
	src/turnDetector.cpp \
	src/valbasedSelector.cpp \
	src/vadV1.cpp \
	src/speechLearn.cpp \
	src/semaineSpeakerID1.cpp \
	src/componentManager.cpp \
	src/dataReader.cpp \
	src/dataWriter.cpp \
	src/dataSource.cpp \
	src/dataSink.cpp \
	src/dataProcessor.cpp \
	src/dataSelector.cpp \
	src/vectorConcat.cpp \
	src/vectorProcessor.cpp \
	src/winToVecProcessor.cpp \
	src/windowProcessor.cpp \
	src/portaudioSource.cpp \
	src/portaudioSink.cpp \
	src/portaudioDuplex.cpp \
	src/portaudioDuplexS.cpp \
	src/portaudioDuplexD.cpp \
	src/preemphasis.cpp \
	src/exampleSource.cpp \
	src/exampleSink.cpp \
	src/arffSink.cpp \
	src/libsvmSink.cpp \
	src/libsvmliveSink.cpp \
	src/rnn.cpp \
	src/rnnSink.cpp \
	src/rnnProcessor.cpp \
	src/rnnVad.cpp \
	src/rnnVad2.cpp \
	src/csvSink.cpp \
	src/csvSource.cpp \
	src/arffSource.cpp \
	src/htkSink.cpp \
	src/htkSource.cpp \
	src/datadumpSink.cpp \
	src/nullSink.cpp \
	src/exampleProcessor.cpp \
	src/vectorPreemphasis.cpp \
	src/deltaRegression.cpp \
	src/weightedDiff.cpp \
	src/contourSmoother.cpp \
	src/multipleWaveSources.cpp \
	src/waveSource.cpp \
	src/framer.cpp \
	src/windower.cpp \
	src/spectral.cpp \
	src/functionals.cpp \
	src/functionalsVecToVec.cpp \
	src/functionalComponent.cpp \
	src/functionalExtremes.cpp \
	src/functionalMeans.cpp \
	src/functionalPeaks.cpp \
	src/functionalSegments.cpp \
	src/functionalOnset.cpp \
	src/functionalSamples.cpp \
	src/functionalMoments.cpp \
	src/functionalCrossings.cpp \
	src/functionalPercentiles.cpp \
	src/functionalRegression.cpp \
	src/functionalTimes.cpp \
	src/functionalDCT.cpp \
	src/functionalLpc.cpp \
	src/mvfCovar.cpp \
	src/energy.cpp \
	src/intensity.cpp \
	src/dbA.cpp \
	src/amdf.cpp \
	src/acf.cpp \
	src/tonespec.cpp \
	src/tonefilt.cpp \
	src/fftsg.c \
	src/transformFft.cpp \
	src/fftmagphase.cpp \
	src/specScale.cpp \
	src/specResample.cpp \
	src/melspec.cpp \
	src/chroma.cpp \
	src/cens.cpp \
  	src/chromaFeatures.cpp  \
	src/mfcc.cpp \
	src/plp.cpp \
	src/maxIndex.cpp \
	src/monoMixdown.cpp \
	src/tumkwsaSink.cpp \
	src/tumkwsjSink.cpp \
	src/juliusSink.cpp \
	src/volanalyse.cpp \
	src/fingerprint.cpp \
	src/waveSink.cpp \
	src/waveSinkCut.cpp \
	src/pitchBase.cpp \
	src/pitchShs.cpp \
	src/pitchACF.cpp \
	src/pitchHpsLinf.cpp \
	src/pitchAmdfAcf.cpp \
	src/pitchSmoother.cpp \
	src/pitchJitter.cpp \
	src/pitchDirection.cpp \
	src/mzcr.cpp \
	src/lpc.cpp \
	src/lsp.cpp \
	src/formantLpc.cpp \
	src/formantSmoother.cpp \
	src/echoAttenuator.cpp \
	src/echoCanceller.cpp \
	src/speexPreprocess.cpp \
	src/speexResample.cpp \
	src/smileResample.cpp \
	src/libSamplerate.cpp \
	src/stretchToTime.cpp \
	src/chromaPerFrame.cpp \
	src/chordFeatures.cpp \
	src/channelMixer.cpp \
	src/gsl_zsolve.c \
	src/semaineTextOutput.cpp \
	svm/svm.cpp 

  SMILE_CPPFLAGS = -Isrc $(SEMAINEAPI_CPPFLAGS) $(PORTAUDIO_CPPFLAGS) -ggdb $(ATKLIB_CPPFLAGS) $(PTHREAD_CFLAGS) $(RTNNLLIB_CFLAGS) 
  SMILE_LIBS = $(PORTAUDIO_LDFLAGS) -lm $(SEMAINEAPI_LDFLAGS) $(ATKLIB_LDFLAGS) $(PTHREAD_LIBS) $(DLIB) $(RTNNLLIB_LIBS)
   
  SMILExtract_SOURCES = src/SMILExtract.cpp 
  SMILExtract_CPPFLAGS = $(SMILE_CPPFLAGS)
  SMILExtract_LDADD = $(SMILE_LIBS) -lopensmile  

  SEMAINExtract_SOURCES =\
	src/SEMAINExtract.cpp \
	src/activeMqSink.cpp \
	src/activeMqSource.cpp \
	src/TumKeywordSpotter.cpp \
	src/TumEmotionAnalyser.cpp \
	src/TumInterestDetector.cpp \
	src/TumNonverbalFeatureExtractor.cpp \
	src/TumAudioFeatureExtractor.cpp \
	src/semaineEmmaSender.cpp \
	src/semaineWordSender.cpp \
	src/semaineEmotionSender.cpp \
	src/semaineNonverbalSender.cpp \
	src/semaineUserinfoSender.cpp 

  SEMAINExtract_CPPFLAGS = $(SMILE_CPPFLAGS) 
  SEMAINExtract_LDADD = $(SMILE_LIBS) -lopensmile  

  lib_LTLIBRARIES = libopensmile.la
  libopensmile_la_SOURCES = $(SMILE_SOURCES)
  libopensmile_la_CPPFLAGS = $(SMILE_CPPFLAGS)
  libopensmile_la_LIBADD = $(SMILE_LIBS)
  libopensmile_la_LDFLAGS = -no-undefined


# endif 


