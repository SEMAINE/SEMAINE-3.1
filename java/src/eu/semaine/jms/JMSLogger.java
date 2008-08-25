/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.jms;

import java.io.PrintWriter;
import java.io.StringWriter;

import javax.jms.JMSException;
import javax.jms.MessageProducer;
import javax.jms.TextMessage;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

/**
 * @author marc
 *
 */
public class JMSLogger extends Sender
{
	private enum Level {error, warn, info, debug};
	
	private MessageProducer error;
	private MessageProducer warn;
	private MessageProducer info;
	private MessageProducer debug;

	private Log fallbackLogger;
	
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
		fallbackLogger = LogFactory.getLog(basename);
		info = producer; // set up in parent code
		error = session.createProducer(session.createTopic(basename+".error"));
		warn = session.createProducer(session.createTopic(basename+".warn"));
		debug = session.createProducer(session.createTopic(basename+".debug"));
		startConnection();
	}
	
	public void error(Object... objects)
	{
		log(error, Level.error, objects);
	}

	public void warn(Object... objects)
	{
		log(warn, Level.warn, objects);
	}

	public void info(Object... objects)
	{
		log(info, Level.info, objects);
	}

	public void debug(Object... objects)
	{
		log(debug, Level.debug, objects);
	}

	private void log(MessageProducer target, Level level, Object... objects)
	{
		StringBuilder builder = new StringBuilder();
		for (Object o : objects) {
			if (builder.length() > 0) {
				builder.append(" ");
			}
			if (o instanceof Throwable) {
				Throwable t = (Throwable) o;
				StringWriter sw = new StringWriter();
				t.printStackTrace(new PrintWriter(sw));
				builder.append(sw.toString());
			} else {
				builder.append(o.toString());
			}
		}
		String logMessageText = builder.toString();
		try {
			TextMessage message = session.createTextMessage(logMessageText);
			target.send(message);
		} catch (JMSException e) {
			fallbackLogger.error("problem with JMS logger", e);
			if (level == Level.error) {
				fallbackLogger.error(logMessageText);
			} else if (level == Level.warn) {
				fallbackLogger.warn(logMessageText);
			} else if (level == Level.info) {
				fallbackLogger.warn(logMessageText);
			} else if (level == Level.debug) {
				fallbackLogger.warn(logMessageText);
			} else {
				fallbackLogger.error("problem with fallback logger -- unknown log level: "+level);
				fallbackLogger.info(logMessageText);
			}
		}
		
	}
	
	
	public boolean isDebugEnabled()
	{
		// in the future, we may be able to use advisory information to know
		// if anyone is listening to this topic
		return true;
	}
}
