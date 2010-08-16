//package eu.semaine.components.dialogue.loebner;
//
//import java.util.ArrayList;
//import java.util.HashMap;
//import java.util.Map;
//import java.util.Random;
//
//import javax.jms.JMSException;
//
//import org.w3c.dom.Document;
//import org.w3c.dom.Element;
//import org.w3c.dom.Node;
//
//import eu.semaine.components.Component;
//import eu.semaine.components.dialogue.datastructures.AgentSpokenUtterance;
//import eu.semaine.components.dialogue.datastructures.AgentUtterance;
//import eu.semaine.components.dialogue.datastructures.EmotionEvent;
//import eu.semaine.components.dialogue.test.DMLogger;
//import eu.semaine.datatypes.stateinfo.ContextStateInfo;
//import eu.semaine.datatypes.stateinfo.DialogStateInfo;
//import eu.semaine.datatypes.stateinfo.StateInfo;
//import eu.semaine.datatypes.xml.BML;
//import eu.semaine.datatypes.xml.FML;
//import eu.semaine.datatypes.xml.SSML;
//import eu.semaine.datatypes.xml.SemaineML;
//import eu.semaine.jms.message.SEMAINEMessage;
//import eu.semaine.jms.message.SEMAINEStateMessage;
//import eu.semaine.jms.message.SEMAINEXMLMessage;
//import eu.semaine.jms.receiver.StateReceiver;
//import eu.semaine.jms.receiver.XMLReceiver;
//import eu.semaine.jms.sender.FMLSender;
//import eu.semaine.jms.sender.StateSender;
//import eu.semaine.util.XMLTool;
//
//public class LoebnerUtteranceActionProposer extends Component
//{
//	/* ===================== Global Variables ===================== */
//	
//	/* Static variables */
//	// Speaking state
//	public final static int WAITING = 0;	// Waiting for the other person to start speaking
//	public final static int LISTENING = 1;	// Listening to the speech of the user
//	public final static int PREPARING_TO_SPEAK = 2;
//	public final static int SPEAKING = 3;	// The agent is speaking
//
//	// Conversational state
//	public final static int STARTUP_1 = 1;
//	public final static int STARTUP_2 = 2;
//	public final static int SITUATION_EXPLAINED = 3;
//	
//	public final static long MAX_SILENCE_TIME = 6000; // The maximum amount of user silence (after an agent utterance) before the agent starts talking again
//	
//	private static final String PRESENT = "present";
//	private static final String ABSENT = "absent";
//	
//	/* Random generator */
//	private Random rand = new Random();
//	
//	/* Data locations */
//	public final static String sentenceDataPath = "/eu/semaine/components/dialogue/data/loebnerSentences.xml";
//	
//	/* Global state information */
//	private int convState = 0;				// The current conversational state (STARTUP_1, STARTUP_2, or SITUATION_EXPLAINED)
//	private boolean isUserPresent = false;	// Indicates if the user is currently present or not
//	private boolean systemStarted = false;	// Indicates if the system has started or not
//	private long systemStartTime = 0;		// The starttime of the system.
//	private boolean firstRandomAnswerGiven = false;
//
//	/* Senders and Receivers */
//	private StateReceiver agentStateReceiver;
//	private StateReceiver userStateReceiver;
//	private StateReceiver contextReceiver;
//	private XMLReceiver callbackReceiver;
//	private FMLSender fmlSender;
//	private StateSender dialogStateSender;
//	private StateSender contextSender;
//
//	/* The current state of the agent */
//	public int agentSpeakingState = 1;			// The current speaking state of the agent (SPEAKING, LISTENING or WAITING)
//	private long agentSpeakingStateTime = 0;	// The starttime of the current speaking state of the agent
//	private long utteranceEndTime = 0; 			// The predicted endtime of the agent utterance
//
//	/* Turn state of speaker */
//	private int userSpeakingState = 1;			// The current speaking state of the user (SPEAKING, LISTENING or WAITING)
//	private long userSpeakingStateTime = 0;		// The starttime of the current speaking state of the user
//	private long userTurnStart = -1;			// The starttime of the user turn, set to -1 after an agent turn
//
//	/* Detected events */
//	private ArrayList<EmotionEvent> detectedEmotions = new ArrayList<EmotionEvent>();
//	private int emotionIndex = 0;
//	
//	private ArrayList<String> detectedWords = new ArrayList<String>();
//	private ArrayList<Long> detectedWordTimes = new ArrayList<Long>();
//	private int detectedWordsIndex = 0;
//
//	/* All utterances of the agent */
//	private ArrayList<AgentSpokenUtterance> agentUtteranceHistory = new ArrayList<AgentSpokenUtterance>();
//
//	/* All agent utterances grouped by utterance-type */
//	private HashMap<Integer,HashMap<String,ArrayList<AgentUtterance>>> allUtterances = new HashMap<Integer,HashMap<String,ArrayList<AgentUtterance>>>();
//
//	/* All utterance categories and their changes of using them */
//	private HashMap<String,Double> categoryChances = new HashMap<String,Double>();
//	private HashMap<String,Double> defaultCategoryChances = new HashMap<String,Double>();
//
//	/* ===================== Constructor ===================== */
//	
//	/**
//	 * Creates a new LoebnerUtteranceActionProposer
//	 */
//	public LoebnerUtteranceActionProposer() throws JMSException
//	{
//		super( "LoebnerUtteranceActionProposer" );
//
//		/* Initialize receivers */
//		agentStateReceiver = new StateReceiver( "semaine.data.state.agent", StateInfo.Type.AgentState );
//		receivers.add( agentStateReceiver );
//		userStateReceiver = new StateReceiver("semaine.data.state.user.behaviour", StateInfo.Type.UserState);
//		receivers.add(userStateReceiver);
//		contextReceiver = new StateReceiver("semaine.data.state.context", StateInfo.Type.ContextState);
//		receivers.add( contextReceiver );
//		callbackReceiver = new XMLReceiver("semaine.callback.output.audio");
//		receivers.add(callbackReceiver);
//
//		/* Initialize senders */
//		fmlSender = new FMLSender("semaine.data.action.candidate.function", getName());
//		senders.add(fmlSender);
//		dialogStateSender = new StateSender("semaine.data.state.dialog", StateInfo.Type.DialogState, getName());
//		senders.add(dialogStateSender);
//		contextSender = new StateSender("semaine.data.state.context", StateInfo.Type.ContextState, getName());
//		senders.add(contextSender);
//
//		/* Reads the utterances of all the characters */
////		SentenceReader sentenceReader = new SentenceReader( sentenceDataPath );
////		if( sentenceReader.readData() ) {
////			allUtterances = sentenceReader.getAllUtterances();
////		} else {
////			log.error("Error reading SAL Sentences");
////		}
//
//		resetDefaultCategoryChances();
//		resetCategoryChances();
//	}
//
//	/* ===================== Methods ===================== */
//	
//	public void react( SEMAINEMessage m ) throws JMSException
//	{
//		if( m instanceof SEMAINEXMLMessage ){
//			SEMAINEXMLMessage xm = ((SEMAINEXMLMessage)m);
//			if( speechStarted(xm) ) {
//				DMLogger.getLogger().log(meta.getTime(), "AgentAction:UtteranceStarted" );
//				agentSpeakingState = SPEAKING;
//				agentSpeakingStateTime = meta.getTime();
//			}
//			
//			if( speechReady(xm) ) {
//				DMLogger.getLogger().log(meta.getTime(), "AgentAction:UtteranceStopped" );
//				agentSpeakingState = LISTENING;
//				agentSpeakingStateTime = meta.getTime();
//				sendListening();
//				userTurnStart = -1;
//			}
//		}
//		
//		/* Processes User state updates */
//		if( m instanceof SEMAINEStateMessage ) {
//			SEMAINEStateMessage sm = ((SEMAINEStateMessage)m);
//			StateInfo stateInfo = sm.getState();
//			if (stateInfo.getType() == StateInfo.Type.UserState) {
//				/* Updates user speaking state (speaking or silent) */
//				setUserSpeakingState(stateInfo);
//
//				/* Updates detected emotions (valence, arousal, interest) */
//				addDetectedEmotions(stateInfo);
//
//				/* Updated analyzed Dialogue Acts history */
//				addDetectedWords( stateInfo);
//			}
//
//			if( stateInfo.getType() == StateInfo.Type.ContextState ) {
//				updateCharacterAndUser( stateInfo );
//			}
//		}
//
//		/* If the TurnTakingInterpreter decides that the agent should speak, determine what to say */
//		if( agentShouldSpeak( m ) ) {
//
//			/* Update agent speaking state */
//			agentSpeakingState = PREPARING_TO_SPEAK;
//
//			AgentSpokenUtterance spokenUtterance = null;
//
//			/* Check if the character start process should be started or continued */
//			spokenUtterance = manageAgentStart();
//			if( spokenUtterance == null ) {
//				/* Get an utterance to say */
//				spokenUtterance = getResponse();
//			}
//
//
//			/* Distribute the chosen utterance */
//			sendUtterance( spokenUtterance );
//		}
//	}
//
//	public void act() throws JMSException
//	{
//		if( !systemStarted ) {
//			return;
//		}
//		processUtteranceEnd();
//		//checkUserSilenceTime();
//	}
//	
//	public boolean speechReady( SEMAINEXMLMessage xm )
//	{
//		Element callbackElem = XMLTool.getChildElementByLocalNameNS(xm.getDocument(), "callback", SemaineML.namespaceURI);
//		if( callbackElem != null ) {
//			Element eventElem = XMLTool.getChildElementByLocalNameNS(callbackElem, "event", SemaineML.namespaceURI);
//			if( eventElem != null ) {
//				if( eventElem.hasAttribute("type") && eventElem.getAttribute("type").equals("end") && agentSpeakingState == SPEAKING ) {
//					return true;
//				}
//			}
//		}
//		return false;
//	}
//	
//	public boolean speechStarted( SEMAINEXMLMessage xm )
//	{
//		Element callbackElem = XMLTool.getChildElementByLocalNameNS(xm.getDocument(), "callback", SemaineML.namespaceURI);
//		if( callbackElem != null ) {
//			Element eventElem = XMLTool.getChildElementByLocalNameNS(callbackElem, "event", SemaineML.namespaceURI);
//			if( eventElem != null ) {
//				if( eventElem.hasAttribute("type") && eventElem.getAttribute("type").equals("start") && (agentSpeakingState == PREPARING_TO_SPEAK || agentSpeakingState == LISTENING ) ) {
//					return true;
//				}
//			}
//		}
//		return false;
//	}
//
//	public AgentSpokenUtterance manageAgentStart()
//	{
//		String detectedKeywords = "";
//		for( int i=detectedWordsIndex; i<detectedWords.size(); i++ ) {
//			detectedKeywords = detectedKeywords + " " + detectedWords.get(i).toLowerCase();
//		}
//		
//		long user_utterance_length = meta.getTime() - userTurnStart;
//		if( convState == STARTUP_1 ) {
//			if( user_utterance_length < 2000 || detectedKeywords.contains("yes") || detectedKeywords.contains("yeah") || detectedKeywords.contains("here") ) {
//				convState = STARTUP_2;
//				return pickUtterances( "startup3" );
//			} else {
//				return null;
//			}
//		}
//		return null;
//	}
//
//	public AgentSpokenUtterance getResponse()
//	{
//		resetCategoryChances();
//		if( detectedWordsIndex == detectedWords.size() ) {
////			if( convState == STARTUP_2 ) {
////				agentSpeakingState = PREPARINT_TO_SPEAK;
////				convState = SITUATION_EXPLAINED;
////				return pickUtterances("explain_what_happened");
////			} else {
//				agentSpeakingState = PREPARING_TO_SPEAK;
//				return pickUtterances("after_a_silence");
////			}
//		}
//		
//		for( int i=detectedWordsIndex; i<detectedWords.size(); i++ ) {
//			String str = detectedWords.get(i).toLowerCase();
//			Double chance;
//
//			/* Category 'explain_what_happened' */
//			chance = categoryChances.get("explain_what_happened");
//			if( str.matches("((\\S)+ +)*what ((\\S)+ +){0,3}(happen)( *(\\S)+)*") ) {
//				chance += 0.4;
//			} else if( str.matches("((\\S)+ +)*(happen|happens|happened)( *(\\S)+)*") ) {
//				chance += 0.2;
//			}
//			if( str.matches("((\\S)+ +)*what ((\\S)+ +){0,2}matter( *(\\S)+)*") ) {
//				chance += 0.4;
//			}
//			if( str.matches("((\\S)+ +)*what ((\\S)+ +){0,2}wrong( *(\\S)+)*") ) {
//				chance += 0.4;
//			}
//			if( str.matches("((\\S)+ +)*why ((\\S)+ +){0,2}you ((\\S)+ +){0,2}here( *(\\S)+)*") || str.matches("why ((\\S)+ +){0,4}here( *(\\S)+)*") ) {
//				chance += 0.4;
//			}
//			if( str.matches("((\\S)+ +)*(happen|happens|happened)( *(\\S)+)*") ) {
//				chance =+ 0.2;
//			}
//			categoryChances.put("explain_what_happened", chance);
//			defaultCategoryChances.put("explain_what_happened", defaultCategoryChances.get("explain_what_happened")-0.5);
//
//			/* Category 'explain_when_it_happened' */
//			chance = categoryChances.get("explain_when_it_happened");
//			if( !agentUtteranceHistory.get( agentUtteranceHistory.size()-1).getUtterance().getCategory().equals("explain_what_happened") ) {
//				chance -= 0.2;
//			}
//			if( str.matches("((\\S)+ +)*when ((\\S)+ +){0,3}(happen)( *(\\S)+)*") ) {
//				chance += 0.6;
//			} else if( str.matches("((\\S)+ +)*when( *(\\S)+)*") ) {
//				chance += 0.4;
//			}
//			if( str.matches("((\\S)+ +)*long ago( *(\\S)+)*") ) {
//				chance += 0.4;
//			} else if( str.matches("((\\S)+ +)*ago( *(\\S)+)*") ) {
//				chance += 0.2;
//			}
//			categoryChances.put("explain_when_it_happened", chance);
//
//
//			/* Category 'explain_what_kind_of_supplies' */
//			chance = categoryChances.get("explain_what_kind_of_supplies");
//			if( !agentUtteranceHistory.get( agentUtteranceHistory.size()-1).getUtterance().getCategory().equals("explain_what_happened") ) {
//				chance -= 0.2;
//			}
//			if( str.matches("((\\S)+ +)*what ((\\S)+ +){0,2}(type|kind) ((\\S)+ +){0,2}supplies( *(\\S)+)*") ) {
//				chance += 0.6;
//			} else if( str.matches("((\\S)+ +)*(type|kind) ((\\S)+ +){0,2}supplies( *(\\S)+)*") ) {
//				chance += 0.4;
//			} else {
//				if( str.matches("((\\S)+ +)*(type|kind)( *(\\S)+)*") ) {
//					chance += 0.0;
//				}
//				if( str.matches("((\\S)+ +)*supplies( *(\\S)+)*") ) {
//					chance += 0.2;
//				}
//			}
//
//			if( str.matches("((\\S)+ +)*what ((\\S)+ +){0,2}(carry|bring)( *(\\S)+)*") ) {
//				chance += 0.6;
//			} else if( str.matches("((\\S)+ +)*(carry|bring)( *(\\S)+)*") ) {
//				chance += 0.4;
//			}
//			categoryChances.put("explain_what_kind_of_supplies", chance);
//
//
//			/* Category 'introduction' */
//			chance = categoryChances.get("introduction");
//			if( str.matches("((\\S)+ +)*who ((\\S)+ +){0,2}you( *(\\S)+)*") ) {
//				chance += 0.6;
//			} else if( str.matches("((\\S)+ +)*who( *(\\S)+)*") ) {
//				chance += 0.3;
//			}
//
//			if( str.matches("((\\S)+ +)*what ((\\S)+ +){0,2}(name|role|rank|spaceship)( *(\\S)+)*") ) {
//				chance += 0.5;
//			}
//			if( str.matches("((\\S)+ +)*where ((\\S)+ +){0,2}from( *(\\S)+)*") ) {
//				chance += 0.4;
//			}
//			if( str.matches("((\\S)+ +)*(name|role|rank|spaceship)( *(\\S)+)*") ) {
//				chance += 0.0;
//			}
//			categoryChances.put("introduction", chance);
//
//			/* Category 'yes_i_am_human' */
//			chance = categoryChances.get("yes_i_am_human");
//			if( str.matches("((\\S)+ +)*you ((\\S)+ +){0,2}human( *(\\S)+)*") ) {
//				chance += 0.4;
//			}
//			if( str.matches("((\\S)+ +)*human( *(\\S)+)*") ) {
//				chance += 0.2;
//			}
//			categoryChances.put("yes_i_am_human", chance);
//
//			/* Category 'i_am_no_unimat' */
//			chance = categoryChances.get("i_am_no_unimat");
//			if( str.matches("((\\S)+ +)*you ((\\S)+ +){0,3}(unimat|spy|infiltrator)( *(\\S)+)*") ) {
//				chance += 0.4;
//			}
//			if( str.matches("((\\S)+ +)*(unimat|spy|infiltrator)( *(\\S)+)*") ) {
//				chance += 0.2;
//			}
//			categoryChances.put("i_am_no_unimat", chance);
//
//			/* Category 'no_more_of_us' */
//			chance = categoryChances.get("no_more_of_us");
//			if( str.matches("((\\S)+ +)*(more|many) ((\\S)+ +){0,2}(you|survivors)( *(\\S)+)*") ) {
//				chance += 0.5;
//			} else if( str.matches("((\\S)+ +)*other ((\\S)+ +){0,2}survivors( *(\\S)+)*") ) {
//				chance += 0.5;
//			} else if( str.matches("((\\S)+ +)*(other|others|more)( *(\\S)+)*") ) {
//				chance += 0.2;
//			}
//			categoryChances.put("no_more_of_us", chance);
//
//			/* Category 'reasons_to_let_me_in' */
//			chance = categoryChances.get("reasons_to_let_me_in");
//			if( str.matches("((\\S)+ +)*why ((\\S)+ +){0,2}(would|should) ((\\S)+ +){0,2}(dock|in|help|do|that)( *(\\S)+)*") ) {
//				chance += 0.6;
//			} else if( str.matches("((\\S)+ +)*(would|should) ((\\S)+ +){0,2}(dock|in|help)( *(\\S)+)*") ) {
//				chance += 0.3;
//			} else if( str.matches("((\\S)+ +)*why( *(\\S)+)*") ) {
//				chance += 0.2;
//			} else if( str.matches("((\\S)+ +)*no( *(\\S)+)*") ) {
//				chance += 0.2;
//			}
//			categoryChances.put("reasons_to_let_me_in", chance);
//
//			/* Category 'reasons_to_not_shoot_me' */
//			chance = categoryChances.get("reasons_to_not_shoot_me");
//			if( str.matches("((\\S)+ +)*why ((\\S)+ +){0,2}(kill|destroy|shoot)( *(\\S)+)*") ) {
//				chance += 0.6;
//			}
//			categoryChances.put("reasons_to_not_shoot_me", chance);
//
//			/* Category 'present_gifts' */
//			chance = categoryChances.get("present_gifts");
//			if( str.matches("((\\S)+ +)*what ((\\S)+ +){0,2}(give|offer|have)( *(\\S)+)*") ) {
//				chance += 0.5;
//			}
//			if( str.matches("((\\S)+ +)*i ((\\S)+ +){0,2}want( *(\\S)+)*") ) {
//				chance += 0.2;
//			}
//			if( str.matches("((\\S)+ +)*you ((\\S)+ +){0,2}(have|get|give)( *(\\S)+)*") ) {
//				chance += 0.4;
//			}
//			categoryChances.put("present_gifts", chance);
//
//			/* Category 'explain_why_dying' */
//			chance = categoryChances.get("explain_why_dying");
//			if( str.matches("((\\S)+ +)*why ((\\S)+ +){0,2}you ((\\S)+ +){0,2}die( *(\\S)+)*") ) {
//				chance += 0.5;
//			} else if( str.matches("((\\S)+ +)*why ((\\S)+ +){0,2}die( *(\\S)+)*") ) {
//				chance += 0.3;
//			}
//			if( str.matches("((\\S)+ +)*what ((\\S)+ +){0,2}(happen|happens|happened) ((\\S)+ +){0,2}(when|if)( *(\\S)+)*") ) {
//				chance += 0.5;
//			}
//			categoryChances.put("explain_why_dying", chance);
//
//			/* Category 'ship_broken' */
//			chance = categoryChances.get("ship_broken");
//			if( str.matches("((\\S)+ +)*what ((\\S)+ +){0,2}wrong ((\\S)+ +){0,2}(ship|vessel)( *(\\S)+)*") ) {
//				chance += 0.6;
//			} else if( str.matches("((\\S)+ +)*wrong ((\\S)+ +){0,2}(ship|vessel)( *(\\S)+)*") ) {
//				chance += 0.5;
//			}
//			if( str.matches("((\\S)+ +)*(anything|ship|vessel) ((\\S)+ +){0,2}broken( *(\\S)+)*") ) {
//				chance += 0.5;
//			}
//			if( str.matches("((\\S)+ +)*why ((\\S)+ +){0,2}(collide|hit)( *(\\S)+)*") ) {
//				chance += 0.5;
//			}
//			if( str.matches("((\\S)+ +)*can ((\\S)+ +){0,2}(steer|move|stop)( *(\\S)+)*") ) {
//				chance += 0.5;
//			} else if( str.matches("((\\S)+ +)*(steer|move|stop)( *(\\S)+)*") ) {
//				chance += 0.3;
//			}
//			categoryChances.put("ship_broken", chance);
//
//			/* Category 'ship_cannot_be_repaired' */
//			chance = categoryChances.get("ship_cannot_be_repaired");
//			if( str.matches("((\\S)+ +)*can ((\\S)+ +){0,2}(repair|fix|do anything)( *(\\S)+)*") ) {
//				chance += 0.5;
//			}
//			categoryChances.put("ship_cannot_be_repaired", chance);
//
//			/* Category 'proof_i_am_human' */
//			chance = categoryChances.get("proof_i_am_human");
//			if( str.matches("((\\S)+ +)*can ((\\S)+ +){0,2}(prove|convince|show) ((\\S)+ +){0,2}(human|unimat)( *(\\S)+)*") ) {
//				chance += 0.6;
//			}
//			if( str.matches("((\\S)+ +)*why ((\\S)+ +){0,2}believe( *(\\S)+)*") ) {
//				chance += 0.4;
//			}
//			if( str.matches("((\\S)+ +)*i ((\\S)+ +){0,2}believe( *(\\S)+)*") ) {
//				chance += 0.2;
//			}
//			if( str.matches("((\\S)+ +)*can ((\\S)+ +){0,2}trust( *(\\S)+)*") ) {
//				chance += 0.4;
//			}
//			if( str.matches("((\\S)+ +)*sure ((\\S)+ +){0,4}cheating( *(\\S)+)*") ) {
//				chance += 0.4;
//			}
//			categoryChances.put("proof_i_am_human", chance);
//		}
//		
//		for( String str : categoryChances.keySet() ) {
//			System.out.println(categoryChances.get(str) + "	- " + str);
//		}
//		
//		double maxChance = -1d;
//		String maxCategory = "";
//		for( String key : categoryChances.keySet() ) {
//			if( categoryChances.get(key) > maxChance ) {
//				maxCategory = key;
//				maxChance = categoryChances.get(key);
//			}
//		}
//
//		if( maxChance < 0.1 ) {
////			if( convState == STARTUP_2 ) {
////				maxCategory = "explain_what_happened";
////				convState = SITUATION_EXPLAINED;
////			} else {
//				if( meta.getTime() - systemStartTime < 70000 ) {
//					maxCategory = "unknown_question_low_arousal";
//				} else {
//					maxCategory = "unknown_question_high_arousal";
//				}
////			}
//		}
//		
//		if( convState == STARTUP_2 ) {
//			convState = SITUATION_EXPLAINED;
//		}
//
//		return pickUtterances(maxCategory);
//	}
//
//	public AgentSpokenUtterance pickUtterances( String type )
//	{
//		/* Get all utterances of the given type that haven't been used for the last x utterances */
//		HashMap<String,ArrayList<AgentUtterance>> utterancesChar = allUtterances.get(3);
//		ArrayList<AgentUtterance> utterancesType = utteranceCopy( utterancesChar.get(type) );
//
//		ArrayList<AgentUtterance> utteranceToRemove = new ArrayList<AgentUtterance>();
//		for( int i=agentUtteranceHistory.size()-1; i>=(Math.max(0, agentUtteranceHistory.size()-3)); i-- ) {
//			AgentSpokenUtterance spUttr = agentUtteranceHistory.get(i);
//			for( AgentUtterance uttr : utterancesType ) {
//				if( uttr.getUtterance().equals(spUttr.getUtterance()) ) {
//					utteranceToRemove.add(uttr);
//				}
//			}
//			for( AgentUtterance uttr : utteranceToRemove ) {
//				utterancesType.remove( uttr );
//			}
//		}
//
//		if( utterancesType.size() == 0 ) {
//			/* If the list is empty do something else */
//			if( meta.getTime() - systemStartTime < 70000 ) {
//				return pickUtterances("unknown_question_low_arousal");
//			} else {
//				return pickUtterances("unknown_question_high_arousal");
//			}
//
//		} else {
//			/* If the list isn't empty randomly pick an utterance from the list */
//			System.out.println("Saying utterance of type: " + type);
//			if( type.equals("unknown_question_low_arousal") && !firstRandomAnswerGiven ) {
//				firstRandomAnswerGiven = true;
//				return new AgentSpokenUtterance( new AgentUtterance( "I'm on a collision course with your ship and it will hit you in a few minutes. Please let me in.",type ) );
//			} else {
//				return new AgentSpokenUtterance( utterancesType.get(rand.nextInt(utterancesType.size())) );
//			}
//		}
//	}
//
//	/**
//	 * Reads the messages from the TurnTakingInterpreter and decides if the agent should 
//	 * start speaking or not.
//	 * @param m - the received message
//	 * @return - true if the user should speak, false it if shouldn't
//	 */
//	public boolean agentShouldSpeak( SEMAINEMessage m )
//	{
//		/* Check if the system has started, if not return false */
//		if( !systemStarted ) {
//			return false;
//		}
//
//		if( m instanceof SEMAINEStateMessage ) {
//			SEMAINEStateMessage am = (SEMAINEStateMessage)m;
//
//			StateInfo agentInfo = am.getState();
//			if (agentInfo.getType() == StateInfo.Type.AgentState) {
//				Map<String,String> agentInfoMap = agentInfo.getInfos();
//
//				String intention = agentInfoMap.get("turnTakingIntention");
//				if( intention != null && intention.equals("startSpeaking") ) {
//					if( agentSpeakingState == SPEAKING || agentSpeakingState == PREPARING_TO_SPEAK ) {
//						return false;
//					} else {
//						return true;
//					}
//				} else {
//					return false;
//				}
//			}
//		}
//		return false;
//	}
//
//	/**
//	 * Reads the received Message and tries to filter out the detected user speaking state.
//	 * @param m - the received message
//	 */
//	public void setUserSpeakingState(StateInfo userInfo)
//	{
//		if( userInfo.hasInfo("speaking") ) {
//			if( userInfo.getInfo("speaking").equals("true") ) {
//				if( userSpeakingState != SPEAKING ) {
//					userSpeakingState = SPEAKING;
//					userSpeakingStateTime = meta.getTime();
//					if( userTurnStart == -1 ) {
//						userTurnStart = userSpeakingStateTime;
//					}
//				}
//			} else if( userInfo.getInfo("speaking").equals("false") ) {
//				if( userSpeakingState != LISTENING ) {
//					userSpeakingState = LISTENING;
//					userSpeakingStateTime = meta.getTime();
//				}
//			}
//		}
//	}
//
//	/**
//	 * Reads the received Message and tries to filter out the detected Emotion Events.
//	 * @param m
//	 */
//	public void addDetectedEmotions(StateInfo userInfo)
//	{
//		if( userInfo.hasInfo("valence") ) {
//			float valence = Float.parseFloat( userInfo.getInfo("valence") );
//			EmotionEvent ee = new EmotionEvent( meta.getTime(), 0, EmotionEvent.VALENCE, valence );
//			detectedEmotions.add( ee );
//		} else if( userInfo.hasInfo("arousal") ) {
//			float arousal = Float.parseFloat( userInfo.getInfo("arousal") );
//			EmotionEvent ee = new EmotionEvent( meta.getTime(), 0, EmotionEvent.AROUSAL, arousal );
//			detectedEmotions.add( ee );
//		} else if( userInfo.hasInfo("interest") ) {
//			float interest = Float.parseFloat( userInfo.getInfo("interest") );
//			EmotionEvent ee = new EmotionEvent( meta.getTime(), 0, EmotionEvent.INTEREST, interest );
//			detectedEmotions.add( ee );
//		}
//	}
//
//	public void addDetectedWords( StateInfo stateInfo )
//	{
//		if( stateInfo.hasInfo("userUtterance") && stateInfo.hasInfo("userUtteranceStartTime") ) {
//			String utterance = stateInfo.getInfo("userUtterance");
//			long time = Long.parseLong(stateInfo.getInfo("userUtteranceStartTime"));
//
//			if( detectedWordTimes.size() > 1 && detectedWordTimes.get(detectedWordTimes.size()-1) == time )  {
//				detectedWords.set(detectedWords.size()-1, utterance);
//			} else {
//				detectedWords.add(utterance);
//				detectedWordTimes.add(time);
//			}
//		}
//	}
//
//	public void updateCharacterAndUser( StateInfo stateInfo ) throws JMSException
//	{
//		if( stateInfo.hasInfo("userPresent") ) {
//			if( stateInfo.getInfo("userPresent").equals(PRESENT) && !isUserPresent ) {
//				userAppeared();
//			} else if( stateInfo.getInfo("userPresent").equals(ABSENT) && isUserPresent ) {
//				userDisappeared();
//			}
//		}
//
//		if( stateInfo.hasInfo("character") ) {
//			String character = stateInfo.getInfo("character");
//			if( !character.toLowerCase().equals("spike") ) {
//				sendNewCharacter("Spike");
//			}
//
//		}
//	}
//
//	/**
//	 * Sends around that the system has changed to a new character
//	 * @param character the new character
//	 * @throws JMSException
//	 */
//	public void sendNewCharacter( String character ) throws JMSException
//	{
//		Map<String,String> contextInfo = new HashMap<String,String>();
//		contextInfo.put("character", character);
//
//		ContextStateInfo csi = new ContextStateInfo(contextInfo);
//		contextSender.sendStateInfo( csi, meta.getTime() );
//	}
//
//	/**
//	 * Called when a user is detected in the screen.
//	 * @throws JMSException
//	 */
//	public void userAppeared() throws JMSException
//	{
//		System.out.println("User appeared");
//		DMLogger.getLogger().log(meta.getTime(), "System:SystemStarted" );
//
//		isUserPresent = true;
//		systemStarted = true;
//		systemStartTime = meta.getTime();
//
//		agentSpeakingState = PREPARING_TO_SPEAK;
//
//		sendUtterance( pickUtterances("startup2") );
//		convState = STARTUP_1;
//	}
//
//	/**
//	 * Called when the user disappears from the screen
//	 * @throws JMSException
//	 */
//	public void userDisappeared() throws JMSException
//	{
//		DMLogger.getLogger().log(meta.getTime(), "System:SystemStopped" );
//		convState = 0;
//		isUserPresent = false;
//
//		systemStarted = false;
//	}
//
//	/**
//	 * Sends the given utterance to the output modules.
//	 * 
//	 * @param utterance
//	 * @throws JMSException
//	 */
//	public void sendUtterance( AgentSpokenUtterance utterance ) throws JMSException
//	{	
//		/* Send utterance to Greta */
//		String response = utterance.getUtterance().getUtterance();
//
//		if( response.contains("thank goodness") ) {
//			sendUtteranceAsFml( "Ah, thank goodness." );
//			sendUtteranceAsFml( "We were attacked by Unimats while bringing supplies to a refugee camp." );
//			sendUtteranceAsFml( "They killed almost all of our crew I could barely escape." );
//			sendUtteranceAsFml( "Please help me and let me dock." );
//		} else {
//			sendUtteranceAsFml( response );
//		}
//
//		/* Send the speaking-state around */
//		sendSpeaking();
//
//		/* Set indices */
//		emotionIndex = detectedEmotions.size();
//		detectedWordsIndex = detectedWords.size();
//
//		/* Add the utterance to the history */
//		utterance.setTime( meta.getTime() );
//		agentUtteranceHistory.add( utterance );
//
//		System.out.println("Agent speaking");
//		DMLogger.getLogger().log(meta.getTime(), "AgentAction:SendUtterance, type=" + utterance.getUtterance().getCategory() + ", utterance=" + utterance.getUtterance() );
//		
//		/* Set end time (temporary) */
//		utteranceEndTime = meta.getTime() + ( (utterance.getUtterance().getUtterance().split(" ").length * 250)+5000 );
//	}
//	
//	public void sendUtteranceAsFml( String response ) throws JMSException
//	{
//		String id = "s1";
//
//		Document doc = XMLTool.newDocument("fml-apml", null, FML.version);
//		Element root = doc.getDocumentElement();
//
//		Element bml = XMLTool.appendChildElement(root, BML.E_BML, BML.namespaceURI);
//		bml.setAttribute(BML.A_ID, "bml1");
//		Element fml = XMLTool.appendChildElement(root, FML.E_FML, FML.namespaceURI);
//		fml.setAttribute(FML.A_ID, "fml1");
//		Element speech = XMLTool.appendChildElement(bml, BML.E_SPEECH);
//		speech.setAttribute(BML.A_ID, id);
//		speech.setAttribute(BML.E_TEXT, response);
//		speech.setAttribute(BML.E_LANGUAGE, "en-GB");
//
//		//speech.setTextContent(response);
//
//		int counter=1;
//		for( String word : response.split(" ") ) {
//			Element mark = XMLTool.appendChildElement(speech, SSML.E_MARK, SSML.namespaceURI);
//			mark.setAttribute(SSML.A_NAME, id+":tm"+counter);
//			Node text = doc.createTextNode(word);
//			speech.appendChild(text);
//			counter++;
//		}
//		Element mark = XMLTool.appendChildElement(speech, SSML.E_MARK, SSML.namespaceURI);
//		mark.setAttribute(SSML.A_NAME, id+":tm"+counter);
//
//		fmlSender.sendXML(doc, meta.getTime());
//	}
//
//	/**
//	 * Sends around that the agent is speaking
//	 * @throws JMSException
//	 */
//	public void sendSpeaking() throws JMSException
//	{
//		Map<String,String> dialogInfo = new HashMap<String,String>();		
//		dialogInfo.put("agentTurnState", "true");
//
//		DialogStateInfo dsi = new DialogStateInfo(dialogInfo, null);
//		dialogStateSender.sendStateInfo(dsi, meta.getTime());
//	}
//
//	/**s
//	 * Sends around that the agent is silent
//	 * @throws JMSException
//	 */
//	public void sendListening() throws JMSException
//	{
//		Map<String,String> dialogInfo = new HashMap<String,String>();		
//		dialogInfo.put("agentTurnState", "false");
//
//		DialogStateInfo dsi = new DialogStateInfo(dialogInfo, null);
//		dialogStateSender.sendStateInfo(dsi, meta.getTime());
//	}
//
//	/**
//	 * Called when the output module messages that the utterance is finished.
//	 * Will put the agent state on listening again and send this state around.
//	 * TODO: This method isn't used in the correct way at the moment!
//	 * @throws JMSException
//	 */
//	public void processUtteranceEnd() throws JMSException
//	{	
//		if( agentSpeakingState == SPEAKING && meta.getTime() > utteranceEndTime ) {
//			System.out.println("Agent silent (timeout)");
//			agentSpeakingState = LISTENING;
//			agentSpeakingStateTime = meta.getTime();
//			sendListening();
//			userTurnStart = -1;
//		}
//	}
//	
//	/**
//	 * Checks if the user is silent for too long.
//	 */
//	public void checkUserSilenceTime() throws JMSException
//	{
//		if( agentSpeakingState == LISTENING && userTurnStart == -1 && (meta.getTime() - agentSpeakingStateTime > MAX_SILENCE_TIME ) ) {
//			agentSpeakingState = PREPARING_TO_SPEAK;
//			sendUtterance( pickUtterances("after_a_silence") );
//		} else if( convState == STARTUP_2 &&  agentSpeakingState == LISTENING && userTurnStart == -1 && (meta.getTime() - agentSpeakingStateTime > 3000 ) ) {
//			agentSpeakingState = PREPARING_TO_SPEAK;
//			sendUtterance( pickUtterances("explain_what_happened") );
//			convState = SITUATION_EXPLAINED;
//		}
//	}
//
//	/**
//	 * Makes a deepcopy of the given ArrayList
//	 * @param utterances - the list to copy
//	 * @return
//	 */
//	public ArrayList<AgentUtterance> utteranceCopy( ArrayList<AgentUtterance> utterances )
//	{
//		if( utterances == null ) {
//			return new ArrayList<AgentUtterance>();
//		}
//		ArrayList<AgentUtterance> newUtterances = new ArrayList<AgentUtterance>();
//		for( AgentUtterance uttr : utterances ) {
//			newUtterances.add( uttr );
//		}
//		return newUtterances;
//	}
//
//	public void resetCategoryChances()
//	{
//		categoryChances.put("explain_what_happened", defaultCategoryChances.get("explain_what_happened"));
//		categoryChances.put("explain_when_it_happened", defaultCategoryChances.get("explain_when_it_happened"));
//		categoryChances.put("explain_what_kind_of_supplies", defaultCategoryChances.get("explain_what_kind_of_supplies"));
//		categoryChances.put("introduction", defaultCategoryChances.get("introduction"));
//		categoryChances.put("yes_i_am_human", defaultCategoryChances.get("yes_i_am_human"));
//		categoryChances.put("i_am_no_unimat", defaultCategoryChances.get("i_am_no_unimat"));
//		categoryChances.put("no_more_of_us", defaultCategoryChances.get("no_more_of_us"));
//		categoryChances.put("reasons_to_let_me_in", defaultCategoryChances.get("reasons_to_let_me_in"));
//		categoryChances.put("reasons_to_not_shoot_me", defaultCategoryChances.get("reasons_to_not_shoot_me"));
//		categoryChances.put("present_gifts", defaultCategoryChances.get("present_gifts"));
//		categoryChances.put("explain_why_dying", defaultCategoryChances.get("explain_why_dying"));
//		categoryChances.put("ship_broken", defaultCategoryChances.get("ship_broken"));
//		categoryChances.put("ship_cannot_be_repaired", defaultCategoryChances.get("ship_cannot_be_repaired"));
//		categoryChances.put("proof_i_am_human", defaultCategoryChances.get("proof_i_am_human"));
//	}
//	
//	public void resetDefaultCategoryChances()
//	{
//		defaultCategoryChances.put("explain_what_happened", 0d);
//		defaultCategoryChances.put("explain_when_it_happened", 0d);
//		defaultCategoryChances.put("explain_what_kind_of_supplies", 0d);
//		defaultCategoryChances.put("introduction", 0d);
//		defaultCategoryChances.put("yes_i_am_human", 0d);
//		defaultCategoryChances.put("i_am_no_unimat", 0d);
//		defaultCategoryChances.put("no_more_of_us", 0d);
//		defaultCategoryChances.put("reasons_to_let_me_in", 0d);
//		defaultCategoryChances.put("reasons_to_not_shoot_me", 0d);
//		defaultCategoryChances.put("present_gifts", 0d);
//		defaultCategoryChances.put("explain_why_dying", 0d);
//		defaultCategoryChances.put("ship_broken", 0d);
//		defaultCategoryChances.put("ship_cannot_be_repaired", 0d);
//		defaultCategoryChances.put("proof_i_am_human", 0d);
//	}
//}
