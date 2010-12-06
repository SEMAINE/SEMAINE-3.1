/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.dummy;

import javax.jms.JMSException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.datatypes.xml.BML;
import eu.semaine.datatypes.xml.FML;
import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.message.SEMAINEXMLMessage;
import eu.semaine.jms.receiver.FMLReceiver;
import eu.semaine.jms.sender.BMLSender;
import eu.semaine.util.XMLTool;

/**
 * This dummy action selection accepts all candidates.
 * 
 * @author marc
 *
 */
public class DummyVisualFML2BML extends Component 
{
	private FMLReceiver fmlReceiver;
	private BMLSender bmlSender;
	
	

	/**
	 * @param componentName
	 * @throws JMSException
	 */
	public DummyVisualFML2BML() throws JMSException 
	{
		this("DummyVisualFML2BML",
			"semaine.data.action.selected.speechpreprocessed",
			"semaine.data.synthesis.plan");
	}
	
	protected DummyVisualFML2BML(String compName,
			String fmlReceiverTopic,
			String bmlSenderTopic) throws JMSException {
		super(compName);
		
		fmlReceiver = new FMLReceiver(fmlReceiverTopic);
		receivers.add(fmlReceiver);
		
		bmlSender = new BMLSender(bmlSenderTopic, getName());
		senders.add(bmlSender);
	}

	@Override
	public void react(SEMAINEMessage m) throws JMSException
	{
		if (!(m instanceof SEMAINEXMLMessage)) {
			throw new MessageFormatException("expected XML message, got "+m.getClass().getSimpleName());
		}
		SEMAINEXMLMessage xm = (SEMAINEXMLMessage)m;
		boolean isFML = "FML".equals(xm.getDatatype());
		if (isFML) {
			// extract the BML and send that along
			if(XMLTool.getChildElementByLocalNameNS(xm.getDocument().getDocumentElement(), BML.E_BML, BML.namespaceURI) != null){
				Element bml = XMLTool.needChildElementByLocalNameNS(
						xm.getDocument().getDocumentElement(), BML.E_BML, BML.namespaceURI);
				Document doc = XMLTool.newDocument(BML.E_BML, BML.namespaceURI);
				doc.adoptNode(bml);
				doc.replaceChild(bml, doc.getDocumentElement());

				bmlSender.sendXML(doc, meta.getTime(), xm.getEventType(), xm.getContentID(), xm.getContentCreationTime(), xm.getContentType());
			}
			else{
				Element backchannel = null;
				Element fml = XMLTool.getChildElementByLocalNameNS(xm.getDocument().getDocumentElement(), FML.E_FML, FML.namespaceURI);
				if(fml != null){
					backchannel = XMLTool.getChildElementByLocalNameNS(fml, FML.E_BACKCHANNEL, FML.namespaceURI);
				}
				if(backchannel != null){
					Document doc = XMLTool.newDocument(BML.E_BML, BML.namespaceURI);
					Element root = doc.getDocumentElement();
					XMLTool.appendChildElement(root, BML.E_BACKCHANNEL, BML.namespaceURI);
					bmlSender.sendXML(doc, xm.getUsertime(), xm.getEventType(), xm.getContentID(), xm.getContentCreationTime(), xm.getContentType());
				}
				else{
					log.debug("Received fml document without bml or backchannel content -- ignoring.");
				}
			}
			
		} 
	}
	
}
