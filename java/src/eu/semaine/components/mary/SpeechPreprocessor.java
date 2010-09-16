/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.mary;

import java.io.IOException;
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
import eu.semaine.datatypes.xml.FML;
import eu.semaine.datatypes.xml.SSML;
import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.exceptions.SystemConfigurationException;
import eu.semaine.jms.JMSLogger;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.message.SEMAINEStateMessage;
import eu.semaine.jms.message.SEMAINEXMLMessage;
import eu.semaine.jms.receiver.BMLReceiver;
import eu.semaine.jms.receiver.FMLReceiver;
import eu.semaine.jms.receiver.StateReceiver;
import eu.semaine.jms.sender.FMLSender;
import eu.semaine.system.CharacterConfigInfo;
import eu.semaine.util.SEMAINEUtils;
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
	private Templates mergingStylesheet;
	private String currentCharacter = CharacterConfigInfo.getDefaultCharacter().getName();
	
	/**
	 * @param componentName
	 * @throws JMSException
	 */
	public SpeechPreprocessor() throws JMSException 
	{
		this("SpeechPreprocessor", 
				"semaine.data.action.selected.function",
				"semaine.data.action.selected.behaviour",
				"semaine.data.action.selected.speechpreprocessed");
	}
	
	/**
	 * Protected constructor which must be called by every constructor
	 * @param compName
	 * @throws JMSException
	 */
	protected SpeechPreprocessor(String compName,
			String fmlReceiverTopic,
			String bmlReceiverTopic,
			String fmlbmlSenderTopic) throws JMSException {
		super(compName);
		
		this.fmlReceiver = new FMLReceiver(fmlReceiverTopic);
		this.bmlReceiver = new BMLReceiver(bmlReceiverTopic);
		receivers.add(fmlReceiver);
		receivers.add(bmlReceiver);
		receivers.add(new StateReceiver("semaine.data.state.context", StateInfo.Type.ContextState));

		this.fmlbmlSender = new FMLSender(fmlbmlSenderTopic, getName());
		senders.add(fmlbmlSender);
	}
	
	protected void customStartIO() throws Exception
	{
		if (tFactory == null) {
            tFactory = TransformerFactory.newInstance();
		 }
    	StreamSource stylesheetStream =
	        new StreamSource(SpeechPreprocessor.class.getResourceAsStream("FML2SSML.xsl"));
		fml2ssmlStylesheet = tFactory.newTemplates(stylesheetStream);
		
	    StreamSource stylesheetStream2 = new StreamSource(SpeechPreprocessor.class.getResourceAsStream("FML-RealisedSpeech-Merge.xsl"));
        mergingStylesheet = tFactory.newTemplates(stylesheetStream2);

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
            	if(!(Mary.STATE_SHUTTING_DOWN == Mary.currentState() || Mary.STATE_OFF == Mary.currentState())){
            		Mary.shutdown();
            	}
            }
        });
    	dummyTestRun();
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
		
		if (m.getTopicName().equals(fmlReceiver.getTopicName())) {
			speechPreProcessor(m);
		}
		
		if (m.getTopicName().equals(bmlReceiver.getTopicName())) {
			//SEMAINEXMLMessage xm = (SEMAINEXMLMessage)m;
			//fmlbmlSender.sendXML(xm.getDocument(), xm.getUsertime(), xm.getEventType());
		}
		
	}
	
	/**
	 * For an FML-APML or BML message, find the content of the 'language' attribute of the speech tag,
	 * and return a corresponding Locale object.
	 * @param xm
	 * @return the message locale, or null if none could be found.
	 */
	public static Locale getMessageLocale(SEMAINEXMLMessage xm) {
		Document doc = xm.getDocument();
		if (doc == null) return null;
		Element root = doc.getDocumentElement();
		if (root == null) return null;
		// Now either root is <bml>, or it has a <bml> child:
		Element bml;
		if (root.getLocalName().equals(BML.E_BML) && root.getNamespaceURI().equals(BML.namespaceURI)) {
			bml = root;
		} else {
			bml = XMLTool.getChildElementByLocalNameNS(root, BML.E_BML, BML.namespaceURI);
		}
		if (bml == null) return null;
		Element speech = XMLTool.getChildElementByLocalNameNS(bml, BML.E_SPEECH, BML.namespaceURI);
		if (speech == null) return null;
		String language = speech.getAttribute("language");
		if (language.equals("")) return null;
		return SEMAINEUtils.string2locale(language);
	}
	
	/**
	 * Find a suitable voice for the given combination of FML-APML message and character name.
	 * Normally, the character name will correspond to an entry in the CharacterConfigInfo, and
	 * its voice locale will match the message's locale. If either of the two is not available,
	 * the method tries to get a good approximation.
	 * @param xm an FML-APML message
	 * @param character a character name
	 * @return a voice object
	 * @throws SystemConfigurationException if no voice can be found for the given character
	 */
	public static Voice getVoice(SEMAINEXMLMessage xm, String character) throws SystemConfigurationException {
		Locale messageLocale = null;
		Voice voice = null;
		if (xm != null) {
			messageLocale = getMessageLocale(xm);
		}
		CharacterConfigInfo charInfo = CharacterConfigInfo.getInfo(character);
		if (charInfo == null) {
			throw new SystemConfigurationException("No config information for character '"+character+"'");
		}
		Locale characterLocale = charInfo.getVoiceLocale();
		if (messageLocale != null && !messageLocale.equals(characterLocale)) {
			JMSLogger.getLog("SpeechPreprocessor").warn("Message locale '"+messageLocale+"' is different from character '"+character+"' locale '"
					+characterLocale+"' -- will ignore character.");
			voice = Voice.getDefaultVoice(messageLocale);
			if (voice == null) {
				throw new SystemConfigurationException("No voice available for locale '"+messageLocale+"'");
			}
		} else {
			String[] possibleVoices = charInfo.getVoices();
			for (String v : possibleVoices) {
				voice = Voice.getVoice(v);
				if (voice != null) {
					break;
				}
			}
			if (voice == null) {
				String message = "None of the voices defined for character '"+charInfo.getName()+"' is available in MARY TTS. ";
				Voice fallbackVoice = Voice.getDefaultVoice(characterLocale);
				if (fallbackVoice == null) {
					throw new SystemConfigurationException(message+"No fallback available.");
				} else {
					JMSLogger.getLog("SpeechPreprocessor").warn(message+"Will fall back to voice '"+fallbackVoice.getName()+"'");
					voice = fallbackVoice;
				}
			} else if (!voice.getLocale().equals(characterLocale)) {
				String message = "Voice '"+voice.getName()+"' defined for character '"+charInfo.getName()
					+"' has locale '"+voice.getLocale()+"' but character expects '"+characterLocale+"'. ";
				Voice fallbackVoice = Voice.getDefaultVoice(characterLocale);
				if (fallbackVoice == null) {
					throw new SystemConfigurationException(message+"No fallback available.");
				} else {
					JMSLogger.getLog("SpeechPreprocessor").warn(message+"Will fall back to voice '"+fallbackVoice.getName()+"'");
					voice = fallbackVoice;
				}
			}
		}
		assert voice != null;
		return voice;
	}
	
	/**
	 * For the given combination of FML-APML message and character name, if they fit together,
	 * return any voice effects listed in the character's entry in the CharacterConfigInfo.
	 * @param xm an FML-APML message
	 * @param character a character name
	 * @return a string containing voice effects, or the empty string.
	 */
	public static String getVoiceEffects(SEMAINEXMLMessage xm, String character) {
		Locale messageLocale = null;
		if (xm != null) {
			messageLocale = getMessageLocale(xm);
		}
		CharacterConfigInfo charInfo = CharacterConfigInfo.getInfo(character);
		if (charInfo != null && (messageLocale == null || charInfo.getVoiceLocale().equals(messageLocale))) {
			return charInfo.getVoiceEffects();
		}
		return "";
	}
	
	
	/**
	 * Speech Preprocessor
	 */
	private void speechPreProcessor(SEMAINEMessage m) throws Exception
	{	
		
		SEMAINEXMLMessage xm = (SEMAINEXMLMessage)m;
		
    	Document inputDoc = xm.getDocument();
		
		String localName    = xm.getDocument().getDocumentElement().getLocalName();
		String namespaceURI = xm.getDocument().getDocumentElement().getNamespaceURI(); 
		
		if (XMLTool.getChildElementByLocalNameNS(inputDoc.getDocumentElement(), BML.E_BML, BML.namespaceURI) != null) {
			// An FML-APML document with a BML child element -- Text-to-Speech mode:
			Voice voice = getVoice(xm, getCurrentCharacter());
			String voiceEffects = getVoiceEffects(xm, getCurrentCharacter());
			AudioFormat af = voice.dbAudioFormat();
	        AudioFileFormat aff = new AudioFileFormat(AudioFileFormat.Type.WAVE, af, AudioSystem.NOT_SPECIFIED);
	    	Request request = new Request(MaryDataType.SSML, MaryDataType.REALISED_ACOUSTPARAMS, 
	    			voice.getLocale(), voice, voiceEffects, "", 1, aff);
			
			Transformer transformer = fml2ssmlStylesheet.newTransformer();
			transformer.setParameter("character.voice", voice.getName());

			// Marc, 27.11.09:
			// Use DOM rather than strings so we can replace identifiers in document with
			// standard ones, and translate them back in the result:
			long tStart = System.currentTimeMillis();
			DOMResult ssmlDR = new DOMResult();
			transformer.transform(new DOMSource(inputDoc), ssmlDR);
			Document ssmlDoc = (Document) ssmlDR.getNode();
			long t1 = System.currentTimeMillis();
			long durExtractSSML = t1 - tStart;
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
			long t2 = System.currentTimeMillis();
			long durReplace1 = t2 - t1;
			
			// This costs as much time as a call to cached mary (~10 ms):
			//String ssmlString = XMLTool.document2String(ssmlDoc);
			//log.debug("Sending the following SSML to MARY:\n"+ssmlString);
			
			long t3 = System.currentTimeMillis();
			long durToString = t3 - t2;
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
			long t4 = System.currentTimeMillis();
			long durMary = t4 - t3;
			// Replace placeholders in maryDoc back into original names:
			NodeIterator ni2 = XMLTool.createNodeIterator(maryDoc, maryDoc, maryDoc.getDocumentElement().getNamespaceURI(), "mark");
			Element elt2 = null;
			while ((elt2 = (Element)ni2.nextNode()) != null) {
				String placeholder = elt2.getAttribute("name");
				String name = placeholders.get(placeholder);
				elt2.setAttribute("name", name);
			}
			long t5 = System.currentTimeMillis();
			long durReplace2 = t5 - t4;
			
			
			//String finalData = XMLTool.mergeTwoXMLFiles(inputText, intonationOS.toString(), SpeechPreprocessor.class.getResourceAsStream("FML-Intonation-Merge.xsl"), "semaine.mary.intonation");
			//String finalData = XMLTool.mergeTwoXMLFiles(inputText, intonationOS.toString(), SpeechPreprocessor.class.getResourceAsStream("FML-RealisedSpeech-Merge.xsl"), "semaine.mary.realised.acoustics");
			Document finalData = XMLTool.mergeTwoXMLFiles(inputDoc, maryDoc, mergingStylesheet, "semaine.mary.realised.acoustics");
			long t6 = System.currentTimeMillis();
			long durMerge = t6 - t5;
			fmlbmlSender.sendXML(finalData, meta.getTime(), xm.getEventType(), xm.getContentID(), xm.getContentCreationTime());
			long t7 = System.currentTimeMillis();
			long durSend = t7 - t7;
			log.debug("Times needed:\n"+durExtractSSML+" extract SSML\n"+durReplace1+" replace1\n"+durToString+" to string\n"+durMary+" MARY\n"
					+durReplace2+" replace2\n"+durMerge+" merge\n"+durSend+" send\n");
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
	
    /**
     * Run the full processing queue once, including XML parsing, XSLT transformation back and forth, and MARY processing.
     * The idea is to trigger just-in-time compilation by Java and thus avoid stalling at the first message.
     */
    private void dummyTestRun()
    throws IOException, SAXException, TransformerConfigurationException, TransformerException, Exception {
    	Document testFmlApml = XMLTool.parse(this.getClass().getResourceAsStream("fml-apml-example.xml"));
		Transformer transformer = fml2ssmlStylesheet.newTransformer();
		Voice voice = getVoice(null, getCurrentCharacter());
		transformer.setParameter("character.voice", voice.getName());
		DOMResult ssmlDR = new DOMResult();
		transformer.transform(new DOMSource(testFmlApml), ssmlDR);
		Document ssmlDoc = (Document) ssmlDR.getNode();

		AudioFormat af = voice.dbAudioFormat();
        AudioFileFormat aff = new AudioFileFormat(AudioFileFormat.Type.WAVE, af, AudioSystem.NOT_SPECIFIED);
		String voiceEffects = getVoiceEffects(null, getCurrentCharacter());
    	Request request = new Request(MaryDataType.SSML, MaryDataType.REALISED_ACOUSTPARAMS, voice.getLocale(), voice, voiceEffects, "", 1, aff);
		MaryData ssmlData = new MaryData(request.getInputType(), request.getDefaultLocale());
		ssmlData.setDocument(ssmlDoc);
		request.setInputData(ssmlData);
		request.process();
		MaryData maryOut = request.getOutputData();
		Document maryDoc = maryOut.getDocument();
		/*Document finalData = */XMLTool.mergeTwoXMLFiles(testFmlApml, maryDoc, mergingStylesheet, "semaine.mary.realised.acoustics");
    }
    
}
