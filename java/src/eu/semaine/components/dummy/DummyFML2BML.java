/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.dummy;

import javax.jms.JMSException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.datatypes.xml.BML;
import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.message.SEMAINEXMLMessage;
import eu.semaine.jms.receiver.BMLReceiver;
import eu.semaine.jms.receiver.FMLReceiver;
import eu.semaine.jms.sender.BMLSender;
import eu.semaine.util.XMLTool;

/**
 * This dummy action selection accepts all candidates.
 * 
 * @author marc
 *
 */
public class DummyFML2BML extends Component 
{
	private FMLReceiver fmlReceiver;
	private BMLReceiver bmlReceiver;
	private BMLSender bmlSender;
	
	

	/**
	 * @param componentName
	 * @throws JMSException
	 */
	public DummyFML2BML() throws JMSException 
	{
		super("DummyFML2BML");
		fmlReceiver = new FMLReceiver("semaine.data.action.selected.function");
		receivers.add(fmlReceiver); // to set up properly
		bmlReceiver = new BMLReceiver("semaine.data.action.selected.behaviour");
		receivers.add(bmlReceiver);
		
		bmlSender = new BMLSender("semaine.data.synthesis.plan", getName());
		senders.add(bmlSender); // so it can be started etc
	}
	

	@Override
	public void react(SEMAINEMessage m) throws JMSException
	{
		if (!(m instanceof SEMAINEXMLMessage)) {
			throw new MessageFormatException("expected XML message, got "+m.getClass().getSimpleName());
		}
		SEMAINEXMLMessage xm = (SEMAINEXMLMessage)m;
		boolean isFML = "FML".equals(xm.getDatatype());
		if (isFML) {
			// extract the BML and send that along
			Element bml = XMLTool.needChildElementByTagNameNS(
					xm.getDocument().getDocumentElement(), BML.E_BML, BML.namespaceURI);
			Document doc = XMLTool.newDocument(BML.E_BML, BML.namespaceURI);
			doc.adoptNode(bml);
			doc.replaceChild(bml, doc.getDocumentElement());
			bmlSender.sendXML(doc, xm.getUsertime(), xm.getEventType());
		} else { // BML -- send as is
			bmlSender.sendXML(xm.getDocument(), xm.getUsertime(), xm.getEventType());
		}
	}
}
