/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.dialogue.interpreters;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.jms.JMSException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.datatypes.stateinfo.ContextStateInfo;
import eu.semaine.datatypes.stateinfo.StateInfo;
import eu.semaine.datatypes.xml.BML;
import eu.semaine.datatypes.xml.EMMA;
import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.jms.message.SEMAINEEmmaMessage;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.message.SEMAINEXMLMessage;
import eu.semaine.jms.receiver.EmmaReceiver;
import eu.semaine.jms.receiver.XMLReceiver;
import eu.semaine.jms.sender.StateSender;
import eu.semaine.util.XMLTool;

/**
 * @author marc
 *
 */
public class UserPresenceInterpreter extends Component
{
	public static final long TIME_THRESHOLD_VOICE_APPEARED = 1000;
	public static final long TIME_THRESHOLD_VOICE_DISAPPEARED = 10000;
	public static final long TIME_THRESHOLD_FACE_APPEARED = 0;
	public static final long TIME_THRESHOLD_FACE_DISAPPEARED = 3000;
	public static final long TIME_THRESHOLD_SYSTEM_STOPPED_SPEAKING = 10000;
	
	private boolean isFacePresent = false;
	private long timeFaceAppeared = -1;
	private long timeFaceDisappeared = -1;
	private boolean isVoicePresent = false;
	private long timeVoiceAppeared = -1;
	private long timeVoiceDisappeared = -1;
	private boolean isSystemSpeaking = false;
	private long timeSystemStoppedSpeaking = -1;
	private boolean userPresent = false;
	

	
	private EmmaReceiver facedetReceiver;
	private EmmaReceiver speakingReceiver;
	private XMLReceiver callbackReceiver;
	private StateSender userPresenceSender;
	
	
	
	
	/**
	 * @param componentName
	 * @throws JMSException
	 */
	public UserPresenceInterpreter()
	throws JMSException
	{
		super("UserPresenceInterpreter");
		facedetReceiver = new EmmaReceiver("semaine.data.state.user.emma.nonverbal.face");
		receivers.add(facedetReceiver);
		
		speakingReceiver = new EmmaReceiver("semaine.data.state.user.emma.nonverbal.voice");
		receivers.add(speakingReceiver);
		
		callbackReceiver = new XMLReceiver("semaine.callback.output.Animation");
		receivers.add(callbackReceiver);
		
		userPresenceSender = new StateSender("semaine.data.state.context", StateInfo.Type.ContextState, getName());
		senders.add(userPresenceSender);
	}
	
	/**
	 * Determine whether the given message indicates the fact that a face has been detected.
	 * @param m an emma message which may or may not indicate face presence.
	 * @return
	 */
	protected boolean isFaceDetected(SEMAINEEmmaMessage m) {
		return "start".equals(getFaceStatusChange(m));
	}
	
	/**
	 * Determine whether the given message indicates the fact that a face has just disappeared.
	 * @param m an emma message which may or may not indicate face presence.
	 * @return
	 */
	protected boolean isFaceDisappeared(SEMAINEEmmaMessage m) {
		return "stop".equals(getFaceStatusChange(m));
	}

	/**
	 * If the given message indicates the fact that a face status change has been detected, provide the type of that change.
	 * @param m an emma message which may or may not indicate face status change.
	 * @param the type of status change to verify
	 * @return the face status change as a string, if any, or the empty string
	 */
	protected String getFaceStatusChange(SEMAINEEmmaMessage m) {
		Element tli = m.getTopLevelInterpretation();
		if (tli == null) {
			return "";
		}
		List<Element> facePresentElements = m.getFacePresentElements(tli);
		if (facePresentElements.isEmpty()) {
			return "";
		}
		Element facePresentElement = facePresentElements.get(0);
		assert facePresentElement.getLocalName().equals(SemaineML.E_FACEPRESENT);
		String statusChange = facePresentElement.getAttribute(SemaineML.A_STATUS_CHANGE);
		return statusChange;
	}
	
	/**
	 * If the given message indicates the fact that a face status change has been detected, 
	 * provide the time of that change, if possible.
	 * @param m an emma message which may or may not indicate face status change.
	 * @return the time of the status change in milliseconds since program start, or -1 if the information is not available.
	 */
	protected long getFaceChangeTime(SEMAINEEmmaMessage m) {
		Element tli = m.getTopLevelInterpretation();
		if (tli == null) {
			return -1;
		}
		List<Element> facePresentElements = m.getFacePresentElements(tli);
		if (facePresentElements.isEmpty()) {
			return -1;
		}
		Element facePresentElement = facePresentElements.get(0);
		assert facePresentElement.getLocalName().equals(SemaineML.E_FACEPRESENT);
		String faceTimeString = facePresentElement.getAttribute(BML.A_START);
		return string2long(faceTimeString);
	}
	
