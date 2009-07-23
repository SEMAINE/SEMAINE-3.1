/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.dummy;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.jms.JMSException;

import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.datatypes.stateinfo.AgentStateInfo;
import eu.semaine.datatypes.stateinfo.DialogStateInfo;
import eu.semaine.datatypes.stateinfo.StateInfo;
import eu.semaine.datatypes.stateinfo.UserStateInfo;
import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.jms.message.SEMAINEEmmaMessage;
import eu.semaine.jms.message.SEMAINEFeatureMessage;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.message.SEMAINEStateMessage;
import eu.semaine.jms.receiver.EmmaReceiver;
import eu.semaine.jms.receiver.FeatureReceiver;
import eu.semaine.jms.receiver.StateReceiver;
import eu.semaine.jms.sender.StateSender;
import eu.semaine.util.XMLTool;

/**
 * This dummy class mimics an interpreter. It reads features and all three kinds of states,
 * and writes all three kinds of states. Real interpreters are likely to only
 * read some of these inputs and only produce some of these outputs.
 * 
 * This dummy interpreter illustrates the API using the following aspects:
 * <ul>
 * <li>the method act() encodes two examples for actions triggered by a timeout
 * rather than by other messages:
 *   <ul>
 *     <li>without any reference to any message states, the agent emotion changes
 *         every ten seconds;</li>
 *     <li>every 10 ms, the agent interest is verified and updated if necessary,
 *         based on the current best guess values of the dialog state and agent state.</li>
 *   </ul>
 * </li>
 * <li>the method react() encodes two examples for action triggered by other messages:
 *   <ul>
 *     <li>if a feature exceeds a certain value, the interpretation of user interest changes;</li>
 *     <li>certain behaviours are interpreted as speaker changes.</li>
 *   </ul>
 * </li>
 * </ul>
 * 
 * 
 * @author marc
 *
 */
public class DummyInterpreter extends Component 
{
	private FeatureReceiver featureReceiver;
	private EmmaReceiver emmaReceiver;
	private StateReceiver userStateReceiver;
	private StateReceiver agentStateReceiver;
	private StateReceiver dialogStateReceiver;
	private StateSender userStateSender;
	private StateSender agentStateSender;
	private StateSender dialogStateSender;
	
	
	// for changing emotional state every ten seconds:
	private long lastEmotionChangeTime = 0;
	private int lastEmotionIndex = 0;
	private final String[] emotionalStates = new String[] {
			"positive-active", "positive-passive", "negative-passive", "negative-active"
	};
	private long lastInterestCheckedTime = 0;
	private String lastInterest = null;
	
	/**
	 * @param componentName
	 * @throws JMSException
	 */
	public DummyInterpreter() throws JMSException 
	{
		super("DummyInterpreter");
		featureReceiver = new FeatureReceiver("semaine.data.analysis.>");
		receivers.add(featureReceiver); // to set up properly
		emmaReceiver = new EmmaReceiver("semaine.data.state.user", "datatype = 'EMMA'");
		receivers.add(emmaReceiver);
		userStateReceiver = new StateReceiver("semaine.data.state.user", "datatype = 'UserState'", StateInfo.Type.UserState);
		receivers.add(userStateReceiver);
		agentStateReceiver = new StateReceiver("semaine.data.state.agent", StateInfo.Type.AgentState);
		receivers.add(agentStateReceiver);
		dialogStateReceiver = new StateReceiver("semaine.data.state.dialog", StateInfo.Type.DialogState);
		receivers.add(dialogStateReceiver);
		
		userStateSender = new StateSender("semaine.data.state.user", StateInfo.Type.UserState, getName());
		senders.add(userStateSender); // so it can be started etc
		agentStateSender = new StateSender("semaine.data.state.agent", StateInfo.Type.AgentState, getName());
		senders.add(agentStateSender); // so it can be started etc
		dialogStateSender = new StateSender("semaine.data.state.dialog", StateInfo.Type.DialogState, getName());
		senders.add(dialogStateSender); // so it can be started etc

	}
	
