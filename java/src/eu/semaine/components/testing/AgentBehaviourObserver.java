/**
 * Copyright (C) 2010 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.testing;

import java.util.HashMap;
import java.util.Map;

import javax.jms.JMSException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.datatypes.stateinfo.AgentStateInfo;
import eu.semaine.datatypes.stateinfo.StateInfo;
import eu.semaine.datatypes.xml.BML;
import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.message.SEMAINEXMLMessage;
import eu.semaine.jms.receiver.BMLReceiver;
import eu.semaine.jms.receiver.XMLReceiver;
import eu.semaine.jms.sender.StateSender;
import eu.semaine.util.XMLTool;

/**
 * This class observes the verbal and nonverbal agent behaviour, and updates the agent state accordingly.
 * @author marc
 *
 */
public class AgentBehaviourObserver extends Component {
	private static final String directBehaviourTopic = "semaine.data.synthesis.plan";
	private static final String prepareBehaviourTopic = "semaine.data.synthesis.prepare";
	private static final String callbackTopic = "semaine.callback.output.Animation"; 
	private static final String agentStateTopic = "semaine.data.state.agent";
	
	private StateSender agentStateSender;
	private Map<String, String> texts = new HashMap<String, String>();
	private String currentID = null;
	
	/**
	 * @param componentName
	 * @throws JMSException
	 */
	public AgentBehaviourObserver() throws JMSException {
		super("AgentObserver");
		receivers.add(new BMLReceiver(directBehaviourTopic));
		receivers.add(new BMLReceiver(prepareBehaviourTopic));
		receivers.add(new XMLReceiver(callbackTopic));
		agentStateSender = new StateSender(agentStateTopic, StateInfo.Type.AgentState, getName());
		senders.add(agentStateSender);
	}

	
	@Override
	protected void react(SEMAINEMessage m) throws JMSException {
		if (!(m instanceof SEMAINEXMLMessage)) {
			return;
		}
		SEMAINEXMLMessage xm = (SEMAINEXMLMessage) m;
		Document doc = xm.getDocument();
		// Messages from behaviour topics tell us what is being said/done:
		if (xm.getTopicName().equals(directBehaviourTopic)
				|| xm.getTopicName().equals(prepareBehaviourTopic)) {
			// expect BML
			String id = xm.getContentID();
			Element bml = doc.getDocumentElement();
			if (!bml.getLocalName().equals(BML.E_BML) || !bml.getNamespaceURI().equals(BML.namespaceURI)) {
				return;
			}
			// Does it contain a speech element?
			Element speech = XMLTool.getChildElementByLocalNameNS(bml, BML.E_SPEECH, BML.namespaceURI);
			if (speech != null) {
				String text = speech.getAttribute("text");
				if (!text.equals("")) {
					texts.put(id, text);
				}
			}
		}
		// Messages from callback topic tell us when behaviours start/end:
		else if (xm.getTopicName().equals(callbackTopic)) {
			// expect a SemaineML <callback>
			Element callback = doc.getDocumentElement();
			if(!callback.getLocalName().equals(SemaineML.E_CALLBACK)
					|| !callback.getNamespaceURI().equals(SemaineML.namespaceURI)) {
				return;
			}
			Element event = XMLTool.getChildElementByLocalNameNS(callback, SemaineML.E_EVENT, SemaineML.namespaceURI);
			if (event == null) {
				return;
			}
			// Do we know the behaviour?
			String id = event.getAttribute(SemaineML.A_ID);
			if (!texts.containsKey(id)) { // not a relevant id
				return;
			}
			String timeString = event.getAttribute(SemaineML.A_TIME);
			long time = Long.parseLong(timeString);
			// We are only interested in start and end messages:
			String type = event.getAttribute(SemaineML.A_TYPE);
			if (type.equals(SemaineML.V_START)) {
				updateAgentState(time, texts.get(id));
				currentID = id;
			} else if (type.equals(SemaineML.V_END)) {
				if (id.equals(currentID)) {
					updateAgentState(time, "");
					currentID = null;
				}
			}

		}
	}

	private void updateAgentState(long startTime, String agentUtterance) throws JMSException {
		Map<String, String> infos = new HashMap<String, String>();
		infos.put("agentUtterance", agentUtterance);
		infos.put("agentUtteranceStartTime", String.valueOf(startTime));
		agentStateSender.sendStateInfo(new AgentStateInfo(infos), meta.getTime());
	}
}
