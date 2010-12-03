/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.util;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.StringReader;
import java.io.StringWriter;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import javax.xml.XMLConstants;
import javax.xml.namespace.NamespaceContext;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.Source;
import javax.xml.transform.Templates;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.URIResolver;
import javax.xml.transform.dom.DOMResult;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;
import javax.xml.transform.stream.StreamSource;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.w3c.dom.DOMConfiguration;
import org.w3c.dom.DOMException;
import org.w3c.dom.DOMImplementation;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.w3c.dom.bootstrap.DOMImplementationRegistry;
import org.w3c.dom.ls.DOMImplementationLS;
import org.w3c.dom.ls.LSOutput;
import org.w3c.dom.ls.LSSerializer;
import org.w3c.dom.traversal.DocumentTraversal;
import org.w3c.dom.traversal.NodeFilter;
import org.w3c.dom.traversal.NodeIterator;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;

import eu.semaine.datatypes.xml.EMMA;
import eu.semaine.datatypes.xml.EmotionML;
import eu.semaine.datatypes.xml.SemaineML;
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
    private static DocumentBuilder docBuilder = null;
    private static Log log = LogFactory.getLog(XMLTool.class);


    // Static constructor:
    static {
        try{
            DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
            factory.setExpandEntityReferences(true);
            factory.setNamespaceAware(true);
            docBuilder = factory.newDocumentBuilder();
            dom = docBuilder.getDOMImplementation();
        } catch (ParserConfigurationException e) {
            log.error("Cannot start up XML parser", e);
        }
    }
    

    public static synchronized Document string2Document(String stringRepresentationOfDocument)
    throws SAXException, IOException
    {
    	return docBuilder.parse(new InputSource(new StringReader(stringRepresentationOfDocument)));
    }
    
    public static synchronized Document parse(File file)
    throws SAXException, IOException
    {
    	return docBuilder.parse(file);
    }
    
    public static synchronized Document parse(InputStream in)
    throws SAXException, IOException
    {
    	return docBuilder.parse(in);
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
     * If node has a namespace prefix, the new element will also use that namespace prefix.
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
	    Element child = (Element) node.appendChild(createElement(node.getOwnerDocument(), childName, node.getNamespaceURI()));
	    String parentPrefix = node.getPrefix();
	    if (parentPrefix != null) {
	    	child.setPrefix(parentPrefix);
	    }
	    return child;
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
	 * Get the direct child of node that is an element with the given
	 * local name and namespace.
	 * @param node
	 * @param childName the child's local name, without a namespace prefix.
	 * @param childNamespace
	 * @return the child element, or null if there is no such child.
	 */
	public static Element getChildElementByLocalNameNS(Node node, String childName, String childNamespace)
	{
		NodeList nl = node.getChildNodes();
		for (int i=0, max=nl.getLength(); i<max; i++) {
			Node n = nl.item(i);
			if (n instanceof Element
					&& n.getLocalName().equals(childName)
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
	 * Same as {@link #getChildElementByLocalNameNS(Node, String, String)}, but
	 * throw a MessageFormatException if there is no such child element.
	 * @param node
	 * @param childName
	 * @param childNamespace
	 * @return a non-null child element
	 * @throws MessageFormatException if there is no such child, 
	 * i.e. when getChildElementByTagNameNS() would return null.
	 */
	public static Element needChildElementByLocalNameNS(Node node, String childName, String childNamespace)
	throws MessageFormatException
	{
		Element e = getChildElementByLocalNameNS(node, childName, childNamespace);
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
	 * Get a list of all direct children with the given local name and namespace.
	 * Whereas getChildElementByTagNameNS() returns the single first child,
	 * this method returns all the children that match.
	 * @param node
	 * @param childName the child's local name
	 * @param childNamespace
	 * @return a list containing the children that match, or an empty list if none match.
	 * @throws MessageFormatException
	 */
	public static List<Element> getChildrenByLocalNameNS(Node node, String childName, String childNamespace)
	{
		List<Element> list = new ArrayList<Element>();
		Element e = getChildElementByLocalNameNS(node, childName, childNamespace);
		if (e != null) {
			list.add(e);
			Node n = e;
			while ((n = n.getNextSibling()) != null) {
				if (n.getNodeType() == Node.ELEMENT_NODE &&
						isSameNamespace(n.getNamespaceURI(), childNamespace) &&
						n.getLocalName().equals(childName)) {
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
	 * Merge two XML files using XSLT
	 * @param xmlFileContent1, first XML file content 
	 * @param xmlFileContent2, second XML file content
	 * @param mergingStylesheet, the XSLT style sheet merging xml2 into xml1.
	 * @param refCodeName, code name used in xsl sheet to refer xmlFile2 (example: semaine.mary.intonation ) 
	 * @return output of merged xml file
	 * @throws Exception
	 * @throws FileNotFoundException
	 */
	public static Document mergeTwoXMLFiles(Document xmlFileContent1, final Document xmlFileContent2, Templates mergingStylesheet, final String refCodeName) throws Exception,FileNotFoundException
    {
	     
        DOMSource xml1Source = new DOMSource(xmlFileContent1);
        DOMResult mergedDR = new DOMResult();
        // Transformer is not guaranteed to be thread-safe -- therefore, we
        // need one per thread.
        Transformer mergingTransformer = mergingStylesheet.newTransformer();
        mergingTransformer.setURIResolver(new URIResolver() {
            public Source resolve(String href, String base) {
                if (href == null) {
                    return null;
                } else if (href.equals(refCodeName)) {
                	return (new DOMSource(xmlFileContent2));
				} else {
                    return null;
                }
            }
        });

        mergingTransformer.transform(xml1Source, mergedDR);
        return (Document) mergedDR.getNode();
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
	

	/**
	 * Create a node iterator for the current document, which will provide exactly
	 * the elements below root in the given namespace whose local names are in localNames.
	 * This is independent of any namespace prefixes.
	 * @param doc
	 * @param root
	 * @param namespaceURI
	 * @param localNames the array of local names to be accepted.
	 * @return
	 */
	public static NodeIterator createNodeIterator(Document doc, Node root, final String namespaceURI, final String... localNames) {
		NodeIterator ni = ((DocumentTraversal)doc).createNodeIterator(root, NodeFilter.SHOW_ELEMENT, 
				new NodeFilter() {
					@Override
					public short acceptNode(Node n) {
						if (!isSameNamespace(n.getNamespaceURI(), namespaceURI)) {
							return NodeFilter.FILTER_SKIP;
						}
						for (String localName : localNames) {
							if (localName.equals(n.getLocalName())) {
								return NodeFilter.FILTER_ACCEPT;
							}
						}
						return NodeFilter.FILTER_SKIP;
					}
		}, false);
		return ni;
	}
	
	/**
	 * Create an XML document from the given XPath expression.
	 * The given string is interpreted as a limited subset of XPath expressions and split into parts.
	 * Each part except the last one is expected to follow precisely the following form:
	 * <code>"/" ( prefix ":" ) ? localname ( "[" "@" attName "=" "'" attValue "'" "]" ) ?</code>
	 * The last part must be either
	 * <code> "/" "text()"</code>
	 * or
	 * </code> "/" "@" attributeName </code>.
	 * @param xpathExpression an xpath expression from which the given document can be created. must not be null.
	 * @param value the value to insert at the location identified by the xpath expression. if this is null, the empty string is added.
	 * @param namespaceContext the namespace context to use for resolving namespace prefixes.
	 *  If this is null, the namespace context returned by {@link #getDefaultNamespaceContext()} will be used.
	 * @param document if not null, the xpath expression + value pair will be added to the document. 
	 * If null, a new document will be created from the xpathExpression and value pair.
	 * @return a document containing the given information
	 * @throws NullPointerException if xpathExpression is null.
	 * @throws IllegalArgumentException if the xpath expression is not valid, or if the xpath expression is incompatible with the given document (e.g., different root node) 
	 */
	public static Document xpath2doc(String xpathExpression, String value, NamespaceContext namespaceContext, Document document)  throws NullPointerException, IllegalArgumentException {
		if (xpathExpression == null) {
			throw new NullPointerException("null argument");
		}
		if (value == null) {
			value = "";
		}
		if (namespaceContext == null) {
			namespaceContext = getDefaultNamespaceContext();
		}
		String[][] parts = splitXPathIntoParts(xpathExpression);
		Element currentElt = null;

		for (int i=0; i<parts.length-1; i++) {
			String[] part = parts[i];
			assert part != null;
			assert part.length == 4;
			String prefix = part[0];
			String localName = part[1];
			String attName = part[2];
			String attValue = part[3];
			String namespaceURI = prefix != null ? namespaceContext.getNamespaceURI(prefix) : null;
			if (prefix != null && namespaceURI.equals("")) {
				throw new IllegalArgumentException("Unknown prefix: "+prefix);
			}
			// Now traverse to or create element defined by prefix, localName and namespaceURI
			if (currentElt == null) { // at top level
				if (document == null) { // create a new document
					try {
						document = XMLTool.newDocument(localName, namespaceURI);
					} catch (DOMException de) {
						throw new IllegalArgumentException("Cannot create document for localname '"+localName+"' and namespaceURI '"+namespaceURI+"'", de);
					}
					currentElt = document.getDocumentElement();
					currentElt.setPrefix(prefix);
				} else {
					currentElt = document.getDocumentElement();
					if (!currentElt.getLocalName().equals(localName)) {
						throw new IllegalArgumentException("Incompatible root node specification: expression requests '"+
								localName+"', but document already has '"+currentElt.getLocalName()+"'!");
					}
					String currentNamespaceURI = currentElt.getNamespaceURI();
					if (!(currentNamespaceURI == null && namespaceURI == null
						 || currentNamespaceURI != null && currentNamespaceURI.equals(namespaceURI))) {
						throw new IllegalArgumentException("Incompatible root namespace specification: expression requests '"+
								namespaceURI+"', but document already has '"+currentNamespaceURI+"'!");
					}
				}
			} else { // somewhere in the tree
				// First check if the requested child already exists
				List<Element> sameNameChildren = XMLTool.getChildrenByLocalNameNS(currentElt, localName, namespaceURI);
				boolean found = false;
				if (attName == null) {
					if (sameNameChildren.size() > 0) {
						currentElt = sameNameChildren.get(0);
						found = true;
					}
				} else {
					for (Element c : sameNameChildren) {
						if (c.hasAttribute(attName)) {
							if (attValue == null || attValue.equals(c.getAttribute(attName))) {
								currentElt = c;
								found = true;
								break;
							}
						}
					}
				}
				if (!found) { // need to create it
					currentElt = XMLTool.appendChildElement(currentElt, localName, namespaceURI);
					if (prefix != null) currentElt.setPrefix(prefix);
					if (attName != null) {
						currentElt.setAttribute(attName, attValue != null ? attValue : "");
					}
				}
			}


		}
		if (currentElt == null) {
			throw new IllegalArgumentException("No elements or no final part created from XPath expression '"+xpathExpression+"'");
		}
		String[] lastPart = parts[parts.length-1];
		assert lastPart.length <= 1;
		if (lastPart.length == 0) { // text content of the given node
			currentElt.setTextContent(value);
		} else {
			String attName = lastPart[0];
			currentElt.setAttribute(attName, value);
		}
		return document;
	}

	/**
	 * The given string is interpreted as a limited subset of XPath expressions and split into parts.
	 * Each part except the last one is expected to follow precisely the following form:
	 * <code>"/" ( prefix ":" ) ? localname ( "[" "@" attName "=" "'" attValue "'" "]" ) ?</code>
	 * The last part must be either
	 * <code> "/" "text()"</code>
	 * or
	 * </code> "/" "@" attributeName </code>.
	 * @param expr the string to be split as an xpath expression
	 * @return an array of string arrays. All except the last entry in this array is guaranteed to have
	 * four elements, with the meaning:
	 * <ul>
	 * <li>[0]: prefix (can be null)</li>
	 * <li>[1]: localname (guaranteed not to be null)</li>
	 * <li>[2]: attributeName (can be null)</li>
	 * <li>[3]: attributeValue (is guaranteed to be null when attributeName is null; 
	 *  when attributeName is non-null, and attributeValue is null,
	 *  then the attribute must be present but there are no constraints about its value)</li>
	 * </ul>
	 * The last element in the returned array has either length 1 or length 0. If it is of length 0, then the XPath expression ended in "text()",
	 * i.e. the value to be referenced is the text content of the enclosing Element; if it is of length 1, the String
	 * contained is guaranteed to be non-null and represents the name of the attribute to be referenced on the enclosing Element.
	 * 
	 * @throws IllegalArgumentException if expr does not match the expected format.
	 */
	public static String[][] splitXPathIntoParts(String expr) throws IllegalArgumentException {
		ArrayList<String[]> parts = new ArrayList<String[]>();
		if (!expr.startsWith("/")) {
			throw new IllegalArgumentException("XPath expression does not start with a slash: "+expr);
		}
		int pos = 1;
		// Structure of each part except the last part:
		// "/" ( prefix ":" ) ? localname ( "[" "@" attName "=" "'" attValue "'" "]" ) ?
		// Avoid regular expression code so it is easier to port to C++.
		// Strategy: 
		// find first of ":[/"; 
		// - if ":", detach prefix, find first of "[/", and continue in next line:
		// - if "/", there is no attribute;
		// - if "[", require "@", find "=" followed by "'", then find next "'", require "]" and "/".
		while (true) { // We loop until we cannot match a '/' anymore, because last part is different
			String prefix = null;
			String localname = null;
			String attName = null;
			String attValue = null;
			int nextColonPos = expr.indexOf(':', pos);
			int nextSlashPos = expr.indexOf('/', pos);
			int nextOpenSqB = expr.indexOf('[', pos);
			if (nextSlashPos == -1) {
				break; // pos is start of last part
			}
			if (nextColonPos != -1 && nextColonPos < nextSlashPos) {
				prefix = expr.substring(pos, nextColonPos);
				pos = nextColonPos + 1;
			}
			// Attributes?
			if (nextOpenSqB != -1 && nextOpenSqB < nextSlashPos) {
				if (nextOpenSqB <= pos) {
					throw new IllegalArgumentException("Wrong square bracket location in XPath expression "+expr);
				}
				localname = expr.substring(pos, nextOpenSqB);
				if (expr.charAt(nextOpenSqB+1)!='@') {
					throw new IllegalArgumentException("Expected '@' character after '[' in XPath expression "+expr);
				}
				int equalPos = expr.indexOf('=', nextOpenSqB);
				int nextCloseSqB = expr.indexOf(']', nextOpenSqB);
				if (equalPos != -1 && equalPos+2<nextCloseSqB) {
					// we have an attribute value
					attName = expr.substring(nextOpenSqB+2, equalPos);
					if (expr.charAt(equalPos+1) != '\'' || expr.charAt(nextCloseSqB-1) != '\'') {
						throw new IllegalArgumentException("Attribute value for attribute '"+attName+"' must be in single quotes, in XPath expression "+expr);
					}
					attValue = expr.substring(equalPos+2, nextCloseSqB-1);
				} else { // only attribute name, no value
					attName = expr.substring(nextOpenSqB+2, nextCloseSqB);
				}
				nextSlashPos = nextCloseSqB+1;
				if (nextSlashPos >= expr.length() || expr.charAt(nextSlashPos) != '/') {
					throw new IllegalArgumentException("XPath expression seems malformed: no slash after closed square bracket: "+expr);
				}
			} else { // no attribute
				localname = expr.substring(pos, nextSlashPos);
				// attName and attValue stay null
			}
			if (localname.isEmpty()) {
				throw new IllegalArgumentException("Found empty localname in expression: "+expr);
			}
			parts.add(new String[] {prefix, localname, attName, attValue});
			pos = nextSlashPos + 1;
		}
		// Last part: expect either 'text()' or '@attributeName'
		if (pos >= expr.length()) {
			throw new IllegalArgumentException("XPath expression is expected to contain as final part either 'text()' or '@attributeName': "+expr);
		}
		if (expr.charAt(pos) == '@') {
			String attName = expr.substring(pos+1);
			parts.add(new String[] {attName});
		} else if (expr.substring(pos).equals("text()")) {
			parts.add(new String[0]);
		} else {
			throw new IllegalArgumentException("XPath expression is expected to contain as final part either 'text()' or '@attributeName': "+expr);
		}
		return (String[][]) parts.toArray(new String[0][]);
	}
	
	/**
	 * Create a namespace context from the given mapping bewteen prefixes and namespace uris.
	 * @param prefixes2NamespaceURIs
	 * @return a namespace context
	 */
	public static NamespaceContext createNamespaceContext(final Map<String, String> prefixes2NamespaceURIs) {
		return new NamespaceContext() {
		    public String getNamespaceURI(String prefix) {
		        if (prefix == null) throw new NullPointerException("Null prefix");
		        else if (prefixes2NamespaceURIs.containsKey(prefix)) return prefixes2NamespaceURIs.get(prefix);
		        else if ("xml".equals(prefix)) return XMLConstants.XML_NS_URI;
		        return XMLConstants.NULL_NS_URI;
		    }
		    // This method isn't necessary for XPath processing.
		    public String getPrefix(String uri) {
		        throw new UnsupportedOperationException();
		    }
		    // This method isn't necessary for XPath processing either.
		    public Iterator<Object> getPrefixes(String uri) {
		        throw new UnsupportedOperationException();
		    }
		};
	}
	
	/**
	 * Get a default namespace context. Its namespace definitions include at least the prefixes "semaine", "emma" and "emotion". 
	 * @return
	 */
	public static NamespaceContext getDefaultNamespaceContext() {
		Map<String, String> m = new HashMap<String, String>();
		m.put("semaine", SemaineML.namespaceURI);
		m.put("emma", EMMA.namespaceURI);
		m.put("emotion", EmotionML.namespaceURI);
		return createNamespaceContext(m);
	}
	
}
