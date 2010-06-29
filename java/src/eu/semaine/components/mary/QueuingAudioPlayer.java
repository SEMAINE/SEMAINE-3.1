/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.mary;

import java.io.ByteArrayInputStream;
import java.util.HashMap;
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
import eu.semaine.jms.message.SEMAINEFeatureMessage;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.receiver.BytesReceiver;
import eu.semaine.jms.receiver.FeatureReceiver;
import eu.semaine.jms.receiver.Receiver;
import eu.semaine.jms.sender.XMLSender;
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
	
	
	private BytesReceiver audioReceiver;
	private Receiver dummyFapReceiver;
	private Receiver dummyBapReceiver;
	private FeatureReceiver commandReceiver;
	private XMLSender callbackSender;
	private HashMap<String, Animation> availableAnimations;
	private HashMap<String, AnimationState> oldAnimationStates;
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
		commandReceiver = new FeatureReceiver("semaine.data.synthesis.lowlevel.command");
		receivers.add(commandReceiver);
		callbackSender = new XMLSender("semaine.callback.output.player", "SemaineML", getName()); 
		senders.add(callbackSender);
		availableAnimations = new HashMap<String, Animation>();
		oldAnimationStates = new HashMap<String, AnimationState>();
		playerQueue = new LinkedBlockingQueue<Animation>();
		player = new Playloop(playerQueue);
	}
	
	@Override
	public void react(SEMAINEMessage m) throws JMSException
	{
		String contentID = m.getContentID();
		long contentCreationTime = m.getContentCreationTime();
		Animation anim;
		synchronized (availableAnimations) {
			anim = availableAnimations.get(contentID);
			if (anim == null) {
				AnimationState oldState = oldAnimationStates.get(contentID);
				if (oldState != null) {
					log.warn("Received "+m.getDatatype()+" message from "+m.getSource()+" (topic "+m.getTopicName()
							+") for content ID '"+contentID+"', but that ID is already in state '"+oldState.toString()+"' -- discarding message.");
					return;
				} else {
					anim = new Animation(contentID, contentCreationTime);
					availableAnimations.put(contentID, anim);
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
			assert m instanceof SEMAINEFeatureMessage;
			SEMAINEFeatureMessage fm = (SEMAINEFeatureMessage) m;
			fillAnimFromFeatureMessage(anim, fm);
		}
		if (anim.isReady()) {
			sendCallbackMessage(SemaineML.V_READY, contentID);
			synchronized (availableAnimations) {
				try {
					playerQueue.put(anim);
				} catch (InterruptedException e) {
					log.error(e);
				}
				availableAnimations.remove(contentID);
				oldAnimationStates.put(contentID, AnimationState.READY);
			}
		} else if (anim.hasFirstData()) {
			sendCallbackMessage(SemaineML.V_CREATED, contentID);
		}

		
	}
	
	
	private boolean haveVerifiedFeatureNames = false;
	
	private void fillAnimFromFeatureMessage(Animation anim, SEMAINEFeatureMessage fm) throws JMSException, MessageFormatException {
		if (!haveVerifiedFeatureNames) {
			String[] featureNames = fm.getFeatureNames();
			for (int i=0; i<featureNames.length; i++) {
				String expectedFeatureName = PlayerFeatures.values()[i].toString();
				if (!featureNames[i].equals(expectedFeatureName)) {
					throw new MessageFormatException("Expected feature number "+i+" to be '"+expectedFeatureName+"', but found '"+featureNames[i]+"'");
				}
			}
			haveVerifiedFeatureNames = true;
		}
		float[] features = fm.getFeatureVector();
		// TODO: ignoring STARTTIME for now -- always treat it as 0 = "when ready"
		float priority = features[PlayerFeatures.PRIORITY.ordinal()];
		int lifetime = Math.round(features[PlayerFeatures.LIFETIME.ordinal()]);
		boolean needsAudio = features[PlayerFeatures.HASAUDIO.ordinal()] > 0.5;
		boolean needsFAP = features[PlayerFeatures.HASFAP.ordinal()] > 0.5;
		boolean needsBAP = features[PlayerFeatures.HASBAP.ordinal()] > 0.5;
		anim.setCommand(priority, lifetime, needsAudio, needsFAP, needsBAP);
	}
	
	
	/**
	 * Playloop is playing the Animations that are put into the player queue.
	 * TODO: The current implementation starts Animations immediately, in the queue order; priority, start time and lifetime are ignored. 
	 * @author marc
	 *
	 */
	public class Playloop extends Thread {
		
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
				ByteArrayInputStream bais = new ByteArrayInputStream(anim.getAudioData());
				
				try {
					sendCallbackMessage(SemaineML.V_START, anim.getContentID());
					oldAnimationStates.put(anim.getContentID(), AnimationState.PLAYING);
					log.debug(anim.getContentID()+" started playing "+(meta.getTime()-anim.getContentCreationTime())+" ms after creation");
					AudioInputStream ais = AudioSystem.getAudioInputStream(bais);
	            	AudioPlayer player = new AudioPlayer(ais);
					player.start();
					player.join();
					sendCallbackMessage(SemaineML.V_END, anim.getContentID());
					oldAnimationStates.put(anim.getContentID(), AnimationState.FINISHED);
					//sleep(1000);
				} catch (Exception e) {
					e.printStackTrace();
				} 
			}
		}
		
	}
	
	public void sendCallbackMessage(String type, String contentID) throws JMSException {
		
		Document doc = XMLTool.newDocument("callback", SemaineML.namespaceURI);
		Element root = doc.getDocumentElement();
		Element callback = XMLTool.appendChildElement(root, SemaineML.E_EVENT , SemaineML.namespaceURI);
		callback.setAttribute(SemaineML.A_TYPE, type);
		//callback.setAttribute("data",  "audio");
		callback.setAttribute(SemaineML.A_ID, contentID != null ? contentID : "unknown");
		callback.setAttribute(SemaineML.A_TIME,  String.valueOf(meta.getTime()));
		
		callbackSender.sendXML(doc, meta.getTime());
		log.debug("Sending callback: ID '"+contentID+"' "+type);
	}

	
	private class Animation {
		private String contentID;
		private long contentCreationTime;
		private float priority;
		private int lifetime;
		private byte[] audioData;
		private Object fapData;
		private Object bapData;
		private boolean needsAudio;
		private boolean needsFAP;
		private boolean needsBAP;
		private boolean haveReceivedCommand;
		private boolean hasReportedFirstData = false;
		
		public Animation(String contentID, long contentCreationTime) {
			this.contentID = contentID;
			this.contentCreationTime = contentCreationTime;
			this.haveReceivedCommand = false;
			this.audioData = null;
			this.fapData = null;
			this.bapData = null;
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

		public void setCommand(float aPriority, int aLifetime, boolean hasAudio, boolean hasFAP, boolean hasBAP) {
			this.priority = aPriority;
			this.lifetime = aLifetime;
			this.needsAudio = hasAudio;
			this.needsFAP = hasFAP;
			this.needsBAP = hasBAP;
			this.haveReceivedCommand = true;
		}
		
		public String getContentID() {
			return contentID;
		}
		
		public long getContentCreationTime() {
			return contentCreationTime;
		}
		
		public float getPriority() {
			return priority;
		}
		
		public int getLifetime() {
			return lifetime;
		}
		
		public byte[] getAudioData() {
			return audioData;
		}
		
		public Object getFAPData() {
			return fapData;
		}

		public Object getBAPData() {
			return bapData;
		}

		/**
		 * Returns true if the animation has any data of any kind. This is independent of the question whether
		 * the animation has received a command.
		 * @return
		 */
		public boolean hasFirstData() {
			if (!hasReportedFirstData && (audioData != null || fapData != null || bapData != null)) {
				// not yet reported, but have data. Answer = yes
				hasReportedFirstData = true;
				return true;
			}
			return false;
		}
		
		/**
		 * The animation is ready when all parts that are needed are present, and the playback modalities are known.
		 * Whether this is the case can only be known when a command has been received,
		 * because the command includes the information about the types of data that must be present.
		 * Therefore, when this method returns true, the Animation is ready to be played.
		 * @return
		 */
		public boolean isReady() {
			return haveReceivedCommand
				&& (!needsAudio || audioData != null)
				&& (!needsFAP || fapData != null)
				&& (!needsBAP || bapData != null);
		}
		

	}
}
