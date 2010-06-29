/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.jms.sender;

import javax.jms.BytesMessage;
import javax.jms.JMSException;

import eu.semaine.jms.message.SEMAINEMessage;

/**
 * An abstraction of Sender for feature vectors.
 * @author marc
 *
 */
public class FeatureSender extends Sender
{
	protected String[] featureNames = null;
	
	/**
	 * Create a new event-based FeatureSender to the given topic on the default JMS server.
	 * The default JMS server is determined as follows:
	 * <ul>
	 * <li>jms URL: read from system property <code>jms.url</code>, default value <code>tcp://localhost:61616</code>;</li>
	 * <li>jms User: read from system property <code>jms.user</code>, default value <code>null</code>;</li>
	 * <li>jms Password: read from system property <code>jms.password</code>, default value <code>null</code>.</li>
	 * </ul>
	 * Datatype and source will be sent with every message.
	 * @param topic the name of the topic to send to.
	 * @param datatype the name of the type of data being sent.
	 * @param source the name of the component sending the data.
	 * @param the period in which we intend to send data, in milliseconds
	 * @throws NullPointerException if any of the parameters topicName, datatype, or source are null. 
	 * @throws IllegalArgumentException if period is <= 0.
	 */
	public FeatureSender(String topicName, String datatype, String source)
	throws JMSException
	{
		super(topicName, datatype, source);
		setEventBased();
	}
	
	/**
	 * Create a new periodic FeatureSender to the given topic on the default JMS server.
	 * The default JMS server is determined as follows:
	 * <ul>
	 * <li>jms URL: read from system property <code>jms.url</code>, default value <code>tcp://localhost:61616</code>;</li>
	 * <li>jms User: read from system property <code>jms.user</code>, default value <code>null</code>;</li>
	 * <li>jms Password: read from system property <code>jms.password</code>, default value <code>null</code>.</li>
	 * </ul>
	 * Datatype and source will be sent with every message.
	 * @param topic the name of the topic to send to.
	 * @param datatype the name of the type of data being sent.
	 * @param source the name of the component sending the data.
	 * @param the period in which we intend to send data, in milliseconds
	 * @throws NullPointerException if any of the parameters topicName, datatype, or source are null. 
	 * @throws IllegalArgumentException if period is <= 0.
	 */
	public FeatureSender(String topicName, String datatype, String source, int period)
	throws JMSException
	{
		super(topicName, datatype, source);
		setPeriodic(period);
	}

	/**
	 * Create a new event-based FeatureSender to the given topic on the given JMS server.
	 * Datatype and source will be sent with every message.
	 * @param jmsUrl the url where to contact the JMS server
	 * @param jmsUser the username to use (can be null)
	 * @param jmsPassword the password to use (can be null)
	 * @param topic the name of the topic to send to.
	 * @param datatype the name of the type of data being sent.
	 * @param source the name of the component sending the data.
	 * @param the period in which we intend to send data, in milliseconds
	 * @throws JMSException
	 * @throws NullPointerException if any of the parameters jmsUrl, topicName, datatype, or source are null. 
	 * @throws IllegalArgumentException if period is <= 0.
	 */
	public FeatureSender(String jmsUrl, String jmsUser, String jmsPassword, String topicName, String datatype, String source)
	throws JMSException
	{
		super(jmsUrl, jmsUser, jmsPassword, topicName, datatype, source);
		setEventBased();
	}

	/**
	 * Create a new periodic FeatureSender to the given topic on the given JMS server.
	 * Datatype and source will be sent with every message.
	 * @param jmsUrl the url where to contact the JMS server
	 * @param jmsUser the username to use (can be null)
	 * @param jmsPassword the password to use (can be null)
	 * @param topic the name of the topic to send to.
	 * @param datatype the name of the type of data being sent.
	 * @param source the name of the component sending the data.
	 * @param the period in which we intend to send data, in milliseconds
	 * @throws JMSException
	 * @throws NullPointerException if any of the parameters jmsUrl, topicName, datatype, or source are null. 
	 * @throws IllegalArgumentException if period is <= 0.
	 */
	public FeatureSender(String jmsUrl, String jmsUser, String jmsPassword, String topicName, String datatype, String source, int period)
	throws JMSException
	{
		super(jmsUrl, jmsUser, jmsPassword, topicName, datatype, source);
		setPeriodic(period);
	}

	/**
	 * Initiate the list of feature names to use. This method must be called
	 * exactly once, after the creation of the object and before the first
	 * send operation.
	 * @param aFeatureNames the feature names, in exactly the same order
	 * as the features that will be sent.
	 * @throws IllegalStateException if feature names have been set before.
	 * @throws NullPointerException if aFeatureNames is null.
	 */
	public void setFeatureNames(String[] aFeatureNames)
	{
		if (this.featureNames != null)
			throw new IllegalStateException("feature names are already set, can only set once");
		if (aFeatureNames == null)
			throw new NullPointerException("feature names vector passed as argument is null");
		this.featureNames = aFeatureNames;
	}
	
	/**
	 * Provides access to the array of feature names. May be useful for debugging.
	 * @return the internal feature names array.
	 */
	public String[] getFeatureNames()
	{
		return featureNames;
	}
	
	/**
	 * Send a vector of float features as a text message. This is for periodic feature vectors.
	 * Before sending features,
	 * {@link #setFeatureNames(String[]) must be called, and the
	 * names must correspond to the features passed in the argument.
	 * @param features the features to send.
	 * @param usertime the "user" time at which this message is being sent,
	 * in milliseconds since 1970.
	 * @throws JMSException
	 */
	public void sendFeatureVector(float[] features, long usertime)
	throws JMSException
	{
		sendFeatureVector(features, usertime, false);
	}
	
