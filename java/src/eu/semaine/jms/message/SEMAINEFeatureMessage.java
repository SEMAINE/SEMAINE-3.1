package eu.semaine.jms.message;

import javax.jms.BytesMessage;
import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.TextMessage;

import eu.semaine.exceptions.MessageFormatException;

public class SEMAINEFeatureMessage extends SEMAINEMessage
{
	protected float[] features = null;
	protected String[] featureNames = null;
	
	public SEMAINEFeatureMessage(Message message)
	throws MessageFormatException
	{
		super(message);
		if (!(message instanceof TextMessage ||
			  message instanceof BytesMessage)) {
			throw new MessageFormatException("Expected either a text message or a bytes message, but got a "+message.getClass().getName());
		}
	}
	
	/**
	 * Get the feature vector from the message.
	 * @return the feature vector contained in the message.
	 * @throws JMSException
	 * @throws MessageFormatException
	 */
	public float[] getFeatureVector()
	throws JMSException, MessageFormatException
	{
		if (features == null) {
			if (message instanceof TextMessage) {
				readFromTextMessage();
			} else {
				readFromBytesMessage();
			}
		}
		assert features != null : "features are still null";
		return features;
	}

	/**
	 * Get the list of feature names if available.
	 * @return an array of strings containing the names of features if available, 
	 * or null if the feature names are not available.
	 * @throws JMSException
	 */
	public String[] getFeatureNames()
	throws JMSException
	{
		if (!(message instanceof TextMessage))
			return null; // can only get feature names from text message
		if (featureNames == null) {
			readFromTextMessage();
		}
		assert featureNames != null : "feature names are still null";
		return featureNames;
	}
	
	/**
	 * Set the feature names corresponding to the features.
	 * @param names an array of feature names, which must correspond
	 * in number and in their order to the features in this message.
	 * @throws MessageFormatException if the number of feature names
	 * does not match the number of features saved in the message
	 */
	public void setFeatureNames(String[] names)
	throws MessageFormatException
	{
		if (names != null && features != null && names.length != features.length)
		{
			throw new MessageFormatException("There are "+features.length+" features but "+names.length+" feature names");
		}
		featureNames = names;
	}
	
	protected void readFromBytesMessage()
	throws JMSException
	{
		assert message instanceof BytesMessage : "this method should only be called for bytes messages, but message is a "+message.getClass().getName();
		BytesMessage bm = (BytesMessage) message;
		int len = bm.readInt();
		if (bm.getBodyLength() != 4*(len+1)) {
			throw new MessageFormatException("Inconsistent bytes message: expected length "+(4*(len+1))+" (one int and "+len+" floats), but have "+bm.getBodyLength()+" bytes");
		}
		features = new float[len];
		for (int i=0; i<len; i++) {
			features[i] = bm.readFloat();
		}
	}
	
	protected void readFromTextMessage()
	throws JMSException
	{
		assert message instanceof TextMessage : "this method should only be called for text messages, but message is a "+message.getClass().getName();
		String text = ((TextMessage)message).getText();
		String[] lines = text.split("\r\n|\r|\n");
		features = new float[lines.length];
		featureNames = new String[lines.length];
		for (int i=0; i<lines.length; i++) {
			// line format is:
			// value featureName
			String[] parts = lines[i].split("\\s+");
			if (parts.length > 2) {
				throw new MessageFormatException("Message contains more than two fields in line "+i+": '"+lines[i]+"'");
			}
			try {
				features[i] = Float.parseFloat(parts[0]);
			} catch (NumberFormatException nfe) {
				throw new MessageFormatException("Cannot read value '"+parts[0]+"' in line "+i+" as a float", nfe);
			}
			featureNames[i] = parts[1];
		}
	}
}
