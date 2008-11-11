package eu.semaine.jms.message;

import javax.jms.BytesMessage;
import javax.jms.JMSException;
import javax.jms.Message;

import eu.semaine.exceptions.MessageFormatException;

public class SEMAINEBytesMessage extends SEMAINEMessage
{
	protected byte[] bytes;
	
	public SEMAINEBytesMessage(Message message)
	throws MessageFormatException
	{
		super(message);
		if (!(message instanceof BytesMessage)) {
			throw new MessageFormatException("Expected a bytes message, but got a "+message.getClass().getName());
		}
		BytesMessage bm = (BytesMessage)message;
		try {
			bytes = new byte[(int) bm.getBodyLength()];
			bm.readBytes(bytes);
		} catch (JMSException e) {
			throw new MessageFormatException("Cannot read message", e);
		}
	}
	
	public byte[] getBytes()
	{
		return bytes;
	}
	
}
