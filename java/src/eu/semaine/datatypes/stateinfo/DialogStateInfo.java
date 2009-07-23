/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.datatypes.stateinfo;

import java.util.Collection;
import java.util.Collections;
import java.util.Map;
import java.util.SortedSet;
import java.util.TreeSet;

import javax.jms.JMSException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.exceptions.SystemConfigurationException;
import eu.semaine.util.XMLTool;

/**
 * @author marc
 *
 */
public class DialogStateInfo extends StateInfo
{
	public static final String APIVersion = "0.2";

	private SortedSet<DialogAct> dialogHistory = new TreeSet<DialogAct>();


	/**
	 * @param doc
	 * @throws MessageFormatException
	 * @throws JMSException
	 */
	public DialogStateInfo(Document doc)
	throws MessageFormatException
	{
		super(doc, "DialogStateInfo", APIVersion, SemaineML.E_DIALOGSTATE, SemaineML.namespaceURI, Type.DialogState);
	}

	/**
	 * 
	 * @param infoItems
	 * @param dialogActs a collection of dialog acts for the dialog history, or null.
	 * @throws JMSException
	 */
	public DialogStateInfo(Map<String, String> infoItems, Collection<DialogAct> dialogActs)
	throws JMSException
	{
		super(infoItems, "DialogStateInfo", APIVersion, Type.DialogState);
		if (dialogActs != null) {
			dialogHistory.addAll(dialogActs);
		}
	}

	/**
	 * @see eu.semaine.datatypes.stateinfo.StateInfo#createDocumentFromInfo()
	 */
	@Override
	protected void createDocumentFromInfo()
	throws SystemConfigurationException
	{
		assert dialogHistory != null : "method seems to be called before constructor is finished";
		super.createDocumentFromInfo();
		if (dialogHistory.size() > 0) {
			Element root = doc.getDocumentElement();
			Element dh = XMLTool.appendChildElement(root, SemaineML.E_DIALOG_HISTORY, SemaineML.namespaceURI);
			for (DialogAct dialogAct : dialogHistory) {
				Element da = XMLTool.appendChildElement(dh, SemaineML.E_DIALOG_ACT, SemaineML.namespaceURI);
				da.setAttribute(SemaineML.A_SPEAKER, dialogAct.getSpeaker());
				da.setAttribute(SemaineML.A_TOPIC, dialogAct.getTopic());
				da.setAttribute(SemaineML.A_TIME, String.valueOf(dialogAct.getTime()));
				da.setTextContent(dialogAct.getText());
			}
		}
	}


	@Override
	protected void analyseDocument(String rootName, String rootNamespace)
	throws MessageFormatException
	{
		super.analyseDocument(rootName, rootNamespace);
		
		NodeList nl = doc.getElementsByTagNameNS(SemaineML.namespaceURI, SemaineML.E_DIALOG_HISTORY);
		if (nl.getLength() > 0) {
			Element el = (Element) nl.item(0);
			NodeList nodes = el.getChildNodes();
			for (int i=0, max=nodes.getLength(); i<max; i++) {
				Node n = nodes.item(i);
				if (!(n.getNodeType() == Node.ELEMENT_NODE)) {
					continue;
				}
				assert n instanceof Element : "Should only see elements here";
				Element child = (Element) n;
				if (!(child.getNamespaceURI().equals(SemaineML.namespaceURI)
					  && child.getTagName().equals(SemaineML.E_DIALOG_ACT))) {
					throw new MessageFormatException("Element '"+SemaineML.E_DIALOG_HISTORY+
							"' in namespace '"+el.getNamespaceURI()+
							"' should only contain child elements named '"+SemaineML.E_DIALOG_ACT+
							"' in the same namespace");
				}
				String text = child.getTextContent();
				String speaker = XMLTool.needAttribute(child, SemaineML.A_SPEAKER);
				String topic = XMLTool.needAttribute(child, SemaineML.A_TOPIC);
				String timeString = XMLTool.needAttribute(child, SemaineML.A_TIME);
				long time = 0;
				try {
					time = Long.valueOf(timeString);
				} catch (NumberFormatException nfe) {
					throw new MessageFormatException("Cannot parse time '"+timeString+"' as long", nfe);
				}
				DialogAct da = new DialogAct(text, speaker, topic, time);
				dialogHistory.add(da);
			}
		}
	}
	
	
	/**
	 * Get read-only access to the dialog history. The iterator of the sorted
	 * set will return the dialog acts sorted by time, most recent first.
	 * @return a set of dialog acts, or null if the message doesn't contain a dialog history.
	 */
	public SortedSet<DialogAct> getDialogHistory()
	{
		return Collections.unmodifiableSortedSet(dialogHistory);
	}

	public static class DialogAct implements Comparable<DialogAct>
	{
		private String text;
		private String speaker;
		private String topic;
		private long time;
		
		public DialogAct(String text, String speaker, String topic, long time)
		{
			this.text = text;
			this.speaker = speaker;
			this.topic = topic;
			this.time = time;
		}
		
		/**
		 * Method used for sorting Dialog acts by their time, newest first.
		 */
		public int compareTo(DialogAct other)
		{
			if (time == other.time) return 0;
			if (time < other.time) return 1;
			return -1;
		}
		
		public String getText()
		{
			return text;
		}
		
		public String getSpeaker()
		{
			return speaker;
		}
		
		public String getTopic()
		{
			return topic;
		}
		
		public long getTime()
		{
			return time;
		}
	}

}
