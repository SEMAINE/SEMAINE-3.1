/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.datatypes;

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
    public static final String EMMA = "emma";
	public static final String ROOT_TAGNAME = EMMA;
	public static final String INTERPRETATION = "interpretation";
	public static final String ONEOF = "one-of";
	
	// Attributes
	public static final String START = "start";
	public static final String END = "end";
	public static final String DURATION = "duration";
	public static final String VERBAL = "verbal";
	public static final String MODE = "mode";
	public static final String CONFIDENCE = "confidence";


}
