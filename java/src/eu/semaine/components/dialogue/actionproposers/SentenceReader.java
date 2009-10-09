package eu.semaine.components.dialogue.actionproposers;

import java.io.InputStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.HashMap;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;

import eu.semaine.components.dialogue.datastructures.AgentUtterance;

public class SentenceReader
{
	private String filename = "";

	private HashMap<Integer,HashMap<String,ArrayList<AgentUtterance>>> allUtterances = new HashMap<Integer,HashMap<String,ArrayList<AgentUtterance>>>();
	
	private HashMap<Integer,HashMap<String,AgentUtterance>> allUtterancesWithoutGroup = new HashMap<Integer,HashMap<String,AgentUtterance>>();
	private HashMap<Integer,HashMap<AgentUtterance,HashMap<ArrayList<String>,String>>> allLinks = new HashMap<Integer,HashMap<AgentUtterance,HashMap<ArrayList<String>,String>>>();
	private HashMap<Integer,HashMap<AgentUtterance,String>> allElaborations = new HashMap<Integer,HashMap<AgentUtterance,String>>(); 

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
						/* Get utterance */
						Element utteranceElem = (Element)utteranceList.item(k);
						String utterance = utteranceElem.getAttribute("value");
						
						/* Get character */
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

						/* Get corresponding utterance-list of that character */
						HashMap<String,ArrayList<AgentUtterance>> utterancesChar = allUtterances.get(character);
						if( utterancesChar == null ) {
							utterancesChar = new HashMap<String,ArrayList<AgentUtterance>>();
						}
						ArrayList<AgentUtterance> utterances = utterancesChar.get(categoryName);
						if( utterances == null ) {
							utterances = new ArrayList<AgentUtterance>();
						}
						
						/* Create new AgentUtterance */
						AgentUtterance uttr = new AgentUtterance( utterance, categoryName );
						
						/* Find linking utterances */
						NodeList linkList = utteranceElem.getElementsByTagName("link");
						HashMap<ArrayList<String>,String> linkingUtterances = new HashMap<ArrayList<String>,String>();
						for( int l=0; l<linkList.getLength(); l++ ) {
							Element linkElem = (Element)linkList.item(l);
							if( linkElem.hasAttribute("pre") && linkElem.hasAttribute("response") ) {
								String pre = linkElem.getAttribute("pre");
								String response = linkElem.getAttribute("response");
								ArrayList<String> preList = new ArrayList<String>(Arrays.asList( pre.split(" ") ));
								linkingUtterances.put(preList, response);
							}
						}
						HashMap<AgentUtterance,HashMap<ArrayList<String>,String>> links = allLinks.get(character);
						if( links == null ) {
							links = new HashMap<AgentUtterance,HashMap<ArrayList<String>,String>>();
						}
						links.put(uttr, linkingUtterances);
						allLinks.put(character, links);
						
						/* Find elaboration utterance */
						NodeList elaborationList = utteranceElem.getElementsByTagName("elaboration");
						for( int l=0; l<elaborationList.getLength(); l++ ) {
							Element elaborationElem = (Element)elaborationList.item(l);
							if( elaborationElem.hasAttribute("response") ) {
								String response = elaborationElem.getAttribute("response");
								HashMap<AgentUtterance,String> elaborations = allElaborations.get(character);
								if( elaborations == null ) {
									elaborations = new HashMap<AgentUtterance,String>();
								}
								elaborations.put( uttr, response );
								allElaborations.put( character, elaborations );
							}
						}
						
						/* Find features */
						NodeList featureNodeList = utteranceElem.getElementsByTagName("features");
						for( int l=0; l<featureNodeList.getLength(); l++ ) {
							Element featureElem = (Element)featureNodeList.item(l);
							if( featureElem.hasAttribute("features") ) {
								String features = featureElem.getAttribute("features");
								ArrayList<String> featureList = new ArrayList<String>(Arrays.asList( features.split(" ") ));
								uttr.setFeatures( featureList );
							}
						}
						
						/* Add AgentUtterance to the list */
						utterances.add(uttr);
						utterancesChar.put(categoryName,utterances);
						allUtterances.put( character,utterancesChar );
						
						HashMap<String,AgentUtterance> utterancesOfChar = allUtterancesWithoutGroup.get(character);
						if( utterancesOfChar == null ) {
							utterancesOfChar = new HashMap<String,AgentUtterance>();
						}
						utterancesOfChar.put(utterance, uttr);
						allUtterancesWithoutGroup.put(character, utterancesOfChar);
					}
				}
			}

		}
		
		/* Fill in the Linking utterances */
		for( int character : allLinks.keySet() ) {
			HashMap<AgentUtterance,HashMap<ArrayList<String>,String>> charLinks = allLinks.get(character);
			for( AgentUtterance uttr : charLinks.keySet() ) {
				HashMap<ArrayList<String>,AgentUtterance> newLinks = new HashMap<ArrayList<String>,AgentUtterance>();
				HashMap<ArrayList<String>,String> uttrLinks = charLinks.get(uttr);
				for( ArrayList<String> features : uttrLinks.keySet() ) {
					AgentUtterance newUttr = allUtterancesWithoutGroup.get(character).get(uttrLinks.get(features));
					if( newUttr == null ) {
						newUttr = new AgentUtterance( uttrLinks.get(features), "LinkingSentence" );
					}
					newLinks.put(features,newUttr);
				}
				uttr.setLinkingUtterances(newLinks);
			}
		}
		
		/* Fill in the Elaboration utterances */
		for( int character : allElaborations.keySet() ) {
			HashMap<AgentUtterance,String> elaborations = allElaborations.get(character);
			for( AgentUtterance uttr : elaborations.keySet() ) {
				AgentUtterance newUttr = allUtterancesWithoutGroup.get(character).get(elaborations.get(uttr));
				if( newUttr == null ) {
					newUttr = new AgentUtterance( elaborations.get(uttr), "ElaborationSentence" );
				}
				uttr.setElaborationUtterance(newUttr);
			}
		}
		
		return true;
	}

	/**
	 * @return the allUtterances
	 */
	public HashMap<Integer, HashMap<String, ArrayList<AgentUtterance>>> getAllUtterances() {
		return allUtterances;
	}
	
	public static void main( String[] args )
	{
		new SentenceReader("/eu/semaine/components/dialogue/data/sentences.xml").readData();
	}
}
