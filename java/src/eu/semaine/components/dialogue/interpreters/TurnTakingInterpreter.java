/**
 * Copyright (C) 2008 University of Twente, HMI. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */

package eu.semaine.components.dialogue.interpreters;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.jms.JMSException;

import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.components.dialogue.util.CyclicBuffer;
import eu.semaine.datatypes.stateinfo.DialogStateInfo;
import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.jms.message.SEMAINEEmmaMessage;
import eu.semaine.jms.message.SEMAINEFeatureMessage;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.receiver.AgentStateReceiver;
import eu.semaine.jms.receiver.DialogStateReceiver;
import eu.semaine.jms.receiver.EmmaReceiver;
import eu.semaine.jms.receiver.FeatureReceiver;
import eu.semaine.jms.receiver.UserStateReceiver;
import eu.semaine.jms.sender.StateSender;
import eu.semaine.util.XMLTool;

/**
 * This class interprets and analyses the turn taking behaviour of the user
 * 
 * @author Mark tM
 * @version 0.1 - dummy class
 *
 */

public class TurnTakingInterpreter extends Component 
{	
	/* Receivers */
	private EmmaReceiver emmaReceiver;
	private UserStateReceiver userStateReceiver;
	private FeatureReceiver featureReceiver;
	
	/* Senders */
	private StateSender dialogStateSender;
	
	/* Turn taking data */
	public static enum State { USER_SPEAKING, AGENT_WAITING }
	private State turntakingState = State.AGENT_WAITING;
	private long pauseTime = 0;
	private boolean userSpeaking = false;
	private CyclicBuffer<Float> logEnergyBuffer = new CyclicBuffer<Float>(20);
	private CyclicBuffer<Float> foStrengthBuffer = new CyclicBuffer<Float>(30);
	
	
	/**
	 * Constructor DialogInterpreter
	 */
	public TurnTakingInterpreter() throws JMSException
	{
		super("TurnTakingInterpreter");
		
		/* Define Receivers */
		emmaReceiver = new EmmaReceiver("semaine.data.state.user", "datatype = 'EMMA'");
		receivers.add(emmaReceiver);
		userStateReceiver = new UserStateReceiver("semaine.data.state.user", "datatype = 'UserState'");
		receivers.add(userStateReceiver);
		featureReceiver = new FeatureReceiver("semaine.data.analysis.features.voice");
		receivers.add(featureReceiver);
		
		/* Define Senders */
		dialogStateSender = new StateSender("semaine.data.state.dialog", "DialogState", getName());
		senders.add(dialogStateSender);
		
	}
	
	/**
	 * Checks if the turn has switched to the agent
	 */
	@Override
	public void act() throws JMSException
	{
		if( pauseTime != 0 && turntakingState == State.USER_SPEAKING && pauseTime + 1500 < System.currentTimeMillis() ) {
			changeTurnState( true, System.currentTimeMillis() );
			turntakingState = State.AGENT_WAITING;
			pauseTime = System.currentTimeMillis();
		} else if( pauseTime != 0 && turntakingState == State.AGENT_WAITING && pauseTime + 5000 < System.currentTimeMillis() ) {
			changeTurnState( true, System.currentTimeMillis() );
			pauseTime = System.currentTimeMillis();
		}
	}
	
	/**
	 * Updates the time since the user is silent and the turn taking state
	 * based on the received message
	 */
	@Override
	public void react( SEMAINEMessage m ) throws JMSException
	{
		System.out.print(".");
		if( processSpeakingFeatures(m) ) {
			if( userSpeaking ) {
				if( turntakingState == State.AGENT_WAITING ) {
					turntakingState = State.USER_SPEAKING;
				}
				pauseTime = 0;
			} else {
				pauseTime = System.currentTimeMillis();
			}
		}
		
		
		
		/* old
		if( isSilence(m) ) {
			pauseTime = System.currentTimeMillis();
		} else if( isSpeaking(m) ) {
			if( turntakingState == State.AGENT_WAITING ) {
				turntakingState = State.USER_SPEAKING;
			}
			pauseTime = 0;
		}
		*/
	}
	
