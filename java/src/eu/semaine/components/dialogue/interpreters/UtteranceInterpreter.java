/**
 * Copyright (C) 2008 University of Twente, HMI. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */

package eu.semaine.components.dialogue.interpreters;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.zip.GZIPInputStream;

import javax.jms.JMSException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

import eu.semaine.components.Component;
import eu.semaine.components.dialogue.datastructures.DialogueAct;
import eu.semaine.components.dialogue.test.DMLogger;
import eu.semaine.datatypes.stateinfo.DialogStateInfo;
import eu.semaine.datatypes.stateinfo.StateInfo;
import eu.semaine.datatypes.stateinfo.UserStateInfo;
import eu.semaine.datatypes.xml.EMMA;
import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.jms.message.SEMAINEStateMessage;
import eu.semaine.jms.message.SEMAINEEmmaMessage;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.receiver.StateReceiver;
import eu.semaine.jms.receiver.EmmaReceiver;
import eu.semaine.jms.sender.StateSender;
import eu.semaine.jms.sender.XMLSender;
import eu.semaine.util.XMLTool;

import opennlp.maxent.io.BinaryGISModelReader;
import opennlp.tools.postag.DefaultPOSContextGenerator;
import opennlp.tools.postag.POSDictionary;
import opennlp.tools.postag.POSTaggerME;

/**
 * This class interprets the dialog and extends it with detected semantic features
 * 
 * @author Mark tM
 * @version 0.1 - dummy class
 *
 */

public class UtteranceInterpreter extends Component 
{	
	public static long SHORT_LENGTH = 5000;
	public static long LONG_LENGTH = 15000;
	
	/* Receivers */
	private EmmaReceiver emmaReceiver;
	private StateReceiver userStateReceiver;
	//private AgentStateReceiver agentStateReceiver;
	private StateReceiver dialogStateReceiver;

	/* Senders */
	private StateSender userStateSender;

	/* PoS Tagger */
	private POSTaggerME tagger;

	/* Dialog variables */
	String utterance = "";
	private String[] negWords = { "blah", "tired", "shouldn't", "tireder", "ill", "aggressive", "bloody", "horrible", "stressful", "insulted", "nerves", "stress", "hell", "terribly", "gloomy", "nasty", "git", "stupid", "pointless", "rude", "rubbish", "sick", "useless", "fool", "manic", "ridiculous", "miserable", "misunderstood", "grim", "angry", "bad", "loser", "shit", "wrong", "sad", "terrible" };
	private ArrayList<String> negativeWords = new ArrayList<String>( Arrays.asList(negWords) );

	private String[] posWords = { "good", "nice", "happy", "lovely", "(laugh)", "amazing", "great", "bright", "wonderful", "fine", "okay", "best", "thank", "smiling", "right" };
	private ArrayList<String> positiveWords = new ArrayList<String>( Arrays.asList(posWords) );

	private String[] emotionalWords = { "happy", "sad", "going mad", "tired", "very well", "pleased", "cheerful", "upbeat", "angry", "good mood", "afraid", "fed up", "wind me up", "mood", "furious", "good frame of mind", "frustrated", "unhappy", "energetic", "better", "comfortable", "cross" };

	/**
	 * Constructor DialogInterpreter
	 */
	public UtteranceInterpreter() throws JMSException, IOException
	{
		super("UtteranceInterpreter");

		emmaReceiver = new EmmaReceiver("semaine.data.state.user.emma.words");
		receivers.add(emmaReceiver);
		userStateReceiver = new StateReceiver( "semaine.data.state.user.behaviour", StateInfo.Type.UserState );
		receivers.add(userStateReceiver);

		dialogStateReceiver = new StateReceiver("semaine.data.state.dialog", StateInfo.Type.DialogState);
		receivers.add(dialogStateReceiver);

		/* Define Senders */
		userStateSender = new StateSender("semaine.data.state.user.behaviour", StateInfo.Type.UserState, getName());
		senders.add(userStateSender);

		/* Initializing PoS Tagger */
		try {
			String modelFile = "/eu/semaine/components/dialogue/data/tag.bin.gz";
			DataInputStream s = new DataInputStream(new BufferedInputStream(new GZIPInputStream(new BufferedInputStream(this.getClass().getResourceAsStream(modelFile)))));
			//String tagdict = "java/lib/opennlp-tools-1.4.1/models/tagdict";
			BufferedReader reader = new BufferedReader(new InputStreamReader(this.getClass().getResourceAsStream("/eu/semaine/components/dialogue/data/tagdict")));


			if( modelFile != null&& reader != null ) {
				tagger = new POSTaggerME( new BinaryGISModelReader(s).getModel(), new DefaultPOSContextGenerator(null),new POSDictionary(reader,true) );
			}
		} catch( FileNotFoundException e ) {
			e.printStackTrace();
		}
	}

	/**
	 * Overrides the act()-method of Component, does nothing currently
	 */
	@Override
	public void act() throws JMSException
	{

	}

