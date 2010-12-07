/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.datatypes.stateinfo;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import javax.xml.namespace.NamespaceContext;
import javax.xml.xpath.XPath;
import javax.xml.xpath.XPathConstants;
import javax.xml.xpath.XPathExpression;
import javax.xml.xpath.XPathExpressionException;
import javax.xml.xpath.XPathFactory;

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
		InputStream is = null;
		// check if we are to read from a resource first, and try config file second:
		String resourcePath = System.getProperty("semaine.stateinfo-config.resource");
		if (resourcePath != null) {
			is = ClassLoader.getSystemResourceAsStream(resourcePath);
			if (is == null) {
				throw new Error("Resource '"+resourcePath+"' is requested but does not exist");
			}
		} else {
			File configFile = SEMAINEUtils.getConfigFile("semaine.stateinfo-config");
			if (configFile == null) {
				throw new Error("No character config file given in property 'semaine.stateinfo-config' -- aborting.");
			}
			try {
				is = new FileInputStream(configFile);
			} catch (IOException ioe) {
				throw new Error("Cannot open stateinfo config file "+configFile.toString(), ioe);
			}
		}
		BufferedReader br = null;
		try {
			br = new BufferedReader(new InputStreamReader(is, "UTF-8"));
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
			throw new Error("Cannot load stateinfo config", ioe);
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
				try {
					XMLTool.splitXPathIntoParts(xpathExpr);
				} catch (IllegalArgumentException iae) {
					throw new SystemConfigurationException("Problem with xpath expression", iae);
				}
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

	protected StateInfo(Map<String,String> infoItems, String whatState, Type type)
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
			String expr = infoNames.get(shortName);
			if (expr == null) {
				throw new SystemConfigurationException("No info entry for '"+shortName+"' -- something seems to be out of sync");
			}
			try {
				doc = XMLTool.xpath2doc(expr, value, namespaceContext, doc);
			} catch (IllegalArgumentException iae) {
				throw new SystemConfigurationException("For shortname '"+shortName+"', Problem with xpath expression: "+expr, iae);
			}

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
			this.namespaceContext = XMLTool.createNamespaceContext(prefixes2NamespaceURIs);
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
