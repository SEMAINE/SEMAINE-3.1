/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.dummy;

import java.io.IOException;

import javax.jms.JMSException;

import eu.semaine.components.Component;
import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.exceptions.SystemConfigurationException;
import eu.semaine.jms.IOBase.Event;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.message.SEMAINEXMLMessage;
import eu.semaine.jms.receiver.BMLReceiver;
import eu.semaine.jms.sender.Sender;
import eu.semaine.util.SEMAINEUtils;

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
	private Sender dataInfoSender;
	private Sender commandSender;
	
	private String dummyFAPData;
	
	private boolean doSendPlayCommand;
	
  
	/**
	 * @param componentName
	 * @throws JMSException
	 */
	public DummyBMLRealiser() throws JMSException 
	{
		this("DummyBMLRealiser",
				"semaine.data.synthesis.plan",
				"semaine.data.synthesis.lowlevel.video.FAP",
				"semaine.data.synthesis.lowlevel.command",
				true);
	}
	
	protected DummyBMLRealiser(String compName,
			String bmlReceiverTopic,
			String fapSenderTopic,
			String dataInfoSenderTopic,
			boolean doSendPlayCommand) throws JMSException {
		super(compName);
		bmlReceiver = new BMLReceiver(bmlReceiverTopic);
		receivers.add(bmlReceiver);
		fapSender = new Sender(fapSenderTopic, "FAP", getName());
		senders.add(fapSender);
		dataInfoSender = new Sender(dataInfoSenderTopic, "dataInfo", getName());
		senders.add(dataInfoSender);
		commandSender = new Sender("semaine.data.synthesis.lowlevel.command", "playCommand", getName());
		senders.add(commandSender);
		
		this.doSendPlayCommand = doSendPlayCommand;
		
		try {
			dummyFAPData = SEMAINEUtils.getStreamAsString(DummyBMLRealiser.class.getResourceAsStream("example2.fap"), "ASCII");
			//dummyFAPData2 = SEMAINEUtils.getStreamAsString(this.getClass().getResourceAsStream("example3.fap"), "ASCII");
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
	    
		fapSender.sendTextMessage(dummyFAPData, meta.getTime(), m.getEventType(), m.getContentID(), m.getContentCreationTime());
		
		String dataInfoString = "HASAUDIO 1\nHASFAP 1\nHASBAP 0\n";
		dataInfoSender.sendTextMessage(dataInfoString, meta.getTime(), Event.single, m.getContentID(), m.getContentCreationTime());

		if (doSendPlayCommand) {
			String commandString = "STARTAT 0\nPRIORITY 0.5\nLIFETIME 5000\n";
			commandSender.sendTextMessage(commandString, meta.getTime(), Event.single, m.getContentID(), m.getContentCreationTime());
		}

	}
}
