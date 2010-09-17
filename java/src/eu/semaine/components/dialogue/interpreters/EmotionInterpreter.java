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
import eu.semaine.components.dialogue.test.DMLogger;
import eu.semaine.datatypes.stateinfo.StateInfo;
import eu.semaine.datatypes.stateinfo.UserStateInfo;
import eu.semaine.datatypes.xml.EMMA;
import eu.semaine.datatypes.xml.EmotionML;
import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.jms.message.SEMAINEEmmaMessage;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.receiver.EmmaReceiver;
import eu.semaine.jms.sender.StateSender;
import eu.semaine.util.XMLTool;

/**
 * The EmotionInterpreter reads the emotions detected by the input modules, and if they
 * are higher than a certain threshold this emotion is send to the user state.
 * Emotions: Valence, Arousal, Potency, Emotion-Quadrant
 * Other factors: Interest, Gender
 * 
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
	private static final float POTENCY_THRESHOLD = 0.2f;
	private static final float INTEREST_THRESHOLD = 0.2f;
	private static final float QUADRANT_CONFIDENCE_THRESHOLD = 0.2f;

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

		emmaReceiver = new EmmaReceiver("semaine.data.state.user.emma.emotion", "datatype = 'EMMA'");
		receivers.add(emmaReceiver);

		userStateSender = new StateSender("semaine.data.state.user.behaviour", StateInfo.Type.UserState, getName());
		senders.add(userStateSender);
	}

	/**
	 * Reads the emotions sends in EmmaMessages and determines the values.
	 * If the values exceed the predetermined thresholds a message is send to the userstate.
	 */
	public void react( SEMAINEMessage m ) throws JMSException
	{
		if( m instanceof SEMAINEEmmaMessage ) {
			SEMAINEEmmaMessage em = (SEMAINEEmmaMessage)m;

			float[] emotions = getEmotions(em);
			//String[] quadrant = getEmotionQuadrant(em);
			String gender = getGender(em);
			
			//sendEmotions( emotions[0], emotions[1], emotions[2], emotions[3], quadrant[0], quadrant[1], gender );
			sendEmotions( emotions[0], emotions[1], emotions[2], emotions[3], gender );
		}
	}
	
	public String getGender( SEMAINEEmmaMessage em ) throws JMSException
	{
		String gender = "";
		
		Element interpretation = XMLTool.getChildElementByLocalNameNS(em.getDocument().getDocumentElement(), EMMA.E_INTERPRETATION, EMMA.namespaceURI);
		if( interpretation != null ) {
			List<Element> genderElements = em.getGenderElements(interpretation);
			for( Element genderElem : genderElements ) {
				if( genderElem.hasAttribute(SemaineML.A_NAME) ) {
					gender = genderElem.getAttribute( SemaineML.A_NAME );
				}
			}
		}
		
		return gender;
	}

