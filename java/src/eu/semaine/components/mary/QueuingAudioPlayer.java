/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.mary;

import java.io.ByteArrayInputStream;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;

import javax.jms.JMSException;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;

import marytts.util.data.audio.AudioPlayer;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.jms.message.SEMAINEBytesMessage;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.receiver.BytesReceiver;
import eu.semaine.jms.receiver.Receiver;
import eu.semaine.jms.sender.XMLSender;
import eu.semaine.util.SEMAINEUtils;
import eu.semaine.util.XMLTool;

/**
 * Audio player
 * @author Sathish Chandra Pammi
 *
 */
public class QueuingAudioPlayer extends Component 
{
	public enum PlayerFeatures { STARTAT, LIFETIME, PRIORITY, HASAUDIO, HASFAP, HASBAP };

	public enum AnimationState { CREATED, READY, PLAYING, FINISHED, DEAD };
	
	public static final String COMMANDTYPE_INFO = "dataInfo";
	public static final String COMMANDTYPE_PLAY = "playCommand";
	
	private BytesReceiver audioReceiver;
	private Receiver dummyFapReceiver;
	private Receiver dummyBapReceiver;
	private Receiver commandReceiver;
	private XMLSender callbackSender;
	private HashMap<String, Animation> availableAnimations;
	private HashMap<String, AnimationState> animationStates;
	private BlockingQueue<Animation> playerQueue;
	Playloop player;
	
	/**
	 * @param componentName
	 * @throws JMSException
	 */
	public QueuingAudioPlayer() throws JMSException 
	{
		super("AudioPlayer", false, true);
		audioReceiver = new BytesReceiver("semaine.data.synthesis.lowlevel.audio");
		receivers.add(audioReceiver);
		dummyFapReceiver = new Receiver("semaine.data.synthesis.lowlevel.video.FAP");
		receivers.add(dummyFapReceiver);
		dummyBapReceiver = new Receiver("semaine.data.synthesis.lowlevel.video.BAP");
		receivers.add(dummyBapReceiver);
		commandReceiver = new Receiver("semaine.data.synthesis.lowlevel.command");
		receivers.add(commandReceiver);
		callbackSender = new XMLSender("semaine.callback.output.Animation", "SemaineML", getName()); 
		senders.add(callbackSender);
		availableAnimations = new HashMap<String, Animation>();
		animationStates = new HashMap<String, AnimationState>();
		playerQueue = new LinkedBlockingQueue<Animation>();
		player = new Playloop(playerQueue);
	}
	
	@Override
	public void react(SEMAINEMessage m) throws JMSException
	{
		String contentID = m.getContentID();
		long contentCreationTime = m.getContentCreationTime();
		String contentType = m.getContentType();
		Animation anim;
		synchronized (availableAnimations) {
			anim = availableAnimations.get(contentID);
			if (anim == null) {
				AnimationState oldState = animationStates.get(contentID);
				if (oldState != null) {
					log.warn(new Object[] {meta.getTime(), "Received "+m.getDatatype()+" message from "+m.getSource()+" (topic "+m.getTopicName()
							+") for content ID '"+contentID+"', but that ID is already in state '"+oldState.toString()+"' -- discarding message."});
					return;
				} else {
					anim = new Animation(contentID, contentCreationTime, contentType);
					availableAnimations.put(contentID, anim);
					animationStates.put(contentID, AnimationState.CREATED);
				}
			} else {
				if (anim.getContentType() == null && contentType != null) {
					anim.setContentType(contentType);
				}
			}
		}
		assert anim != null;
		// Now we have an animation at an uncertain state of maturity.
		if(m.getTopicName().equals(audioReceiver.getTopicName())) {
			assert m.isBytesMessage();
			SEMAINEBytesMessage bm = (SEMAINEBytesMessage) m;
			anim.setAudioData(bm.getBytes());
		} else if (m.getTopicName().equals(dummyFapReceiver.getTopicName())) {
			anim.setFAPData(m.getText());
		} else if (m.getTopicName().equals(dummyBapReceiver.getTopicName())) {
			anim.setBAPData(m.getText());
		} else if (m.getTopicName().equals(commandReceiver.getTopicName())) {
			fillAnimFromCommandMessage(anim, m);
		}
		// Have we just got ready?
		if (animationStates.get(contentID) == AnimationState.CREATED && anim.isReady()) {
			sendCallbackMessage(SemaineML.V_READY, contentID, anim.getContentType());
			animationStates.put(contentID, AnimationState.READY);
		}
		if (anim.isReady() && anim.haveReceivedPlayCommand()) {
			synchronized (availableAnimations) {
				try {
					playerQueue.put(anim);
				} catch (InterruptedException e) {
					log.error(new Object[] {meta.getTime(), e});
				}
				availableAnimations.remove(contentID);
			}
		}

		
	}
	
	
	private float[] getFloatValues(Map<String, String> features, String ... keys) throws MessageFormatException {
		float[] vals = new float[keys.length];
		for (int i=0; i<keys.length; i++) {
			String key = keys[i];
			String val = features.get(key);
			if (val == null) {
				throw new MessageFormatException("Feature '"+key+"' is missing");
			}
			try {
				vals[i] = Float.parseFloat(val);
			} catch (NumberFormatException nfe) {
				throw new MessageFormatException("Expected float value for feature '"+key+"', but got '"+val+"'");
			}
		}
		return vals;
	}
	
