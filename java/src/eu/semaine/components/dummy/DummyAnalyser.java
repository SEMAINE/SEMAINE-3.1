/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.dummy;

import javax.jms.JMSException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.datatypes.stateinfo.StateInfo;
import eu.semaine.datatypes.xml.EMMA;
import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.jms.IOBase.Event;
import eu.semaine.jms.message.SEMAINEFeatureMessage;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.message.SEMAINEStateMessage;
import eu.semaine.jms.receiver.FeatureReceiver;
import eu.semaine.jms.receiver.StateReceiver;
import eu.semaine.jms.sender.EmmaSender;
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
		dialogStateReceiver = new StateReceiver("semaine.data.state.dialog", StateInfo.Type.DialogState);
		receivers.add(dialogStateReceiver);
		userStateSender = new EmmaSender("semaine.data.state.user", getName());
		senders.add(userStateSender); // so it can be started etc

	}

	public void react(SEMAINEMessage m) throws JMSException
	{
		if (m instanceof SEMAINEFeatureMessage) {
			SEMAINEFeatureMessage fm = (SEMAINEFeatureMessage) m;
			float[] features = fm.getFeatureVector();
			//log.debug("received feature vector with "+features.length+" features, "+(System.currentTimeMillis()-fm.getUsertime())+" ms after user time");

			if (features[0] < 0.002 && userIsSpeaking) { // some arbitrary condition
				// Simulate a turn-yielding event
				// Create and fill a simple EMMA document
				Document document = XMLTool.newDocument(EMMA.E_EMMA, EMMA.namespaceURI, EMMA.version);
				Element interpretation = XMLTool.appendChildElement(document.getDocumentElement(), EMMA.E_INTERPRETATION);
				Element behaviour = XMLTool.appendChildElement(interpretation, SemaineML.E_BEHAVIOUR, SemaineML.namespaceURI);
				interpretation.setAttribute(EMMA.A_START, String.valueOf(fm.getUsertime()));
				behaviour.setAttribute(SemaineML.A_NAME, "gaze-away");
				behaviour.setAttribute(SemaineML.A_INTENSITY, "0.9");
				// Now send it
				userStateSender.sendXML(document, fm.getUsertime(), Event.single);
			} else if (features[0] > 0.998 && !userIsSpeaking) {
				// Simulate a turn-request signal
				// Create and fill a simple EMMA document
				Document document = XMLTool.newDocument(EMMA.E_EMMA, EMMA.namespaceURI, EMMA.version);
				Element interpretation = XMLTool.appendChildElement(document.getDocumentElement(), EMMA.E_INTERPRETATION);
				Element behaviour = XMLTool.appendChildElement(interpretation, SemaineML.E_BEHAVIOUR, SemaineML.namespaceURI);
				interpretation.setAttribute(EMMA.A_START, String.valueOf(fm.getUsertime()));
				behaviour.setAttribute(SemaineML.A_NAME, "gaze-at-agent");
				behaviour.setAttribute(SemaineML.A_INTENSITY, "0.9");
				// Now send it
				userStateSender.sendXML(document, fm.getUsertime(), Event.single);
			}
		} else if (m instanceof SEMAINEStateMessage) {
			SEMAINEStateMessage sm = (SEMAINEStateMessage) m;
			if (sm.getState().getType() != StateInfo.Type.DialogState) {
				throw new MessageFormatException("expected state info of type DialogState, but got "+sm.getState().getType().toString());
			}
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
