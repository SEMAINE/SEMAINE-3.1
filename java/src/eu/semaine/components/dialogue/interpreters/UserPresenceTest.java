/**
 * Copyright (C) 2010 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.dialogue.interpreters;

import static org.junit.Assert.*;

import javax.jms.JMSException;

import org.junit.BeforeClass;
import org.junit.Test;

import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.jms.IOBase;
import eu.semaine.jms.message.SEMAINEEmmaMessage;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.message.SEMAINEMessageTestUtils;
import eu.semaine.jms.message.SEMAINEXMLMessage;
import eu.semaine.util.SEMAINEUtils;


/**
 * @author marc
 *
 */
public class UserPresenceTest {

	private static final String XPATH_FACE_PRESENCE = "/emma:emma/emma:interpretation/semaine:face-present/@statusChange";
	private static final String XPATH_FACE_TIME = "/emma:emma/emma:interpretation/semaine:face-present/@start";
	private static final String XPATH_VOICE_PRESENCE = "/emma:emma/emma:interpretation/semaine:speaking/@statusChange";
	private static final String XPATH_VOICE_TIME = "/emma:emma/emma:interpretation/@emma:offset-to-start";
	private static final String XPATH_CALLBACK_ID = "/semaine:callback/semaine:event/@id";
	private static final String XPATH_CALLBACK_TIME = "/semaine:callback/semaine:event/@time";
	private static final String XPATH_CALLBACK_TYPE = "/semaine:callback/semaine:event/@type";
	private static final String XPATH_CALLBACK_CONTENTTYPE = "/semaine:callback/semaine:event/@contentType";
	
	
	SEMAINEEmmaMessage getFacePresenceMessage(String statusChange) throws JMSException {
		return SEMAINEMessageTestUtils.createEMMAMessage(XPATH_FACE_PRESENCE, statusChange);
	}
	
	SEMAINEEmmaMessage getFacePresenceMessage(String statusChange, long time) throws JMSException {
		return SEMAINEMessageTestUtils.createEMMAMessage(XPATH_FACE_PRESENCE, statusChange, XPATH_FACE_TIME, String.valueOf(time));
	}
	

	SEMAINEEmmaMessage getVoicePresenceMessage(String statusChange) throws JMSException {
		return SEMAINEMessageTestUtils.createEMMAMessage(XPATH_VOICE_PRESENCE, statusChange);
	}
	
	SEMAINEEmmaMessage getVoicePresenceMessage(String statusChange, long time) throws JMSException {
		return SEMAINEMessageTestUtils.createEMMAMessage(XPATH_VOICE_PRESENCE, statusChange, XPATH_VOICE_TIME, String.valueOf(time));
	}
	
	SEMAINEXMLMessage getCallbackMessage(String contentType, String callbackType, long time) throws JMSException {
		return SEMAINEMessageTestUtils.createXMLMessage(XPATH_CALLBACK_ID, "dummy123",
				XPATH_CALLBACK_TIME, String.valueOf(time),
				XPATH_CALLBACK_TYPE, callbackType,
				XPATH_CALLBACK_CONTENTTYPE, contentType);
	}
	
	
	
	@BeforeClass
	public static void beforeClass() throws Exception {
		SEMAINEUtils.setupLog4j();
		IOBase.useEmbeddedBroker();
	}
	