	private void fillAnimFromCommandMessage(Animation anim, SEMAINEMessage message) throws JMSException, MessageFormatException {
		Map<String, String> features = SEMAINEUtils.string2map(message.getText(), true);
		if (message.getDatatype().equals(COMMANDTYPE_INFO)) {
			// expect HASAUDIO, HASFAP and HASBAP features
			float[] vals = getFloatValues(features, PlayerFeatures.HASAUDIO.toString(), PlayerFeatures.HASFAP.toString(), PlayerFeatures.HASBAP.toString());
			boolean needsAudio = (vals[0] > 0.5);
			boolean needsFAP   = (vals[1] > 0.5);
			boolean needsBAP   = (vals[2] > 0.5);
			anim.setDataInfo(needsAudio, needsFAP, needsBAP);
		} else if (message.getDatatype().equals(COMMANDTYPE_PLAY)) {
			// expect STARTAT, PRIORITY and LIFETIME
			float[] vals = getFloatValues(features, PlayerFeatures.STARTAT.toString(), PlayerFeatures.PRIORITY.toString(), PlayerFeatures.LIFETIME.toString());
			int startAt = Math.round(vals[0]);
			float priority = vals[1];
			int lifetime = Math.round(vals[2]);
			anim.setPlayCommand(startAt, priority, lifetime);
		}
	}
	
	
	/**
	 * Playloop is playing the Animations that are put into the player queue.
	 * TODO: The current implementation starts Animations immediately, in the queue order; priority, start time and lifetime are ignored. 
	 * @author marc
	 *
	 */
	public final class Playloop extends Thread {
		
		protected BlockingQueue<Animation> inputWaiting;
		protected boolean playing = false;
		
		public Playloop(BlockingQueue<Animation> inputWaiting){
			this.inputWaiting = inputWaiting;	
			start();
		}
		
		public void run() {
			for (;;)  {  
				Animation anim = null;
				try {
					// block until input becomes available
					anim = inputWaiting.poll(waitingTime, TimeUnit.MILLISECONDS);
					if(anim == null) continue;
				} catch (InterruptedException ie) {
					// if we have no input, we'll keep on waiting
					continue;
				}
				if (!anim.needsAudio) {
					continue;
				}
				log.debug(new Object[] {meta.getTime(), "Now playing: "+anim.contentID});
				ByteArrayInputStream bais = new ByteArrayInputStream(anim.getAudioData());
				
				try {
					AudioInputStream ais = AudioSystem.getAudioInputStream(bais);
	            	AudioPlayer player = new AudioPlayer(ais);
					player.start();
					sendCallbackMessage(SemaineML.V_START, anim.getContentID(), anim.getContentType());
					animationStates.put(anim.getContentID(), AnimationState.PLAYING);
					log.debug(new Object[] {meta.getTime(), anim.getContentID()+" started playing "+(meta.getTime()-anim.getContentCreationTime())+" ms after creation"});
					player.join();
					sendCallbackMessage(SemaineML.V_END, anim.getContentID(), anim.getContentType());
					animationStates.put(anim.getContentID(), AnimationState.FINISHED);
				} catch (Exception e) {
					log.warn(e);
				} 
			}
		}
		
	}
	
