/**
 * Copyright (C) 2009 University of Twente. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */

package eu.semaine.components.dialogue.actionproposers;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;
import java.util.Random;

import javax.jms.JMSException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

import eu.semaine.components.Component;
import eu.semaine.components.dialogue.datastructures.AgentSpokenUtterance;
import eu.semaine.components.dialogue.datastructures.AgentUtterance;
import eu.semaine.components.dialogue.datastructures.ContextTemplate;
import eu.semaine.components.dialogue.datastructures.DialogueAct;
import eu.semaine.components.dialogue.datastructures.EmotionEvent;
import eu.semaine.components.dialogue.test.DMLogger;
import eu.semaine.datatypes.stateinfo.ContextStateInfo;
import eu.semaine.datatypes.stateinfo.DialogStateInfo;
import eu.semaine.datatypes.stateinfo.StateInfo;
import eu.semaine.datatypes.xml.BML;
import eu.semaine.datatypes.xml.FML;
import eu.semaine.datatypes.xml.SSML;
import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.message.SEMAINEStateMessage;
import eu.semaine.jms.message.SEMAINEXMLMessage;
import eu.semaine.jms.receiver.StateReceiver;
import eu.semaine.jms.receiver.XMLReceiver;
import eu.semaine.jms.sender.FMLSender;
import eu.semaine.jms.sender.StateSender;
import eu.semaine.util.XMLTool;

/**
 * The UtteranceActionProposer determines what to say based on the current context.
 * TODO: uitbreiden, precieze werking beschrijven in details
 * 
 * Input
 * AgentStateReceiver('emaine.data.state.agent')			--> take/release turn messages
 * UserStateReceiver('semaine.data.state.user.behaviour') 	--> user speaking state and detected emotions
 * XMLReceiver('semaine.data.state.context')				--> For context information such as user present and the current character
 * 
 * Output
 * FMLSender('semaine.data.action.candidate.function')		--> utterances to the output modules
 * DialogStateSender('semaine.data.state.dialog')			--> dialog state (speaker & listener)
 * XMLSender('semaine.data.state.context')					--> For context information such as user present and the current character
 * 
 * @author MaatM
 *
 */

public class UtteranceActionProposer extends Component
{	

	/* Speaking states */
	public final static int WAITING = 0;	// Waiting for the other person to start speaking
	public final static int LISTENING = 1;	// Listening to the speech of the user
	public final static int PREPARING_TO_SPEAK = 2;
	public final static int SPEAKING = 3;	// The agent is speaking

	/* The four characters */
	public final static int POPPY = 1;
	public final static int PRUDENCE = 2;
	public final static int SPIKE = 3;
	public final static int OBADIAH = 4;

	/* Character-change states */
	public final static int NEUTRAL = 0;
	public final static int CHANGE_ASKED = 1;
	public final static int CHAR_SUGGESTED = 2;
	public final static int CHAR_ASKED = 3;

	/* Introduction states */
	public final static int INTRODUCED = 1;
	public final static int HOW_ARE_YOU_ASKED = 2;

	/* Thresholds */
	public final static float HIGH_AROUSAL = 1.0f;
	public final static float LOW_AROUSAL = 0.1f;
	public final static long SMALL_UTTERANCE = 3000;

	/* User-presence variables */
	private static final String PRESENT = "present";
	private static final String ABSENT = "absent";

	/* Data locations */
	private final static String sentenceDataPath = "/eu/semaine/components/dialogue/data/sentences.xml";
	private final static String contextFile = "/eu/semaine/components/dialogue/data/Context.csv";

	/* Senders and Receivers */
	private StateReceiver agentStateReceiver;
	private StateReceiver userStateReceiver;
	private StateReceiver contextReceiver;
	private XMLReceiver callbackReceiver;
	private FMLSender fmlSender;
	private StateSender dialogStateSender;
	private StateSender contextSender;

	/* Random generator */
	private Random rand = new Random();

	/* The current state of the agent */
	private int currChar; // The current character

	/* The current state of the agent */
	public int agentSpeakingState = 1; 			// The current speaking state of the agent (SPEAKING, LISTENING or WAITING)
	private long agentSpeakingStateTime = 0;	// The starttime of the current speaking state of the agent
	private long utteranceEndTime = 0;			// The predicted endtime of the agent utterance
	private ArrayList<AgentSpokenUtterance> utteranceHistory = new ArrayList<AgentSpokenUtterance>();
	private int subjectIndex = 0;				// The index to the Agent utterance which is the start of the current topic

	/* The current state of the user */
	private int userSpeakingState = 1;			// The current speaking state of the agent (SPEAKING, LISTENING or WAITING)
	private long userSpeakingStateTime = 0;		// The starttime of the current speaking state of the agent
	private long userTurnStart = -1;			// The starttime of the user turn, set to -1 after an agent turn

	/* Global state information */
	private boolean systemStarted = false;		// Indicates if the system has started or not
	private boolean isUserPresent = false;		// Indicates if the user is currently present or not
	private int nrTopicChanges = 0;				// The number of topic changes for the current character so far
	private int tellMeMoreCounter = 0;			// How many Tell Me More utterances are uttered within this subject
	public int charChangeState = 0;				// The current state in the character-change process
	public int charStartupState = 0;			// The current state in the startup process
	public int suggestedChar = 0;				// The suggested next character

	/* Detected events */
	/* List of detected emotion events (generated by the EmotionInterpreter) */
	private ArrayList<EmotionEvent> detectedEmotions = new ArrayList<EmotionEvent>();
	private int emotionIndex = 0;

	/* List of all detected Dialogue Acts (generated by the UtteranceInterpreter) */
	private ArrayList<DialogueAct> detectedDActs = new ArrayList<DialogueAct>();
	private int dActIndex = 0;

	private HashMap<Integer,String> charNames = new HashMap<Integer,String>();
	private HashMap<String,Integer> charNumbers = new HashMap<String,Integer>();
	private HashMap<Integer,Boolean> charHistory = new HashMap<Integer,Boolean>();

	/* All agent utterances grouped by utterance-type */
	private HashMap<Integer,HashMap<String,ArrayList<AgentUtterance>>> allUtterances = new HashMap<Integer,HashMap<String,ArrayList<AgentUtterance>>>();

	/* A list with all context-templates, used for picking a response */
	private ArrayList<ContextTemplate> templates ;


	/**
	 * Constructor of UtteranceActionProposer
	 * Initializes the senders and receivers, randomly determines the first character and 
	 * initializes some data
	 * @throws JMSException
	 */
	public UtteranceActionProposer() throws JMSException
	{
		super( "UtteranceActionProposer" );

		/* Initialize receivers */
		agentStateReceiver = new StateReceiver( "semaine.data.state.agent", StateInfo.Type.AgentState );
		receivers.add( agentStateReceiver );
		userStateReceiver = new StateReceiver("semaine.data.state.user.behaviour", StateInfo.Type.UserState);
		receivers.add(userStateReceiver);
		contextReceiver = new StateReceiver("semaine.data.state.context", StateInfo.Type.ContextState);
		receivers.add( contextReceiver );
		callbackReceiver = new XMLReceiver("semaine.callback.output.audio");
		receivers.add(callbackReceiver);

		/* Initialize senders */
		fmlSender = new FMLSender("semaine.data.action.candidate.function", getName());
		senders.add(fmlSender);
		dialogStateSender = new StateSender("semaine.data.state.dialog", StateInfo.Type.DialogState, getName());
		senders.add(dialogStateSender);
		contextSender = new StateSender("semaine.data.state.context", StateInfo.Type.ContextState, getName());
		senders.add(contextSender);

		/* Determine the first character */
		//currChar = rand.nextInt(4)+1;
		currChar = PRUDENCE; // always start with Prudence

		/* Initialize some data */
		initData();

		/* Import response-model data */		
		DataImporter importer = new DataImporter( contextFile, null );
		templates = importer.importContextData();
	}

