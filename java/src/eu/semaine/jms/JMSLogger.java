/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.jms;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;

import javax.jms.Connection;
import javax.jms.ConnectionFactory;
import javax.jms.JMSException;
import javax.jms.MessageProducer;
import javax.jms.Session;
import javax.jms.TextMessage;

import org.apache.activemq.ActiveMQConnectionFactory;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import eu.semaine.components.MessageLogComponent;

/**
 * A relatively lightweight class logging messages to a JMS topic if possible,
 * and to the local log system if the JMS connection fails. All instances of 
 * JMSLogger use the same JMS connection and the same session.
 * @author marc
 *
 */
public class JMSLogger
{
	////////// Static stuff, used by all JMSLoggers ///////////
	private static Connection connection;
	private static Session session;
	private static Map<String,JMSLogger> loggers = new HashMap<String, JMSLogger>();
	
	static {
		try {
			String jmsUrl = System.getProperty("jms.url", "tcp://localhost:61616");
			String jmsUser = System.getProperty("jms.user", null);
			String jmsPassword = System.getProperty("jms.password", null);
			ConnectionFactory factory = new ActiveMQConnectionFactory(jmsUser, jmsPassword, jmsUrl);
			connection = factory.createConnection();
			session = connection.createSession(false /*not transacted*/, Session.AUTO_ACKNOWLEDGE);
			connection.start();
		} catch (JMSException e) {
			LogFactory.getLog(JMSLogger.class).warn("Cannot set up JMS logger connection and session", e);
		}
	};
	
	
	public static JMSLogger getLog(String source)
	{
		if (!loggers.containsKey(source)) {
			loggers.put(source, new JMSLogger(source));
		}
		return loggers.get(source);

	}
	
	/////////////// Individual stuff ////////////////
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
	private JMSLogger(String source)
	{
		String basename = "semaine.log."+source;
		fallbackLogger = LogFactory.getLog(basename);
		try {
			error = session.createProducer(session.createTopic(basename+".error"));
			warn = session.createProducer(session.createTopic(basename+".warn"));
			info = session.createProducer(session.createTopic(basename+".info"));
			debug = session.createProducer(session.createTopic(basename+".debug"));
		} catch (JMSException e) {
			fallbackLogger.warn("Cannot set up JMS log connections for "+source+"; will use fallback instead");
		}
	}
	
	public void error(Object... objects)
	{
		log(error, Level.error, -1, objects);
	}

	public void warn(Object... objects)
	{
		log(warn, Level.warn, -1, objects);
	}

	public void info(Object... objects)
	{
		log(info, Level.info, -1, objects);
	}

	public void debug(Object... objects)
	{
		log(debug, Level.debug, -1, objects);
	}

	public void error(long usertime, Object... objects)
	{
		log(error, Level.error, usertime, objects);
	}

	public void warn(long usertime, Object... objects)
	{
		log(warn, Level.warn, usertime, objects);
	}

	public void info(long usertime, Object... objects)
	{
		log(info, Level.info, usertime, objects);
	}

	public void debug(long usertime, Object... objects)
	{
		log(debug, Level.debug, usertime, objects);
	}
	
	
	


	private void log(MessageProducer target, Level level, long usertime, Object... objects)
	{
		String logMessageText = toLogMessageText(usertime, objects);
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

	public static String toLogMessageText(Object... objects) {
		return toLogMessageText(-1, objects);
	}
	
	public static String toLogMessageText(long usertime, Object... objects) {
		StringBuilder builder = new StringBuilder();
		if (usertime >= 0) {
			builder.append("[").append(MessageLogComponent.TIME_FORMAT.format(new Date(usertime))).append("]");

		}
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
		return logMessageText;
	}
	
	
	public boolean isDebugEnabled()
	{
		// in the future, we may be able to use advisory information to know
		// if anyone is listening to this topic
		return true;
	}
}
