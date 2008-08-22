/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.jms;

import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageProducer;
import javax.jms.TextMessage;

/**
 * @author marc
 *
 */
public class JMSLogger extends Sender
{
	MessageProducer info;
	MessageProducer error;
	MessageProducer warn;
	MessageProducer debug;

	/**
	 * @param topicName
	 * @param datatype
	 * @param source
	 * @throws JMSException
	 */
	public JMSLogger(String source)
			throws JMSException 
	{
		super("semaine.log."+source+".info", "log", source);
		setupLoggers("semaine.log."+source);
	}

	/**
	 * @param jmsUrl
	 * @param jmsUser
	 * @param jmsPassword
	 * @param topicName
	 * @param datatype
	 * @param source
	 * @throws JMSException
	 */
	public JMSLogger(String jmsUrl, String jmsUser, String jmsPassword, String source)
	throws JMSException
	{
		super(jmsUrl, jmsUser, jmsPassword, "semaine.log."+source+".info", "log", source);
		setupLoggers("semaine.log."+source);
	}

	private void setupLoggers(String basename)
	throws JMSException
	{
		info = producer; // set up in parent code
		error = session.createProducer(session.createTopic(basename+".error"));
		warn = session.createProducer(session.createTopic(basename+".warn"));
		debug = session.createProducer(session.createTopic(basename+".debug"));
		startConnection();
	}
	
	public void error(String text)
	throws JMSException
	{
		TextMessage message = session.createTextMessage(text);
		error.send(message);
	}

	public void warn(String text)
	throws JMSException
	{
		TextMessage message = session.createTextMessage(text);
		warn.send(message);
	}

	public void info(String text)
	throws JMSException
	{
		TextMessage message = session.createTextMessage(text);
		info.send(message);
	}

	public void debug(String text)
	throws JMSException
	{
		TextMessage message = session.createTextMessage(text);
		debug.send(message);
	}

	public boolean isDebugEnabled()
	{
		// in the future, we may be able to use advisory information to know
		// if anyone is listening to this topic
		return true;
	}
}
