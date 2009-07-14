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
import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.exceptions.SystemConfigurationException;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.message.SEMAINEXMLMessage;
import eu.semaine.jms.receiver.BMLReceiver;
import eu.semaine.jms.receiver.XMLReceiver;
import eu.semaine.jms.sender.BMLSender;
import eu.semaine.jms.sender.BytesSender;
import eu.semaine.util.XMLTool;

/**
 * Speech preprocessor : To find pitch accent and boundaries
 * Speech BML realiser : Audio synthesis and phone timings
 * 
 * @author Sathish Chandra Pammi
 *
 */
public class SpeechBMLRealiser extends Component 
{
	private BMLReceiver bmlPlanReceiver;
	private BMLSender bmlSender;
	private BytesSender audioSender;
	
	private static TransformerFactory tFactory = null;
	private static Templates bml2ssmlStylesheet = null;
    private Transformer transformer;
    private int backchannelNumber = 0;
    private int MaxNoOfBackchannels = 4;
    private String currentCharacter = ParticipantControlGUI.SPIKE;
    
	/**
	 * @param componentName
	 * @throws JMSException
	 */
	public SpeechBMLRealiser() throws JMSException 
	{
		super("SpeechBMLRealiser");
		
		bmlPlanReceiver = new BMLReceiver("semaine.data.synthesis.plan");
		receivers.add(bmlPlanReceiver);
		receivers.add(new XMLReceiver("semaine.data.state.context"));
		bmlSender = new BMLSender("semaine.data.synthesis.plan.speechtimings", getName());
		senders.add(bmlSender);
		audioSender = new BytesSender("semaine.data.synthesis.lowlevel.audio","AUDIO",getName());
		senders.add(audioSender); // so it can be started etc
	}
	
	protected void customStartIO() throws Exception
	{
		if (tFactory == null) {
            tFactory = TransformerFactory.newInstance();
		 }
    	StreamSource stylesheetStream =
	        new StreamSource( SpeechBMLRealiser.class.getResourceAsStream(          
	                 "BML2SSML.xsl"));
		bml2ssmlStylesheet = tFactory.newTemplates(stylesheetStream);
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
    	//startup();
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
		if (!(m instanceof SEMAINEXMLMessage)) {
			throw new MessageFormatException("expected XML message, got "+m.getClass().getSimpleName());
		}
		if (m.getTopicName().equals("semaine.data.state.context")) {
			updateCharacter(m);
		}
		if(m.getTopicName().equals("semaine.data.synthesis.plan")){
			speechBMLRealiser(m);
		}
	}
	
	
	/**
	 * speech BML Realiser
	 * @param m
	 * @throws JMSException
	 */
	private void speechBMLRealiser(SEMAINEMessage m) throws Exception{
		
		SEMAINEXMLMessage xm = (SEMAINEXMLMessage)m;
		Document input = xm.getDocument();
		String inputText = xm.getText();
		ByteArrayOutputStream ssmlos = new ByteArrayOutputStream();

		if(XMLTool.getChildElementByTagNameNS(input.getDocumentElement(), BML.E_BACKCHANNEL, BML.namespaceURI) != null){
			// Back-channel synthesis
			
			
			backchannelNumber++;
			if(backchannelNumber >= MaxNoOfBackchannels){
				backchannelNumber = 0;
			}
			// Backchannel input to MARY is hard-coded
			String words = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" 
			 + "<maryxml xmlns=\"http://mary.dfki.de/2002/MaryXML\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" version=\"0.4\" xml:lang=\"en-GB\">"
			 + "<p>"
			 //+ "<voice name=\"cmu-slt-arctic\">"
			 + "<voice name=\""+getCurrentCharacter().toLowerCase()+"\">"
			 + "<nvv variant=\""+backchannelNumber+"\"/>"
			 + "</voice>"
			 + "</p>"
			 + "</maryxml>";
			
			
			Voice voice = Voice.getDefaultVoice(Locale.ENGLISH);
			
			AudioFormat af = voice.dbAudioFormat();
	        AudioFileFormat aff = new AudioFileFormat(AudioFileFormat.Type.WAVE,
	            af, AudioSystem.NOT_SPECIFIED);
	        Request request = new Request(MaryDataType.WORDS, MaryDataType.AUDIO, Locale.ENGLISH, 
		            voice, "", "", 1, aff);
	        Reader reader = new StringReader(words);
			ByteArrayOutputStream audioos = new ByteArrayOutputStream();
			request.readInputData(reader);
			request.process();
			request.writeOutputData(audioos);
			audioSender.sendBytesMessage(audioos.toByteArray(),  xm.getUsertime());
			
			request = new Request(MaryDataType.get("WORDS"),MaryDataType.get("REALISED_ACOUSTPARAMS"),Locale.ENGLISH,voice,"","",1,aff);
			ByteArrayOutputStream  realisedOS = new ByteArrayOutputStream();
			reader = new StringReader(words);
			request.readInputData(reader);
			request.process();
			request.writeOutputData(realisedOS);
			
			String finalData = XMLTool.mergeTwoXMLFiles(inputText, realisedOS.toString(), SpeechBMLRealiser.class.getResourceAsStream("Backchannel-Realised-Merge.xsl"), "semaine.mary.realised.acoustics");
			bmlSender.sendTextMessage(finalData,  xm.getUsertime(), xm.getEventType());
		}
		else{
			
			// Utterance synthesis
			transformer = bml2ssmlStylesheet.newTransformer();
			transformer.setParameter("character.voice", getCurrentCharacter().toLowerCase());
			transformer.transform(new DOMSource(input), new StreamResult(ssmlos));
			
			// SSML to Realised Acoustics using MARY 
			Voice voice = Voice.getDefaultVoice(Locale.ENGLISH);
			AudioFormat af = voice.dbAudioFormat();
	        AudioFileFormat aff = new AudioFileFormat(AudioFileFormat.Type.WAVE,
	            af, AudioSystem.NOT_SPECIFIED);
			Request request = new Request(MaryDataType.get("SSML"),MaryDataType.get("REALISED_ACOUSTPARAMS"),Locale.ENGLISH,voice,"","",1,aff);
			
			Reader reader = new StringReader(ssmlos.toString());
			ByteArrayOutputStream  realisedOS = new ByteArrayOutputStream();
			request.readInputData(reader);
			request.process();
			request.writeOutputData(realisedOS);
			//Merge realised acoustics into output format
			String finalData = XMLTool.mergeTwoXMLFiles(inputText, realisedOS.toString(), SpeechBMLRealiser.class.getResourceAsStream("BML-RealisedSpeech-Merge.xsl"), "semaine.mary.realised.acoustics");
			log.debug("input bml: "+inputText);
			log.debug("output maryxml: "+realisedOS.toString());
			log.debug("merged speech output: "+finalData);
			bmlSender.sendTextMessage(finalData,  xm.getUsertime(), xm.getEventType());
			
			// SSML to AUDIO using MARY 
	        request = new Request(MaryDataType.SSML, MaryDataType.AUDIO, Locale.ENGLISH, 
	            voice, "", "", 1, aff);
			reader = new StringReader(ssmlos.toString());
			ByteArrayOutputStream audioos = new ByteArrayOutputStream();
			request.readInputData(reader);
			request.process();
			request.writeOutputData(audioos);
			audioSender.sendBytesMessage(audioos.toByteArray(),  xm.getUsertime());
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
	
    public String getCurrentCharacter()
    {
        return currentCharacter;
    }
	
}
