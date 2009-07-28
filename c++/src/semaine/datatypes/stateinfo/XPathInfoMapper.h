/*
 * StateInfo.h
 *
 *  Created on: 25.07.2009
 *      Author: marc
 */

#ifndef SEMAINE_DATATYPES_XPATHINFOMAPPER_H
#define SEMAINE_DATATYPES_XPATHINFOMAPPER_H

#include <string>
#include <map>

#include <xercesc/dom/DOM.hpp>

#include <semaine/util/XMLTool.h>

#include <semaine/cms/CMSLogger.h>
#include <semaine/cms/exceptions/SystemConfigurationException.h>
#include <semaine/cms/exceptions/MessageFormatException.h>

#include <semaine/datatypes/stateinfo/NamespaceContext.h>
#include <xqilla/xqilla-dom3.hpp>

namespace semaine {

namespace datatypes {

namespace stateinfo {


class XPathInfoMapper {
public:
	XPathInfoMapper(const std::map<std::string, std::string> & prefixes2NamespaceURIs,
		const std::map<std::string, std::string> & names2XpathExpressions);
	virtual ~XPathInfoMapper() {}
	const NamespaceContext * getNamespaceContext()
	{
		return &namespaceContext;
	}

	/**
	 * Get the XPath expression for the given name,
	 * or NULL if there is no such entry.
	 * @param shortName
	 * @return
	 */
	const std::string * getExpression(const std::string & shortName)
	{
		std::map<std::string, std::string>::const_iterator it = names2exprString.find(shortName);
		if (it != names2exprString.end()) return &(it->second);
		return NULL;
	}

	const std::map<std::string, std::string> & getExpressionMap()
	{
		return names2exprString;
	}

private:
	NamespaceContext namespaceContext;
	const std::map<std::string, std::string> names2exprString;

};




}

}

}

#endif /* SEMAINE_DATATYPES_XPATHINFOMAPPER_H */