	/** * Called when a new message is received * Checks if the message contains 
	an input-sentence. * Also checks if the user is finished speaking */ 
	@Override public void react( SEMAINEMessage m ) throws JMSException
	{

		if( m instanceof SEMAINEEmmaMessage ) {
			SEMAINEEmmaMessage em = (SEMAINEEmmaMessage)m;
			
			/* Reading words */
			//System.out.println(em.getText());
			Element wordSequence = em.getSequence();
			if( wordSequence != null ) {
				//System.out.println("WordSequence found");
				List<Element> wordElements = XMLTool.getChildrenByLocalNameNS(wordSequence, EMMA.E_INTERPRETATION, EMMA.namespaceURI);
				if( wordElements.size() > 0 ) {
					//System.out.println("WordElements found");
					String detectedKeywords = "";
					String starttime = null;
					String detectedTimes = "";
					String confidences = "";
					for( Element wordElem : wordElements ) {
						if( wordElem.hasAttribute(EMMA.A_TOKENS) ) {
							detectedKeywords = detectedKeywords + wordElem.getAttribute(EMMA.A_TOKENS) + " ";
						}
						if( wordElem.hasAttribute(EMMA.A_OFFSET_TO_START) ) {
							detectedTimes = detectedTimes + wordElem.getAttribute(EMMA.A_OFFSET_TO_START) + " ";
						}
						if( wordElem.hasAttribute(EMMA.A_CONFIDENCE) ) {
							confidences = confidences + wordElem.getAttribute(EMMA.A_CONFIDENCE) + " ";
						}
						if( starttime == null && wordSequence.hasAttribute(EMMA.A_OFFSET_TO_START) ) {
							starttime = wordSequence.getAttribute(EMMA.A_OFFSET_TO_START);
						}
					}
					detectedKeywords = detectedKeywords.replaceAll("<s>", "").replaceAll("</s>", "").trim();
					detectedTimes = detectedTimes.trim();
					if(starttime != null) {
						DMLogger.getLogger().logWords( Long.parseLong( starttime ), detectedKeywords, detectedTimes, confidences );
						DialogueAct act = analyseData( detectedKeywords, Long.parseLong( starttime ) );
						sendDialogueAct( act );
					}
				}
			}
		}
		
		if( m instanceof SEMAINEStateMessage ) {
			SEMAINEStateMessage sm = ((SEMAINEStateMessage)m);
			StateInfo stateInfo = sm.getState();
			StateInfo.Type stateInfoType = stateInfo.getType();
			
			switch (stateInfoType) {
			case UserState:
				processHeadMovements( stateInfo );
				break;
			}
		}
	}
	
	public void processHeadMovements( StateInfo stateInfo ) throws JMSException
	{
		if( stateInfo.hasInfo("headGesture") && stateInfo.hasInfo("headGestureStarted") && stateInfo.hasInfo("headGestureStopped") ) {
			if( stateInfo.getInfo("headGesture").equals("NOD") ) {
				DialogueAct act = new DialogueAct("*nod*", ((long)(Double.parseDouble( stateInfo.getInfo("headGestureStarted"))*1000)) );
				act.setAgree(true);
				sendDialogueAct(act);
			} else if( stateInfo.getInfo("headGesture").equals("SHAKE") ) {
				DialogueAct act = new DialogueAct("*shake*", ((long)(Double.parseDouble( stateInfo.getInfo("headGestureStarted"))*1000)) );
				act.setDisagree(true);
				sendDialogueAct(act);
			}
		}
	}	
	
	public void sendDialogueAct( DialogueAct act ) throws JMSException
	{
		if( act.getUtterance().length() < 1 ) {
			return;
		}
		Map<String,String> userStateInfo = new HashMap<String,String>();
		userStateInfo.put("userUtterance", act.getUtterance());
		userStateInfo.put("userUtteranceStartTime", ""+act.getStarttime());
		userStateInfo.put("userUtteranceFeatures", act.getFeatures());
		//System.out.println("Detected the following utterance (t:" + act.getStarttime() + "): " + act.getUtterance() + ". ("+act.getFeatures()+")");
		
		UserStateInfo usi = new UserStateInfo(userStateInfo	);
		userStateSender.sendStateInfo(usi, meta.getTime());	
	}

