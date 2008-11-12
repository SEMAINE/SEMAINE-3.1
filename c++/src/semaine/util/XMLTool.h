/*
 *  XMLTool.h
 *  semaine
 *
 *  Created by Marc Schröder on 03.11.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#ifndef SEMAINE_UTIL_XMLTOOL_H
#define SEMAINE_UTIL_XMLTOOL_H


#include <semaine/config.h>

#include <semaine/cms/exceptions/MessageFormatException.h>

#include <list>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#if defined(XERCES_NEW_IOSTREAMS)
  #include <iostream>
#else
  #include <iostream.h>
#endif


using namespace semaine::cms::exceptions;

namespace semaine {
namespace util {

class XMLTool
{
public:
	/**
	 * This needs to be called once before any of the methods here can be used.
	 */
	static void startupXMLTools();
	/**
	 * This should be called to clean up after the code is done.
	 */
	static void shutdownXMLTools();

	/**
	 * Get a DOM implementation.
	 */
	static XERCES_CPP_NAMESPACE::DOMImplementation * getDOMImplementation();

	/**
	 * Convert a string in XML representation into a standard string. If xmlString is NULL,
	 * an empty string is returned.
	 */
	static const std::string transcode(const XMLCh * xmlString);

	static XERCES_CPP_NAMESPACE::DOMDocument * parse(const std::string & xmlAsText);

    /**
     * Create a new document with the given name and namespace for the root element.
     * @param rootTagname
     * @param namespace the namespace URI, e.g. <code>http://www.w3.org/2003/04/emma</code>,
     * or null if no namespace is to be associated with the new element.
     * @return
     */
	static XERCES_CPP_NAMESPACE::DOMDocument * newDocument(const std::string & rootTagname, const std::string & aNamespace);

    /**
     * Create a new document with the given name and namespace for the root element,
     * and set the 'version' attribute of the root element to the given value.
     * @param rootTagname
     * @param namespace the namespace URI, e.g. <code>http://www.w3.org/2003/04/emma</code>,
     * or null if no namespace is to be associated with the new element.
     * @param version a value to add to the 'version' attribute of the root element
     * @return
     */
	static XERCES_CPP_NAMESPACE::DOMDocument * newDocument(const std::string & rootTagname, const std::string & aNamespace, const std::string & version);

    /**
     * In the given document, create a new element of the given name,
     * with the same namespace as the document element.
     * @param doc a document
     * @param elementName the name of the new element
     * @return an element which is not yet included in the tree hierarchy of the document.
     */
	static XERCES_CPP_NAMESPACE::DOMElement * createElement(XERCES_CPP_NAMESPACE::DOMDocument * doc, const std::string & elementName);


    /**
     * In the given document, create a new element with the given name and
     * the given namespace.
     * @param doc 
     * @param elementName
     * @param namespace the namespace URI, e.g. <code>http://www.w3.org/2003/04/emma</code>,
     * or null if no namespace is to be associated with the new element.
     * @return an element which is not yet included in the tree hierarchy of the document.
     * @throws NullPointerException if doc or elementName is null.
     */
	static XERCES_CPP_NAMESPACE::DOMElement * createElement(XERCES_CPP_NAMESPACE::DOMDocument * doc, const std::string & elementName, const std::string & aNamespace);

	/**
     * Create a child element with the given name and append it below node.
     * The new element will have the same namespace as node.
     * @param node
     * @param childName
     * @return the child element
     */
	 static XERCES_CPP_NAMESPACE::DOMElement * appendChildElement(XERCES_CPP_NAMESPACE::DOMNode * node, const std::string & childName);
	 
	/**
     * Create a child element with the given name and namespace, and append it below node.
     * @param node
     * @param childName
     * @param childNamespace the namespace of the child, or NULL if no namespace is desired.
     * @return the child element
     */
	static XERCES_CPP_NAMESPACE::DOMElement * appendChildElement(XERCES_CPP_NAMESPACE::DOMNode * node, const std::string & childName, const std::string & childNamespace);


	/**
	 * For the given element, return the value of the given attribute if it exists,
	 * or an empty string if it doesn't exist.
	 * @param e the element whose attribute to return
	 * @param attributeName the name of the attribute to look up.
	 * @return the String value of the attribute if it exists, or an empty string
	 */
	 static const std::string getAttribute(XERCES_CPP_NAMESPACE::DOMElement * e, const std::string & attributeName);

	/**
	 * For the given element, return the value of the given attribute if it exists,
	 * or complain with a MessageFormatException if it doesn't exist.
	 * @param e the element whose attribute to return
	 * @param attributeName the name of the attribute to look up.
	 * @return the String value of the attribute if it exists
	 * @throws MessageFormatException if the attribute doesn't exist.
	 */
	 static const std::string needAttribute(XERCES_CPP_NAMESPACE::DOMElement * e, const std::string & attributeName)
	 throw(MessageFormatException);

	/**
	 * For the given element, set the attribute to the value.
	 */
	static void setAttribute(XERCES_CPP_NAMESPACE::DOMElement * e, const std::string & attribute, const std::string & value);
	 
	/**
	 * Get the namespace of the given node, or the empty string if the node has no namespace.
	 */ 
	static const std::string getNamespaceURI(XERCES_CPP_NAMESPACE::DOMNode * node);
	
	/**
	 * Get the node name of the given node.
	 */ 
	static const std::string getNodeName(XERCES_CPP_NAMESPACE::DOMNode * node);
	
	/**
	 * Get the tag name of the given element.
	 */ 
	static const std::string getTagName(XERCES_CPP_NAMESPACE::DOMElement * e);

	/**
	 * Get the direct child of node that is an element with the given
	 * tag name and namespace.
	 * @param node
	 * @param childName
	 * @param childNamespace
	 * @return the child element, or null if there is no such child.
	 */
	static XERCES_CPP_NAMESPACE::DOMElement * getChildElementByTagNameNS(XERCES_CPP_NAMESPACE::DOMNode * node, const std::string & childName, const std::string & childNamespace);

	/**
	 * Same as {@link #getChildElementByTagNameNS(Node, String, String)}, but
	 * throw a MessageFormatException if there is no such child element.
	 * @param node
	 * @param childName
	 * @param childNamespace
	 * @return a non-null child element
	 * @throws MessageFormatException if there is no such child, 
	 * i.e. when getChildElementByTagNameNS() would return null.
	 */
	static XERCES_CPP_NAMESPACE::DOMElement * needChildElementByTagNameNS(XERCES_CPP_NAMESPACE::DOMNode * node, const std::string & childName, const std::string & childNamespace)
	throw(MessageFormatException);

	/**
	 * Get a list of all direct children with the given tag name and namespace.
	 * Whereas getChildElementByTagNameNS() returns the single first child,
	 * this method returns all the children that match.
	 * The calling code must delete the list when it is no longer needed.
	 * @param node
	 * @param childName
	 * @param childNamespace
	 * @return a list containing the children that match, or an empty list if none match.
	 * @throws MessageFormatException
	 */
	static std::list<XERCES_CPP_NAMESPACE::DOMElement *> * getChildrenByTagNameNS(XERCES_CPP_NAMESPACE::DOMNode * node, const std::string & childName, const std::string & childNamespace)
	 throw(MessageFormatException);

private:
	static 	XERCES_CPP_NAMESPACE::XercesDOMParser * parser;
	
};

} // namespace util
} // namespace semaine



#endif

