package eu.semaine.components.dialogue.loebner;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.jms.JMSException;

import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.components.dialogue.datastructures.DialogueAct;
import eu.semaine.datatypes.stateinfo.StateInfo;
import eu.semaine.datatypes.stateinfo.UserStateInfo;
import eu.semaine.datatypes.xml.EMMA;
import eu.semaine.jms.message.SEMAINEEmmaMessage;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.receiver.EmmaReceiver;
import eu.semaine.jms.receiver.StateReceiver;
import eu.semaine.jms.sender.StateSender;
import eu.semaine.jms.sender.XMLSender;
import eu.semaine.util.XMLTool;

public class LoebnerUtteranceInterpreter extends Component
{
	/* Receivers */
	private EmmaReceiver emmaReceiver;
	private StateReceiver dialogStateReceiver;

	/* Senders */
	private StateSender userStateSender;
	
	public LoebnerUtteranceInterpreter() throws JMSException
	{
		super( "LoebnerUtteranceInterpreter" );
		
		emmaReceiver = new EmmaReceiver("semaine.data.state.user.emma");
		receivers.add(emmaReceiver);

		/* Define Senders */
		userStateSender = new StateSender("semaine.data.state.user.behaviour", StateInfo.Type.UserState, getName());
		senders.add(userStateSender);
	}
	
	@Override
	public void act()
	{
		
	}
	
	@Override
	public void react( SEMAINEMessage m ) throws JMSException
	{
		String[] detectedWordsPlusTime = getDetectedWords( m );
		
		sendDetectedWords( detectedWordsPlusTime );
	}
	
	public String[] getDetectedWords( SEMAINEMessage m ) throws JMSException
	{
		String[] detectedWordsPlusTime = new String[2];
		detectedWordsPlusTime[0] = "";
		detectedWordsPlusTime[1] = "";
		
		if( m instanceof SEMAINEEmmaMessage ) {
			SEMAINEEmmaMessage em = (SEMAINEEmmaMessage)m;
			
			/* Reading words */
			//System.out.println(em.getText());
			Element wordSequence = em.getSequence();
			if( wordSequence != null ) {
				//System.out.println("WordSequence found");
				List<Element> wordElements = XMLTool.getChildrenByLocalNameNS(wordSequence, EMMA.E_INTERPRETATION, EMMA.namespaceURI);
				if( wordElements.size() > 0 ) {
					//System.out.println("WordElements found");
					String starttime = null;
					for( Element wordElem : wordElements ) {
						if( wordElem.hasAttribute("tokens") ) {
							detectedWordsPlusTime[0] = detectedWordsPlusTime[0] + wordElem.getAttribute("tokens") + " ";
						}
						if( starttime == null && wordSequence.hasAttribute("offset-to-start") ) {
							starttime = wordSequence.getAttribute("offset-to-start");
						}
					}
					detectedWordsPlusTime[0] = detectedWordsPlusTime[0].trim();
					detectedWordsPlusTime[1] = starttime;
				}
			}
		}
		return detectedWordsPlusTime;
	}
	
	public void sendDetectedWords( String[] detectedWordsPlusTime ) throws JMSException
	{
		if( detectedWordsPlusTime[0].length() < 1 ) {
			return;
		}
		Map<String,String> userStateInfo = new HashMap<String,String>();
		userStateInfo.put("userUtterance", detectedWordsPlusTime[0]);
		userStateInfo.put("userUtteranceStartTime", ""+detectedWordsPlusTime[1]);
		
		UserStateInfo usi = new UserStateInfo(userStateInfo	);
		userStateSender.sendStateInfo(usi, meta.getTime());	
	}
}
