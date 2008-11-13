/*
 *  SEMAINEXMLMessage.h
 *  semaine
 *
 *  Created by Marc Schröder on 19.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#ifndef SEMAINE_CMS_MESSAGE_SEMAINEBYTESMESSAGE_H
#define SEMAINE_CMS_MESSAGE_SEMAINEBYTESMESSAGE_H

#include <semaine/config.h>
#include <cstdlib>
#include <string>

#include <semaine/cms/message/SEMAINEMessage.h>
#include <semaine/cms/exceptions/MessageFormatException.h>
#include <semaine/cms/exceptions/SystemConfigurationException.h>

#include <cms/Message.h>
#include <cms/CMSException.h>




using namespace cms;
using namespace semaine::cms::exceptions;

namespace semaine {
namespace cms {
namespace message {

class SEMAINEBytesMessage : public SEMAINEMessage
{
public:
	SEMAINEBytesMessage(const Message * message)
	throw(MessageFormatException, SystemConfigurationException);
	virtual ~SEMAINEBytesMessage();
	std::size_t getBytes(unsigned char *&bytes);

private:
	unsigned char * bytes;
	std::size_t len;

};

} // namespace message
} // namespace cms
} // namespace semaine







#endif


