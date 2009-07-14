package eu.semaine.components.mary;


import java.io.ByteArrayOutputStream;
import java.io.FileReader;
import java.io.Reader;
import java.io.StringReader;
import java.util.Locale;

import javax.sound.sampled.AudioFileFormat;
import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioSystem;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
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
import org.xml.sax.InputSource;

import eu.semaine.datatypes.xml.BML;
import eu.semaine.exceptions.SystemConfigurationException;
import eu.semaine.util.XMLTool;


/**
 * A MARY component for simple test cases 
 * @author sathish
 *
 */
public class TestMaryComponent {

	private static TransformerFactory tFactory = null;
	private static Templates fml2ssmlStylesheet = null;
	private static Transformer transformer;
	
	
	/**
	 * @param args
	 * @throws Exception 
	 */
	public static void main(String[] args) throws Exception {
		// TODO Auto-generated method stub
		
		//System.setProperty("mary.base", "/home/sathish/Work/dfki400alpha/present/MARY399alpha");
		System.setProperty("mary.base", "/home/sathish/Work/semaine/openmary");
			
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
		
    	
    	ByteArrayOutputStream ssmlos = new ByteArrayOutputStream();
		//Request request = new Request(MaryDataType.SSML,MaryDataType.INTONATION,Locale.US,Voice.getDefaultVoice(Locale.ENGLISH),null,null,1,null);
		//Request request = new Request(MaryDataType.get("SSML"),MaryDataType.get("REALISED_ACOUSTPARAMS"),Locale.US,voice,"","",1,aff);
    	
    	Voice voice = Voice.getDefaultVoice(Locale.ENGLISH);
		AudioFormat af = voice.dbAudioFormat();
        AudioFileFormat aff = new AudioFileFormat(AudioFileFormat.Type.WAVE,
            af, AudioSystem.NOT_SPECIFIED);
    	Request request = new Request(MaryDataType.SSML,MaryDataType.get("REALISED_ACOUSTPARAMS"),Locale.ENGLISH,voice,"","",1,aff);
		DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
		factory.setNamespaceAware(true);
		DocumentBuilder builder = factory.newDocumentBuilder();
		Document inputDoc = builder.parse(new InputSource(new FileReader("/home/sathish/Desktop/SSML.xml")));
		String inputText = XMLTool.document2String(inputDoc);
		
		transformer = fml2ssmlStylesheet.newTransformer();
		
		transformer.setParameter("character.voice", "spike");

		transformer.transform(new DOMSource(inputDoc), new StreamResult(ssmlos));
		String ssml = ssmlos.toString();
		System.out.println("SSML OUTPUT: "+ ssml);
		Reader reader = new StringReader(ssml);
		ByteArrayOutputStream  intonationOS = new ByteArrayOutputStream();
		try {
			request.readInputData(reader);
			request.process();
			request.writeOutputData(intonationOS);
		} catch (Exception e) {
			throw new Exception("MARY cannot process input -- SSML input was:\n"+ssml, e);
		}
		
		System.out.println("Input Text : "+inputText);
		System.out.println("MARY OUT Text : "+intonationOS.toString());
		
		String words = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" 
			 + "<maryxml xmlns=\"http://mary.dfki.de/2002/MaryXML\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" version=\"0.4\" xml:lang=\"en-US\">"
			 + "<p>"
			 + "<voice name=\"cmu-slt-arctic\">"
			 + "<nvv variant=\""+3+"\"/>"
			 + "</voice>"
			 + "</p>"
			 + "</maryxml>";
		
		
		System.out.println("Words: "+words);
		//String finalData = XMLTool.mergeTwoXMLFiles(inputText, intonationOS.toString(), SpeechPreprocessor.class.getResourceAsStream("FML-Intonation-Merge.xsl"), "semaine.mary.intonation");
		//String finalData = XMLTool.mergeTwoXMLFiles(inputText, intonationOS.toString(), SpeechPreprocessor.class.getResourceAsStream("FML-RealisedSpeech-Merge.xsl"), "semaine.mary.realised.acoustics");
		String finalData = XMLTool.mergeTwoXMLFiles(inputText, intonationOS.toString(), SpeechPreprocessor.class.getResourceAsStream("BML-RealisedSpeech-Merge.xsl"), "semaine.mary.realised.acoustics");
		System.out.println("Final data: "+finalData);
		
		
	}

}
