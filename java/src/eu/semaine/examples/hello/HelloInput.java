package eu.semaine.examples.hello;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

import javax.jms.JMSException;

import eu.semaine.components.Component;
import eu.semaine.jms.sender.Sender;

public class HelloInput extends Component {
	
	private Sender textSender = new Sender("semaine.data.hello.text", "TEXT", getName());;
	private BufferedReader inputReader = new BufferedReader(new InputStreamReader(System.in));
	
	public HelloInput() throws JMSException {
		super("HelloInput", true/*is input*/, false);
		senders.add(textSender);
	}
	
	@Override
	protected void act() throws IOException, JMSException {
		if (inputReader.ready()) {
			String line = inputReader.readLine();
			textSender.sendTextMessage(line, meta.getTime());
		}
	}
}
