/*
 * TTSInputComponent.java
 *
 * Created on 26. Mai 2010, 09:40
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package eu.semaine.components.dummy;


import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import javax.jms.JMSException;

import marytts.util.Pair;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.components.mary.QueuingAudioPlayer.PlayerFeatures;
import eu.semaine.datatypes.xml.BML;
import eu.semaine.datatypes.xml.FML;
import eu.semaine.datatypes.xml.SSML;
import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.jms.IOBase.Event;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.message.SEMAINEXMLMessage;
import eu.semaine.jms.receiver.XMLReceiver;
import eu.semaine.jms.sender.FMLSender;
import eu.semaine.jms.sender.Sender;
import eu.semaine.util.XMLTool;

/**
 *
 * @author marc
 */
public class UtterancePreparationTestComponent extends Component {
    protected enum Action {Prepare, PlayPrepared, PlayDirect, Play};
    
    
	private FMLSender fmlSender;
	private FMLSender queuingFMLSender;
	private Sender commandSender;
	private XMLReceiver callbackReceiver;
	private UtterancePreparationTestGUI gui;
	private String newText = null;
	private String newID = null;
	private Action newAction = null;

	private Map<String, Pair<Action, Long>> triggeredActions = new HashMap<String, Pair<Action,Long>>();
	

	private Set<String> readyAnimations = new HashSet<String>();
	
    /** Creates a new instance of TTSInputComponent */
    public UtterancePreparationTestComponent()
    throws JMSException {
        super("UtterancePreparation", true, false);
		fmlSender = new FMLSender("semaine.data.action.selected.function", getName());
		senders.add(fmlSender); // so it can be started etc
		queuingFMLSender = new FMLSender("semaine.data.action.prepare.function", getName());
		senders.add(queuingFMLSender);
		commandSender = new Sender("semaine.data.synthesis.lowlevel.command", "playCommand", getName());
		senders.add(commandSender);
        callbackReceiver = new XMLReceiver("semaine.callback.output.Animation");        
		receivers.add(callbackReceiver);

		PlayerFeatures[] playerFeatures = PlayerFeatures.values();
		String[] playerFeatureNames = new String[playerFeatures.length];
		for (int i=0; i<playerFeatures.length; i++) {
			playerFeatureNames[i] = playerFeatures[i].toString();
		}

		gui = new UtterancePreparationTestGUI(this);
		gui.setVisible(true);
    }
    
    
    public void triggerAction(Action action, String text, String id) {
    	newAction = action;
        newText = text;
    	newID = id;
    }
    
    
    private Document constructTTSDocument(String text, String id) {
		Document doc = XMLTool.newDocument("fml-apml", null, FML.version);
		Element root = doc.getDocumentElement();
		Element bml = XMLTool.appendChildElement(root, BML.E_BML, BML.namespaceURI);
		bml.setAttribute(BML.A_ID, id);
		Element fml = XMLTool.appendChildElement(root, FML.E_FML, FML.namespaceURI);
		fml.setAttribute(FML.A_ID, "fml1");
		Element speech = XMLTool.appendChildElement(bml, BML.E_SPEECH);
		speech.setAttribute(BML.A_ID, "s1");
		speech.setAttribute(BML.E_TEXT, text);
		speech.setAttribute(BML.E_LANGUAGE, "en-GB");
		speech.setTextContent(text);
		Element mark = XMLTool.appendChildElement(speech, SSML.E_MARK, SSML.namespaceURI);
		mark.setAttribute(SSML.A_NAME, "s1:m1");		
		return doc;
    }
    
    private String constructPlayCommand() {
    	return "STARTAT 0\nPRIORITY 0.5\nLIFETIME 5000\n";
    }
    

	private void prepare(String contentID, String text, long time) throws JMSException {
		Document doc = constructTTSDocument(text, contentID);
		queuingFMLSender.sendXML(doc, time, contentID, time);
	}

	private void playDirect(String contentID, String text, long time) throws JMSException {
		Document doc = constructTTSDocument(text, contentID);
		fmlSender.sendXML(doc, time, contentID, time);
	}

	private void trigger(String contentID, long time) throws JMSException {
		String cmd = constructPlayCommand();
		commandSender.sendTextMessage(cmd, time, Event.single, contentID, time);
	}
	
	
    // Send in act() so we benefit from superclass' state and error handling:
    @Override
    protected void act() throws JMSException {

    	if (newAction != null && newText != null && newID != null) {
    		long time = meta.getTime();
    		String directID = newID + "direct";
    		String prepareID = newID+"prepare";
    		Pair<Action, Long> val = new Pair<Action, Long>(newAction, time); 
    		switch (newAction) {
                case Prepare:
                    triggeredActions.put(prepareID, val);
                	prepare(prepareID, newText, time);
                    gui.log(meta.getTime()+" "+newAction+" "+prepareID);
                    break;
                case PlayPrepared:
                    triggeredActions.put(prepareID, val);
                	trigger(prepareID, time);
                    gui.log(meta.getTime()+" "+newAction+" "+prepareID);
                    break;
                case PlayDirect:
                    triggeredActions.put(directID, val);
                	playDirect(directID, newText, time);
                    gui.log(meta.getTime()+" "+newAction+" "+directID);
                    break;
                case Play:
                	if (isReady(newID)) {
                        triggeredActions.put(prepareID, val);
                		trigger(prepareID, time);
                        gui.log(meta.getTime()+" "+newAction+" "+prepareID);
                	} else {
                        triggeredActions.put(directID, val);
                		playDirect(directID, newText, time);
                        gui.log(meta.getTime()+" "+newAction+" "+directID);
                	}
                    break;
            }
                
    		newText = null;
    		newID = null;
            newAction = null;
    	}
    }



    public boolean isReady(String contentID) {
    	return readyAnimations.contains(contentID+"prepare");
    }
    
    // In case anyone else changes the character, we want to see it:
    @Override
    protected void react(SEMAINEMessage m) throws Exception {
    	if (m instanceof SEMAINEXMLMessage) {
        	SEMAINEXMLMessage xm = (SEMAINEXMLMessage) m;
        	Document doc = xm.getDocument();
        	Element event = XMLTool.getChildElementByLocalNameNS(doc.getDocumentElement(), SemaineML.E_EVENT, SemaineML.namespaceURI);
        	if (event != null) {
        		String id = event.getAttribute(SemaineML.A_ID);
        		long time = Long.parseLong(event.getAttribute(SemaineML.A_TIME));
        		String type = event.getAttribute(SemaineML.A_TYPE);
        		if (type.equals(SemaineML.V_READY)) {
        			readyAnimations.add(id);
        		} else {
        			readyAnimations.remove(id);
        		}
        		StringBuilder msg = new StringBuilder(time+" "+id+" ");
        		if (type.equals(SemaineML.V_START)) {
        			msg.append("started playing");
        		} else if (type.equals(SemaineML.V_READY)) {
        			msg.append("ready");
        		} else {
        			msg.append(type);
        		}
    			if (triggeredActions.containsKey(id)) {
    				Pair<Action, Long> p = triggeredActions.get(id);
    				msg.append(" "+(time - p.getSecond())+" ms after "+p.getFirst());
    			}
    			gui.log(msg.toString());
        		gui.verifyButtonsEnabled();
        	}
    	}
    }

}