	/**
	 * Creates an EmmaMessage with the utterance and all detected features
	 * @param act
	 * @throws JMSException
	 */
	public void sendDialogueActOld( DialogueAct act ) throws JMSException
	{
		//System.out.println("Analysis: " + act.toString());
//		Document document = XMLTool.newDocument(EMMA.E_EMMA, EMMA.namespaceURI, EMMA.version);
//		Element interpretation = XMLTool.appendChildElement(document.getDocumentElement(), EMMA.E_INTERPRETATION);
//		interpretation.setAttribute(EMMA.A_START, String.valueOf(meta.getTime()));
//		interpretation.setAttribute( "processed", "true" );
//		Element text = XMLTool.appendChildElement(interpretation, SemaineML.E_TEXT, SemaineML.namespaceURI);
//		text.setTextContent( act.getUtterance() );

		Document document = XMLTool.newDocument(SemaineML.E_USERSTATE, SemaineML.namespaceURI, SemaineML.version);
		Element text = XMLTool.appendChildElement(document.getDocumentElement(), SemaineML.E_TEXT);
		text.setAttribute(SemaineML.A_TIME, String.valueOf(act.getStarttime()));
		Node textNode = document.createTextNode(act.getUtterance());
		text.appendChild(textNode);

		if( act.isPositive() ) {
			Element feature = XMLTool.appendChildElement(text, "feature", SemaineML.namespaceURI);
			feature.setAttribute("name", "positive");
		}
		if( act.isNegative() ) {
			Element feature = XMLTool.appendChildElement(text, "feature", SemaineML.namespaceURI);
			feature.setAttribute("name", "negative");
		}
		if( act.isAgree() ) {
			Element feature = XMLTool.appendChildElement(text, "feature", SemaineML.namespaceURI);
			feature.setAttribute("name", "agree");
		}
		if( act.isDisagree() ) {
			Element feature = XMLTool.appendChildElement(text, "feature", SemaineML.namespaceURI);
			feature.setAttribute("name", "disagree");
		}
		if( act.isAboutOtherPeople() ) {
			Element feature = XMLTool.appendChildElement(text, "feature", SemaineML.namespaceURI);
			feature.setAttribute("name", "about other people");
		}
		if( act.isAboutOtherCharacter() ) {
			Element feature = XMLTool.appendChildElement(text, "feature", SemaineML.namespaceURI);
			feature.setAttribute("name", "about other character");
		}
		if( act.isAboutCurrentCharacter() ) {
			Element feature = XMLTool.appendChildElement(text, "feature", SemaineML.namespaceURI);
			feature.setAttribute("name", "about current character");
		}
		if( act.isAboutOwnFeelings() ) {
			Element feature = XMLTool.appendChildElement(text, "feature", SemaineML.namespaceURI);
			feature.setAttribute("name", "about own feelings");
		}
		if( act.isPragmatic() ) {
			Element feature = XMLTool.appendChildElement(text, "feature", SemaineML.namespaceURI);
			feature.setAttribute("name", "pragmatic");
		}
		if( act.isTalkAboutSelf() ) {
			Element feature = XMLTool.appendChildElement(text, "feature", SemaineML.namespaceURI);
			feature.setAttribute("name", "about self");
		}
		if( act.isFuture() ) {
			Element feature = XMLTool.appendChildElement(text, "feature", SemaineML.namespaceURI);
			feature.setAttribute("name", "future");
		}
		if( act.isPast() ) {
			Element feature = XMLTool.appendChildElement(text, "feature", SemaineML.namespaceURI);
			feature.setAttribute("name", "past");
		}
		if( act.isEvent() ) {
			Element feature = XMLTool.appendChildElement(text, "feature", SemaineML.namespaceURI);
			feature.setAttribute("name", "event");
		}
		if( act.isAction() ) {
			Element feature = XMLTool.appendChildElement(text, "feature", SemaineML.namespaceURI);
			feature.setAttribute("name", "action");
		}
		if( act.isLaugh() ) {
			Element feature = XMLTool.appendChildElement(text, "feature", SemaineML.namespaceURI);
			feature.setAttribute("name", "laugh");
		}
		if( act.isChangeSpeaker() ) {
			Element feature = XMLTool.appendChildElement(text, "feature", SemaineML.namespaceURI);
			feature.setAttribute("name", "change speaker");
		}
		if( act.getTargetCharacter() != null ) {
			Element feature = XMLTool.appendChildElement(text, "feature", SemaineML.namespaceURI);
			feature.setAttribute("name", "target character");
			feature.setAttribute("target", act.getTargetCharacter());
		}

		try {
			userStateSender.sendXML(document, meta.getTime());
		}catch( JMSException e ){}
	}

	/**
	 * Analyses the given data (the utterance) for features
	 * @param data
	 * @return
	 */
	public DialogueAct analyseData( String data, long starttime )
	{
		if( data == null ) {
			data = "";
		}
		DialogueAct act = new DialogueAct( data, starttime );
		String taggedUtterance = getTaggedUtterance( data );
		String[] tags = getTags( taggedUtterance );

		act.setTaggedUtterance( taggedUtterance );
		act.setTags( tags );

		int polarity = detectPolarity( act );
		if( polarity == 0 ) {
			act.setNegative(true);
		} else if( polarity == 2 ) {
			act.setPositive(true);
		}

		act.setAgree( detectAgree( act ) );
		act.setDisagree( detectDisagree( act ) );
		act.setAboutOtherPeople( detectAboutOtherPeople( act ) );
		act.setAboutOtherCharacter( detectAboutOtherCharacter( act ) );
		act.setAboutCurrentCharacter( detectAboutCurrentCharacter( act ) );

		act.setAboutOwnFeelings( detectAboutOwnFeelings( act ) );
		act.setPragmatic( detectPragmatic( act ) );
		act.setTalkAboutSelf( detectTalkAboutSelf( act ) );
		act.setFuture( detectFuture( act ) );
		act.setPast( detectPast( act ) );
		act.setEvent( detectEvent( act ) );
		act.setAction( detectAction( act ) );
		act.setChangeSpeaker( detectChangeSpeaker( act ) );
		act.setTargetCharacter( detectTargetCharacter( act ) );
		act.setLength( detectLength( act ) );

		return act;
	}

