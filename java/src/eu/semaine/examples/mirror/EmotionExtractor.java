package eu.semaine.examples.mirror;

import java.util.List;

import javax.jms.JMSException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.datatypes.xml.EmotionML;
import eu.semaine.jms.message.SEMAINEEmmaMessage;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.receiver.EmmaReceiver;
import eu.semaine.jms.sender.XMLSender;
import eu.semaine.util.XMLTool;

public class EmotionExtractor extends Component {
	private XMLSender emotionSender = new XMLSender("semaine.data.hello.emotion", "EmotionML", getName());

	public EmotionExtractor() throws JMSException {
		super("EmotionExtractor");
		receivers.add(new EmmaReceiver("semaine.data.state.user.emma"));
		senders.add(emotionSender);
	}

	@Override protected void react(SEMAINEMessage m) throws JMSException {
		SEMAINEEmmaMessage emmaMessage = (SEMAINEEmmaMessage) m;
		Element interpretation = emmaMessage.getTopLevelInterpretation();
		if (interpretation == null) {
			return;
		}
		List<Element> emotionElements = emmaMessage.getEmotionElements(interpretation);
		if (emotionElements.size() > 0) {
			Element emotion = emotionElements.get(0);
			Document emotionML = XMLTool.newDocument(EmotionML.ROOT_ELEMENT, EmotionML.namespaceURI);
			emotionML.adoptNode(emotion);
			emotionML.getDocumentElement().appendChild(emotion);
			emotionSender.sendXML(emotionML, meta.getTime());
		}
	}
}