//	public String[] getEmotionQuadrant( SEMAINEEmmaMessage em ) throws JMSException
//	{
//		String[] quadrant = new String[2];
//		quadrant[0] = "";
//		quadrant[1] = "";
//
//		Element interpretation = XMLTool.getChildElementByLocalNameNS(em.getDocument().getDocumentElement(), EMMA.E_INTERPRETATION, EMMA.namespaceURI);
//		if( interpretation != null ) {
//			List<Element> categoryElements = em.getCategoryElements(interpretation);
//			for( Element category : categoryElements ) {
//				if( category.hasAttribute(EmotionML.A_SET) && category.getAttribute(EmotionML.A_SET).equals("fourQuadrants") ) {
//					if( category.hasAttribute(EmotionML.A_NAME) && category.hasAttribute(EmotionML.A_CONFIDENCE) ) {
//						quadrant[0] = category.getAttribute(EmotionML.A_NAME);
//						quadrant[1] = category.getAttribute(EmotionML.A_CONFIDENCE);
//					}
//				}
//			}
//		}
//		
//		return quadrant;
//	}

	/**
	 * Retrieves the emotions from the given Message
	 * @param m - the message to read
	 * @return	- the values for the emotions in this order: [valence, arousal, Potency, Interest]
	 */
	public float[] getEmotions( SEMAINEEmmaMessage em ) throws JMSException
	{
		// valence, arousal, interest (in this order)
		float[] emotions = new float[4];
		emotions[0] = 0;
		emotions[1] = 0;
		emotions[2] = 0;
		emotions[3] = 0;
		boolean emotionsChanged = false;

		//System.out.println(em.getText());
		/* Reading valence and arousal */
		Element interpretation = XMLTool.getChildElementByLocalNameNS(em.getDocument().getDocumentElement(), EMMA.E_INTERPRETATION, EMMA.namespaceURI);
		if( interpretation != null ) {
			List<Element> emotionElements = em.getEmotionElements(interpretation);
			for( Element emotion : emotionElements ) {
				List<Element> dimensions = XMLTool.getChildrenByLocalNameNS(emotion, EmotionML.E_DIMENSION, EmotionML.namespaceURI);
				for (Element dim : dimensions) {
					String name = dim.getAttribute(EmotionML.A_NAME);
					float value = Float.parseFloat(dim.getAttribute(EmotionML.A_VALUE));
					if (name.equals(EmotionML.VOC_FSRE_DIMENSION_VALENCE)) {
						if( Float.parseFloat(dim.getAttribute(EmotionML.A_CONFIDENCE)) >= VALENCE_THRESHOLD ) {
							emotions[0] = EmotionML.fsreValence2SemaineValence(value);
							emotionsChanged = true;
						}
					} else if (name.equals(EmotionML.VOC_FSRE_DIMENSION_AROUSAL)) {
						if( Float.parseFloat(dim.getAttribute(EmotionML.A_CONFIDENCE)) >= AROUSAL_THRESHOLD ) {
							emotions[1] = EmotionML.fsreArousal2SemaineArousal(value);
							emotionsChanged = true;
						}
					}  else if (name.equals(EmotionML.VOC_FSRE_DIMENSION_POTENCY)) {
						if( Float.parseFloat(dim.getAttribute(EmotionML.A_CONFIDENCE)) >= POTENCY_THRESHOLD ) {
							emotions[2] = EmotionML.fsrePotency2SemainePower(value);
							emotionsChanged = true;
						}
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
				/*     ------- OLD CODE removed by Florian: emotions[3] = -1*bored + interested; */
				if (  (( bored == 0.0 ) && (neutral == 0.0) && (interested >= 0.99))  
						||(( bored == 0.0 ) && (neutral >= 0.99) && (interested == 0.00))
						||(( bored >= 0.99 ) && (neutral >= 0.00) && (interested == 0.00)) ) {
					// the above cases indicate an invalid classifier output, in this case we assume neutral interest...
					emotions[3] = 0;
				} else {
					// a continuous emotion value is computed from the class probabilities:
					emotions[3] = ( 1 * bored + 2 * neutral + 3 * interested ) - 2;
				}
				/*     ------------ */

				emotionsChanged = true;
			}
		}
		
		//System.out.println("***** " + emotions[0] + " " + emotions[1] + " " + emotions[2] + " " + emotions[3]);
		return emotions;
	}

	/**
	 * Sends the given emotion with the given value to the UserState
	 * @param emotion	- the emotion to send
	 * @param value		- the value of the emotion to send
	 * @throws JMSException
	 */
	//public void sendEmotions( float valence, float arousal, float potency, float interest, String emoQuadrant, String emoQuadrantConfidence, String gender ) throws JMSException
	public void sendEmotions( float valence, float arousal, float potency, float interest, String gender ) throws JMSException
	{	
		boolean newValue = false;
		Map<String,String> userStateInfo = new HashMap<String,String>();
		if( valence != 0 ) {
			DMLogger.getLogger().log(meta.getTime(), "UserAction:Valence value="+valence);
			userStateInfo.put("valence",""+valence);
			newValue = true;
		}
		if( arousal != 0 ) {
			DMLogger.getLogger().log(meta.getTime(), "UserAction:Arousal value="+arousal);
			userStateInfo.put("arousal",""+arousal);
			newValue = true;
		}
		if( potency != 0 ) {
			DMLogger.getLogger().log(meta.getTime(), "UserAction:Potency value="+potency);
			userStateInfo.put("potency",""+potency);
			newValue = true;
		}
		if( interest != 0 ) {
			DMLogger.getLogger().log(meta.getTime(), "UserAction:Interest value="+interest);
			userStateInfo.put("interest",""+interest);
			newValue = true;
		}
		
//		if( emoQuadrant.length() > 0 && emoQuadrantConfidence.length() > 0 && Float.parseFloat(emoQuadrantConfidence) > QUADRANT_CONFIDENCE_THRESHOLD ) {
//			userStateInfo.put("emotion-quadrant",""+emoQuadrant);
//			newValue = true;
//		}
		
		if( gender != null && gender.length() > 0 ) {
			userStateInfo.put("gender",""+gender);
			newValue = true;
		}
		
		if( newValue ) {
//			System.out.println("++++++++++++++++++++++ Sending to User State");
//			for( String key : userStateInfo.keySet() ) {
//				System.out.println(key + ": " + userStateInfo.get(key));
//			}
			UserStateInfo usi = new UserStateInfo(userStateInfo	);
			userStateSender.sendStateInfo(usi, meta.getTime());
		}
	}
}
