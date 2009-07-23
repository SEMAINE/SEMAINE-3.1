/**
 * Copyright (C) 2009 University of Twente. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */

package eu.semaine.components.dialogue.interpreters;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.jms.JMSException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.components.dialogue.actionproposers.UtteranceActionProposer;
import eu.semaine.components.dialogue.datastructures.EmotionEvent;
import eu.semaine.datatypes.stateinfo.AgentStateInfo;
import eu.semaine.datatypes.stateinfo.DialogStateInfo;
import eu.semaine.datatypes.stateinfo.StateInfo;
import eu.semaine.datatypes.stateinfo.UserStateInfo;
import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.message.SEMAINEStateMessage;
import eu.semaine.jms.message.SEMAINEXMLMessage;
import eu.semaine.jms.receiver.StateReceiver;
import eu.semaine.jms.receiver.XMLReceiver;
import eu.semaine.jms.sender.StateSender;
import eu.semaine.util.XMLTool;

/**
 * The TurnTakingInterpreter looks at the behaviour of the user and has to decide when is
 * a good moment to start speaking. When this moment is decided the decision is send forward.
 * 
 * Input
 * UserStateReceiver('semaine.data.state.user')		--> user speaking state and detected emotions
 * AgentStateReceiver('semaine.data.state.agent')	--> current character
 * 
 * Output
 * AgentStateSender('semaine.data.state.agent')		--> take/release turn messages
 * 
 * @author MaatM
 *
 */

public class TurnTakingInterpreter extends Component
{
	/* Possible turn states */
	private final static int UNKNOWN_STATE = 0;
	private final static int WAITING = 0;
	private final static int SILENT = 1;
	private final static int SPEAKING = 2;
	
	private HashMap<String,Integer> charNumbers = new HashMap<String,Integer>();
	
	/* Take/release turn threshold */
	private final static int POPPY_TT_THRESHOLD = 60;
	private final static int PRUDENCE_TT_THRESHOLD = 80;
	private final static int SPIKE_TT_THRESHOLD = 30;
	private final static int OBADIAH_TT_THRESHOLD = 100;
	private int curr_TT_Threshold = 100;

	/* Senders and Receivers */
	private StateReceiver userStateReceiver;
	private StateReceiver agentStateReceiver;
	private StateReceiver dialogStateReceiver;
	private XMLReceiver contextReceiver;
	private StateSender agentStateSender;

	/* Turn state of speaker */
	private int userSpeakingState = 0; // 	0 = unknown, 1 = silence, 2 = speaking
	private long userSpeakingStateTime = 0;
	private long latestUtteranceLength = 0;
	
	/* Turn state of Agent */
	private int agentSpeakingState = 1; //	1 = silence, 2 = speaking
	private long agentSpeakingStateTime = 0;
	private int agentSpeakingIntention = 0; //	1 = silence, 2 = speaking
	private long agentSpeakingIntentionTime = 0;
	
	/*  The current character*/
	private static final String USER = "user";
	private int character;
	
	/* List of detected emotion events (generated by the EmotionInterpreter) */
	private ArrayList<EmotionEvent> detectedEmotions = new ArrayList<EmotionEvent>();
	private int index = 0;
	
	/* If a backchannel was given this silence */
	private boolean backchannel_given = false;

	/**
	 * Constructor of TurnTakingInterpreter
	 * Initializes the senders and receivers, and sets the waitingtime for Act()	
	 * @throws JMSException
	 */
	public TurnTakingInterpreter() throws JMSException
	{
		super("TurnTakingInterpreter");
		
		charNumbers.put("poppy", UtteranceActionProposer.POPPY);
		charNumbers.put("prudence", UtteranceActionProposer.PRUDENCE);
		charNumbers.put("spike", UtteranceActionProposer.SPIKE);
		charNumbers.put("obadiah", UtteranceActionProposer.OBADIAH);
		
		waitingTime = 50;

		userStateReceiver = new StateReceiver( "semaine.data.state.user", StateInfo.Type.UserState);
		receivers.add( userStateReceiver );
		agentStateReceiver = new StateReceiver( "semaine.data.state.agent", StateInfo.Type.AgentState);
		receivers.add( agentStateReceiver );
		dialogStateReceiver = new StateReceiver( "semaine.data.state.dialog", StateInfo.Type.DialogState);
		receivers.add( dialogStateReceiver );
		contextReceiver = new XMLReceiver("semaine.data.state.context");
		receivers.add( contextReceiver );
		
		agentStateSender = new StateSender( "semaine.data.state.agent", StateInfo.Type.AgentState, getName() );
		senders.add( agentStateSender );
	}

	
	/**
	 * Reads the message, filters out the detected user speaking state and the detected emotions, 
	 * and tries to determine if the agent should start speaking.
	 */
	public void react( SEMAINEMessage m ) throws JMSException
	{
		if (m instanceof SEMAINEStateMessage) {
			SEMAINEStateMessage sm = (SEMAINEStateMessage) m;
			StateInfo stateInfo = sm.getState();
			StateInfo.Type stateInfoType = stateInfo.getType();
			switch (stateInfoType) {
			case UserState:
				/* Processes User state updates */

				/* Updates user speaking state (speaking or silent) */
				setUserSpeakingState(stateInfo);
				
				/* Updates detected emotions (valence, arousal, interest) */
				addDetectedEmotions(stateInfo);
				
				/* called to determine the turn state of the agent */
				determineAgentTurn();
				break;

			case DialogState:
				/* Processes Dialog state updates */
				/* updates agent speaking state */
				setAgentSpeakingState(stateInfo);
				break;
				
			case AgentState:
				/* Processes Agent state changes */
				/* processes an agent backchannel */
				processBackchannel(stateInfo);
				break;
		    default:
		    	// We could complain here if we were certain we don't expect other state infos, as in:
		    	// throw new MessageFormatException("Unexpected state info type: "+stateInfo.getType().toString());
			}
		}
		
		/* Processes XML updates */
		if( m instanceof SEMAINEXMLMessage ) {
			SEMAINEXMLMessage xm = ((SEMAINEXMLMessage)m);
			
			/* Updates the current character and the user */
			updateCharacterAndUser( xm );
		}
	}
	
