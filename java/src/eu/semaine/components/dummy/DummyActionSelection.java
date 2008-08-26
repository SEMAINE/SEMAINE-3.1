/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.dummy;

import javax.jms.JMSException;

import eu.semaine.components.Component;
import eu.semaine.datatypes.SEMAINEMessage;
import eu.semaine.datatypes.SEMAINEXMLMessage;
import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.jms.BMLReceiver;
import eu.semaine.jms.BMLSender;
import eu.semaine.jms.FMLReceiver;
import eu.semaine.jms.FMLSender;

/**
 * This dummy action selection accepts all candidates.
 * 
 * @author marc
 *
 */
public class DummyActionSelection extends Component 
{
	private FMLReceiver fmlReceiver;
	private BMLReceiver bmlReceiver;
	private FMLSender fmlSender;
	private BMLSender bmlSender;
	
	
	
	/**
	 * @param componentName
	 * @throws JMSException
	 */
	public DummyActionSelection() throws JMSException 
	{
		super("DummyActionSelection");
		fmlReceiver = new FMLReceiver("semaine.data.action.candidate.function");
		receivers.add(fmlReceiver); // to set up properly
		bmlReceiver = new BMLReceiver("semaine.data.action.candidate.behaviour");
		receivers.add(bmlReceiver);
		
		fmlSender = new FMLSender("semaine.data.action.selected.function", getName());
		senders.add(fmlSender); // so it can be started etc
		bmlSender = new BMLSender("semaine.data.action.selected.behaviour", getName());
		senders.add(bmlSender); // so it can be started etc
	}
	

	@Override
	protected void react(SEMAINEMessage m) throws JMSException
	{
		if (!(m instanceof SEMAINEXMLMessage)) {
			throw new MessageFormatException("expected XML message, got "+m.getClass().getSimpleName());
		}
		SEMAINEXMLMessage xm = (SEMAINEXMLMessage)m;
		boolean isFML = "FML".equals(xm.getDatatype());
		if (isFML) {
			fmlSender.sendXML(xm.getDocument(), xm.getUsertime(), xm.getEventType());
		} else {
			bmlSender.sendXML(xm.getDocument(), xm.getUsertime(), xm.getEventType());
		}
	}
}