	/**
	 * Determine whether the given message indicates the fact that a voice has been detected.
	 * @param m an emma message which may or may not indicate voice presence.
	 * @return
	 */
	protected boolean isVoiceDetected(SEMAINEEmmaMessage m) {
		return "start".equals(getVoiceStatusChange(m));
	}
	
	/**
	 * Determine whether the given message indicates the fact that a voice has just disappeared.
	 * @param m an emma message which may or may not indicate voice presence.
	 * @return
	 */
	protected boolean isVoiceDisappeared(SEMAINEEmmaMessage m) {
		return "stop".equals(getVoiceStatusChange(m));
	}

	/**
	 * If the given message indicates the fact that a voice status change has been detected, provide the type of that change.
	 * @param m an emma message which may or may not indicate voice status change.
	 * @param the type of status change to verify
	 * @return the voice status change as a string, if any, or the empty string
	 */
	protected String getVoiceStatusChange(SEMAINEEmmaMessage m) {
		Element tli = m.getTopLevelInterpretation();
		if (tli == null) {
			return "";
		}
		List<Element> voicePresentElements = m.getSpeakingElements(tli);
		if (voicePresentElements.isEmpty()) {
			return "";
		}
		Element voicePresentElement = voicePresentElements.get(0);
		assert voicePresentElement.getLocalName().equals(SemaineML.E_SPEAKING);
		String statusChange = voicePresentElement.getAttribute(SemaineML.A_STATUS_CHANGE);
		return statusChange;
	}
	
	/**
	 * If the given message indicates the fact that a face status change has been detected, 
	 * provide the time of that change, if possible.
	 * @param m an emma message which may or may not indicate face status change.
	 * @return the time of the status change in milliseconds since program start, or -1 if the information is not available.
	 */
	protected long getVoiceChangeTime(SEMAINEEmmaMessage m) {
		Element tli = m.getTopLevelInterpretation();
		if (tli == null) {
			return -1;
		}
		List<Element> voicePresentElements = m.getSpeakingElements(tli);
		if (voicePresentElements.isEmpty()) {
			return -1;
		}
		Element voicePresentElement = voicePresentElements.get(0);
		assert voicePresentElement.getLocalName().equals(SemaineML.E_SPEAKING);
		String voiceTimeString = tli.getAttribute(EMMA.A_OFFSET_TO_START);
		return string2long(voiceTimeString);
	}
	
	protected boolean isSystemStartsSpeaking(SEMAINEXMLMessage callbackMessage) throws MessageFormatException, NullPointerException {
		Element event = getEventElement(callbackMessage);
		String contentType = event.getAttribute(SemaineML.A_CONTENTTYPE);
		String type = event.getAttribute(SemaineML.A_TYPE);
		return contentType.equals(SEMAINEMessage.CONTENT_TYPE_UTTERANCE) && type.equals(SemaineML.V_START);
	}

	protected boolean isSystemStopsSpeaking(SEMAINEXMLMessage callbackMessage) throws MessageFormatException, NullPointerException {
		Element event = getEventElement(callbackMessage);
		String contentType = event.getAttribute(SemaineML.A_CONTENTTYPE);
		String type = event.getAttribute(SemaineML.A_TYPE);
		return contentType.equals(SEMAINEMessage.CONTENT_TYPE_UTTERANCE) && type.equals(SemaineML.V_END);
	}
	
	protected long getSystemSpeakingEventTime(SEMAINEXMLMessage callbackMessage) throws MessageFormatException, NullPointerException {
		Element event = getEventElement(callbackMessage);
		String timeString = event.getAttribute(SemaineML.A_TIME);
		return string2long(timeString);
	}


	/**
	 * @param callbackMessage
	 * @return
	 * @throws NullPointerException
	 * @throws MessageFormatException
	 */
	private Element getEventElement(SEMAINEXMLMessage callbackMessage)
			throws NullPointerException, MessageFormatException {
		if (callbackMessage == null) {
			throw new NullPointerException("null argument");
		}
		Document doc = callbackMessage.getDocument();
		if (doc == null) {
			throw new NullPointerException("null document");
		}
		Element root = doc.getDocumentElement();
		if (root == null || !(root.getLocalName().equals(SemaineML.E_CALLBACK) && root.getNamespaceURI().equals(SemaineML.namespaceURI))) {
			throw new MessageFormatException("Expected root element '"+SemaineML.E_CALLBACK+"' in namespace '"+SemaineML.namespaceURI+"' but got '"
					+root.getLocalName()+"' in namespace '"+root.getNamespaceURI()+"'");
		}
		Element event = XMLTool.needChildElementByLocalNameNS(doc.getDocumentElement(), SemaineML.E_EVENT, SemaineML.namespaceURI);
		return event;
	}
	
