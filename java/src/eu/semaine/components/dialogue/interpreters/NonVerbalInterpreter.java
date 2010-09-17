/**
 * Copyright (C) 2009 University of Twente. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */

package eu.semaine.components.dialogue.interpreters;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;

import javax.jms.JMSException;

import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.datatypes.stateinfo.StateInfo;
import eu.semaine.datatypes.stateinfo.UserStateInfo;
import eu.semaine.datatypes.xml.BML;
import eu.semaine.datatypes.xml.EMMA;
import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.jms.message.SEMAINEEmmaMessage;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.receiver.EmmaReceiver;
import eu.semaine.jms.sender.StateSender;
import eu.semaine.util.XMLTool;

/**
 * The NonVerbalInterpreter receives Non-Verbal data from the NonVerbal Fusion component, 
 * and if the confidence is higher than the specified threshold it will put it in the userstate.
 * 
 * Input
 * EmmaReceiver('semaine.data.state.user.emma.nonverbal')
 * 
 * Output
 * StateSender('semaine.data.state.user.behaviour')
 * 
 * 
 * @author Mark ter Maat
 */

public class NonVerbalInterpreter extends Component
{
	/* Threshold values */
	private final static double THRESHOLD_HEADGESTURE = 0.2;
	private final static double THRESHOLD_PITCH = 0.1;
	private final static double THRESHOLD_VOCALIZATION = 0.1;
	private final static double THRESHOLD_FACEPRESENCE = 0.33;
	private final static double THRESHOLD_ACTIONUNITS = 0.3;
	private final static double THRESHOLD_SPEAKING = 0.3;
	
	/* Senders and Receivers */
	private EmmaReceiver emmaReceiver;
	private StateSender userStateSender;
	
	private HashSet<Integer> activeActionUnits = new HashSet<Integer>();
	
	/**
	 * Constructor of NonVerBalInterpreter
	 * @throws JMSException
	 */
	public NonVerbalInterpreter() throws JMSException
	{
		super( "NonVerbalInterpreter" );
		emmaReceiver = new EmmaReceiver("semaine.data.state.user.emma.nonverbal", "datatype = 'EMMA'");
		receivers.add(emmaReceiver);
		
		userStateSender = new StateSender("semaine.data.state.user.behaviour", StateInfo.Type.UserState, getName());
		senders.add(userStateSender);
	}
	