	@Override
	public void act() throws JMSException
	{
		long time = meta.getTime();
		// Example for a change with no relation to any messages:
		// every ten seconds, the agent's emotional state changes
		if (time - lastEmotionChangeTime > 10000) {
			lastEmotionIndex++;
			if (lastEmotionIndex >= emotionalStates.length) lastEmotionIndex = 0;
			Map<String,String>agentInfo = new HashMap<String,String>();
			agentInfo.put("emotion-quadrant", emotionalStates[lastEmotionIndex]);
			AgentStateInfo asi = new AgentStateInfo(agentInfo);
			agentStateSender.sendStateInfo(asi, time);
			lastEmotionChangeTime = time;
		}
		// Example showing access to the StateReceivers' current best guess values:
		// if the user is talking and the agent is in a negative emotion,
		// the agent is uninterested, else interested
		if (time - lastInterestCheckedTime > 10) { // check interest every 10 ms only
			String speaker = dialogStateReceiver.getCurrentBestGuess("speaker");
			String agentEmotion = agentStateReceiver.getCurrentBestGuess("emotion-quadrant");
			String agentInterest;
			if (speaker != null && speaker.equals("user") &&
					agentEmotion != null && agentEmotion.startsWith("negative")) {
				agentInterest = "uninterested";
			} else {
				agentInterest = "interested";
			}
			if (!agentInterest.equals(lastInterest)) {
				Map<String,String> agentInfo = new HashMap<String,String>();
				agentInfo.put("interest", agentInterest);
				agentStateSender.sendStateInfo(new AgentStateInfo(agentInfo), time);
				lastInterest = agentInterest;
			}
			lastInterestCheckedTime = time;
		}
	}

	@Override
	public void react(SEMAINEMessage m) throws JMSException
	{
		if (m instanceof SEMAINEFeatureMessage) {
			SEMAINEFeatureMessage fm = (SEMAINEFeatureMessage) m;
			float[] features = fm.getFeatureVector();
			if (features[1] < 0.02) { // some arbitrary condition
				// assume user interest is low
				Map<String,String>userInfo = new HashMap<String,String>();
				userInfo.put("interest", "uninterested");
				UserStateInfo usi = new UserStateInfo(userInfo);
				userStateSender.sendStateInfo(usi, fm.getUsertime());
			} else if (features[1] > 0.98) { // arbitrary
				// assume user interest is high
				Map<String,String>userInfo = new HashMap<String,String>();
				userInfo.put("interest", "interested");
				UserStateInfo usi = new UserStateInfo(userInfo);
				userStateSender.sendStateInfo(usi, fm.getUsertime());
			}
		} else if (m instanceof SEMAINEEmmaMessage) {
			SEMAINEEmmaMessage em = (SEMAINEEmmaMessage)m;
			Element interpretation = em.getTopLevelInterpretation();
			if (interpretation != null) {
				List<Element> behaviours = em.getBehaviourElements(interpretation);
				for (Element behaviour : behaviours) {
					String fName = XMLTool.needAttribute(behaviour, SemaineML.A_NAME);
					//String fIntensity = XMLTool.needAttribute(behaviour, SemaineML.INTENSITY);
					if (fName.equals("gaze-away")) {
						// when user gazes away, the agent becomes the speaker
						Map<String,String>dialogInfo = new HashMap<String,String>();
						dialogInfo.put("speaker", "agent");
						DialogStateInfo dsi = new DialogStateInfo(dialogInfo, null);
						dialogStateSender.sendStateInfo(dsi, em.getUsertime());
					} else if (fName.equals("gaze-at-agent")) {
						// when user gazes at agent, user becomes the speaker
						Map<String,String>dialogInfo = new HashMap<String,String>();
						dialogInfo.put("speaker", "user");
						DialogStateInfo dsi = new DialogStateInfo(dialogInfo, null);
						dialogStateSender.sendStateInfo(dsi, em.getUsertime());
					}
				}
			}
		} else if (m instanceof SEMAINEStateMessage) {
			StateInfo si = ((SEMAINEStateMessage)m).getState();
			switch (si.getType()) {
			case AgentState:
			case DialogState:
			case UserState:
				// do nothing for now
				break;
		    default:
		    	throw new MessageFormatException("Unexpected state info type: "+si.getType().toString());
			}
		} else {
			throw new MessageFormatException("unexpected message type: "+m.getClass().getSimpleName());
		}
	}
}
