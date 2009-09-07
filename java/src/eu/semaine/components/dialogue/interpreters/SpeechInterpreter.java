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
import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.jms.message.SEMAINEEmmaMessage;
import eu.semaine.jms.message.SEMAINEFeatureMessage;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.receiver.EmmaReceiver;
import eu.semaine.jms.receiver.FeatureReceiver;
import eu.semaine.jms.sender.StateSender;
import eu.semaine.util.XMLTool;
 	
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
		emmaReceiver = new EmmaReceiver("semaine.data.state.user.emma");
		receivers.add(emmaReceiver);
		
		userStateSender = new StateSender("semaine.data.state.user.behaviour", StateInfo.Type.UserState, getName());
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
			for( int i=0; i<featureNames.length; i++ ) {
				if( featureNames[i].equals("RMSenergy") ) {
					rmsEnergy = featureValues[i];
				}
			}
		} else if( m instanceof SEMAINEEmmaMessage ) {
			SEMAINEEmmaMessage em = (SEMAINEEmmaMessage)m;
			//System.out.println("\n\n" + em.getText() );
			
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
				
				List<Element> pitchList = em.getPitchElements(interpretation);
				for (Element pitchElem : pitchList) {
					if( pitchElem.hasAttribute(SemaineML.A_DIRECTION) ) {
						String direction = pitchElem.getAttribute(SemaineML.A_DIRECTION);
						sendPitchDirection( direction );
					}
				}
				
			}
			
			/* Reading words */
			//System.out.println(em.getText());
			/*
			Element wordSequence = em.getSequence();
			if( wordSequence != null ) {
				System.out.println("WordSequence found");
				List<Element> wordElements = XMLTool.getChildrenByLocalNameNS(wordSequence, EMMA.E_INTERPRETATION, EMMA.namespaceURI);
				if( wordElements.size() > 0 ) {
					System.out.println("WordElements found");
					String detectedKeywords = "";
					String starttime = null;
					for( Element wordElem : wordElements ) {
						if( wordElem.hasAttribute("tokens") ) {
							detectedKeywords = detectedKeywords + wordElem.getAttribute("tokens") + " ";
						}
						if( starttime == null && wordElem.hasAttribute("offset-to-start") ) {
							starttime = wordElem.getAttribute("offset-to-start");
						}
					}
					detectedKeywords = detectedKeywords.trim();
					if(starttime != null) {
						sendDetectedKeywords(detectedKeywords, starttime);
					}
				}
			}
			*/
		}
		/* Determines the new user speaking state */
		//if( speakingParam && rmsEnergy >= 0.003 ) {
		if( speakingParam ) {
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
	
	public void sendPitchDirection( String direction ) throws JMSException
	{
		Map<String,String> userStateInfo = new HashMap<String,String>();
		userStateInfo.put("pitchDirection", direction);
		
		UserStateInfo usi = new UserStateInfo(userStateInfo	);
		userStateSender.sendStateInfo(usi, meta.getTime());	
	}
	
	/**
	 * Sends the current user speech state as an EMMA message around
	 * @throws JMSException
	 */
	public void sendSpeechState(  ) throws JMSException
	{
		Map<String,String> userStateInfo = new HashMap<String,String>();
		if( speaking ) {
			userStateInfo.put("speaking", "true");
		} else {
			userStateInfo.put("speaking", "false");
		}
		
		UserStateInfo usi = new UserStateInfo(userStateInfo	);
		userStateSender.sendStateInfo(usi, meta.getTime());	
	}
	
	public void sendDetectedKeywords( String detectedKeywords, String starttime ) throws JMSException
	{
		Map<String,String> userStateInfo = new HashMap<String,String>();
		userStateInfo.put("userUtterance", detectedKeywords);
		userStateInfo.put("userUtteranceStartTime", starttime);
		System.out.println("Detected the following utterance (t:" + starttime + "): " + detectedKeywords + ".");
		
		UserStateInfo usi = new UserStateInfo(userStateInfo	);
		userStateSender.sendStateInfo(usi, meta.getTime());	
	}
}
