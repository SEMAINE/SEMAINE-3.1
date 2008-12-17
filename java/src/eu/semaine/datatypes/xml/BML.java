/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.datatypes.xml;

/**
 * @author marc
 *
 */
public class BML 
{
    public static final String version = "1.0";
    public static final String namespaceURI = "http://www.mindmakers.org/projects/BML";

    // Elements
    public static final String E_BML = "bml";
	public static final String ROOT_TAGNAME = E_BML;
	public static final String E_SPEECH = "speech";
	public static final String E_TEXT = "text";
	public static final String E_LANGUAGE = "language";
	public static final String E_SYNC = "sync";
	public static final String E_DESCRIPTION = "description";
	public static final String E_HEAD = "head";
	public static final String E_GAZE = "gaze";
	public static final String E_GESTURE = "gesture";
	public static final String E_LOCOMOTION = "locomotion";
	public static final String E_FACE = "face";
	public static final String E_LIPS = "lips";
	public static final String E_BACKCHANNEL = "backchannel";
	
	// Attributes
	public static final String A_ID = "id";
	public static final String A_START = "start";
	public static final String A_END = "end";
	public static final String A_TYPE = "type";

}
