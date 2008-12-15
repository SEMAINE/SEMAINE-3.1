/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.mary;

import java.io.ByteArrayInputStream;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;

import javax.jms.JMSException;
import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioInputStream;

import marytts.util.data.audio.AudioPlayer;
import eu.semaine.components.Component;
import eu.semaine.jms.message.SEMAINEBytesMessage;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.receiver.BytesReceiver;

/**
 * Audio player
 * @author Sathish Chandra Pammi
 *
 */
public class SemaineAudioPlayer extends Component 
{
	private BytesReceiver audioReceiver;
	private BlockingQueue<byte[]> inputWaiting;
	Playloop player;
	
	/**
	 * @param componentName
	 * @throws JMSException
	 */
	public SemaineAudioPlayer() throws JMSException 
	{
		super("SemaineAudioPlayer", false, true);
		audioReceiver = new BytesReceiver("semaine.data.lowlevel.audio");
		receivers.add(audioReceiver); // to set up properly
		inputWaiting = new LinkedBlockingQueue<byte[]>();
		player =  new Playloop(inputWaiting);
	}
	
	@Override
	public void react(SEMAINEMessage m) throws JMSException
	{
		if(m.isBytesMessage() && m.getTopicName().equals("semaine.data.lowlevel.audio")){
			SEMAINEBytesMessage bm = (SEMAINEBytesMessage)m;
			byte[] audioData =  bm.getBytes();
			try {
				inputWaiting.put(audioData);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
		
	}
	
	public class Playloop extends Thread{
		
		protected BlockingQueue<byte[]> inputWaiting;
		protected boolean playing = false;
		
		public Playloop(BlockingQueue<byte[]> inputWaiting){
			this.inputWaiting = inputWaiting;	
			start();
		}
		
		public void run(){
			for (;;)  {  
				byte[] audioData = null;
				try {
					// block until input becomes available
					audioData = inputWaiting.poll(waitingTime, TimeUnit.MILLISECONDS);
					if(audioData == null) continue;
				} catch (InterruptedException ie) {
					// if we have no input, we'll keep on waiting
					continue;
				}
				ByteArrayInputStream bais = new ByteArrayInputStream(audioData);
		        boolean signed = true; //true,false
		        int bitsPerSample = 16;
		        int samplingRate = 16000;
		        int channels = 1;
		        boolean bigEndian = false; 
		        long lengthInSamples = audioData.length / (bitsPerSample/8);
		        
		        AudioFormat af = new AudioFormat(
		        samplingRate,
		        bitsPerSample,
		        channels,
		        signed,
		        bigEndian);
		        
				AudioInputStream ais = new AudioInputStream(bais, af, lengthInSamples);
				
				try {
	            	AudioPlayer player = new AudioPlayer(ais);
					player.start();
					player.join();
					//sleep(1000);
				} catch (InterruptedException e) {
					e.printStackTrace();
				} 
			}
		}
		
	}
		
	
}
