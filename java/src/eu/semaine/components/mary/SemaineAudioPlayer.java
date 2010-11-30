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
	private BlockingQueue<SEMAINEBytesMessage> inputWaiting;
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
		inputWaiting   = new LinkedBlockingQueue<SEMAINEBytesMessage>();
		player =  new Playloop(inputWaiting);
	}
	
	@Override
	public void react(SEMAINEMessage m) throws JMSException
	{
		if(m.isBytesMessage()){
			assert m instanceof SEMAINEBytesMessage;
			SEMAINEBytesMessage bm = (SEMAINEBytesMessage)m;
			try {
				inputWaiting.put(bm);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
		
	}
	
	public final class Playloop extends Thread{
		
		protected BlockingQueue<SEMAINEBytesMessage> inputWaiting;
		protected boolean playing = false;
		
		public Playloop(BlockingQueue<SEMAINEBytesMessage> inputWaiting){
			this.inputWaiting = inputWaiting;	
			start();
		}
		
		public void run(){
			for (;;)  {  
				SEMAINEBytesMessage bm = null;
				try {
					// block until input becomes available
					bm = inputWaiting.poll(waitingTime, TimeUnit.MILLISECONDS);
					if(bm == null) continue;
				} catch (InterruptedException ie) {
					// if we have no input, we'll keep on waiting
					continue;
				}
				ByteArrayInputStream bais = new ByteArrayInputStream(bm.getBytes());
				
				try {
					sendCallbackMessage("start", bm.getContentID());
					log.debug(bm.getContentID()+" started playing "+(meta.getTime()-bm.getContentCreationTime())+" ms after creation");
					AudioInputStream ais = AudioSystem.getAudioInputStream(bais);
	            	AudioPlayer player = new AudioPlayer(ais);
					player.start();
					player.join();
					sendCallbackMessage("end", bm.getContentID());
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
		callback.setAttribute("type", type);
		callback.setAttribute("data",  "audio");
		callback.setAttribute("id", contentID != null ? contentID : "unknown");
		callback.setAttribute(SemaineML.A_TIME,  String.valueOf(meta.getTime()));
		//callback.setTextContent("Synthesis request handled successfully.");
		
		callbackSender.sendXML(doc, meta.getTime());
		//System.out.println(XMLTool.document2String(doc));
	}
	
}
