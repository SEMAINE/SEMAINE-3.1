package eu.semaine.examples.hello;

import java.util.List;

import javax.jms.JMSException;

import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.datatypes.xml.EmotionML;
import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.message.SEMAINEXMLMessage;
import eu.semaine.jms.receiver.XMLReceiver;
import eu.semaine.util.XMLTool;

public class EmoticonOutput extends Component {

	public EmoticonOutput() throws JMSException {
		super("EmoticonOutput", false, true /*is output*/);
		receivers.add(new XMLReceiver("semaine.data.hello.emotion"));
	}
	
	@Override protected void react(SEMAINEMessage m) throws MessageFormatException {
		SEMAINEXMLMessage xm = (SEMAINEXMLMessage) m;
		Element emotion = (Element) xm.getDocument().getElementsByTagNameNS(EmotionML.namespaceURI, EmotionML.E_EMOTION).item(0);
		String dimensionVocabularyURI = emotion.getAttribute(EmotionML.A_DIMENSION_VOCABULARY);
		if (!dimensionVocabularyURI.equals(EmotionML.VOC_FSRE_DIMENSION_DEFINITION)) return;
		List<Element> dimensions = XMLTool.getChildrenByLocalNameNS(emotion, EmotionML.E_DIMENSION, EmotionML.namespaceURI);
		float a = 0.5f, v = 0.5f; // neutral values
		boolean haveSomething = false;
		for (Element dim : dimensions) {
			String name = dim.getAttribute(EmotionML.A_NAME);
			float value = Float.parseFloat(dim.getAttribute(EmotionML.A_VALUE));
			if (name.equals(EmotionML.VOC_FSRE_DIMENSION_AROUSAL)) {
				a = value;
				haveSomething = true;
			} else if (name.equals(EmotionML.VOC_FSRE_DIMENSION_VALENCE)) {
				v = value;
				haveSomething = true;
			}
		}
		if (!haveSomething) {
			return;
		}
		
		String eyes  = a > 0.66 ? "8"/*active*/   : a < 0.33 ? "*"/*passive*/  : ":"/*neutral*/;
		String mouth = v > 0.66 ? ")"/*positive*/ : v < 0.33 ? "("/*negative*/ : "|"/*neutral*/;
		System.out.println(eyes+"-"+mouth);
	}
}
