package eu.semaine.gui.monitor;

import java.awt.Color;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;


public class TopicInfo extends Info
{
	private static final int MEMORY = 100; // number of topic posts to remember
	private static final Color ACTIVECOLOR = new Color(255, 255, 100);
	private static final Color PASSIVECOLOR = new Color(180, 180, 180);
	private static final int STEPS = 6;
	private static final int STEPTIME = 500; // in ms
	
	private String name;
	private Map<String,ConnectionInfo> sendingComponents;
	private Map<String,ConnectionInfo> receivingComponents;
	private LinkedList<String> messages;
	private int currentStep;
	private long nextStepDue;
	
	public TopicInfo(String name)
	{
		this.name = name;
		sendingComponents = new HashMap<String, ConnectionInfo>();
		receivingComponents = new HashMap<String, ConnectionInfo>();
		messages = new LinkedList<String>();
		currentStep = 0;
		nextStepDue = 0;
	}
	
	public String getName() { return name; }
	
	public Map<String,ConnectionInfo> sendingComponents() { return sendingComponents; }

	public Map<String,ConnectionInfo> receivingComponents() { return receivingComponents; }

	public String toString()
	{
		String prefix = "semaine.data.";
		if (name.startsWith(prefix)) {
			return name.substring(prefix.length());
		}
		return name; 
	}
	
	public void addMessage(String message, String sender)
	{
		if (messages.size() >= MEMORY) {
			messages.removeFirst();
		}
		messages.addLast(message);
		setActive();
		ConnectionInfo senderCI = sendingComponents.get(sender);
		if (senderCI != null) {
			senderCI.setActive();
		}
		if (dialog != null) {
			dialog.setText(getInfo());
		}
	}

	private void setActive()
	{
		long time = System.currentTimeMillis();
		if (currentStep == STEPS && nextStepDue > time) {
			// this one is already showing, push back next one:
			nextStepDue = time+STEPTIME;
		} else {
			currentStep = STEPS;
			nextStepDue = time; // now!
		}
	}


	public Color getColor()
	{
		if (currentStep == 0) return PASSIVECOLOR;
		if (currentStep == STEPS) return ACTIVECOLOR;
		// interpolate
		assert currentStep > 0 && currentStep < STEPS;
		int red = PASSIVECOLOR.getRed() + (ACTIVECOLOR.getRed()-PASSIVECOLOR.getRed())*currentStep/STEPS;
		int green = PASSIVECOLOR.getGreen() + (ACTIVECOLOR.getGreen()-PASSIVECOLOR.getGreen())*currentStep/STEPS;
		int blue = PASSIVECOLOR.getBlue() + (ACTIVECOLOR.getBlue()-PASSIVECOLOR.getBlue())*currentStep/STEPS;
		return new Color(red, green, blue);
	}
	
	@Override
	public synchronized boolean needsUpdate()
	{
		if (currentStep > 0) {
			long currentTime = System.currentTimeMillis();
			if (currentTime >= nextStepDue) {
				nextStepDue += STEPTIME;
				currentStep--;
				return true;
			}
		}
		return false;
	}
	
	public String getInfo()
	{
		StringBuilder sb = new StringBuilder();
		for (String message : messages) {
			sb.append(message);
			sb.append("\n\n");
		}
		return sb.toString();
	}
}
