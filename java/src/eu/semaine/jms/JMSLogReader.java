/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.jms;


import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.TextMessage;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.log4j.BasicConfigurator;
import org.apache.log4j.ConsoleAppender;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.apache.log4j.PatternLayout;

import eu.semaine.jms.receiver.Receiver;

/**
 * @author marc
 *
 */
public class JMSLogReader extends Receiver 
{
	//DateFormat time = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS");
	
	/**
	 * @param topicName
	 * @throws JMSException
	 */
	public JMSLogReader(String topicName) throws JMSException
	{
		super(topicName);
		
		consumer.setMessageListener(this);
		startConnection();
	}
	
	
	@Override
	public void onMessage(Message m)
	{
		try {
			if (!(m instanceof TextMessage)) {
				return; // silently ignore
			}
			String dest = m.getJMSDestination().toString();
			// dest is expected to have the form
			// semaine.log.component.log-level
			String[] parts = dest.split("\\.");
			String level = parts[parts.length-1].toLowerCase();
			String component = parts[parts.length-2];
			Log log = LogFactory.getLog("semaine.log."+component);
			String text = ((TextMessage)m).getText();
			//text = time.format(new Date(m.getJMSTimestamp())) + " " + text;
			if (level.equals("info")) log.info(text);
			else if (level.equals("warn")) log.warn(text);
			else if (level.equals("error")) log.error(text);
			else if (level.equals("debug")) log.debug(text);
			else log.info(text);
		} catch (Exception e) {
			
		}
	}
	


	/**
	 * @param args
	 */
	public static void main(String[] args)
	throws Exception
	{
		// Use log4j for logging:
		System.setProperty("org.apache.commons.logging.Log", "org.apache.commons.logging.impl.Log4JLogger");
		// Configure log4j
		Logger.getRootLogger().setLevel(Level.DEBUG);
		Logger.getLogger("org.apache").setLevel(Level.INFO);
		PatternLayout layout = new PatternLayout("%-5p %-10c %m\n");
		BasicConfigurator.configure(new ConsoleAppender(layout));
		String component = "*";
		String level = "*";
		if (args.length > 0) component = args[0];
		if (args.length > 1) level = args[1];
		System.out.println("Looking for log messages in topic 'semaine.log."+component+"."+level+"'");
		new JMSLogReader("semaine.log."+component+"."+level);
	}

}
