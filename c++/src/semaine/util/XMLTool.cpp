/*
 *  XMLTool.cpp
 *  semaine
 *
 *  Created by Marc Schröder on 03.11.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#include "XMLTool.h"

using namespace XERCES_CPP_NAMESPACE;


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
	XMLCh * xmlNamespaceURI = XMLString::transcode(aNamespace.c_str());
	XMLCh * xmlQualifiedName = XMLString::transcode(rootTagname.c_str());
	DOMImplementation * dom = getDOMImplementation();
	DOMDocument * doc = dom->createDocument(xmlNamespaceURI, xmlQualifiedName, NULL);
	XMLString::release(&xmlNamespaceURI);
	XMLString::release(&xmlQualifiedName);
	return doc;
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
	XMLCh * xmlElementName = XMLString::transcode(elementName.c_str());
	DOMElement * e = doc->createElementNS(xmlNamespaceURI, xmlElementName);
	XMLString::release(&xmlElementName);
	return e;
}

DOMElement * XMLTool::createElement(DOMDocument * doc, const std::string & elementName, const std::string & aNamespace)
{
	XMLCh * xmlNamespaceURI = XMLString::transcode(aNamespace.c_str());
	XMLCh * xmlElementName = XMLString::transcode(elementName.c_str());
	DOMElement * e = doc->createElementNS(xmlNamespaceURI, xmlElementName);
	XMLString::release(&xmlNamespaceURI);
	XMLString::release(&xmlElementName);
	return e;
}

DOMText * XMLTool::createTextNode(DOMDocument * doc, const std::string & textContent)
{
	XMLCh * xmlTextContent = XMLString::transcode(textContent.c_str());
	DOMText * t = doc->createTextNode(xmlTextContent);
	XMLString::release(&xmlTextContent);
	return t;
}

DOMElement * XMLTool::appendChildElement(DOMNode * node,  const std::string & childName)
{
	const XMLCh * xmlNamespaceURI = node->getNamespaceURI();
	XMLCh * xmlElementName = XMLString::transcode(childName.c_str());
	DOMElement * newElement = node->getOwnerDocument()->createElementNS(xmlNamespaceURI, xmlElementName);
	DOMElement * e = (DOMElement *) node->appendChild(newElement);
	XMLString::release(&xmlElementName);
	return e;
}

DOMElement * XMLTool::appendChildElement(DOMNode * node,  const std::string & childName, const std::string & childNamespace)
{
	XMLCh * xmlNamespaceURI = XMLString::transcode(childNamespace.c_str());
	XMLCh * xmlElementName = XMLString::transcode(childName.c_str());
	DOMElement * newElement = node->getOwnerDocument()->createElementNS(xmlNamespaceURI, xmlElementName);
	DOMElement * e = (DOMElement *) node->appendChild(newElement);
	XMLString::release(&xmlNamespaceURI);
	XMLString::release(&xmlElementName);
	return e;
}

DOMText * XMLTool::appendChildTextNode(DOMNode * node, const std::string & textContent)
{
	return (DOMText *) node->appendChild(createTextNode(node->getOwnerDocument(), textContent));
}


const std::string XMLTool::getTextContent(DOMNode * node)
{
	return transcode(node->getTextContent());
}


DOMElement * XMLTool::getChildElementByTagNameNS(DOMNode * node, const std::string & childName, const std::string & childNamespace)
{
	XMLCh * xmlChildName = XMLString::transcode(childName.c_str());
	XMLCh * xmlNamespaceURI = XMLString::transcode(childNamespace.c_str());
	DOMNodeList * nl = node->getChildNodes();
	DOMElement * e = NULL;
	for (int i=0, max=nl->getLength(); i<max; i++) {
		DOMNode * n = nl->item(i);
		if (n->getNodeType() == DOMNode::ELEMENT_NODE
		  && XMLString::equals(n->getNodeName(), xmlChildName)
		  && XMLString::equals(n->getNamespaceURI(), xmlNamespaceURI)) {
			e = (DOMElement *) n;
			break;
		}
	}
	XMLString::release(&xmlChildName);
	XMLString::release(&xmlNamespaceURI);
	return e;
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
	XMLCh * xmlChildName = XMLString::transcode(childName.c_str());
	XMLCh * xmlNamespaceURI = XMLString::transcode(childNamespace.c_str());
	DOMNodeList * nl = node->getChildNodes();
	for (int i=0, max=nl->getLength(); i<max; i++) {
		DOMNode * n = nl->item(i);
		if (n->getNodeType() == DOMNode::ELEMENT_NODE
		  && XMLString::equals(n->getNodeName(), xmlChildName)
		  && XMLString::equals(n->getNamespaceURI(), xmlNamespaceURI)) {
			children->push_back((DOMElement *)n);
		}
	}
	XMLString::release(&xmlChildName);
	XMLString::release(&xmlNamespaceURI);
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
	XMLCh * xmlAttributeName = XMLString::transcode(attributeName.c_str());
	std::string s = transcode(e->getAttribute(xmlAttributeName));
	XMLString::release(&xmlAttributeName);
	return s;
}

const std::string XMLTool::needAttribute(DOMElement * e, const std::string & attributeName)
throw(MessageFormatException)
{
	XMLCh * xmlAttributeName = XMLString::transcode(attributeName.c_str());
	if (!e->hasAttribute(xmlAttributeName)) {
		throw MessageFormatException("Element '"+getTagName(e)+"' in namespace '"+
					getNamespaceURI(e)+"' needs an attribute '"+attributeName+"'");
	}
	std::string s = transcode(e->getAttribute(xmlAttributeName));
	XMLString::release(&xmlAttributeName);
	return s;
}


void XMLTool::setAttribute(DOMElement * e, const std::string & attributeName, const std::string & value)
{
	XMLCh * xmlAttributeName = XMLString::transcode(attributeName.c_str());
	XMLCh * xmlAttributeValue = XMLString::transcode(value.c_str());
	e->setAttribute(xmlAttributeName, xmlAttributeValue);
	XMLString::release(&xmlAttributeName);
	XMLString::release(&xmlAttributeValue);
}



const std::string XMLTool::transcode(const XMLCh * xmlString)
{
	if (xmlString == NULL) return std::string("");
	char * chars =XMLString::transcode(xmlString);
	std::string s = std::string(chars);
	XMLString::release(&chars);
	return s;
}







} // namespace util
} // namespace semaine

