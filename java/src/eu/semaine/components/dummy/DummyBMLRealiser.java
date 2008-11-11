/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.dummy;

import java.io.IOException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;

import javax.jms.JMSException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.datatypes.xml.BML;
import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.exceptions.SystemConfigurationException;
import eu.semaine.jms.IOBase.Event;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.message.SEMAINEXMLMessage;
import eu.semaine.jms.receiver.BMLReceiver;
import eu.semaine.jms.sender.Sender;
import eu.semaine.util.SEMAINEUtils;
import eu.semaine.util.XMLTool;

/**
 * This dummy action selection accepts all candidates.
 * 
 * @author marc
 *
 */
public class DummyBMLRealiser extends Component 
{
	private BMLReceiver bmlReceiver;
	private Sender fapSender;

	private String dummyFAPData;
	
	/**
	 * @param componentName
	 * @throws JMSException
	 */
	public DummyBMLRealiser() throws JMSException 
	{
		super("DummyBMLRealiserAndPlayer");
		bmlReceiver = new BMLReceiver("semaine.data.synthesis.plan");
		receivers.add(bmlReceiver);
		fapSender = new Sender("semaine.data.synthesis.lowlevel.video", "FAP", getName());
		senders.add(fapSender);
		
		try {
			dummyFAPData = SEMAINEUtils.getStreamAsString(this.getClass().getResourceAsStream("example2.fap"), "ASCII");
		} catch (IOException ioe) {
			throw new SystemConfigurationException("Cannot get FAP example", ioe);
		}
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

		fapSender.sendTextMessage(dummyFAPData, System.currentTimeMillis());
	}
}
