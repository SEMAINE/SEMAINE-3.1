package eu.semaine.components.dialogue.interpreters;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.jms.JMSException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.components.dialogue.actionproposers.UtteranceActionProposer;
import eu.semaine.datatypes.stateinfo.AgentStateInfo;
import eu.semaine.datatypes.stateinfo.DialogStateInfo;
import eu.semaine.datatypes.stateinfo.StateInfo;
import eu.semaine.datatypes.xml.EmotionML;
import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.message.SEMAINEStateMessage;
import eu.semaine.jms.message.SEMAINEXMLMessage;
import eu.semaine.jms.receiver.StateReceiver;
import eu.semaine.jms.receiver.XMLReceiver;
import eu.semaine.jms.sender.Sender;
import eu.semaine.jms.sender.StateSender;
import eu.semaine.jms.sender.XMLSender;
import eu.semaine.system.CharacterConfigInfo;
import eu.semaine.util.XMLTool;

/**
 * disagreement -> Spike: high, rest: low
 * agreement -> Spike: low, rest (nu) average
 * acceptance -> spike low, rest default
 * refusal -> spike high, rest low
 * belief -> spike low, rest high
 * disbelief -> spike high, rest low
 * liking -> spike low, obadiah middle, rest high
 * disliking -> spike high, obadiah middle, rest low
 * interest -> spike and obadiah low, poppy and prudence high
 * no_interest -> spike and obadiah high, poppy and prudence low
 * understanding -> all high
 * no_understanding -> all low
 * 
 * @author MaatM
 *
 */

public class AgentMentalStateInterpreter extends Component
{
	private enum MentalState {agreement, acceptance, belief, liking, understanding, interest,
		anger, sadness, amusement, happiness, contempt, anticipation, solidarity, antagonism};
	private static Map<MentalState, String> state2VocabularyURI = setupVocabularyURIs();
	
	private static Map<MentalState, String> setupVocabularyURIs() {
		Map<MentalState, String> m = new HashMap<MentalState, String>(MentalState.values().length);
		// Communicative functions:
		for (MentalState ms : new MentalState[] {MentalState.agreement, MentalState.acceptance, MentalState.belief,
				MentalState.liking, MentalState.understanding, MentalState.interest}) {
			m.put(ms, EmotionML.VOC_SEMAINE_COMMFUNC_DIMENSION_DEFINITION);
		}
		// Listener meanings:
		for (MentalState ms : new MentalState[] {MentalState.anger, MentalState.sadness,
				MentalState.amusement, MentalState.happiness, MentalState.contempt,
				MentalState.anticipation, MentalState.solidarity, MentalState.antagonism}) {
			m.put(ms, EmotionML.VOC_SEMAINE_LISTMEAN_DIMENSION_DEFINITION);
		}
		return m;
	}
		
	private HashMap<String,HashMap<MentalState,Float>> baseMentalStates = new HashMap<String,HashMap<MentalState,Float>>();
	private HashMap<MentalState,Float> mentalState = new HashMap<MentalState,Float>();
	
	//private HashMap<String,Integer> charNumbers = new HashMap<String,Integer>();
	private String currChar;
	
	private StateSender mentalStateSender;
	private StateReceiver contextReceiver;
	private StateReceiver userStateReceiver;
	
	public AgentMentalStateInterpreter() throws JMSException
	{
		super( "AgentMentalStateInterpreter" );
		
		contextReceiver = new StateReceiver("semaine.data.state.context", StateInfo.Type.ContextState);
		receivers.add( contextReceiver );
		userStateReceiver = new StateReceiver("semaine.data.state.user.behaviour", StateInfo.Type.UserState);
		receivers.add(userStateReceiver);
		
		mentalStateSender = new StateSender("semaine.data.state.agent", StateInfo.Type.AgentState, getName());
		senders.add(mentalStateSender);
		
		initializeBaseMentalStates();
		
//		charNumbers.put("poppy", UtteranceActionProposer.POPPY);
//		charNumbers.put("prudence", UtteranceActionProposer.PRUDENCE);
//		charNumbers.put("spike", UtteranceActionProposer.SPIKE);
//		charNumbers.put("obadiah", UtteranceActionProposer.OBADIAH);
	}
	
