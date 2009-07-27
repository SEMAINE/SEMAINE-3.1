/*
 * StateInfo.cpp
 *
 *  Created on: 25.07.2009
 *      Author: marc
 */

#include <semaine/datatypes/stateinfo/NamespaceContext.h>

#include <fstream>
#include <string>
#include <list>

using namespace XERCES_CPP_NAMESPACE;
using namespace semaine::util;

namespace semaine {

namespace datatypes {

namespace stateinfo {


NamespaceContext::NamespaceContext(const std::map<std::string, std::string> & prefix2NamespaceURI)
: DOMXPathNSResolver(), pref2ns(prefix2NamespaceURI)
{
}

const XMLCh * NamespaceContext::lookupNamespaceURI(const XMLCh * prefix) const
{
	if (prefix == NULL) return NULL;
	std::string pref = XMLTool::transcode(prefix);
	std::map<std::string, std::string>::const_iterator p = pref2ns.find(pref);
	if (p != pref2ns.end()) { // found
		std::string ns = p->second;
		const XMLCh * namespaceURI = XMLString::transcode(ns.c_str());
		// TODO: memory leak!
		return namespaceURI;

	} // not found
	return NULL;
}

const XMLCh * NamespaceContext::lookupPrefix(const XMLCh *URI) const
{
	throw SystemConfigurationException("Not implemented");
}

void NamespaceContext::addNamespaceBinding(const XMLCh* prefix, const XMLCh* namespaceURI)
{
	std::string pref = XMLTool::transcode(prefix);
	std::string ns = XMLTool::transcode(namespaceURI);
	pref2ns[pref] = ns;
}

const std::string NamespaceContext::getNamespaceURI(const std::string & prefix) const
{
	std::map<std::string, std::string>::const_iterator p = pref2ns.find(prefix);
	if (p != pref2ns.end()) { // found
		return p->second;
	}
	return "";
}








}

}

}
