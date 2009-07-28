/*
 * ParseErrorHandler.h
 *
 *  Created on: 27.07.2009
 *      Author: marc
 */

#ifndef SEMAINE_UTIL_PARSEERRORHANDLER_H
#define SEMAINE_UTIL_PARSEERRORHANDLER_H

#include <xercesc/sax2/DefaultHandler.hpp>

#include <sstream>
#include <semaine/cms/exceptions/SEMAINEException.h>

namespace semaine {

namespace util {

class ParseErrorHandler	: /*public XERCES_CPP_NAMESPACE::DefaultHandler,*/ public XERCES_CPP_NAMESPACE::DOMErrorHandler {

public:
	ParseErrorHandler() {}
	virtual ~ParseErrorHandler() {}
	 bool handleError(const XERCES_CPP_NAMESPACE::DOMError &domError) {
		 std::stringstream stream;
		 XERCES_CPP_NAMESPACE::DOMError::ErrorSeverity severity = domError.getSeverity();
		 bool goOn;
		 std::string sev;
		 switch (severity) {
		 case XERCES_CPP_NAMESPACE::DOMError::DOM_SEVERITY_WARNING:
			 sev = "warning";
			 goOn = true;
			 break;
		 case XERCES_CPP_NAMESPACE::DOMError::DOM_SEVERITY_ERROR:
			 sev = "error";
			 goOn = true;
			 break;
		 case XERCES_CPP_NAMESPACE::DOMError::DOM_SEVERITY_FATAL_ERROR:
			 sev = "fatal error";
			 goOn = false;
		 }
		 stream << "Parser " << sev << ": " << UTF8(domError.getMessage());
		 XERCES_CPP_NAMESPACE::DOMLocator * loc = domError.getLocation();
		 if (loc != NULL) {
			 stream << " in " << loc->getURI() << " line " << loc->getLineNumber() << ", column " << loc->getColumnNumber();
		 }
		 errors.push_back(stream.str());
		 //std::cerr << "Remembering error message: " << stream.str() << std::endl;
//		 std::cerr << "About to throw an exception: " << stream.str() << std::endl;
//		 throw semaine::cms::exceptions::SEMAINEException(stream.str());
		 return goOn;
	 }

	 bool hasErrors()
	 {
		 return !errors.empty();
	 }

	 std::string getErrors()
	 {
		 std::stringstream stream;
		 for (std::list<std::string>::iterator it = errors.begin(); it != errors.end(); ++it) {
			 stream << *it << std::endl;
		 }
		 return stream.str();
	 }
private:
	std::list<std::string> errors;

	 /*
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
*/




};

}

}

#endif /* SEMAINE_UTIL_PARSEERRORHANDLER_H */