	/**
	 * Detects the polarity in the given DialogueAct
	 * @param act
	 * @return
	 */
	public int detectPolarity( DialogueAct act )
	{
		String utterance = act.getUtterance();
		int posCount = getNumberOfPosWords( utterance );
		int negCount = getNumberOfNegWords( utterance );

		if( posCount > negCount && ((utterance.length() < 50 && posCount >= 1) || (utterance.length() < 100 && posCount >= 2) || (posCount >= 3)) ) {
			return 2;
		} else {
			if( negCount > posCount && ((utterance.length() < 50 && negCount >= 1) || (utterance.length() < 100 && negCount >= 2) || (negCount >= 3)) ) {
				return 0;
			} else {
				return 1;
			}
		}
	}

	/**
	 * Detects agreement in the given DialogueAct
	 * @param act
	 * @return
	 */
	public boolean detectAgree( DialogueAct act )
	{
		String utterance = act.getUtterance();
		String pattern0 = ".*(((^| )yes)|((^| )yeah)|((^| )true)|((^| )right)|((^| )okay)).*";
		String replacePattern0 = "(((^| )yes)|((^| )yeah)|((^| )true)|((^| )right)|((^| )okay))";

		int count = 0;

		while( utterance.matches(pattern0) ) {
			count+=1;
			utterance = utterance.replaceFirst(replacePattern0, " ");
		}

		if( utterance.length() >= 3 && utterance.substring(0, 3).toLowerCase().equals("yes") ) {
			count++;
		}
		if( utterance.length() >= 4 && (utterance.substring(0, 4).toLowerCase().equals("yeah") ||
				utterance.substring(0, 4).toLowerCase().equals("true") ||
				utterance.substring(0, 4).toLowerCase().equals("okay")) ) {
			count++;
		}
		if( utterance.length() >= 5 && utterance.substring(0, 5).toLowerCase().equals("right") ) {
			count++;
		}

		if( count >= 1 ) {
			return true;
		} else {
			return false;
		}
	}

	/**
	 * Detects disagreement in the given DialogueAct
	 * @param act
	 * @return
	 */
	public boolean detectDisagree( DialogueAct act )
	{	
		String utterance = act.getUtterance();
		String pattern0 = ".*(^| )no( |,|\\.|!).*";
		String replacePattern0 = "(^| )no( |,|\\.|!)";

		int count = 0;

		String part = utterance.substring(0, Math.min(utterance.length(), 50));
		while( part.matches(pattern0) ) {
			count+=1;
			part = part.replaceFirst(replacePattern0, " ");
		}
		if( utterance.length() >= 2 && utterance.substring(0, 2).toLowerCase().equals("no") ) {
			count++;
		}

		if( count >= 1 ) {
			return true;
		} else {
			return false;
		}
	}

	/**
	 * Detects the 'aboutOtherPeople'-feature in the given DialogueAct
	 * @param act
	 * @return
	 */
	public boolean detectAboutOtherPeople( DialogueAct act )
	{
		String utterance = act.getUtterance();
		String pattern0 = ".* people .*";
		String replacePattern0 = " people ";
		String pattern1 = ".*(( they )|( they[''])|( their )|( them )).*";
		String replacePattern1 = "(( they )|( they[''])|( their )|( them ))";

		String negPattern0 = ".*(( i )|( i[''] ) ).*";
		String replaceNegPattern0 = "(( i )|( i[''] ) )";

		int count = 0;
		int negCount = 0;

		while( utterance.matches(pattern0) ) {
			count+=3;
			utterance = utterance.replaceFirst(replacePattern0, " ");
		}
		while( utterance.matches(pattern1) ) {
			count+=2;
			utterance = utterance.replaceFirst(replacePattern1, " ");
		}
		while( utterance.matches(negPattern0) ) {
			negCount+=2;
			utterance = utterance.replaceFirst(replaceNegPattern0, " ");
		}
		count -= Math.floor( (double)negCount/3d );

		if( count >= 4 ) {
			return true;
		} else {
			return false;
		}
	}

	/**
	 * Detects the 'aboutOtherCharacter'-feature in the given DialogueAct
	 * Not implemented yet
	 * @param act
	 * @return
	 */
	public boolean detectAboutOtherCharacter( DialogueAct act )
	{

		return false;
	}

	/**
	 * Detects the 'aboutCurrentCharacter'-feature in the given DialogueAct
	 * @param act
	 * @return
	 */
	public boolean detectAboutCurrentCharacter( DialogueAct act )
	{
		String utterance = act.getUtterance();
		String pattern0 = ".*(( you )|( you['']re )).*";
		String replacePattern0 = "(( you )|( you['']re ))";

		String negPattern0 = ".*you know.*";
		String replaceNegPattern0 = "you know";

		int count = 0;

		while( utterance.matches(negPattern0) ) {
			utterance = utterance.replaceFirst(replaceNegPattern0, " ");
		}
		while( utterance.matches(pattern0) ) {
			count+=1;
			utterance = utterance.replaceFirst(replacePattern0, " ");
		}

		if( count >= 2 ) {
			return true;
		} else {
			return false;
		}
	}

