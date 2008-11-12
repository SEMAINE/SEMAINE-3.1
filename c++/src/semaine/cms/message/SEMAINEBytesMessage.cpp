/*
 *  SEMAINEXMLMessage.cpp
 *  semaine
 *
 *  Created by Marc Schröder on 19.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#include "SEMAINEBytesMessage.h"

#include <sstream>

namespace semaine {
namespace cms {
namespace message {


SEMAINEBytesMessage::SEMAINEBytesMessage(const Message * message)
throw(MessageFormatException,SystemConfigurationException) :
	SEMAINEMessage(message)
{
	const BytesMessage * bm = dynamic_cast<const BytesMessage *>(message);
	if (bm == NULL) {
		throw MessageFormatException(std::string("Expected a bytes message, but got a ")+typeid(*message).name());
	}
	
	try {
		len = bm->getBodyLength();
		bytes = (unsigned char *) malloc(len);
		std::size_t actualLength = bm->readBytes(bytes, len);
		if (actualLength != len) {
			std::stringstream buf;
			buf << "Message has " << len << " bytes, but could read only " << actualLength;
			throw MessageFormatException(buf.str());
		}
	} catch (const CMSException & ce) {
		ce.printStackTrace();
		throw MessageFormatException(std::string("Cannot read message"));
	}
}

/**
 * Access the binary data contained in this message. The caller will receive a copy of the byte data
 * in an array pointed to by returnBytes; it is the caller's responsibility to free the array after use.
 * @return the number of bytes contained in the array.
 */
std::size_t SEMAINEBytesMessage::getBytes(unsigned char *& returnBytes)
{
	returnBytes = (unsigned char *) malloc(len);
	memcpy(returnBytes, bytes, len);
	return len;
}


SEMAINEBytesMessage::~SEMAINEBytesMessage()
{
	free(bytes);
}


} // namespace message
} // namespace cms
} // namespace semaine