	public void updateCharacterAndUser( SEMAINEXMLMessage xm ) throws JMSException
	{
		Document doc = xm.getDocument();
		Element root = doc.getDocumentElement();
		if (!root.getTagName().equals(SemaineML.E_CONTEXT)) {
			return;
		}
		if (!root.getNamespaceURI().equals(SemaineML.namespaceURI)) {
			throw new MessageFormatException("Unexpected document element namespace: expected '"+SemaineML.namespaceURI+"', found '"+root.getNamespaceURI()+"'");
		}
		
		List<Element> characters = XMLTool.getChildrenByTagNameNS(root, SemaineML.E_USER, SemaineML.namespaceURI);
		for( Element characterElem : characters ) {
			
			String charName = characterElem.getAttribute(SemaineML.A_NAME);
			if (!charName.equals(USER)) {
				Integer charInt = charNumbers.get(charName.toLowerCase());
				if( charInt != null ) {
					character = charInt;
				}
			}
		}
	}
	
	/**
	 * Called every 50ms, reevaluates the current situation and tries to determine if the agent
	 * should start talking. 
	 */
	public void act() throws JMSException
	{
		determineAgentTurn();
	}
	
	/**
	 * Reads the received Message and tries to filter out the detected user speaking state.
	 * @param m - the received message
	 */
	public void setAgentSpeakingState(StateInfo dialogInfo)
	{
		Map<String,String> userInfoMap = dialogInfo.getInfo();

		if( userInfoMap.get("speaker").equals("agent") ) {
			backchannel_given = false;
			agentSpeakingState = SPEAKING;
			agentSpeakingStateTime = meta.getTime();
		} else if( userInfoMap.get("listener").equals("agent") ) {
			backchannel_given = false;
			agentSpeakingState = SILENT;
			agentSpeakingStateTime = meta.getTime();
			if( userSpeakingState == SILENT ) {
				userSpeakingState = WAITING;
				userSpeakingStateTime = meta.getTime();
			}
		}
	}

	/**
	 * Reads the received Message and tries to filter out the detected user speaking state.
	 * @param m - the received message
	 */
	public void setUserSpeakingState(StateInfo userInfo)
	{
		Map<String,String> userInfoMap = userInfo.getInfo();

		if( userInfoMap.get("behaviour").equals("speaking") ) {
			if( userSpeakingState != SPEAKING ) {
				backchannel_given = false;
				userSpeakingState = SPEAKING;
				userSpeakingStateTime = meta.getTime();
			}
		} else if( userInfoMap.get("behaviour").equals("silence") ) {
			if( userSpeakingState != SILENT ) {
				latestUtteranceLength = meta.getTime() - userSpeakingStateTime;
				userSpeakingState = SILENT;
				userSpeakingStateTime = meta.getTime();
			}
		}
	}
	
	/**
	 * Reads the received Message and tries to filter out a change of character
	 * @param am - the received message
	 */
	public void setCharacter(StateInfo agentInfo)
	{
		Map<String,String> agentInfoMap = agentInfo.getInfo();
		
		String newChar = agentInfoMap.get( "character" );
		if( newChar != null ) {
			if( newChar.equals("poppy") ) {
				character = UtteranceActionProposer.POPPY;
				curr_TT_Threshold = POPPY_TT_THRESHOLD;
			} else if( newChar.equals("prudence") ) {
				character = UtteranceActionProposer.PRUDENCE;
				curr_TT_Threshold = PRUDENCE_TT_THRESHOLD;
			} else if( newChar.equals("spike") ) {
				character = UtteranceActionProposer.SPIKE;
				curr_TT_Threshold = SPIKE_TT_THRESHOLD;
			} else if( newChar.equals("obadiah") ) {
				character = UtteranceActionProposer.OBADIAH;
				curr_TT_Threshold = OBADIAH_TT_THRESHOLD;
			}
		}
	}
	
