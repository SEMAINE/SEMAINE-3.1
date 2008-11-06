/*
 *  XMLTool.cpp
 *  semaine
 *
 *  Created by Marc Schröder on 03.11.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#include "XMLTool.h"

namespace semaine {
namespace util {

XercesDOMParser * XMLTool::parser;

void XMLTool::startupXMLTools()
{
    XMLPlatformUtils::Initialize();
	parser = new XercesDOMParser();
    //parser->setValidationScheme(XercesDOMParser::Val_Always);    
	parser->setDoNamespaces(true);
}

void XMLTool::shutdownXMLTools()
{
	delete parser;
	XMLPlatformUtils::Terminate();
}



DOMImplementation * XMLTool::getDOMImplementation()
{
	XMLCh tempStr[100];
	XMLString::transcode("LS", tempStr, 99);
	DOMImplementation * impl = DOMImplementationRegistry::getDOMImplementation(tempStr);
	//DOMImplementationLS * implLS = dynamic_cast<DOMImplementationLS *>(impl);
	return impl;
}


DOMDocument * XMLTool::parse(const std::string & xmlAsText)
{
		//std::cerr << "Message text: " << std::endl << xmlAsText << std::endl;
		const char * msgTextC = xmlAsText.c_str();
		MemBufInputSource* memIS = new MemBufInputSource((const XMLByte *)msgTextC, strlen(msgTextC), "test", false);
        parser->parse(*memIS);
		DOMDocument * document = parser->getDocument();
		return document;


}

DOMDocument * XMLTool::newDocument(const std::string & rootTagname, const std::string & aNamespace)
{
	XMLCh xmlNamespaceURI[aNamespace.length()+1];
	XMLString::transcode(aNamespace.c_str(), xmlNamespaceURI, aNamespace.length());
	XMLCh xmlQualifiedName[rootTagname.length()+1];
	XMLString::transcode(rootTagname.c_str(), xmlQualifiedName, rootTagname.length());
	DOMImplementation * dom = getDOMImplementation();
	return dom->createDocument(xmlNamespaceURI, xmlQualifiedName, NULL);
}

DOMDocument * XMLTool::newDocument(const std::string & rootTagname, const std::string & aNamespace, const std::string & version)
{
	DOMDocument * doc = newDocument(rootTagname, aNamespace);
	DOMElement * root = doc->getDocumentElement();
    setAttribute(root, "version", version);
	return doc;
}


DOMElement * XMLTool::createElement(DOMDocument * doc, const std::string & elementName)
{
	const XMLCh * xmlNamespaceURI = doc->getDocumentElement()->getNamespaceURI();
	XMLCh xmlElementName[elementName.length()+1];
	XMLString::transcode(elementName.c_str(), xmlElementName, elementName.length());
	return doc->createElementNS(xmlNamespaceURI, xmlElementName);
}

DOMElement * XMLTool::createElement(DOMDocument * doc, const std::string & elementName, const std::string & aNamespace)
{
	XMLCh xmlNamespaceURI[aNamespace.length()+1];
	XMLString::transcode(aNamespace.c_str(), xmlNamespaceURI, aNamespace.length());
	XMLCh xmlElementName[elementName.length()+1];
	XMLString::transcode(elementName.c_str(), xmlElementName, elementName.length());
	return doc->createElementNS(xmlNamespaceURI, xmlElementName);
}

DOMElement * XMLTool::appendChildElement(DOMNode * node,  const std::string & childName)
{
	const XMLCh * xmlNamespaceURI = node->getNamespaceURI();
	XMLCh xmlElementName[childName.length()+1];
	XMLString::transcode(childName.c_str(), xmlElementName, childName.length());
	DOMElement * newElement = node->getOwnerDocument()->createElementNS(xmlNamespaceURI, xmlElementName);
	return (DOMElement *) node->appendChild(newElement);
}

DOMElement * XMLTool::appendChildElement(DOMNode * node,  const std::string & childName, const std::string & childNamespace)
{
	XMLCh xmlNamespaceURI[childNamespace.length()+1];
	XMLString::transcode(childNamespace.c_str(), xmlNamespaceURI, childNamespace.length());
	XMLCh xmlElementName[childName.length()+1];
	XMLString::transcode(childName.c_str(), xmlElementName, childName.length());
	DOMElement * newElement = node->getOwnerDocument()->createElementNS(xmlNamespaceURI, xmlElementName);
	return (DOMElement *) node->appendChild(newElement);
}

DOMElement * XMLTool::getChildElementByTagNameNS(DOMNode * node, const std::string & childName, const std::string & childNamespace)
{
	XMLCh xmlChildName[childName.length()+1];
	XMLString::transcode(childName.c_str(), xmlChildName, childName.length());
	XMLCh xmlNamespaceURI[childNamespace.length()+1];
	XMLString::transcode(childNamespace.c_str(), xmlNamespaceURI, childNamespace.length());
	DOMNodeList * nl = node->getChildNodes();
	for (int i=0, max=nl->getLength(); i<max; i++) {
		DOMNode * n = nl->item(i);
		if (n->getNodeType() == DOMNode::ELEMENT_NODE
		  && XMLString::equals(n->getNodeName(), xmlChildName)
		  && XMLString::equals(n->getNamespaceURI(), xmlNamespaceURI)) {
			return (DOMElement *) n;
		}
	}
	return NULL;
}

DOMElement * XMLTool::needChildElementByTagNameNS(DOMNode * node, const std::string & childName, const std::string & childNamespace)
throw(MessageFormatException)
{
	DOMElement * e = getChildElementByTagNameNS(node, childName, childNamespace);
	if (e == NULL) {
		const std::string nodeNamespace = getNamespaceURI(node);
		bool sameNamespace = nodeNamespace == childNamespace;
		throw MessageFormatException("Node '"+getNodeName(node)+"' in namespace '"+
					nodeNamespace+" needs to have a child '"+childName+"' in "+
					(sameNamespace ? "the same namespace" : "namespace '"+childNamespace+"'"));
	}
	return e;
}

std::list<DOMElement *> * XMLTool::getChildrenByTagNameNS(DOMNode * node, const std::string & childName, const std::string & childNamespace)
throw(MessageFormatException)
{
	std::list<DOMElement *> * children = new std::list<DOMElement *>();
	XMLCh xmlChildName[childName.length()+1];
	XMLString::transcode(childName.c_str(), xmlChildName, childName.length());
	XMLCh xmlNamespaceURI[childNamespace.length()+1];
	XMLString::transcode(childNamespace.c_str(), xmlNamespaceURI, childNamespace.length());
	DOMNodeList * nl = node->getChildNodes();
	for (int i=0, max=nl->getLength(); i<max; i++) {
		DOMNode * n = nl->item(i);
		if (n->getNodeType() == DOMNode::ELEMENT_NODE
		  && XMLString::equals(n->getNodeName(), xmlChildName)
		  && XMLString::equals(n->getNamespaceURI(), xmlNamespaceURI)) {
			children->push_back((DOMElement *)n);
		}
	}
	return children;
}


const std::string XMLTool::getNamespaceURI(DOMNode * node)
{
	return transcode(node->getNamespaceURI());
}

const std::string XMLTool::getNodeName(DOMNode * node)
{
	return transcode(node->getNodeName());
}

const std::string XMLTool::getTagName(DOMElement * e)
{
	return transcode(e->getTagName());
}


const std::string XMLTool::getAttribute(DOMElement * e, const std::string & attributeName)
{
	XMLCh xmlAttributeName[attributeName.length()+1];
	XMLString::transcode(attributeName.c_str(), xmlAttributeName, attributeName.length());
	return transcode(e->getAttribute(xmlAttributeName));
}

const std::string XMLTool::needAttribute(DOMElement * e, const std::string & attributeName)
throw(MessageFormatException)
{
	XMLCh xmlAttributeName[attributeName.length()+1];
	XMLString::transcode(attributeName.c_str(), xmlAttributeName, attributeName.length());
	if (!e->hasAttribute(xmlAttributeName)) {
		throw MessageFormatException("Element '"+getTagName(e)+"' in namespace '"+
					getNamespaceURI(e)+"' needs an attribute '"+attributeName+"'");
	}
	return transcode(e->getAttribute(xmlAttributeName));
}


void XMLTool::setAttribute(DOMElement * e, const std::string & attribute, const std::string & value)
{
	XMLCh attributeName[attribute.length()+1];
	XMLString::transcode(attribute.c_str(), attributeName, attribute.length());
	XMLCh attributeValue[value.length()+1];
	XMLString::transcode(value.c_str(), attributeValue, value.length());
	e->setAttribute(attributeName, attributeValue);
}



const std::string XMLTool::transcode(const XMLCh * xmlString)
{
	if (xmlString == NULL) return std::string("");
	int len = XMLString::stringLen(xmlString);
	char chars[2*len+1]; // one XML char can be several bytes in native encoding
	XMLString::transcode(xmlString, chars, 2*len);
	return std::string(chars);
}








} // namespace util
} // namespace semaine