	/**
	 * Initializes the utterances of the characters, the character names, and the character history
	 */
	public void initData()
	{
		/* Reads the utterances of all the characters */
		SentenceReader sentenceReader = new SentenceReader( sentenceDataPath );
		if( sentenceReader.readData() ) {
			allUtterances = sentenceReader.getAllUtterances();
		} else {
			// TODO: Log error
		}

		/* Set the character name to number conversion maps */
		charNames.put( POPPY, "Poppy" );
		charNames.put( PRUDENCE, "Prudence" );
		charNames.put( SPIKE, "Spike" );
		charNames.put( OBADIAH, "Obadiah" );

		charNumbers.put("Poppy", POPPY);
		charNumbers.put("Prudence", PRUDENCE);
		charNumbers.put("Spike", SPIKE);
		charNumbers.put("Obadiah", OBADIAH);

		/* resets the chat history of the characters (this determines if the characters have spoken
		 * with these characters before in this conversation */
		charHistory.put( POPPY, false );
		charHistory.put( PRUDENCE, false );
		charHistory.put( SPIKE, false );
		charHistory.put( OBADIAH, false );
	}

	/**
	 * Checks if the system has started. If it hasn't it will start it by making the first
	 * character introduce itself.
	 */
	public void act() throws JMSException
	{
		processUtteranceEnd();
//		if( !systemStarted && isUserPresent ) {
//		/* Update agent speaking state */
//		agentSpeakingState = SPEAKING;

//		charStartupState = INTRODUCED;
//		sendNewCharacter( currChar );

//		charHistory.put(currChar, true);
//		sendUtterance( pickUtterances("intro_new") );

//		systemStarted = true;

//		/* TEMPORARILY called until the end of the agent utterance is received from the output module */
//		processUtteranceEnd();
//		}
	}

	/**
	 * Sets context variables if updates are received.
	 * If it receives the message that the agent should start talking it will determine what to say
	 * and output this.
	 */
	public void react( SEMAINEMessage m ) throws JMSException
	{
		if( m instanceof SEMAINEXMLMessage ){
			SEMAINEXMLMessage xm = ((SEMAINEXMLMessage)m);
			if( speechStarted(xm) ) {
				DMLogger.getLogger().log(meta.getTime(), "AgentAction:UtteranceStarted" );
				agentSpeakingState = SPEAKING;
				agentSpeakingStateTime = meta.getTime();
			}

			if( speechReady(xm) ) {
				DMLogger.getLogger().log(meta.getTime(), "AgentAction:UtteranceStopped" );
				agentSpeakingState = LISTENING;
				agentSpeakingStateTime = meta.getTime();
				sendListening();
				userTurnStart = -1;
			}
		}

		/* Processes User state updates */
		if( m instanceof SEMAINEStateMessage ) {
			SEMAINEStateMessage sm = ((SEMAINEStateMessage)m);
			StateInfo stateInfo = sm.getState();
			StateInfo.Type stateInfoType = stateInfo.getType();

			switch (stateInfoType) {
			case UserState:
				/* Updates user speaking state (speaking or silent) */
				setUserSpeakingState(stateInfo);

				/* Updates detected emotions (valence, arousal, interest) */
				addDetectedEmotions(stateInfo);

				/* Updates the detected and analysed user utterances */
				addDetectedDActs(stateInfo);
				break;
			case ContextState:
				/* Updates the current character and the user */
				updateCharacterAndUser( stateInfo );
				break;
			}
		}

		/* If the TurnTakingInterpreter decides that the agent should speak, determine what to say */
		if( agentShouldSpeak( m ) ) {

			/* Update agent speaking state */
			agentSpeakingState = PREPARING_TO_SPEAK;

			AgentSpokenUtterance spokenUtterance = null;

			/* Check if the character change process should be started or continued */
			spokenUtterance = manageCharChange();
			if( spokenUtterance == null ) {
				/* Check if the character start process should be started or continued */
				spokenUtterance = manageAgentStart();
				if( spokenUtterance == null ) {
					/* Get an utterance to say */
					//spokenUtterance = getResponse(); // TODO: TESTING
					spokenUtterance = getResponse2(); // TODO: TESTING
				}
			}

			if( spokenUtterance.getNewCharacter() != -1 ) {
				sendNewCharacter( spokenUtterance.getNewCharacter() );
			} else {
				/* Distribute the chosen utterance */
				sendUtterance( spokenUtterance );
			}			
		}
	}

	public boolean speechReady( SEMAINEXMLMessage xm )
	{
		Element callbackElem = XMLTool.getChildElementByLocalNameNS(xm.getDocument(), "callback", SemaineML.namespaceURI);
		if( callbackElem != null ) {
			Element eventElem = XMLTool.getChildElementByLocalNameNS(callbackElem, "event", SemaineML.namespaceURI);
			if( eventElem != null ) {
				if( eventElem.hasAttribute("type") && eventElem.getAttribute("type").equals("end") && agentSpeakingState == SPEAKING ) {
					return true;
				}
			}
		}
		return false;
	}

	public boolean speechStarted( SEMAINEXMLMessage xm )
	{
		Element callbackElem = XMLTool.getChildElementByLocalNameNS(xm.getDocument(), "callback", SemaineML.namespaceURI);
		if( callbackElem != null ) {
			Element eventElem = XMLTool.getChildElementByLocalNameNS(callbackElem, "event", SemaineML.namespaceURI);
			if( eventElem != null ) {
				if( eventElem.hasAttribute("type") && eventElem.getAttribute("type").equals("start") && (agentSpeakingState == PREPARING_TO_SPEAK || agentSpeakingState == LISTENING ) ) {
					return true;
				}
			}
		}
		return false;
	}

