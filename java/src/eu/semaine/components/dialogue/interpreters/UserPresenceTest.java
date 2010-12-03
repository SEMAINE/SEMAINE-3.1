/**
 * Copyright (C) 2010 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.dialogue.interpreters;

import static org.junit.Assert.*;

import javax.jms.JMSException;

import org.junit.BeforeClass;
import org.junit.Test;

import eu.semaine.jms.IOBase;
import eu.semaine.jms.message.SEMAINEEmmaMessage;
import eu.semaine.jms.message.SEMAINEMessageTestUtils;
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
		long currentTime = upi.getMeta().getTime() - UserPresenceInterpreter.TIME_THRESHOLD_VOICE_APPEARED - 1;
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
		long currentTime = upi.getMeta().getTime() - UserPresenceInterpreter.TIME_THRESHOLD_VOICE_DISAPPEARED - 1;
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
		long currentTime = upi.getMeta().getTime() - UserPresenceInterpreter.TIME_THRESHOLD_FACE_APPEARED - 1;
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
		long currentTime = upi.getMeta().getTime() - UserPresenceInterpreter.TIME_THRESHOLD_FACE_DISAPPEARED - 1;
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
		long currentTime = upi.getMeta().getTime() - UserPresenceInterpreter.TIME_THRESHOLD_VOICE_APPEARED - 1;
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
		long currentTime = upi.getMeta().getTime() - UserPresenceInterpreter.TIME_THRESHOLD_VOICE_APPEARED - 1;
		SEMAINEEmmaMessage message = getVoicePresenceMessage("start", currentTime);
		currentTime = upi.getMeta().getTime() - UserPresenceInterpreter.TIME_THRESHOLD_FACE_DISAPPEARED - 1;
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
		long currentTime = upi.getMeta().getTime() - UserPresenceInterpreter.TIME_THRESHOLD_FACE_APPEARED - 1;
		SEMAINEEmmaMessage message = getFacePresenceMessage("start", currentTime);
		currentTime = upi.getMeta().getTime() - UserPresenceInterpreter.TIME_THRESHOLD_VOICE_DISAPPEARED - 1;
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
		long currentTime = upi.getMeta().getTime() - UserPresenceInterpreter.TIME_THRESHOLD_VOICE_DISAPPEARED - 1;
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
		long currentTime = upi.getMeta().getTime() - UserPresenceInterpreter.TIME_THRESHOLD_FACE_DISAPPEARED - 1;
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
		long currentTime = upi.getMeta().getTime() - UserPresenceInterpreter.TIME_THRESHOLD_VOICE_APPEARED - 1;
		SEMAINEEmmaMessage message = getVoicePresenceMessage("start", currentTime);
		currentTime = upi.getMeta().getTime() - UserPresenceInterpreter.TIME_THRESHOLD_VOICE_DISAPPEARED - 1;
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
		long currentTime = upi.getMeta().getTime() - UserPresenceInterpreter.TIME_THRESHOLD_FACE_APPEARED - 1;
		SEMAINEEmmaMessage message = getFacePresenceMessage("start", currentTime);
		currentTime = upi.getMeta().getTime() - UserPresenceInterpreter.TIME_THRESHOLD_FACE_DISAPPEARED - 1;
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
		long currentTime = upi.getMeta().getTime() - UserPresenceInterpreter.TIME_THRESHOLD_FACE_APPEARED - 1;
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
		long currentTime = upi.getMeta().getTime() - UserPresenceInterpreter.TIME_THRESHOLD_FACE_APPEARED - 1;
		SEMAINEEmmaMessage message = getFacePresenceMessage("start", currentTime);
		currentTime = upi.getMeta().getTime() - UserPresenceInterpreter.TIME_THRESHOLD_FACE_DISAPPEARED - 1;
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
		long currentTime = upi.getMeta().getTime() - UserPresenceInterpreter.TIME_THRESHOLD_FACE_DISAPPEARED - 1;
		SEMAINEEmmaMessage message = getFacePresenceMessage("stop", currentTime);
		upi.react(message);
		// verify:
		assertFalse(upi.updateUserPresence());
		assertFalse(upi.isUserPresent());
	}
}
