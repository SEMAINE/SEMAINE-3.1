/**
 * Copyright (C) 2008 University of Twente, HMI. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.dialogue.actionproposers;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Random;

import javax.jms.JMSException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

import eu.semaine.components.Component;
import eu.semaine.components.dialogue.util.DialogueAct;
import eu.semaine.jms.message.SEMAINEEmmaMessage;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.receiver.EmmaReceiver;
import eu.semaine.jms.receiver.UserStateReceiver;
import eu.semaine.jms.sender.FMLSender;
import eu.semaine.datatypes.xml.BML;
import eu.semaine.datatypes.xml.FML;
import eu.semaine.datatypes.xml.SSML;
import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.util.XMLTool;

/**
 * This class waits for the Agent to have the turn, takes the analysis of the user input
 * and chooses a response for the agent
 * 
 * @author Mark tM
 * @version 0.1 - dummy class
 *
 */

public class UtteranceProposer extends Component 
{
	/* A representation of the four characters */
	private enum Personality {Poppy, Obadiah, Spike, Prudence}
	
	/* The minimum number of sentences before a sentence can be repeated */
	private static final int MIN_REPEAT_RATE = 10;
	
	/* The files with the response-model */
	private String contextFile = "/eu/semaine/components/dialogue/data/Context.csv";
	private String groupFile = "/eu/semaine/components/dialogue/data/SentenceGroups.xml";
	
	/* The current personality */
	private Personality currPersonality;
	
	/* The number of responses the current personality has uttered so far */
	private int personalityConversationLength = 0;
	
	/* The number of sentences since a new topic was introduced by SAL */
	private int newTopicSentenceCount = 0;
	
	/* The personality the user wants to talk to next */
	private Personality nextPersonality = null;
	
	/* A history of all personalities if they have talked or not */
	private HashMap<Personality, Boolean> personalityHistory = new HashMap<Personality, Boolean>();
	
	/*
	 * The conversation state
	 * 0 - Neutral state
	 * 1 - Start of character, name asked
	 * 2 - Character has started and asked for a name
	 * 7 - Character has started and asked 'how are you'
	 * 3 - System decision change of speaker made
	 * 4 - System decision change of speaker made and a character will be suggested
	 * 5 - User has desire to talk to another character, a character is asked
	 * 6 - User has desire to talk to another character, a character is suggested
	 */
	private int convState = 1;
	
	/* For random data */
	private Random rand = new Random();
	
	/* A list with all context-templates, used for picking a response */
	private ArrayList<ContextTemplate> templates ;
	
	/* A map with all response-groups plus its corresponding utterances */
	private HashMap<String,ArrayList<String>> groupData;
	
	/* The response-history */
	private ArrayList<String> pastResponses = new ArrayList<String>();
	
	/* A list with sentences which can be uttered always */
	private ArrayList<String> safeResponses = new ArrayList<String>();
	
	/* Receivers and Senders */
	private UserStateReceiver userStateReceiver;
	private EmmaReceiver emmaReceiver;
	private FMLSender fmlSender;
	
	
	/**
	 * Creates a new UtteranceProposer
	 * + Creates the Senders and Receivers
	 * + Import response-model data
	 * + Sets a random starting personality
	 * @throws JMSException
	 */
	public UtteranceProposer() throws JMSException
	{
		super("UtteranceProposer");
		
		/* Creates the Senders and Receivers */
		emmaReceiver = new EmmaReceiver("semaine.data.state.user", "datatype = 'EMMA'");
		receivers.add(emmaReceiver);
		userStateReceiver = new UserStateReceiver("semaine.data.state.user", "datatype = 'UserState'");
		receivers.add(userStateReceiver);
		
		fmlSender = new FMLSender("semaine.data.action.candidate.function", getName());
		senders.add(fmlSender); // so it can be started etc
		
		/* Import response-model data */
		
		DataImporter importer = new DataImporter( contextFile, groupFile );
		templates = importer.importContextData();
		groupData = importer.importGroupData();
		
		/* Initialize some lists */
		
		safeResponses.add("*Group* Start new subject");
		safeResponses.add("*Group* Tell me more");
		
		personalityHistory.put( Personality.Poppy, false );
		personalityHistory.put( Personality.Obadiah, false );
		personalityHistory.put( Personality.Spike, false );
		personalityHistory.put( Personality.Prudence, false );
		
		/* Set a random starting personality */
		
		currPersonality = choosePersonality();
	}
	
	/**
	 * Starts the conversation if this is called for the first time
	 */
	@Override
	protected void act( ) throws JMSException
	{
		if( pastResponses.size() == 0 && startOfConversation() ) {
			personalityConversationLength = 0;
			return;
		}
	}
	
