/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.StringTokenizer;

import javax.jms.JMSException;
import javax.jms.TextMessage;

import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.receiver.Receiver;

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
	
	public static final DateFormat TIME_FORMAT = new SimpleDateFormat("mm:ss.SSS");


	/**
	 * @param componentName
	 * @throws JMSException
	 */
	public MessageLogComponent(String topics, String messageSelector)
	throws JMSException
	{
		super("MessageLogComponent");
		StringTokenizer st = new StringTokenizer(topics);
		while (st.hasMoreTokens()) {
			String topic = st.nextToken().trim();
			if (!"".equals(topic)) {
				receiver = new Receiver(topic, messageSelector);
				receivers.add(receiver);
			}
		}
	}

	@Override
	protected void react(SEMAINEMessage m)
	throws JMSException
	{
		// log message content
		log.debug(message2logString(m));
	}

	public static String message2logString(SEMAINEMessage m)
	{
		try {
			StringBuilder buf = new StringBuilder();
			buf.append("[").append(TIME_FORMAT.format(new Date(m.getUsertime()))).append("] ");
			buf.append(m.getDatatype()).append(" ");
			buf.append(m.getSource()).append("->").append(m.getMessage().getJMSDestination().toString()).append(" ");
			if (m.isEventBased()) {
				buf.append(m.getEventType()).append(" event ");
			} else {
				buf.append("period=").append(m.getPeriod()).append(" ");
			}
			String contentID = m.getContentID();
			if (contentID != null) {
				buf.append("ID=").append(contentID).append(" ");
			}
			long contentCreationTime = m.getContentCreationTime();
			if (contentCreationTime >= 0) {
				buf.append("(created ").append(contentCreationTime).append(") ");
			}
			if (m.getMessage() instanceof TextMessage) {
				buf.append("\n");
				buf.append(m.getText());
				buf.append("\n");
			} else {
				buf.append(m.getMessage().getClass().getSimpleName());
				buf.append("\n");
			}
			return buf.toString();
		} catch (JMSException je) {
			StringWriter errWriter = new StringWriter();
			PrintWriter pw = new PrintWriter(errWriter);
			pw.println("Exception trying to log message content: ");
			je.printStackTrace(pw);
			return errWriter.toString();
		}
	}

}
