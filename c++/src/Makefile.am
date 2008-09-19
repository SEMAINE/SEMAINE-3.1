# Automake file

semaine_api_sources_cpp = \
	semaine/cms/CMSLogger.cpp \
	semaine/cms/IOBase.cpp \
	semaine/cms/message/SEMAINEMessage.cpp \
        semaine/cms/message/SEMAINEFeatureMessage.cpp \
	semaine/cms/message/SEMAINEXMLMessage.cpp \
	semaine/cms/receiver/Receiver.cpp \
	semaine/cms/sender/Sender.cpp \
	semaine/cms/sender/FeatureSender.cpp \
	semaine/components/Component.cpp \
	semaine/components/dummy/GenericTestComponent.cpp \
	semaine/components/dummy/DummyFeatureExtractor.cpp \
	semaine/components/meta/MetaMessenger.cpp \
	semaine/system/ComponentRunner.cpp \
	semaine/cms/sender/XMLSender.cpp 



semaine_api_sources_h = \
	semaine/cms/CMSLogger.h \
	semaine/cms/event.h \
	semaine/cms/exceptions/MessageFormatException.h \
	semaine/cms/exceptions/SEMAINEException.h \
	semaine/cms/exceptions/SystemConfigurationException.h \
	semaine/cms/IOBase.h \
	semaine/cms/loglevel.h \
	semaine/cms/message/SEMAINEMessage.h \
        semaine/cms/message/SEMAINEFeatureMessage.h \
        semaine/cms/message/SEMAINEXMLMessage.h \
	semaine/cms/receiver/Receiver.h \
	semaine/cms/receiver/FeatureReceiver.h \
	semaine/cms/SEMAINEMessageAvailableListener.h \
	semaine/cms/sender/Sender.h \
	semaine/cms/sender/FeatureSender.h \
	semaine/components/Component.h \
	semaine/components/dummy/GenericTestComponent.h  \
        semaine/components/dummy/DummyFeatureExtractor.h \
	semaine/components/meta/MetaMessenger.h \
	semaine/config.h \
	semaine/system/ComponentRunner.h \
	semaine/cms/receiver/XMLReceiver.h \
	semaine/cms/message/SEMAINEXMLMessage.h \
	semaine/cms/sender/XMLSender.h


dummy_sources = main.cpp

bin_PROGRAMS = test
test_SOURCES = $(semaine_api_sources_cpp) $(semaine_api_sources_h) $(dummy_sources)
test_CXXFLAGS = $(SEMAINE_CXXFLAGS)
test_LDADD = $(SEMAINE_LDFLAGS)
