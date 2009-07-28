/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.control;

import eu.semaine.components.control.ParticipantControlGUI;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.message.SEMAINEStateMessage;
import eu.semaine.jms.receiver.StateReceiver;

import java.util.HashMap;
import java.util.Map;

import javax.jms.JMSException;

import eu.semaine.components.Component;
import eu.semaine.datatypes.stateinfo.ContextStateInfo;
import eu.semaine.datatypes.stateinfo.StateInfo;
import eu.semaine.jms.sender.StateSender;

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
    private String currentCharacter = ParticipantControlGUI.PRUDENCE;

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
            gui.updateWhoIsPresent();
        }

	private void sendWhoIsPresent()
	throws JMSException
	{
		Map<String, String> info = new HashMap<String, String>();
		info.put("userPresent", isUserPresent ? "present" : "absent");
		info.put("character", currentCharacter);
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
    
    public void setCurrentCharacter(String name)
    {
        this.currentCharacter = name;
        try {
            sendWhoIsPresent();
        } catch (JMSException e) {
            e.printStackTrace();
        }
    }
}
