/**
 * Copyright (C) 2010 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.jms.message;


import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.TextMessage;


import org.apache.activemq.command.ActiveMQTextMessage;
import org.w3c.dom.Document;

import eu.semaine.exceptions.SystemConfigurationException;
import eu.semaine.jms.IOBase.Event;
import eu.semaine.util.XMLTool;

/**
 * This class provides test utility methods for writing readable unit tests.
 * @author marc
 *
 */
public class SEMAINEMessageTestUtils {
	
	public static TextMessage createActiveMQTextMessage() throws JMSException {
		TextMessage tm = new ActiveMQTextMessage();
		fillGenericProperties(tm);
		fillXMLProperty(tm);
		return tm;
	}
	
	private static void fillGenericProperties(Message m) throws JMSException {
		m.setLongProperty(SEMAINEMessage.USERTIME, System.currentTimeMillis());
		m.setStringProperty(SEMAINEMessage.DATATYPE, "TEST");
		m.setStringProperty(SEMAINEMessage.SOURCE, "JUnit");
		m.setStringProperty(SEMAINEMessage.EVENT, Event.single.toString());
	}
	
	private static void fillXMLProperty(Message m) throws JMSException {
		m.setBooleanProperty(SEMAINEXMLMessage.IS_XML, true);
	}
	
	/**
	 * Create an EMMA message from the XPath string/value pairs given as parameters
	 * @param xpathValuePairs an even number of Strings, representing pairs of xpath strings
	 *  used for generating the document and the values to store at the given location.
	 * @return an EMMA message containing an EMMA document with the given data.
	 * @throws JMSException if the underlying message object cannot be created
	 * @throws IllegalArgumentException if the number of strings provided as parameters is not an even number,
	 * or if they cannot be interpreted
	 * @see {@link XMLTool#xpath2doc(String, String, javax.xml.namespace.NamespaceContext, Document)}
	 */
	public static SEMAINEEmmaMessage createEMMAMessage(String... xpathValuePairs) throws JMSException {
		TextMessage m = createActiveMQMessageFromXPath(xpathValuePairs);
		SEMAINEEmmaMessage em = new SEMAINEEmmaMessage(m);
		return em;
	}

	/**
	 * Create an XML message from the XPath string/value pairs given as parameters
	 * @param xpathValuePairs an even number of Strings, representing pairs of xpath strings
	 *  used for generating the document and the values to store at the given location.
	 * @return an XML message containing a document with the given data.
	 * @throws JMSException if the underlying message object cannot be created
	 * @throws IllegalArgumentException if the number of strings provided as parameters is not an even number,
	 * or if they cannot be interpreted
	 * @see {@link XMLTool#xpath2doc(String, String, javax.xml.namespace.NamespaceContext, Document)}
	 */
	public static SEMAINEXMLMessage createXMLMessage(String... xpathValuePairs) throws JMSException {
		TextMessage m = createActiveMQMessageFromXPath(xpathValuePairs);
		SEMAINEXMLMessage xm = new SEMAINEXMLMessage(m);
		return xm;
	}

	/**
	 * @param xpathValuePairs
	 * @return
	 * @throws IllegalArgumentException
	 * @throws NullPointerException
	 * @throws JMSException
	 * @throws SystemConfigurationException
	 */
	private static TextMessage createActiveMQMessageFromXPath(String... xpathValuePairs)
	throws IllegalArgumentException, NullPointerException, JMSException, SystemConfigurationException {
		if (xpathValuePairs.length == 0 || xpathValuePairs.length % 2 != 0) {
			throw new IllegalArgumentException("Not an even number of strings");
		}
		Document doc = null;
		for (int i=0; i<xpathValuePairs.length; i+=2) {
			String xpathExpression = xpathValuePairs[i];
			String value = xpathValuePairs[i+1];
			doc = XMLTool.xpath2doc(xpathExpression, value, null, doc);
		}
		TextMessage m = createActiveMQTextMessage();
		m.setText(XMLTool.document2String(doc));
		return m;
	}
	

}
