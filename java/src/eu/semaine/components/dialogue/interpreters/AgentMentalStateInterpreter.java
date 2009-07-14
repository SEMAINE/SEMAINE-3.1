package eu.semaine.components.dialogue.interpreters;

import java.util.HashMap;
import java.util.List;

import javax.jms.JMSException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.components.dialogue.actionproposers.UtteranceActionProposer;
import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.message.SEMAINEXMLMessage;
import eu.semaine.jms.receiver.XMLReceiver;
import eu.semaine.jms.sender.Sender;
import eu.semaine.jms.sender.XMLSender;
import eu.semaine.util.XMLTool;

/**
 * disagreement -> Spike: high, rest: low
 * agreement -> Spike: low, rest (nu) average
 * acceptance -> spike low, rest default
 * refusal -> spike high, rest low
 * belief -> spike low, rest high
 * disbelief -> spike high, rest low
 * liking -> spike low, obadiah middle, rest high
 * disliking -> spike high, obadiah middle, rest low
 * interest -> spike and obadiah low, poppy and prudence high
 * no_interest -> spike and obadiah high, poppy and prudence low
 * understanding -> all high
 * no_understanding -> all low
 * 
 * @author MaatM
 *
 */

public class AgentMentalStateInterpreter extends Component
{
	private static String PoppyState = "<?xml version=\"1.0\"?><profilexmlns=\"http://www.w3schools.com\"xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"xsi:schemaLocation=\"profiles.xsd\">	<parameter-set name=\"listener_fuction\">		<Parameter name=\"disagreement.value\" value=\"0.0\"/>		<Parameter name=\"disagreement.min\" value=\"0.0\"/><Parameter name=\"disagreement.max\" value=\"1.0\"/>		<Parameter name=\"agreement.value\" value=\"0.3\"/>		<Parameter name=\"agreement.min\" value=\"0.0\"/><Parameter name=\"agreement.max\" value=\"1.0\"/>		<Parameter name=\"acceptance.value\" value=\"0.3\"/>		<Parameter name=\"acceptance.min\" value=\"0.0\"/><Parameter name=\"acceptance.max\" value=\"1.0\"/>		<Parameter name=\"refusal.value\" value=\"0.0\"/>		<Parameter name=\"refusal.min\" value=\"0.0\"/><Parameter name=\"refusal.max\" value=\"1.0\"/>		<Parameter name=\"belief.value\" value=\"0.5\"/>		<Parameter name=\"belief.min\" value=\"0.0\"/><Parameter name=\"belief.max\" value=\"1.0\"/>		<Parameter name=\"disbelief.value\" value=\"0.0\"/>		<Parameter name=\"disbelief.min\" value=\"0.0\"/><Parameter name=\"disbelief.max\" value=\"1.0\"/>		<Parameter name=\"liking.value\" value=\"0.8\"/>		<Parameter name=\"liking.min\" value=\"0.0\"/><Parameter name=\"liking.max\" value=\"1.0\"/>		<Parameter name=\"disliking.value\" value=\"0.0\"/>		<Parameter name=\"disliking.min\" value=\"0.0\"/><Parameter name=\"disliking.max\" value=\"1.0\"/>		<Parameter name=\"interest.value\" value=\"0.8\"/>		<Parameter name=\"interest.min\" value=\"0.0\"/><Parameter name=\"interest.max\" value=\"1.0\"/>		<Parameter name=\"no_interest.value\" value=\"0.0\"/>		<Parameter name=\"no_interest.min\" value=\"0.0\"/><Parameter name=\"no_interest.max\" value=\"1.0\"/>		<Parameter name=\"understanding.value\" value=\"0.8\"/>		<Parameter name=\"understanding.min\" value=\"0.0\"/><Parameter name=\"understanding.max\" value=\"1.0\"/>		<Parameter name=\"no_understanding.value\" value=\"0.0\"/>		<Parameter name=\"no_understanding.min\" value=\"0.0\"/><Parameter name=\"no_understanding.max\" value=\"1.0\"/>	</parameter-set></profile>";
	private static String PrudenceState = "<?xml version=\"1.0\"?><profilexmlns=\"http://www.w3schools.com\"xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"xsi:schemaLocation=\"profiles.xsd\">	<parameter-set name=\"listener_fuction\">		<Parameter name=\"disagreement.value\" value=\"0.0\"/>		<Parameter name=\"disagreement.min\" value=\"0.0\"/><Parameter name=\"disagreement.max\" value=\"1.0\"/>		<Parameter name=\"agreement.value\" value=\"0.3\"/>		<Parameter name=\"agreement.min\" value=\"0.0\"/><Parameter name=\"agreement.max\" value=\"1.0\"/>		<Parameter name=\"acceptance.value\" value=\"0.3\"/>		<Parameter name=\"acceptance.min\" value=\"0.0\"/><Parameter name=\"acceptance.max\" value=\"1.0\"/>		<Parameter name=\"refusal.value\" value=\"0.0\"/>		<Parameter name=\"refusal.min\" value=\"0.0\"/><Parameter name=\"refusal.max\" value=\"1.0\"/>		<Parameter name=\"belief.value\" value=\"0.5\"/>		<Parameter name=\"belief.min\" value=\"0.0\"/><Parameter name=\"belief.max\" value=\"1.0\"/>		<Parameter name=\"disbelief.value\" value=\"0.0\"/>		<Parameter name=\"disbelief.min\" value=\"0.0\"/><Parameter name=\"disbelief.max\" value=\"1.0\"/>		<Parameter name=\"liking.value\" value=\"0.8\"/>		<Parameter name=\"liking.min\" value=\"0.0\"/><Parameter name=\"liking.max\" value=\"1.0\"/>		<Parameter name=\"disliking.value\" value=\"0.0\"/>		<Parameter name=\"disliking.min\" value=\"0.0\"/><Parameter name=\"disliking.max\" value=\"1.0\"/>		<Parameter name=\"interest.value\" value=\"0.8\"/>		<Parameter name=\"interest.min\" value=\"0.0\"/><Parameter name=\"interest.max\" value=\"1.0\"/>		<Parameter name=\"no_interest.value\" value=\"0.0\"/>		<Parameter name=\"no_interest.min\" value=\"0.0\"/><Parameter name=\"no_interest.max\" value=\"1.0\"/>		<Parameter name=\"understanding.value\" value=\"0.8\"/>		<Parameter name=\"understanding.min\" value=\"0.0\"/><Parameter name=\"understanding.max\" value=\"1.0\"/>		<Parameter name=\"no_understanding.value\" value=\"0.0\"/>		<Parameter name=\"no_understanding.min\" value=\"0.0\"/><Parameter name=\"no_understanding.max\" value=\"1.0\"/>	</parameter-set></profile>";
	private static String ObadiahState = "<?xml version=\"1.0\"?><profilexmlns=\"http://www.w3schools.com\"xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"xsi:schemaLocation=\"profiles.xsd\">	<parameter-set name=\"listener_fuction\">		<Parameter name=\"disagreement.value\" value=\"0.0\"/>		<Parameter name=\"disagreement.min\" value=\"0.0\"/><Parameter name=\"disagreement.max\" value=\"1.0\"/>		<Parameter name=\"agreement.value\" value=\"0.3\"/>		<Parameter name=\"agreement.min\" value=\"0.0\"/><Parameter name=\"agreement.max\" value=\"1.0\"/>		<Parameter name=\"acceptance.value\" value=\"0.3\"/>		<Parameter name=\"acceptance.min\" value=\"0.0\"/><Parameter name=\"acceptance.max\" value=\"1.0\"/>		<Parameter name=\"refusal.value\" value=\"0.0\"/>		<Parameter name=\"refusal.min\" value=\"0.0\"/><Parameter name=\"refusal.max\" value=\"1.0\"/>		<Parameter name=\"belief.value\" value=\"0.5\"/>		<Parameter name=\"belief.min\" value=\"0.0\"/><Parameter name=\"belief.max\" value=\"1.0\"/>		<Parameter name=\"disbelief.value\" value=\"0.0\"/>		<Parameter name=\"disbelief.min\" value=\"0.0\"/><Parameter name=\"disbelief.max\" value=\"1.0\"/>		<Parameter name=\"liking.value\" value=\"0.4\"/>		<Parameter name=\"liking.min\" value=\"0.0\"/><Parameter name=\"liking.max\" value=\"1.0\"/>		<Parameter name=\"disliking.value\" value=\"0.4\"/>		<Parameter name=\"disliking.min\" value=\"0.0\"/><Parameter name=\"disliking.max\" value=\"1.0\"/>		<Parameter name=\"interest.value\" value=\"0.2\"/>		<Parameter name=\"interest.min\" value=\"0.0\"/><Parameter name=\"interest.max\" value=\"1.0\"/>		<Parameter name=\"no_interest.value\" value=\"0.7\"/>		<Parameter name=\"no_interest.min\" value=\"0.0\"/><Parameter name=\"no_interest.max\" value=\"1.0\"/>		<Parameter name=\"understanding.value\" value=\"0.8\"/>		<Parameter name=\"understanding.min\" value=\"0.0\"/><Parameter name=\"understanding.max\" value=\"1.0\"/>		<Parameter name=\"no_understanding.value\" value=\"0.0\"/>		<Parameter name=\"no_understanding.min\" value=\"0.0\"/><Parameter name=\"no_understanding.max\" value=\"1.0\"/>	</parameter-set></profile>";
	private static String SpikeState = "<?xml version=\"1.0\"?><profilexmlns=\"http://www.w3schools.com\"xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"xsi:schemaLocation=\"profiles.xsd\">	<parameter-set name=\"listener_fuction\">		<Parameter name=\"disagreement.value\" value=\"0.8\"/>		<Parameter name=\"disagreement.min\" value=\"0.0\"/><Parameter name=\"disagreement.max\" value=\"1.0\"/>		<Parameter name=\"agreement.value\" value=\"0.0\"/>		<Parameter name=\"agreement.min\" value=\"0.0\"/><Parameter name=\"agreement.max\" value=\"1.0\"/>		<Parameter name=\"acceptance.value\" value=\"0.0\"/>		<Parameter name=\"acceptance.min\" value=\"0.0\"/><Parameter name=\"acceptance.max\" value=\"1.0\"/>		<Parameter name=\"refusal.value\" value=\"0.8\"/>		<Parameter name=\"refusal.min\" value=\"0.0\"/><Parameter name=\"refusal.max\" value=\"1.0\"/>		<Parameter name=\"belief.value\" value=\"0.0\"/>		<Parameter name=\"belief.min\" value=\"0.0\"/><Parameter name=\"belief.max\" value=\"1.0\"/>		<Parameter name=\"disbelief.value\" value=\"0.8\"/>		<Parameter name=\"disbelief.min\" value=\"0.0\"/><Parameter name=\"disbelief.max\" value=\"1.0\"/>		<Parameter name=\"liking.value\" value=\"0.0\"/>		<Parameter name=\"liking.min\" value=\"0.0\"/><Parameter name=\"liking.max\" value=\"1.0\"/>		<Parameter name=\"disliking.value\" value=\"0.8\"/>		<Parameter name=\"disliking.min\" value=\"0.0\"/><Parameter name=\"disliking.max\" value=\"1.0\"/>		<Parameter name=\"interest.value\" value=\"0.0\"/>		<Parameter name=\"interest.min\" value=\"0.0\"/><Parameter name=\"interest.max\" value=\"1.0\"/>		<Parameter name=\"no_interest.value\" value=\"0.8\"/>		<Parameter name=\"no_interest.min\" value=\"0.0\"/><Parameter name=\"no_interest.max\" value=\"1.0\"/>		<Parameter name=\"understanding.value\" value=\"0.8\"/>		<Parameter name=\"understanding.min\" value=\"0.0\"/><Parameter name=\"understanding.max\" value=\"1.0\"/>		<Parameter name=\"no_understanding.value\" value=\"0.0\"/>		<Parameter name=\"no_understanding.min\" value=\"0.0\"/><Parameter name=\"no_understanding.max\" value=\"1.0\"/>	</parameter-set></profile>";
	
