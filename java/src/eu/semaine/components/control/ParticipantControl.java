/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.control;

import eu.semaine.components.control.ParticipantControlGUI;
import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.message.SEMAINEXMLMessage;
import eu.semaine.jms.receiver.XMLReceiver;
import java.util.List;
import java.util.Random;

import javax.jms.JMSException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.jms.sender.FeatureSender;
import eu.semaine.jms.sender.XMLSender;
import eu.semaine.util.XMLTool;

/**
 * @author marc
 *
 */
public class ParticipantControl extends Component 
{

    private ParticipantControlGUI gui;
	private XMLSender contextSender;
        private boolean isUserPresent = false;
        private String currentCharacter = ParticipantControlGUI.PRUDENCE;
	/**
	 * @param componentName
	 * @throws JMSException
	 */
	public ParticipantControl() throws JMSException 
	{
		super("ParticipantControlGUI", true, false);
		contextSender = new XMLSender("semaine.data.state.context", "SemaineML", getName());
		senders.add(contextSender); // so it can be started etc
                receivers.add(new XMLReceiver("semaine.data.state.context"));
                gui = new ParticipantControlGUI(this);
                gui.setVisible(true);
	}

        @Override
        protected void react(SEMAINEMessage m)
        throws Exception
        {
            SEMAINEXMLMessage xm = (SEMAINEXMLMessage) m;
            Document doc = xm.getDocument();
            Element root = doc.getDocumentElement();
            if (!root.getTagName().equals(SemaineML.E_CONTEXT)) {
                throw new MessageFormatException("Unexpected document element: expected tag name '"+SemaineML.E_CONTEXT+"', found '"+root.getTagName()+"'");
            }
            if (!root.getNamespaceURI().equals(SemaineML.namespaceURI)) {
                throw new MessageFormatException("Unexpected document element namespace: expected '"+SemaineML.namespaceURI+"', found '"+root.getNamespaceURI()+"'");
            }
            Element user = XMLTool.getChildElementByTagNameNS(root, SemaineML.E_USER, SemaineML.namespaceURI);
            if (user != null) {
            	isUserPresent = user.getAttribute(SemaineML.A_STATUS).equals(SemaineML.V_PRESENT);
            }
            Element character = XMLTool.getChildElementByTagNameNS(root, SemaineML.E_CHARACTER, SemaineML.namespaceURI);
            if (character != null) {
            	currentCharacter = character.getAttribute(SemaineML.A_NAME);
            }
            gui.updateWhoIsPresent();
        }

	private void sendWhoIsPresent()
	throws JMSException
	{
		Document semaineML = XMLTool.newDocument(SemaineML.E_CONTEXT, SemaineML.namespaceURI, SemaineML.version);
		Element rootNode = semaineML.getDocumentElement();
		Element user = XMLTool.appendChildElement(rootNode, SemaineML.E_USER);
		String status = (isUserPresent ? SemaineML.V_PRESENT : SemaineML.V_ABSENT);
		user.setAttribute(SemaineML.A_STATUS, status);
		if (currentCharacter != null) {
			Element character = XMLTool.appendChildElement(rootNode, SemaineML.E_CHARACTER);
			character.setAttribute(SemaineML.A_NAME, currentCharacter);
		}
		contextSender.sendXML(semaineML, meta.getTime());
	}

    public boolean isUserPresent()
    {
        return isUserPresent;
    }
    
    public void setUserPresent(boolean userPresent)
    {
        this.isUserPresent = userPresent;
        try {
            sendWhoIsPresent();
        } catch (JMSException e) {
            e.printStackTrace();
        }
    }
    
    public String getCurrentCharacter()
    {
        return currentCharacter;
    }
    
    public void setCurrentCharacter(String name)
    {
        this.currentCharacter = name;
        try {
            sendWhoIsPresent();
        } catch (JMSException e) {
            e.printStackTrace();
        }
    }
}
