/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.mary;

import java.io.ByteArrayOutputStream;
import java.io.Reader;
import java.io.StringReader;
import java.util.HashMap;
import java.util.Locale;
import java.util.Map;

import javax.jms.JMSException;
import javax.sound.sampled.AudioFileFormat;
import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioSystem;
import javax.xml.transform.Templates;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMResult;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;
import javax.xml.transform.stream.StreamSource;

import marytts.datatypes.MaryData;
import marytts.datatypes.MaryDataType;
import marytts.modules.synthesis.Voice;
import marytts.server.Mary;
import marytts.server.Request;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.traversal.DocumentTraversal;
import org.w3c.dom.traversal.NodeFilter;
import org.w3c.dom.traversal.NodeIterator;

import eu.semaine.components.Component;
import eu.semaine.components.control.ParticipantControlGUI;
import eu.semaine.datatypes.stateinfo.StateInfo;
import eu.semaine.datatypes.xml.BML;
import eu.semaine.datatypes.xml.FML;
import eu.semaine.datatypes.xml.SSML;
import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.exceptions.SystemConfigurationException;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.message.SEMAINEStateMessage;
import eu.semaine.jms.message.SEMAINEXMLMessage;
import eu.semaine.jms.receiver.BMLReceiver;
import eu.semaine.jms.receiver.FMLReceiver;
import eu.semaine.jms.receiver.StateReceiver;
import eu.semaine.jms.receiver.XMLReceiver;
import eu.semaine.jms.sender.FMLSender;
import eu.semaine.util.XMLTool;

/**
 * Speech preprocessor : To find pitch accent and boundaries
 * Speech BML realiser : Audio synthesis and phone timings
 * 
 * @author Sathish Chandra Pammi
 *
 */
public class SpeechPreprocessor extends Component 
{
	private FMLReceiver fmlReceiver;
	private BMLReceiver bmlReceiver;
	private FMLSender fmlbmlSender;
	private static TransformerFactory tFactory = null;
	private static Templates fml2ssmlStylesheet = null;
	private Transformer transformer;
	private String currentCharacter = ParticipantControlGUI.PRUDENCE;
	
	static final Map<String,String> characters2voices = fillCharacters2Voices();
	
	private static Map<String,String> fillCharacters2Voices()
	{
		Map<String,String> c2v = new HashMap<String, String>();
		c2v.put("Prudence", "dfki-prudence");
		c2v.put("Poppy", "dfki-poppy");
		c2v.put("Obadiah", "dfki-obadiah");
		c2v.put("Spike", "dfki-spike");
		return c2v;
	}
    
    
	/**
	 * @param componentName
	 * @throws JMSException
	 */
	public SpeechPreprocessor() throws JMSException 
	{
		super("SpeechPreprocessor");
		
		fmlReceiver = new FMLReceiver("semaine.data.action.selected.function");
		bmlReceiver = new BMLReceiver("semaine.data.action.selected.behaviour");
		receivers.add(fmlReceiver); // to set up properly
		receivers.add(bmlReceiver);
		receivers.add(new StateReceiver("semaine.data.state.context", StateInfo.Type.ContextState));
		
		fmlbmlSender = new FMLSender("semaine.data.action.selected.speechpreprocessed", getName());
		senders.add(fmlbmlSender); // so it can be started etc
	}
	
	protected void customStartIO() throws Exception
	{
		if (tFactory == null) {
            tFactory = TransformerFactory.newInstance();
		 }
    	StreamSource stylesheetStream =
	        new StreamSource( SpeechPreprocessor.class.getResourceAsStream(          
	                 "FML2SSML.xsl"));
		fml2ssmlStylesheet = tFactory.newTemplates(stylesheetStream);
		// Read properties:
        // (Will throw exceptions if problems are found)
    	System.setProperty("log.level", "WARN"); // avoid flood of MARY messages
		if(Mary.currentState() == Mary.STATE_OFF){
			Mary.startup();
		}
    	while (Mary.currentState() == Mary.STATE_STARTING) {
    		try {
    			Thread.sleep(10);
    		} catch (InterruptedException ie) {}
    	}
    	if (Mary.currentState() != Mary.STATE_RUNNING) {
    		throw new SystemConfigurationException("Could not start MARY system");
    	}
		
    	Runtime.getRuntime().addShutdownHook(new Thread() {
            public void run() {
            	if(Mary.STATE_SHUTTING_DOWN == Mary.currentState() || Mary.STATE_OFF == Mary.currentState()){
            		Mary.shutdown();
            	}
            }
        });
	}
	
  
	@Override
	public void react(SEMAINEMessage m) throws Exception
	{
		if ( !(m instanceof SEMAINEXMLMessage) ) {
			throw new MessageFormatException("expected XML message, got "+m.getClass().getSimpleName());
		}
		
		// if you receive a state message 
		if ( m instanceof SEMAINEStateMessage) {
			SEMAINEStateMessage ssm = (SEMAINEStateMessage) m;
			StateInfo state = ssm.getState();
			if (state.hasInfo("character")) {
                currentCharacter = state.getInfo("character");
            } 
		}
		
		if (m.getTopicName().equals("semaine.data.action.selected.function")) {
			speechPreProcessor(m);
		}
		
		if (m.getTopicName().equals("semaine.data.action.selected.behaviour")) {
			//SEMAINEXMLMessage xm = (SEMAINEXMLMessage)m;
			//fmlbmlSender.sendXML(xm.getDocument(), xm.getUsertime(), xm.getEventType());
		}
		
	}
	
