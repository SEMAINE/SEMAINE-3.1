/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.dummy;

import java.util.Random;

import javax.jms.JMSException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.datatypes.EMMA;
import eu.semaine.datatypes.SEMAINEFeatureMessage;
import eu.semaine.datatypes.SEMAINEMessage;
import eu.semaine.datatypes.SEMAINEXMLMessage;
import eu.semaine.datatypes.SemaineML;
import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.jms.EmmaSender;
import eu.semaine.jms.FeatureReceiver;
import eu.semaine.jms.StateReceiver;
import eu.semaine.jms.StateSender;
import eu.semaine.jms.Sender;
import eu.semaine.jms.IOBase.Event;
import eu.semaine.util.XMLTool;

/**
 * This dummy class mimics an analyser. It reads features and dialogue state,
 * remembers whether the user is speaking, and applies some criterion to 
 * extract features from the stream and pass them on.
 * @author marc
 *
 */
public class DummyAnalyser extends Component 
{
	private EmmaSender userStateSender;
	private StateReceiver dialogStateReceiver;
	private FeatureReceiver featureReceiver;
	
	private boolean userIsSpeaking = false;
	
	/**
	 * @param componentName
	 * @throws JMSException
	 */
	public DummyAnalyser() throws JMSException 
	{
		super("DummyAnalyser");
		featureReceiver = new FeatureReceiver("semaine.data.analysis.>");
		receivers.add(featureReceiver); // to set up properly
		dialogStateReceiver = new StateReceiver("semaine.data.state.dialog");
		receivers.add(dialogStateReceiver);
		userStateSender = new EmmaSender("semaine.data.state.user", getName());
		senders.add(userStateSender); // so it can be started etc

	}

	public void react(SEMAINEMessage m) throws JMSException
	{
		if (m instanceof SEMAINEFeatureMessage) {
			SEMAINEFeatureMessage fm = (SEMAINEFeatureMessage) m;
			float[] features = fm.getFeatureVector();
			if (features[0] < 0.02) { // some arbitrary condition
				// Create and fill a simple EMMA document
				Document document = XMLTool.newDocument(EMMA.EMMA, EMMA.namespace, EMMA.version);
				Element interpretation = XMLTool.appendChildElement(document.getDocumentElement(), EMMA.INTERPRETATION);
				Element feature = XMLTool.appendChildElement(interpretation, SemaineML.FEATURE, SemaineML.namespace);
				interpretation.setAttribute(EMMA.START, String.valueOf(fm.getUsertime()));
				feature.setAttribute(SemaineML.NAME, fm.getFeatureNames()[0]);
				feature.setAttribute(SemaineML.VALUE, String.valueOf(features[0]));
				// Now send it
				userStateSender.sendXML(document, fm.getUsertime(), Event.single);
			}
		} else if (m instanceof SEMAINEXMLMessage) {
			SEMAINEXMLMessage sm = (SEMAINEXMLMessage) m;
			// extract dialog state and remember it
			Document doc = sm.getDocument();
			// The only thing we want to know is if the user is speaking
			Element speaker = XMLTool.getChildElementByTagNameNS(doc.getDocumentElement(), SemaineML.SPEAKER, SemaineML.namespace);
			if (speaker.getAttribute(SemaineML.WHO).equals("user")) 
				userIsSpeaking = true;
			else
				userIsSpeaking = false;
		} else {
			throw new MessageFormatException("unexpected message type: "+m.getClass().getSimpleName());
		}
	}
}
