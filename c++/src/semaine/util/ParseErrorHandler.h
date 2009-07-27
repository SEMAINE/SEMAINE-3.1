/*
 * ParseErrorHandler.h
 *
 *  Created on: 27.07.2009
 *      Author: marc
 */

#ifndef SEMAINE_UTIL_PARSEERRORHANDLER_H
#define SEMAINE_UTIL_PARSEERRORHANDLER_H

#include <xercesc/sax2/DefaultHandler.hpp>


namespace semaine {

namespace util {

class ParseErrorHandler	: public XERCES_CPP_NAMESPACE::DefaultHandler {

public:
	ParseErrorHandler() {}
	virtual ~ParseErrorHandler() {}

	void warning(const XERCES_CPP_NAMESPACE::SAXParseException &exc)
	{
		std::cerr << "Parser warning in line " << exc.getLineNumber() << ": "
		<< XERCES_CPP_NAMESPACE::XMLString::transcode(exc.getMessage()) << std::endl;
	}

	void error(const XERCES_CPP_NAMESPACE::SAXParseException &exc)
	{
		std::cerr << "Parser error in line " << exc.getLineNumber() << ": "
		<< XERCES_CPP_NAMESPACE::XMLString::transcode(exc.getMessage()) << std::endl;
	}


	void fatalError(const XERCES_CPP_NAMESPACE::SAXParseException &exc)
	{
		std::cerr << "Fatal parser error in line " << exc.getLineNumber() << ": "
			<< XERCES_CPP_NAMESPACE::XMLString::transcode(exc.getMessage()) << std::endl;

		throw exc;

		//DefaultManager::fatalError(exc);

	}





};

}

}

#endif /* SEMAINE_UTIL_PARSEERRORHANDLER_H */