	/**
	 * Detects the 'aboutOwnFeelings'-feature in the given DialogueAct
	 * @param act
	 * @return
	 */
	public boolean detectAboutOwnFeelings( DialogueAct act )
	{
		/* Regex Patterns */
		String emotionalWordsString = "";
		for( String w : emotionalWords ) {
			emotionalWordsString = emotionalWordsString + w + "|";
		}
		emotionalWordsString = emotionalWordsString.substring(0, emotionalWordsString.length()-1);

		String pattern1 = ".*(\\s|^)i((['']m)| ((\\S)+ +){0,3}(am|was|get)) +((\\S)+ +){0,6}("+emotionalWordsString+").*";
		String pattern2 = ".*(\\s|^)i ((\\S)+ +){0,3}feel.*";
		String pattern3 = ".*(\\s|^)i ((\\S)+ +){0,3}felt.*";
		String pattern4 = ".*(\\s|^)i ((\\S)+ +){0,5}feeling.*";
		String pattern5 = ".*(\\s|^)me ((\\S)+ +){0,5}("+emotionalWordsString+").*";

		String utterance = act.getUtterance();

		if( utterance.matches(pattern1) || utterance.matches(pattern2) ||
				utterance.matches(pattern3) || utterance.matches(pattern4) ||
				utterance.matches(pattern5)) {
			return true;
		}
		return false;
	}

	/**
	 * Detects the 'pragmatic'-feature in the given DialogueAct
	 * @param act
	 * @return
	 */
	public boolean detectPragmatic( DialogueAct act )
	{
		String utterance = act.getUtterance();
		String words = "(sensible|pragmatic|reasonable|rational|practical|rationale|realistically|balance|objectively)";
		String pattern1 = ".*"+words+".*";
		String replacePattern1 = words;
		String pattern2 = ".*((not )|(n['']t )|( appearing )|( she)|( she[''])|( he)|( he[''])|( them ))((\\S)+ +){0,4}"+words+".*";
		String replacePattern2 = "((not )|(n['']t )|( you )|( you[''])|( appearing )|( she)|( she[''])|( he)|( he[''])|( them ))((\\S)+ +){0,4}"+words+"";

		int count = 0;

		while( utterance.matches(pattern2) ) {
			utterance = utterance.replaceFirst(replacePattern2, " ");
		}

		while( utterance.matches(pattern1) ) {
			count+=1;
			utterance = utterance.replaceFirst(replacePattern1, " ");
		}

		if( count >= 1 ) {
			return true;
		} else {
			return false;
		}
	}

	/**
	 * Detects the 'talkAboutSelf'-feature in the given DialogueAct
	 * @param act
	 * @return
	 */
	public boolean detectTalkAboutSelf( DialogueAct act )
	{
		String utterance = act.getUtterance();

		String pattern0 = ".*(^| )i'(m|ve) .*";
		String replacePattern0 = "(^| )i'(m|ve) ";
		String pattern1 = ".*(^| )i(( )|(')|(')).*";
		String replacePattern1 = "(^| )i(( )|(')|('))";
		String pattern2 = ".* am .*";
		String replacePattern2 = " am ";
		String pattern3 = ".* my .*";
		String replacePattern3 = " my ";
		String pattern4 = ".* me .*";
		String replacePattern4 = " me ";
		String pattern5 = ".* have .*";
		String replacePattern5 = " have ";

		int count = 0;

		while( utterance.matches(pattern0) ) {
			count+=4;
			utterance = utterance.replaceFirst(replacePattern0, " ");
		}
		while( utterance.matches(pattern1) ) {
			count+=3;
			utterance = utterance.replaceFirst(replacePattern1, " ");
		}
		while( utterance.matches(pattern2) ) {
			count+=1;
			utterance = utterance.replaceFirst(replacePattern2, " ");
		}
		while( utterance.matches(pattern3) ) {
			count+=1;
			utterance = utterance.replaceFirst(replacePattern3, " ");
		}
		while( utterance.matches(pattern4) ) {
			count+=1;
			utterance = utterance.replaceFirst(replacePattern4, " ");
		}
		while( utterance.matches(pattern5) ) {
			count+=1;
			utterance = utterance.replaceFirst(replacePattern5, " ");
		}

		if( count >= 5 || (act.getUtterance().length() < 100 && count >=4) ) {
			return true;
		} else {
			return false;
		}
	}

