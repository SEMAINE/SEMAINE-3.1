/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.util;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.w3c.dom.DOMImplementation;
import org.w3c.dom.Document;
import org.w3c.dom.DocumentType;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;


/**
 * Utility methods for working wth XML documents.
 * @author marc
 *
 */
public class XMLTool 
{
    private static DOMImplementation dom = null;
    private static Log log = LogFactory.getLog(XMLTool.class);


    // Static constructor:
    static {
        try{
            DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
            factory.setExpandEntityReferences(true);
            factory.setNamespaceAware(true);
            DocumentBuilder docBuilder = factory.newDocumentBuilder();
            dom = docBuilder.getDOMImplementation();
        } catch (ParserConfigurationException e) {
            log.error("Cannot start up XML parser", e);
        }
    }
    

    /**
     * Create a new document with the given name and namespace for the root element.
     * @param rootTagname
     * @param namespace the namespace URI, e.g. <code>http://www.w3.org/2003/04/emma</code>,
     * or null if no namespace is to be associated with the new element.
     * @return
     */
    public static Document newDocument(String rootTagname, String namespace)
    {
        return dom.createDocument(namespace, rootTagname, null);
    }
    
    /**
     * Create a new document with the given name and namespace for the root element,
     * and set the 'version' attribute of the root element to the given value.
     * @param rootTagname
     * @param namespace the namespace URI, e.g. <code>http://www.w3.org/2003/04/emma</code>,
     * or null if no namespace is to be associated with the new element.
     * @param version a value to add to the 'version' attribute of the root element
     * @return
     */
    public static Document newDocument(String rootTagname, String namespace, String version)
    {
        Document doc = newDocument(namespace, rootTagname);
        Element root = doc.getDocumentElement();
        root.setAttribute("version", version);
        return doc;
    }

    /**
     * In the given document, create a new element of the given name,
     * with the same namespace as the document element.
     * @param doc a document
     * @param elementName the name of the new element
     * @return an element which is not yet included in the tree hierarchy of the document.
     * @throws NullPointerException if doc or elementName is null.
     */
    public static Element createElement(Document doc, String elementName)
    {
    	if (doc == null)
    		throw new NullPointerException("Received null document");
    	return createElement(doc, elementName, doc.getDocumentElement().getNamespaceURI());
    }
    
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
    public static Element createElement(Document doc, String elementName, String namespace)
    {
    	if (doc == null)
    		throw new NullPointerException("Received null document");
    	if (elementName == null)
    		throw new NullPointerException("Received null elementName");
		return doc.createElementNS(namespace, elementName);
    }

    /**
     * Create a child element with the given name and append it below node.
     * The new element will have the same namespace as node.
     * @param node
     * @param childName
     * @return the child element
     */
	public static Element appendChildElement(Node node, String childName)
	{
		if (node == null)
			throw new NullPointerException("Received null node");
		if (childName == null)
			throw new NullPointerException("Received null childName");
	    return (Element) node.appendChild(createElement(node.getOwnerDocument(), childName, node.getNamespaceURI()));
	}

    /**
     * Create a child element with the given name and namespace, and append it below node.
     * @param node
     * @param childName
     * @param childNamespace the namespace of the child, or null if no namespace is desired.
     * @return the child element
     */
	public static Element appendChildElement(Node node, String childName, String childNamespace)
	{
		if (node == null)
			throw new NullPointerException("Received null node");
		if (childName == null)
			throw new NullPointerException("Received null childName");
	    return (Element) node.appendChild(createElement(node.getOwnerDocument(), childName, childNamespace));
	}

	/**
	 * Get the direct child of node that is an element with the given
	 * tag name and namespace.
	 * @param node
	 * @param childName
	 * @param childNamespace
	 * @return the child element, or null if there is no such child.
	 */
	public static Element getChildElementByTagNameNS(Node node, String childName, String childNamespace)
	{
		NodeList nl = node.getChildNodes();
		for (int i=0, max=nl.getLength(); i<max; i++) {
			Node n = nl.item(i);
			if (n instanceof Element
					&& n.getNodeName().equals(childName)
					&& n.getNamespaceURI().equals(childNamespace)) {
				return (Element) n;
			}
		}
		return null;
	}
}
