/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.dummy;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Random;

import javax.jms.JMSException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.datatypes.xml.BML;
import eu.semaine.datatypes.xml.FML;
import eu.semaine.jms.receiver.AgentStateReceiver;
import eu.semaine.jms.receiver.DialogStateReceiver;
import eu.semaine.jms.receiver.UserStateReceiver;
import eu.semaine.jms.sender.FMLSender;
import eu.semaine.util.XMLTool;

/**
 * This dummy class mimics an action proposer generating FML actions.
 * 
 * @author marc
 *
 */
public class DummyFMLActionProposer extends Component 
{
	private UserStateReceiver userStateReceiver;
	private AgentStateReceiver agentStateReceiver;
	private DialogStateReceiver dialogStateReceiver;
	private FMLSender fmlSender;
	
	
	private long lastTimeISaidSomething;
	private Map<String,List<String>>emotionalUtterances;
	
	/**
	 * @param componentName
	 * @throws JMSException
	 */
	public DummyFMLActionProposer() throws JMSException 
	{
		super("DummyFMLActionProposer");
		userStateReceiver = new UserStateReceiver("semaine.data.state.user", "datatype = 'UserState'");
		receivers.add(userStateReceiver);
		agentStateReceiver = new AgentStateReceiver("semaine.data.state.agent");
		receivers.add(agentStateReceiver);
		dialogStateReceiver = new DialogStateReceiver("semaine.data.state.dialog");
		receivers.add(dialogStateReceiver);
		
		fmlSender = new FMLSender("semaine.data.action.candidate.function", getName());
		senders.add(fmlSender); // so it can be started etc
		
		emotionalUtterances = new HashMap<String, List<String>>();
		emotionalUtterances.put("positive-active", new ArrayList<String>(Arrays.asList(new String[] {
			"What a nice day!",
			"Do you like swimming?",
			"How would you feel about doing something really cool today?"
		})));
		emotionalUtterances.put("positive-passive", new ArrayList<String>(Arrays.asList(new String[] {
				"Calm and nice, that's how I like it.",
				"Isn't it amazing, just to sit and relax...",
				"Being reasonable is really the best choice, isn't it?"
			})));
		emotionalUtterances.put("negative-passive", new ArrayList<String>(Arrays.asList(new String[] {
				"This boredom really kills me.",
				"It's like being stuck in a swamp, you just can't get out.",
				"You knew it wasn't worth getting up in the morning, didn't you?"
			})));
		emotionalUtterances.put("negative-active", new ArrayList<String>(Arrays.asList(new String[] {
				"It makes you so angry, you could tear it all down!",
				"It's enraging, just so enraging!",
				"Someone should show them their limits, this behaviour is unacceptable."
			})));
		emotionalUtterances.put(null, new ArrayList<String>(Arrays.asList(new String[] {
				"I don't quite know how I am feeling today.",
				"Maybe I should read the newspaper and find out how I feel today.",
				"Strange this dumb impression of not knowing how you feel..."
			})));
		
	}
	
	@Override
	public void act() throws JMSException
	{
		// As a speaker, say something every 4 seconds.
		long time = meta.getTime();
		if (time - lastTimeISaidSomething > 4000 &&
				"agent".equals(dialogStateReceiver.getCurrentBestGuess("speaker"))) {
			String emotion = agentStateReceiver.getCurrentBestGuess("emotion-quadrant");
			List<String> utterancesAvailable = emotionalUtterances.get(emotion);
			assert utterancesAvailable != null;
			int i = new Random().nextInt(utterancesAvailable.size());
			String utterance = utterancesAvailable.get(i);
			// Now construct an FML document around it
			Document doc = XMLTool.newDocument("fml-apml", null, FML.version);
			Element root = doc.getDocumentElement();
			Element bml = XMLTool.appendChildElement(root, BML.BML, BML.namespace);
			bml.setAttribute(BML.ID, "bml1");
			Element fml = XMLTool.appendChildElement(root, FML.FML, FML.namespace);
			fml.setAttribute(FML.ID, "fml1");
			Element speech = XMLTool.appendChildElement(bml, BML.SPEECH);
			speech.setAttribute(BML.ID, "s1");
			Element text = XMLTool.appendChildElement(speech, BML.TEXT);
			text.setTextContent(utterance);
			fmlSender.sendXML(doc, time);
			lastTimeISaidSomething = time;
		}
	}
}
