/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.dummy;


import javax.jms.JMSException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.datatypes.BML;
import eu.semaine.datatypes.DialogStateInfo;
import eu.semaine.datatypes.SEMAINEDialogStateMessage;
import eu.semaine.datatypes.SEMAINEEmmaMessage;
import eu.semaine.datatypes.SEMAINEFeatureMessage;
import eu.semaine.datatypes.SEMAINEMessage;
import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.jms.BMLSender;
import eu.semaine.jms.DialogStateReceiver;
import eu.semaine.jms.EmmaReceiver;
import eu.semaine.jms.FeatureReceiver;
import eu.semaine.jms.IOBase.Event;
import eu.semaine.util.XMLTool;

/**
 * This dummy class mimics an action proposer generating behaviour directly.
 * 
 * @author marc
 *
 */
public class DummyBMLActionProposer extends Component 
{
	private FeatureReceiver featureReceiver;
	private EmmaReceiver emmaReceiver;
	private DialogStateReceiver dialogStateReceiver;
	private BMLSender bmlSender;
	
	

	private long lastTimeINodded = 0;
	private boolean smiling = false;
	
	
	/**
	 * @param componentName
	 * @throws JMSException
	 */
	public DummyBMLActionProposer() throws JMSException 
	{
		super("DummyBMLActionProposer");
		featureReceiver = new FeatureReceiver("semaine.data.analysis.>");
		receivers.add(featureReceiver); // to set up properly
		emmaReceiver = new EmmaReceiver("semaine.data.state.user", "datatype = 'EMMA'");
		receivers.add(emmaReceiver);
		dialogStateReceiver = new DialogStateReceiver("semaine.data.state.dialog");
		receivers.add(dialogStateReceiver);
		
		bmlSender = new BMLSender("semaine.data.action.candidate.behaviour", getName());
		senders.add(bmlSender); // so it can be started etc
	}
	
	@Override
	public void act() throws JMSException
	{
		// When in the listener role, we nod every second
		long time = System.currentTimeMillis();
		if (time - lastTimeINodded > 1000 &&
				"user".equals(dialogStateReceiver.getCurrentBestGuess("speaker"))) {
			Document doc = XMLTool.newDocument(BML.BML, BML.namespace, BML.version);
			Element root = doc.getDocumentElement();
			Element head = XMLTool.appendChildElement(root, BML.HEAD);
			head.setAttribute(BML.TYPE, "NOD");
			bmlSender.sendXML(doc, time);
			lastTimeINodded = time;
		}
	}

	@Override
	public void react(SEMAINEMessage m) throws JMSException
	{
		// in reaction to some feature, start or stop smiling
		if (m instanceof SEMAINEFeatureMessage) {
			SEMAINEFeatureMessage fm = (SEMAINEFeatureMessage) m;
			float[] features = fm.getFeatureVector();
			if (features[2] < 0.001 && !smiling) { // some arbitrary condition
				// start smiling
				Document doc = XMLTool.newDocument(BML.BML, BML.namespace, BML.version);
				Element root = doc.getDocumentElement();
				Element face = XMLTool.appendChildElement(root, BML.FACE);
				face.setAttribute(BML.TYPE, "smile");
				bmlSender.sendXML(doc, System.currentTimeMillis(), Event.start);
				smiling = true;
			} else if (features[2] > 0.99 && smiling) { // arbitrary
				// stop smiling
				Document doc = XMLTool.newDocument(BML.BML, BML.namespace, BML.version);
				Element root = doc.getDocumentElement();
				Element face = XMLTool.appendChildElement(root, BML.FACE);
				face.setAttribute(BML.TYPE, "smile");
				bmlSender.sendXML(doc, System.currentTimeMillis(), Event.end);
				smiling = false;
			}
		} else if (m instanceof SEMAINEEmmaMessage) {
			SEMAINEEmmaMessage em = (SEMAINEEmmaMessage)m;
			// do nothing for now
		} else if (m instanceof SEMAINEDialogStateMessage) {
			DialogStateInfo dsi = ((SEMAINEDialogStateMessage)m).getState();
			// do nothing for now
		} else {
			throw new MessageFormatException("unexpected message type: "+m.getClass().getSimpleName());
		}
	}
}
