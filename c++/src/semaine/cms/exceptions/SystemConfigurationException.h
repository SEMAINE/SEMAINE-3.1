/*
 *  SystemConfigurationException.h
 *  semaine
 *
 *  Created by Marc Schröder on 09.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#ifndef SEMAINE_CMS_EXCEPTIONS_SYSTEMCONFIGURATIONEXCEPTION_H
#define SEMAINE_CMS_EXCEPTIONS_SYSTEMCONFIGURATIONEXCEPTION_H

#include <semaine/cms/exceptions/SEMAINEException.h>

namespace semaine {
namespace cms {
namespace exceptions {


class SystemConfigurationException : public SEMAINEException
{
public:
		inline SystemConfigurationException(const std::string & text) throw() :
		SEMAINEException(text)
		{
		}
		
		inline ~SystemConfigurationException() throw() {}

};


} // namespace exceptions
} // namespace cms
} // namespace semaine



#endif

