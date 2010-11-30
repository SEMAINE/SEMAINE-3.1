package eu.semaine.components.dialogue.behaviourselection;

import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.util.ArrayList;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;
import org.xml.sax.ErrorHandler;
import org.xml.sax.SAXException;
import org.xml.sax.SAXParseException;

import eu.semaine.components.dialogue.behaviourselection.template.Template;
import eu.semaine.components.dialogue.exceptions.TemplateParseException;

/**
 * The TemplateParser is given a String filename, parses the XML-file, and returns a list of Templates found in that File.
 * 
 * @author Mark ter Maat
 * @version 0.1
 */

public class TemplateParser
{
	/* The current filename */
	protected String filename;
	
	/**
	 * Creates a new TemplateParser
	 */
	public TemplateParser()
	{
		
	}
	
	/**
	 *  Given the Filename, find and parse the file, and return a list of found Templates
	 * 
	 * @param filename - the file to parse
	 * @return a list of Templates
	 * @throws ParserConfigurationException
	 * @throws SAXException
	 * @throws IOException
	 */
	public ArrayList<Template> parseFile( String filename ) throws ParserConfigurationException,SAXException,IOException
	{
		/* Find the file */
		this.filename = filename;
		InputStream inStream = this.getClass().getResourceAsStream(filename);
		if( inStream == null ) {
			throw new IOException("File '"+filename+"' does not exists");
		}
		ArrayList<Template> templates = new ArrayList<Template>();
		
		/* Create a new Document-builder, based on the specified XSD */
		DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
		factory.setNamespaceAware(true);
		factory.setValidating(true);
		
		//URL schemaUrl = new URL("file:bin/eu/semaine/components/dialogue/data/templates/template.xsd");
		URL xsdURL = this.getClass().getResource("/eu/semaine/components/dialogue/data/templates/template.xsd");
		
		factory.setAttribute("http://java.sun.com/xml/jaxp/properties/schemaLanguage", "http://www.w3.org/2001/XMLSchema");
		factory.setAttribute("http://java.sun.com/xml/jaxp/properties/schemaSource", xsdURL.toString());
		DocumentBuilder docBuilder = factory.newDocumentBuilder();
		docBuilder.setErrorHandler( new XSDErrorHandler() );
		Document doc = docBuilder.parse(inStream);
		
		/* Find all <template>-Elements, and parse those */
		NodeList templateNodeList = doc.getElementsByTagName("template");
		for( int i=0; i<templateNodeList.getLength(); i++ ) {
			Element templateElement = (Element)templateNodeList.item(i);
			try {
				Template template = Template.parseTemplateElement( templateElement );
				templates.add(template);
			}catch( TemplateParseException e ) {
				System.out.println( "Template Parse Error in '"+filename+"': " + e.getMessage() );
			}
		}
		
		return templates;
	}
	
	/**
	 * Inner Class to handle XSD-Errors
	 * @author mark
	 * @version 0.1
	 */
	public class XSDErrorHandler implements ErrorHandler
	{
		public void error(SAXParseException exception) {
			System.out.println("XSD Error ("+filename+"): "+ exception.getMessage());
		}

		public void fatalError(SAXParseException exception) {
			System.out.println("XSD Error ("+filename+"): "+ exception.getMessage());
		}

		public void warning(SAXParseException exception) {
			System.out.println("XSD Error ("+filename+"): "+ exception.getMessage());
		}
	}
}
