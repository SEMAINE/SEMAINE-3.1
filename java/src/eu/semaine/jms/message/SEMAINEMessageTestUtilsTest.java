/**
 * Copyright (C) 2010 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.jms.message;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.jms.JMSException;
import javax.jms.Message;

import org.junit.BeforeClass;
import org.junit.Test;
import org.w3c.dom.Element;

import eu.semaine.datatypes.stateinfo.ContextStateInfo;
import eu.semaine.datatypes.stateinfo.StateInfo;
import eu.semaine.datatypes.stateinfo.StateInfo.Type;
import eu.semaine.datatypes.xml.EMMA;
import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.jms.IOBase;
import eu.semaine.util.SEMAINEUtils;

/**
 * @author marc
 *
 */
public class SEMAINEMessageTestUtilsTest {
	public static final String XPATHEXAMPLE_VALID = "/emma:emma/emma:interpretation/semaine:face-present/@statusChange";

	@BeforeClass
	public static void beforeClass() throws Exception {
		// For StateInfo.class:
		System.setProperty("semaine.stateinfo-config.resource", "eu/semaine/jms/message/stateinfo-for-tests.config");
		SEMAINEUtils.setupLog4j();
		IOBase.useEmbeddedBroker();
	}
	
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
	
	
	@Test
	public void createStateMessage_shouldSucceed() throws JMSException {
		// setup
		Type type = Type.ContextState;
		String shortname = StateInfo.getShortNames(type).iterator().next();
		String value = "bla bla";
		Map<String, String> infoMap = new HashMap<String, String>();
		infoMap.put(shortname, value);
		StateInfo info = new ContextStateInfo(infoMap);
		// exercise
		SEMAINEStateMessage ssm = SEMAINEMessageTestUtils.createStateMessage(info);
		// verify
		assertEquals(type, ssm.getState().getType());
		assertEquals(value, ssm.getState().getInfo(shortname));
	}
	
	
}
