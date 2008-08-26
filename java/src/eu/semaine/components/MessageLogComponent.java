/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;

import javax.jms.JMSException;
import javax.jms.TextMessage;

import eu.semaine.datatypes.SEMAINEMessage;
import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.jms.JMSLogger;
import eu.semaine.jms.Receiver;

/**
 * This component can be used to view all or a selection of components.
 * It writes debug output to the JMSLogger.
 * 
 * Add this to the component list in the config file if you want detailed log info.
 * @author marc
 *
 */
public class MessageLogComponent extends Component 
{
	private Receiver receiver;
	
	private DateFormat format = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS");


	/**
	 * @param componentName
	 * @throws JMSException
	 */
	public MessageLogComponent(String topic, String messageSelector)
	throws JMSException
	{
		super("MessageLogComponent");
		receiver = new Receiver(topic, messageSelector);
		receivers.add(receiver);
	}

	@Override
	protected void react(SEMAINEMessage m)
	throws JMSException
	{
		// log message content
		StringBuilder buf = new StringBuilder();
		buf.append("[").append(format.format(new Date(m.getUsertime()))).append("] ");
		buf.append(m.getDatatype()).append(" ");
		buf.append(m.getSource()).append("->").append(m.getMessage().getJMSDestination().toString()).append(" ");
		if (m.isEventBased()) {
			buf.append(m.getEventType()).append(" event ");
		} else {
			buf.append("period=").append(m.getPeriod()).append(" ");
		}
		if (m.getMessage() instanceof TextMessage) {
			buf.append("\n");
			buf.append(m.getText());
			buf.append("\n");
		} else {
			buf.append(m.getMessage().getClass().getSimpleName());
			buf.append("\n");
		}
	}
}