	/**
	 * Detects talking about the future in the given DialogueAct
	 * @param act
	 * @return
	 */
	public boolean detectFuture( DialogueAct act )
	{
		/* Regex Patterns */
		String pattern1 = ".* going to .*";
		String replacePattern1 = " going to ";
		String pattern2 = ".*(( will )|('ll )|('ll)).*";
		String replacePattern2 = "(( will )|('ll )|('ll))";
		String pattern3 = ".* gonna .*";
		String replacePattern3 = " gonna ";
		String pattern4 = ".* i ((\\S)+ +){0,3}need ((\\S)+ +){0,3}to .*";
		String replacePattern4 = " i ((\\S)+ +){0,3}need ((\\S)+ +){0,3}to ";
		String pattern5 = ".* is (\\S)+ing .*";
		String replacePattern5 = " is (\\S)+ing ";
		String pattern6 = ".*(( tomorrow )|( next week )|( planning )|( coming )( i've got to )|( coming up )).*";
		String replacePattern6 = "(( tomorrow )|( next week )|( planning )|( coming )( i've got to )|( coming up ))";

		String utterance = act.getUtterance();
		int score = 0;
		int countScore = 0;
		if( utterance.matches(pattern1) ) {
			score++;
			while( utterance.matches(pattern1) ) {
				countScore++;
				utterance = utterance.replaceFirst(replacePattern1, " ");
			}
		}
		if( utterance.matches(pattern2) ) {
			score++;
			while( utterance.matches(pattern2) ) {
				countScore++;
				utterance = utterance.replaceFirst(replacePattern2, " ");
			}
		}
		if( utterance.matches(pattern3) ) {
			score++;
			while( utterance.matches(pattern3) ) {
				countScore++;
				utterance = utterance.replaceFirst(replacePattern3, " ");
			}
		}
		if( utterance.matches(pattern4) ) {
			score++;
			while( utterance.matches(pattern4) ) {
				countScore++;
				utterance = utterance.replaceFirst(replacePattern4, " ");
			}
		}
		if( utterance.matches(pattern5) ) {
			score++;
			while( utterance.matches(pattern5) ) {
				countScore++;
				utterance = utterance.replaceFirst(replacePattern5, " ");
			}
		}
		if( utterance.matches(pattern6) ) {
			score++;
			while( utterance.matches(pattern6) ) {
				countScore++;
				utterance = utterance.replaceFirst(replacePattern6, " ");
			}
		}

		if( countScore >= 1 ) {
			return true;
		} else {
			return false;
		}
	}

	/**
	 * Detects talking about the past in the given DialogueAct
	 * @param act
	 * @return
	 */
	public boolean detectPast( DialogueAct act )
	{
		/* Regex Patterns */
		String pattern1 = ".*(( was )|( wasn't )).*";
		String pattern2 = ".* been .*";
		String pattern3 = ".*(( had )|( hadn't )|( hadn't )).*";
		String pattern4 = "..* went .*";
		String pattern5 = ".*(( were )|( weren't )|( weren't )).*";
		String pattern6 = ".*(( did )|( didn't )|( didn't )).*";

		String utterance = act.getUtterance();

		int score = 0;
		if( utterance.matches(pattern1) ) {
			score++;
		}
		if( utterance.matches(pattern2) ) {
			score++;
		}
		if( utterance.matches(pattern3) ) {
			score++;
		}
		if( utterance.matches(pattern4) ) {
			score++;
		}
		if( utterance.matches(pattern5) ) {
			score++;
		}
		if( utterance.matches(pattern6) ) {
			score++;
		}

		utterance = act.getUtterance();

		if( utterance.contains("\\?") ) {
			return false;
		}
		if( detectFuture(act) ) {
			return false;
		}
		// Wrong verbs: you know, 'cause, 
		String taggedUtterance = act.getTaggedUtterance();
		String[] tags = act.getTags();

		taggedUtterance = taggedUtterance.replaceAll("have/VBP been/VBN" , "been/VBN");
		taggedUtterance = taggedUtterance.replaceAll("has/VBZ been/VBN" , "been/VBN");

		int nrPastVerbs = 0;
		int nrNonPastVerbs = 0;

		for( String tag : tags ) {
			if( tag.equals("VB") || tag.equals("VBP") || tag.equals("VBZ") ) {
				nrNonPastVerbs++;
			} else if( tag.equals("VBD") || tag.equals("VBN") ) {
				nrPastVerbs++;
			}
		}
		if( nrPastVerbs >= 2 || nrPastVerbs > nrNonPastVerbs ) {
			return true;
		}

		String lastVerb = "";
		String oneToLastVerb = "";
		for( int i=tags.length-1; i>=0; i-- ) {
			String t = tags[i];
			if( lastVerb.equals("") && (t.equals("VB") || t.equals("VBD") || t.equals("VBN") || t.equals("VBP") || t.equals("VBZ")) ) {
				lastVerb = t;
			} else if( !lastVerb.equals("") && (t.equals("VB") || t.equals("VBD") || t.equals("VBN") || t.equals("VBP") || t.equals("VBZ")) ) {
				oneToLastVerb = t;
				break;
			}
		}

		if( (lastVerb.equals("VBD") || lastVerb.equals("VBN")) && (oneToLastVerb.equals("VBD") || oneToLastVerb.equals("VBN")) ) {
			return true;
		}
		return false;
	}

