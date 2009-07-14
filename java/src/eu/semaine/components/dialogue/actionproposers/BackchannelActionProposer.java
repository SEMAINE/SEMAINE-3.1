package eu.semaine.components.dialogue.actionproposers;

import java.util.HashMap;
import java.util.Map;
import java.util.Random;

import javax.jms.JMSException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eu.semaine.datatypes.stateinfo.AgentStateInfo;
import eu.semaine.datatypes.stateinfo.DialogStateInfo;
import eu.semaine.datatypes.stateinfo.UserStateInfo;
import eu.semaine.datatypes.xml.FML;
import eu.semaine.jms.message.SEMAINEAgentStateMessage;
import eu.semaine.jms.message.SEMAINEDialogStateMessage;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.message.SEMAINEUserStateMessage;
import eu.semaine.jms.receiver.AgentStateReceiver;
import eu.semaine.jms.receiver.DialogStateReceiver;
import eu.semaine.jms.receiver.UserStateReceiver;
import eu.semaine.jms.sender.FMLSender;
import eu.semaine.jms.sender.StateSender;
import eu.semaine.util.XMLTool;

public class BackchannelActionProposer extends eu.semaine.components.Component
{
	/* Possible turn states */
	private final static int UNKNOWN_STATE = 0;
	private final static int SILENT = 1;
	private final static int SPEAKING = 2;
	
	/* Backchannel time threshold */
	private final static int BACKCHANNEL_TIME_THRESHOLD = 200;
	
	/* Turn state of speaker */
	private int userSpeakingState = 0; // 	0 = unknown, 1 = silence, 2 = speaking
	private long userSpeakingStateTime = 0;
	
	/* Turn state of Agent */
	private int agentSpeakingState = 1; //	1 = silence, 2 = speaking
	private long agentSpeakingStateTime = 0;
	
	/* Senders and Receivers */
	private UserStateReceiver userStateReceiver;
	private AgentStateReceiver agentStateReceiver;
	private DialogStateReceiver dialogStateReceiver;
	private FMLSender fmlSender;
	private StateSender agentStateSender;
	
	/* The current character */
	private int character;
	
	/* Backchannel send this silence period */
	private boolean backchannel_send = false;
	
	/* Random class */
	private Random rand = new Random();
	
	public BackchannelActionProposer() throws JMSException
	{
		super( "BackchannelActionProposer" );
		
		waitingTime = 20;
		
		agentStateReceiver = new AgentStateReceiver( "semaine.data.state.agent" );
		receivers.add( agentStateReceiver );
		dialogStateReceiver = new DialogStateReceiver( "semaine.data.state.dialog" );
		receivers.add( dialogStateReceiver );
		userStateReceiver = new UserStateReceiver( "semaine.data.state.user" );
		receivers.add( userStateReceiver );
		
		fmlSender = new FMLSender("semaine.data.action.candidate.function", getName());
		senders.add(fmlSender);
		agentStateSender = new StateSender( "semaine.data.state.agent", "AgentState", getName() );
		senders.add( agentStateSender );
	}
	
	public void act() throws JMSException
	{
		// if( agent=silent && user=silent && time>200ms && no bc given this silence) {
		//   send backchannel
		if( agentSpeakingState == SILENT && userSpeakingState == SILENT 
				&& userSpeakingStateTime > BACKCHANNEL_TIME_THRESHOLD && !backchannel_send ) {
			if( rand.nextInt(2) == 0 ) {
				sendBackchannel();
			}
			/* Set backchannel_send on true */
			backchannel_send = true;
		}
	}
	
	public void react( SEMAINEMessage m ) throws JMSException
	{
		if( m instanceof SEMAINEUserStateMessage ) {
			SEMAINEUserStateMessage um = ((SEMAINEUserStateMessage)m);
			setUserSpeakingState( um );
		}
		
		if( m instanceof SEMAINEDialogStateMessage ) {
			SEMAINEDialogStateMessage dm = ((SEMAINEDialogStateMessage)m);
			setAgentSpeakingState(dm);
		}
		
		if( m instanceof SEMAINEAgentStateMessage ) {
			SEMAINEAgentStateMessage am = ((SEMAINEAgentStateMessage)m);
			setCharacter(am);
		}
	}
	
	/**
	 * Reads the received Message and tries to filter out the detected user speaking state.
	 * @param m - the received message
	 */
	public void setAgentSpeakingState( SEMAINEDialogStateMessage m )
	{
		DialogStateInfo userInfo = m.getState();
		Map<String,String> userInfoMap = userInfo.getInfo();

		if( userInfoMap.get("speaker").equals("agent") ) {
			agentSpeakingState = SPEAKING;
			agentSpeakingStateTime = meta.getTime();
		} else if( userInfoMap.get("listener").equals("agent") ) {
			agentSpeakingState = SILENT;
			agentSpeakingStateTime = meta.getTime();
			if( userSpeakingState == SILENT ) {
				userSpeakingStateTime = meta.getTime();
			}
		}
	}

	/**
	 * Reads the received Message and tries to filter out the detected user speaking state.
	 * @param m - the received message
	 */
	public void setUserSpeakingState( SEMAINEUserStateMessage m )
	{
		UserStateInfo userInfo = m.getState();
		Map<String,String> userInfoMap = userInfo.getInfo();

		if( userInfoMap.get("behaviour").equals("speaking") ) {
			if( userSpeakingState != SPEAKING ) {
				userSpeakingState = SPEAKING;
				userSpeakingStateTime = meta.getTime();
				backchannel_send = false;
			}
		} else if( userInfoMap.get("behaviour").equals("silence") ) {
			if( userSpeakingState != SILENT ) {
				userSpeakingState = SILENT;
				userSpeakingStateTime = meta.getTime();
			}
		}
	}
	
	/**
	 * Reads the received Message and tries to filter out a change of character
	 * @param am - the received message
	 */
	public void setCharacter( SEMAINEAgentStateMessage am )
	{
		AgentStateInfo agentInfo = am.getState();
		Map<String,String> agentInfoMap = agentInfo.getInfo();
		
		String newChar = agentInfoMap.get( "character" );
		if( newChar != null ) {
			if( newChar.equals("poppy") ) {
				character = UtteranceActionProposer.POPPY;
			} else if( newChar.equals("prudence") ) {
				character = UtteranceActionProposer.PRUDENCE;
			} else if( newChar.equals("spike") ) {
				character = UtteranceActionProposer.SPIKE;
			} else if( newChar.equals("obadiah") ) {
				character = UtteranceActionProposer.OBADIAH;
			}
		}
	}
	
	/**
	 * 
	 * @throws JMSException
	 */
	public void sendBackchannel() throws JMSException
	{
		/* Send backchannel to output modules */
		System.out.println("backchannel");
		Document doc = XMLTool.newDocument("fml-apml", null, FML.version);
		Element root = doc.getDocumentElement();
		Element fml = XMLTool.appendChildElement(root, FML.E_FML, FML.namespaceURI);
		Element backchannel = XMLTool.appendChildElement(fml, FML.E_BACKCHANNEL, FML.namespaceURI);
		
		fmlSender.sendXML(doc, meta.getTime());
		
		/* Send backchannel behaviour around */
		Map<String,String> agentStateInfo = new HashMap<String,String>();
		agentStateInfo.put("intention", "backchannel");
		
		AgentStateInfo asi = new AgentStateInfo( agentStateInfo );
		agentStateSender.sendStateInfo(asi, meta.getTime());
	}
}
