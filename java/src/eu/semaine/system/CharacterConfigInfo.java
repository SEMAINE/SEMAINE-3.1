/**
 * Copyright (C) 2010 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.system;

import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.StringTokenizer;
import java.util.TreeMap;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.util.SEMAINEUtils;
import eu.semaine.util.XMLTool;

/**
 * This class is (intended to become) the single Java entry point for all 
 * configuration information needed to define the appearance and behaviour of
 * the characters in the system.
 * 
 * Everything a Java component needs to know about a Character should be defined here.
 * 
 * The information is read from an XML config file; the filename must be given as
 * property "semaine.character-config", in the java config file. 
 *  
 * @author marc
 *
 */
public class CharacterConfigInfo {
	private static CharacterConfigInfo defaultCharacter = null;
	private static Map<String, CharacterConfigInfo> allInfos = initialize();
	
	private static Map<String, CharacterConfigInfo> initialize() {
		Map<String, CharacterConfigInfo> theMap = new TreeMap<String, CharacterConfigInfo>();
		String filename = System.getProperty("semaine.character-config", "");
		File configFile = new File(filename);
		if (!configFile.isAbsolute()) {
			// relative filename is relative to the config folder
			// (i.e., the same folder as the component runner config file)
			String componentConfigFilename = System.getProperty("semaine.config-file");
			assert componentConfigFilename != null;
			File configFolder = new File(componentConfigFilename).getParentFile();
			configFile = new File(configFolder, filename);
		}
		if (!configFile.exists()) {
			throw new Error("No character config file given in property 'semaine.character-config' -- aborting.");
		}
		try {
			Document doc = XMLTool.parse(configFile);
			for (Element character : XMLTool.getChildrenByLocalNameNS(doc.getDocumentElement(), "character", SemaineML.namespaceURI)) {
				String characterName = character.getAttribute("name");
				boolean isDefault = Boolean.parseBoolean(character.getAttribute("is-default"));
				Element voice = XMLTool.getChildElementByLocalNameNS(character, "voice", SemaineML.namespaceURI);
				if (voice != null) {
					String voiceNames = voice.getAttribute("name");
					String locale = voice.getAttribute("locale");
					String effects = voice.getAttribute("effects");
					List<String> voices = new ArrayList<String>();
					StringTokenizer st = new StringTokenizer(voiceNames);
					while (st.hasMoreTokens()) {
						voices.add(st.nextToken());
					}
					CharacterConfigInfo cci = new CharacterConfigInfo(characterName, voices.toArray(new String[voices.size()]), locale, effects);
					theMap.put(characterName, cci);
					if (isDefault) {
						if (defaultCharacter != null) {
							throw new Error("More than one character marked as is-default=\"true\" in character config file '"+configFile.getPath()+"' -- aborting.");
						}
						defaultCharacter = cci;
					}
				}
			}
			
		} catch (Throwable t) {
			throw new Error("Cannot parse character config file '"+configFile.getPath()+"' -- aborting.", t);
		}
		
		if (defaultCharacter == null) {
			throw new Error("No character marked as is-default=\"true\" in character config file '"+configFile.getPath()+"' -- aborting.");
		}
		return theMap;
	}
	
	/**
	 * The public entry point for getting access to character info by name.
	 * @param characterName the character's name.
	 * @return a CharacterConfigInfo object describing the Character's properties.
	 */
	public static CharacterConfigInfo getInfo(String characterName) {
		return allInfos.get(characterName);
	}
	
	/**
	 * Get all character names for which chracter config info is available.
	 * @return
	 */
	public static Iterable<String> getCharacterNames() {
		return allInfos.keySet();
	}
	
	public static CharacterConfigInfo getDefaultCharacter() {
		return defaultCharacter;
	}
	
	///////////////////////////////////////////////////////////////////////
	
	private String name;
	private String[] voices;
	private String localeString;
	private String voiceEffects;
	
	private CharacterConfigInfo(String name, String[] voices, String voiceLocale, String voiceEffects) {
		this.name = name;
		this.voices = voices;
		this.localeString = voiceLocale;
		this.voiceEffects = voiceEffects;
	}
	
	public String getName() {
		return name;
	}
	
	public String[] getVoices() {
		return voices;
	}
	
	public String getVoiceLocaleString() {
		return localeString;
	}
	
	public Locale getVoiceLocale() {
		return SEMAINEUtils.string2locale(localeString);
	}

	public String getVoiceEffects() {
		return voiceEffects;
	}
	
        @Override
        public String toString() {
        	StringBuilder sb = new StringBuilder(name);
        	sb.append(" (voice ");
        	for (int i=0; i<voices.length; i++) {
        		if (i>0) sb.append("|");
        		sb.append(voices[i]);
        	}
        	sb.append(", locale ").append(localeString).append(")");
        	return sb.toString();
        }
	

}