	public void initializeBaseMentalStates()
	{
		String[] characters = {"Poppy","Prudence","Obadiah","Spike"};
		for( String ch : characters ) {
			CharacterConfigInfo info = CharacterConfigInfo.getInfo(ch);
			// Now the basic idea is to iterate over our own list of mental states,
			// and see if the info has something about it:
			for (MentalState mental : MentalState.values()) {
			    String vocabularyURI = state2VocabularyURI.get(mental);
			    assert vocabularyURI != null;
			    float predisposition = info.getEmotionalPredisposition(vocabularyURI, mental.toString());
			    if (predisposition != -1) { // yes, present
			        HashMap<MentalState, Float> values = baseMentalStates.get(ch);
			        if( values == null ) values = new HashMap<MentalState,Float>();
			    	values.put(mental, predisposition);
			    	baseMentalStates.put(ch,values);
			    }
			}
		}
	}
	
	public void react( SEMAINEMessage m ) throws JMSException
	{
		/* Processes User state updates */
		if( m instanceof SEMAINEStateMessage ) {
			SEMAINEStateMessage sm = ((SEMAINEStateMessage)m);
			StateInfo stateInfo = sm.getState();
			StateInfo.Type stateInfoType = stateInfo.getType();
			
			boolean stateChanged = false;
			
			switch (stateInfoType) {
			case ContextState:
				/* Updates the current character and the user */
				stateChanged = ( updateCharacter( stateInfo ) || stateChanged);
				break;
			case UserState:
				stateChanged = ( updateEmotions( stateInfo ) || stateChanged);
				break;
			}
			if( stateChanged ) {
				sendMentalState();
			}
		}
	}
	
	public boolean resetMentalState()
	{
		if( currChar != null ) {
			try {
				mentalState = (HashMap<MentalState,Float>) baseMentalStates.get(currChar).clone();
			} catch( ClassCastException e ) {
				System.out.println( "ClassCastException while resetting Mental State" );
				return false;
			} catch( Exception e ) {
				System.out.println("Unknown character found, using empty hashmap");
				mentalState = new HashMap<MentalState,Float>();
				return false;
			}
			return true;
		}
		return false;
	}
	
	public boolean updateCharacter( StateInfo stateInfo ) throws JMSException
	{
		if( stateInfo.hasInfo("character") ) {
			currChar = stateInfo.getInfo("character");
			if(resetMentalState()) {
				return true;
			} else {
				return false;
			}
		} else {
			return false;
		}
	}
	
