package eu.semaine.examples.swimmer;

import javax.jms.JMSException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.datatypes.xml.BML;
import eu.semaine.datatypes.xml.SSML;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.receiver.Receiver;
import eu.semaine.jms.sender.BMLSender;
import eu.semaine.util.XMLTool;

public class Commentator extends Component {
	private BMLSender bmlSender = new BMLSender("semaine.data.synthesis.plan", getName());
	private boolean started = false;
	private boolean inDanger = false;
	private boolean nearlyWon = false;
	
	public Commentator() throws JMSException {
		super("Commentator");
		receivers.add(new Receiver("semaine.data.swimmer.position"));
		senders.add(bmlSender);
	}

	@Override protected void react(SEMAINEMessage m) throws JMSException {
		float percent = Float.valueOf(m.getText());
		if (percent < 30) {
			if (!inDanger) say("Your swimmer needs help! Shout to wake him up!");
			inDanger = true;
		} else if (percent > 50) {
			inDanger = false;
		}
		if (percent > 70) {
			if (!nearlyWon) say("Just a little more.");
			nearlyWon = true;
		} else if (percent < 50) {
			nearlyWon = false;
		}
	}
	
	@Override protected void act() throws JMSException {
		if (!started) {
			started = true;
			say("Welcome to the swimmers game! The swimmer needs your support to reach the river bank. Cheer him up!");
		}
	}
	
	private void say(String text) throws JMSException {
		Document bml = XMLTool.newDocument(BML.ROOT_TAGNAME, BML.namespaceURI);
		Element speech = XMLTool.appendChildElement(bml.getDocumentElement(), BML.E_SPEECH);
		speech.setAttribute("language", "en-GB");
		speech.setTextContent(text);
		Element mark = XMLTool.appendChildElement(speech, SSML.E_MARK, SSML.namespaceURI);
		mark.setAttribute(SSML.A_NAME, "m1");
		bmlSender.sendXML(bml, meta.getTime());
	}
}