	public void updateCharacterAndUser( StateInfo stateInfo ) throws JMSException
	{
		//Map<String,String> contextInfo = stateInfo.getInfos();
		boolean newUser = false;

		if( stateInfo.hasInfo("userPresent") ) {
			if( stateInfo.getInfo("userPresent").equals("present") && !isUserPresent ) {
				newUser = true;
				userAppeared();
			} else if( stateInfo.getInfo("userPresent").equals("absent") && isUserPresent ) {
				userDisappeared();
			}
		}

		if( stateInfo.hasInfo("character") ) {
			currChar = charNumbers.get( stateInfo.getInfo("character") );
			if( systemStarted ) {
				charStartupState = INTRODUCED;
				nrTopicChanges = 0;

				/* Update agent speaking state */
				agentSpeakingState = PREPARING_TO_SPEAK;

				if( charHistory.get(currChar) ) {
					sendUtterance( pickUtterances("intro_old") );
				} else {
					charHistory.put(currChar, true);
					sendUtterance( pickUtterances("intro_new") );
				}
			}
		}

		if( newUser && charStartupState != INTRODUCED ) {
			charStartupState = INTRODUCED;
			nrTopicChanges = 0;

			/* Update agent speaking state */
			agentSpeakingState = PREPARING_TO_SPEAK;
			AgentSpokenUtterance spokenUtterance;

			if( charHistory.get(currChar) ) {
				sendUtterance( pickUtterances("intro_old") );
			} else {
				charHistory.put(currChar, true);
				sendUtterance( pickUtterances("intro_new") );
			}
		}
	}

	/**
	 * Called when a user is detected in the screen.
	 * @throws JMSException
	 */
	public void userAppeared() throws JMSException
	{
		DMLogger.getLogger().log(meta.getTime(), "System:SystemStarted" );
		System.out.println("User appeared");
		isUserPresent = true;

		systemStarted = true;
	}

	/**
	 * Called when the user disappears from the screen
	 * @throws JMSException
	 */
	public void userDisappeared() throws JMSException
	{
		DMLogger.getLogger().log(meta.getTime(), "System:SystemStopped" );
		charStartupState = NEUTRAL;
		isUserPresent = false;

		/* Update agent speaking state */
		agentSpeakingState = PREPARING_TO_SPEAK;

		sendUtterance( pickUtterances("goodbye") );

		/* resets the chat history of the characters (this determines if the characters have spoken
		 * with these characters before in this conversation */
		charHistory.put( POPPY, false );
		charHistory.put( PRUDENCE, false );
		charHistory.put( SPIKE, false );
		charHistory.put( OBADIAH, false );

		systemStarted = false;
	}

	/**
	 * Manages the character change process.
	 * If the user is in this process it will determine what the next step is and return 
	 * an AgentUtterance to speak.
	 * If the user is not in the character change process it will return null
	 * @throws JMSException
	 */
	public AgentSpokenUtterance manageCharChange() throws JMSException
	{
		/* Make a list of all analyzed Dialogue Acts since the last time the agent talked */
		ArrayList<DialogueAct> recentDActs = new ArrayList<DialogueAct>( detectedDActs.subList(dActIndex, detectedDActs.size()) );

		if( charChangeState == NEUTRAL ) {
			/* Determine if the user wants to change the character */
			boolean wantChange = false;
			String targetCharacter = null;
			for( DialogueAct act : recentDActs ) {
				if( act.isChangeSpeaker() ) {
					// User wants to change the speaker
					wantChange = true;
				}
				if( act.getTargetCharacter() != null ) {
					targetCharacter = act.getTargetCharacter();
				}
			}

			/* If the use wants to change the system has to determine the next character */
			if( wantChange ) {
				if( targetCharacter != null ) {
					/* If the user already mentioned a new character then take this */
//					currChar = charNumbers.get(targetCharacter);
//					charStartupState = INTRODUCED;
//					nrTopicChanges = 0;
					charChangeState = NEUTRAL;
//					sendNewCharacter( charNumbers.get(targetCharacter) );
					return new AgentSpokenUtterance( charNumbers.get(targetCharacter) );
//					if( charHistory.get(currChar) ) {
//					return pickUtterances("intro_old");
//					} else {
//					charHistory.put(currChar, true);
//					return pickUtterances("intro_new");
//					}
				} else {
					/* If the user did not mention a character then either ask for it or propose one */
					if( rand.nextBoolean() ) {
						// Ask for the character
						charChangeState = CHAR_ASKED;
						return pickUtterances("ask_next_character");
					} else {
						// Suggest a character
						charChangeState = CHAR_SUGGESTED;
						suggestedChar = rand.nextInt(4)+1;
						while( suggestedChar == currChar ) {
							suggestedChar = rand.nextInt(4)+1;
						}
						return new AgentSpokenUtterance( new AgentUtterance("Do you want to talk to " + charNames.get(suggestedChar) + "?","change_character") );
					}
				}
			}
		} else if( charChangeState == CHAR_ASKED ) {
			/* If the system just asked for the next character it will have to determine if a suggestion was made */
			String targetCharacter = null;
			for( DialogueAct act : recentDActs ) {
				if( act.getTargetCharacter() != null ) {
					targetCharacter = act.getTargetCharacter();
				}
			}

			if( targetCharacter != null ) {
				/* If the user chose a character then take this one */
				charChangeState = NEUTRAL;
//				sendNewCharacter( charNumbers.get(targetCharacter) );
				return new AgentSpokenUtterance( charNumbers.get(targetCharacter) );
//				charStartupState = INTRODUCED;
//				nrTopicChanges = 0;
//				if( charHistory.get(currChar) ) {
//				return pickUtterances("intro_old");
//				} else {
//				charHistory.put(currChar, true);
//				return pickUtterances("intro_new");
//				}
			} else {
				/* If the user did not choose a character then try to repair it */
				return pickUtterances("repair_ask_next_character");
			}
		} else if( charChangeState == CHAR_SUGGESTED ) {
			/* If the system just suggested a character than it will have to determine if the user
			 * agreed or disagreed with this suggestion */
			boolean agree = false;
			boolean disagree = false;
			for( DialogueAct act : recentDActs ) {
				if( act.isAgree() ) {
					agree = true;
				}
				if( act.isDisagree() ) {
					disagree = true;
				}
			}
			if( !agree && !disagree ) {
				/* If the user did not give any sign try to repair it */
				return pickUtterances("repair_suggest_next_character");
			} else if( agree ) {
				/* If the user agreed to the suggestion then use it */
				charChangeState = NEUTRAL;
//				sendNewCharacter( suggestedChar );
				return new AgentSpokenUtterance( suggestedChar );
//				charStartupState = INTRODUCED;
//				nrTopicChanges = 0;
//				if( charHistory.get(currChar) ) {
//				return pickUtterances("intro_old");
//				} else {
//				charHistory.put(currChar, true);
//				return pickUtterances("intro_new");
//				}
			} else if( disagree ) {
				/* If the user disagreed to the suggestion then ask for the next character */
				charChangeState = CHAR_ASKED;
				return pickUtterances("ask_next_character");
			}
		}
		return null;
	}

	/**
	 * Manages the agent startup process
	 * If the user is in this process it will determine what the next step in the process is
	 * and return an AgentUtterance to speak.
	 * If the user is not in this process it will return null.
	 */
	public AgentSpokenUtterance manageAgentStart( )
	{
		if( charStartupState == INTRODUCED ) {
			/* If the system just introduced himself then ask how the user feels today */
			if( userTurnStart == -1 ) {
				return pickUtterances("after_silence");
			}
			charStartupState = HOW_ARE_YOU_ASKED;
			return pickUtterances("intro_how_are_you");
		} else if( charStartupState == HOW_ARE_YOU_ASKED ) {
			/* If the system just asked how the user feels it will ask the user to tell it more */
			if( userTurnStart == -1 ) {
				return pickUtterances("after_silence");
			}
			charStartupState = NEUTRAL;
			return pickUtterances("intro_tell_me_more");
		}
		return null;
	}