	/**
	 * Called when a new message arrives
	 * Checks whether the message is an EmmaMessage containing processed input.
	 * If so, then it will use this input to pick a response
	 */
	@Override
	protected void react(SEMAINEMessage m) throws JMSException
	{
		if( m instanceof SEMAINEEmmaMessage ) {
			SEMAINEEmmaMessage em = (SEMAINEEmmaMessage)m;
			Element interpretation = em.getTopLevelInterpretation();
			if ( interpretation != null && interpretation.getAttribute("processed").equals("true") ) {
				List<Element> texts = em.getTextElements(interpretation);
				DialogueAct act = null;
				for (Element text : texts) {
					String utterance = text.getTextContent();
					if( utterance != null ) {
						act = new DialogueAct(utterance);
					}
				}
				if( act != null ) {
					List<Element> features = em.getFeatureElements( interpretation );
					for( Element feature : features ) {
						String f = feature.getAttribute( "name" );
						if( f.equals("positive") ) act.setPositive(true);
						if( f.equals("negative") ) act.setNegative(true);
						if( f.equals("agree") ) act.setAgree(true);
						if( f.equals("disagree") ) act.setDisagree(true);
						if( f.equals("about other people") ) act.setAboutOtherPeople(true);
						if( f.equals("about other character") ) act.setAboutOtherCharacter(true);
						if( f.equals("about current character") ) act.setAboutCurrentCharacter(true);
						if( f.equals("about own feelings") ) act.setAboutOwnFeelings(true);
						if( f.equals("pragmatic") ) act.setPragmatic(true);
						if( f.equals("about self") ) act.setTalkAboutSelf(true);
						if( f.equals("future") ) act.setFuture(true);
						if( f.equals("past") ) act.setPast(true);
						if( f.equals("event") ) act.setEvent(true);
						if( f.equals("action") ) act.setAction(true);
						if( f.equals("laugh") ) act.setLaugh(true);
						if( f.equals("change speaker") ) act.setChangeSpeaker(true);
						if( f.equals("target character") ) act.setTargetCharacter( feature.getAttribute("target") );
					}
					System.out.println("Choosing response");
					pickResponse(act);
				}
			}
		}
	}
	
	/**
	 * Tries to pick a response based on the given DialogueAct (which contains
	 * of the detected utterance + detected features)
	 * @param act
	 * @throws JMSException
	 */
	public void pickResponse( DialogueAct act ) throws JMSException
	{
		if( changeOfCharacter(act) ) {
			return;
		}
		
		if( startOfConversation() ) {
			personalityConversationLength = 0;
			return;
		}
		personalityConversationLength++;
		
		HashMap<String,Integer> scores = giveResponseRatings(act);
		
		String reaction;
		int maxReactions = 4;
		int firstScore = -1;
		int latestScore = -1;
		ArrayList<String> bestResponses = new ArrayList<String>();
		while( scores.keySet().size()>0 && ( bestResponses.size() < maxReactions || ( bestResponses.size() > maxReactions && latestScore == firstScore ) ) ) {
			reaction = getMaxScorer( scores );
			if( firstScore == -1 ) {
				firstScore = scores.get(reaction);
			}
			if( ! pastResponses.contains(reaction) ) {
				latestScore = scores.get(reaction);
				bestResponses.add(reaction);
			}
			System.out.println("- "+scores.get(reaction) + " - " + reaction);
			scores.remove(reaction);
			
		}
		
		String response = bestResponses.get( rand.nextInt( bestResponses.size() ) );
		
		if( firstScore <= 0 ) {
			response = safeResponses.get( rand.nextInt(safeResponses.size()) );
			while( pastResponses.contains(response) || (newTopicSentenceCount < 3 && response.equals("*Group* Start new subject")) ) {
				response = safeResponses.get( rand.nextInt(safeResponses.size()) );
			}
		}
		if( groupData.containsKey(currPersonality+":"+response) ) {
			ArrayList<String> options = groupData.get(currPersonality+":"+response);
			response = options.get( rand.nextInt(options.size()) );
			while( pastResponses.contains(response) ) {
				response = options.get( rand.nextInt(options.size()) );
			}
		}
		
		while( pastResponses.size() > MIN_REPEAT_RATE ) {
			pastResponses.remove(0);
		}
		newTopicSentenceCount++;
		respond(response);
	}
	
