/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.datatypes.xml;

/**
 * @author marc
 *
 */
public class FML 
{
    public static final String version = "0.1";
    public static final String namespaceURI = "http://www.mindmakers.org/projects/FML";

    // Elements
    public static final String E_FML = "fml";
	public static final String ROOT_TAGNAME = E_FML;
	public static final String E_PERFORMATIVE = "performative";
	public static final String E_THEME = "theme";
	public static final String E_RHEME = "rheme";
	public static final String E_WORLD = "world";
	public static final String E_EMOTION = "emotion";
	public static final String E_BACKCHANNEL = "backchannel";

	// Attributes
	public static final String A_ID = "id";
	public static final String A_START = "start";
	public static final String A_END = "end";
	public static final String A_TYPE = "type";


}
