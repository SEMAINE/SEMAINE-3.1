/**
 * Copyright (C) 2008 University of Twente, HMI. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.dialogue.actionproposers;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;

import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.BufferedReader;
import java.io.IOException;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;

import eu.semaine.components.dialogue.datastructures.ContextTemplate;

/**
 * This class imports the data needed for the UtteranceProposer
 * 
 * @author Mark tM
 * @version 0.1 - dummy class
 *
 */

public class DataImporter
{
	/* The file that contains information about the context of all responses */
	private String contextFile;
	
	/* The file that contains information about the response-groups */
	private String groupFile;
	
	/* A reader to read a file */
	private BufferedReader reader;
	
	/* A ContextTemplate */
	private ContextTemplate template;

	
	/**
	 * Creates a new DataImporter
	 * @param contextFile
	 * @param groupFile
	 */
	public DataImporter( String contextFile, String groupFile )
	{
		this.groupFile = groupFile;
		this.contextFile = contextFile;
	}
	
	/**
	 * Reads the GroupData and returs this
	 * @return
	 */
	public HashMap<String,ArrayList<String>> importGroupData()
	{
		InputStream is = this.getClass().getResourceAsStream(groupFile);
				
		Document doc;
		try{
            DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
            factory.setExpandEntityReferences(true);
            factory.setNamespaceAware(true);
            DocumentBuilder docBuilder = factory.newDocumentBuilder();
            doc = docBuilder.parse(is);
        } catch (Exception e) {
            System.out.println("Error parsing xml file");
            e.printStackTrace();
            return null;
        }
        if( doc == null ) {
        	return null;
        }
        
        HashMap<String,ArrayList<String>> groupData = new HashMap<String,ArrayList<String>>(); 
        
        Element root = doc.getDocumentElement();
        if( root.getTagName().equals("SentenceGroups") ) {
        	NodeList groups = root.getElementsByTagName("Group");
        	for( int i=0; i<groups.getLength(); i++ ) {
            	Element group = (Element)groups.item(i);
            	if( group != null ) {
            		String groupName = group.getAttribute("name");
            		String personality = group.getAttribute("character");
            		ArrayList<String> sentenceList = new ArrayList<String>();
            		NodeList sentences = group.getElementsByTagName("Sentence");
            		for( int j=0; j<sentences.getLength(); j++ ) {
            			Element sentence = (Element)sentences.item(j);
            			sentenceList.add( sentence.getAttribute("value") );
            		}
            		
            		groupData.put(personality + ":" + groupName, sentenceList);
            	}
            }
        }
        return groupData;
	}

