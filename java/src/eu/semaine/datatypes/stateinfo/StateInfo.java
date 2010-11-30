/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.datatypes.stateinfo;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import javax.xml.XMLConstants;
import javax.xml.namespace.NamespaceContext;
import javax.xml.xpath.XPath;
import javax.xml.xpath.XPathConstants;
import javax.xml.xpath.XPathExpression;
import javax.xml.xpath.XPathExpressionException;
import javax.xml.xpath.XPathFactory;

import org.w3c.dom.DOMException;
import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.exceptions.SystemConfigurationException;
import eu.semaine.jms.JMSLogger;
import eu.semaine.util.SEMAINEUtils;
import eu.semaine.util.XMLTool;

/**
 * A class representing one of the information states.
 * It can be created either from an XML document or from
 * a Map of information items, and can be read either as an
 * XML document or as a Map of information items. 
 * @author marc
 *
 */
public abstract class StateInfo
{
	public enum Type {AgentState, DialogState, UserState, ContextState, SystemState};
	
	
	/////////////////////////////// Static stuff /////////////////////////////////
	
	public static final String APIVersion = "0.2";
	
	public static final Map<Type, XPathInfoMapper> infosByType = getInfosByType();
	
	/**
	 * Get the short names that are defined for the given state info type.
	 * @param stateInfoType
	 * @return a set of Strings containing the short names. The iterator of this set
	 * returns the Strings in the order in which they were defined in the stateinfo.config file.
	 */
	public static Set<String> getShortNames(Type stateInfoType) {
		if (infosByType == null) return null;
		XPathInfoMapper m = infosByType.get(stateInfoType);
		if (m == null) return null;
		Map<String, String> expr = m.getExpressionMap();
		if (expr == null) return null;
		Set<String> shortNames = expr.keySet();
		if (shortNames == null) return null;
		return Collections.unmodifiableSet(expr.keySet());
	}
	
	
	private static Map<Type, XPathInfoMapper> getInfosByType()
	{
		Map<Type, List<String>> configSections = new HashMap<Type, List<String>>();
		File configFile = SEMAINEUtils.getConfigFile("semaine.stateinfo-config");
		if (configFile == null) {
			throw new Error("No character config file given in property 'semaine.stateinfo-config' -- aborting.");
		}
		BufferedReader br = null;
		try {
			br = new BufferedReader(new InputStreamReader(new FileInputStream(configFile), "UTF-8"));
			String line = null;
			Type currentType = null;
			List<String> currentSection = new ArrayList<String>();
			while ((line = br.readLine()) != null) {
				line = line.trim();
				if (line.equals("") || line.startsWith("#")) {
					continue;
				}
				if (line.startsWith("[")) {
					for (Type t : Type.values()) {
						if (line.equals("["+t.toString()+"]")) {
							if (currentType != null) {
								configSections.put(currentType, currentSection);
							}
							currentType = t;
							currentSection = new ArrayList<String>();
							break;
						}
					}
				}
				currentSection.add(line);
			}
			if (currentType != null) {
				configSections.put(currentType, currentSection);
			}
		} catch (IOException ioe) {
			throw new Error("Cannot load stateinfo config file from "+configFile.toString(), ioe);
		} finally {
			try {
				if (br != null) {
					br.close();
				}
			} catch (IOException ioe) {
				throw new Error("Cannot close stateinfo config file");
			}
		}

		Map<Type, XPathInfoMapper> t2m = new HashMap<Type, XPathInfoMapper>();
		try {
			for (Type t : Type.values()) {
				List<String> configSection = configSections.get(t);
				if (configSection == null)
					throw new SystemConfigurationException("No config section for "+t);
				t2m.put(t, getXPathExpressions(t, configSection));
			}
		} catch (SystemConfigurationException e) {
			throw new Error("Problem setting up stateinfo config", e);
		}
		return Collections.unmodifiableMap(t2m);
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
	 * @throws SystemConfigurationException if expr does not match the expected format.
	 */
	private static String[][] splitXPathIntoParts(String expr) throws SystemConfigurationException {
		ArrayList<String[]> parts = new ArrayList<String[]>();
		if (!expr.startsWith("/")) {
			throw new SystemConfigurationException("XPath expression does not start with a slash: "+expr);
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
					throw new SystemConfigurationException("Wrong square bracket location in XPath expression "+expr);
				}
				localname = expr.substring(pos, nextOpenSqB);
				if (expr.charAt(nextOpenSqB+1)!='@') {
					throw new SystemConfigurationException("Expected '@' character after '[' in XPath expression "+expr);
				}
				int equalPos = expr.indexOf('=', nextOpenSqB);
				int nextCloseSqB = expr.indexOf(']', nextOpenSqB);
				if (equalPos != -1 && equalPos+2<nextCloseSqB) {
					// we have an attribute value
					attName = expr.substring(nextOpenSqB+2, equalPos);
					if (expr.charAt(equalPos+1) != '\'' || expr.charAt(nextCloseSqB-1) != '\'') {
						throw new SystemConfigurationException("Attribute value for attribute '"+attName+"' must be in single quotes, in XPath expression "+expr);
					}
					attValue = expr.substring(equalPos+2, nextCloseSqB-1);
				} else { // only attribute name, no value
					attName = expr.substring(nextOpenSqB+2, nextCloseSqB);
				}
				nextSlashPos = nextCloseSqB+1;
				if (nextSlashPos >= expr.length() || expr.charAt(nextSlashPos) != '/') {
					throw new SystemConfigurationException("XPath expression seems malformed: no slash after closed square bracket: "+expr);
				}
			} else { // no attribute
				localname = expr.substring(pos, nextSlashPos);
				// attName and attValue stay null
			}
			parts.add(new String[] {prefix, localname, attName, attValue});
			pos = nextSlashPos + 1;
		}
		// Last part: expect either 'text()' or '@attributeName'
		if (pos >= expr.length()) {
			throw new SystemConfigurationException("XPath expression is expected to contain as final part either 'text()' or '@attributeName': "+expr);
		}
		if (expr.charAt(pos) == '@') {
			String attName = expr.substring(pos+1);
			parts.add(new String[] {attName});
		} else if (expr.substring(pos).equals("text()")) {
			parts.add(new String[0]);
		} else {
			throw new SystemConfigurationException("XPath expression is expected to contain as final part either 'text()' or '@attributeName': "+expr);
		}
		return (String[][]) parts.toArray(new String[0][]);
	}

	
	
	
	protected static XPathInfoMapper getXPathExpressions(Type typeForTable, List<String> configSection)
	throws SystemConfigurationException
	{
		// list of namespace prefixes matched with namespace URIs
		final Map<String, String> namespacePrefixes = new HashMap<String, String>();
		// To be read from same or different config file: shortcut names for info mapped to XPath expression for getting it 
		Map<String, String> shortNames = new LinkedHashMap<String, String>();
		// Temporary map to make sure no two short names have the same expression:
		Map<String, String> expr2shortname = new HashMap<String, String>();

		boolean readNamespacePrefixes = false;
		boolean readShortNames = false;

		for (String line : configSection) {
			if (line.equals("[namespace prefixes]")) {
				readNamespacePrefixes = true;
			} else if (line.equals("[short names]")) {
				readShortNames = true;
				readNamespacePrefixes = false;
				if (namespacePrefixes.isEmpty()) {
					throw new SystemConfigurationException("Namespace prefixes must be defined before short names");
				}
			} else if (readNamespacePrefixes) {
				String[] parts = line.split("[ =:]+", 2);
				if (parts.length < 2) {
					throw new SystemConfigurationException("Expected namespace prefix definition, got '"+line+"'");
				}
				String prefix = parts[0].trim();
				String namespaceURI = parts[1].trim();
				namespacePrefixes.put(prefix, namespaceURI);
			} else if (readShortNames) {
				String[] parts = line.split("[\t =:]+", 2);
				if (parts.length < 2) {
					throw new SystemConfigurationException("Expected short name to XPath definition, got '"+line+"'");
				}
				String shortName = parts[0].trim();
				String xpathExpr = parts[1].trim();
				// Fail-early strategy: we want to verify at load time whether all entries match our expected format:
				splitXPathIntoParts(xpathExpr);
				if (shortNames.containsKey(shortName)) {
					throw new SystemConfigurationException("In section "+typeForTable
					    +", short name '"+shortName+"' occurs more than once");
				}
				if (expr2shortname.containsKey(xpathExpr)) {
					throw new SystemConfigurationException("In section "+typeForTable
					    +", short names '"+shortName+"' and '"+expr2shortname.get(xpathExpr)+"' have the same XPath expression");
				}
				shortNames.put(shortName, xpathExpr);
				expr2shortname.put(xpathExpr, shortName);
			}
		}
		
		return new XPathInfoMapper(namespacePrefixes, shortNames);
	}
	
	

	
	/////////////////// Actual class stuff (non-static) /////////////////
	
	
	protected Map<String,String> info;
	protected Document doc;
	protected JMSLogger log;
	protected String stateName;
	protected Type type;

	protected StateInfo(Document doc, String whatState, String rootName, String rootNamespace, Type type)
	throws MessageFormatException
	{
		this.doc = doc;
		this.stateName = whatState;
		this.type = type;
		log = JMSLogger.getLog(whatState);
		info = new HashMap<String, String>();
		analyseDocument(rootName, rootNamespace);
	}

	public StateInfo(Map<String,String> infoItems, String whatState, Type type)
	throws IllegalArgumentException
	{
		this.stateName = whatState;
		this.type = type;
		log = JMSLogger.getLog(whatState);
		// Verify that we can handle these info items:
		Map<String, String> info2expr = infosByType.get(type).getExpressionMap();
		for (String key : infoItems.keySet()) {
			if (!info2expr.containsKey(key)) {
				throw new IllegalArgumentException("Don't know how to handle info item '"+key+"' -- something seems to be out of sync");
			}
			
		}
		info = new HashMap<String, String>(infoItems);
		
		
		// we will only create a Document from this if needed, i.e.
		// in getDocument().
	}
	
	/**
	 * Get the type of state info of this object: AgentState, DialogState, UserState, ContextState, or SystemState.
	 * @return
	 */
	public Type getType()
	{
		return type;
	}
	
	
	protected void createDocumentFromInfo()
	throws SystemConfigurationException
	{
		if (info.isEmpty()) {
			throw new SystemConfigurationException("Cannot create document from empty info");
		}
		doc = null;
		
		XPathInfoMapper xpathInfos = infosByType.get(type);
		NamespaceContext namespaceContext = xpathInfos.getNamespaceContext();
		Map<String, String> infoNames = xpathInfos.getExpressionMap();
		for (String shortName : info.keySet()) {
			String value = info.get(shortName);
			if (value == null) continue;
			Element currentElt = null;
			String expr = infoNames.get(shortName);
			if (expr == null) {
				throw new SystemConfigurationException("No info entry for '"+shortName+"' -- something seems to be out of sync");
			}
			String[][] parts = splitXPathIntoParts(expr);
			for (int i=0; i<parts.length-1; i++) {
				String[] part = parts[i];
				assert part != null;
				assert part.length == 4;
				String prefix = part[0];
				String localName = part[1];
				String attName = part[2];
				String attValue = part[3];
				String namespaceURI = prefix != null ? namespaceContext.getNamespaceURI(prefix) : null;
				// Now traverse to or create element defined by prefix, localName and namespaceURI
				if (currentElt == null) { // at top level
					if (doc == null) { // create a new document
						try {
							doc = XMLTool.newDocument(localName, namespaceURI);
						} catch (DOMException de) {
							throw new SystemConfigurationException("For info '"+shortName+"', cannot create document for localname '"+localName+"' and namespaceURI '"+namespaceURI+"'", de);
						}
						currentElt = doc.getDocumentElement();
						currentElt.setPrefix(prefix);
					} else {
						currentElt = doc.getDocumentElement();
						if (!currentElt.getLocalName().equals(localName)) {
							throw new SystemConfigurationException("Incompatible root node specification: expression for '"+shortName+"' requests '"+
									localName+"', but previous expressions have requested '"+currentElt.getLocalName()+"'!");
						}
						String currentNamespaceURI = currentElt.getNamespaceURI();
						if (!(currentNamespaceURI == null && namespaceURI == null
							 || currentNamespaceURI != null && currentNamespaceURI.equals(namespaceURI))) {
							throw new SystemConfigurationException("Incompatible root namespace specification: expression for '"+shortName+"' requests '"+
									namespaceURI+"', but previous expressions have requested '"+currentNamespaceURI+"'!");
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
				throw new SystemConfigurationException("No elements or no final part created for info '"
						+shortName+"' from XPath expression '"+expr+"'");
			}
			String[] lastPart = parts[parts.length-1];
			assert lastPart.length <= 1;
			if (lastPart.length == 0) { // text content of the given node
				currentElt.setTextContent(value);
			} else {
				String attName = lastPart[0];
				currentElt.setAttribute(attName, value);
			}
			

			
/*			
			//System.out.println("shortname: "+shortName+", expr: "+expr);
			String[] parts = expr.split("/"); 
			// Now go through all parts except the last one:
			for (int i=0; i<parts.length-1; i++) {
				String part = parts[i];
				// if there is more than one slash, indicating arbitrary substructures, we treat it like a single slash (direct child)
				if (part.equals("")) continue;
				String[] prefixAndName = part.split(Pattern.quote(":"), 2); // right-hand side can contain colon, e.g. in attribute values
//				if (prefixAndName.length > 2)
//					throw new SystemConfigurationException("Erroneous XPath expression for info '"+shortName+"': part '"+part+"'");
				String prefix = prefixAndName.length == 2 ? prefixAndName[0] : null;
				String namespaceURI = prefix != null ? namespaceContext.getNamespaceURI(prefix) : null;
				String localNameExpr = prefixAndName[prefixAndName.length - 1];
				String localName;
				String attName = null;
				String attValue = null;
				if (localNameExpr.contains("[")) { // attribute-based selection of nodes
					int openB = localNameExpr.indexOf('[');
					int closeB = localNameExpr.indexOf(']');
					if (closeB < openB)
						throw new SystemConfigurationException("Erroneous XPath expression for info '"+shortName+"': part '"+part+"'");
					localName = localNameExpr.substring(0, openB);
					String attValPair = localNameExpr.substring(openB+1, closeB);
					if (!attValPair.startsWith("@")) {
						throw new SystemConfigurationException("Cannot generate document from XPath expression for info '"+shortName+"': part '"+part+"'");
					}
					int iEqualSign = attValPair.indexOf('=');
					if (iEqualSign != -1) {
						attName = attValPair.substring(1, iEqualSign);
						attValue = attValPair.substring(iEqualSign+1);
						if (!(attValue.startsWith("'") && attValue.endsWith("'")
								|| attValue.startsWith("\"") && attValue.endsWith("\""))) {
							throw new SystemConfigurationException("For info '"+shortName+"', unexpected attribute value "+attValue+" found in part '"+part+"' -- not quoted?");
						}
						attValue = attValue.substring(1, attValue.length()-1);
					} else {
						attName = attValPair.substring(1);
						attValue = null;
					}
				} else {
					localName = localNameExpr;
				}
				// Now traverse to or create element defined by prefix, localName and namespaceURI
				if (currentElt == null) { // at top level
					if (doc == null) { // create a new document
						try {
							doc = XMLTool.newDocument(localName, namespaceURI);
						} catch (DOMException de) {
							throw new SystemConfigurationException("For info '"+shortName+"', cannot create document for localname '"+localName+"' and namespaceURI '"+namespaceURI+"'", de);
						}
						currentElt = doc.getDocumentElement();
						currentElt.setPrefix(prefix);
					} else {
						currentElt = doc.getDocumentElement();
						if (!currentElt.getLocalName().equals(localName)) {
							throw new SystemConfigurationException("Incompatible root node specification: expression for '"+shortName+"' requests '"+
									localName+"', but previous expressions have requested '"+currentElt.getLocalName()+"'!");
						}
						String currentNamespaceURI = currentElt.getNamespaceURI();
						if (!(currentNamespaceURI == null && namespaceURI == null
							 || currentNamespaceURI != null && currentNamespaceURI.equals(namespaceURI))) {
							throw new SystemConfigurationException("Incompatible root namespace specification: expression for '"+shortName+"' requests '"+
									namespaceURI+"', but previous expressions have requested '"+currentNamespaceURI+"'!");
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
			} // for all parts except the last one
			if (currentElt == null)
				throw new SystemConfigurationException("No elements or no final part created for info '"
						+shortName+"' from XPath expression '"+expr+"'");
			String lastPart = parts[parts.length-1].trim();
			if (lastPart.equals("text()")) { // textual content of currentElt
				currentElt.setTextContent(value);
			} else if (lastPart.startsWith("@")) {
				String attName = lastPart.substring(1);
				currentElt.setAttribute(attName, value);
			} else {
				throw new SystemConfigurationException("Do not know how to assign value for info '"+shortName+"' from last part of xpath expression '"
						+lastPart+"' -- expected either 'text()' or '@attributeName'");
			}
			*/
		}
	}

	

	
	/**
	 * Read information from the message document and fill our info 
	 * as much as possible.
	 * If information is found that cannot be interpreted, 
	 * the code will emit a warning but continue to work. 
	 * @throws MessageFormatException if the structure of the document is
	 * inconsistent, i.e. structure expectations are violated.
	 */
	protected void analyseDocument(String rootName, String rootNamespace)
	throws MessageFormatException
	{
		Element root = doc.getDocumentElement();
		if (!root.getLocalName().equals(rootName)) {
			throw new MessageFormatException("XML document should have root node '"+
					rootName+"', but has '"+root.getLocalName()+"'!");
		}
		if (!XMLTool.isSameNamespace(root.getNamespaceURI(), rootNamespace)) {
			throw new MessageFormatException("root node should have namespace '"+
					rootNamespace+"' but has '"+root.getNamespaceURI()+"'!");
		}
		
		// Now extract the information from the document using the XPath expressions given in a config file
		Map<String, XPathExpression> infoNames = infosByType.get(type).getCompiledExpressionMap();
		for (String shortName : infoNames.keySet()) {
			XPathExpression xpathExpr = infoNames.get(shortName);
			try {
				Object result = xpathExpr.evaluate(doc, XPathConstants.STRING);
				String resultString = (String) result;
				if (resultString != null && !resultString.equals("")) {
					info.put(shortName, resultString);
					//log.debug("Read info: "+shortName+" = "+resultString);
				}
			} catch (XPathExpressionException xee) {
				throw new MessageFormatException("Problem analysing value of '"+shortName+"'", xee);
			}
		}
	}

	/**
	 * Provide a read-only access to the information in this message.
	 * The map contains all information that is part of the message.
	 * @return a map with string keys and string values.
	 * @see StateInfo#getShortNames(Type) for a list of short names that can be used as keys for the information.
	 */
	public Map<String, String> getInfos()
	{
		return Collections.unmodifiableMap(info);
	}
	
	/**
	 * Indicate whether the current info set contains an entry for the named information.
	 * @param name
	 * @return
	 */
	public boolean hasInfo(String name)
	{
		return info.containsKey(name);
	}
	
	/**
	 * Get the named information, or null if there is no such information.
	 * @param name
	 * @return
	 */
	public String getInfo(String name)
	{
		return info.get(name);
	}

	public void setInfo(String name, String value)
	{
		// Verify that we can handle these info items:
		Map<String, String> info2expr = infosByType.get(type).getExpressionMap();
		if (!info2expr.containsKey(name)) {
			throw new IllegalArgumentException("Don't know how to handle info item '"+name+"' -- something seems to be out of sync");
		}
		info.put(name, value);
		doc = null; // if it was there, it's now out of date
	}
	
	public Document getDocument()
	{
		if (doc == null) {
			assert info != null : "Both doc and info are null - this shouldn't happen!";
			try {
				createDocumentFromInfo();
			} catch (SystemConfigurationException e) {
				throw new RuntimeException(e);
			}
		}
		assert doc != null : "Couldn't create document";
		return doc;
	}

	@Override
	public String toString()
	{
		return stateName;
	}

	public String getAPIVersion()
	{
		return APIVersion;
	}


	/**
	 * A helper class that supports us in mapping short names to XPath expressions.
	 * @author marc
	 *
	 */
	public static class XPathInfoMapper
	{
		private NamespaceContext namespaceContext;
		private Map<String, String> name2exprString;
		private Map<String, XPathExpression> name2compiledExpression;
		
		public XPathInfoMapper(final Map<String, String> prefixes2NamespaceURIs,
				final Map<String, String> names2XpathExpressions)
		{
			this.namespaceContext = new NamespaceContext() {
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
			this.name2exprString = Collections.unmodifiableMap(names2XpathExpressions);
			Map<String, XPathExpression> s2x = new LinkedHashMap<String, XPathExpression>();
			
			// Now compile the xpath expressions
			XPathFactory xpathFact = XPathFactory.newInstance();
			XPath xpath = xpathFact.newXPath();
			xpath.setNamespaceContext(namespaceContext);
			
			for (String shortName : name2exprString.keySet()) {
				String xpathExprString = name2exprString.get(shortName);
				try {
					XPathExpression xpathExpr = xpath.compile(xpathExprString);
					s2x.put(shortName, xpathExpr);
				} catch (XPathExpressionException e) {
					throw new Error("Cannot compile XPath expression for shortname "+shortName+": '"+xpathExprString+"'", e);
				}
			}
			this.name2compiledExpression = Collections.unmodifiableMap(s2x);
		}
		
		public NamespaceContext getNamespaceContext()
		{
			return namespaceContext;
		}
		
		/**
		 * Get the XPath expression for the given name,
		 * or null if there is no such entry. 
		 * @param shortName
		 * @return
		 */
		public String getExpression(String shortName)
		{
			return name2exprString.get(shortName);
		}
		
		public Map<String, String> getExpressionMap()
		{
			return name2exprString;
		}
		
		public XPathExpression getCompiledExpression(String shortName)
		{
			return name2compiledExpression.get(shortName);
		}
		
		public Map<String, XPathExpression> getCompiledExpressionMap()
		{
			return name2compiledExpression;
		}
	}

}