	/**
	 * Reads the messages from the TurnTakingInterpreter and decides if the agent should 
	 * start speaking or not.
	 * @param m - the received message
	 * @return - true if the user should speak, false it if shouldn't
	 */
	public boolean agentShouldSpeak( SEMAINEMessage m )
	{
		/* Check if the system has started, if not return false */
		if( !systemStarted ) {
			return false;
		}

		if( m instanceof SEMAINEStateMessage ) {
			SEMAINEStateMessage am = (SEMAINEStateMessage)m;

			StateInfo agentInfo = am.getState();
			if (agentInfo.getType() == StateInfo.Type.AgentState) {
				Map<String,String> agentInfoMap = agentInfo.getInfos();

				String intention = agentInfoMap.get("turnTakingIntention");
				if( intention != null && intention.equals("startSpeaking") ) {
					if( agentSpeakingState == SPEAKING || agentSpeakingState == PREPARING_TO_SPEAK ) {
						return false;
					} else {
						return true;
					}
				} else {
					return false;
				}
			}
		}
		return false;
	}
	
	public AgentSpokenUtterance getResponse2()
	{
		ArrayList<AgentSpokenUtterance> possibleUtterances = new ArrayList<AgentSpokenUtterance>();
		ArrayList<AgentSpokenUtterance> suggestions;
		
		/* Get combined dialogue acts of the previous user turn */
		DialogueAct act = getCombinedUserDialogueAct();
		
		/* After silence */
		suggestions = suggestAfterSilence();
		possibleUtterances.addAll(suggestions);
		
		/* Linking sentences */
		suggestions = suggestLinkingSentence( act );
		possibleUtterances.addAll(suggestions);
		
		/* Based on content features */
		suggestions = suggestOnContentFeatures( act );
		possibleUtterances.addAll(suggestions);
		
		/* Based on arousal */
		suggestions = suggestOnArousal();
		possibleUtterances.addAll(suggestions);
		
		/* Last resort options, will always return something */
		suggestions = suggestLastOptions();
		possibleUtterances.addAll(suggestions);
		
		// qualities aanpassen based on history
		adaptQualityToHistory(possibleUtterances);
		
		// Find best sentences
		ArrayList<AgentSpokenUtterance> bestSentences = new ArrayList<AgentSpokenUtterance>();
		double bestValue = -1d;
		for( AgentSpokenUtterance uttr : possibleUtterances ) {
			if( uttr.getQuality() > bestValue ) {
				bestSentences.clear();
				bestSentences.add( uttr );
				bestValue = uttr.getQuality();
			} else if( uttr.getQuality() == bestValue ) {
				bestSentences.add( uttr );
			}
		}
		
		/* TEST print all options TEST */
		for( AgentSpokenUtterance uttr : bestSentences ) {
			System.out.println(uttr.getQuality() + "	- " + uttr.getUtterance().getUtterance() + "("+uttr.getUtterance().getCategory()+")");
		}
		
		// Pick one
		AgentSpokenUtterance response = bestSentences.get( rand.nextInt(bestSentences.size()) );
		
		/* Set corresponding parameters */
		if( response.getUtterance().getCategory().equals("tell_me_more") ) {
			tellMeMoreCounter++;
		}
		if( response.getUtterance().getCategory().equals("change_subject") ) {
			tellMeMoreCounter++;
			nrTopicChanges++;
			subjectIndex = utteranceHistory.size()-1;
		}
		if( response.getUtterance().getCategory().equals("ask_next_character") ) {
			tellMeMoreCounter++;
			nrTopicChanges = 0;
			charChangeState = CHAR_ASKED;
		}
		if( response.getUtterance().getCategory().equals("change_character") ) {
			tellMeMoreCounter++;
			charChangeState = CHAR_SUGGESTED;
			suggestedChar = response.getNewCharacter();
		}
		
		return response;
	}
	
	public ArrayList<AgentSpokenUtterance> suggestAfterSilence()
	{
		AgentSpokenUtterance lastUtterance = utteranceHistory.get( utteranceHistory.size()-1 );
		double quality = 1.0d;
		
		/* If the user didn't say anything, motivate him */
		if( userTurnStart == -1 ) {
			AgentUtterance elaborationUtterance = lastUtterance.getUtterance().getElaborationUtterance();
			if( elaborationUtterance != null ) {
				ArrayList<AgentSpokenUtterance> spokenUtterances = new ArrayList<AgentSpokenUtterance>();
				AgentSpokenUtterance uttr =  new AgentSpokenUtterance( elaborationUtterance );
				uttr.setQuality(quality);
				spokenUtterances.add(uttr);
				return spokenUtterances;
			}
			int r = rand.nextInt(2);
			if( r == 0 ) {
				return getUtterancesFromCategory("after_silence", quality);
			} else if( r == 1 ) {
				return getUtterancesFromCategory("change_subject", quality);
			}
		}
		return new ArrayList<AgentSpokenUtterance>();
	}
	
	public DialogueAct getCombinedUserDialogueAct()
	{
		DialogueAct act = null;
		System.out.print( "Combining: " );
		for( int i=dActIndex; i<detectedDActs.size(); i++ ) {
			System.out.print("("+detectedDActs.get(i).getUtterance() + ") ");
			if( act == null ) {
				if( i == dActIndex ) {
					act = detectedDActs.get(i);
				} else {
					// Something is wrong
					// TODO: report error
				}
			} else {
				act = new DialogueAct(act,detectedDActs.get(i));
			}
		}
		System.out.println("");
		return act;
	}
	
	public ArrayList<AgentSpokenUtterance> suggestLinkingSentence( DialogueAct act )
	{
		if( act == null ) {
			return new ArrayList<AgentSpokenUtterance>();
		}
		
		double quality = 0.7;
		AgentSpokenUtterance lastUtterance = utteranceHistory.get( utteranceHistory.size()-1 );
		ArrayList<String> features = new ArrayList<String>(Arrays.asList(act.getFeatures().trim().split(" ")));
		ArrayList<AgentUtterance> linkingUtterances = lastUtterance.getUtterance().getLinkingUtterances( features );
		
		ArrayList<AgentSpokenUtterance> linkingSpokenUtterances = new ArrayList<AgentSpokenUtterance>();
		for( AgentUtterance link : linkingUtterances ) {
			if( allUtterances.get(currChar).get(link.getUtterance()) != null ) {
				linkingSpokenUtterances.addAll( getUtterancesFromCategory(link.getUtterance(), quality) ) ;
				
			} else {
				AgentSpokenUtterance uttr = new AgentSpokenUtterance( link );
				uttr.setQuality(quality);
				linkingSpokenUtterances.add(uttr);
			}
		}
		return linkingSpokenUtterances;
	}
	
