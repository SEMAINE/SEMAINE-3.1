/**
 * Copyright (C) 2009 University of Twente. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */

package eu.semaine.components.dialogue.interpreters;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.jms.JMSException;

import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.datatypes.stateinfo.StateInfo;
import eu.semaine.datatypes.stateinfo.UserStateInfo;
import eu.semaine.datatypes.xml.EMMA;
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
		
		if( emotions != null ) {
			sendEmotions(emotions);
		}
	}
	
	/**
	 * Retrieves the emotions from the given Message
	 * @param m - the message to read
	 * @return	- the values for the emotions in this order: [valence, arousal, interest]
	 */
	public float[] getEmotions( SEMAINEMessage m ) throws JMSException
	{
		// valence, arousal, interest (in this order)
		float[] emotions = new float[3];
		emotions[0] = -2;
		emotions[1] = -2;
		emotions[2] = -2;
		boolean emotionsChanged = false;
		
		/* Reading SemaineMessage */
		if( m instanceof SEMAINEEmmaMessage ) {
			SEMAINEEmmaMessage em = (SEMAINEEmmaMessage)m;

			/* Reading valence and arousal */
			Element interpretation = XMLTool.getChildElementByLocalNameNS(em.getDocument().getDocumentElement(), EMMA.E_INTERPRETATION, EMMA.namespaceURI);
			if( interpretation != null ) {
				List<Element> emotionElements = em.getEmotionElements(interpretation);
				for( Element emotion : emotionElements ) {
					List<Element> dimensionElements = XMLTool.getChildrenByLocalNameNS(emotion, EmotionML.E_DIMENSIONS, EmotionML.namespaceURI);
					for( Element dimension : dimensionElements ) {
						Element valenceElement = XMLTool.getChildElementByLocalNameNS(dimension, EmotionML.E_VALENCE, EmotionML.namespaceURI);
						if( valenceElement != null && valenceElement.hasAttribute(EmotionML.A_VALUE) ) {
							emotions[0] = Float.parseFloat( valenceElement.getAttribute(EmotionML.A_VALUE) );
							emotionsChanged = true;
						}
						Element arousalElement = XMLTool.getChildElementByLocalNameNS(dimension, EmotionML.E_AROUSAL, EmotionML.namespaceURI);
						if( arousalElement != null && arousalElement.hasAttribute(EmotionML.A_VALUE) ) {
							emotions[1] = Float.parseFloat( arousalElement.getAttribute(EmotionML.A_VALUE) );
							emotionsChanged = true;
						}
					}
				}
			}
			
			/* Reading interest */
			Element oneOf = em.getOneOf();
			if(oneOf != null) {
				List<Element> interpretations = XMLTool.getChildrenByLocalNameNS(oneOf, EMMA.E_INTERPRETATION, EMMA.namespaceURI);
				if( interpretations.size() > 0 ) {
					float bored = 0;
					float neutral = 0;
					float interested = 0;
					for( Element interpretationElem : interpretations ) {
						List<Element> emotionElements = em.getEmotionElements(interpretationElem);
						for( Element emotion : emotionElements ) {
							Element category = XMLTool.getChildElementByLocalNameNS(emotion, EmotionML.E_CATEGORY, EmotionML.namespaceURI);
							if( category != null && category.hasAttribute(EmotionML.A_NAME) ) {
								String cat = category.getAttribute(EmotionML.A_NAME);
								if( cat.equals("bored") ) {
									bored = Float.parseFloat( interpretationElem.getAttribute(EMMA.A_CONFIDENCE) );
								} else if( cat.equals("neutral") ) {
									neutral = Float.parseFloat( interpretationElem.getAttribute(EMMA.A_CONFIDENCE) );
								} else if( cat.equals("interested") ) {
									interested = Float.parseFloat( interpretationElem.getAttribute(EMMA.A_CONFIDENCE) );
								}
							}
						}
					}
					/* Calculate combined interest-value */
					emotions[2] = -1*bored + interested;
					emotionsChanged = true;
				}
			}
		}
		if( emotionsChanged ) {
			return emotions;
		} else {
			return null;
		}
	}
	
	/**
	 * Sends the given emotion with the given value to the UserState
	 * @param emotion	- the emotion to send
	 * @param value		- the value of the emotion to send
	 * @throws JMSException
	 */
	public void sendEmotions( float[] emotions ) throws JMSException
	{
		Map<String,String> userStateInfo = new HashMap<String,String>();
		if( emotions[0] != -2 ) {
			userStateInfo.put("valence",""+emotions[0]);
		}
		if( emotions[1] != -2 ) {
			userStateInfo.put("arousal",""+emotions[1]);
		}
		if( emotions[2] != -2 ) {
			userStateInfo.put("interest",""+emotions[2]);
		}
		
		UserStateInfo usi = new UserStateInfo(userStateInfo	);
		userStateSender.sendStateInfo(usi, meta.getTime());	
	}
}
