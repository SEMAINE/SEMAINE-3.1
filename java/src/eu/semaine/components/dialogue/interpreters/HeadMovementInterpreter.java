package eu.semaine.components.dialogue.interpreters;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.jms.JMSException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

import eu.semaine.components.Component;
import eu.semaine.components.dialogue.test.DMLogger;
import eu.semaine.datatypes.stateinfo.ContextStateInfo;
import eu.semaine.datatypes.stateinfo.StateInfo;
import eu.semaine.datatypes.stateinfo.UserStateInfo;
import eu.semaine.datatypes.xml.BML;
import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.jms.message.SEMAINEEmmaMessage;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.receiver.EmmaReceiver;
import eu.semaine.jms.sender.StateSender;
import eu.semaine.jms.sender.XMLSender;
import eu.semaine.util.XMLTool;

public class HeadMovementInterpreter extends Component
{
	/* Senders and Receivers */
	private EmmaReceiver emmaReceiver;
	private StateSender userStateSender;
	
	public HeadMovementInterpreter() throws JMSException
	{
		super( "HeadMovementInterpreter" );
		
		userStateSender = new StateSender("semaine.data.state.user.behaviour", StateInfo.Type.UserState, getName());
		senders.add(userStateSender); // so it can be started etc
		
		emmaReceiver = new EmmaReceiver("semaine.data.state.user.emma", "datatype = 'EMMA'");
		receivers.add(emmaReceiver);
	}
	
	public void react( SEMAINEMessage m ) throws JMSException
	{
		int[] headMovement = getHeadMovement(m);
		if( headMovement[0] != 0 ) {
			sendHeadMovement( headMovement );
		}
	}
	
	/**
	 * @param m
	 * @return 0 if nothing was detected, 1 if a nod was detected, and 2 if a shake was detected
	 * @throws JMSException
	 */
	public int[] getHeadMovement( SEMAINEMessage m ) throws JMSException
	{
		int[] result = new int[3];
		if( m instanceof SEMAINEEmmaMessage ) {
			SEMAINEEmmaMessage em = (SEMAINEEmmaMessage)m;
			
			Element interpretation = em.getTopLevelInterpretation();
			if (interpretation != null) {
				List<Element> behaviours = em.getBehaviourElements(interpretation);
				Element behaviourElem = XMLTool.getChildElementByTagNameNS(interpretation, "bml", BML.namespaceURI);
				//for( Element behaviourElem : behaviours ) {
				if( behaviourElem != null ) {
					Element headMovement = XMLTool.getChildElementByLocalNameNS(behaviourElem, BML.E_HEAD, BML.namespaceURI);
					if( headMovement != null && headMovement.hasAttribute(BML.A_TYPE) && headMovement.hasAttribute(BML.A_START) && headMovement.hasAttribute(BML.A_END) ) {
						if( headMovement.getAttribute(BML.A_TYPE).equals("NOD") ) {
							result[0] = 1;
							result[1] = ((int)(Double.parseDouble( headMovement.getAttribute(BML.A_START) )*1000));
							result[2] = ((int)(Double.parseDouble( headMovement.getAttribute(BML.A_END) )*1000));
							return result;
						} else if( headMovement.getAttribute(BML.A_TYPE).equals("SHAKE") ) {
							result[0] = 2;
							result[1] = ((int)(Double.parseDouble( headMovement.getAttribute(BML.A_START) )*1000));
							result[2] = ((int)(Double.parseDouble( headMovement.getAttribute(BML.A_END) )*1000));
							return result;
						}
					}
				}
			}
		}
		result[0] = 0;
		return result;
	}
	
	public void sendHeadMovement( int[] headMovement ) throws JMSException
	{
		Map<String,String> userInfo = new HashMap<String,String>();
		if( headMovement[0] == 1 ) {
			DMLogger.getLogger().log(meta.getTime(), "UserAction:HeadMovement movement=nod starttime="+headMovement[1]+" endtime="+headMovement[2]);
			userInfo.put("headGesture", "NOD");
		} else if( headMovement[0] == 2 ) {
			DMLogger.getLogger().log(meta.getTime(), "UserAction:HeadMovement movement=shake starttime="+headMovement[1]+" endtime="+headMovement[2]);
			userInfo.put("headGesture", "SHAKE");
		} else {
			return;
		}
		userInfo.put("headGestureStarted", ""+headMovement[1]);
		userInfo.put("headGestureStopped", ""+headMovement[2]);
		

		UserStateInfo csi = new UserStateInfo(userInfo);
		userStateSender.sendStateInfo( csi, meta.getTime() );
	}
}
