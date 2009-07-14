package eu.semaine.components.dialogue.interpreters;

import java.util.List;

import javax.jms.JMSException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

import eu.semaine.components.Component;
import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.jms.message.SEMAINEEmmaMessage;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.receiver.EmmaReceiver;
import eu.semaine.jms.sender.XMLSender;
import eu.semaine.util.XMLTool;

public class HeadMovementInterpreter extends Component
{
	/* Senders and Receivers */
	private EmmaReceiver emmaReceiver;
	private XMLSender userStateSender;
	
	public HeadMovementInterpreter() throws JMSException
	{
		super( "HeadMovementInterpreter" );
		
		userStateSender = new XMLSender("semaine.data.state.user.behaviour", "datatype = 'UserState'", "");
		senders.add(userStateSender); // so it can be started etc
		
		emmaReceiver = new EmmaReceiver("semaine.data.state.user.emma", "datatype = 'EMMA'");
		receivers.add(emmaReceiver);
	}
	
	public void react( SEMAINEMessage m ) throws JMSException
	{
		if( nodDetected(m) ) {
			processNod();
		} else if( shakeDetected(m) ) {
			processShake();
		}
	}
	
	/**
	 * Checks the message for Nods
	 * TODO: Implement
	 * @param m
	 * @return
	 */
	public boolean nodDetected( SEMAINEMessage m ) throws JMSException
	{
		String sentence = getSentence(m);
		if( sentence != null && sentence.toLowerCase().contains("*nod*") ) {
			return true;
		} else { 
			return false;
		}
	}
	
	/**
	 * Checks the message for Shakes
	 * TODO: Implement
	 * @param m
	 * @return
	 */
	public boolean shakeDetected( SEMAINEMessage m ) throws JMSException
	{
		String sentence = getSentence(m);
		if( sentence != null && sentence.toLowerCase().contains("*shake*") ) {
			return true;
		} else { 
			return false;
		}
	}
	
	/**
	 * Returns the detected sentence in the SEMAINEMessage m
	 * @param m the message to detect the sentence in
	 * TODO: TEMPORARY, only to simulate nods and shakes by typing them
	 * @return the detected sentence
	 */
	public String getSentence( SEMAINEMessage m ) throws JMSException
	{
		if( m instanceof SEMAINEEmmaMessage ) {
			SEMAINEEmmaMessage em = (SEMAINEEmmaMessage)m;
			Element interpretation = em.getTopLevelInterpretation();
			if (interpretation != null) {
				List<Element> texts = em.getTextElements(interpretation);
				for (Element text : texts) {
					
					String utterance = text.getTextContent();
					if( utterance != null ) {
						return utterance;
					}
					/*
					if( text.getAttribute("name") != null ) {
						return text.getAttribute("name");
					}
					*/
				}
			}
		}
		return null;
	}
	
	/**
	 * Processes the appearance of a nod
	 * TODO: Extend
	 * Currently only sends 'agree' as user behaviour
	 */
	public void processNod() throws JMSException
	{
		Document document = XMLTool.newDocument(SemaineML.E_USERSTATE, SemaineML.namespaceURI, SemaineML.version);
		Element text = XMLTool.appendChildElement(document.getDocumentElement(), SemaineML.E_TEXT);
		text.setAttribute(SemaineML.A_TIME, String.valueOf(meta.getTime()));
		Node textNode = document.createTextNode("");
		text.appendChild(textNode);
		
		Element feature = XMLTool.appendChildElement(text, "feature", SemaineML.namespaceURI);
		feature.setAttribute("name", "agree");
		
		userStateSender.sendXML(document, meta.getTime());
	}
	
	/**
	 * Processes the appearance of a shake
	 * TODO: Extend
	 * Currently only sends 'disagree' as user behaviour
	 */
	public void processShake() throws JMSException
	{
		Document document = XMLTool.newDocument(SemaineML.E_USERSTATE, SemaineML.namespaceURI, SemaineML.version);
		Element text = XMLTool.appendChildElement(document.getDocumentElement(), SemaineML.E_TEXT);
		text.setAttribute(SemaineML.A_TIME, String.valueOf(meta.getTime()));
		Node textNode = document.createTextNode("");
		text.appendChild(textNode);
		
		Element feature = XMLTool.appendChildElement(text, "feature", SemaineML.namespaceURI);
		feature.setAttribute("name", "disagree");
		
		userStateSender.sendXML(document, meta.getTime());
	}
}
