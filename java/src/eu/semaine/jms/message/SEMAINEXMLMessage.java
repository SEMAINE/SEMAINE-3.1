package eu.semaine.jms.message;

import java.io.StringReader;

import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.TextMessage;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.w3c.dom.Document;
import org.xml.sax.InputSource;

import eu.semaine.exceptions.MessageFormatException;

public class SEMAINEXMLMessage extends SEMAINEMessage
{
	public static final String IS_XML = "xml";
	
	protected Document doc = null;
	
	public SEMAINEXMLMessage(Message message)
	throws MessageFormatException
	{
		super(message);
		if (!(message instanceof TextMessage)) {
			throw new MessageFormatException("Expected a text message, but got a "+message.getClass().getName());
		}
		try {
			// the following will return false if the property is not set
			// or if it does not contain the value "true".
			boolean isXML = message.getBooleanProperty(IS_XML);
			if (!isXML) throw new MessageFormatException("expected XML message");
		} catch (JMSException j) {
			throw new MessageFormatException("Cannot read property '"+IS_XML+"'", j);
		}
		parseDocument();
	}
	
	public Document getDocument()
	throws MessageFormatException
	{
		return doc;
	}
	
	protected void parseDocument()
	throws MessageFormatException
	{
		try {
			DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
			factory.setNamespaceAware(true);
			DocumentBuilder builder = factory.newDocumentBuilder();
			doc = builder.parse(new InputSource(new StringReader(getText())));
		} catch (Exception e) {
			throw new MessageFormatException("Cannot parse XML message", e);
		}
	}
	
}