	public void processBackchannel(StateInfo agentInfo)
	{
		Map<String,String> agentInfoMap = agentInfo.getInfo();
		
		String intention = agentInfoMap.get( "intention" );
		if( intention != null && intention.equals("backchannel") ) {
			backchannel_given = true;
		}
	}
	
	/**
	 * Reads the received Message and tries to filter out the detected Emotion Events.
	 * @param m - the received message
	 */
	public void addDetectedEmotions(StateInfo userInfo)
	{
		Map<String,String> dialogInfoMap = userInfo.getInfo();
		
		if( dialogInfoMap.get("behaviour").equals("valence") ) {
			float intensity = Float.parseFloat( dialogInfoMap.get("behaviour intensity") );
			EmotionEvent ee = new EmotionEvent( meta.getTime(), 0, EmotionEvent.VALENCE, intensity );
			detectedEmotions.add( ee );
		} else if( dialogInfoMap.get("behaviour").equals("arousal") ) {
			float intensity = Float.parseFloat( dialogInfoMap.get("behaviour intensity") );
			EmotionEvent ee = new EmotionEvent( meta.getTime(), 0, EmotionEvent.AROUSAL, intensity );
			detectedEmotions.add( ee );
		} else if( dialogInfoMap.get("behaviour").equals("interest") ) {
			float intensity = Float.parseFloat( dialogInfoMap.get("behaviour intensity") );
			EmotionEvent ee = new EmotionEvent( meta.getTime(), 0, EmotionEvent.INTEREST, intensity );
			detectedEmotions.add( ee );
		}
	}
	
	/**
	 * Determines if the agent should start speaking or not. It bases this decision
	 * on the speaking_intention, a value for how much the agent wants the turn.
	 */
	public void determineAgentTurn() throws JMSException
	{
		int speakingIntention = getSpeakingIntentionValue();
		if( speakingIntention >= curr_TT_Threshold && agentSpeakingIntention != SPEAKING ) {
			agentSpeakingIntention = SPEAKING;
			agentSpeakingIntentionTime = meta.getTime();
			sendAgentTurnState();
		} else if( speakingIntention < curr_TT_Threshold && agentSpeakingIntention != SILENT ) {
			agentSpeakingIntention = SILENT;
			agentSpeakingIntentionTime = meta.getTime();
			sendAgentTurnState();
		}
	} 
	
	/**
	 * Calculates the speaking intention value for the current moment.
	 * This is a value for how much the agent wants the turn.
	 * This is calculated based on detected emotion events, the user speaking state, 
	 * the duration of this state, and the agent speaking state plus its duration.
	 * The higher the value, the higher the intention to speak.
	 * 
	 * Currently, the speaking intention value is the sum of three other values: 
	 * user_silence_time_value, emotion_value and time_turn_value.
	 * user_silence_time_value: 	a value between 0 and 100 which is higher when the user is silent for a longer time (max is reached after 0.8 seconds).
	 * emotion_value: 				a value based on the number of detected emotion events. More detected events lead to a higher value.
	 * agent_silence_time_value: 	a value that's higher when the agent is silent for a longer period (max is reached after 30 seconds).
	 * @return
	 */
	public int getSpeakingIntentionValue()
	{
		int speakingIntention = 0;
		
		/* The components of the speaking intention value */
		int user_silence_time_value = 0;
		int emotion_value = 0;
		int agent_silence_time_value = 0;
		
		/* silence_time_value */
		if( userSpeakingState == SILENT ) {
			double time = ((double)meta.getTime() - (double)userSpeakingStateTime)/1000;
			if( backchannel_given ) {
				time = time - 1;
			}
			//double value = Math.max( Math.min( -(Math.sqrt(time))+1, 100 ), 0 );
			double value;
			if( time >= 0 ) {
				value = Math.max( Math.min( Math.pow(time+0.3,2), 1 ), 0 );
			} else {
				value = 0;
			}
			user_silence_time_value = (int)(value * 100);
		}
		
		/* emotion_value */
		emotion_value = (detectedEmotions.size()-index)*10;
		
		/* time_turn_value */
		if( agentSpeakingState == SILENT ) {
			double time = ((double)meta.getTime() - (double)agentSpeakingStateTime)/1000;
			double value = Math.min( (4/3)*time, 30 );
			agent_silence_time_value = (int)(value);
		}
		
		/* Calculating the speaking intention value */
		speakingIntention = user_silence_time_value + emotion_value + agent_silence_time_value;
		//System.out.println( speakingIntention + "	= " + user_silence_time_value + "	+ " + emotion_value + " + " + agent_silence_time_value + "			: currTime: " + meta.getTime() );
		
		return speakingIntention;
	}
	
	public void sendAgentTurnState() throws JMSException
	{
		Map<String,String> agentStateInfo = new HashMap<String,String>();
		if( agentSpeakingIntention == SPEAKING ) {
			agentStateInfo.put("intention", "speaking");
		} else if( agentSpeakingIntention == SILENT ) {
			agentStateInfo.put("intention", "silence");
		} else {
			return;
		}
		
		AgentStateInfo asi = new AgentStateInfo( agentStateInfo );
		agentStateSender.sendStateInfo(asi, meta.getTime());	
	}
}