	public ArrayList<AgentSpokenUtterance> suggestOnContentFeatures( DialogueAct act )
	{
		if( act == null ) {
			return new ArrayList<AgentSpokenUtterance>();
		}
		
		double quality = 0.8;
		HashMap<String,Integer> scores = giveResponseRatings(act);
		AgentSpokenUtterance reaction;
		int maxReactions = 4;
		int firstScore = -1;
		int latestScore = -1;
		ArrayList<String> bestResponses = new ArrayList<String>();
		while( scores.keySet().size()>0 && ( bestResponses.size() < maxReactions || ( bestResponses.size() > maxReactions && latestScore == firstScore ) ) ) {
			reaction = getMaxUtterance( scores, "Content_Based" );
			if( reaction != null ) {
				String reactionUtterance = reaction.getUtterance().getUtterance();
				if( firstScore == -1 ) {
					firstScore = scores.get(reactionUtterance);
				}
				latestScore = scores.get(reactionUtterance);
				bestResponses.add(reaction.getUtterance().getUtterance());

				scores.remove(reactionUtterance);
			} else {
				break;
			}
		}
		ArrayList<AgentSpokenUtterance> spokenUtterances = new ArrayList<AgentSpokenUtterance>();
		if( firstScore > 0 ) {
			for( String utterance : bestResponses ) {
				if( allUtterances.get(currChar).get( utterance ) != null ) {
					spokenUtterances.addAll( getUtterancesFromCategory( utterance, quality) ) ;
				} 
				else {
					AgentSpokenUtterance uttr = new AgentSpokenUtterance( findAgentUtterance(utterance, "Content_Based") );
					uttr.setQuality( uttr.getQuality() / 10 * quality );
					spokenUtterances.add(uttr);
				}
			}
		}
		return spokenUtterances;
	}
	
	public ArrayList<AgentSpokenUtterance> suggestOnArousal()
	{
		double quality = 0.4;
		/* Determine high and low arousal indicators and user utterance length */
		int high_intensity_arousal = 0;
		int low_intensity_arousal = 0;

		for( int i=emotionIndex; i< detectedEmotions.size(); i++ ) {
			EmotionEvent ee = detectedEmotions.get(i);
			if( ee.getType() == EmotionEvent.AROUSAL ) {
				if( ee.getIntensity() > HIGH_AROUSAL ) {
					high_intensity_arousal++;
				} else if( ee.getIntensity() < LOW_AROUSAL ) {
					low_intensity_arousal++;
				}
			}
		}

		ArrayList<AgentSpokenUtterance> suggestions = new ArrayList<AgentSpokenUtterance>();
		if( high_intensity_arousal-low_intensity_arousal > 0 || (high_intensity_arousal > 0 && low_intensity_arousal == high_intensity_arousal) ) {
			/* If there are high arousal indicators and there are more than low arousal indicators
			 * respond with a 'high arousal utterance' */
			suggestions.addAll( getUtterancesFromCategory( "high_arousal", quality ) );
		} else if( low_intensity_arousal-high_intensity_arousal > 0 ) {
			/* If there are low arousal indicators and there are more than high arousal indicators
			 * respond with a 'low arousal utterance' */
			suggestions.addAll( getUtterancesFromCategory( "low_arousal", quality ) );
		}
		return suggestions;
	}
	
	public ArrayList<AgentSpokenUtterance> suggestLastOptions()
	{
		double quality = 0.1;
		ArrayList<AgentSpokenUtterance> suggestions = new ArrayList<AgentSpokenUtterance>();
		
		long user_utterance_length = meta.getTime() - userTurnStart;
		
		if( tellMeMoreCounter >= 2 ) {
			/* If the number of 'tell me more' utterances is greater than 2
			 * change the subject or change the character */
			if( nrTopicChanges < 4 ) {
				return getUtterancesFromCategory("change_subject", quality);
			} else {
				if( rand.nextBoolean() ) {
					// Ask for the character
					return getUtterancesFromCategory("ask_next_character", quality);
				} else {
					// Suggest a character
					int suggestedChar = rand.nextInt(4)+1;
					while( suggestedChar == currChar ) {
						suggestedChar = rand.nextInt(4)+1;
					}
					AgentSpokenUtterance suggestion =  new AgentSpokenUtterance( new AgentUtterance("Do you want to talk to " + charNames.get(suggestedChar) + "?","change_character") );
					suggestion.setNewCharacter(suggestedChar);
					suggestions.add(suggestion);
				}
			}
		} else if( user_utterance_length < SMALL_UTTERANCE ) {
			/* If the user utterance is smaller than a predefined threshold
			 * respond with a 'tell me more utterance' */
			return getUtterancesFromCategory( "tell_me_more", quality);
		} else {
			/* If no utterance can be determined pick one of the random utterances */
			return getUtterancesFromCategory( "random", quality);
		}
		
		return suggestions;
	}

