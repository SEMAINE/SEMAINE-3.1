/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.mary;

import java.io.ByteArrayOutputStream;
import java.io.Reader;
import java.io.StringReader;
import java.io.StringWriter;
import java.net.URL;
import java.net.URLClassLoader;
import java.util.Locale;

import javax.jms.JMSException;
import javax.sound.sampled.AudioFileFormat;
import javax.sound.sampled.AudioFormat;
import javax.xml.transform.Templates;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;
import javax.xml.transform.stream.StreamSource;

import marytts.Version;
import marytts.datatypes.MaryDataType;
import marytts.datatypes.MaryXML;
import marytts.datatypes.MaryDataType.Traits;
import marytts.modules.MaryModule;
import marytts.modules.ModuleRegistry;
import marytts.modules.Synthesis;
import marytts.modules.synthesis.Voice;
import marytts.server.EnvironmentChecks;
import marytts.server.Mary;
import marytts.server.MaryProperties;
import marytts.server.Request;

import java.lang.reflect.Method;

import org.apache.log4j.BasicConfigurator;
import org.apache.log4j.Category;
import org.apache.log4j.FileAppender;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.apache.log4j.PatternLayout;
import org.apache.log4j.WriterAppender;
import org.w3c.dom.DOMImplementation;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.bootstrap.DOMImplementationRegistry;
import org.w3c.dom.ls.DOMImplementationLS;
import org.w3c.dom.ls.LSOutput;
import org.w3c.dom.ls.LSSerializer;
import org.xml.sax.InputSource;

import eu.semaine.components.Component;
import eu.semaine.datatypes.xml.BML;
import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.exceptions.SystemConfigurationException;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.message.SEMAINEXMLMessage;
import eu.semaine.jms.receiver.BMLReceiver;
import eu.semaine.jms.receiver.FMLReceiver;
import eu.semaine.jms.sender.BMLSender;
import eu.semaine.jms.sender.FMLSender;
import eu.semaine.util.XMLTool;

/**
 * Speech BML realiser
 * 
 * @author Sathish Chandra Pammi
 *
 */
public class SpeechPreProcessor extends Component 
{
	private FMLReceiver fmlReceiver;
	private BMLReceiver bmlReceiver;
	private FMLSender fmlbmlSender;
	private static TransformerFactory tFactory = null;
	private static Templates stylesheet = null;
    private static Logger logger; // only used for extensive debug output
    private Transformer transformer;
    
	/**
	 * @param componentName
	 * @throws JMSException
	 */
	public SpeechPreProcessor() throws JMSException 
	{
		super("SpeechPreProcessor");
		fmlReceiver = new FMLReceiver("semaine.data.action.selected.function");
		receivers.add(fmlReceiver); // to set up properly
		bmlReceiver = new BMLReceiver("semaine.data.action.selected.behaviour");
		receivers.add(bmlReceiver);
		
		fmlbmlSender = new FMLSender("semaine.data.action.selected.speechpreprocess", getName());
		senders.add(fmlbmlSender); // so it can be started etc
	}
	
	protected void customStartIO() throws JMSException{
		
	    try {
        	long startTime = System.currentTimeMillis();
        	Mary.addJarsToClasspath();
            // Read properties:
            // (Will throw exceptions if problems are found)
			MaryProperties.readProperties();
			System.err.print("MARY server " + Version.specificationVersion() + " starting...");
			Mary.startup();
        	
			//startup();
            Runtime.getRuntime().addShutdownHook(new Thread() {
                public void run() {
                    Mary.shutdown();
                }
            });
            System.err.println(" started in " + (System.currentTimeMillis()-startTime)/1000. + " s");
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
  
	@Override
	public void react(SEMAINEMessage m) throws JMSException
	{
		if (!(m instanceof SEMAINEXMLMessage)) {
			throw new MessageFormatException("expected XML message, got "+m.getClass().getSimpleName());
		}
		
		SEMAINEXMLMessage xm = (SEMAINEXMLMessage)m;
		ByteArrayOutputStream ssmlos = new ByteArrayOutputStream();
		boolean isFML = "FML".equals(xm.getDatatype());
		Request request = new Request(MaryDataType.SSML,MaryDataType.INTONATION,Locale.US,Voice.getDefaultVoice(Locale.ENGLISH),null,null,1,null);
		try{
		if (isFML) {
			
			
        	// part 1
			Document inputDoc = xm.getDocument();
			String inputText = xm.getText();
			//System.out.println("Printing Message Text content: "+inputText);
			
			/*// part 2
			 * Document inputDoc;
            DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
			factory.setNamespaceAware(true);
			DocumentBuilder builder = factory.newDocumentBuilder();
			inputDoc = builder.parse(new InputSource(new FileReader("dataformat1.xml")));
			String outData = XMLTool.document2String(inputDoc);
			*/
			
			if (tFactory == null) {
	            tFactory = TransformerFactory.newInstance();
			 }
			if (stylesheet == null) {
		        StreamSource stylesheetStream =
		        new StreamSource( SpeechPreProcessor.class.getResourceAsStream(          
		                 "FML2SSML.xsl"));
		        stylesheet = tFactory.newTemplates(stylesheetStream);
		     }
			transformer = stylesheet.newTransformer();
			transformer.transform(new DOMSource(inputDoc), new StreamResult(ssmlos));
				
			Reader reader = new StringReader(ssmlos.toString());
			ByteArrayOutputStream  intonationOS = new ByteArrayOutputStream();
			request.readInputData(reader);
			request.process();
			request.writeOutputData(intonationOS);	
			
			String finalData = XMLTool.mergeTwoXMLFiles(inputText, intonationOS.toString(), SpeechPreProcessor.class.getResourceAsStream("FML-Intonation-Merge.xsl"), "semaine.mary.intonation");
			
			// need to merge to get 'DataFormat2'
			//System.out.println("********** PREPROCESSOR OUT: ********");
			//System.out.println(finalData);
			//System.out.println("******************");
			fmlbmlSender.sendTextMessage(finalData, xm.getUsertime(), xm.getEventType());
			
		} else { // BML -- send as is
			fmlbmlSender.sendXML(xm.getDocument(), xm.getUsertime(), xm.getEventType());
		}
		
		} catch (TransformerConfigurationException e2) {
			// TODO Auto-generated catch block
			e2.printStackTrace();
		} catch (TransformerException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
