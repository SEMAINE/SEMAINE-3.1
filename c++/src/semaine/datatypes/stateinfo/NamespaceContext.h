/*
 * StateInfo.h
 *
 *  Created on: 25.07.2009
 *      Author: marc
 */

#ifndef SEMAINE_DATATYPES_NAMESPACECONTEXT_H
#define SEMAINE_DATATYPES_NAMESPACECONTEXT_H

#include <string>
#include <map>

#include <xercesc/dom/DOM.hpp>

#include <semaine/util/XMLTool.h>

namespace semaine {

namespace datatypes {

namespace stateinfo {



class NamespaceContext : public XERCES_CPP_NAMESPACE::DOMXPathNSResolver {
public:
	NamespaceContext(const std::map<std::string, std::string> & prefix2NamespaceURI);
	virtual ~NamespaceContext() {}
	const XMLCh * lookupNamespaceURI(const XMLCh * prefix) const;
	const XMLCh * lookupPrefix(const XMLCh *URI) const;
	void addNamespaceBinding(const XMLCh*, const XMLCh*);
	void release() {}
	// Custom extension:
	const std::string getNamespaceURI(const std::string & prefix) const;
private:
	std::map<std::string, std::string> pref2ns;
};



}

}

}

#endif /* SEMAINE_DATATYPES_NAMESPACECONTEXT_H */