	/**
	 * Determines what to say based on the context
	 * Currently, the context means: detected arousal, length of the user utterance,
	 * and the history of agent utterances.
	 * @return the AgentUtterance to speak next
	 */
	public AgentSpokenUtterance getResponse()
	{
		AgentSpokenUtterance lastUtterance = utteranceHistory.get( utteranceHistory.size()-1 );
		
		/* If the user didn't say anything, motivate him */
		if( userTurnStart == -1 ) {
			AgentUtterance elaborationUtterance = lastUtterance.getUtterance().getElaborationUtterance();
			if( elaborationUtterance != null ) {
				return new AgentSpokenUtterance( elaborationUtterance );
			}
			int r = rand.nextInt(2);
			if( r == 0 ) {
				return pickUtterances("after_silence");
			} else if( r == 1 ) {
				return pickUtterances("change_subject");
			}
		}

		/* Determine high and low arousal indicators and user utterance length */
		int high_intensity_arousal = 0;
		int low_intensity_arousal = 0;
		long user_utterance_length = meta.getTime() - userTurnStart;

		for( int i=emotionIndex; i< detectedEmotions.size(); i++ ) {
			EmotionEvent ee = detectedEmotions.get(i);
			if( ee.getType() == EmotionEvent.AROUSAL ) {
				if( ee.getIntensity() > HIGH_AROUSAL ) {
					high_intensity_arousal++;
				} else if( ee.getIntensity() < LOW_AROUSAL ) {
					low_intensity_arousal++;
				}
			}
		}

		/* Combine Dialogue Acts */
		DialogueAct act = null;
		System.out.print( "Combining: " );
		for( int i=dActIndex; i<detectedDActs.size(); i++ ) {
			System.out.print("("+detectedDActs.get(i).getUtterance() + ") ");
			if( act == null ) {
				if( i == dActIndex ) {
					act = detectedDActs.get(i);
				} else {
					// Something is wrong
					// TODO: report error
				}
			} else {
				act = new DialogueAct(act,detectedDActs.get(i));
			}
		}
		System.out.println("");
		
		System.out.println(act.toString());
		
		/* Determine if a linking sentence can be used */
		ArrayList<String> features = new ArrayList<String>(Arrays.asList(act.getFeatures().trim().split(" ")));
		ArrayList<AgentUtterance> linkingUtterances = lastUtterance.getUtterance().getLinkingUtterances( features );
		if( linkingUtterances.size() > 0 ) {
			return new AgentSpokenUtterance( linkingUtterances.get( rand.nextInt(linkingUtterances.size()) ) );
		}
		
		/* Determine a response based on content-features */
		if( act != null ) {
			HashMap<String,Integer> scores = giveResponseRatings(act);
			String reaction;
			int maxReactions = 4;
			int firstScore = -1;
			int latestScore = -1;
			ArrayList<String> bestResponses = new ArrayList<String>();
			while( scores.keySet().size()>0 && ( bestResponses.size() < maxReactions || ( bestResponses.size() > maxReactions && latestScore == firstScore ) ) ) {
				reaction = getMaxScorer( scores );
				if( reaction != null ) {
					if( firstScore == -1 ) {
						firstScore = scores.get(reaction);
					}
					latestScore = scores.get(reaction);
					bestResponses.add(reaction);

					scores.remove(reaction);
				} else {
					break;
				}
			}
			if( firstScore > 0 ) {
				HashMap<String,ArrayList<AgentUtterance>> utterancesChar = allUtterances.get(currChar);
				if( utterancesChar.keySet().contains(bestResponses.get(0)) ) {
					return pickUtterances( bestResponses.get(0) );
				} else {
					return new AgentSpokenUtterance( new AgentUtterance(bestResponses.get(0), "Content_Based") );
				}
				// TODO: Fixen dat er geen dubbele zinnen achter elkaar komen.
			}
		}

		/* Determine the number of 'tell me more' utterances in this subject */
		int tellMeMoreCounter = 0;
		for( int i=subjectIndex; i<utteranceHistory.size(); i++ ) {
			AgentSpokenUtterance spokenUtterance = utteranceHistory.get(i);
			if( spokenUtterance.getUtterance().getCategory().equals("tell_me_more") ) {
				tellMeMoreCounter++;
			}
		}

		if( high_intensity_arousal-low_intensity_arousal > 0 || (high_intensity_arousal > 0 && low_intensity_arousal == high_intensity_arousal) ) {
			/* If there are high arousal indicators and there are more than low arousal indicators
			 * respond with a 'high arousal utterance' */
			AgentSpokenUtterance uttr = pickUtterances( "high_arousal" ) ;
			if( uttr != null ) {
				return uttr;
			}
		} else if( low_intensity_arousal-high_intensity_arousal > 0 ) {
			/* If there are low arousal indicators and there are more than high arousal indicators
			 * respond with a 'low arousal utterance' */
			AgentSpokenUtterance uttr = pickUtterances( "low_arousal" ) ;
			if( uttr != null ) {
				return uttr;
			}
		}

		if( tellMeMoreCounter >= 2 ) {
			/* If the number of 'tell me more' utterances is greater than 2
			 * change the subject or change the character */
			tellMeMoreCounter = 0;
			if( nrTopicChanges < 4 ) {
				subjectIndex = utteranceHistory.size()-1;
				nrTopicChanges++;
				return pickUtterances("change_subject");
			} else {
				nrTopicChanges = 0;
				if( rand.nextBoolean() ) {
					// Ask for the character
					charChangeState = CHAR_ASKED;
					return pickUtterances("ask_next_character");
				} else {
					// Suggest a character
					charChangeState = CHAR_SUGGESTED;
					suggestedChar = rand.nextInt(4)+1;
					while( suggestedChar == currChar ) {
						suggestedChar = rand.nextInt(4)+1;
					}
					return new AgentSpokenUtterance( new AgentUtterance("Do you want to talk to " + charNames.get(suggestedChar) + "?","change_character") );
				}
			}
		} else if( user_utterance_length < SMALL_UTTERANCE ) {
			/* If the user utterance is smaller than a predefined threshold
			 * respond with a 'tell me more utterance' */
			AgentSpokenUtterance uttr = pickUtterances( "tell_me_more" ) ;
			if( uttr != null ) {
				return uttr;
			}
		} else {
			/* If no utterance can be determined pick one of the random utterances */
			AgentSpokenUtterance uttr = pickUtterances( "random" ) ;
			if( uttr != null ) {
				return uttr;
			}
		}
		return null;
	}
	
	/**
	 * Based on the given type of sentence this method tries to find an utterance of that type
	 * that hasn't been said for the last x agent utterances.
	 * @param type - the type of the utterance
	 * @return - the AgentUtterance which includes the utterance type and the utterance itself
	 */
	public ArrayList<AgentSpokenUtterance> getUtterancesFromCategory( String category, double quality )
	{
		/* Get all utterances of the given type */
		HashMap<String,ArrayList<AgentUtterance>> utterancesChar = allUtterances.get(currChar);
		ArrayList<AgentSpokenUtterance> spokenUtterances = convertToSpokenUtterances( utterancesChar.get(category) );
		for( AgentSpokenUtterance uttr : spokenUtterances ) {
			uttr.setQuality(quality);
		}
		return spokenUtterances;
	}
	
	public ArrayList<AgentSpokenUtterance> convertToSpokenUtterances( ArrayList<AgentUtterance> uttrs )
	{
		ArrayList<AgentSpokenUtterance> spokenUtterances = new ArrayList<AgentSpokenUtterance>();
		if( uttrs == null ) {
			return spokenUtterances;
		}
		for( AgentUtterance uttr : uttrs ) {
			spokenUtterances.add( new AgentSpokenUtterance( uttr ) );
		}
		return spokenUtterances;
	}
	
	public void adaptQualityToHistory( ArrayList<AgentSpokenUtterance> utterances )
	{
		HashMap<String,Integer> utteranceDistances = new HashMap<String,Integer>();
		int size = utteranceHistory.size();
		for( int i=0; i<utteranceHistory.size(); i++ ) {
			AgentSpokenUtterance uttr = utteranceHistory.get(i);
			utteranceDistances.put(uttr.getUtterance().getUtterance(),(size-i) );
		}
		
		for( AgentSpokenUtterance uttr : utterances ) {
			Integer distance = utteranceDistances.get(uttr.getUtterance().getUtterance());
			if( distance == null ) {
				distance = 0;
			}
			if( distance >= 1 && distance <= 5 ) {
				uttr.setQuality( uttr.getQuality() - 0.8d );
			}
			else if( distance >= 6 && distance <= 10 ) {
				uttr.setQuality( uttr.getQuality() - 0.4d );
			}
			else if( distance > 10 ) {
				uttr.setQuality( uttr.getQuality() - 0.2d );
			}
		}
	}