	/**
	 * Detects talking about an event in the given DialogueAct
	 * @param act
	 * @return
	 */
	public boolean detectEvent( DialogueAct act )
	{
		if( (detectPast(act) || detectFuture(act)) ){
			int count = 0;
			String utterance = act.getUtterance();

			String pattern1 = ".*(( he )|( he')|( she )|( she')|( they )|( they')|( them )|( people )|( spike)|( poppy)|( prudence)|( obadiah)).*";
			String replacePattern1 = "(( he )|( he')|( she )|( she')|( they )|( they')|( them )|( people )|( spike)|( poppy)|( prudence)|( obadiah))";

			if( !detectAction(act) ) {
				count += 1;
			}

			if( utterance.matches(pattern1) ) {
				while( utterance.matches(pattern1) ) {
					count+=1;
					utterance = utterance.replaceFirst(replacePattern1, " ");
				}
			}			

			if( count >= 2 ) {
				return true;
			}
		}
		return false;
	}

	/**
	 * Detects talking about an action in the given DialogueAct
	 * @param act
	 * @return
	 */
	public boolean detectAction( DialogueAct act )
	{
		if( detectPast(act) || detectFuture(act) ) {
			String utterance = act.getUtterance();
			String taggedUtterance = act.getTaggedUtterance();

			String pattern1 = ".*(( i )|( i')).*";
			String replacePattern1 = "(( i )|( i'))";

			String posPattern1 = ".* [iI]/PRP (\\S)+/VBP.*";
			String replacePosPattern1 = " [iI]/PRP (\\S)+/VBP";

			String antiPattern1 = ".*(( he )|( he')|( she )|( she')|( they )|( they')|( them )|( people )|( spike)|( poppy)|( prudence)|( obadiah)).*";
			String replaceAntiPattern1 = "(( he )|( he')|( she )|( she')|( they )|( they')|( them )|( people )|( spike)|( poppy)|( prudence)|( obadiah))";

			int count = 0;
			if( utterance.matches(pattern1) ) {
				while( utterance.matches(pattern1) ) {
					count++;
					utterance = utterance.replaceFirst(replacePattern1, " ");
				}
			}
			if( taggedUtterance.matches(posPattern1) ) {
				while( taggedUtterance.matches(posPattern1) ) {
					count+=3;
					taggedUtterance = taggedUtterance.replaceFirst(replacePosPattern1, " ");
				}
			}
			if( utterance.matches(antiPattern1) ) {
				while( utterance.matches(antiPattern1) ) {
					count--;
					utterance = utterance.replaceFirst(replaceAntiPattern1, " ");
				}
			}

			if( count>=3 ) {
				return true;
			}
		}
		return false;
	}

	/**
	 * Detects a request to change the speaker in the given DialogueAct
	 * @param act
	 * @return
	 */
	public boolean detectChangeSpeaker( DialogueAct act )
	{
		String utterance = act.getUtterance().toLowerCase();
		String pattern1 = ".*(speak|talk) to (someone|somebody) else.*";
		String pattern2 = ".*(switch|go( back)*) (to (someone|somebody) else|to another|to another character|character).*";
		String pattern3 = ".*talk to ((\\S)+ +){0,2}(spike|poppy|obadiah|prudence).*";
		String pattern4 = ".*speak to ((\\S)+ +){0,2}(spike|poppy|obadiah|prudence).*";
		String pattern5 = ".*switch to ((\\S)+ +){0,2}(spike|poppy|obadiah|prudence).*";
		String pattern6 = ".*(speak|talk) to.*";
		String pattern7 = ".*(speak|talk).*(spike|poppy|obadiah|prudence).*";
		if( utterance.matches(pattern1) || utterance.matches(pattern2) || utterance.matches(pattern3) || utterance.matches(pattern4) || utterance.matches(pattern5) || utterance.matches(pattern6) || utterance.matches(pattern7) ) {
			return true;
		} else {
			return false;
		}
	}

	/**
	 * Detects the target character in the given DialogueAct
	 * @param act
	 * @return
	 */
	public String detectTargetCharacter( DialogueAct act )
	{
		String utterance = act.getUtterance().toLowerCase();
		int i1 = utterance.lastIndexOf("poppy");
		int i2 = utterance.lastIndexOf("prudence");
		int i3 = utterance.lastIndexOf("spike");
		int i4 = utterance.lastIndexOf("obadiah");
		if( i1 == -1 && i2 == -1 && i3 == -1 && i4 == -1 ) {
			return null;
		}
		if( i1 > i2 && i1 > i3 && i1 > i4 ) {
			return "Poppy";
		} else if( i2 > i1 && i2 > i3 && i2 > i4 ) {
			return "Prudence";
		} else if( i3 > i1 && i3 > i2 && i3 > i4 ) {
			return "Spike";
		} else {
			return "Obadiah";
		}
	}
	
	public DialogueAct.Length detectLength( DialogueAct act )
	{	
		if( meta.getTime() - act.getStarttime() < SHORT_LENGTH ) {
			return DialogueAct.Length.SHORT;
		} else if( meta.getTime() - act.getStarttime() > LONG_LENGTH ) {
			return DialogueAct.Length.LONG;
		}
		return DialogueAct.Length.NORMAL;
	}

