/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.dummy;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.StringReader;

import javax.jms.JMSException;

import eu.semaine.components.Component;
import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.exceptions.SystemConfigurationException;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.message.SEMAINEXMLMessage;
import eu.semaine.jms.receiver.BMLReceiver;
import eu.semaine.jms.sender.Sender;
import eu.semaine.util.SEMAINEUtils;

/**
 * This dummy action selection accepts all candidates.
 * 
 * @author marc
 *
 */
public class DummyBMLRealiser extends Component 
{
	private BMLReceiver bmlReceiver;
	private Sender fapSender;

	private String dummyFAPData1;
	private String dummyFAPData2;
  private boolean nextIsOne = true;
  
	/**
	 * @param componentName
	 * @throws JMSException
	 */
	public DummyBMLRealiser() throws JMSException 
	{
		super("DummyBMLRealiser");
		bmlReceiver = new BMLReceiver("semaine.data.synthesis.plan");
		receivers.add(bmlReceiver);
		fapSender = new Sender("semaine.data.synthesis.lowlevel.video", "FAP", getName());
		senders.add(fapSender);
		
		try {
			dummyFAPData1 = SEMAINEUtils.getStreamAsString(this.getClass().getResourceAsStream("example2.fap"), "ASCII");
			//dummyFAPData2 = SEMAINEUtils.getStreamAsString(this.getClass().getResourceAsStream("example3.fap"), "ASCII");
		} catch (IOException ioe) {
			throw new SystemConfigurationException("Cannot get FAP example", ioe);
		}
	}
	

	@Override
	public void react(SEMAINEMessage m) throws JMSException
	{
		if (!(m instanceof SEMAINEXMLMessage)) {
			throw new MessageFormatException("expected XML message, got "+m.getClass().getSimpleName());
		}
		SEMAINEXMLMessage xm = (SEMAINEXMLMessage)m;
		boolean isBML = "BML".equals(xm.getDatatype());
		if (!isBML)
			throw new MessageFormatException("Expected BML message, got "+xm.getDatatype());
	    String dummyFAPData;
	    if (true || nextIsOne) {
	        dummyFAPData = dummyFAPData1;
	        nextIsOne = false;
	    } else {
	        dummyFAPData = dummyFAPData2;
	        nextIsOne = true;
	    }
	    
	    // Replace the time codes with current ones.
	    StringBuilder faps = new StringBuilder();
	    try {
	    	int n = 0; // line number
	    	long time = (meta.getTime() + 2000); // intended start time: two seconds from now
	    	time -= time % 40; // make sure time is a multiple of 40, Greta needs that
			String line;
			BufferedReader sr = new BufferedReader(new StringReader(dummyFAPData));
			while ((line = sr.readLine()) != null) {
				if (n % 2 == 0) {
					faps.append(time);
					// discard number before the first space
					int iSpace = line.indexOf(' ');
					if (iSpace >= 0) faps.append(line.substring(iSpace));
					faps.append("\n");
				} else {
					faps.append(line).append("\n");
				}
				n++;
				time += 40;
			}
		} catch (IOException e) {
			// shouldn't happen
		}
	    
		fapSender.sendTextMessage(faps.toString(), meta.getTime());
	}
}