	private HashMap<String,Integer> charNumbers = new HashMap<String,Integer>();
	private int currChar;
	
	private Sender mentalStateSender;
	private XMLReceiver contextReceiver;
	
	public AgentMentalStateInterpreter() throws JMSException
	{
		super( "AgentMentalStateInterpreter" );
		
		contextReceiver = new XMLReceiver("semaine.data.state.context");
		receivers.add( contextReceiver );
		
		mentalStateSender = new Sender("", "", getName());
		senders.add( mentalStateSender );
		
		charNumbers.put("poppy", UtteranceActionProposer.POPPY);
		charNumbers.put("prudence", UtteranceActionProposer.PRUDENCE);
		charNumbers.put("spike", UtteranceActionProposer.SPIKE);
		charNumbers.put("obadiah", UtteranceActionProposer.OBADIAH);
	}
	
	public void react( SEMAINEMessage m ) throws JMSException
	{
		if( updateCharacter(m) ) {
			sendMentalState();
		}
	}
	
	public boolean updateCharacter( SEMAINEMessage m ) throws JMSException
	{
		if( m instanceof SEMAINEXMLMessage ) {
			SEMAINEXMLMessage xm = ((SEMAINEXMLMessage)m);
			
			Document doc = xm.getDocument();
			Element root = doc.getDocumentElement();
			if (!root.getTagName().equals(SemaineML.E_CONTEXT)) {
				return false;
			}
			if (!root.getNamespaceURI().equals(SemaineML.namespaceURI)) {
				throw new MessageFormatException("Unexpected document element namespace: expected '"+SemaineML.namespaceURI+"', found '"+root.getNamespaceURI()+"'");
			}
			
			List<Element> characters = XMLTool.getChildrenByTagNameNS(root, SemaineML.E_CHARACTER, SemaineML.namespaceURI);
			for( Element characterElem : characters ) {
				String charName = characterElem.getAttribute(SemaineML.A_NAME);
				currChar = charNumbers.get( charName.toLowerCase() );
				return true;
			}
		}
		return false;
	}
	
	public void sendMentalState() throws JMSException
	{
		switch( currChar ) {
			case UtteranceActionProposer.POPPY:
				mentalStateSender.sendTextMessage(PoppyState, meta.getTime());
				break;
			case UtteranceActionProposer.PRUDENCE:
				mentalStateSender.sendTextMessage(PrudenceState, meta.getTime());
				break;
			case UtteranceActionProposer.OBADIAH:
				mentalStateSender.sendTextMessage(ObadiahState, meta.getTime());
				break;
			case UtteranceActionProposer.SPIKE:
				mentalStateSender.sendTextMessage(SpikeState, meta.getTime());
				break;
		}
		
	}
}
