/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.datatypes.xml;

/**
 * Static helper methods for interpreting and creating EMMA documents
 * @author marc
 *
 */
public class EMMA 
{
    public static final String version = "1.0";
    public static final String namespaceURI = "http://www.w3.org/2003/04/emma";

    // Elements
    public static final String E_EMMA = "emma";
	public static final String ROOT_TAGNAME = E_EMMA;
	public static final String E_INTERPRETATION = "interpretation";
	public static final String E_ONEOF = "one-of";
	public static final String E_SEQUENCE = "sequence";
	public static final String E_GROUP = "group";
	
	// Attributes
	public static final String A_START = "emma:start";
	public static final String A_OFFSET_TO_START = "emma:offset-to-start";
	public static final String A_END = "emma:end";
	public static final String A_DURATION = "emma:duration";
	public static final String A_VERBAL = "emma:verbal";
	public static final String A_MODE = "emma:mode";
	public static final String A_CONFIDENCE = "emma:confidence";
	public static final String A_TOKENS = "emma:tokens";


}
