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
import java.util.Map;

import javax.jms.JMSException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.datatypes.stateinfo.ContextStateInfo;
import eu.semaine.datatypes.stateinfo.StateInfo;
import eu.semaine.datatypes.xml.BML;
import eu.semaine.datatypes.xml.FML;
import eu.semaine.datatypes.xml.SSML;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.message.SEMAINEStateMessage;
import eu.semaine.jms.receiver.StateReceiver;
import eu.semaine.jms.sender.FMLSender;
import eu.semaine.jms.sender.StateSender;
import eu.semaine.util.XMLTool;

/**
 *
 * @author marc
 */
public class TTSInputComponent extends Component {
	private FMLSender fmlSender;
	private StateSender contextSender;
	private StateReceiver contextReceiver;
	private TTSGui gui;
	private String newText = null;
	private String newLocaleString = null;
	private String newCharacterName = null;
	
    /** Creates a new instance of TTSInputComponent */
    public TTSInputComponent()
    throws JMSException {
        super("TTSInput", true, false);
		fmlSender = new FMLSender("semaine.data.action.selected.function", getName());
		senders.add(fmlSender); // so it can be started etc
		contextSender = new StateSender("semaine.data.state.context", StateInfo.Type.ContextState, getName());
		senders.add(contextSender); // so it can be started etc
        contextReceiver = new StateReceiver("semaine.data.state.context", StateInfo.Type.ContextState);        
		receivers.add(contextReceiver);

		gui = new TTSGui(this);
		gui.setVisible(true);
    }
    
    
    public void speak(String text, String localeString) {
    	newText = text;
    	newLocaleString = localeString;
    }
    
    public void setCharacter(String characterName) {
    	newCharacterName = characterName;
    }
    
    
    private ContextStateInfo constructContextStateInfo(String currentCharacter) {
    	Map<String, String> info = new HashMap<String, String>();
		info.put("character", currentCharacter);
		ContextStateInfo context = new ContextStateInfo(info);
		return context;
    }
    
    private Document constructTTSDocument(String text, String localeString) {
		Document doc = XMLTool.newDocument("fml-apml", null, FML.version);
		Element root = doc.getDocumentElement();
		Element bml = XMLTool.appendChildElement(root, BML.E_BML, BML.namespaceURI);
		bml.setAttribute(BML.A_ID, "bml1");
		Element fml = XMLTool.appendChildElement(root, FML.E_FML, FML.namespaceURI);
		fml.setAttribute(FML.A_ID, "fml1");
		Element speech = XMLTool.appendChildElement(bml, BML.E_SPEECH);
		speech.setAttribute(BML.A_ID, "s1");
		speech.setAttribute(BML.E_TEXT, text);
		speech.setAttribute(BML.E_LANGUAGE, localeString);
		speech.setTextContent(text);
		Element mark = XMLTool.appendChildElement(speech, SSML.E_MARK, SSML.namespaceURI);
		mark.setAttribute(SSML.A_NAME, "m1");		
		return doc;
    }
    
    // Send in act() so we benefit from superclass' state and error handling:
    @Override
    protected void act() throws JMSException {
    	if (newCharacterName != null) {
    		ContextStateInfo context = constructContextStateInfo(newCharacterName);
    		contextSender.sendStateInfo(context, meta.getTime());
    		newCharacterName = null;
    	}
    	if (newText != null && newLocaleString != null) {
    		Document doc = constructTTSDocument(newText, newLocaleString);
    		fmlSender.sendXML(doc, meta.getTime());
    		newText = null;
    		newLocaleString = null;
    	}
    }

    // In case anyone else changes the character, we want to see it:
    @Override
    protected void react(SEMAINEMessage m) throws Exception {
        SEMAINEStateMessage xm = (SEMAINEStateMessage) m;
        StateInfo state = xm.getState();
        if (state.hasInfo("character")) {
        	gui.updateCharacterDisplay(state.getInfo("character"));
        }
    }

}
