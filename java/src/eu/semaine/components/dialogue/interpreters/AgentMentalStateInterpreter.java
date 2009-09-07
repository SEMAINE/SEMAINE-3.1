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
	private static int AGREEMENT = 0;
	private static int DISAGREEMENT = 1;
	private static int ACCEPTANCE = 2;
	private static int REFUSAL = 3;
	private static int BELIEF = 4;
	private static int DISBELIEF = 5;
	private static int LIKING = 6;
	private static int DISLIKING = 7;
	private static int UNDERSTANDING = 8;
	private static int NOUNDERSTANDING = 9;
	private static int INTEREST = 10;
	private static int NOINTEREST = 11;
	
	private static String PoppyState = "<?xml version=\"1.0\"?><profilexmlns=\"http://www.w3schools.com\"xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"xsi:schemaLocation=\"profiles.xsd\">	<parameter-set name=\"listener_fuction\">		<Parameter name=\"disagreement.value\" value=\"0.0\"/>		<Parameter name=\"disagreement.min\" value=\"0.0\"/><Parameter name=\"disagreement.max\" value=\"1.0\"/>		<Parameter name=\"agreement.value\" value=\"0.3\"/>		<Parameter name=\"agreement.min\" value=\"0.0\"/><Parameter name=\"agreement.max\" value=\"1.0\"/>		<Parameter name=\"acceptance.value\" value=\"0.3\"/>		<Parameter name=\"acceptance.min\" value=\"0.0\"/><Parameter name=\"acceptance.max\" value=\"1.0\"/>		<Parameter name=\"refusal.value\" value=\"0.0\"/>		<Parameter name=\"refusal.min\" value=\"0.0\"/><Parameter name=\"refusal.max\" value=\"1.0\"/>		<Parameter name=\"belief.value\" value=\"0.5\"/>		<Parameter name=\"belief.min\" value=\"0.0\"/><Parameter name=\"belief.max\" value=\"1.0\"/>		<Parameter name=\"disbelief.value\" value=\"0.0\"/>		<Parameter name=\"disbelief.min\" value=\"0.0\"/><Parameter name=\"disbelief.max\" value=\"1.0\"/>		<Parameter name=\"liking.value\" value=\"0.8\"/>		<Parameter name=\"liking.min\" value=\"0.0\"/><Parameter name=\"liking.max\" value=\"1.0\"/>		<Parameter name=\"disliking.value\" value=\"0.0\"/>		<Parameter name=\"disliking.min\" value=\"0.0\"/><Parameter name=\"disliking.max\" value=\"1.0\"/>		<Parameter name=\"interest.value\" value=\"0.8\"/>		<Parameter name=\"interest.min\" value=\"0.0\"/><Parameter name=\"interest.max\" value=\"1.0\"/>		<Parameter name=\"no_interest.value\" value=\"0.0\"/>		<Parameter name=\"no_interest.min\" value=\"0.0\"/><Parameter name=\"no_interest.max\" value=\"1.0\"/>		<Parameter name=\"understanding.value\" value=\"0.8\"/>		<Parameter name=\"understanding.min\" value=\"0.0\"/><Parameter name=\"understanding.max\" value=\"1.0\"/>		<Parameter name=\"no_understanding.value\" value=\"0.0\"/>		<Parameter name=\"no_understanding.min\" value=\"0.0\"/><Parameter name=\"no_understanding.max\" value=\"1.0\"/>	</parameter-set></profile>";
	private static String PrudenceState = "<?xml version=\"1.0\"?><profilexmlns=\"http://www.w3schools.com\"xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"xsi:schemaLocation=\"profiles.xsd\">	<parameter-set name=\"listener_fuction\">		<Parameter name=\"disagreement.value\" value=\"0.0\"/>		<Parameter name=\"disagreement.min\" value=\"0.0\"/><Parameter name=\"disagreement.max\" value=\"1.0\"/>		<Parameter name=\"agreement.value\" value=\"0.3\"/>		<Parameter name=\"agreement.min\" value=\"0.0\"/><Parameter name=\"agreement.max\" value=\"1.0\"/>		<Parameter name=\"acceptance.value\" value=\"0.3\"/>		<Parameter name=\"acceptance.min\" value=\"0.0\"/><Parameter name=\"acceptance.max\" value=\"1.0\"/>		<Parameter name=\"refusal.value\" value=\"0.0\"/>		<Parameter name=\"refusal.min\" value=\"0.0\"/><Parameter name=\"refusal.max\" value=\"1.0\"/>		<Parameter name=\"belief.value\" value=\"0.5\"/>		<Parameter name=\"belief.min\" value=\"0.0\"/><Parameter name=\"belief.max\" value=\"1.0\"/>		<Parameter name=\"disbelief.value\" value=\"0.0\"/>		<Parameter name=\"disbelief.min\" value=\"0.0\"/><Parameter name=\"disbelief.max\" value=\"1.0\"/>		<Parameter name=\"liking.value\" value=\"0.8\"/>		<Parameter name=\"liking.min\" value=\"0.0\"/><Parameter name=\"liking.max\" value=\"1.0\"/>		<Parameter name=\"disliking.value\" value=\"0.0\"/>		<Parameter name=\"disliking.min\" value=\"0.0\"/><Parameter name=\"disliking.max\" value=\"1.0\"/>		<Parameter name=\"interest.value\" value=\"0.8\"/>		<Parameter name=\"interest.min\" value=\"0.0\"/><Parameter name=\"interest.max\" value=\"1.0\"/>		<Parameter name=\"no_interest.value\" value=\"0.0\"/>		<Parameter name=\"no_interest.min\" value=\"0.0\"/><Parameter name=\"no_interest.max\" value=\"1.0\"/>		<Parameter name=\"understanding.value\" value=\"0.8\"/>		<Parameter name=\"understanding.min\" value=\"0.0\"/><Parameter name=\"understanding.max\" value=\"1.0\"/>		<Parameter name=\"no_understanding.value\" value=\"0.0\"/>		<Parameter name=\"no_understanding.min\" value=\"0.0\"/><Parameter name=\"no_understanding.max\" value=\"1.0\"/>	</parameter-set></profile>";
	private static String ObadiahState = "<?xml version=\"1.0\"?><profilexmlns=\"http://www.w3schools.com\"xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"xsi:schemaLocation=\"profiles.xsd\">	<parameter-set name=\"listener_fuction\">		<Parameter name=\"disagreement.value\" value=\"0.0\"/>		<Parameter name=\"disagreement.min\" value=\"0.0\"/><Parameter name=\"disagreement.max\" value=\"1.0\"/>		<Parameter name=\"agreement.value\" value=\"0.3\"/>		<Parameter name=\"agreement.min\" value=\"0.0\"/><Parameter name=\"agreement.max\" value=\"1.0\"/>		<Parameter name=\"acceptance.value\" value=\"0.3\"/>		<Parameter name=\"acceptance.min\" value=\"0.0\"/><Parameter name=\"acceptance.max\" value=\"1.0\"/>		<Parameter name=\"refusal.value\" value=\"0.0\"/>		<Parameter name=\"refusal.min\" value=\"0.0\"/><Parameter name=\"refusal.max\" value=\"1.0\"/>		<Parameter name=\"belief.value\" value=\"0.5\"/>		<Parameter name=\"belief.min\" value=\"0.0\"/><Parameter name=\"belief.max\" value=\"1.0\"/>		<Parameter name=\"disbelief.value\" value=\"0.0\"/>		<Parameter name=\"disbelief.min\" value=\"0.0\"/><Parameter name=\"disbelief.max\" value=\"1.0\"/>		<Parameter name=\"liking.value\" value=\"0.4\"/>		<Parameter name=\"liking.min\" value=\"0.0\"/><Parameter name=\"liking.max\" value=\"1.0\"/>		<Parameter name=\"disliking.value\" value=\"0.4\"/>		<Parameter name=\"disliking.min\" value=\"0.0\"/><Parameter name=\"disliking.max\" value=\"1.0\"/>		<Parameter name=\"interest.value\" value=\"0.2\"/>		<Parameter name=\"interest.min\" value=\"0.0\"/><Parameter name=\"interest.max\" value=\"1.0\"/>		<Parameter name=\"no_interest.value\" value=\"0.7\"/>		<Parameter name=\"no_interest.min\" value=\"0.0\"/><Parameter name=\"no_interest.max\" value=\"1.0\"/>		<Parameter name=\"understanding.value\" value=\"0.8\"/>		<Parameter name=\"understanding.min\" value=\"0.0\"/><Parameter name=\"understanding.max\" value=\"1.0\"/>		<Parameter name=\"no_understanding.value\" value=\"0.0\"/>		<Parameter name=\"no_understanding.min\" value=\"0.0\"/><Parameter name=\"no_understanding.max\" value=\"1.0\"/>	</parameter-set></profile>";
	private static String SpikeState = "<?xml version=\"1.0\"?><profilexmlns=\"http://www.w3schools.com\"xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"xsi:schemaLocation=\"profiles.xsd\">	<parameter-set name=\"listener_fuction\">		<Parameter name=\"disagreement.value\" value=\"0.8\"/>		<Parameter name=\"disagreement.min\" value=\"0.0\"/><Parameter name=\"disagreement.max\" value=\"1.0\"/>		<Parameter name=\"agreement.value\" value=\"0.0\"/>		<Parameter name=\"agreement.min\" value=\"0.0\"/><Parameter name=\"agreement.max\" value=\"1.0\"/>		<Parameter name=\"acceptance.value\" value=\"0.0\"/>		<Parameter name=\"acceptance.min\" value=\"0.0\"/><Parameter name=\"acceptance.max\" value=\"1.0\"/>		<Parameter name=\"refusal.value\" value=\"0.8\"/>		<Parameter name=\"refusal.min\" value=\"0.0\"/><Parameter name=\"refusal.max\" value=\"1.0\"/>		<Parameter name=\"belief.value\" value=\"0.0\"/>		<Parameter name=\"belief.min\" value=\"0.0\"/><Parameter name=\"belief.max\" value=\"1.0\"/>		<Parameter name=\"disbelief.value\" value=\"0.8\"/>		<Parameter name=\"disbelief.min\" value=\"0.0\"/><Parameter name=\"disbelief.max\" value=\"1.0\"/>		<Parameter name=\"liking.value\" value=\"0.0\"/>		<Parameter name=\"liking.min\" value=\"0.0\"/><Parameter name=\"liking.max\" value=\"1.0\"/>		<Parameter name=\"disliking.value\" value=\"0.8\"/>		<Parameter name=\"disliking.min\" value=\"0.0\"/><Parameter name=\"disliking.max\" value=\"1.0\"/>		<Parameter name=\"interest.value\" value=\"0.0\"/>		<Parameter name=\"interest.min\" value=\"0.0\"/><Parameter name=\"interest.max\" value=\"1.0\"/>		<Parameter name=\"no_interest.value\" value=\"0.8\"/>		<Parameter name=\"no_interest.min\" value=\"0.0\"/><Parameter name=\"no_interest.max\" value=\"1.0\"/>		<Parameter name=\"understanding.value\" value=\"0.8\"/>		<Parameter name=\"understanding.min\" value=\"0.0\"/><Parameter name=\"understanding.max\" value=\"1.0\"/>		<Parameter name=\"no_understanding.value\" value=\"0.0\"/>		<Parameter name=\"no_understanding.min\" value=\"0.0\"/><Parameter name=\"no_understanding.max\" value=\"1.0\"/>	</parameter-set></profile>";
	
	
	private double[] poppyState = new double[12];
	private double[] prudenceState = new double[12];
	private double[] obadiahState = new double[12];
	private double[] spikeState = new double[12];
	private double[] poppyStateDefault = {0.3, 0.0, 0.3, 0.0, 0.5, 0.0, 0.8, 0.0, 0.8, 0.8, 0.8, 0.0};
	private double[] prudenceStateDefault = {0.3, 0.0, 0.3, 0.0, 0.5, 0.0, 0.8, 0.0, 0.8, 0.0, 0.8, 0.0};
	private double[] obadiahStateDefault = {0.3, 0.0, 0.3, 0.0, 0.5, 0.0, 0.4, 0.4, 0.8, 0.0, 0.2, 0.7};
	private double[] spikeStateDefault = {0.0, 0.8, 0.0, 0.8, 0.0, 0.8, 0.0, 0.8, 0.8, 0.0, 0.0, 0.8};
	
	private HashMap<String,Integer> charNumbers = new HashMap<String,Integer>();
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
		
		charNumbers.put("poppy", UtteranceActionProposer.POPPY);
		charNumbers.put("prudence", UtteranceActionProposer.PRUDENCE);
		charNumbers.put("spike", UtteranceActionProposer.SPIKE);
		charNumbers.put("obadiah", UtteranceActionProposer.OBADIAH);
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
	
	public double[] getMentalState()
	{
		if( currChar != null ) {
			if( currChar.equals("Poppy") ) {
				return poppyState;
			} else if( currChar.equals("Prudence") ) {
				return prudenceState;
			} else if( currChar.equals("Obadiah") ) {
				return obadiahState;
			} else if( currChar.equals("Spike") ) {
				return spikeState;
			} else {
				System.out.println("ERROR: Unknown character");
				return null;
			}
		}
		return null;
	}
	
	public void setMentalState( double[] state )
	{
		if( currChar != null ) {
			if( currChar.equals("Poppy") ) {
				poppyState = state;
			} else if( currChar.equals("Prudence") ) {
				prudenceState = state;
			} else if( currChar.equals("Obadiah") ) {
				obadiahState = state;
			} else if( currChar.equals("Spike") ) {
				spikeState = state;
			} else {
				System.out.println("ERROR: Unknown character");
			}
		}
	}
	
	public void resetMentalState()
	{
		if( currChar != null ) {
			if( currChar.equals("Poppy") ) {
				poppyState = poppyStateDefault;
			} else if( currChar.equals("Prudence") ) {
				prudenceState = prudenceStateDefault;
			} else if( currChar.equals("Obadiah") ) {
				obadiahState = obadiahStateDefault;
			} else if( currChar.equals("Spike") ) {
				spikeState = spikeStateDefault;
			} else {
				System.out.println("ERROR: Unknown character");
			}
		}
	}
	
	public boolean updateCharacter( StateInfo stateInfo ) throws JMSException
	{
		if( stateInfo.hasInfo("character") ) {
			currChar = stateInfo.getInfo("character");
			resetMentalState();
			return true;
		} else {
			return false;
		}
	}
	public boolean updateEmotions( StateInfo stateInfo ) throws JMSException
	{
		if( currChar == null ) {
			return false;
		}
		
		double[] state = getMentalState();
		boolean changed = false;
		
		if( stateInfo.hasInfo("arousal") ) {
			float arousal = Float.parseFloat( stateInfo.getInfo("arousal") );
			if( arousal > 0.5f ) {
				// High Arousal
				if( currChar.equals("Poppy") || currChar.equals("Spike") ) {
					state[AGREEMENT] = state[AGREEMENT] + 0.1;
					state[LIKING] = state[LIKING] + 0.1;
					state[UNDERSTANDING] = state[UNDERSTANDING] + 0.1;
					
					state[DISAGREEMENT] = state[DISAGREEMENT] - 0.1;
					state[DISLIKING] = state[DISLIKING] - 0.1;
					state[NOUNDERSTANDING] = state[NOUNDERSTANDING] - 0.1;
				} else {
					state[AGREEMENT] = state[AGREEMENT] - 0.1;
					state[LIKING] = state[LIKING] - 0.1;
					state[UNDERSTANDING] = state[UNDERSTANDING] - 0.1;
					
					state[DISAGREEMENT] = state[DISAGREEMENT] + 0.1;
					state[DISLIKING] = state[DISLIKING] + 0.1;
					state[NOUNDERSTANDING] = state[NOUNDERSTANDING] + 0.1;
				}
				changed = true;
			}
			if( arousal < -0.5f ) {
				// High Arousal
				if( currChar.equals("Poppy") || currChar.equals("Spike") ) {
					state[AGREEMENT] = state[AGREEMENT] - 0.1;
					state[LIKING] = state[LIKING] - 0.1;
					state[UNDERSTANDING] = state[UNDERSTANDING] - 0.1;
					
					state[DISAGREEMENT] = state[DISAGREEMENT] + 0.1;
					state[DISLIKING] = state[DISLIKING] + 0.1;
					state[NOUNDERSTANDING] = state[NOUNDERSTANDING] + 0.1;
				} else {
					state[AGREEMENT] = state[AGREEMENT] + 0.1;
					state[LIKING] = state[LIKING] + 0.1;
					state[UNDERSTANDING] = state[UNDERSTANDING] + 0.1;
					
					state[DISAGREEMENT] = state[DISAGREEMENT] - 0.1;
					state[DISLIKING] = state[DISLIKING] - 0.1;
					state[NOUNDERSTANDING] = state[NOUNDERSTANDING] - 0.1;
				}
				changed = true;
			}
		}
		if( stateInfo.hasInfo("valence") ) {
			float valence = Float.parseFloat( stateInfo.getInfo("valence") );
			if( valence > 0.5f ) {
				// High Arousal
				if( currChar.equals("Poppy") || currChar.equals("Prudence") ) {
					state[AGREEMENT] = state[AGREEMENT] + 0.1;
					state[LIKING] = state[LIKING] + 0.1;
					state[UNDERSTANDING] = state[UNDERSTANDING] + 0.1;
					
					state[DISAGREEMENT] = state[DISAGREEMENT] - 0.1;
					state[DISLIKING] = state[DISLIKING] - 0.1;
					state[NOUNDERSTANDING] = state[NOUNDERSTANDING] - 0.1;
				} else {
					state[AGREEMENT] = state[AGREEMENT] - 0.1;
					state[LIKING] = state[LIKING] - 0.1;
					state[UNDERSTANDING] = state[UNDERSTANDING] - 0.1;
					
					state[DISAGREEMENT] = state[DISAGREEMENT] + 0.1;
					state[DISLIKING] = state[DISLIKING] + 0.1;
					state[NOUNDERSTANDING] = state[NOUNDERSTANDING] + 0.1;
				}
				changed = true;
			}
			if( valence < -0.5f ) {
				// High Arousal
				if( currChar.equals("Poppy") || currChar.equals("Prudence") ) {
					state[AGREEMENT] = state[AGREEMENT] - 0.1;
					state[LIKING] = state[LIKING] - 0.1;
					state[UNDERSTANDING] = state[UNDERSTANDING] - 0.1;
					
					state[DISAGREEMENT] = state[DISAGREEMENT] + 0.1;
					state[DISLIKING] = state[DISLIKING] + 0.1;
					state[NOUNDERSTANDING] = state[NOUNDERSTANDING] + 0.1;
				} else {
					state[AGREEMENT] = state[AGREEMENT] + 0.1;
					state[LIKING] = state[LIKING] + 0.1;
					state[UNDERSTANDING] = state[UNDERSTANDING] + 0.1;
					
					state[DISAGREEMENT] = state[DISAGREEMENT] - 0.1;
					state[DISLIKING] = state[DISLIKING] - 0.1;
					state[NOUNDERSTANDING] = state[NOUNDERSTANDING] - 0.1;
				}
				changed = true;
			}
		}
		if( changed ) {
			setMentalState(state);
			return true;
		} else {
			return false;
		}
	}
	
	public void sendMentalState() throws JMSException
	{
		double[] state = getMentalState();
		Map<String,String> mentalStateInfo = new HashMap<String,String>();		
		
		/* StateInfo: [Agree Disagree Acceptance Refusal Belief Disbelief Liking Disliking Understanding NoUnderstanding Interest NoInterest] */
		mentalStateInfo.put("listenerAgreement", ""+state[AGREEMENT]);
		mentalStateInfo.put("listenerDisagreement", ""+state[DISAGREEMENT]);
		mentalStateInfo.put("listenerAcceptance", ""+state[ACCEPTANCE]);
		mentalStateInfo.put("listenerRefusal", ""+state[REFUSAL]);
		mentalStateInfo.put("listenerBelief", ""+state[BELIEF]);
		mentalStateInfo.put("listenerDisbelief", ""+state[DISBELIEF]);
		mentalStateInfo.put("listenerLiking", ""+state[LIKING]);
		mentalStateInfo.put("listenerDisliking", ""+state[DISLIKING]);
		mentalStateInfo.put("listenerUnderstanding", ""+state[UNDERSTANDING]);
		mentalStateInfo.put("listenerNoUnderstanding", ""+state[NOUNDERSTANDING]);
		mentalStateInfo.put("listenerInterest", ""+state[INTEREST]);
		mentalStateInfo.put("listenerNoInterest", ""+state[NOINTEREST]);

		AgentStateInfo asi = new AgentStateInfo(mentalStateInfo);	
		mentalStateSender.sendStateInfo(asi, meta.getTime());
	}
}
