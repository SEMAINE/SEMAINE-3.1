/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.gui.monitor;

import java.awt.Color;
import java.awt.Font;
import java.awt.font.FontRenderContext;
import java.awt.font.TextLayout;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;


public class TopicInfo extends Info
{
	public enum TopicType { Data, Callback };
	
	private static final int MEMORY = 100; // number of topic posts to remember
	private static final Color ACTIVECOLOR = new Color(255, 255, 100);
	private static final Color PASSIVECOLOR_DATA = new Color(180, 180, 180);
	private static final Color PASSIVECOLOR_CALLBACK = new Color(90, 180, 255);
	private static final int STEPS = 2;
	private static final int STEPTIME = 500; // in ms
	
	private TopicType type;
	private Map<String,ConnectionInfo> sendingComponents;
	private Map<String,ConnectionInfo> receivingComponents;
	private LinkedList<String> messages;
	private int currentStep;
	private long nextStepDue;
	private boolean isShowingActive = false;
	
	public TopicInfo(String name, TopicType type)
	{
		super(name);
		this.type = type;
		sendingComponents = new HashMap<String, ConnectionInfo>();
		receivingComponents = new HashMap<String, ConnectionInfo>();
		messages = new LinkedList<String>();
		currentStep = 0;
		nextStepDue = 0;
	}
	

	
	public TopicType getType() {
		return type;
	}
	
	public Map<String,ConnectionInfo> sendingComponents() { return sendingComponents; }

	public Map<String,ConnectionInfo> receivingComponents() { return receivingComponents; }

	/**
	 * Compute how to display the name in the message flow graph. We shorten it to gain readability.
	 */
	@Override
	protected String computeLabel(String name)
	{
		String[] prefixes = new String[] {
				"semaine.data.",
				"semaine.callback."
		};
		String shortname = name;
		for (String prefix : prefixes) {
			if (name.startsWith(prefix)) {
				shortname = name.substring(prefix.length());
			}
		}
		// Now measure if we will fit into the space available:
		Font font = SystemMonitor.getLabelFont();
		FontRenderContext frc = new FontRenderContext(null, true, true);
		TextLayout tl = new TextLayout(shortname, font, frc);
		float width = tl.getVisibleAdvance();
		int allowedWidth = SystemMonitor.getComponentWidth() - 4; // leave two pixels at each end
		if (width > allowedWidth) {
//			System.out.println("Topic label '"+shortname+"' is too wide: "+width+" is more than "+allowedWidth);
			shortname = shortenLabel(shortname, allowedWidth, font, frc, true);
		}
		return shortname; 
	}



	/**
	 * Shorten the given text so that it fits within the allowed width.
	 * The idea is to take the final part preceded by two dots preceded by whatever fits, e.g.
	 * 'state.user.emma.emotion.voice' -> 'state.user.e..voice'. 
	 * If shortname contains no dot, use the prefix that fits if followed by two dots.
	 * @param longLabel
	 * @param font
	 * @param frc
	 * @return the shortened label
	 */
	private String shortenLabel(String longLabel, float allowedWidth, Font font, FontRenderContext frc, boolean splitAtDot) {
		float longWidth = new TextLayout(longLabel, font, frc).getVisibleAdvance();
//		System.out.println("Now trying to shorten '"+longLabel+"' from "+((int)longWidth)+" to "+((int)allowedWidth));
		if (longWidth < allowedWidth) {
			return longLabel;
		}
		float twodotWidth = new TextLayout("..", font, frc).getVisibleAdvance();
		if (twodotWidth >= allowedWidth) {
			// well, we can't do anything
			return "..";
		}
		int lastDotPos = longLabel.lastIndexOf('.');
		if (splitAtDot && lastDotPos > -1) {
			String finalPart = longLabel.substring(lastDotPos+1);
			float finalPartWidth = new TextLayout(finalPart, font, frc).getVisibleAdvance();
			if (twodotWidth+finalPartWidth > allowedWidth) { // cannot even show the full final part
				String finalPartShortened = shortenLabel(finalPart, allowedWidth - twodotWidth, font, frc, false);
				return ".."+finalPartShortened;
			}
			// Now for the other parts
			String prefix = longLabel.substring(0, lastDotPos);
			prefix = shortenLabel(prefix, allowedWidth - finalPartWidth, font, frc, false);
			return prefix+finalPart;
		}
		// no dot -- just shorten
		// Use longest possible prefix that fits
		// Estimate:
		int len = (int) ((float)longLabel.length() * (allowedWidth - twodotWidth) / longWidth);
		assert len >= 0;
		if (len == 0) { // no space except for the dots
			return "..";
		}
		String prefix = longLabel.substring(0, len);
		float prefixWidth = new TextLayout(prefix, font, frc).getVisibleAdvance();
		while (prefix.length() > 0 && prefixWidth > allowedWidth - twodotWidth) {
			prefix = prefix.substring(0, prefix.length() - 1);
			prefixWidth = new TextLayout(prefix, font, frc).getVisibleAdvance();
		}
		return prefix+"..";
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
			dialog.appendText(message+"\n\n");
		}
	}

	private synchronized void setActive()
	{
		long time = System.currentTimeMillis();
		if (isShowingActive) {
			// this one is already showing, push back next one:
			nextStepDue = time+STEPTIME;
		} else {
			currentStep = STEPS;
			nextStepDue = time; // now!
		}
	}


	public synchronized Color getColor()
	{
		Color passiveColor = TopicType.Callback.equals(type) ? PASSIVECOLOR_CALLBACK : PASSIVECOLOR_DATA;
		if (currentStep == STEPS) {
			isShowingActive = true;
			return ACTIVECOLOR;
		}
		isShowingActive = false;
		if (currentStep == 0) {
			return passiveColor;
		}
		// interpolate
		assert currentStep > 0 && currentStep < STEPS : "currentStep="+currentStep+" is out of expected range [0, "+STEPS+"]";
		int red = passiveColor.getRed() + (ACTIVECOLOR.getRed()-passiveColor.getRed())*currentStep/STEPS;
		int green = passiveColor.getGreen() + (ACTIVECOLOR.getGreen()-passiveColor.getGreen())*currentStep/STEPS;
		int blue = passiveColor.getBlue() + (ACTIVECOLOR.getBlue()-passiveColor.getBlue())*currentStep/STEPS;
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
