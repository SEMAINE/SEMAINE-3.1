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
    public static final String namespace = "org.mindmakers.bml";

    // Elements
    public static final String BML = "bml";
	public static final String ROOT_TAGNAME = BML;
	public static final String SPEECH = "speech";
	public static final String TEXT = "text";
	public static final String SYNC = "sync";
	public static final String DESCRIPTION = "description";
	public static final String HEAD = "head";
	public static final String GAZE = "gaze";
	public static final String GESTURE = "gesture";
	public static final String LOCOMOTION = "locomotion";
	public static final String FACE = "face";
	public static final String LIPS = "lips";
	
	// Attributes
	public static final String ID = "id";
	public static final String START = "start";
	public static final String END = "end";
	public static final String TYPE = "type";

}
