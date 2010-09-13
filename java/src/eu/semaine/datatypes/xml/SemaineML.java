/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.datatypes.xml;

/**
 * @author marc
 *
 */
public class SemaineML
{
    public static final String version = "0.0.3";
    public static final String namespaceURI = "http://www.semaine-project.eu/semaineml";

    // Elements
    public static final String E_CONTEXT = "situational-context";
    public static final String E_USERSTATE = "user-state";
    public static final String E_AGENTSTATE = "agent-state";
    public static final String E_DIALOGSTATE = "dialog-state";
    public static final String E_SETUP = "setup";
    public static final String E_FEATURE = "feature";
    public static final String E_EVENT = "event";
    public static final String E_BEHAVIOUR = "behaviour";
    public static final String E_TEXT = "text";
    public static final String E_EMOTIONALLY_CONCORDANT_WITH_USER = "emotionally-concordant-with-user";
    public static final String E_DIALOG_ACT = "dialog-act";
    public static final String E_DIALOG_HISTORY = "dialog-history";
    public static final String E_SPEAKER = "speaker";
    public static final String E_LISTENER = "listener";
    public static final String E_TOPIC = "topic";
    public static final String E_USER = "user";
    public static final String E_CHARACTER = "character";
    public static final String E_SPEAKING = "speaking";
    public static final String E_PITCH = "pitch";
    public static final String E_FACEPRESENT = "face-present";
    public static final String E_VOCALIZATION = "vocalization";
    
    // Attributes
    public static final String A_WHO = "who";
    public static final String A_NAME = "name";
    public static final String A_SPEAKER = "speaker";
    public static final String A_TOPIC = "topic";
    public static final String A_VALUE = "value";
    public static final String A_TIME = "time";
    public static final String A_INTENSITY = "intensity";
    public static final String A_STATUS = "status";
    public static final String A_DIRECTION = "direction";
    public static final String A_ID = "id";
    public static final String A_DATA = "data";
    public static final String A_TYPE = "type";
    
    // Attribute values
    // For A_STATUS:
    public static final String V_PRESENT = "present";
    public static final String V_ABSENT = "absent";
    // For A_TYPE
    public static final String V_READY = "ready";
    public static final String V_DELETED = "deleted";
    public static final String V_START = "start";
    public static final String V_STOPPED = "stopped";
    public static final String V_END = "end";
    // For <vocalization name="...">:
    public static final String V_LAUGHTER = "(laughter)";
    public static final String V_SIGH = "(sigh)";
    
}
