/**
 * Copyright (C) 2010 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.testing;

import java.util.HashMap;
import java.util.List;
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
	private static final String AGENT_UTTERANCE_START_TIME = "agentUtteranceStartTime";
	private static final String AGENT_UTTERANCE = "agentUtterance";
	private static final String AGENT_TORSO_START_TIME = "agentTorsoStartTime";
	private static final String AGENT_TORSO = "agentTorso";
	private static final String AGENT_GESTURE_START_TIME = "agentGestureStartTime";
	private static final String AGENT_GESTURE = "agentGesture";
	private static final String AGENT_GAZE_START_TIME = "agentGazeStartTime";
	private static final String AGENT_GAZE = "agentGaze";
	private static final String AGENT_FACE_START_TIME = "agentFaceStartTime";
	private static final String AGENT_FACE = "agentFace";
	private static final String AGENT_HEAD_START_TIME = "agentHeadStartTime";
	private static final String AGENT_HEAD = "agentHead";
	private static final String directBehaviourTopic = "semaine.data.synthesis.plan";
	private static final String prepareBehaviourTopic = "semaine.data.synthesis.prepare";
	private static final String callbackTopic = "semaine.callback.output.Animation"; 
	private static final String agentStateTopic = "semaine.data.state.agent";
	
	private StateSender agentStateSender;
	private Map<String, AgentBehaviour> behaviours = new HashMap<String, AgentBehaviour>();

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
	protected void customStartIO() {
		verifyStatesExist();
	}

	private void verifyStatesExist() throws IllegalArgumentException {
		Map<String, String> testMap = new HashMap<String, String>();
		for (String s : new String[] {
				AGENT_FACE, AGENT_FACE_START_TIME, AGENT_GAZE, AGENT_GAZE_START_TIME, AGENT_GESTURE, AGENT_GESTURE_START_TIME, AGENT_HEAD, AGENT_HEAD_START_TIME,
				AGENT_TORSO, AGENT_TORSO_START_TIME, AGENT_UTTERANCE, AGENT_UTTERANCE_START_TIME
				}) {
			testMap.put(s, "dummy");
		}
		// The following will throw an IllegalArgumentException if one of the keys is not defined in stateinfo.config:
		new AgentStateInfo(testMap);
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
			AgentBehaviour beh = behaviours.get(id);
			if (beh == null) {
				beh = new AgentBehaviour();
			}
			// Does it contain a speech element?
			Element speech = XMLTool.getChildElementByLocalNameNS(bml, BML.E_SPEECH, BML.namespaceURI);
			if (speech != null) {
				String text = speech.getAttribute("text");
				if (!text.equals("")) {
					beh.text = text;
				}
			}
			// Any non-verbal expressions?
			beh.head = extractGretaBehaviour(bml, BML.E_HEAD);
			beh.face = extractGretaBehaviour(bml, BML.E_FACE);
			beh.gaze = extractGretaBehaviour(bml, BML.E_GAZE);
			beh.gesture = extractGretaBehaviour(bml, BML.E_GESTURE);
			beh.torso = extractGretaBehaviour(bml, "torso");
			// Anything?
			if (beh.text != null || beh.head != null || beh.face != null || beh.gaze != null
					|| beh.gesture != null || beh.torso != null) {
				behaviours.put(id, beh);
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
			AgentBehaviour beh = behaviours.get(id);
			if (beh == null) { // not a relevant id
				return;
			}
			String timeString = event.getAttribute(SemaineML.A_TIME);
			long time = Long.parseLong(timeString);
			// We are only interested in start and end messages:
			String type = event.getAttribute(SemaineML.A_TYPE);
			if (type.equals(SemaineML.V_START)) {
				updateAgentState(time, beh, true);
				currentID = id;
			} else if (type.equals(SemaineML.V_END)) {
				if (id.equals(currentID)) {
					updateAgentState(time, beh, false);
					currentID = null;
					behaviours.remove(id);
				}
			}

		}
	}

	/**
	 * Extract behaviour markup from a BML document in Greta-specific notation,
	 * and combine them such that all behaviours below the given behaviour element name
	 * are combined in a single string, separated by spaces.
	 * 
	 * Expected format is direct children of &lt;bml&gt;, in the BML namespace, such as:
	 * <pre>
	 * <head id="hd1"  start="0.0" end="4">
	 *   <description level="1" type="gretabml">
	 *     <reference>head=head_nod</reference>
	 *   </description>
	 * </head>
	 * </pre>
	 * @param bml
	 * @param behaviourElementName
	 * @return a string containing the behaviour annotations, or null if there are no such annotations.
	 */
	private String extractGretaBehaviour(Element bml, String behaviourElementName) {
		StringBuilder sb = new StringBuilder();
		List<Element> behaviourElements = XMLTool.getChildrenByLocalNameNS(bml, behaviourElementName, BML.namespaceURI);
		for (Element e : behaviourElements) {
			Element description = XMLTool.getChildElementByLocalNameNS(e, BML.E_DESCRIPTION, BML.namespaceURI);
			if (description != null) {
				Element reference = XMLTool.getChildElementByLocalNameNS(description, "reference", BML.namespaceURI);
				if (reference != null) {
					String behaviour = reference.getTextContent();
					if (sb.length() > 0) {
						sb.append(" ");
					}
					sb.append(behaviour);
				}
			}
		}
		if (sb.length() == 0) {
			return null;
		}
		return sb.toString();

	}
	
	private void updateAgentState(long startTime, AgentBehaviour behaviour, boolean isStart) throws JMSException {
		Map<String, String> infos = new HashMap<String, String>();
		String startTimeString = String.valueOf(startTime);
		if (behaviour.text != null) {
			infos.put(AGENT_UTTERANCE, isStart ? behaviour.text : "");
			infos.put(AGENT_UTTERANCE_START_TIME, startTimeString);
		}
		if (behaviour.head != null) {
			infos.put(AGENT_HEAD, isStart ? behaviour.head : "");
			infos.put(AGENT_HEAD_START_TIME, startTimeString);
		}
		if (behaviour.face != null) {
			infos.put(AGENT_FACE, isStart ? behaviour.face : "");
			infos.put(AGENT_FACE_START_TIME, startTimeString);
		}
		if (behaviour.gaze != null) {
			infos.put(AGENT_GAZE, isStart ? behaviour.gaze : "");
			infos.put(AGENT_GAZE_START_TIME, startTimeString);
		}
		if (behaviour.gesture != null) {
			infos.put(AGENT_GESTURE, isStart ? behaviour.gesture : "");
			infos.put(AGENT_GESTURE_START_TIME, startTimeString);
		}
		if (behaviour.torso != null) {
			infos.put(AGENT_TORSO, isStart ? behaviour.torso : "");
			infos.put(AGENT_TORSO_START_TIME, startTimeString);
		}
		if (!infos.isEmpty()) {
			agentStateSender.sendStateInfo(new AgentStateInfo(infos), meta.getTime());
		}
	}
	
	
	private static class AgentBehaviour {
		String text;
		String head;
		String face;
		String gaze;
		String gesture;
		String torso;
	}
}
