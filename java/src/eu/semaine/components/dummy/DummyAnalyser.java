/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.dummy;

import javax.jms.JMSException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.datatypes.EMMA;
import eu.semaine.datatypes.SEMAINEDialogStateMessage;
import eu.semaine.datatypes.SEMAINEFeatureMessage;
import eu.semaine.datatypes.SEMAINEMessage;
import eu.semaine.datatypes.SemaineML;
import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.jms.EmmaSender;
import eu.semaine.jms.FeatureReceiver;
import eu.semaine.jms.UserStateReceiver;
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
	private UserStateReceiver dialogStateReceiver;
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
		dialogStateReceiver = new UserStateReceiver("semaine.data.state.dialog");
		receivers.add(dialogStateReceiver);
		userStateSender = new EmmaSender("semaine.data.state.user", getName());
		senders.add(userStateSender); // so it can be started etc

	}

	public void react(SEMAINEMessage m) throws JMSException
	{
		if (m instanceof SEMAINEFeatureMessage) {
			SEMAINEFeatureMessage fm = (SEMAINEFeatureMessage) m;
			float[] features = fm.getFeatureVector();
			log.debug("Received feature vector, features[0]="+features[0]);
			if (features[0] < 0.02 && userIsSpeaking) { // some arbitrary condition
				// Simulate a turn-yielding event
				// Create and fill a simple EMMA document
				Document document = XMLTool.newDocument(EMMA.EMMA, EMMA.namespace, EMMA.version);
				Element interpretation = XMLTool.appendChildElement(document.getDocumentElement(), EMMA.INTERPRETATION);
				Element behaviour = XMLTool.appendChildElement(interpretation, SemaineML.BEHAVIOUR, SemaineML.namespace);
				interpretation.setAttribute(EMMA.START, String.valueOf(fm.getUsertime()));
				behaviour.setAttribute(SemaineML.NAME, "gaze-away");
				behaviour.setAttribute(SemaineML.INTENSITY, "0.9");
				// Now send it
				userStateSender.sendXML(document, fm.getUsertime(), Event.single);
			} else if (features[0] > 0.98 && !userIsSpeaking) {
				// Simulate a turn-request signal
				// Create and fill a simple EMMA document
				Document document = XMLTool.newDocument(EMMA.EMMA, EMMA.namespace, EMMA.version);
				Element interpretation = XMLTool.appendChildElement(document.getDocumentElement(), EMMA.INTERPRETATION);
				Element behaviour = XMLTool.appendChildElement(interpretation, SemaineML.BEHAVIOUR, SemaineML.namespace);
				interpretation.setAttribute(EMMA.START, String.valueOf(fm.getUsertime()));
				behaviour.setAttribute(SemaineML.NAME, "gaze-at-agent");
				behaviour.setAttribute(SemaineML.INTENSITY, "0.9");
				// Now send it
				userStateSender.sendXML(document, fm.getUsertime(), Event.single);
			}
		} else if (m instanceof SEMAINEDialogStateMessage) {
			SEMAINEDialogStateMessage sm = (SEMAINEDialogStateMessage) m;
			String speaker = sm.getState().getInfo().get("speaker");
			if (speaker != null) {
				if (speaker.equals("user")) 
					userIsSpeaking = true;
				else
					userIsSpeaking = false;
			}
		} else {
			throw new MessageFormatException("unexpected message type: "+m.getClass().getSimpleName());
		}
	}
}
