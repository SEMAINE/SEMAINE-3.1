/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.datatypes.xml;

/**
 * @author marc
 *
 */
public class EmotionML 
{
	public static final String namespaceURI = "http://www.w3.org/2005/Incubator/emotion";
	public static final String version = "0.0.1";

	// Elements
	public static final String E_EMOTION = "emotion";
	public static final String ROOT_ELEMENT = E_EMOTION;
	public static final String E_CATEGORY = "category";
	public static final String E_DIMENSIONS = "dimensions";
	public static final String E_AROUSAL = "arousal";
	public static final String E_VALENCE = "valence";
	//public static final String E_POTENCY = "potency";
	public static final String E_POWER = "power";
	public static final String E_INTENSITY = "intensity";
	public static final String E_EXPECTATION = "expectation";
	public static final String E_OBJECT = "object";
	
	// Attributes
	public static final String A_SET = "set";
	public static final String A_NAME = "name";
	public static final String A_VALUE = "value";
	public static final String A_CONFIDENCE = "confidence";
	public static final String A_TYPE = "type";

}
