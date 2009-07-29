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
import eu.semaine.jms.message.SEMAINEEmmaMessage;
import eu.semaine.jms.message.SEMAINEFeatureMessage;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.receiver.EmmaReceiver;
import eu.semaine.jms.receiver.FeatureReceiver;
import eu.semaine.jms.sender.StateSender;
 	
/**
 * The SpeechInterpreter determines exactly if the user is speaking or if he's silent.
 * It receives audiofeatures, and if the right mix of features appear it will send
 * a 'speaking' or 'silence' EMMA message.
 * 
 * Input
 * FeatureReceiver('semaine.data.analysis.features.voice')
 * 
 * Output
 * EmmaSender('semaine.data.state.user.emma')
 * 
 * 
 * @author Mark ter Maat
 */

public class SpeechInterpreter extends Component
{
	private float rmsEnergy = -999f;
	private boolean speakingParam = false;
	
	/* The current speaking state of the user */
	private boolean speaking = false;
	
	/* Senders and Receivers */
	private FeatureReceiver featureReceiver;
	private EmmaReceiver emmaReceiver;
	private StateSender userStateSender;
	
	/**
	 * Constructor of SpeechInterpreter
	 * Initializes the Senders and Receivers
	 * @throws JMSException
	 */
	public SpeechInterpreter() throws JMSException
	{
		super( "SpeechInterpreter" );
		
		featureReceiver = new FeatureReceiver("semaine.data.analysis.features.voice");
		receivers.add(featureReceiver);
		emmaReceiver = new EmmaReceiver("semaine.data.state.user.emma ");
		receivers.add(emmaReceiver);
		
		userStateSender = new StateSender("semaine.data.state.user", StateInfo.Type.UserState, getName());
		senders.add(userStateSender);
	}
	
	/**
	 * Reads the FeatureMessage, reads the values for 'rmsEnergy' and 'speaking'
	 * and decides, based on these 2 values, if the agent is speaking or not.
	 * If this new value is different than the current state of the user an 
	 * EMMA message is send with the new user speaking state.
	 */
	public void react( SEMAINEMessage m ) throws JMSException
	{
		if( m instanceof SEMAINEFeatureMessage ) {
			SEMAINEFeatureMessage fm = (SEMAINEFeatureMessage)m;
			
			/* Reads the feature names and values */
			String[] featureNames = fm.getFeatureNames();
			float[] featureValues = fm.getFeatureVector();
			
			/* Find the values for 'rmsEnergy' and 'speaking' */
			float rmsEnergy = -999f;
			boolean speakingParam = false;
			for( int i=0; i<featureNames.length; i++ ) {
				if( featureNames[i].equals("RMSenergy ") ) {
					rmsEnergy = featureValues[i];
				}
				if( featureNames[i].equals("speaking") ) {
					speakingParam = ( featureValues[i] == 1 );
				}
			}
		} else if( m instanceof SEMAINEEmmaMessage ) {
			SEMAINEEmmaMessage em = (SEMAINEEmmaMessage)m;
			
			Element interpretation = em.getTopLevelInterpretation();
			if (interpretation != null) {
				List<Element> speakingList = em.getSpeakingElements(interpretation);
				for (Element speaking : speakingList) {
					if( speaking.hasAttribute("statusChange") ) {
						if( speaking.getAttribute("statusChange").equals("start") ) {
							speakingParam = true;
						} else if( speaking.getAttribute("statusChange").equals("stop") ) {
							speakingParam = false;
						}
					}
				}
			}
		}
		/* Determines the new user speaking state */
		if( speakingParam && rmsEnergy >= 0.003 ) {
			if( !speaking ) {
				/* Change the user speaking state to 'true' and send this change around */
				speaking = true;
				sendSpeechState(  );
			}
		} else {
			if( speaking ) {
				/* Change the user speaking state to 'false' and send this change around */
				speaking = false;
				sendSpeechState(  );
			}
		}
	}
	
	/**
	 * Sends the current user speech state as an EMMA message around
	 * @throws JMSException
	 */
	public void sendSpeechState(  ) throws JMSException
	{
		Map<String,String> userStateInfo = new HashMap<String,String>();
		if( speaking ) {
			//System.out.println("User Speaking");
			userStateInfo.put("speaking", "true");
		} else {
			//System.out.println("User Speaking");
			userStateInfo.put("speaking", "false");
		}
		
		UserStateInfo usi = new UserStateInfo(userStateInfo	);
		userStateSender.sendStateInfo(usi, meta.getTime());	
	}
}
