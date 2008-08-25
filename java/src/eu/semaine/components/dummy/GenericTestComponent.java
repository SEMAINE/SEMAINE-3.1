package eu.semaine.components.dummy;

import javax.jms.JMSException;

import eu.semaine.components.Component;
import eu.semaine.datatypes.SEMAINEMessage;
import eu.semaine.jms.Receiver;
import eu.semaine.jms.Sender;

public class GenericTestComponent extends Component
{
	private int boredTime = 3000; // ms
	private long lastMessageTime = 0;
	
	public GenericTestComponent(String name, String receiveTopic, String sendTopic)
	throws JMSException
	{
		super(name);
		receivers.add(new Receiver(receiveTopic));
		senders.add(new Sender(sendTopic, sendTopic, this.getClass().getSimpleName()));
	}

	@Override
	protected void act()
	throws JMSException
	{
		long time = System.currentTimeMillis();
		if (time - lastMessageTime > boredTime) {
			senders.get(0).sendTextMessage("I'm bored", time);
			lastMessageTime = time;
		}
	}
	
	@Override
	protected void react(SEMAINEMessage message)
	throws JMSException
	{
		String text = message.getText();
		log.info("Received message from '"+message.getSource()+"' of type '"+message.getDatatype()+"':");
		log.debug(text);
		try {	
			Thread.sleep(1000);
		} catch (InterruptedException ie) {
			log.debug("sleep interrupted", ie);
		}
		long time = System.currentTimeMillis();
		senders.get(0).sendTextMessage("Hello there", time);
		lastMessageTime = time;
	}

}