	/**
	 * Send a vector of float features. This is for periodic feature vectors. Before sending features,
	 * {@link #setFeatureNames(String[]) must be called, and the
	 * names must correspond to the features passed in the argument.
	 * @param features the features to send.
	 * @param usertime the "user" time at which this message is being sent,
	 * in milliseconds since 1970.
	 * @throws JMSException
	 */
	public void sendFeatureVector(float[] features, long usertime, boolean sendBinary)
	throws JMSException
	{
		if (featureNames == null)
			throw new IllegalStateException("you must set feature names before sending feature vectors");
		if (features == null)
			throw new NullPointerException("features passed as argument are null");
		if (featureNames.length != features.length)
			throw new IllegalArgumentException("vectors are not of equal length: names="+featureNames.length+", features="+features.length);
		if (sendBinary)
			sendBinaryFeatureVector(features, usertime);
		else
			sendTextFeatureVector(features, usertime);
	}

	/**
	 * Send a vector of float features as a text message. This is for event-based feature vectors.
	 * Before sending features,
	 * {@link #setFeatureNames(String[]) must be called, and the
	 * names must correspond to the features passed in the argument.
	 * @param features the features to send.
	 * @param usertime the "user" time at which this message is being sent,
	 * in milliseconds since 1970.
	 * @throws JMSException
	 */
	public void sendFeatureVector(float[] features, long usertime, Event eventType, String contentID, long contentCreationTime)
	throws JMSException
	{
		sendFeatureVector(features, usertime, false, eventType, contentID, contentCreationTime);
	}
	
	/**
	 * Send a vector of float features. This is for event-based feature vectors. Before sending features,
	 * {@link #setFeatureNames(String[]) must be called, and the
	 * names must correspond to the features passed in the argument.
	 * @param features the features to send.
	 * @param usertime the "user" time at which this message is being sent,
	 * in milliseconds since 1970.
	 * @throws JMSException
	 */
	public void sendFeatureVector(float[] features, long usertime, boolean sendBinary, Event eventType, String contentID, long contentCreationTime)
	throws JMSException
	{
		if (featureNames == null)
			throw new IllegalStateException("you must set feature names before sending feature vectors");
		if (features == null)
			throw new NullPointerException("features passed as argument are null");
		if (featureNames.length != features.length)
			throw new IllegalArgumentException("vectors are not of equal length: names="+featureNames.length+", features="+features.length);
		if (sendBinary)
			sendBinaryFeatureVector(features, usertime, eventType, contentID, contentCreationTime);
		else
			sendTextFeatureVector(features, usertime, eventType, contentID, contentCreationTime);
	}

	
	/**
	 * For periodic messages
	 * @param features
	 * @param usertime
	 * @throws JMSException
	 */
	protected void sendBinaryFeatureVector(float[] features, long usertime)
	throws JMSException
	{
		if (!isConnectionStarted)
			throw new IllegalStateException("Connection is not started!");
		if (!isPeriodic()) {
			throw new IllegalStateException("This method is for periodic feature messages only");
		}
		BytesMessage message = session.createBytesMessage();
		message.writeInt(features.length);
		for (int i=0; i<features.length; i++) {
			message.writeFloat(features[i]);
		}
		fillMessageProperties(message, usertime);
		message.setIntProperty(SEMAINEMessage.PERIOD, getPeriod());
		producer.send(message);
	}
	
	/**
	 * For event-based messages
	 * @param features
	 * @param usertime
	 * @throws JMSException
	 */
	protected void sendBinaryFeatureVector(float[] features, long usertime, Event eventType, String contentID, long contentCreationTime)
	throws JMSException
	{
		if (!isConnectionStarted)
			throw new IllegalStateException("Connection is not started!");
		if (isPeriodic()) {
			throw new IllegalStateException("This method is for event-based feature messages only");
		}
		BytesMessage message = session.createBytesMessage();
		message.writeInt(features.length);
		for (int i=0; i<features.length; i++) {
			message.writeFloat(features[i]);
		}
		fillMessageProperties(message, usertime, contentID, contentCreationTime);
		message.setStringProperty(SEMAINEMessage.EVENT, eventType.toString());
		producer.send(message);
	}

	/**
	 * For periodic messages
	 * @param features
	 * @param usertime
	 * @throws JMSException
	 */
	protected void sendTextFeatureVector(float[] features, long usertime)
	throws JMSException
	{
		if (!isPeriodic()) {
			throw new IllegalStateException("This method is for periodic feature messages only");
		}
		StringBuilder buf = new StringBuilder();
		for (int i=0; i<features.length; i++) {
			buf.append(features[i]).append(" ").append(featureNames[i]).append("\n");
		}
		sendTextMessage(buf.toString(), usertime);
	}

	/**
	 * For event-based messages
	 * @param features
	 * @param usertime
	 * @param eventType
	 * @param contentID
	 * @param contentCreationTime
	 * @throws JMSException
	 */
	protected void sendTextFeatureVector(float[] features, long usertime, Event eventType, String contentID, long contentCreationTime)
	throws JMSException
	{
		if (isPeriodic()) {
			throw new IllegalStateException("This method is for event-based feature messages only");
		}
		StringBuilder buf = new StringBuilder();
		for (int i=0; i<features.length; i++) {
			buf.append(features[i]).append(" ").append(featureNames[i]).append("\n");
		}
		sendTextMessage(buf.toString(), usertime, eventType, contentID, contentCreationTime);
	}
}