	/**
	 * Based on the given type of sentence this method tries to find an utterance of that type
	 * that hasn't been said for the last x agent utterances.
	 * @param type - the type of the utterance
	 * @return - the AgentUtterance which includes the utterance type and the utterance itself
	 */
	public AgentSpokenUtterance pickUtterances( String type )
	{
		/* Get all utterances of the given type that haven't been used for the last x utterances */
		HashMap<String,ArrayList<AgentUtterance>> utterancesChar = allUtterances.get(currChar);
		ArrayList<AgentUtterance> utterancesType = utteranceCopy( utterancesChar.get(type) );

		ArrayList<AgentUtterance> utteranceToRemove = new ArrayList<AgentUtterance>();
		for( int i=utteranceHistory.size()-1; i>=(Math.max(0, utteranceHistory.size()-3)); i-- ) {
			AgentSpokenUtterance spUttr = utteranceHistory.get(i);
			for( AgentUtterance uttr : utterancesType ) {
				if( uttr.getUtterance().equals(spUttr.getUtterance()) ) {
					utteranceToRemove.add(uttr);
				}
			}
			for( AgentUtterance uttr : utteranceToRemove ) {
				utterancesType.remove( uttr );
			}
		}

		// TODO: Random kiezen tussen change_subject en tell_me_more
		if( utterancesType.size() == 0 ) {
			/* If the list is empty do something else */
			/*if( !type.equals("ask_next_character") && !type.equals("repair_ask_next_character") && !type.equals("suggest_next_character")&& !type.equals("repair_suggest_next_character") ) {
				charChangeState = CHAR_ASKED;
				System.out.println("Changing " + type + " to ask_next_character");
				return pickUtterances("ask_next_character");
			} else {
				charChangeState = NEUTRAL;
				subjectIndex = utteranceHistory.size()-1;
				System.out.println("Changing "+type+" to change_subject");
				return pickUtterances("change_subject");
			}*/
			if( rand.nextBoolean() ) {
				charChangeState = NEUTRAL;
				System.out.println("Changing "+type+" to tell_me_more");
				return pickUtterances("tell_me_more");
			} else {
				charChangeState = NEUTRAL;
				subjectIndex = utteranceHistory.size()-1;
				System.out.println("Changing "+type+" to change_subject");
				return pickUtterances("change_subject");
			}
		} else {
			/* If the list isn't empty randomly pick an utterance from the list */
			return new AgentSpokenUtterance( utterancesType.get(rand.nextInt(utterancesType.size())) );
		}
	}
	
	public AgentUtterance findAgentUtterance( String utterance, String newType )
	{
		HashMap<String,ArrayList<AgentUtterance>> utterancesChar = allUtterances.get(currChar);
		for( ArrayList<AgentUtterance> utterances : utterancesChar.values() ) {
			for( AgentUtterance au : utterances ) {
				if( au.getUtterance().equals(utterance) ) {
					return au;
				}
			}
		}
		return new AgentUtterance( utterance, newType );
	}

	/**
	 * Called when the output module messages that the utterance is finished.
	 * Will put the agent state on listening again and send this state around.
	 * @throws JMSException
	 */
	public void processUtteranceEnd() throws JMSException
	{	
		if( (agentSpeakingState == SPEAKING || agentSpeakingState == PREPARING_TO_SPEAK) && meta.getTime() > utteranceEndTime ) {
			System.out.println("Agent silent (timeout)");
			DMLogger.getLogger().log(meta.getTime(), "AgentAction:UtteranceStopped" );
			agentSpeakingState = LISTENING;
			agentSpeakingStateTime = meta.getTime();
			sendListening();
			userTurnStart = -1;
		}
	}

	/**
	 * Sends the given utterance to the output modules.
	 * 
	 * @param utterance
	 * @throws JMSException
	 */
	public void sendUtterance( AgentSpokenUtterance utterance ) throws JMSException
	{	
		/* Send utterance to Greta */
		String response = utterance.getUtterance().getUtterance();

		String id = "s1";

		Document doc = XMLTool.newDocument("fml-apml", null, FML.version);
		Element root = doc.getDocumentElement();

		Element bml = XMLTool.appendChildElement(root, BML.E_BML, BML.namespaceURI);
		bml.setAttribute(BML.A_ID, "bml1");
		Element fml = XMLTool.appendChildElement(root, FML.E_FML, FML.namespaceURI);
		fml.setAttribute(FML.A_ID, "fml1");
		Element speech = XMLTool.appendChildElement(bml, BML.E_SPEECH);
		speech.setAttribute(BML.A_ID, id);
		speech.setAttribute(BML.E_TEXT, response);
		speech.setAttribute(BML.E_LANGUAGE, "en-GB");
		speech.setAttribute("voice", "activemary");

		//speech.setTextContent(response);

		int counter=1;
		for( String word : response.split(" ") ) {
			Element mark = XMLTool.appendChildElement(speech, SSML.E_MARK, SSML.namespaceURI);
			mark.setAttribute(SSML.A_NAME, id+":tm"+counter);
			Node text = doc.createTextNode(word);
			speech.appendChild(text);
			counter++;
		}
		Element mark = XMLTool.appendChildElement(speech, SSML.E_MARK, SSML.namespaceURI);
		mark.setAttribute(SSML.A_NAME, id+":tm"+counter);

		fmlSender.sendXML(doc, meta.getTime());

		/* Send the speaking-state around */
		sendSpeaking();

		/* Set indices */
		emotionIndex = detectedEmotions.size();
		dActIndex = detectedDActs.size();

		/* Add the utterance to the history */
		utterance.setTime( meta.getTime() );
		utteranceHistory.add( utterance );


		System.out.println("Agent speaking");
		/* Set end time (temporary) */
		utteranceEndTime = meta.getTime() + ( (utterance.getUtterance().getUtterance().split(" ").length * 250)+8000 );

		DMLogger.getLogger().log(meta.getTime(), "AgentAction:SendUtterance, type=" + utterance.getUtterance().getCategory() + ", utterance=" + utterance.getUtterance().getUtterance() );
	}

	/**
	 * Sends around that the agent is speaking
	 * @throws JMSException
	 */
	public void sendSpeaking() throws JMSException
	{
		Map<String,String> dialogInfo = new HashMap<String,String>();		
		dialogInfo.put("agentTurnState", "true");

		DialogStateInfo dsi = new DialogStateInfo(dialogInfo, null);
		dialogStateSender.sendStateInfo(dsi, meta.getTime());
	}

	/**
	 * Sends around that the agent is silent
	 * @throws JMSException
	 */
	public void sendListening() throws JMSException
	{
		Map<String,String> dialogInfo = new HashMap<String,String>();		
		dialogInfo.put("agentTurnState", "false");

		DialogStateInfo dsi = new DialogStateInfo(dialogInfo, null);
		dialogStateSender.sendStateInfo(dsi, meta.getTime());
	}

	/**
	 * Sends around that the system has changed to a new character
	 * @param character the new character
	 * @throws JMSException
	 */
	public void sendNewCharacter( int character ) throws JMSException
	{
		Map<String,String> contextInfo = new HashMap<String,String>();
		contextInfo.put("character", charNames.get(character));

		ContextStateInfo csi = new ContextStateInfo(contextInfo);
		contextSender.sendStateInfo( csi, meta.getTime() );
	}

	/**
	 * Reads the received Message and tries to filter out the detected user speaking state.
	 * @param m - the received message
	 */
	public void setUserSpeakingState(StateInfo userInfo)
	{
		if( userInfo.hasInfo("speaking") ) {
			if( userInfo.getInfo("speaking").equals("true") ) {
				if( userSpeakingState != SPEAKING ) {
					userSpeakingState = SPEAKING;
					userSpeakingStateTime = meta.getTime();
					if( userTurnStart == -1 ) {
						userTurnStart = userSpeakingStateTime;
					}
				}
			} else if( userInfo.getInfo("speaking").equals("false") ) {
				if( userSpeakingState != LISTENING ) {
					userSpeakingState = LISTENING;
					userSpeakingStateTime = meta.getTime();
				}
			}
		}
	}