	/**
	 * Reads the ContextData and returns this
	 * @return
	 */
	public ArrayList<ContextTemplate> importContextData()
	{
		reader = new BufferedReader( new InputStreamReader( this.getClass().getResourceAsStream(contextFile) ) );

		ArrayList<ContextTemplate> templates = new ArrayList<ContextTemplate>();

		String line = readLine(reader);
		while( line != null ) {
			
			String[] parts = line.split(";");
			String phrase;
			template = new ContextTemplate( parts[0] );
			
			HashSet<Integer> chars = new HashSet<Integer>();
			
			/* Personality */
			phrase = parts[1].toLowerCase().trim();
			if( phrase.contains("poppy") ) {
				chars.add( ContextTemplate.POPPY );
			}
			if( phrase.contains("obadiah") ) {
				chars.add( ContextTemplate.OBADIAH );
			}
			if( phrase.contains("prudence") ) {
				chars.add( ContextTemplate.PRUDENCE );
			}
			if( phrase.contains("spike") ) {
				chars.add( ContextTemplate.SPIKE );
			}
			
			for( Integer character : chars ) {
				template = new ContextTemplate( parts[0] );
				template.addRequiredFeature( character );
			
				/* Emotion */
				phrase = parts[2].toLowerCase().trim();
				if( phrase.equals("happy") ) {
					template.addRequiredFeature( ContextTemplate.HAPPY );
				} else if( phrase.equals("sad") ) {
					template.addRequiredFeature( ContextTemplate.SAD );
				} else if( phrase.equals("angry") ) {
					template.addRequiredFeature( ContextTemplate.ANGRY );
				} else if( phrase.equals("(happy)") ) {
					template.addExtraFeature( ContextTemplate.HAPPY );
				} else if( phrase.equals("(sad)") ) {
					template.addExtraFeature( ContextTemplate.SAD );
				} else if( phrase.equals("(angry)") ) {
					template.addExtraFeature( ContextTemplate.ANGRY );
				}
				
				/* Polarity */
				phrase = parts[3].toLowerCase().trim();
				if( phrase.equals("positive") ) {
					template.addRequiredFeature( ContextTemplate.POSITIVE );
				} else if( phrase.equals("negative") ) {
					template.addRequiredFeature( ContextTemplate.NEGATIVE );
				} else if( phrase.equals("(positive)") ) {
					template.addExtraFeature( ContextTemplate.POSITIVE );
				} else if( phrase.equals("(negative)") ) {
					template.addExtraFeature( ContextTemplate.NEGATIVE );
				}
				
				/* Semantics */
				String[] semanticList = parts[4].split("/");
				for( String semantic : semanticList ) {
					semantic = semantic.toLowerCase().trim();
					if( semantic.equals("agree") ) {
						template.addRequiredFeature( ContextTemplate.AGREE );
					} else if( semantic.equals("disagree") ) {
						template.addRequiredFeature( ContextTemplate.DISAGREE );
					} else if( semantic.equals("about other people") ) {
						template.addRequiredFeature( ContextTemplate.ABOUT_OTHER_PEOPLE );
					} else if( semantic.equals("about other character") ) {
						template.addRequiredFeature( ContextTemplate.ABOUT_OTHER_CHARACTER );
					} else if( semantic.equals("about current character") ) {
						template.addRequiredFeature( ContextTemplate.ABOUT_CURRENT_CHARACTER );
					} else if( semantic.equals("about own feelings") ) {
						template.addRequiredFeature( ContextTemplate.ABOUT_OWN_FEELINGS );
					} else if( semantic.equals("pragmatic") ) {
						template.addRequiredFeature( ContextTemplate.PRAGMATIC );
					} else if( semantic.equals("(agree)") ) {
						template.addExtraFeature( ContextTemplate.AGREE );
					} else if( semantic.equals("(disagree)") ) {
						template.addExtraFeature( ContextTemplate.DISAGREE );
					} else if( semantic.equals("(about other people)") ) {
						template.addExtraFeature( ContextTemplate.ABOUT_OTHER_PEOPLE );
					} else if( semantic.equals("(about other character)") ) {
						template.addExtraFeature( ContextTemplate.ABOUT_OTHER_CHARACTER );
					} else if( semantic.equals("(about current character)") ) {
						template.addExtraFeature( ContextTemplate.ABOUT_CURRENT_CHARACTER );
					} else if( semantic.equals("(about own feelings)") ) {
						template.addExtraFeature( ContextTemplate.ABOUT_OWN_FEELINGS );
					} else if( semantic.equals("(pragmatic)") ) {
						template.addExtraFeature( ContextTemplate.PRAGMATIC );
					}
				}
				
				/* Group */
				phrase = parts[5].toLowerCase().trim();
				if( phrase.length() > 1 ) {
					line = readLine(reader);
					continue;
				}
				
				/* Notes, gebeurd nog niks mee */
				phrase = parts[6].toLowerCase().trim();
				
				/* Talk about self */
				phrase = parts[7].toLowerCase().trim();
				if( phrase.equals("1") ) {
					template.addRequiredFeature( ContextTemplate.TALK_ABOUT_SELF );
				} else if( phrase.equals("-1") ) {
					template.addExtraFeature( ContextTemplate.TALK_ABOUT_SELF );
				}
				
				/* Past Event */
				phrase = parts[8].toLowerCase().trim();
				if( phrase.equals("1") ) {
					template.addRequiredFeature( ContextTemplate.PAST_EVENT );
				} else if( phrase.equals("-1") ) {
					template.addExtraFeature( ContextTemplate.PAST_EVENT);
				}
				
				/* Future Event */
				phrase = parts[9].toLowerCase().trim();
				if( phrase.equals("1") ) {
					template.addRequiredFeature( ContextTemplate.FUTURE_EVENT );
				} else if( phrase.equals("-1") ) {
					template.addExtraFeature( ContextTemplate.FUTURE_EVENT );
				}
				
				/* Past Action */
				phrase = parts[10].toLowerCase().trim();
				if( phrase.equals("1") ) {
					template.addRequiredFeature( ContextTemplate.PAST_ACTION );
				} else if( phrase.equals("-1") ) {
					template.addExtraFeature( ContextTemplate.PAST_ACTION);
				}
				
				/* Future Action */
				phrase = parts[11].toLowerCase().trim();
				if( phrase.equals("1") ) {
					template.addRequiredFeature( ContextTemplate.FUTURE_ACTION );
				} else if( phrase.equals("-1") ) {
					template.addExtraFeature( ContextTemplate.FUTURE_ACTION );
				}
				
				/* Laugh */
				phrase = parts[12].toLowerCase().trim();
				if( phrase.equals("1") ) {
					template.addRequiredFeature( ContextTemplate.LAUGH );
				} else if( phrase.equals("-1") ) {
					template.addExtraFeature( ContextTemplate.LAUGH );
				}
				
				if( template.getRequiredFeatures().size() > 1 )  {
					templates.add( template );
				}
			}
			
			line = readLine(reader);
		}
		return templates;
	}
	
	/**
	 * Returns the Feature-integer representation of the given Character
	 * @param phrase
	 * @return
	 */
	public int getCharacter( String phrase )
	{
		phrase = phrase.toLowerCase().trim();
		if( phrase.equals("poppy") ) {
			return ContextTemplate.POPPY;
		} else if( phrase.equals("obadiah") ) {
			return ContextTemplate.OBADIAH;
		} else if( phrase.equals("prudence") ) {
			return ContextTemplate.PRUDENCE;
		} else if( phrase.equals("spike") ) {
			return ContextTemplate.SPIKE;
		} else {
			return -1;
		}
	}

	/**
	 * Reads the next line of the given Reader
	 * @param reader
	 * @return
	 */
	public String readLine( BufferedReader reader )
	{
		try {
			return reader.readLine();
		} catch( IOException e ) {
			e.printStackTrace();
			return null;
		}
	}

}
