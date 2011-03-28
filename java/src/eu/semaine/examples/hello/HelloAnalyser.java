package eu.semaine.examples.hello;

import javax.jms.JMSException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.datatypes.xml.EmotionML;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.receiver.Receiver;
import eu.semaine.jms.sender.XMLSender;
import eu.semaine.util.XMLTool;

public class HelloAnalyser extends Component {

	private XMLSender emotionSender = new XMLSender("semaine.data.hello.emotion", "EmotionML", getName());

	public HelloAnalyser() throws JMSException {
		super("HelloAnalyser");
		receivers.add(new Receiver("semaine.data.hello.text"));
		senders.add(emotionSender);
	}
	
	@Override protected void react(SEMAINEMessage m) throws JMSException {
		float arousalValue = 0.5f, valenceValue = 0.5f;
		String input = m.getText();
		if (input.contains("very")) arousalValue = 1;
		else if (input.contains("a bit")) arousalValue = 0;
		if (input.contains("happy")) valenceValue = 1;
		else if (input.contains("sad")) valenceValue = 0;
		Document emotionML = createEmotionML(arousalValue, valenceValue);
		emotionSender.sendXML(emotionML, meta.getTime());
	}

	private Document createEmotionML(float arousalValue, float valenceValue) {
		Document emotionML = XMLTool.newDocument(EmotionML.E_ROOT_TAGNAME, EmotionML.namespaceURI);
		Element emotion = XMLTool.appendChildElement(emotionML.getDocumentElement(), EmotionML.E_EMOTION);
		emotion.setAttribute(EmotionML.A_DIMENSION_VOCABULARY, EmotionML.VOC_FSRE_DIMENSION_DEFINITION);
		Element arousal = XMLTool.appendChildElement(emotion, EmotionML.E_DIMENSION);
		arousal.setAttribute(EmotionML.A_NAME, EmotionML.VOC_FSRE_DIMENSION_AROUSAL);
		arousal.setAttribute(EmotionML.A_VALUE, String.valueOf(arousalValue));
		Element valence = XMLTool.appendChildElement(emotion, EmotionML.E_DIMENSION);
		valence.setAttribute(EmotionML.A_NAME, EmotionML.VOC_FSRE_DIMENSION_VALENCE);
		valence.setAttribute(EmotionML.A_VALUE, String.valueOf(valenceValue));
		return emotionML;
	}
}
