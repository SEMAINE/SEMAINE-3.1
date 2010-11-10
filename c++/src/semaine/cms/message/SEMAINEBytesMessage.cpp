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
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifndef HAVE_MEMCPY
void * memcpy(void *s1, void *s2, int n) 
{
  int i;
  if (n % sizeof(int) == 0) {
    int *s1i = (int*)s1;
    int *s2i = (int*)s2;
    for (i=0; i<n; n += sizeof(int)) { *s1i = *s2i; s1i++; s2i++; }

  } else {
    char *s1c = (char*)s1;
    char *s2c = (char*)s2;
    for (i=0; i<n; n++) { *s1c = *s2c; s1c++; s2c++; }
  } 
  return s1;
}
#endif

namespace semaine {
namespace cms {
namespace message {


SEMAINEBytesMessage::SEMAINEBytesMessage(const Message * message)
throw(semaine::cms::exceptions::MessageFormatException,semaine::cms::exceptions::SystemConfigurationException) :
	SEMAINEMessage(message)
{
	const BytesMessage * bm = dynamic_cast<const BytesMessage *>(message);
	if (bm == NULL) {
		throw semaine::cms::exceptions::MessageFormatException(std::string("Expected a bytes message, but got a ")+typeid(*message).name());
	}
	
	try {
		len = bm->getBodyLength();
		bytes = (unsigned char *) malloc(len);
		std::size_t actualLength = bm->readBytes(bytes, len);
		if (actualLength != len) {
			std::stringstream buf;
			buf << "Message has " << len << " bytes, but could read only " << actualLength;
			throw semaine::cms::exceptions::MessageFormatException(buf.str());
		}
	} catch (const CMSException & ce) {
		ce.printStackTrace();
		throw semaine::cms::exceptions::MessageFormatException(std::string("Cannot read message"));
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