	private long string2long(String s) {
		if (s.equals("")) {
			return -1;
		}
		try {
			long t = Long.parseLong(s);
			return t;
		} catch (NumberFormatException nfe) {
			log.warn("Unexpected long number: "+s, nfe);
			return -1;
		}
	}
	
	
	
	
	@Override
	protected void react(SEMAINEMessage m)
	throws JMSException {
		if (m instanceof SEMAINEEmmaMessage) {
			SEMAINEEmmaMessage em = (SEMAINEEmmaMessage) m;
			if (isFaceDetected(em)) {
				isFacePresent = true;
				timeFaceAppeared = getFaceChangeTime(em);
			} else if (isFaceDisappeared(em)) {
				isFacePresent = false;
				timeFaceDisappeared = getFaceChangeTime(em);
			}
			if (isVoiceDetected(em)) {
				isVoicePresent = true;
				timeVoiceAppeared = getVoiceChangeTime(em);
			} else if (isVoiceDisappeared(em)) {
				isVoicePresent = false;
				timeVoiceDisappeared = getVoiceChangeTime(em);
			}
		} else if (m instanceof SEMAINEXMLMessage) {
			SEMAINEXMLMessage xm = (SEMAINEXMLMessage) m;
			if (isSystemStartsSpeaking(xm)) {
				isSystemSpeaking = true;
			} else if (isSystemStopsSpeaking(xm)) {
				isSystemSpeaking = false;
				timeSystemStoppedSpeaking = getSystemSpeakingEventTime(xm);
			}
		} // else ignore
	}

	
	/**
	 * Determine whether the voice is currently present,
	 * or presence is so recent that we still consider the voice to be present.
	 * @return true if present, false otherwise.
	 */
	protected boolean isVoicePresent() {
		long currentTime = meta.getTime();
		return isVoicePresent && currentTime - timeVoiceAppeared >= TIME_THRESHOLD_VOICE_APPEARED
			|| currentTime - timeVoiceDisappeared <= TIME_THRESHOLD_VOICE_DISAPPEARED;
	}
	
	/**
	 * Determine whether the face is currently present,
	 * or presence is so recent that we still consider the face to be present.
	 * @return true if present, false otherwise.
	 */
	protected boolean isFacePresent() {
		long currentTime = meta.getTime();
		return isFacePresent && currentTime - timeFaceAppeared >= TIME_THRESHOLD_FACE_APPEARED
			|| currentTime - timeFaceDisappeared <= TIME_THRESHOLD_FACE_DISAPPEARED;
	}
	
	/**
	 * Determine whether the system is currently speaking,
	 * or stopped speaking so recently that we still consider the system to be speaking.
	 * @return
	 */
	protected boolean isSystemSpeaking() {
		long currentTime = meta.getTime();
		return isSystemSpeaking 
			|| currentTime - timeSystemStoppedSpeaking <= TIME_THRESHOLD_SYSTEM_STOPPED_SPEAKING;
	}
	
	
	/**
	 * Determine based on the observed evidence (face, voice) whether the user is currently present, 
	 * or presence is so recent that we still consider the user to be present.
	 * @return true if present, false otherwise.
	 * @return
	 */
	protected boolean isUserPresent() {
		return userPresent;
	}
	
	
	/**
	 * Update the internal state of the component reflecting the user presence,
	 * which can be obtained from {@link #isUserPresent()}.
	 * @return true if user presence has changed during the update, false otherwise
	 */
	protected boolean updateUserPresence() {
		// system speaking prevents the user from disappearing, but does not make him/her appear.
		if (userPresent && !(isFacePresent() || isVoicePresent() || isSystemSpeaking())) {
			// user has disappeared
			userPresent = false;
			return true; // changed
		} else if (!userPresent && (isFacePresent() || isVoicePresent())) {
			// user has appeared
			userPresent = true;
			return true; // changed
		}
		return false;
		
	}
	


	@Override
	protected void act()
	throws JMSException
	{
		if (updateUserPresence()) {
			sendUserPresenceChange();
		}
		
	}

	
	protected void sendUserPresenceChange()
	throws JMSException
	{
		String presenceState = userPresent ? SemaineML.V_PRESENT : SemaineML.V_ABSENT;
		log.info("Sending user presence update: New state = "+presenceState);
		Map<String, String> info = new HashMap<String, String>();
		info.put("userPresent", presenceState);
		ContextStateInfo csi = new ContextStateInfo(info);
		userPresenceSender.sendStateInfo(csi, meta.getTime());
	}



}
