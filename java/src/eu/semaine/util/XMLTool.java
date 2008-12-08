/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.util;

import java.io.FileNotFoundException;
import java.io.InputStream;
import java.io.StringReader;
import java.io.StringWriter;
import java.util.ArrayList;
import java.util.List;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.Source;
import javax.xml.transform.Templates;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.URIResolver;
import javax.xml.transform.stream.StreamResult;
import javax.xml.transform.stream.StreamSource;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.w3c.dom.DOMConfiguration;
import org.w3c.dom.DOMImplementation;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.w3c.dom.bootstrap.DOMImplementationRegistry;
import org.w3c.dom.ls.DOMImplementationLS;
import org.w3c.dom.ls.LSOutput;
import org.w3c.dom.ls.LSSerializer;

import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.exceptions.SystemConfigurationException;


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
        Document doc = newDocument(rootTagname, namespace);
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
	public static Element needChildElementByTagNameNS(Node node, String childName, String childNamespace)
	throws MessageFormatException
	{
		Element e = getChildElementByTagNameNS(node, childName, childNamespace);
		if (e == null) {
			String nodeNamespace = node.getNamespaceURI();
			boolean sameNamespace = childNamespace != null && childNamespace.equals(nodeNamespace)
								|| childNamespace == null && nodeNamespace == null;
			throw new MessageFormatException("Node '"+node.getNodeName()+"' in namespace '"+
					nodeNamespace+" needs to have a child '"+childName+"' in "+
					(sameNamespace ? "the same namespace" : "namespace '"+childNamespace+"'"));
		}
		return e;
	}
	
	/**
	 * For the given element, return the value of the given attribute if it exists,
	 * or complain with a MessageFormatException if it doesn't exist.
	 * @param e the element whose attribute to return
	 * @param attributeName the name of the attribute to look up.
	 * @return the String value of the attribute if it exists
	 * @throws MessageFormatException if the attribute doesn't exist.
	 */
	public static String needAttribute(Element e, String attributeName)
	throws MessageFormatException
	{
		if (!e.hasAttribute(attributeName)) {
			throw new MessageFormatException("Element '"+e.getTagName()+"' in namespace '"+
					e.getNamespaceURI()+"' needs an attribute '"+attributeName+"'");
		}
		return e.getAttribute(attributeName);
	}

	/**
	 * For the given element, return the value of the given attribute if it exists,
	 * or null if it doesn't exist. Note that this is different from calling
	 * e.getAttribute(), which will return the empty string both if the attribute
	 * doesn't exist and if it exists and has the empty value. 
	 * @param e
	 * @param attributeName
	 * @return the String value of the attribute if it exists, or null
	 */
	public static String getAttributeIfAvailable(Element e, String attributeName)
	{
		if (!e.hasAttribute(attributeName)) {
			return null;
		}
		return e.getAttribute(attributeName);
	}
	
	/**
	 * Get a list of all direct children with the given tag name and namespace.
	 * Whereas getChildElementByTagNameNS() returns the single first child,
	 * this method returns all the children that match.
	 * @param node
	 * @param childName
	 * @param childNamespace
	 * @return a list containing the children that match, or an empty list if none match.
	 * @throws MessageFormatException
	 */
	public static List<Element> getChildrenByTagNameNS(Node node, String childName, String childNamespace)
	{
		List<Element> list = new ArrayList<Element>();
		Element e = getChildElementByTagNameNS(node, childName, childNamespace);
		if (e != null) {
			list.add(e);
			Node n = e;
			while ((n = n.getNextSibling()) != null) {
				if (n.getNodeType() == Node.ELEMENT_NODE &&
						isSameNamespace(n.getNamespaceURI(), childNamespace) &&
						n.getNodeName().equals(childName)) {
					list.add((Element)n);
				}
			}
		}
		return list;
	}
	
	/**
	 * Determine whether the two namespaces are the same.
	 * @param namespaceA a string representing a namespace, or null
	 * @param namespaceB a string representing a namespace, or null
	 * @return true if both are null or both are the same string, false otherwise.
	 */
	public static boolean isSameNamespace(String namespaceA, String namespaceB)
	{
		if (namespaceA != null)
			return namespaceA.equals(namespaceB);
		else // namespaceA == null
			return namespaceB == null;
	}
	
	/**
	 * Merge two XML files using XSLT
	 * @param xmlFileContent1, first XML file content 
	 * @param xmlFileContent2, second XML file content
	 * @param xmlStyleSheet, XSL style sheet as a inputstream 
	 * @param refCodeName, code name used in xsl sheet to refer xmlFile2 (example: semaine.mary.intonation ) 
	 * @return output of merged xml file
	 * @throws Exception
	 * @throws FileNotFoundException
	 */
	public static String mergeTwoXMLFiles(String xmlFileContent1, final String xmlFileContent2, InputStream xmlStyleSheet, final String refCodeName) throws Exception,FileNotFoundException
    {
	     
	    Templates mergeXML2IntoStylesheet;
	    TransformerFactory tFactory = TransformerFactory.newInstance();
	    //StreamSource stylesheetStream = new StreamSource(new FileReader(xmlStyleSheet));
	    StreamSource stylesheetStream = new StreamSource(xmlStyleSheet);
        
        mergeXML2IntoStylesheet = tFactory.newTemplates(stylesheetStream);
        StreamSource xml1Source = new StreamSource(new StringReader(xmlFileContent1));
        StringWriter mergedWriter = new StringWriter();
        StreamResult mergedResult = new StreamResult(mergedWriter);
        // Transformer is not guaranteed to be thread-safe -- therefore, we
        // need one per thread.
        Transformer mergingTransformer = mergeXML2IntoStylesheet.newTransformer();
        mergingTransformer.setURIResolver(new URIResolver() {
            public Source resolve(String href, String base) {
                if (href == null) {
                    return null;
                } else if (href.equals(refCodeName)) {
                	return (new StreamSource(new StringReader(xmlFileContent2)));
				} else {
                    return null;
                }
            }
        });

        mergingTransformer.transform(xml1Source, mergedResult);
        return mergedWriter.toString();
    }
	
	/**
	 * Document type to String format conversion 
	 * @param document
	 * @return
	 * @throws Exception
	 * @throws FileNotFoundException
	 */
	public static String document2String(Document document) throws SystemConfigurationException
    {
		LSSerializer serializer;
		DOMImplementationLS domImplLS;
		try {
			DOMImplementation implementation = DOMImplementationRegistry.newInstance().getDOMImplementation("XML 3.0");
			domImplLS = (DOMImplementationLS) implementation.getFeature("LS", "3.0");
			serializer = domImplLS.createLSSerializer();
			DOMConfiguration config = serializer.getDomConfig();
			config.setParameter("format-pretty-print", Boolean.TRUE);
			//config.setParameter("canonical-form", Boolean.TRUE);
		} catch (Exception e) {
			throw new SystemConfigurationException("Problem instantiating XML serializer code", e);
		}
		LSOutput output = domImplLS.createLSOutput();
		output.setEncoding("UTF-8");
		StringWriter buf = new StringWriter();
		output.setCharacterStream(buf);
		serializer.write(document, output);
		return buf.toString();
    }
	

}