	public boolean updateEmotions( StateInfo stateInfo ) throws JMSException
	{
		if( currChar == null ) {
			return false;
		}
		
		boolean changed = false;
		
		if( stateInfo.hasInfo("arousal") ) {
			float arousal = Float.parseFloat( stateInfo.getInfo("arousal") );
			if( arousal > 0.5f ) {
				// High Arousal
				if( currChar.equals("Poppy") || currChar.equals("Spike") ) {
					if( mentalState.get(MentalState.agreement) != null ) mentalState.put(MentalState.agreement, mentalState.get(MentalState.agreement)+0.05f);
					if( mentalState.get(MentalState.liking) != null ) mentalState.put(MentalState.liking, mentalState.get(MentalState.liking)+0.05f);
					if( mentalState.get(MentalState.understanding) != null ) mentalState.put(MentalState.understanding, mentalState.get(MentalState.understanding)+0.05f);
				} else {
					if( mentalState.get(MentalState.agreement) != null ) mentalState.put(MentalState.agreement, mentalState.get(MentalState.agreement)-0.05f);
					if( mentalState.get(MentalState.liking) != null )mentalState.put(MentalState.liking, mentalState.get(MentalState.liking)-0.05f);
					if( mentalState.get(MentalState.understanding) != null ) mentalState.put(MentalState.understanding, mentalState.get(MentalState.understanding)-0.05f);
				}
				changed = true;
			}
			if( arousal < -0.5f ) {
				// High Arousal
				if( currChar.equals("Poppy") || currChar.equals("Spike") ) {
					if( mentalState.get(MentalState.agreement) != null ) mentalState.put(MentalState.agreement, mentalState.get(MentalState.agreement)-0.05f);
					if( mentalState.get(MentalState.liking) != null )mentalState.put(MentalState.liking, mentalState.get(MentalState.liking)-0.05f);
					if( mentalState.get(MentalState.understanding) != null ) mentalState.put(MentalState.understanding, mentalState.get(MentalState.understanding)-0.05f);
				} else {
					if( mentalState.get(MentalState.agreement) != null ) mentalState.put(MentalState.agreement, mentalState.get(MentalState.agreement)+0.05f);
					if( mentalState.get(MentalState.liking) != null )mentalState.put(MentalState.liking, mentalState.get(MentalState.liking)+0.05f);
					if( mentalState.get(MentalState.understanding) != null ) mentalState.put(MentalState.understanding, mentalState.get(MentalState.understanding)+0.05f);
				}
				changed = true;
			}
		}
		if( stateInfo.hasInfo("valence") ) {
			float valence = Float.parseFloat( stateInfo.getInfo("valence") );
			if( valence > 0.5f ) {
				// High Arousal
				if( currChar.equals("Poppy") || currChar.equals("Prudence") ) {
					if( mentalState.get(MentalState.agreement) != null ) mentalState.put(MentalState.agreement, mentalState.get(MentalState.agreement)+0.05f);
					if( mentalState.get(MentalState.liking) != null )mentalState.put(MentalState.liking, mentalState.get(MentalState.liking)+0.05f);
					if( mentalState.get(MentalState.understanding) != null ) mentalState.put(MentalState.understanding, mentalState.get(MentalState.understanding)+0.05f);
				} else {
					if( mentalState.get(MentalState.agreement) != null ) mentalState.put(MentalState.agreement, mentalState.get(MentalState.agreement)-0.05f);
					if( mentalState.get(MentalState.liking) != null )mentalState.put(MentalState.liking, mentalState.get(MentalState.liking)-0.05f);
					if( mentalState.get(MentalState.understanding) != null ) mentalState.put(MentalState.understanding, mentalState.get(MentalState.understanding)-0.05f);
				}
				changed = true;
			}
			if( valence < -0.5f ) {
				// High Arousal
				if( currChar.equals("Poppy") || currChar.equals("Prudence") ) {
					if( mentalState.get(MentalState.agreement) != null ) mentalState.put(MentalState.agreement, mentalState.get(MentalState.agreement)-0.05f);
					if( mentalState.get(MentalState.liking) != null )mentalState.put(MentalState.liking, mentalState.get(MentalState.liking)-0.05f);
					if( mentalState.get(MentalState.understanding) != null ) mentalState.put(MentalState.understanding, mentalState.get(MentalState.understanding)-0.05f);
				} else {
					if( mentalState.get(MentalState.agreement) != null ) mentalState.put(MentalState.agreement, mentalState.get(MentalState.agreement)+0.05f);
					if( mentalState.get(MentalState.liking) != null )mentalState.put(MentalState.liking, mentalState.get(MentalState.liking)+0.05f);
					if( mentalState.get(MentalState.understanding) != null ) mentalState.put(MentalState.understanding, mentalState.get(MentalState.understanding)+0.05f);
				}
				changed = true;
			}
		}
		if( changed ) {
			return true;
		} else {
			return false;
		}
	}
	
	public void sendMentalState() throws JMSException
	{
		Map<String,String> mentalStateInfo = new HashMap<String,String>();
		
		for( MentalState key : mentalState.keySet() ) {
			mentalStateInfo.put(key.toString(), ""+mentalState.get(key));
		}
		
		AgentStateInfo asi = new AgentStateInfo(mentalStateInfo);	
		mentalStateSender.sendStateInfo(asi, meta.getTime());
	}
}
