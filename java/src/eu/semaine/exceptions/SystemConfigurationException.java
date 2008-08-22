package eu.semaine.exceptions;

import javax.jms.JMSException;

/**
 * An exception related to a wrong system configuration. Ideally thrown
 * early in the lifecycle of the application, e.g. from constructors.
 * @author marc
 *
 */
@SuppressWarnings("serial")
public class SystemConfigurationException extends JMSException 
{
	public SystemConfigurationException(String text)
	{
		super(text);
	}
	
	public SystemConfigurationException(String text, Throwable cause)
	{
		super(text);
		initCause(cause);
	}

}
