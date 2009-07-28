# Automake file

semaine_api_sources_cpp = \
	semaine/util/XMLTool.cpp \
	semaine/cms/CMSLogger.cpp \
	semaine/cms/IOBase.cpp \
	semaine/cms/message/SEMAINEMessage.cpp \
	semaine/cms/message/SEMAINEFeatureMessage.cpp \
	semaine/cms/message/SEMAINEXMLMessage.cpp \
	semaine/cms/message/SEMAINEStateMessage.cpp \
	semaine/cms/message/SEMAINEBytesMessage.cpp \
	semaine/cms/receiver/Receiver.cpp \
	semaine/cms/receiver/StateReceiver.cpp \
	semaine/cms/sender/Sender.cpp \
	semaine/cms/sender/FeatureSender.cpp \
	semaine/cms/sender/StateSender.cpp \
	semaine/components/Component.cpp \
	semaine/components/meta/MetaMessenger.cpp \
	semaine/system/ComponentRunner.cpp \
	semaine/cms/sender/XMLSender.cpp \
	semaine/cms/sender/BytesSender.cpp \
	semaine/datatypes/xml/EMMA.cpp \
	semaine/datatypes/xml/EmotionML.cpp \
	semaine/datatypes/xml/SemaineML.cpp \
	semaine/datatypes/xml/FML.cpp \
	semaine/datatypes/xml/BML.cpp \
	semaine/datatypes/xml/SSML.cpp \
	semaine/datatypes/stateinfo/NamespaceContext.cpp \
	semaine/datatypes/stateinfo/XPathInfoMapper.cpp \
	semaine/datatypes/stateinfo/StateInfo.cpp




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
	semaine/cms/message/SEMAINEBytesMessage.h \
	semaine/cms/receiver/Receiver.h \
	semaine/cms/receiver/FeatureReceiver.h \
	semaine/cms/receiver/BytesReceiver.h \
	semaine/cms/SEMAINEMessageAvailableListener.h \
	semaine/cms/sender/Sender.h \
	semaine/cms/sender/FeatureSender.h \
	semaine/cms/sender/BytesSender.h \
	semaine/components/Component.h \
	semaine/components/meta/MetaMessenger.h \
	semaine/config.h \
	semaine/util/XMLTool.h \
	semaine/util/ParseErrorHandler.h \
	semaine/system/ComponentRunner.h \
	semaine/cms/receiver/XMLReceiver.h \
	semaine/cms/receiver/FMLReceiver.h \
	semaine/cms/receiver/BMLReceiver.h \
	semaine/cms/message/SEMAINEXMLMessage.h \
	semaine/cms/message/SEMAINEStateMessage.h \
	semaine/cms/sender/XMLSender.h \
	semaine/cms/sender/StateSender.h \
	semaine/cms/sender/FMLSender.h \
	semaine/cms/sender/BMLSender.h \
	semaine/cms/sender/EmmaSender.h \
	semaine/datatypes/xml/EMMA.h \
	semaine/datatypes/xml/EmotionML.h \
	semaine/datatypes/xml/SemaineML.h \
	semaine/datatypes/xml/FML.h \
	semaine/datatypes/xml/BML.h \
	semaine/datatypes/xml/SSML.h \
	semaine/datatypes/stateinfo/NamespaceContext.h \
	semaine/datatypes/stateinfo/XPathInfoMapper.h \
	semaine/datatypes/stateinfo/StateInfo.h \
	semaine/datatypes/stateinfo/UserStateInfo.h \
	semaine/datatypes/stateinfo/DialogStateInfo.h \
	semaine/datatypes/stateinfo/ContextStateInfo.h \
	semaine/datatypes/stateinfo/SystemStateInfo.h \
	semaine/datatypes/stateinfo/AgentStateInfo.h



lib_LTLIBRARIES = libsemaineapi.la
libsemaineapi_la_SOURCES = $(semaine_api_sources_cpp) $(semaine_api_sources_h)
libsemaineapi_la_CXXFLAGS = $(SEMAINE_CXXFLAGS)
include_HEADERS = $(semaine_api_sources_h)

bin_PROGRAMS = basictests jimandjohn dummyfeatextr dummyanalyser dummyfml2bml teststateinfo participantcontrol

basictests_SOURCES = semaine/executables/basictests.cpp
basictests_CXXFLAGS = $(SEMAINE_CXXFLAGS)
basictests_LDADD = libsemaineapi.la $(SEMAINE_LDFLAGS)

jimandjohn_SOURCES = semaine/executables/jimandjohn.cpp \
	semaine/components/dummy/GenericTestComponent.cpp
jimandjohn_CXXFLAGS = $(SEMAINE_CXXFLAGS)
jimandjohn_LDADD = libsemaineapi.la $(SEMAINE_LDFLAGS)

dummyfeatextr_SOURCES = semaine/executables/dummyfeatextr.cpp \
	semaine/components/dummy/DummyFeatureExtractor.cpp
dummyfeatextr_CXXFLAGS = $(SEMAINE_CXXFLAGS)
dummyfeatextr_LDADD = libsemaineapi.la $(SEMAINE_LDFLAGS)

dummyanalyser_SOURCES = semaine/executables/dummyanalyser.cpp \
	semaine/components/dummy/DummyAnalyser.cpp
dummyanalyser_CXXFLAGS = $(SEMAINE_CXXFLAGS)
dummyanalyser_LDADD = libsemaineapi.la $(SEMAINE_LDFLAGS)

dummyfml2bml_SOURCES = semaine/executables/dummyfml2bml.cpp \
	semaine/components/dummy/DummyFML2BML.cpp
dummyfml2bml_CXXFLAGS = $(SEMAINE_CXXFLAGS)
dummyfml2bml_LDADD = libsemaineapi.la $(SEMAINE_LDFLAGS)

teststateinfo_SOURCES = semaine/executables/testStateInfo.cpp
teststateinfo_CXXFLAGS = $(SEMAINE_CXXFLAGS)
teststateinfo_LDADD = libsemaineapi.la $(SEMAINE_LDFLAGS)

participantcontrol_SOURCES = semaine/executables/participantcontrol.cpp semaine/components/control/ParticipantControl.cpp
participantcontrol_CXXFLAGS = $(SEMAINE_CXXFLAGS)
participantcontrol_LDADD = libsemaineapi.la $(SEMAINE_LDFLAGS)

