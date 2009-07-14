package eu.semaine.components.dialogue.actionproposers;

import java.io.InputStream;
import java.util.ArrayList;
import java.util.HashMap;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;

public class SentenceReader
{
	private String filename = "";

	private HashMap<Integer,HashMap<String,ArrayList<String>>> allUtterances = new HashMap<Integer,HashMap<String,ArrayList<String>>>();
//	private HashMap<String,ArrayList<String>> utterancesPoppy = new HashMap<String,ArrayList<String>>();
//	private HashMap<String,ArrayList<String>> utterancesPrudence = new HashMap<String,ArrayList<String>>();
//	private HashMap<String,ArrayList<String>> utterancesSpike = new HashMap<String,ArrayList<String>>();
//	private HashMap<String,ArrayList<String>> utterancesObadiah = new HashMap<String,ArrayList<String>>();

	public SentenceReader( String filename )
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
		if( root.getTagName().equals("semaine_sentences") ) {
			NodeList characters = root.getElementsByTagName("character");
			for( int i=0; i<characters.getLength(); i++ ) {
				Element characterElem = (Element)characters.item(i);
				String characterName = characterElem.getAttribute("name");

				NodeList categories = characterElem.getElementsByTagName("category");
				for( int j=0; j<categories.getLength(); j++ ) {
					Element category = (Element)categories.item(j);
					String categoryName = category.getAttribute("name");

					NodeList utteranceList = category.getElementsByTagName("utterance");
					for( int k=0; k<utteranceList.getLength(); k++ ) {
						Element utteranceElem = (Element)utteranceList.item(k);
						String utterance = utteranceElem.getAttribute("value");
						int character = 0;

						if( characterName.toLowerCase().equals("poppy") ) {
							character = UtteranceActionProposer.POPPY;
						} else if( characterName.toLowerCase().equals("prudence") ) {
							character = UtteranceActionProposer.PRUDENCE;
						} else if( characterName.toLowerCase().equals("spike") ) {
							character = UtteranceActionProposer.SPIKE;
						} else if( characterName.toLowerCase().equals("obadiah") ) {
							character = UtteranceActionProposer.OBADIAH;
						}

						HashMap<String,ArrayList<String>> utterancesChar = allUtterances.get(character);
						if( utterancesChar == null ) {
							utterancesChar = new HashMap<String,ArrayList<String>>();
						}

						ArrayList<String> utterances = utterancesChar.get(categoryName);
						if( utterances == null ) {
							utterances = new ArrayList<String>();
						}
						utterances.add(utterance);
						utterancesChar.put(categoryName,utterances);
						allUtterances.put( character,utterancesChar );
					}
				}
			}

		}
		return true;
	}

	/**
	 * @return the allUtterances
	 */
	public HashMap<Integer, HashMap<String, ArrayList<String>>> getAllUtterances() {
		return allUtterances;
	}
}
