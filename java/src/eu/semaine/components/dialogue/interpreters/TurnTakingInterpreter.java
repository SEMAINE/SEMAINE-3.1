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
import eu.semaine.jms.message.SEMAINEDialogStateMessage;
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
	private DialogStateReceiver dialogStateReceiver;
	
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
		dialogStateReceiver = new DialogStateReceiver("semaine.data.state.dialog");
		receivers.add(dialogStateReceiver);
		
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
		if( pauseTime != 0 && turntakingState == State.USER_SPEAKING && pauseTime + 1500 < meta.getTime() ) {
			changeTurnState( true, meta.getTime() );
			turntakingState = State.AGENT_WAITING;
			pauseTime = meta.getTime();
		} else if( pauseTime != 0 && turntakingState == State.AGENT_WAITING && pauseTime + 5000 < meta.getTime() ) {
			//changeTurnState( true, meta.getTime() );
			pauseTime = meta.getTime();
		}
	}
	
	/**
	 * Updates the time since the user is silent and the turn taking state
	 * based on the received message
	 */
	@Override
	public void react( SEMAINEMessage m ) throws JMSException
	{
		/* old
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
		*/
		
		if( !processTurnMessage(m) ) {
			if( isSilence(m) ) {
				pauseTime = meta.getTime();
			} else if( isSpeaking(m) ) {
				if( turntakingState == State.AGENT_WAITING ) {
					turntakingState = State.USER_SPEAKING;
					changeTurnState(false, meta.getTime());
				}
				pauseTime = 0;
			}
		}
	}
	
	public boolean processTurnMessage(SEMAINEMessage m)
	{
		if( m instanceof SEMAINEDialogStateMessage ) {
			DialogStateInfo dialogInfo = ((SEMAINEDialogStateMessage)m).getState();
			Map<String,String> dialogInfoMap = dialogInfo.getInfo();
			
			if( dialogInfoMap.get("speaker").equals("agent") ) {
				turntakingState = State.AGENT_WAITING;
				return true;
			} else if( dialogInfoMap.get("speaker").equals("user") ) {
				turntakingState = State.USER_SPEAKING;
				return true;
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
		if( m instanceof SEMAINEEmmaMessage ) {
			SEMAINEEmmaMessage em = (SEMAINEEmmaMessage)m;
			Element interpretation = em.getTopLevelInterpretation();
			if (interpretation != null) {
				List<Element> behaviours = em.getBehaviourElements(interpretation);
				for (Element behaviour : behaviours) {
					String fName = XMLTool.needAttribute(behaviour, SemaineML.A_NAME);
					//String fIntensity = XMLTool.needAttribute(behaviour, SemaineML.INTENSITY);
					if (fName.equals("silence")) {
						return true;
					}
				}
			}
		}
		return false;
		
		
		/* old
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
		*/
	}
	
	/**
	 * Checks if the Message is ASR output and if it contains a word.
	 * @param m the message to check
	 * @return true if the message says that the user is currently silent
	 */
	public boolean isSpeaking( SEMAINEMessage m ) throws JMSException
	{
		if( m instanceof SEMAINEEmmaMessage ) {
			SEMAINEEmmaMessage em = (SEMAINEEmmaMessage)m;
			Element interpretation = em.getTopLevelInterpretation();
			if (interpretation != null) {
				List<Element> behaviours = em.getBehaviourElements(interpretation);
				for (Element behaviour : behaviours) {
					String fName = XMLTool.needAttribute(behaviour, SemaineML.A_NAME);
					//String fIntensity = XMLTool.needAttribute(behaviour, SemaineML.INTENSITY);
					if (fName.equals("speaking")) {
						return true;
					}
				}
			}
		}
		return false;
		
		/* old
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
		*/
	}
	
	/**
	 * Changes the Turn-holder in the Dialog-state
	 * @param agentTurn - 	if true  -> Agent has turn
	 * 						if false -> User has turn
	 */
	public void changeTurnState( boolean agentTurn, long time ) throws JMSException
	{
		Map<String,String> dialogInfo = new HashMap<String,String>();
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
