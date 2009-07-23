/**
 * Copyright (C) 2009 University of Twente. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */

package eu.semaine.components.dialogue.interpreters;

import java.util.HashMap;
import java.util.Map;

import javax.jms.JMSException;

import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.datatypes.stateinfo.StateInfo;
import eu.semaine.datatypes.stateinfo.UserStateInfo;
import eu.semaine.datatypes.xml.EmotionML;
import eu.semaine.jms.message.SEMAINEEmmaMessage;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.receiver.EmmaReceiver;
import eu.semaine.jms.sender.StateSender;
import eu.semaine.util.XMLTool;

/**
 * The EmotionInterpreter reads the emotions detected by the input modules, and if they
 * are higher than a certain threshold this emotion is send to the user state.
 * 
 * Input
 * EmmaReceiver('semaine.data.state.user.emma')
 * 
 * Output
 * StateSender('semaine.data.state.user')
 * 
 * 
 * @author MaatM
 *
 */

public class EmotionInterpreter extends Component
{
	/* The thresholds for the emotions */
	private static final float VALENCE_THRESHOLD = 0.2f;
	private static final float AROUSAL_THRESHOLD = 0.2f;
	private static final float INTEREST_THRESHOLD = 0.2f;
	
	/* Senders and Receivers */
	private EmmaReceiver emmaReceiver;
	private StateSender userStateSender;
	
	
	/**
	 * Constructor of EmotionInterpreter
	 * Initializes the Senders and Receivers
	 * @throws JMSException
	 */
	public EmotionInterpreter() throws JMSException
	{
		super( "EmotionInterpreter" );
		
		emmaReceiver = new EmmaReceiver("semaine.data.state.user.emma", "datatype = 'EMMA'");
		receivers.add(emmaReceiver);
		
		userStateSender = new StateSender("semaine.data.state.user", StateInfo.Type.UserState, getName());
		senders.add(userStateSender);
	}
	
	/**
	 * Reads the emotions sends in EmmaMessages and determines the values.
	 * If the values exceed the predetermined thresholds a message is send to the userstate.
	 */
	public void react( SEMAINEMessage m ) throws JMSException
	{
		/* Read the emotions from the message */
		float[] emotions = getEmotions(m);
		
		/* Determine if the values exceed the thresholds */
		if( emotions[0] >= VALENCE_THRESHOLD ) {
			sendEmotion( "valence", emotions[0] );
		} 
		if( emotions[1] >= AROUSAL_THRESHOLD ) {
			sendEmotion( "arousal", emotions[1] );
		}
		if( emotions[2] >= INTEREST_THRESHOLD ) {
			sendEmotion( "interest", emotions[2] );
		}
	}
	
	/**
	 * Retrieves the emotions from the given Message
	 * @param m - the message to read
	 * @return	- the values for the emotions in this order: [valence, arousal, interest]
	 */
	public float[] getEmotions( SEMAINEMessage m ) throws JMSException
	{
		/* Initializing the emotion array */
		float[] emotions = new float[3];
		emotions[0] = 0f;
		emotions[1] = 0f;
		emotions[2] = 0f;
		
		/* Reading SemaineMessage */
		if( m instanceof SEMAINEEmmaMessage ) {
			SEMAINEEmmaMessage em = (SEMAINEEmmaMessage)m;

			/* Reading dimension-Element */
			Element dimensions = (Element) em.getDocument().getElementsByTagNameNS( EmotionML.namespaceURI, EmotionML.E_DIMENSIONS).item(0);
			if( dimensions != null ) {
				Element valenceElem = XMLTool.needChildElementByTagNameNS(dimensions, EmotionML.E_VALENCE, EmotionML.namespaceURI);
				Element arousalElem = XMLTool.needChildElementByTagNameNS(dimensions, EmotionML.E_AROUSAL, EmotionML.namespaceURI);

				float valence = Float.parseFloat(valenceElem.getAttribute(EmotionML.A_VALUE));
				float arousal = Float.parseFloat(arousalElem.getAttribute(EmotionML.A_VALUE));
				
				emotions[0] = valence;
				emotions[1] = arousal;
			}
			
			/* Reading category-Element */
			Element category = (Element) em.getDocument().getElementsByTagNameNS(EmotionML.namespaceURI, EmotionML.E_CATEGORY).item(0);
			if( category != null ) {
				if( category.getAttribute("name").equals("interest") ){
					float interest = Float.parseFloat(category.getAttribute("value"));
					emotions[2] = interest;
				}
			}
		}
		return emotions;
	}
	
	/**
	 * Sends the given emotion with the given value to the UserState
	 * @param emotion	- the emotion to send
	 * @param value		- the value of the emotion to send
	 * @throws JMSException
	 */
	public void sendEmotion( String emotion, float value ) throws JMSException
	{
		Map<String,String> userStateInfo = new HashMap<String,String>();
		userStateInfo.put("behaviour", emotion);
		userStateInfo.put("behaviour intensity", ""+value );
		
		UserStateInfo usi = new UserStateInfo(userStateInfo	);
		userStateSender.sendStateInfo(usi, meta.getTime());	
	}
}