	/**
	 * Checks if the received message is an EmmaMessage, and based on the content it will check
	 * if the confidence is high enough to put the received data in the UserState.
	 */
	public void react( SEMAINEMessage m ) throws JMSException
	{
		if( m instanceof SEMAINEEmmaMessage ) {
			SEMAINEEmmaMessage em = (SEMAINEEmmaMessage)m;
			Element interpretation = em.getTopLevelInterpretation();
			if( interpretation != null ) {
			
				double confidence;
				if( interpretation.hasAttribute(EMMA.A_CONFIDENCE) ) {
					confidence = Double.parseDouble(interpretation.getAttribute(EMMA.A_CONFIDENCE));
				} else {
					return;
				}
				
				/* Process BML elements */
				List<Element> bmlElements = em.getBMLElements(interpretation);
				for( Element bmlElement : bmlElements ) {
					Element headElement = XMLTool.getChildElementByTagNameNS(bmlElement, BML.E_HEAD, BML.namespaceURI);
					
					/* Process head movements */
					if( headElement != null && headElement.hasAttribute(BML.A_TYPE) && headElement.hasAttribute(BML.A_START) && headElement.hasAttribute(BML.A_END) ) {
						String moveType = headElement.getAttribute(BML.A_TYPE);
						String moveStart = headElement.getAttribute(BML.A_START);
						String moveEnd = headElement.getAttribute(BML.A_END);
						String[] shortNames = {"headGesture","headGestureStarted","headGestureStopped"};
						String[] values = {moveType, moveStart, moveEnd};
						if( confidence >= THRESHOLD_HEADGESTURE ) {
							sendUserStateChange(shortNames, values);
						}
					}
				}
				
				/* Process Speech/Non-Speech */
				List<Element> speakingElements = em.getSpeakingElements(interpretation);
				for( Element speakingElement : speakingElements ) {
					if( speakingElement.hasAttribute(SemaineML.A_STATUS_CHANGE) ) {
						String speaking = speakingElement.getAttribute(SemaineML.A_STATUS_CHANGE);
						if( confidence >= THRESHOLD_SPEAKING ) {
							if( speaking.equals("start") ) {
								sendUserStateChange("speaking", "true");
							} else {
								sendUserStateChange("speaking", "false");
							}
						}
					}
				}
				
				/* Process Pitch elements */
				List<Element> pitchElements = em.getPitchElements(interpretation);
				for( Element pitchElement : pitchElements ) {
					if( pitchElement.hasAttribute(SemaineML.A_DIRECTION) ) {
						String pitchDirection = pitchElement.getAttribute(SemaineML.A_DIRECTION);
						if( confidence >= THRESHOLD_PITCH ) {
							sendUserStateChange("pitchDirection", pitchDirection);
						}
					}
				}
				
				/* Process Non-verbal vocalisations */
				List<Element> vocalizationElements = em.getVocalizationElements(interpretation);
				for( Element vocalizationElement : vocalizationElements ) {
					if( vocalizationElement.hasAttribute(SemaineML.A_NAME) ) {
						String vocalization = vocalizationElement.getAttribute(SemaineML.A_NAME);
						if( confidence >= THRESHOLD_VOCALIZATION ) {
							sendUserStateChange("vocalization", vocalization);
						}
					}
				}
				
				/* Process Face Presence */
				List<Element> facePresenceElements = em.getFacePresentElements(interpretation);
				for( Element facePresenceElement : facePresenceElements ) {
					if( facePresenceElement.hasAttribute(SemaineML.A_STATUS_CHANGE) ) {
						String facePresence = facePresenceElement.getAttribute(SemaineML.A_STATUS_CHANGE);
						if( confidence >= THRESHOLD_FACEPRESENCE ) {
							if( facePresence.equals("start") ) {
								sendUserStateChange("facePresent", "true");
							} else {
								sendUserStateChange("facePresent", "false");
							}
						}
					}
				}
			}
			
			/* Process Action Units */
			Element group = em.getGroup();
			if( group != null ) {
				HashSet<Integer> auList = new HashSet<Integer>();
				String time = ""+meta.getTime();
				
				List<Element> interpretationList = XMLTool.getChildrenByLocalNameNS(group, EMMA.E_INTERPRETATION, EMMA.namespaceURI);
				for( Element inter : interpretationList ) {
					double conf = 0;
					if( inter.hasAttribute(EMMA.A_OFFSET_TO_START) ) {
						time = inter.getAttribute(EMMA.A_OFFSET_TO_START);
					}
					if( inter.hasAttribute(EMMA.A_CONFIDENCE) ) {
						conf = Double.parseDouble(inter.getAttribute(EMMA.A_CONFIDENCE));
					}
					List<Element> bmlList = em.getBMLElements(inter);
					for( Element bml : bmlList ) {
						Element face = XMLTool.getChildElementByTagNameNS(bml, BML.E_FACE, BML.namespaceURI);
						if( face != null && face.hasAttribute("au") ) {
							if( conf >= THRESHOLD_ACTIONUNITS ) {
								auList.add(Integer.parseInt(face.getAttribute("au").replaceAll("au", "")));
							}
						}
					}
				}
				if( ! auList.equals(activeActionUnits) ) {
					activeActionUnits = auList;
					ArrayList<Integer> ausToSend = new ArrayList<Integer>();
					ausToSend.addAll(activeActionUnits);
					Collections.sort(ausToSend);
					String aus = "";
					for( Integer au : ausToSend ) {
						aus = aus + au + " ";
					}
					aus = aus.trim();
					String[] shortNames = {"facialActionUnits","facialActionUnitsStarted"};
					String[] values = {aus, time};
					sendUserStateChange(shortNames, values);
				}
			}
		}
	}
	
	/**
	 * Sends the given variable name and value to the UserState.
	 */
	public void sendUserStateChange( String shortName, String value ) throws JMSException
	{
		String[] shortNames = {shortName};
		String[] values = {value};
		sendUserStateChange(shortNames,values);
	}
	
	/**
	 * Sends the given variable names and values to the UserState.
	 */
	public void sendUserStateChange( String[] shortNames, String[] values ) throws JMSException
	{
		//System.out.println("**********************************************************");
		//System.out.print("Sending: ");
		if( shortNames.length != values.length ) return;
		Map<String,String> userStateInfo = new HashMap<String,String>();
		for( int i=0; i<shortNames.length; i++ ) {
			//System.out.print("("+shortNames[i]+"|"+values[i]+")");
			userStateInfo.put(shortNames[i], values[i]);
		}
		//System.out.println();
		
		UserStateInfo usi = new UserStateInfo(userStateInfo	);
		userStateSender.sendStateInfo(usi, meta.getTime());
	}
}
