package eu.semaine.components.dialogue.actionproposers;

import java.io.InputStream;
import java.util.ArrayList;
import java.util.HashMap;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;

import eu.semaine.components.dialogue.datastructures.Response;

public class ResponseReader
{
	private HashMap<String,Response> responses = new HashMap<String,Response>();
	private HashMap<String,ArrayList<Response>> responseGroups = new HashMap<String,ArrayList<Response>>();
	private String filename;

	public ResponseReader( String filename )
	{
		this.filename = filename;
	}

	public boolean readData()
	{
		InputStream is = this.getClass().getResourceAsStream(filename);

		Document doc;
		try{
			DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
			factory.setExpandEntityReferences(true);
			factory.setNamespaceAware(true);
			DocumentBuilder docBuilder = factory.newDocumentBuilder();
			doc = docBuilder.parse(is);
		} catch (Exception e) {
			System.out.println("Error parsing sentences xml file");
			e.printStackTrace();
			return false;
		}
		if( doc == null ) {
			return false;
		}

		Element root = doc.getDocumentElement();
		if( root.getTagName().equals("responses") ) {
			HashMap<String,Response> charResponses = new HashMap<String,Response>();
			HashMap<String,ArrayList<Response>> charGroups = new HashMap<String,ArrayList<Response>>();

			/* Read Responses */
			NodeList responseElements = root.getElementsByTagName("response");
			for( int j=0; j<responseElements.getLength(); j++ ) {
				Element responseElement = (Element)responseElements.item(j);
				if( responseElement.hasAttribute("id") && responseElement.hasAttribute("response") ) {
					Response r = new Response(responseElement.getAttribute("id"),responseElement.getAttribute("response"));
					NodeList argElements = responseElement.getElementsByTagName("argument");
					for( int k=0; k<argElements.getLength(); k++ ) {
						Element argElement = (Element)argElements.item(k);
						if( argElement.hasAttribute("name") && argElement.hasAttribute("value") ) {
							r.addArgument(argElement.getAttribute("name"), argElement.getAttribute("value"));
						} else {
							System.out.println("Argument-element has wrong attributes");
						}
					}
					responses.put(responseElement.getAttribute("id"), r);
				} else {
					System.out.println("Response-element has wrong attributes");
				}
			}

			/* Read Groups*/
			NodeList groupElements = root.getElementsByTagName("group");
			for( int j=0; j<groupElements.getLength(); j++ ) {
				Element groupElement = (Element)groupElements.item(j);
				if( groupElement.hasAttribute("id") ) {
					ArrayList<Response> groupResponses = new ArrayList<Response>();
					NodeList memberElements = groupElement.getElementsByTagName("member");
					for( int k=0; k<memberElements.getLength(); k++ ) {
						Element memberElement = (Element)memberElements.item(k);
						if( memberElement.hasAttribute("id") ) {
							Response r = responses.get( memberElement.getAttribute("id") );
							if( r != null ) {
								groupResponses.add(r);
							}
						} else {
							System.out.println("Member-element has wrong attributes");
						}
					}
					responseGroups.put(groupElement.getAttribute("id"), groupResponses);
				} else {
					System.out.println("Group-element has wrong attributes");
				}
			}

		}

		return true;
	}

	/**
	 * @return the responses
	 */
	public HashMap<String, Response> getResponses() {
		return responses;
	}

	/**
	 * @return the responseGroups
	 */
	public HashMap<String, ArrayList<Response>> getResponseGroups() {
		return responseGroups;
	}
}