	/**
	 * Regulates the start of a conversation (done at the start of each
	 * character)
	 * @return
	 * @throws JMSException
	 */
	public boolean startOfConversation(  ) throws JMSException
	{
		if( convState == 1 ) {
			String response = "";
			if( currPersonality == Personality.Poppy ) {
				if( personalityHistory.get( Personality.Poppy ) ) {
					// Character has talked before
					response = "Hi! I'm Poppy. How are you?";
					convState = 0;
				} else {
					// Character hasn't talked before
					response = "Hi, I'm Poppy, the eternal optimist. What's your name? ";
					convState = 2;
				}
			} else if( currPersonality == Personality.Obadiah ) {
				if( personalityHistory.get( Personality.Obadiah ) ) {
					// Character has talked before
					response = "Hello, I'm Obidiah. What's the matter?";
					convState = 0;
				} else {
					// Character hasn't talked before
					response = "Hello, my name is Obadiah, and I'm feeling very depressed right now. And who are you?";
					convState = 2;
				}
			} else if( currPersonality == Personality.Spike ) {
				if( personalityHistory.get( Personality.Spike ) ) {
					// Character has talked before
					response = "Well I'm Spike, what's your problem?";
					convState = 0;
				} else {
					// Character hasn't talked before
					response = "I'm Spike and I've a bit of a temper. Who are you?";
					convState = 2;
				}
			} else if( currPersonality == Personality.Prudence ) {
				if( personalityHistory.get( Personality.Prudence ) ) {
					// Character has talked before
					response = "Hello, it's Prudence. What would you like to talk about?";
					convState = 0;
				} else {
					// Character hasn't talked before
					response = "Hello there, I'm Prudence and I'm very matter of fact. So tell me, what is your name?";
					convState = 2;
				}
			}
			respond(response);
			return true;
		}
		
		if( convState == 2 ) {
			String response = "";
			if( currPersonality == Personality.Poppy ) {
				response = "Ok, and how are you today?";
				convState = 7;
			} else if( currPersonality == Personality.Obadiah ) {
				response = "Ah. So how are you feeling today? Not great I suppose.";
				convState = 7;
			} else if( currPersonality == Personality.Spike ) {
				response = "And what state are you in today?";
				convState = 7;
			} else if( currPersonality == Personality.Prudence ) {
				response = "I see. And how are you today?";
				convState = 7;
			}
			respond(response);
			return true;
		}
		
		
		
		if( convState == 7 ) {
			String response = "";
			if( currPersonality == Personality.Poppy ) {
				response = "Is that so ' tell me about it";
				convState = 0;
			} else if( currPersonality == Personality.Obadiah ) {
				response = "Is that so - Tell me more about it ";
				convState = 0;
			} else if( currPersonality == Personality.Spike ) {
				response = "Is that so - Tell me more about it ";
				convState = 0;
			} else if( currPersonality == Personality.Prudence ) {
				response = "Is that so - Tell me more about it ";
				convState = 0;
			}
			respond(response);
			return true;
		}
		return false;
	}
	
	/**
	 * Regulates the process of changing the character
	 * @param act
	 * @return
	 * @throws JMSException
	 */
	public boolean changeOfCharacter( DialogueAct act ) throws JMSException
	{
		String response = null;
		if( convState == 0 && systemWantsSpeakerChange() ) {
			if( rand.nextBoolean() ) {
				response = "Would you like to talk to someone else?";
				convState = 3;
			} else {
				nextPersonality = choosePersonality();
				response = "Why don't you talk to " + nextPersonality + "?";
				convState = 4;
			}
		} else if( convState == 0 && act.isChangeSpeaker() && act.getTargetCharacter() == null ) {
			if( rand.nextBoolean() ) {
				response = "Who would you like to talk to?";
				convState = 5;
			} else {
				nextPersonality = choosePersonality();
				response = "Why don't you talk to " + nextPersonality + "?";
				convState = 6;
			}
		} else if( convState == 0 && act.isChangeSpeaker() && act.getTargetCharacter() != null ) {
			response = null;
			currPersonality = getPersonality( act.getTargetCharacter() );
			convState = 1;
			return false;
		} else if( convState == 3 && act.isDisagree() ) {
			convState = 0;
		} else if( convState == 3 && act.isAgree() ) {
			if( act.getTargetCharacter() == null ) {
				if( rand.nextBoolean() ) {
					response = "Who would you like to talk to?";
					convState = 5;
				} else {
					nextPersonality = choosePersonality();
					response = "Why don't you talk to " + nextPersonality + "?";
					convState = 6;
				}
			} else {
				response = null;
				currPersonality = getPersonality( act.getTargetCharacter() );
				convState = 1;
				return false;
			}
		} else if( convState == 3 ) {
			response = "I'm sorry, I didn't understand that.";
		}else if( convState == 4 && act.isDisagree() ) {
			response = "Would you like to talk to someone else?";
			nextPersonality = null;
			convState = 3;
		} else if( convState == 4 && act.isAgree() ) {
			response = null;
			currPersonality = nextPersonality;
			nextPersonality = null;
			convState = 1;
			return false;
		} else if( convState == 4 ) {
			response = "I'm sorry, I didn't understand that.";
		} else if( convState == 5 ) {
			if( act.getTargetCharacter() != null ) {
				response = null;
				currPersonality = getPersonality( act.getTargetCharacter() );
				convState = 1;
				return false;
			} else {
				response = "I'm sorry, who do you want to talk to?";
			}
		} else if( convState == 6 ) {
			if( act.isDisagree() ) {
				if( act.getTargetCharacter() != null ) {
					response = null;
					currPersonality = getPersonality( act.getTargetCharacter() );
					nextPersonality = null;
					convState = 1;
					return false;
				} else {
					response = "Then who would you like to talk to?";
					nextPersonality = null;
					convState = 5;
				}
			} else if( act.isAgree() ) {
				response = null;
				currPersonality = nextPersonality;
				convState = 1;
				return false;
			} else {
				response = "I'm sorry, I didn't understand that.";
			}
		}
		if( response != null ) {
			respond(response);
			return true;
		} else {
			return false;
		}
	}
	