	/**
	 * Returns the utterance tagged with PoS-tags
	 * @param str
	 * @return
	 */
	public String getTaggedUtterance( String str )
	{
		/* Give reading signs extra space */
		str = str.replaceAll(",", " , ");
		str = str.replaceAll("\\.", " . ");
		str = str.replaceAll(";", " ; ");
		str = str.replaceAll(":", " : ");
		str = str.replaceAll("!", " ! ");
		str = str.replaceAll("\\?", " ? ");

		/* Write short-hand words out */
		str = str.replace("'", "'");
		str = str.replace("‘", "'");
		str = str.replace(" I'll ", " I will ");
		str = str.replaceAll("^I'll ", "I will ");
		str = str.replace(" i'll ", " I will ");
		str = str.replaceAll("^i'll ", "I will ");
		str = str.replace(" I'm ", " I am ");
		str = str.replaceAll("^I'm ", "I am ");
		str = str.replaceAll("I've", "I have");
		str = str.replaceAll("i've", "i have");
		str = str.replaceAll("we've", "we have");
		str = str.replaceAll(" don't ", " do not ");
		str = str.replace(" i'm ", " I am ");
		str = str.replaceAll("^i'm ", "I am ");
		str = str.replace(" i'd ", " I would ");
		str = str.replaceAll("^i'd ", "I would ");
		str = str.replace(" it's been ", " it has been ");
		str = str.replaceAll("^it's been ", "it has been ");
		str = str.replace(" it's ", " it is ");
		str = str.replaceAll("^it's ", "it is ");
		str = str.replace(" you're ", " you are ");
		str = str.replace("n't", " not");
		str = str.replaceAll("t's ", "t is ");
		str = str.replace(" i ", " I ");
		str = str.replaceAll("^i ", "I ");
		str = str.replaceAll(" i'",  " I'");

		String result = "";
		/*
		try {
			result =  MaxentTagger.tagString(str);
		} catch( Exception e ) {}
		 */

		result = tagger.tag(str);

		result = result.replaceAll("you/PRP +know/VBP", " " );
		result = result.replaceAll("'cause/VB", "'cause/IN");
		//result = result.replaceAll("I/PRP mean/VBP", "I/PRP mean/FW");
		result = result.replaceAll("to/TO \\S+/VB" , "to/TO verb/FW");
		result = result.replaceAll("do/VBP" , "do/FW");

		return result;
	}

	/**
	 * Returns an array with all tags found in the given tagged utterance
	 * @param taggedUtterance
	 * @return
	 */
	public String[] getTags( String taggedUtterance )
	{
		String[] tags = taggedUtterance.split(" ");
		for( int i=0; i<tags.length; i++ ) {
			String strTemp = tags[i];
			strTemp = strTemp.substring(strTemp.indexOf("/")+1);
			tags[i] = strTemp;
		}
		return tags;
	}

	/**
	 * Returns the number of positive words/phrases in the given string
	 * @param utteranceOriginal
	 * @return
	 */
	public int getNumberOfPosWords( String utteranceOriginal )
	{
		String pattern1 = ".*^1.*";
		String replacePattern1 = "^1";
		String pattern2 = ".*((not )|(n't ))((\\S)+ +){0,3}^1.*";
		String replacePattern2 = "((not )|(n't ))((\\S)+ +){0,3}^1";
		String utterance;
		int posCount = 0;

		for( String word : negativeWords ) {
			utterance = utteranceOriginal;
			String pat2 = pattern2.replace("^1", word);
			String rPat2 = replacePattern2.replace("^1", word);
			while( utterance.matches(pat2) ) {
				posCount++;
				utterance = utterance.replaceFirst(rPat2, " ");
			}
		}

		for( String word : positiveWords ) {
			utterance = utteranceOriginal;
			String pat1 = pattern1.replace("^1", word);
			String rPat1 = replacePattern1.replace("^1", word);
			while( utterance.matches(pat1) ) {
				posCount++;
				utterance = utterance.replaceFirst(rPat1, " ");
			}
		}
		return posCount;
	}

	/**
	 * Returns the number of negative words/phrases in the given string
	 * @param utteranceOriginal
	 * @return
	 */
	public int getNumberOfNegWords( String utteranceOriginal )
	{
		String pattern1 = ".*^1.*";
		String replacePattern1 = "^1";
		String pattern2 = ".*((not )|(n't ))((\\S)+ +){0,3}^1.*";
		String replacePattern2 = "((not )|(n't ))((\\S)+ +){0,3}^1";
		String utterance;
		int negCount = 0;

		for( String word : positiveWords ) {
			utterance = utteranceOriginal;
			String pat2 = pattern2.replace("^1", word);
			String rPat2 = replacePattern2.replace("^1", word);
			while( utterance.matches(pat2) ) {
				negCount++;
				utterance = utterance.replaceFirst(rPat2, " ");
			}
		}
		for( String word : negativeWords ) {
			utterance = utteranceOriginal;
			String pat1 = pattern1.replace("^1", word);
			String rPat1 = replacePattern1.replace("^1", word);
			while( utterance.matches(pat1) ) {
				negCount++;
				utterance = utterance.replaceFirst(rPat1, " ");
			}
		}
		return negCount;
	}
}
