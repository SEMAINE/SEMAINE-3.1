/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.mary;

import java.io.ByteArrayOutputStream;
import java.io.Reader;
import java.io.StringReader;
import java.util.Locale;

import javax.jms.JMSException;
import javax.sound.sampled.AudioFileFormat;
import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioSystem;
import javax.xml.transform.Templates;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;
import javax.xml.transform.stream.StreamSource;

import marytts.datatypes.MaryDataType;
import marytts.modules.synthesis.Voice;
import marytts.server.Mary;
import marytts.server.Request;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.components.control.ParticipantControlGUI;
import eu.semaine.datatypes.xml.BML;
import eu.semaine.datatypes.xml.FML;
import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.exceptions.SystemConfigurationException;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.message.SEMAINEXMLMessage;
import eu.semaine.jms.receiver.BMLReceiver;
import eu.semaine.jms.receiver.FMLReceiver;
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
	private String currentCharacter = ParticipantControlGUI.SPIKE; 
    
    
	/**
	 * @param componentName
	 * @throws JMSException
	 */
	public SpeechPreprocessor() throws JMSException 
	{
		super("SpeechPreprocessor");
		fmlReceiver = new FMLReceiver("semaine.data.action.selected.function");
		receivers.add(fmlReceiver); // to set up properly
		bmlReceiver = new BMLReceiver("semaine.data.action.selected.behaviour");
		receivers.add(bmlReceiver);
		receivers.add(new XMLReceiver("semaine.data.state.context"));
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
		
		if (m.getTopicName().equals("semaine.data.state.context")) {
			updateCharacter(m);
		}
		if (m.getTopicName().equals("semaine.data.action.selected.function")) {
			speechPreProcessor(m);
		}
		if (m.getTopicName().equals("semaine.data.action.selected.behaviour")) {
			//SEMAINEXMLMessage xm = (SEMAINEXMLMessage)m;
			//fmlbmlSender.sendXML(xm.getDocument(), xm.getUsertime(), xm.getEventType());
		}
		
	}
	
	
	
	private void updateCharacter(SEMAINEMessage m) throws MessageFormatException 
	{
        SEMAINEXMLMessage xm = (SEMAINEXMLMessage) m;
        Document doc = xm.getDocument();
        Element root = doc.getDocumentElement();
        if (!root.getTagName().equals(SemaineML.E_CONTEXT)) {
            throw new MessageFormatException("Unexpected document element: expected tag name '"+SemaineML.E_CONTEXT+"', found '"+root.getTagName()+"'");
        }
        if (!root.getNamespaceURI().equals(SemaineML.namespaceURI)) {
            throw new MessageFormatException("Unexpected document element namespace: expected '"+SemaineML.namespaceURI+"', found '"+root.getNamespaceURI()+"'");
        }
        
        Element character = XMLTool.getChildElementByTagNameNS(root, SemaineML.E_CHARACTER, SemaineML.namespaceURI);
        if (character != null) {
        	currentCharacter = character.getAttribute(SemaineML.A_NAME);
        }
		
	}

	/**
	 * Speech Preprocessor
	 */
	private void speechPreProcessor(SEMAINEMessage m) throws Exception
	{	
		
		SEMAINEXMLMessage xm = (SEMAINEXMLMessage)m;
		ByteArrayOutputStream ssmlos = new ByteArrayOutputStream();
		//Voice voice = Voice.getDefaultVoice(Locale.ENGLISH);
		Voice voice = Voice.getDefaultVoice(Locale.ENGLISH);
		AudioFormat af = voice.dbAudioFormat();
        AudioFileFormat aff = new AudioFileFormat(AudioFileFormat.Type.WAVE,
            af, AudioSystem.NOT_SPECIFIED);
    	
        //Request request = new Request(MaryDataType.SSML,MaryDataType.INTONATION,Locale.US,Voice.getDefaultVoice(Locale.ENGLISH),null,null,1,null);
    	Request request = new Request(MaryDataType.SSML,MaryDataType.get("REALISED_ACOUSTPARAMS"),Locale.ENGLISH,voice,"","",1,aff);
		
    	Document inputDoc = xm.getDocument();
		String inputText = xm.getText();
		System.out.println(inputText);
		if (XMLTool.getChildElementByTagNameNS(inputDoc.getDocumentElement(), BML.E_BML, BML.namespaceURI) != null) {
			
			transformer = fml2ssmlStylesheet.newTransformer();
			transformer.setParameter("character.voice", getCurrentCharacter().toLowerCase());
			transformer.transform(new DOMSource(inputDoc), new StreamResult(ssmlos));
			String ssml = ssmlos.toString();
			Reader reader = new StringReader(ssml);
			ByteArrayOutputStream  intonationOS = new ByteArrayOutputStream();
			try {
				request.readInputData(reader);
				request.process();
				request.writeOutputData(intonationOS);
			} catch (Exception e) {
				throw new Exception("MARY cannot process input -- SSML input was:\n"+ssml, e);
			}
			
			//String finalData = XMLTool.mergeTwoXMLFiles(inputText, intonationOS.toString(), SpeechPreprocessor.class.getResourceAsStream("FML-Intonation-Merge.xsl"), "semaine.mary.intonation");
			String finalData = XMLTool.mergeTwoXMLFiles(inputText, intonationOS.toString(), SpeechPreprocessor.class.getResourceAsStream("FML-RealisedSpeech-Merge.xsl"), "semaine.mary.realised.acoustics");
			
			fmlbmlSender.sendTextMessage(finalData, xm.getUsertime(), xm.getEventType());
		} 
		else {
			Element backchannel = null;
			Element fml = XMLTool.getChildElementByTagNameNS(xm.getDocument().getDocumentElement(), FML.E_FML, FML.namespaceURI);
			if (fml != null) {
				backchannel = XMLTool.getChildElementByTagNameNS(fml, FML.E_BACKCHANNEL, FML.namespaceURI);
			}
			if (backchannel != null) {
				fmlbmlSender.sendXML(inputDoc, xm.getUsertime(), xm.getEventType());
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