	public void sendCallbackMessage(String type, String contentID, String contentType) throws JMSException {
		
		Document doc = XMLTool.newDocument("callback", SemaineML.namespaceURI);
		Element root = doc.getDocumentElement();
		Element callback = XMLTool.appendChildElement(root, SemaineML.E_EVENT , SemaineML.namespaceURI);
		callback.setAttribute(SemaineML.A_TYPE, type);
		//callback.setAttribute("data",  "audio");
		callback.setAttribute(SemaineML.A_ID, contentID != null ? contentID : "unknown");
		callback.setAttribute(SemaineML.A_TIME,  String.valueOf(meta.getTime()));
		if (contentType != null) {
			callback.setAttribute(SemaineML.A_CONTENTTYPE, contentType);
		}
		
		callbackSender.sendXML(doc, meta.getTime());
		log.debug(new Object[] {meta.getTime(), "Sending callback: ID '"+contentID+"' "+type});
	}

	
	private static class Animation {
		private String contentID;
		private long contentCreationTime;
		private String contentType;
		
		// Play command params:
		private int startAt;
		private float priority;
		private int lifetime;
		// dataInfo command params:
		private boolean needsAudio;
		private boolean needsFAP;
		private boolean needsBAP;
		// data
		private byte[] audioData;
		private Object fapData;
		private Object bapData;

		private boolean haveReceivedPlayCommand;
		
		public Animation(String contentID, long contentCreationTime, String contentType) {
			this.contentID = contentID;
			this.contentCreationTime = contentCreationTime;
			this.contentType = contentType;
			this.haveReceivedPlayCommand = false;
			this.audioData = null;
			this.fapData = null;
			this.bapData = null;
			// Default is conservative: we need everything:
			this.needsAudio = true;
			this.needsFAP = true;
			this.needsBAP = true;
		}
		
		
		public void setAudioData(byte[] newAudioData) {
			this.audioData = newAudioData;
		}
		
		public void setFAPData(Object newFAPData) {
			this.fapData = newFAPData;
		}
		
		public void setBAPData(Object newBAPData) {
			this.bapData = newBAPData;
		}

		public void setDataInfo(boolean hasAudio, boolean hasFAP, boolean hasBAP) {
			this.needsAudio = hasAudio;
			this.needsFAP = hasFAP;
			this.needsBAP = hasBAP;
		}
		
		public void setPlayCommand(int aStartAt, float aPriority, int aLifetime) {
			this.startAt = aStartAt;
			this.priority = aPriority;
			this.lifetime = aLifetime;
			this.haveReceivedPlayCommand = true;
		}
		
		public void setContentType(String aContentType) {
			this.contentType = aContentType;
		}
		
		public String getContentID() {
			return contentID;
		}
		
		public long getContentCreationTime() {
			return contentCreationTime;
		}
		
		public String getContentType() {
			return contentType;
		}
		
		@SuppressWarnings("unused")
		public int getStartAt() {
			return startAt;
		}
		
		@SuppressWarnings("unused")
		public float getPriority() {
			return priority;
		}
		
		@SuppressWarnings("unused")
		public int getLifetime() {
			return lifetime;
		}
		
		public byte[] getAudioData() {
			return audioData;
		}
		
		@SuppressWarnings("unused")
		public Object getFAPData() {
			return fapData;
		}

		@SuppressWarnings("unused")
		public Object getBAPData() {
			return bapData;
		}

		/**
		 * Whether a play command has been received for this animation.
		 * @return
		 */
		public boolean haveReceivedPlayCommand() {
			return haveReceivedPlayCommand;
		}

		
		/**
		 * The animation is ready when all parts that are needed are present.
		 * Whether this is the case is known when a dataInfo command has been received,
		 * because the command includes the information about the types of data that must be present.
		 * Therefore, when this method returns true, the Animation can be triggered at any time by a playCommand.
		 * @return
		 */
		public boolean isReady() {
			return (!needsAudio || audioData != null)
				&& (!needsFAP || fapData != null)
				&& (!needsBAP || bapData != null);
		}
		

	}
}
