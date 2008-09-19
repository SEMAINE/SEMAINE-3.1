/*
 *  SEMAINEXMLMessage.h
 *  semaine
 *
 *  Created by Marc Schröder on 19.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#ifndef SEMAINE_CMS_MESSAGE_SEMAINEXMLMESSAGE_H
#define SEMAINE_CMS_MESSAGE_SEMAINEXMLMESSAGE_H

#include <semaine/config.h>

#include <semaine/cms/message/SEMAINEMessage.h>
#include <semaine/cms/exceptions/MessageFormatException.h>
#include <semaine/cms/exceptions/SystemConfigurationException.h>

#include <cms/Message.h>
#include <cms/CMSException.h>


#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#if defined(XERCES_NEW_IOSTREAMS)
  #include <iostream>
#else
  #include <iostream.h>
#endif

XERCES_CPP_NAMESPACE_USE



using namespace cms;
using namespace semaine::cms::exceptions;

namespace semaine {
namespace cms {
namespace message {

class SEMAINEXMLMessage : public SEMAINEMessage
{
public:
	const static std::string IS_XML;

	SEMAINEXMLMessage(const Message * message)
	throw(MessageFormatException, SystemConfigurationException);
	virtual ~SEMAINEXMLMessage();
	DOMDocument * getDocument()
	{
		return document;
	}


protected:
	DOMDocument * document;
	void parseDocument() throw(MessageFormatException, SystemConfigurationException);



};

} // namespace message
} // namespace cms
} // namespace semaine







#endif


