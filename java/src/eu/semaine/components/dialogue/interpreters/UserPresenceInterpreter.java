/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.dialogue.interpreters;

import java.util.HashMap;
import java.util.Map;

import javax.jms.JMSException;

import eu.semaine.components.Component;
import eu.semaine.datatypes.stateinfo.ContextStateInfo;
import eu.semaine.datatypes.stateinfo.StateInfo;
import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.receiver.FeatureReceiver;
import eu.semaine.jms.receiver.XMLReceiver;
import eu.semaine.jms.sender.StateSender;

/**
 * @author marc
 *
 */
public class UserPresenceInterpreter extends Component
{
	/**
	 * 	Limit that no life sign is detected, in ms
	 */
	public static final long SEMAINE_LIFESIGNLIMIT = 3000;

	
	private long lastFaceTime = 0; // last time we've seen a face
	private long faceElapsed = 0; // Estimation of time elapsed since seeing a face
	private long lastVoiceTime = 0; // last time we've heard a voice
	private long voiceElapsed = 0; // Estimation of time elapsed since hearing a voice
	private long lifeElapsed = 0; // Estimation of time elapsed since sensing life
	private boolean userPresent = false;
	
	private FeatureReceiver facedetReceiver;
	private XMLReceiver speakingReceiver;
	private StateSender userPresenceSender;
	
	
	
	
	/**
	 * @param componentName
	 * @throws JMSException
	 */
	public UserPresenceInterpreter()
	throws JMSException
	{
		super("UserPresenceInterpreter");
		facedetReceiver = new FeatureReceiver("semaine.data.analysis.features.video.facedetection");
		receivers.add(facedetReceiver);
		
		speakingReceiver = new XMLReceiver("semaine.data.state.user.behaviour");
		receivers.add(speakingReceiver);
		
		userPresenceSender = new StateSender("semaine.data.state.context", StateInfo.Type.ContextState, getName());
		senders.add(userPresenceSender);
	}

	private void updateTimeElapsed(boolean faceSeen, boolean voiceHeard)
	{
		long current = meta.getTime();
		if (faceSeen) {
			lastFaceTime = current; 
		}
		if (voiceHeard) {
			lastVoiceTime = current;
		}
		faceElapsed = current - lastFaceTime;
		voiceElapsed = current - lastVoiceTime;
		
		lifeElapsed = Math.min(faceElapsed, voiceElapsed);
	}


	/**
	 * Check if user presence is changed. Also updates internal state of userPresent
	 * @return
	 */
	private boolean isUserPresenceChanged()
	{
		boolean presenceChanged = false;
		if (lifeElapsed > SEMAINE_LIFESIGNLIMIT) {
			if (userPresent) {
				presenceChanged = true;
			}
			userPresent = false;
		} else {
			if (!userPresent) {
				presenceChanged = true;
			}
			userPresent = true;
		}
		return presenceChanged;
	}
	
	private void sendUserPresenceChange()
	throws JMSException
	{
		Map<String, String> info = new HashMap<String, String>();
		info.put("userPresent", userPresent ? SemaineML.V_PRESENT : SemaineML.V_ABSENT);
		ContextStateInfo csi = new ContextStateInfo(info);
		userPresenceSender.sendStateInfo(csi, meta.getTime());
	}


	@Override
	protected void act()
	throws JMSException
	{
		updateTimeElapsed(false, false);
		if (isUserPresenceChanged()) {
			sendUserPresenceChange();
		}
		
	}

	@Override
	protected void react(SEMAINEMessage m)
	throws JMSException
	{
		boolean face = false;
		boolean voice = false;
		String topicName  = m.getTopicName();
		if (topicName.equals(facedetReceiver.getTopicName())) {
			// -- Wehn we receve a faceloc, it MEANS that a face is detected (no need to read message)
			face = true;
		} else if (topicName.equals(speakingReceiver.getTopicName())) {
			// TODO: verify if the message is a speaking message
			voice = true;
		}
		updateTimeElapsed(face, voice);
	}

}