	/**
	 * @return true if the System decides to change the speaker
	 * TODO: implement	
	 */
	public boolean systemWantsSpeakerChange()
	{
		if( personalityConversationLength >= 10 ) {
			if( rand.nextInt(3) == 0 ) {
				return true;
			}
		}
		return false;
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

		if( currPersonality == Personality.Poppy ) {
			props.add( 1 );
		}else if( currPersonality == Personality.Obadiah ) {
			props.add( 2 );
		} else if( currPersonality == Personality.Prudence ) {
			props.add( 3 );
		} else if( currPersonality == Personality.Spike ) {
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
		return maxString;
	}
	
	/**
	 * Returns a random personality
	 * @return
	 */
	public Personality choosePersonality()
	{
		Personality p = currPersonality;
		while( p == currPersonality ) {
			switch( rand.nextInt(4) ) {
			case 0:
				p = Personality.Poppy;
				break;
			case 1:
				p = Personality.Obadiah;
				break;
			case 2:
				p = Personality.Spike;
				break;
			case 3:
				p = Personality.Prudence;
				break;
			}
		}
		return p;
	}
	
	/**
	 * Returns the Personality based on the String representation of the character
	 * @param p
	 * @return
	 */
	public Personality getPersonality( String p ) {
		p = p.toLowerCase();
		if( p.equals("poppy") ) {
			return Personality.Poppy;
		} else if( p.equals("obadiah") ) {
			return Personality.Obadiah;
		} else if( p.equals("spike") ) {
			return Personality.Spike;
		} else if( p.equals("prudence") ) {
			return Personality.Prudence;
		} else {
			return null;
		}
	}
	
	/**
	 * Sends the chosen response to the FML-channel
	 * @param response
	 * @throws JMSException
	 */
	public void respond( String response ) throws JMSException
	{
		String id = "s1";
		pastResponses.add(response);
		
		Document doc = XMLTool.newDocument("fml-apml", null, FML.version);
		Element root = doc.getDocumentElement();

		Element bml = XMLTool.appendChildElement(root, BML.E_BML, BML.namespaceURI);
		bml.setAttribute(BML.A_ID, "bml1");
		Element fml = XMLTool.appendChildElement(root, FML.E_FML, FML.namespaceURI);
		fml.setAttribute(FML.A_ID, "fml1");
		Element speech = XMLTool.appendChildElement(bml, BML.E_SPEECH);
		speech.setAttribute(BML.A_ID, id);
		speech.setAttribute(BML.E_TEXT, response);
		speech.setAttribute(BML.E_LANGUAGE, "en_US");

		//speech.setTextContent(response);
		
		int counter=1;
		for( String word : response.split(" ") ) {
			Element mark = XMLTool.appendChildElement(speech, SSML.E_MARK, SSML.namespaceURI);
			mark.setAttribute(SSML.A_NAME, id+":tm"+counter);
			Node text = doc.createTextNode(word);
			speech.appendChild(text);
			counter++;
		}
		Element mark = XMLTool.appendChildElement(speech, SSML.E_MARK);
		mark.setAttribute(SSML.A_NAME, id+":tm"+counter);
		
		
		fmlSender.sendXML(doc, meta.getTime());
	}
}