	/**
	 * Processes the speaking-features and update the speaking/listening state
	 * @param m
	 */
	public boolean processSpeakingFeatures(SEMAINEMessage m) throws JMSException
	{
		if( m instanceof SEMAINEFeatureMessage ) {
			System.out.println(m.getText());
			SEMAINEFeatureMessage fm = (SEMAINEFeatureMessage)m;
			String[] featureNames = fm.getFeatureNames();
			float[] featuresValues = fm.getFeatureVector();
			for( int i=0; i<featureNames.length; i++ ) {
				String name = featureNames[i];
				if( name.equals("logEnergy") ) {
					float value = featuresValues[i];
					logEnergyBuffer.putValue(value);
					ArrayList<Float> values = logEnergyBuffer.getValues();
					float avg = 0;
					for( Float f : values ) {
						if( f != null ) {
							avg = avg + f;
						}
					}
					avg = avg / values.size();
					if( userSpeaking ) {
						if( avg < -10 ) {
							userSpeaking = false;
							System.out.println("User silent");
							return true;
						}
					} else {
						if( avg > -8 ) {
							System.out.println("User speaking");
							userSpeaking = true;
							return true;
						}
					}
					
				} else if( name.equals("F0strength") ) {
					float value = featuresValues[i];
					foStrengthBuffer.putValue(value);
					ArrayList<Float> values = foStrengthBuffer.getValues();
					
					boolean hasPeaks = false;
					int peakCount = 0;
					int peakSubCount = 0;
					float threshold;
					if( userSpeaking ) {
						threshold = 0.45f;
					} else {
						threshold = 0.55f;
					}
					for( Float f : values ) {
						if( f != null ) {
							if( f >= threshold ) {
								peakSubCount++;
								
							} else {
								if( peakSubCount >= 3 ) {
									peakCount++;
									peakSubCount = 0;
								}
							}
						}
					}
					if( peakCount >= 3 && !userSpeaking ) {
						userSpeaking = true;
						System.out.println("User speaking");
						return true;
					} else if( peakCount < 3 & userSpeaking ) {
						userSpeaking = false;
						System.out.println("User silent");
						return true;
					}
				}
			}
		}
		return false;
	}

	
	/**
	 * Checks if the Message is ASR output and if it contains Silence.
	 * @param m the message to check
	 * @return true if the message says that the user is currently silent
	 */
	public boolean isSilence( SEMAINEMessage m ) throws JMSException
	{
		/* This code should be used when the silence-behaviour is send in an EMMA message
		if( m instanceof SEMAINEEmmaMessage ) {
			SEMAINEEmmaMessage em = (SEMAINEEmmaMessage)m;
			Element interpretation = em.getTopLevelInterpretation();
			if (interpretation != null) {
				List<Element> behaviours = em.getBehaviourElements(interpretation);
				for (Element behaviour : behaviours) {
					String fName = XMLTool.needAttribute(behaviour, SemaineML.NAME);
					//String fIntensity = XMLTool.needAttribute(behaviour, SemaineML.INTENSITY);
					if (fName.equals("silence")) {
						return true;
					}
				}
			}
		}
		return false;
		*/
		if( m instanceof SEMAINEFeatureMessage ) {
			SEMAINEFeatureMessage fm = (SEMAINEFeatureMessage)m;
			String[] featureNames = fm.getFeatureNames();
			float[] featuresValues = fm.getFeatureVector();
			for( int i=0; i<featureNames.length; i++ ) {
				String name = featureNames[i];
				if( name.equals("speaking") ) {
					if( featuresValues[i] == 0 ) {
						return true;
					} else {
						return false;
					}
				}
			}
		}
		return false;
	}
	
	/**
	 * Checks if the Message is ASR output and if it contains a word.
	 * @param m the message to check
	 * @return true if the message says that the user is currently silent
	 */
	public boolean isSpeaking( SEMAINEMessage m ) throws JMSException
	{
		/* This code should be used when the silence-behaviour is send in an EMMA message
		if( m instanceof SEMAINEEmmaMessage ) {
			SEMAINEEmmaMessage em = (SEMAINEEmmaMessage)m;
			Element interpretation = em.getTopLevelInterpretation();
			if (interpretation != null) {
				List<Element> behaviours = em.getBehaviourElements(interpretation);
				for (Element behaviour : behaviours) {
					String fName = XMLTool.needAttribute(behaviour, SemaineML.NAME);
					//String fIntensity = XMLTool.needAttribute(behaviour, SemaineML.INTENSITY);
					if (fName.equals("speaking")) {
						return true;
					}
				}
			}
		}
		return false;
		*/
		if( m instanceof SEMAINEFeatureMessage ) {
			SEMAINEFeatureMessage fm = (SEMAINEFeatureMessage)m;
			String[] featureNames = fm.getFeatureNames();
			float[] featuresValues = fm.getFeatureVector();
			for( int i=0; i<featureNames.length; i++ ) {
				String name = featureNames[i];
				if( name.equals("speaking") ) {
					if( featuresValues[i] == 1 ) {
						return true;
					} else {
						return false;
					}
				}
			}
		}
		return false;
	}
	
	/**
	 * Changes the Turn-holder in the Dialog-state
	 * @param agentTurn - 	if true  -> Agent has turn
	 * 						if false -> User has turn
	 */
	public void changeTurnState( boolean agentTurn, long time ) throws JMSException
	{
		Map<String,String>dialogInfo = new HashMap<String,String>();
		if( agentTurn ) {
			dialogInfo.put("speaker", "agent");
			dialogInfo.put("listener", "user");
		} else {
			dialogInfo.put("listener", "agent");
			dialogInfo.put("speaker", "user");
		}
		DialogStateInfo dsi = new DialogStateInfo(dialogInfo, null);
		dialogStateSender.sendStateInfo(dsi, time);
	}
}