	/**
	 * Reads the received Message and tries to filter out the detected Emotion Events.
	 * @param m
	 */
	public void addDetectedEmotions(StateInfo userInfo)
	{
		//Map<String,String> dialogInfoMap = userInfo.getInfos();

		if( userInfo.hasInfo("valence") ) {
			float valence = Float.parseFloat( userInfo.getInfo("valence") );
			EmotionEvent ee = new EmotionEvent( meta.getTime(), 0, EmotionEvent.VALENCE, valence );
			detectedEmotions.add( ee );
		} else if( userInfo.hasInfo("arousal") ) {
			float arousal = Float.parseFloat( userInfo.getInfo("arousal") );
			EmotionEvent ee = new EmotionEvent( meta.getTime(), 0, EmotionEvent.AROUSAL, arousal );
			detectedEmotions.add( ee );
		} else if( userInfo.hasInfo("interest") ) {
			float interest = Float.parseFloat( userInfo.getInfo("interest") );
			EmotionEvent ee = new EmotionEvent( meta.getTime(), 0, EmotionEvent.INTEREST, interest );
			detectedEmotions.add( ee );
		}
	}

	/**
	 * Reads the received Message and tries to filter out the detected Dialogue Acts.
	 * @param m
	 * @throws JMSException
	 */
	public void addDetectedDActs( StateInfo stateInfo ) throws JMSException
	{
		if( stateInfo.hasInfo("userUtterance") && stateInfo.hasInfo("userUtteranceStartTime") ) {
			String utterance = stateInfo.getInfo("userUtterance");
			long time = Long.parseLong(stateInfo.getInfo("userUtteranceStartTime"));
			String f = stateInfo.getInfo("userUtteranceFeatures");
			if( f == null ) {
				f = "";
			}
			DialogueAct act = new DialogueAct( utterance, time );
			if( f.contains("positive") ) act.setPositive(true);
			if( f.contains("negative") ) act.setNegative(true);
			if( f.contains("disagree") ) {
				act.setDisagree(true);
				f = f.replace("disagree", "");
			}
			if( f.contains("agree") ) act.setAgree(true);
			if( f.contains("about_other_people") ) act.setAboutOtherPeople(true);
			if( f.contains("about_other_character") ) act.setAboutOtherCharacter(true);
			if( f.contains("about_current_character") ) act.setAboutCurrentCharacter(true);
			if( f.contains("about_own_feelings") ) act.setAboutOwnFeelings(true);
			if( f.contains("pragmatic") ) act.setPragmatic(true);
			if( f.contains("abou_self") ) act.setTalkAboutSelf(true);
			if( f.contains("future") ) act.setFuture(true);
			if( f.contains("past") ) act.setPast(true);
			if( f.contains("event") ) act.setEvent(true);
			if( f.contains("action") ) act.setAction(true);
			if( f.contains("laugh") ) act.setLaugh(true);
			if( f.contains("change_speaker") ) act.setChangeSpeaker(true);
			if( f.contains("target:") ) act.setTargetCharacter( f.substring(f.indexOf("target:")+7, Math.max(f.length(),f.indexOf(" ", f.indexOf("target:")+7))) );
			detectedDActs.add(act);
		}
	}

	/**
	 * Returns a map with all possible responses plus the ratings of those
	 * responses (based on the response model)
	 * @param act
	 * @return
	 */
	public HashMap<String,Integer> giveResponseRatings( DialogueAct act )
	{
		ArrayList<Integer> props = new ArrayList<Integer>();
		HashMap<String,Integer> scores = new HashMap<String, Integer>();

		if( currChar == POPPY ) {
			props.add( 1 );
		}else if( currChar == OBADIAH ) {
			props.add( 2 );
		} else if( currChar == PRUDENCE ) {
			props.add( 3 );
		} else if( currChar == SPIKE ) {
			props.add( 4 );
		}

		if( act.isPositive() ) props.add( 8 );
		if( act.isNegative() ) props.add( 9 );
		if( act.isAgree() ) props.add( 10 );
		if( act.isDisagree() ) props.add( 11 );
		if( act.isAboutOtherPeople() ) props.add( 12 );
		if( act.isAboutOtherCharacter() ) props.add( 13 );
		if( act.isAboutCurrentCharacter() ) props.add( 14 );
		if( act.isAboutOwnFeelings() ) props.add( 15 );
		if( act.isPragmatic() ) props.add( 16 );
		if( act.isTalkAboutSelf() ) props.add( 17 );
		if( act.isPast() && act.isEvent() ) props.add( 18 );
		if( act.isFuture() && act.isEvent() ) props.add( 19 );
		if( act.isPast() && act.isAction() ) props.add( 20 );
		if( act.isFuture() && act.isAction() ) props.add( 21 );
		if( act.isLaugh() ) props.add( 22 );

		int score;
		for( ContextTemplate template : templates ) {

			ArrayList<Integer> required = template.getRequiredFeatures();
			ArrayList<Integer> optional = template.getExtraFeatures();
			score = 10;
			String reaction = template.getReaction();
			for( int feature : props ) {
				if( !required.contains(feature) && !optional.contains(feature) ) {
					score -= 2;
				}
			}
			for( int feature : required ) {
				if( !props.contains(feature) ) {
					score -= 10;
				}
			}
			for( int feature : optional ) {
				if( !props.contains(feature) ) {
					score -= 1;
				}
			}

			if( ! scores.containsKey(reaction) || scores.get(reaction) < score  ) {
				scores.put(reaction, score);
			}
		}
		return scores;
	}

	/**
	 * Returns the best possible utterance from the given map with responses and scores
	 * Only returns utterances with a score higher than 0.
	 * When the highest score <= 0 is will return null.
	 * @param scores
	 * @return
	 */
	public String getMaxScorer( HashMap<String, Integer> scores )
	{
		String maxString = "";
		int maxScore = -99;
		for( Map.Entry<String,Integer> entry : scores.entrySet() ) {
			if( entry.getValue() > maxScore ) {
				maxString = entry.getKey();
				maxScore = scores.get(entry.getKey());
			}
		}
		if( maxScore <= 0 ) {
			return null;
		} else {
			return maxString;
		}
	}
	
	public AgentSpokenUtterance getMaxUtterance( HashMap<String, Integer> scores, String type )
	{
		String maxString = "";
		int maxScore = -99;
		for( Map.Entry<String,Integer> entry : scores.entrySet() ) {
			if( entry.getValue() > maxScore ) {
				maxString = entry.getKey();
				maxScore = scores.get(entry.getKey());
			}
		}
		if( maxScore <= 0 ) {
			return null;
		} else {
			AgentSpokenUtterance maxUttr = new AgentSpokenUtterance( findAgentUtterance(maxString, type) );
			maxUttr.setQuality(maxScore);
			return maxUttr;
		}
	}

	/**
	 * Makes a deepcopy of the given ArrayList
	 * @param utterances - the list to copy
	 * @return
	 */
	public ArrayList<AgentUtterance> utteranceCopy( ArrayList<AgentUtterance> utterances )
	{
		if( utterances == null ) {
			return new ArrayList<AgentUtterance>();
		}
		ArrayList<AgentUtterance> newUtterances = new ArrayList<AgentUtterance>();
		for( AgentUtterance uttr : utterances ) {
			newUtterances.add( uttr );
		}
		return newUtterances;
	}
}
