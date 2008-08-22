package eu.semaine.components.dummy;

import javax.jms.JMSException;

import eu.semaine.components.Component;
import eu.semaine.jms.Receiver;
import eu.semaine.jms.SEMAINEMessage;
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
	protected void proactiveActions()
	{
		try {
			long time = System.currentTimeMillis();
			if (time - lastMessageTime > boredTime) {
				senders.get(0).sendTextMessage("I'm bored", time);
				lastMessageTime = time;
			}
		} catch (Exception e) {
			e.printStackTrace(System.err);
		}
	}
	
	@Override
	protected void processMessage(SEMAINEMessage message) 
	{
		try {
			String text = message.getText();
			System.out.println("Received message from '"+message.getSource()+"' of type '"+message.getDatatype()+"':");
			System.out.println(text);
			System.out.println();
			Thread.sleep(1000);
			long time = System.currentTimeMillis();
			senders.get(0).sendTextMessage("Hello there", time);
			lastMessageTime = time;
		} catch (Exception e) {
			e.printStackTrace(System.err);
		}
	}

}
