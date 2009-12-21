/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.mary;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
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
import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMResult;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamSource;

import marytts.datatypes.MaryData;
import marytts.datatypes.MaryDataType;
import marytts.modules.synthesis.Voice;
import marytts.server.Mary;
import marytts.server.Request;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.traversal.NodeIterator;
import org.xml.sax.SAXException;

import eu.semaine.components.Component;
import eu.semaine.components.control.ParticipantControlGUI;
import eu.semaine.datatypes.stateinfo.StateInfo;
import eu.semaine.datatypes.xml.BML;
import eu.semaine.datatypes.xml.SSML;
import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.exceptions.SystemConfigurationException;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.message.SEMAINEStateMessage;
import eu.semaine.jms.message.SEMAINEXMLMessage;
import eu.semaine.jms.receiver.BMLReceiver;
import eu.semaine.jms.receiver.StateReceiver;
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
	private static Templates bml2BackchannelStylesheet = null;
	private static Templates bmlSpeechTimingRemoverStylesheet = null;
	private Templates mergingStylesheet;
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
	        new StreamSource( SpeechBMLRealiser.class.getResourceAsStream("BML2SSML.xsl"));
		bml2ssmlStylesheet = tFactory.newTemplates(stylesheetStream);
		
	    StreamSource stylesheetStream2 = new StreamSource(SpeechPreprocessor.class.getResourceAsStream("FML-RealisedSpeech-Merge.xsl"));
        mergingStylesheet = tFactory.newTemplates(stylesheetStream2);
        
        // BML2backchannel conversion
        StreamSource bml2BackchannelStream =
	        new StreamSource( SpeechBMLRealiser.class.getResourceAsStream("BML2Backchannel.xsl"));
		bml2BackchannelStylesheet = tFactory.newTemplates(bml2BackchannelStream);

		
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
            	if(!(Mary.STATE_SHUTTING_DOWN == Mary.currentState() || Mary.STATE_OFF == Mary.currentState())) {
            		Mary.shutdown();
            	}
            }
        });
        dummyTestRun();
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
                //System.out.println(currentCharacter);
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
		
		boolean isBackChannel = isBackChannelRequest(xm);
		
		
		if(isBackChannel){
			// Back-channel synthesis
			Element bmlSpeech = XMLTool.getChildElementByLocalNameNS(input.getDocumentElement(), BML.E_SPEECH, BML.namespaceURI);
			if(bmlSpeech == null) {
				bmlSender.sendXML(xm.getDocument(), meta.getTime(), xm.getEventType(), xm.getContentID(), xm.getContentCreationTime());
				return;
			}
			
			Transformer backchannelTransformer = bml2BackchannelStylesheet.newTransformer();
			backchannelTransformer.setParameter("character.voice", SpeechPreprocessor.characters2voices.get(getCurrentCharacter()));
			DOMResult backchannelDR = new DOMResult();
			backchannelTransformer.transform(new DOMSource(input), backchannelDR);
			Document bcWordDoc = (Document) backchannelDR.getNode();
			String words = XMLTool.document2String(bcWordDoc);
			
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
			audioSender.sendBytesMessage(audioos.toByteArray(),  xm.getUsertime(), xm.getContentID(), xm.getContentCreationTime());
			
			request = new Request(MaryDataType.get("WORDS"),MaryDataType.get("REALISED_ACOUSTPARAMS"),Locale.ENGLISH,voice,"","",1,aff);
			ByteArrayOutputStream  realisedOS = new ByteArrayOutputStream();
			reader = new StringReader(words);
			request.readInputData(reader);
			request.process();
			request.writeOutputData(realisedOS);
			//System.out.println("MARY OUT: " + realisedOS.toString());
			String finalData = XMLTool.mergeTwoXMLFiles(inputText, realisedOS.toString(), SpeechBMLRealiser.class.getResourceAsStream("Backchannel-Realised-Merge.xsl"), "semaine.mary.realised.acoustics");
			//System.out.println("Final data : " + finalData);
			bmlSender.sendTextMessage(finalData,  xm.getUsertime(), xm.getEventType(), xm.getContentID(), xm.getContentCreationTime());
		}
		else if(input.getDocumentElement().getElementsByTagNameNS(BML.namespaceURI, BML.E_SPEECH).getLength() == 0){
			bmlSender.sendXML(xm.getDocument(), meta.getTime(), xm.getEventType(), xm.getContentID(), xm.getContentCreationTime());
		}
		else{
			
			long t0 = System.currentTimeMillis();
			// BML speech timing info removal
			Transformer transformer = bmlSpeechTimingRemoverStylesheet.newTransformer();
			//transformer.setParameter("character.voice", "spike");
			long t1 = System.currentTimeMillis();
			long durCreateTransformer = t1 - t0;
			
			// Marc, 27.11.09:
			// Use DOM rather than strings so we can replace identifiers in document with
			// standard ones, and translate them back in the result:

			DOMResult bmlDR = new DOMResult();
			transformer.transform(new DOMSource(input), bmlDR);
			Document bmlDoc = (Document) bmlDR.getNode();
			long t2 = System.currentTimeMillis();
			long durExtractBML = t2 - t1;
			
			// Utterance synthesis
			Transformer transformer2 = bml2ssmlStylesheet.newTransformer();
			transformer2.setParameter("character.voice", SpeechPreprocessor.characters2voices.get(getCurrentCharacter()));
			//transformer.transform(new DOMSource(input), new StreamResult(ssmlos));
			DOMResult ssmlDR = new DOMResult();
			transformer2.transform(new DOMSource(bmlDoc), ssmlDR);
			Document ssmlDoc = (Document) ssmlDR.getNode();
			
			long t3 = System.currentTimeMillis();
			long durExtractSSML = t3 - t2;
			
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
			long t4 = System.currentTimeMillis();
			long durReplace1 = t4 - t3;
			
			// document2String is very expensive, commenting out:
			//String ssmlString = XMLTool.document2String(ssmlDoc);
			//log.debug("Sending the following SSML to MARY:\n"+ssmlString);
			long t5 = System.currentTimeMillis();
			long durToString1 = t5 - t4;
			
			// SSML to Realised Acoustics using MARY 
			Voice voice = Voice.getDefaultVoice(Locale.ENGLISH);
			AudioFormat af = voice.dbAudioFormat();
	        AudioFileFormat aff = new AudioFileFormat(AudioFileFormat.Type.WAVE,
	            af, AudioSystem.NOT_SPECIFIED);
			Request request = new Request(MaryDataType.SSML,MaryDataType.REALISED_ACOUSTPARAMS,Locale.ENGLISH,voice,"","",1,aff);

			Document maryDoc = null;
			try {
				MaryData ssmlData = new MaryData(request.getInputType(), request.getDefaultLocale());
				ssmlData.setDocument(ssmlDoc);
				request.setInputData(ssmlData);
				request.process();
				MaryData maryOut = request.getOutputData();
				maryDoc = maryOut.getDocument();
			} catch (Exception e) {
				throw new Exception("MARY cannot process input -- SSML input was:\n"+XMLTool.document2String(ssmlDoc), e);
			}
			assert maryDoc != null;
			long t6 = System.currentTimeMillis();
			long durRealisedAc = t6 - t5;
			
			// Replace placeholders in maryDoc back into original names:
			NodeIterator ni2 = XMLTool.createNodeIterator(maryDoc, maryDoc, maryDoc.getDocumentElement().getNamespaceURI(), "mark");
			Element elt2 = null;
			while ((elt2 = (Element)ni2.nextNode()) != null) {
				String placeholder = elt2.getAttribute("name");
				String name = placeholders.get(placeholder);
				elt2.setAttribute("name", name);
			}
			long t7 = System.currentTimeMillis();
			long durReplace2 = t7 - t6;
			
			
			
			//String finalData = XMLTool.mergeTwoXMLFiles(inputText, intonationOS.toString(), SpeechPreprocessor.class.getResourceAsStream("FML-Intonation-Merge.xsl"), "semaine.mary.intonation");
			//String finalData = XMLTool.mergeTwoXMLFiles(inputText, intonationOS.toString(), SpeechPreprocessor.class.getResourceAsStream("FML-RealisedSpeech-Merge.xsl"), "semaine.mary.realised.acoustics");
			Document finalData = XMLTool.mergeTwoXMLFiles(input, maryDoc, mergingStylesheet, "semaine.mary.realised.acoustics");
			long t8 = System.currentTimeMillis();
			long durMerge = t8 - t7;

			// document2String is very expensive, commenting out:
			//log.debug("input bml: "+XMLTool.document2String(input));
			//log.debug("output maryxml: "+XMLTool.document2String(maryDoc));
			//log.debug("merged speech output: "+XMLTool.document2String(finalData));
			long t9 = System.currentTimeMillis();
			long durToString2 = t9 - t8;
			
			bmlSender.sendXML(finalData,  meta.getTime(), xm.getEventType(), xm.getContentID(), xm.getContentCreationTime());
			long t10 = System.currentTimeMillis();
			long durSendXML = t10 - t9;
			
			// SSML to AUDIO using MARY 
	        request = new Request(MaryDataType.SSML, MaryDataType.AUDIO, Locale.ENGLISH, 
	            voice, "", "", 1, aff);
			ByteArrayOutputStream audioos = new ByteArrayOutputStream();
			try {
				MaryData ssmlData = new MaryData(request.getInputType(), request.getDefaultLocale());
				ssmlData.setDocument(ssmlDoc);
				request.setInputData(ssmlData);
				request.process();
				request.writeOutputData(audioos);
			} catch (Exception e) {
				throw new Exception("MARY cannot process input -- SSML input was:\n"+XMLTool.document2String(ssmlDoc), e);
			}
			long t11 = System.currentTimeMillis();
			long durAudio = t11 - t10;
			audioSender.sendBytesMessage(audioos.toByteArray(),  meta.getTime(), xm.getContentID(), xm.getContentCreationTime());
			long t12 = System.currentTimeMillis();
			long durSendAudio = t12 - t11;
			log.debug("Times needed:\n"+durCreateTransformer+" create transformer\n"+durExtractBML+" extract BML\n"+durExtractSSML+" extract SSML\n"
					+durReplace1+" replace1\n"+durToString1+" to string1\n"+durRealisedAc+" MARY realised acoustparams\n"
					+durReplace2+" replace2\n"+durMerge+" merge\n"+durToString2+" to string2\n"+durSendXML+" send xml\n"
					+durAudio+" MARY audio\n"+durSendAudio+" send audio\n");

		}
	}
	
    private boolean isBackChannelRequest(SEMAINEXMLMessage xm) {
    	Document input = xm.getDocument();
    	Element bmlSpeech = XMLTool.getChildElementByLocalNameNS(input.getDocumentElement(), BML.E_SPEECH, BML.namespaceURI);
    	
    	if((bmlSpeech == null) || XMLTool.getChildElementByLocalNameNS(bmlSpeech, SSML.E_MARK, SSML.namespaceURI) == null){
    		return true;
    	}
    	
		return false;
	}

	public String getCurrentCharacter()
    {
        return currentCharacter;
    }
	
    /**
     * Run the full processing queue once, including XML parsing, XSLT transformation back and forth, and MARY processing.
     * The idea is to trigger just-in-time compilation by Java and thus avoid stalling at the first message.
     */
    private void dummyTestRun()
    throws IOException, SAXException, TransformerConfigurationException, TransformerException, Exception {
    	Document testBml = XMLTool.parse(this.getClass().getResourceAsStream("bml-example.xml"));
		Transformer transformer = bmlSpeechTimingRemoverStylesheet.newTransformer();
		DOMResult bmlDR = new DOMResult();
		transformer.transform(new DOMSource(testBml), bmlDR);
		Document bmlDoc = (Document) bmlDR.getNode();
		Transformer transformer2 = bml2ssmlStylesheet.newTransformer();
		transformer2.setParameter("character.voice", SpeechPreprocessor.characters2voices.get(getCurrentCharacter()));
		DOMResult ssmlDR = new DOMResult();
		transformer2.transform(new DOMSource(bmlDoc), ssmlDR);
		Document ssmlDoc = (Document) ssmlDR.getNode();

		Voice voice = Voice.getDefaultVoice(Locale.ENGLISH);
		AudioFormat af = voice.dbAudioFormat();
        AudioFileFormat aff = new AudioFileFormat(AudioFileFormat.Type.WAVE, af, AudioSystem.NOT_SPECIFIED);
    	Request request = new Request(MaryDataType.SSML,MaryDataType.REALISED_ACOUSTPARAMS,Locale.ENGLISH,voice,"","",1,aff);
		MaryData ssmlData = new MaryData(request.getInputType(), request.getDefaultLocale());
		ssmlData.setDocument(ssmlDoc);
		request.setInputData(ssmlData);
		request.process();
		MaryData maryOut = request.getOutputData();
		Document maryDoc = maryOut.getDocument();
		/*Document finalData = */XMLTool.mergeTwoXMLFiles(testBml, maryDoc, mergingStylesheet, "semaine.mary.realised.acoustics");
		
        request = new Request(MaryDataType.SSML, MaryDataType.AUDIO, Locale.ENGLISH, voice, "", "", 1, aff);
		ByteArrayOutputStream audioos = new ByteArrayOutputStream();
		MaryData ssmlData2 = new MaryData(request.getInputType(), request.getDefaultLocale());
		ssmlData2.setDocument(ssmlDoc);
		request.setInputData(ssmlData2);
		request.process();
		request.writeOutputData(audioos);
    }

	
}
