/*
 *  MessageFormatException.h
 *  semaine
 *
 *  Created by Marc Schröder on 09.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#ifndef SEMAINE_CMS_EXCEPTIONS_MESSAGEFORMATEXCEPTION_H
#define SEMAINE_CMS_EXCEPTIONS_MESSAGEFORMATEXCEPTION_H

#include <semaine/config.h>
#include <semaine/cms/exceptions/SEMAINEException.h>

namespace semaine {
namespace cms {
namespace exceptions {


class MessageFormatException : public SEMAINEException
{
public:
		inline MessageFormatException(const std::string & text) throw() :
		SEMAINEException(text)
		{
		}
		
		inline ~MessageFormatException() throw() {}
};


} // namespace exceptions
} // namespace cms
} // namespace semaine



#endif

