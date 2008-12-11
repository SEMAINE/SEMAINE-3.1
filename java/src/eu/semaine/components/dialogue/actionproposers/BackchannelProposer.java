package eu.semaine.components.dialogue.actionproposers;

import java.util.List;
import java.util.Map;

import javax.jms.JMSException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.components.dialogue.interpreters.TurnTakingInterpreter.State;
import eu.semaine.components.dialogue.util.CyclicBuffer;
import eu.semaine.datatypes.stateinfo.DialogStateInfo;
import eu.semaine.datatypes.xml.BML;
import eu.semaine.datatypes.xml.FML;
import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.jms.message.SEMAINEDialogStateMessage;
import eu.semaine.jms.message.SEMAINEEmmaMessage;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.receiver.DialogStateReceiver;
import eu.semaine.jms.receiver.EmmaReceiver;
import eu.semaine.jms.sender.FMLSender;
import eu.semaine.util.XMLTool;

public class BackchannelProposer extends Component 
{
	private EmmaReceiver emmaReceiver;
	private DialogStateReceiver dialogStateReceiver;
	private FMLSender fmlSender;
	
	public static enum State { USER_SPEAKING, AGENT_WAITING }
	private State turntakingState = State.AGENT_WAITING;
	private long pauseTime = 0;
	private boolean userSpeaking = false;
	private boolean didBackchannel = true;
	private CyclicBuffer<Float> logEnergyBuffer = new CyclicBuffer<Float>(20);
	private CyclicBuffer<Float> foStrengthBuffer = new CyclicBuffer<Float>(30);
	
	public BackchannelProposer() throws JMSException
	{
		super("BackchannelPropser");
		
		/* Define Receivers */
		emmaReceiver = new EmmaReceiver("semaine.data.state.user.emma", "datatype = 'EMMA'");
		receivers.add(emmaReceiver);
		dialogStateReceiver = new DialogStateReceiver("semaine.data.state.dialog");
		receivers.add(dialogStateReceiver);
		
		fmlSender = new FMLSender("semaine.data.action.candidate.function", getName());
		senders.add(fmlSender);
	}
	
	/**
	 * Checks if the turn has switched to the agent
	 */
	@Override
	public void act() throws JMSException
	{
		if( !didBackchannel && pauseTime != 0 && turntakingState == State.USER_SPEAKING && pauseTime + 300 < meta.getTime() ) {
			sendBackchannelSignal();
			didBackchannel = true;
		}
	}
	
	/**
	 * Updates the time since the user is silent and the turn taking state
	 * based on the received message
	 */
	@Override
	public void react( SEMAINEMessage m ) throws JMSException
	{
		
		if( !processTurnMessage(m) ) {
			if( isSilence(m) ) {
				didBackchannel = false;
				pauseTime = meta.getTime();
			} else if( isSpeaking(m) ) {
				if( turntakingState == State.AGENT_WAITING ) {
					turntakingState = State.USER_SPEAKING;
				}
				pauseTime = 0;
			}
		}
	}
	
	public void sendBackchannelSignal() throws JMSException
	{
		System.out.println("backchannel");
		Document doc = XMLTool.newDocument("fml-apml", null, FML.version);
		Element root = doc.getDocumentElement();
		Element fml = XMLTool.appendChildElement(root, FML.E_FML, FML.namespaceURI);
		Element backchannel = XMLTool.appendChildElement(fml, FML.E_BACKCHANNEL, FML.namespaceURI);
		
		fmlSender.sendXML(doc, meta.getTime());
	}
	
	/**
	 * Checks if the Message is ASR output and if it contains Silence.
	 * @param m the message to check
	 * @return true if the message says that the user is currently silent
	 */
	public boolean isSilence( SEMAINEMessage m ) throws JMSException
	{
		if( m instanceof SEMAINEEmmaMessage ) {
			SEMAINEEmmaMessage em = (SEMAINEEmmaMessage)m;
			Element interpretation = em.getTopLevelInterpretation();
			if (interpretation != null) {
				List<Element> behaviours = em.getBehaviourElements(interpretation);
				for (Element behaviour : behaviours) {
					String fName = XMLTool.needAttribute(behaviour, SemaineML.A_NAME);
					//String fIntensity = XMLTool.needAttribute(behaviour, SemaineML.INTENSITY);
					if (fName.equals("silent")) {
						return true;
					}
				}
			}
		}
		return false;
	}
	
	/**
	 * Checks if the Message is ASR output and if it contains a word.
	 * @param m the message to check
	 * @return true if the message says that the user is currently silent
	 */
	public boolean isSpeaking( SEMAINEMessage m ) throws JMSException
	{
		if( m instanceof SEMAINEEmmaMessage ) {
			SEMAINEEmmaMessage em = (SEMAINEEmmaMessage)m;
			Element interpretation = em.getTopLevelInterpretation();
			if (interpretation != null) {
				List<Element> behaviours = em.getBehaviourElements(interpretation);
				for (Element behaviour : behaviours) {
					String fName = XMLTool.needAttribute(behaviour, SemaineML.A_NAME);
					//String fIntensity = XMLTool.needAttribute(behaviour, SemaineML.INTENSITY);
					if (fName.equals("speaking")) {
						return true;
					}
				}
			}
		}
		return false;
	}
	
	public boolean processTurnMessage(SEMAINEMessage m)
	{
		if( m instanceof SEMAINEDialogStateMessage ) {
			DialogStateInfo dialogInfo = ((SEMAINEDialogStateMessage)m).getState();
			Map<String,String> dialogInfoMap = dialogInfo.getInfo();
			
			if( dialogInfoMap.get("speaker").equals("agent") ) {
				turntakingState = State.AGENT_WAITING;
				return true;
			} else if( dialogInfoMap.get("speaker").equals("user") ) {
				turntakingState = State.USER_SPEAKING;
				return true;
			}
		}
		return false;
	}
}
