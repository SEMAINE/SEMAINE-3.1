/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.datatypes;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.jms.JMSLogger;
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
	protected Map<String,String> info;
	protected Document doc;
	protected JMSLogger log;
	protected String stateName;
	protected String apiVersion;

	protected StateInfo(Document doc, String whatState, String apiVersion)
	throws MessageFormatException
	{
		this.stateName = whatState;
		this.apiVersion = apiVersion;
		log = new JMSLogger(whatState);
		info = new HashMap<String, String>();
		setupInfoKeys();
		analyseDocument();
		assert info != null;
	}

	public StateInfo(Map<String,String> infoItems, String whatState, String apiVersion)
	{
		this.stateName = whatState;
		this.apiVersion = apiVersion;
		log = new JMSLogger(whatState);
		info = new HashMap<String, String>(infoItems);
		createDocumentFromInfo();
		assert doc != null;
	}
	
	/**
	 * Set up the possible values that we can know about.
	 * Things that are not previewed here will not be read from the document.
	 * When this changes, the APIVersion must change with it.
	 */
	protected abstract void setupInfoKeys();
	
	protected abstract void createDocumentFromInfo();

	/**
	 * Make sense of elements in the markup that come directly below the
	 * document element.
	 * 
	 * This base version can interpret <emotion:category> markup for which
	 * the 'set' attribute is a key in the info map.
	 * 
	 * Subclasses will need to override this method to get additional 
	 * analysis functionality. They can call <code>super.analyseElement(el)</code>
	 * to access this implementation.
	 * @param el an element whose parent is the document element.
	 * @return true if the element could be analysed, false otherwise.
	 */
	protected boolean analyseElement(Element el)
	throws MessageFormatException
	{
		String namespace = el.getNamespaceURI();
		String tagname = el.getTagName();
		if (namespace.equals(EmotionML.namespace)) {
			if (tagname.equals(EmotionML.CATEGORY)) {
				String set = el.getAttribute(EmotionML.SET);
				if (info.containsKey(set)) {
					String name = XMLTool.needAttribute(el, EmotionML.NAME);
					info.put(set, name);
					return true;
				}
			}
		}
		return false;
	}
	

	
	/**
	 * Read information from the message document and fill our info 
	 * as much as possible.
	 * If information is found that cannot be interpreted, 
	 * the code will emit a warning but continue to work. 
	 * @throws MessageFormatException if the structure of the document is
	 * inconsistent, i.e. structure expectations are violated.
	 */
	protected void analyseDocument()
	throws MessageFormatException
	{
		Element root = doc.getDocumentElement();
		if (!root.getTagName().equals(SemaineML.AGENTSTATE)) {
			throw new MessageFormatException("XML document should have root node '"+
					SemaineML.AGENTSTATE+"', but has '"+root.getTagName()+"'!");
		}
		NodeList nodes = root.getChildNodes();
		for (int i=0, max=nodes.getLength(); i<max; i++) {
			Node n = nodes.item(i);
			if (!(n.getNodeType() == Node.ELEMENT_NODE)) {
				continue;
			}
			assert n instanceof Element : "Should only see elements here";
			Element el = (Element) n;
			// Now see what element it is and make sense of it
			boolean ok = analyseElement(el);
			if (!ok) {
				log.warn("do not know how to analyse element "+el.getTagName()+
						" in namespace "+el.getNamespaceURI());
			}
		}
	}

	/**
	 * Provide a read-only access to the information in this message.
	 * The map contains as keys all information that can be known according
	 * to the API version, and as non-null values the values taken from the message.
	 * Values for information items not contained in the message will be null. 
	 * @return a map with string keys and string values.
	 */
	public Map<String, String> getInfo()
	{
		return Collections.unmodifiableMap(info);
	}

	public Document getDocument()
	{
		return doc;
	}

	@Override
	public String toString()
	{
		return stateName;
	}

	public String getAPIVersion()
	{
		return apiVersion;
	}



}
