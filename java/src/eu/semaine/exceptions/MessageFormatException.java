package eu.semaine.exceptions;

import javax.jms.JMSException;

/**
 * An exception related to the format of a message.
 * @author marc
 *
 */
public class MessageFormatException extends JMSException 
{
	public MessageFormatException(String text)
	{
		super(text);
	}
	
	public MessageFormatException(String text, Throwable cause)
	{
		super(text);
		initCause(cause);
	}

}