	/**
	 * Speech Preprocessor
	 */
	private void speechPreProcessor(SEMAINEMessage m) throws Exception
	{	
		
		SEMAINEXMLMessage xm = (SEMAINEXMLMessage)m;
		//Voice voice = Voice.getDefaultVoice(Locale.ENGLISH);
		Voice voice = Voice.getDefaultVoice(Locale.ENGLISH);
		AudioFormat af = voice.dbAudioFormat();
        AudioFileFormat aff = new AudioFileFormat(AudioFileFormat.Type.WAVE,
            af, AudioSystem.NOT_SPECIFIED);
    	
        //Request request = new Request(MaryDataType.SSML,MaryDataType.INTONATION,Locale.US,Voice.getDefaultVoice(Locale.ENGLISH),null,null,1,null);
    	Request request = new Request(MaryDataType.SSML,MaryDataType.REALISED_ACOUSTPARAMS,Locale.ENGLISH,voice,"","",1,aff);
		
    	Document inputDoc = xm.getDocument();
		
		String localName    = xm.getDocument().getDocumentElement().getLocalName();
		String namespaceURI = xm.getDocument().getDocumentElement().getNamespaceURI(); 
		
		if (XMLTool.getChildElementByLocalNameNS(inputDoc.getDocumentElement(), BML.E_BML, BML.namespaceURI) != null) {
			
			transformer = fml2ssmlStylesheet.newTransformer();
			transformer.setParameter("character.voice", characters2voices.get(getCurrentCharacter()));

			// Marc, 27.11.09:
			// Use DOM rather than strings so we can replace identifiers in document with
			// standard ones, and translate them back in the result:
			DOMResult ssmlDR = new DOMResult();
			transformer.transform(new DOMSource(inputDoc), ssmlDR);
			Document ssmlDoc = (Document) ssmlDR.getNode();
			// Normalise documents for MARY Cache:
			// Replace identifiers (<mark name="..."/> attributes) with placeholders, and remember them:
			Map<String, String> placeholders = new HashMap<String, String>();
			NodeIterator ni = XMLTool.createNodeIterator(ssmlDoc, ssmlDoc, SSML.namespaceURI, SSML.E_MARK);
			Element elt = null;
			int i = 0;
			while ((elt = (Element)ni.nextNode()) != null) {
				String name = elt.getAttribute(SSML.A_NAME);
				if (!"".equals(name)) {
					String placeholder = "m"+i;
					elt.setAttribute(SSML.A_NAME, placeholder);
					placeholders.put(placeholder, name);
					i++;
				}
			}
			
			String ssmlString = XMLTool.document2String(ssmlDoc);
			log.debug("Sending the following SSML to MARY:\n"+ssmlString);
			Document maryDoc = null;
			try {
				MaryData ssmlData = new MaryData(request.getInputType(), request.getDefaultLocale());
				ssmlData.setDocument(ssmlDoc);
				request.setInputData(ssmlData);
				request.process();
				MaryData maryOut = request.getOutputData();
				maryDoc = maryOut.getDocument();
			} catch (Exception e) {
				throw new Exception("MARY cannot process input -- SSML input was:\n"+ssmlString, e);
			}
			assert maryDoc != null;
			// Replace placeholders in maryDoc back into original names:
			NodeIterator ni2 = XMLTool.createNodeIterator(maryDoc, maryDoc, maryDoc.getDocumentElement().getNamespaceURI(), "mark");
			Element elt2 = null;
			while ((elt2 = (Element)ni2.nextNode()) != null) {
				String placeholder = elt2.getAttribute("name");
				String name = placeholders.get(placeholder);
				elt2.setAttribute("name", name);
			}
			
			
			
			//String finalData = XMLTool.mergeTwoXMLFiles(inputText, intonationOS.toString(), SpeechPreprocessor.class.getResourceAsStream("FML-Intonation-Merge.xsl"), "semaine.mary.intonation");
			//String finalData = XMLTool.mergeTwoXMLFiles(inputText, intonationOS.toString(), SpeechPreprocessor.class.getResourceAsStream("FML-RealisedSpeech-Merge.xsl"), "semaine.mary.realised.acoustics");
			Document finalData = XMLTool.mergeTwoXMLFiles(inputDoc, maryDoc, SpeechPreprocessor.class.getResourceAsStream("FML-RealisedSpeech-Merge.xsl"), "semaine.mary.realised.acoustics");
			fmlbmlSender.sendXML(finalData, meta.getTime(), xm.getEventType(), xm.getContentID(), xm.getContentCreationTime());
		}
		else if ( localName.equals(FML.E_FML) && namespaceURI.equals(FML.namespaceURI)) {
			fmlbmlSender.sendXML(xm.getDocument(), meta.getTime(), xm.getEventType(), xm.getContentID(), xm.getContentCreationTime());
		}
		else if ( localName.equals(BML.E_BML) && namespaceURI.equals(BML.namespaceURI) ) {
			fmlbmlSender.sendXML(xm.getDocument(), meta.getTime(), xm.getEventType(), xm.getContentID(), xm.getContentCreationTime());
		}
		else {
			Element backchannel = null;
			Element fml = XMLTool.getChildElementByLocalNameNS(xm.getDocument().getDocumentElement(), FML.E_FML, FML.namespaceURI);
			if (fml != null) {
				backchannel = XMLTool.getChildElementByLocalNameNS(fml, FML.E_BACKCHANNEL, FML.namespaceURI);
			}
			if (backchannel != null) {
				fmlbmlSender.sendXML(inputDoc, meta.getTime(), xm.getEventType(), xm.getContentID(), xm.getContentCreationTime());
			}
			else{
				log.debug("Received fml document without bml or backchannel content -- ignoring.");
			}
		}
	}
	
	
    public String getCurrentCharacter()
    {
        return currentCharacter;
    }
	
}
