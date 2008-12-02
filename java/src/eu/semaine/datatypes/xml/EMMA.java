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
    public static final String namespace = "http://www.w3.org/2003/04/emma";

    // Elements
    public static final String E_EMMA = "emma";
	public static final String ROOT_TAGNAME = E_EMMA;
	public static final String E_INTERPRETATION = "interpretation";
	public static final String E_ONEOF = "one-of";
	
	// Attributes
	public static final String A_START = "start";
	public static final String A_END = "end";
	public static final String A_DURATION = "duration";
	public static final String A_VERBAL = "verbal";
	public static final String A_MODE = "mode";
	public static final String A_CONFIDENCE = "confidence";


}
