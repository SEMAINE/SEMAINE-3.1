/**
 * Copyright (C) 2010 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.jms.message;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

import java.util.List;

import javax.jms.JMSException;
import javax.jms.Message;

import org.junit.Test;
import org.w3c.dom.Element;

import eu.semaine.datatypes.xml.EMMA;
import eu.semaine.datatypes.xml.SemaineML;

/**
 * @author marc
 *
 */
public class SEMAINEMessageTestUtilsTest {
	public static final String XPATHEXAMPLE_VALID = "/emma:emma/emma:interpretation/semaine:face-present/@statusChange";

	
	@Test
	public void createEMMAMessage_shouldSucceed() throws JMSException {
		SEMAINEMessageTestUtils.createEMMAMessage(XPATHEXAMPLE_VALID, "start");
	}
	
	@Test(expected = NullPointerException.class)
	public void createEMMAMessage_shouldThrowNPE() throws JMSException {
		SEMAINEMessageTestUtils.createEMMAMessage(null, "start");
	}

	
	@Test
	public void checkEMMAMessage_shouldYieldExpectedValue() throws JMSException {
		// setup & exercise
		String value = "start";
		SEMAINEEmmaMessage message = SEMAINEMessageTestUtils.createEMMAMessage(XPATHEXAMPLE_VALID, value);
		// verify
		assertNotNull(message);
		Element tli = message.getTopLevelInterpretation();
		assertNotNull(tli);
		assertEquals(tli.getLocalName(), EMMA.E_INTERPRETATION);
		List<Element> fpes = message.getFacePresentElements(tli);
		assertEquals(1, fpes.size());
		Element fpe = fpes.get(0);
		assertEquals(SemaineML.E_FACEPRESENT, fpe.getLocalName());
		String statusChange = fpe.getAttribute(SemaineML.A_STATUS_CHANGE);
		assertEquals(value, statusChange);
	}
	
	@Test
	public void createActiveMQMessage() throws JMSException {
		// setup/exercise
		Message m = SEMAINEMessageTestUtils.createActiveMQTextMessage();
		// verify
		assertNotNull(m);
	}
	
	
	@Test
	public void createXMLMessage_shouldSucceed() throws JMSException {
		SEMAINEXMLMessage m = SEMAINEMessageTestUtils.createXMLMessage(XPATHEXAMPLE_VALID, "start");
		assertNotNull(m.getDocument());
	}
	
	@Test(expected = NullPointerException.class)
	public void createXMLMessage_shouldThrowNPE() throws JMSException {
		SEMAINEMessageTestUtils.createXMLMessage(null, "start");
	}
}
