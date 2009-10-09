package eu.semaine.examples.swimmer;

import java.util.List;

import javax.jms.JMSException;

import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.datatypes.xml.EmotionML;
import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.jms.message.SEMAINEEmmaMessage;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.receiver.EmmaReceiver;
import eu.semaine.jms.sender.Sender;
import eu.semaine.util.XMLTool;

public class PositionComputer extends Component {
	private Sender positionSender = new Sender("semaine.data.swimmer.position", "TEXT", getName());
	private float position = 50;
	
	public PositionComputer() throws JMSException {
		super("PositionComputer");
		receivers.add(new EmmaReceiver("semaine.data.state.user.emma"));
		senders.add(positionSender);
	}
	
	@Override protected void react(SEMAINEMessage m) throws MessageFormatException {
		SEMAINEEmmaMessage emmaMessage = (SEMAINEEmmaMessage) m;
		Element interpretation = emmaMessage.getTopLevelInterpretation();
		if (interpretation == null) {
			return;
		}
		List<Element> emotionElements = emmaMessage.getEmotionElements(interpretation);

		for (Element emotion : emotionElements) {
			Element dimensions = XMLTool.getChildElementByLocalNameNS(emotion, EmotionML.E_DIMENSIONS, EmotionML.namespaceURI);
			if (dimensions != null) {
				Element arousal = XMLTool.getChildElementByLocalNameNS(dimensions, EmotionML.E_AROUSAL, EmotionML.namespaceURI);
				if (arousal != null) {
					float arousalValue = Float.parseFloat(arousal.getAttribute(EmotionML.A_VALUE));
					// Arousal influences the swimmer's position:
					position += 3*arousalValue;
					System.out.println("At time " + meta.getTime()+", arousal "+arousalValue+" -- new position "+position);
				}
			}
		}
	}

	@Override protected void act() throws JMSException {
		// The river slowly pulls back the swimmer:
		position -= 0.1;
		positionSender.sendTextMessage(String.valueOf(position), meta.getTime());
	}
	
}
