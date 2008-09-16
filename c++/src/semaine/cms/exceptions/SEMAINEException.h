/*
 *  SEMAINEException.h
 *  semaine
 *
 *  Created by Marc Schröder on 09.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#ifndef SEMAINE_CMS_EXCEPTIONS_SEMAINEEXCEPTION_H
#define SEMAINE_CMS_EXCEPTIONS_SEMAINEEXCEPTION_H


namespace semaine {
namespace cms {
namespace exceptions {


class SEMAINEException : public std::exception
{
public:
		inline SEMAINEException(const std::string & text) throw()
		{
			message = text;
		}
		
		inline ~SEMAINEException() throw() {}

		const char * what() { return message.c_str(); }

protected:
	std::string message;

};


} // namespace exceptions
} // namespace cms
} // namespace semaine



#endif

