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
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;

import marytts.util.data.audio.AudioPlayer;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.jms.message.SEMAINEBytesMessage;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.receiver.BytesReceiver;
import eu.semaine.jms.sender.XMLSender;
import eu.semaine.util.XMLTool;

/**
 * Audio player
 * @author Sathish Chandra Pammi
 *
 */
public class SemaineAudioPlayer extends Component 
{
	private BytesReceiver audioReceiver;
	private XMLSender callbackSender;
	private BlockingQueue<byte[]> inputWaiting;
	Playloop player;
	
	/**
	 * @param componentName
	 * @throws JMSException
	 */
	public SemaineAudioPlayer() throws JMSException 
	{
		super("SemaineAudioPlayer", false, true);
		audioReceiver  = new BytesReceiver("semaine.data.synthesis.lowlevel.audio");
		callbackSender = new XMLSender("semaine.callback.output.audio", "SemaineML", getName()); 
		receivers.add(audioReceiver); // to set up properly
		senders.add(callbackSender);
		inputWaiting   = new LinkedBlockingQueue<byte[]>();
		player =  new Playloop(inputWaiting);
	}
	
	@Override
	public void react(SEMAINEMessage m) throws JMSException
	{
		if(m.isBytesMessage()){
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
				
				try {
					sendCallbackMessage("start");
					AudioInputStream ais = AudioSystem.getAudioInputStream(bais);
	            	AudioPlayer player = new AudioPlayer(ais);
					player.start();
					player.join();
					sendCallbackMessage("end");
					//sleep(1000);
				} catch (Exception e) {
					e.printStackTrace();
				} 
			}
		}
		
	}
	
	public void sendCallbackMessage(String type) throws JMSException {
		
		Document doc = XMLTool.newDocument("callback", SemaineML.namespaceURI);
		Element root = doc.getDocumentElement();
		Element callback = XMLTool.appendChildElement(root, SemaineML.E_EVENT , SemaineML.namespaceURI);
		callback.setAttribute("type", type);
		callback.setAttribute("data",  "audio");
		callback.setAttribute("id", "audio-01");
		callback.setAttribute(SemaineML.A_TIME,  String.valueOf(meta.getTime()));
		//callback.setTextContent("Synthesis request handled successfully.");
		
		callbackSender.sendXML(doc, meta.getTime());
		//System.out.println(XMLTool.document2String(doc));
	}
	
}
