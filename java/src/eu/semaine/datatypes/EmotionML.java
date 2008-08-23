/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.datatypes;

/**
 * @author marc
 *
 */
public class EmotionML 
{
	public static final String namespace = "http://www.w3.org/2005/Incubator/emotion";
	public static final String version = "0.0.1";

	// Elements
	public static final String EMOTION = "emotion";
	public static final String ROOT_ELEMENT = EMOTION;
	public static final String CATEGORY = "category";
	public static final String DIMENSIONS = "dimensions";
	public static final String AROUSAL = "arousal";
	public static final String VALENCE = "valence";
	public static final String POTENCY = "potency";
	
	// Attributes
	public static final String SET = "set";
	public static final String NAME = "name";
	public static final String VALUE = "value";
	public static final String CONFIDENCE = "confidence";

}
