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
import eu.semaine.datatypes.stateinfo.StateInfo;
import eu.semaine.datatypes.xml.BML;
import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.exceptions.SystemConfigurationException;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.message.SEMAINEStateMessage;
import eu.semaine.jms.message.SEMAINEXMLMessage;
import eu.semaine.jms.receiver.BMLReceiver;
import eu.semaine.jms.receiver.StateReceiver;
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
	private static Templates bmlSpeechTimingRemoverStylesheet = null;
    private Transformer transformer;
    private int backchannelNumber = 0;
    private int MaxNoOfBackchannels = 4;
    private String currentCharacter = ParticipantControlGUI.PRUDENCE;
    
	/**
	 * @param componentName
	 * @throws JMSException
	 */
	public SpeechBMLRealiser() throws JMSException 
	{
		super("SpeechBMLRealiser");
		
		bmlSender = new BMLSender("semaine.data.synthesis.plan.speechtimings", getName());
		audioSender = new BytesSender("semaine.data.synthesis.lowlevel.audio","AUDIO",getName());
		senders.add(bmlSender);
		senders.add(audioSender); // so it can be started etc
		
		bmlPlanReceiver = new BMLReceiver("semaine.data.synthesis.plan");
		receivers.add(bmlPlanReceiver);
		receivers.add(new StateReceiver("semaine.data.state.context", StateInfo.Type.ContextState));
		
	}
	
	protected void customStartIO() throws Exception
	{
		if (tFactory == null) {
            tFactory = TransformerFactory.newInstance();
		 }
		
		//BML timing info removal
		StreamSource bmlstylesheetStream =
	        new StreamSource( SpeechBMLRealiser.class.getResourceAsStream(          
	                 "BMLSpeechTimingInfoRemover.xsl"));
		bmlSpeechTimingRemoverStylesheet = tFactory.newTemplates(bmlstylesheetStream);
		
		// BML2SSML conversion
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
		
		// if you receive a state message  
		if ( m instanceof SEMAINEStateMessage) {
			SEMAINEStateMessage ssm = (SEMAINEStateMessage) m;
			StateInfo state = ssm.getState();
			if (state.hasInfo("character")) {
                currentCharacter = state.getInfo("character");
                System.out.println(currentCharacter);
            } 
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
		ByteArrayOutputStream bmlos = new ByteArrayOutputStream();
		
		if(XMLTool.getChildElementByLocalNameNS(input.getDocumentElement(), BML.E_BACKCHANNEL, BML.namespaceURI) != null){
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
		else if(input.getDocumentElement().getElementsByTagNameNS(BML.namespaceURI, BML.E_SPEECH).getLength() == 0){
			bmlSender.sendXML(xm.getDocument(), xm.getUsertime(), xm.getEventType());
		}
		else{
			
			// BML speech timing info removal
			transformer = bmlSpeechTimingRemoverStylesheet.newTransformer();
			//transformer.setParameter("character.voice", "spike");
			transformer.transform(new DOMSource(input), new StreamResult(bmlos));
			String bml = bmlos.toString();
			//System.out.println("BML OUTPUT: "+ bml);
			
			
			// Utterance synthesis
			transformer = bml2ssmlStylesheet.newTransformer();
			transformer.setParameter("character.voice", getCurrentCharacter().toLowerCase());
			//transformer.transform(new DOMSource(input), new StreamResult(ssmlos));
			transformer.transform(new DOMSource(XMLTool.string2Document(bml)), new StreamResult(ssmlos));
			
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
	
    public String getCurrentCharacter()
    {
        return currentCharacter;
    }
	
}
