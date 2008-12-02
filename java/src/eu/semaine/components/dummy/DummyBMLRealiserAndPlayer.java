/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.dummy;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;

import javax.jms.JMSException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.datatypes.xml.BML;
import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.jms.IOBase.Event;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.message.SEMAINEXMLMessage;
import eu.semaine.jms.receiver.BMLReceiver;
import eu.semaine.util.XMLTool;

/**
 * This dummy action selection accepts all candidates.
 * 
 * @author marc
 *
 */
public class DummyBMLRealiserAndPlayer extends Component 
{
	private BMLReceiver bmlReceiver;
	
	private DateFormat format = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS");

	/**
	 * @param componentName
	 * @throws JMSException
	 */
	public DummyBMLRealiserAndPlayer() throws JMSException 
	{
		super("DummyBMLRealiserAndPlayer");
		bmlReceiver = new BMLReceiver("semaine.data.synthesis.plan");
		receivers.add(bmlReceiver);
	}
	

	@Override
	public void react(SEMAINEMessage m) throws JMSException
	{
		if (!(m instanceof SEMAINEXMLMessage)) {
			throw new MessageFormatException("expected XML message, got "+m.getClass().getSimpleName());
		}
		SEMAINEXMLMessage xm = (SEMAINEXMLMessage)m;
		boolean isBML = "BML".equals(xm.getDatatype());
		if (!isBML)
			throw new MessageFormatException("Expected BML message, got "+xm.getDatatype());
		
		Event event = xm.getEventType();
		Document doc = xm.getDocument();
		Element bml = doc.getDocumentElement();
		Element speech = XMLTool.getChildElementByTagNameNS(bml, BML.E_SPEECH, BML.namespace);
		if (speech != null) {
			//Element text = XMLTool.needChildElementByTagNameNS(speech, BML.TEXT, BML.namespace);
			String utterance = speech.getTextContent();
			System.out.println(format.format(new Date(xm.getUsertime()))+ " " + utterance);
		}
		Element head = XMLTool.getChildElementByTagNameNS(bml, BML.E_HEAD, BML.namespace);
		if (head != null) {
			String type = head.getAttribute(BML.A_TYPE);
			if (type.equals("NOD")) {
				System.out.println(format.format(new Date(xm.getUsertime()))+ " (nods)");
			}
		}
		Element face = XMLTool.getChildElementByTagNameNS(bml, BML.E_FACE, BML.namespace);
		if (face != null) {
			String type = face.getAttribute(BML.A_TYPE);
			if (type.equals("smile")) {
				String action = event == Event.start ? "starts" : "stops";
				System.out.println(format.format(new Date(xm.getUsertime()))+ " ("+action+" smiling)");
			}
		}
	}
}
