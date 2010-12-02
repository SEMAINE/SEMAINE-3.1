/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.control;

import java.util.HashMap;
import java.util.Map;

import javax.jms.JMSException;

import eu.semaine.components.Component;
import eu.semaine.datatypes.stateinfo.ContextStateInfo;
import eu.semaine.datatypes.stateinfo.StateInfo;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.message.SEMAINEStateMessage;
import eu.semaine.jms.receiver.StateReceiver;
import eu.semaine.jms.sender.StateSender;
import eu.semaine.system.CharacterConfigInfo;

/**
 * @author marc
 *
 */
public class ParticipantControl extends Component 
{

    private ParticipantControlGUI gui;
	private StateSender contextSender;
	private StateReceiver contextReceiver;
    private boolean isUserPresent = false;
    private String currentCharacter = CharacterConfigInfo.getDefaultCharacter().getName();
    private String nextCharacter = null;
    private String currentDialogContext = null;

    /**
	 * @param componentName
	 * @throws JMSException
	 */
	public ParticipantControl() throws JMSException 
	{
		super("ParticipantControlGUI", true, false);
		contextSender = new StateSender("semaine.data.state.context", StateInfo.Type.ContextState, getName());
		senders.add(contextSender); // so it can be started etc
        contextReceiver = new StateReceiver("semaine.data.state.context", StateInfo.Type.ContextState);        
		receivers.add(contextReceiver);
        gui = new ParticipantControlGUI(this);
        gui.setVisible(true);
	}
	
	@Override
	protected void act() throws JMSException {
		if (hasSystemJustBecomeReady()) {
			sendWhoIsPresent();
		}
	}

        @Override
        protected void react(SEMAINEMessage m)
        throws Exception
        {
            SEMAINEStateMessage xm = (SEMAINEStateMessage) m;
            StateInfo state = xm.getState();
            if (state.hasInfo("userPresent")) {
            	isUserPresent = "present".equals(state.getInfo("userPresent"));
            }
            if (state.hasInfo("character")) {
            	currentCharacter = state.getInfo("character");
            }
            if (state.hasInfo("nextCharacter")) {
            	nextCharacter = state.getInfo("nextCharacter");
            }
            if (state.hasInfo("currentDialogContext")) {
            	currentDialogContext = state.getInfo("currentDialogContext");
            }
            gui.updateWhoIsPresent();
        }

	private void sendWhoIsPresent()
	throws JMSException
	{
		Map<String, String> info = new HashMap<String, String>();
		info.put("userPresent", isUserPresent ? "present" : "absent");
		info.put("character", currentCharacter);
		if (nextCharacter != null) {
			info.put("nextCharacter", nextCharacter);
		}
		if (currentDialogContext != null) {
			info.put("dialogContext", currentDialogContext);
		}
		ContextStateInfo context = new ContextStateInfo(info);
		contextSender.sendStateInfo(context, meta.getTime());
	}

    public boolean isUserPresent()
    {
        return isUserPresent;
    }
    
    public void setUserPresent(boolean userPresent)
    {
        this.isUserPresent = userPresent;
        try {
            sendWhoIsPresent();
        } catch (JMSException e) {
            e.printStackTrace();
        }
    }
    
    public String getCurrentCharacter()
    {
        return currentCharacter;
    }

    public String getNextCharacter() {
    	return nextCharacter;
    }
    
    public String getDialogContext() {
    	return currentDialogContext;
    }
    
    public void setCurrentCharacter(String name)
    {
    	setCharacterInfo(name, null, null);
    }
    
    public void setCharacterInfo(String name, String nextChar, String dialogContext) {
    	this.currentCharacter = name;
    	this.nextCharacter = nextChar;
    	this.currentDialogContext = dialogContext;
        try {
            sendWhoIsPresent();
        } catch (JMSException e) {
            e.printStackTrace();
        }
    }
    
}
