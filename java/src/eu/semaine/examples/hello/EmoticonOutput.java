package eu.semaine.examples.hello;

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
		Element dimensions = (Element) xm.getDocument().getElementsByTagNameNS(EmotionML.namespaceURI, EmotionML.E_DIMENSIONS).item(0);
		Element arousal = XMLTool.getChildElementByLocalNameNS(dimensions, EmotionML.E_AROUSAL, EmotionML.namespaceURI);
		Element valence = XMLTool.getChildElementByLocalNameNS(dimensions, EmotionML.E_VALENCE, EmotionML.namespaceURI);

		if (arousal == null && valence == null) {
			return;
		}
		float a = Float.parseFloat(arousal == null ? "0" : arousal.getAttribute(EmotionML.A_VALUE));
		float v = Float.parseFloat(valence == null ? "0" : valence.getAttribute(EmotionML.A_VALUE));
		
		String eyes  = a > 0.3 ? "8"/*active*/   : a < -0.3 ? "*"/*passive*/  : ":"/*neutral*/;
		String mouth = v > 0.3 ? ")"/*positive*/ : v < -0.3 ? "("/*negative*/ : "|"/*neutral*/;
		System.out.println(eyes+"-"+mouth);
	}
}
