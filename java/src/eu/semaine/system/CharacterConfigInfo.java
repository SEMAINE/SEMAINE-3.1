/**
 * Copyright (C) 2010 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.system;

import java.io.File;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Set;
import java.util.StringTokenizer;
import java.util.TreeMap;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eu.semaine.datatypes.xml.EmotionML;
import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.exceptions.SystemConfigurationException;
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
				// Is there a voice definition for the character?
				Element voice = XMLTool.getChildElementByLocalNameNS(character, "voice", SemaineML.namespaceURI);
				String[] voiceNames = null;
				String locale = null;
				String effects = null;
				if (voice != null) {
					String voiceNameAttribute = voice.getAttribute("name");
					locale = voice.getAttribute("locale");
					effects = voice.getAttribute("effects");
					List<String> voices = new ArrayList<String>();
					StringTokenizer st = new StringTokenizer(voiceNameAttribute);
					while (st.hasMoreTokens()) {
						voices.add(st.nextToken());
					}
					voiceNames = voices.toArray(new String[voices.size()]);
				}
				// Is there a definition of emotional predispositions for the character?
				Element predispositionElement = XMLTool.getChildElementByLocalNameNS(character, "predisposition", SemaineML.namespaceURI);
				Map<String, Float> predispositions = null;
				if (predispositionElement != null) {
					predispositions = fillPredispositions(predispositionElement);
				}
				// Are there any other features present?
				List<Element> settingElements = XMLTool.getChildrenByLocalNameNS(character, "setting", SemaineML.namespaceURI);
				assert settingElements != null;
				Map<String, String> settings = null;
				for (Element settingElement : settingElements) {
					String name = settingElement.getAttribute("name");
					if ("".equals(name)) {
						throw new SystemConfigurationException("Found setting without a 'name' attribute for character '"+characterName+"'");
					}
					String value = settingElement.getAttribute("value");
					if ("".equals(value)) {
						throw new SystemConfigurationException("Found setting without a 'value' attribute for character '"+characterName+"'");
					}
					if (settings == null) {
						settings = new HashMap<String, String>();
					}
					settings.put(name, value);
				}
				
				CharacterConfigInfo cci = new CharacterConfigInfo(characterName, voiceNames, locale, effects, predispositions, settings);
				theMap.put(characterName, cci);
				if (isDefault) {
					if (defaultCharacter != null) {
						throw new Error("More than one character marked as is-default=\"true\" in character config file '"+configFile.getPath()+"' -- aborting.");
					}
					defaultCharacter = cci;
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
	
	private static Map<String, Float> fillPredispositions(Element predispositionElement)
	throws SystemConfigurationException {
		Map<String, Float> predispositions = new HashMap<String, Float>();
		List<Element> emotionElements = XMLTool.getChildrenByLocalNameNS(predispositionElement, EmotionML.E_EMOTION, EmotionML.namespaceURI);
		for (Element emotion : emotionElements) {
			String vocabularyURI = emotion.getAttribute(EmotionML.A_DIMENSION_VOCABULARY);
			if ("".equals(vocabularyURI)) {
				throw new SystemConfigurationException("Emotion definition does not have a valid vocabulary URI in attribute "+EmotionML.A_DIMENSION_VOCABULARY);
			}
			List<Element> dimensions = XMLTool.getChildrenByLocalNameNS(emotion, EmotionML.E_DIMENSION, EmotionML.namespaceURI);
			for (Element dimension : dimensions) {
				String name = dimension.getAttribute(EmotionML.A_NAME);
				if ("".equals(name)) {
					throw new SystemConfigurationException("Dimension has no name");
				}
				String valueString = dimension.getAttribute(EmotionML.A_VALUE);
				// Fail-early strategy: insist that the value must be a valid float value in the range between 0 and 1.
				// That avoids having errors go unnoticed.
				try {
					float value = Float.parseFloat(valueString);
					if (value < 0 || value > 1) {
						throw new SystemConfigurationException("Value '"+value+"' of dimension '"+name+"' is outside of the valid value range [0, 1]");
					}
					// Finally, we have a valid triplet of vocabulary URI, dimension name, and value -- remember it:
					String key = vocabularyURI+"|"+name;
					predispositions.put(key, value);
				} catch (NumberFormatException nfe) {
					throw new SystemConfigurationException("Value '"+valueString+"' of dimension '"+name+"' is not a float number", nfe);
				}
			}
		}
		return predispositions;
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
	
	private final String name;
	private final String[] voices;
	private final String localeString;
	private final String voiceEffects;
	// predispositions has as keys "uri|name", and as values the emotion dimension's value.
	private final Map<String, Float> predispositions;
	// settings are generic String key-value pairs
	private final Map<String, String> settings;
	
	private CharacterConfigInfo(String name, String[] voices, String voiceLocale, String voiceEffects, Map<String, Float> predispositions, Map<String, String> settings) {
		this.name = name;
		this.voices = voices;
		this.localeString = voiceLocale;
		this.voiceEffects = voiceEffects;
		this.predispositions = predispositions;
		this.settings = settings;
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
	
	/**
	 * For the given emotion dimension, provide any predisposition that the character may have.
	 * @param vocabularyURI the URI identifying the emotion dimension vocabulary being used
	 * @param dimension the name of an emotion dimension, which must be part of the given vocabulary
	 * @return a float value in the interval [0, 1] if the current character has such a predisposition,
	 * or -1 if there is no such predisposition (i.e. the value is undefined).
	 */
	public float getEmotionalPredisposition(String vocabularyURI, String dimension) {
		if (predispositions == null) {
			return -1;
		}
		String key = vocabularyURI+"|"+dimension;
		Float val = predispositions.get(key);
		if (val != null) {
			return val;
		}
		return -1;
	}
	
	/**
	 * Get a complete list of the emotional predispositions defined for this character.
	 * @return a list of two-item String arrays, or an empty list if there are no predispositions.
	 * For each array a, a[0] is the vocabularyURI,
	 * and a[1] is the name of the dimension. Together these can be used to get the actual
	 * value of the predisposition from {@link #getEmotionalPredisposition(String, String)}.
	 */
	public List<String[]> getEmotionalPredispositions() {
		if (predispositions == null) {
			return new ArrayList<String[]>();
		}
		List<String[]> epList = new ArrayList<String[]>(predispositions.size());
		for (String key : predispositions.keySet()) {
			String[] uriAndName = key.split("\\|");
			assert uriAndName.length == 2;
			epList.add(uriAndName);
		}
		return epList;
	}
	
	/**
	 * Generic mechanism for providing String-valued key-value pairs from the config file, using
	 * <code>&lt;setting name="..." value="..."/&gt;</code>. 
	 * @param name the setting's name, which must match the <code>name</code> attribute in the config file. 
	 * @return the corresponding String value, or null if there is no such setting.
	 */
	public String getSetting(String name) {
		if (settings == null) {
			return null;
		}
		return settings.get(name);
	}
	
	/**
	 * Provide the list of all setting names which are defined for this character.
	 * The corresponding values can then be obtained using {@link #getSetting(String)}.
	 * @return a set of Strings, or the empty set if there are no settings for this character.
	 */
	public Set<String> getSettings() {
		if (settings == null) {
			return new HashSet<String>();
		}
		return Collections.unmodifiableSet(settings.keySet());
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
