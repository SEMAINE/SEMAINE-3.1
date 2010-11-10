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

#include <semaine/util/XMLTool.h>

#include <semaine/cms/message/SEMAINEMessage.h>
#include <semaine/cms/exceptions/MessageFormatException.h>
#include <semaine/cms/exceptions/SystemConfigurationException.h>

#include <cms/Message.h>
#include <cms/CMSException.h>




using namespace cms;
using namespace semaine::util;

namespace semaine {
namespace cms {
namespace message {

class SEMAINEXMLMessage : public SEMAINEMessage
{
public:
	const static std::string IS_XML;

	SEMAINEXMLMessage(const Message * message)
	throw(semaine::cms::exceptions::MessageFormatException, semaine::cms::exceptions::SystemConfigurationException);
	virtual ~SEMAINEXMLMessage();
	XERCES_CPP_NAMESPACE::DOMDocument * getDocument()
	{
		return document;
	}


protected:
	XERCES_CPP_NAMESPACE::DOMDocument * document;
	void parseDocument() throw(semaine::cms::exceptions::MessageFormatException, semaine::cms::exceptions::SystemConfigurationException);



};

} // namespace message
} // namespace cms
} // namespace semaine







#endif


