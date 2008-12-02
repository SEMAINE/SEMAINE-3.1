/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.dummy;


import javax.jms.JMSException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.datatypes.stateinfo.DialogStateInfo;
import eu.semaine.datatypes.xml.BML;
import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.jms.IOBase.Event;
import eu.semaine.jms.message.SEMAINEDialogStateMessage;
import eu.semaine.jms.message.SEMAINEEmmaMessage;
import eu.semaine.jms.message.SEMAINEFeatureMessage;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.receiver.DialogStateReceiver;
import eu.semaine.jms.receiver.EmmaReceiver;
import eu.semaine.jms.receiver.FeatureReceiver;
import eu.semaine.jms.sender.BMLSender;
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
		long time = meta.getTime();
		if (time - lastTimeINodded > 1000 &&
				"user".equals(dialogStateReceiver.getCurrentBestGuess("speaker"))) {
			Document doc = XMLTool.newDocument(BML.E_BML, BML.namespaceURI, BML.version);
			Element root = doc.getDocumentElement();
			Element head = XMLTool.appendChildElement(root, BML.E_HEAD);
			head.setAttribute(BML.A_TYPE, "NOD");
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
				Document doc = XMLTool.newDocument(BML.E_BML, BML.namespaceURI, BML.version);
				Element root = doc.getDocumentElement();
				Element face = XMLTool.appendChildElement(root, BML.E_FACE);
				face.setAttribute(BML.A_TYPE, "smile");
				bmlSender.sendXML(doc, meta.getTime(), Event.start);
				smiling = true;
			} else if (features[2] > 0.99 && smiling) { // arbitrary
				// stop smiling
				Document doc = XMLTool.newDocument(BML.E_BML, BML.namespaceURI, BML.version);
				Element root = doc.getDocumentElement();
				Element face = XMLTool.appendChildElement(root, BML.E_FACE);
				face.setAttribute(BML.A_TYPE, "smile");
				bmlSender.sendXML(doc, meta.getTime(), Event.end);
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