	@Test
	public void isFaceDetected_shouldBeTrue() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		SEMAINEEmmaMessage message = getFacePresenceMessage("start"); 
		// exercise & verify
		assertTrue(upi.isFaceDetected(message));
	}
	
	@Test
	public void isFaceDetected_shouldBeFalse1() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		SEMAINEEmmaMessage message = getFacePresenceMessage("stop");
		// exercise & verify
		assertFalse(upi.isFaceDetected(message));
	}

	@Test
	public void isFaceDetected_shouldBeFalse2() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		SEMAINEEmmaMessage message = getVoicePresenceMessage("start");
		// exercise & verify
		assertFalse(upi.isFaceDetected(message));
	}

	@Test
	public void isFaceDisappeared_shouldBeTrue() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		SEMAINEEmmaMessage message = getFacePresenceMessage("stop");
		// exercise & verify
		assertTrue(upi.isFaceDisappeared(message));
	}
	
	@Test
	public void checkFaceChangeTime() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		long expectedTime = 123;
		SEMAINEEmmaMessage message = getFacePresenceMessage("stop", expectedTime);
		// exercise
		long actualTime = upi.getFaceChangeTime(message);
		// verify
		assertEquals(expectedTime, actualTime);
	}
	
	
	@Test
	public void isVoiceDetected_shouldBeTrue() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		SEMAINEEmmaMessage message = getVoicePresenceMessage("start"); 
		// exercise & verify
		assertTrue(upi.isVoiceDetected(message));
	}
	
	@Test
	public void isVoiceDetected_shouldBeFalse1() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		SEMAINEEmmaMessage message = getVoicePresenceMessage("stop");
		// exercise & verify
		assertFalse(upi.isVoiceDetected(message));
	}

	@Test
	public void isVoiceDetected_shouldBeFalse2() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		SEMAINEEmmaMessage message = getFacePresenceMessage("start");
		// exercise & verify
		assertFalse(upi.isVoiceDetected(message));
	}

	@Test
	public void isVoiceDisappeared_shouldBeTrue() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		SEMAINEEmmaMessage message = getVoicePresenceMessage("stop");
		// exercise & verify
		assertTrue(upi.isVoiceDisappeared(message));
	}
	
	@Test
	public void checkVoiceChangeTime() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		long expectedTime = 123;
		SEMAINEEmmaMessage message = getVoicePresenceMessage("stop", expectedTime);
		// exercise
		long actualTime = upi.getVoiceChangeTime(message);
		// verify
		assertEquals(expectedTime, actualTime);
	}

	@Test
	public void getMetaTime_shouldBeGreaterZero() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		// exercise & verify
		assertTrue(upi.getMeta().getTime() > 0);
		
	}
	
	@Test
	public void isVoicePresent_shouldBePresent1() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		SEMAINEEmmaMessage message = getVoicePresenceMessage("start"); 
		// exercise
		upi.react(message);
		// verify
		assertTrue(upi.isVoicePresent());
	}

	@Test
	public void isVoicePresent_shouldBePresent2() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		long currentTime = upi.getMeta().getTime() - upi.getThresholdVoiceAppeared() - 1;
		SEMAINEEmmaMessage message = getVoicePresenceMessage("start", currentTime); 
		// exercise
		upi.react(message);
		// verify
		assertTrue(upi.isVoicePresent());
	}

	@Test
	public void isVoicePresent_shouldBePresent3() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		long currentTime = upi.getMeta().getTime();
		SEMAINEEmmaMessage message = getVoicePresenceMessage("stop", currentTime); 
		// exercise
		upi.react(message);
		// verify
		assertTrue(upi.isVoicePresent());
	}

	@Test
	public void isVoicePresent_shouldNotBePresent1() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		long currentTime = upi.getMeta().getTime();
		SEMAINEEmmaMessage message = getVoicePresenceMessage("start", currentTime); 
		// exercise
		upi.react(message);
		// verify
		assertFalse(upi.isVoicePresent());
	}

	@Test
	public void isVoicePresent_shouldNotBePresent2() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		long currentTime = upi.getMeta().getTime() - upi.getThresholdVoiceDisappeared() - 1;
		SEMAINEEmmaMessage message = getVoicePresenceMessage("stop", currentTime); 
		// exercise
		upi.react(message);
		// verify
		assertFalse(upi.isVoicePresent());
	}
	
	@Test
	public void isFacePresent_shouldBePresent1() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		SEMAINEEmmaMessage message = getFacePresenceMessage("start"); 
		// exercise
		upi.react(message);
		// verify
		assertTrue(upi.isFacePresent());
	}

	@Test
	public void isFacePresent_shouldBePresent2() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		long currentTime = upi.getMeta().getTime() - upi.getThresholdFaceAppeared() - 1;
		SEMAINEEmmaMessage message = getFacePresenceMessage("start", currentTime);
		// exercise
		upi.react(message);
		// verify
		assertTrue(upi.isFacePresent());
	}

	@Test
	public void isFacePresent_shouldBePresent3() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		long currentTime = upi.getMeta().getTime();
		SEMAINEEmmaMessage message = getFacePresenceMessage("stop", currentTime); 
		// exercise
		upi.react(message);
		// verify
		assertTrue(upi.isFacePresent());
	}

	@Test
	public void isFacePresent_shouldNotBePresent() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		long currentTime = upi.getMeta().getTime() - upi.getThresholdFaceDisappeared() - 1;
		SEMAINEEmmaMessage message = getFacePresenceMessage("stop", currentTime); 
		// exercise
		upi.react(message);
		// verify
		assertFalse(upi.isFacePresent());
	}

	
	@Test
	public void isUserPresent_shouldBePresent1() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		SEMAINEEmmaMessage message = getFacePresenceMessage("start"); 
		// exercise
		upi.react(message);
		upi.updateUserPresence();
		// verify
		assertTrue(upi.isUserPresent());
	}

	@Test
	public void isUserPresent_shouldBePresent2() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		long currentTime = upi.getMeta().getTime() - upi.getThresholdVoiceAppeared() - 1;
		SEMAINEEmmaMessage message = getVoicePresenceMessage("start", currentTime); 
		// exercise
		upi.react(message);
		upi.updateUserPresence();
		// verify
		assertTrue(upi.isUserPresent());
	}

	@Test
	public void isUserPresent_shouldBePresent3() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		long currentTime = upi.getMeta().getTime() - upi.getThresholdVoiceAppeared() - 1;
		SEMAINEEmmaMessage message = getVoicePresenceMessage("start", currentTime);
		currentTime = upi.getMeta().getTime() - upi.getThresholdFaceDisappeared() - 1;
		SEMAINEEmmaMessage message2 = getFacePresenceMessage("stop", currentTime); 
		// exercise
		upi.react(message);
		upi.react(message2);
		upi.updateUserPresence();
		// verify
		assertTrue(upi.isUserPresent());
	}

	@Test
	public void isUserPresent_shouldBePresent4() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		long currentTime = upi.getMeta().getTime() - upi.getThresholdFaceAppeared() - 1;
		SEMAINEEmmaMessage message = getFacePresenceMessage("start", currentTime);
		currentTime = upi.getMeta().getTime() - upi.getThresholdVoiceDisappeared() - 1;
		SEMAINEEmmaMessage message2 = getVoicePresenceMessage("stop", currentTime); 
		// exercise
		upi.react(message);
		upi.react(message2);
		upi.updateUserPresence();
		// verify
		assertTrue(upi.isUserPresent());
	}

	@Test
	public void isUserPresent_shouldNotBePresent1() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		long currentTime = upi.getMeta().getTime() - upi.getThresholdVoiceDisappeared() - 1;
		SEMAINEEmmaMessage message = getVoicePresenceMessage("stop", currentTime); 
		// exercise
		upi.react(message);
		upi.updateUserPresence();
		// verify
		assertFalse(upi.isUserPresent());
	}

	@Test
	public void isUserPresent_shouldNotBePresent2() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		long currentTime = upi.getMeta().getTime() - upi.getThresholdFaceDisappeared() - 1;
		SEMAINEEmmaMessage message = getFacePresenceMessage("stop", currentTime); 
		// exercise
		upi.react(message);
		upi.updateUserPresence();
		// verify
		assertFalse(upi.isUserPresent());
	}

	@Test
	public void isUserPresent_shouldNotBePresent3() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		long currentTime = upi.getMeta().getTime() - upi.getThresholdVoiceAppeared() - 1;
		SEMAINEEmmaMessage message = getVoicePresenceMessage("start", currentTime);
		currentTime = upi.getMeta().getTime() - upi.getThresholdVoiceDisappeared() - 1;
		SEMAINEEmmaMessage message2 = getVoicePresenceMessage("stop", currentTime); 
		// exercise
		upi.react(message);
		upi.react(message2);
		upi.updateUserPresence();
		// verify
		assertFalse(upi.isUserPresent());
	}

	@Test
	public void isUserPresent_shouldNotBePresent4() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		long currentTime = upi.getMeta().getTime() - upi.getThresholdFaceAppeared() - 1;
		SEMAINEEmmaMessage message = getFacePresenceMessage("start", currentTime);
		currentTime = upi.getMeta().getTime() - upi.getThresholdFaceDisappeared() - 1;
		SEMAINEEmmaMessage message2 = getFacePresenceMessage("stop", currentTime); 
		// exercise
		upi.react(message);
		upi.react(message2);
		upi.updateUserPresence();
		// verify
		assertFalse(upi.isUserPresent());
	}

	
	@Test
	public void hasUserPresenceChanged_shouldHaveAppeared() throws JMSException{
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		// user appears:
		long currentTime = upi.getMeta().getTime() - upi.getThresholdFaceAppeared() - 1;
		SEMAINEEmmaMessage message = getFacePresenceMessage("start", currentTime);
		upi.react(message);
		// verify:
		assertTrue(upi.updateUserPresence());
		assertTrue(upi.isUserPresent());
	}

	@Test
	public void hasUserPresenceChanged_shouldHaveDisppeared() throws JMSException{
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		// user appears:
		long currentTime = upi.getMeta().getTime() - upi.getThresholdFaceAppeared() - 1;
		SEMAINEEmmaMessage message = getFacePresenceMessage("start", currentTime);
		currentTime = upi.getMeta().getTime() - upi.getThresholdFaceDisappeared() - 1;
		SEMAINEEmmaMessage message2 = getFacePresenceMessage("stop", currentTime);
		upi.react(message);
		upi.updateUserPresence();
		upi.react(message2);
		// verify:
		assertTrue(upi.updateUserPresence());
		assertFalse(upi.isUserPresent());
	}

	@Test
	public void hasUserPresenceChanged_shouldBeFalse() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		// user disappears (but was not there):
		long currentTime = upi.getMeta().getTime() - upi.getThresholdFaceDisappeared() - 1;
		SEMAINEEmmaMessage message = getFacePresenceMessage("stop", currentTime);
		upi.react(message);
		// verify:
		assertFalse(upi.updateUserPresence());
		assertFalse(upi.isUserPresent());
	}
	
	
	@Test
	public void isSystemSpeaking_shouldBeTrue() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		long currentTime = upi.getMeta().getTime();
		SEMAINEXMLMessage m = getCallbackMessage(SEMAINEMessage.CONTENT_TYPE_UTTERANCE, "start", currentTime);
		// exercise & verify
		assertTrue(upi.isSystemStartsSpeaking(m));
	}
	
	@Test
	public void isSystemSpeaking_shouldBeFalse1() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		long currentTime = upi.getMeta().getTime();
		SEMAINEXMLMessage m = getCallbackMessage(SEMAINEMessage.CONTENT_TYPE_UTTERANCE, "stop", currentTime);
		// exercise & verify
		assertFalse(upi.isSystemStartsSpeaking(m));
	}

	@Test
	public void isSystemSpeaking_shouldBeFalse2() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		long currentTime = upi.getMeta().getTime();
		SEMAINEXMLMessage m = getCallbackMessage(SEMAINEMessage.CONTENT_TYPE_LISTENERVOCALISATION, "start", currentTime);
		// exercise & verify
		assertFalse(upi.isSystemStartsSpeaking(m));
	}

	@Test(expected = NullPointerException.class)
	public void isSystemSpeaking_shouldThrowNPE() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		// exercise & verify
		upi.isSystemStartsSpeaking(null);
	}

	@Test(expected = MessageFormatException.class)
	public void isSystemSpeaking_shouldThrowMFE() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		long currentTime = upi.getMeta().getTime();
		SEMAINEEmmaMessage message = getFacePresenceMessage("stop", currentTime);
		// exercise & verify
		upi.isSystemStartsSpeaking(message);
	}

	@Test
	public void isSystemStopSpeaking_shouldBeTrue() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		long currentTime = upi.getMeta().getTime();
		SEMAINEXMLMessage m = getCallbackMessage(SEMAINEMessage.CONTENT_TYPE_UTTERANCE, "end", currentTime);
		// exercise & verify
		assertTrue(upi.isSystemStopsSpeaking(m));
	}

	
	@Test
	public void checkUserPresentWhileSystemIsSpeaking() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		// set fixture state to voice present
		long currentTime = upi.getMeta().getTime() - upi.getThresholdVoiceAppeared() - 1;
		SEMAINEEmmaMessage message = getVoicePresenceMessage("start", currentTime); 
		upi.react(message);
		upi.updateUserPresence();
		// exercise: system starts speaking, then a voice stop is received with a timeout that would normally trigger user not present
		SEMAINEXMLMessage callbackMessage = getCallbackMessage(SEMAINEMessage.CONTENT_TYPE_UTTERANCE, "start", currentTime);
		upi.react(callbackMessage);
		long currentTime2 = upi.getMeta().getTime() - upi.getThresholdVoiceDisappeared() - 1;
		SEMAINEEmmaMessage message2 = getVoicePresenceMessage("stop", currentTime2); 
		upi.react(message2);
		upi.updateUserPresence();
		// verify
		assertTrue(upi.isUserPresent());
	}
	
	@Test
	public void checkSystemSpeaking_shouldBeTrue1() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		// exercise
		long currentTime = upi.getMeta().getTime();
		SEMAINEXMLMessage callbackMessage = getCallbackMessage(SEMAINEMessage.CONTENT_TYPE_UTTERANCE, "start", currentTime);
		upi.react(callbackMessage);
		// verify
		assertTrue(upi.isSystemSpeaking());
	}

	@Test
	public void checkSystemSpeaking_shouldBeTrue2() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		// exercise: immediately after stopped speaking
		long currentTime = upi.getMeta().getTime();
		SEMAINEXMLMessage callbackMessage = getCallbackMessage(SEMAINEMessage.CONTENT_TYPE_UTTERANCE, "end", currentTime);
		upi.react(callbackMessage);
		// verify
		assertTrue(upi.isSystemSpeaking());
	}

	@Test
	public void checkSystemSpeaking_shouldBeFalse() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		// exercise: timeout after stopped speaking exceeded
		long currentTime = upi.getMeta().getTime() - upi.getThresholdSystemStoppedSpeaking() - 1;
		SEMAINEXMLMessage callbackMessage = getCallbackMessage(SEMAINEMessage.CONTENT_TYPE_UTTERANCE, "end", currentTime);
		upi.react(callbackMessage);
		// verify
		assertFalse(upi.isSystemSpeaking());
	}
	
	@Test
	public void checkSystemSpeakingTime() throws JMSException {
		// setup
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		// exercise
		long currentTime = 123;
		SEMAINEXMLMessage callbackMessage = getCallbackMessage(SEMAINEMessage.CONTENT_TYPE_UTTERANCE, "end", currentTime);
		// verify
		assertEquals(currentTime, upi.getSystemSpeakingEventTime(callbackMessage));
	}
	
	@Test
	public void configSettings_voiceStart() throws JMSException {
		// setup
		long expected = 12345;
		System.setProperty(UserPresenceInterpreter.PROPERTY_VOICE_START_THRESHOLD, String.valueOf(expected));
		// exercise
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		// verify
		assertEquals(expected, upi.getThresholdVoiceAppeared());
	}

	@Test
	public void configSettings_voiceEnd() throws JMSException {
		// setup
		long expected = 12345;
		System.setProperty(UserPresenceInterpreter.PROPERTY_VOICE_END_THRESHOLD, String.valueOf(expected));
		// exercise
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		// verify
		assertEquals(expected, upi.getThresholdVoiceDisappeared());
	}

	@Test
	public void configSettings_faceStart() throws JMSException {
		// setup
		long expected = 12345;
		System.setProperty(UserPresenceInterpreter.PROPERTY_FACE_START_THRESHOLD, String.valueOf(expected));
		// exercise
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		// verify
		assertEquals(expected, upi.getThresholdFaceAppeared());
	}

	@Test
	public void configSettings_faceEnd() throws JMSException {
		// setup
		long expected = 12345;
		System.setProperty(UserPresenceInterpreter.PROPERTY_FACE_END_THRESHOLD, String.valueOf(expected));
		// exercise
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		// verify
		assertEquals(expected, upi.getThresholdFaceDisappeared());
	}

	@Test
	public void configSettings_systemEnd() throws JMSException {
		// setup
		long expected = 12345;
		System.setProperty(UserPresenceInterpreter.PROPERTY_SYSTEM_END_THRESHOLD, String.valueOf(expected));
		// exercise
		UserPresenceInterpreter upi = new UserPresenceInterpreter();
		// verify
		assertEquals(expected, upi.getThresholdSystemStoppedSpeaking());
	}

}
