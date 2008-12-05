package eu.semaine.gui.monitor;

import java.awt.Color;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import eu.semaine.components.Component;

public class ComponentInfo extends Info
{
	private String name;
	private List<String> receiveTopics;
	private List<String> sendTopics;
	private boolean isInput;
	private boolean isOutput;
	private Component.State state;
	private boolean topicsChanged;
	
	public ComponentInfo(String name, String[] receiveTopics, String[] sendTopics,
			boolean isInput, boolean isOutput)
	{
		this.name = name;
		this.receiveTopics = receiveTopics != null ? new ArrayList<String>(Arrays.asList(receiveTopics)) : null;
		this.sendTopics = sendTopics != null ? new ArrayList<String>(Arrays.asList(sendTopics)) : null;
		this.isInput = isInput;
		this.isOutput = isOutput;
	}
	
	public boolean isInput()
	{
		return isInput;
	}
	
	public boolean isOutput()
	{
		return isOutput;
	}
	
	public String[] receiveTopics()
	{
		return receiveTopics != null ? receiveTopics.toArray(new String[0]) : null;
	}
	
	public String[] sendTopics()
	{
		return sendTopics != null ? sendTopics.toArray(new String[0]) : null;
	}
	
	public synchronized Component.State getState()
	{
		return state;
	}
	
	public synchronized void addReceiveTopics(String... newReceiveTopics) {
		if (newReceiveTopics == null || newReceiveTopics.length == 0) return;
		if (receiveTopics != null) {
			receiveTopics.addAll(Arrays.asList(newReceiveTopics));
		} else {
			receiveTopics = new ArrayList<String>(Arrays.asList(newReceiveTopics));
		}
		setTopicsChanged(true);
	}
	
	public synchronized void addSendTopics(String... newSendTopics) {
		if (newSendTopics == null || newSendTopics.length == 0) return;
		if (sendTopics != null) {
			sendTopics.addAll(Arrays.asList(newSendTopics));
		} else {
			sendTopics = new ArrayList<String>(Arrays.asList(newSendTopics));
		}
		setTopicsChanged(true);
	}
	
	
	public synchronized void setState(Component.State newState)
	{
		state = newState;
		setChanged(true);
		if (dialog != null) {
			dialog.setText(getInfo());
		}
	}
		
	public boolean canReceive(String topicName)
	{
		if (receiveTopics == null) return false;
		for (String receiveTopic : receiveTopics) {
			if (receiveTopic.endsWith(".>")) { // prefix match
				if (topicName.startsWith(receiveTopic.substring(0, receiveTopic.length()-2)))
					return true;
			} else { // exact match
				if (receiveTopic.equals(topicName))
					return true;
			}
		}
		return false;
	}
	
	public Color getColor()
	{
		if (state == null) return Color.gray;
		switch (state) {
		case failure: return Color.red;
		case starting: return new Color(100, 255, 100);
		case ready: return Color.green;
		case stopped: return Color.blue;
		default: return Color.gray;
		}
	}
	
	@Override
	public synchronized void setChanged(boolean newChanged)
	{
		super.setChanged(newChanged);
		if (newChanged = false) {
			setTopicsChanged(false);
		}
	}
	
	public synchronized void setTopicsChanged(boolean newTopicsChanged)
	{
		topicsChanged = newTopicsChanged;
		if (topicsChanged)
			super.setChanged(true);
	}
	
	public synchronized boolean topicsChanged()
	{
		return topicsChanged;
	}
	
	public String toString() { return name; }

	public String getInfo()
	{
		StringBuilder sb = new StringBuilder();
		sb.append("Component name: ").append(name).append("\n");
		sb.append("State: ").append(state).append("\n");
		if (receiveTopics != null && receiveTopics.size() > 0) {
			sb.append("Receiving from topics:\n");
			for (String t : receiveTopics) {
				sb.append("    ").append(t).append("\n");
			}
		}
		if (sendTopics != null && sendTopics.size() > 0) {
			sb.append("Sending to topics:\n");
			for (String t : sendTopics) {
				sb.append("    ").append(t).append("\n");
			}
		}
		return sb.toString();
	}
	
	
	
	
	
	/**
	 * Helper to sort CompareInfo objects so that input components are first,
	 * output components last, and the others in an order consistent with 
	 * sending/receiving topic properties.
	 * @author marc
	 *
	 */
	public static class Comparator implements java.util.Comparator<ComponentInfo>
	{
		private Set<ComponentInfo> cis;
		public Comparator(Set<ComponentInfo> allCIs)
		{
			this.cis = allCIs;
		}
		
		public int compare(ComponentInfo a, ComponentInfo b)
		{
			if (a.equals(b)) return 0; // equal
			if (a.isInput()) {
				if (b.isInput()) return 0; // equal
				return -1; // a smaller
			}
			if (a.isOutput()) {
				if (b.isOutput()) return 0;
				return 1; // a bigger
			}
			// Compare sendTopics and receiveTopics. Basic strategy:
			// if a's sendTopics match any of b's receiveTopics, then a<b, and vice versa,
			// but if both is the case, then they are equal;
			// if no direct relationship can be found, then look iteratively for all
			// ComponentInfos which can use a's sendTopics, look at their sendTopics
			// etc. until one of b's receiveTopics is found, and count the path length;
			// in the same way, count the path length the other way round;
			// shorter path length for a->b means a<b;
			// if there are no paths, or both path lengths are equal, then a and b are equal.
			int abPathLength = countPathLength(a, b);
			int baPathLength = countPathLength(b, a);
			if (abPathLength == 0) { // no path found
				if (baPathLength == 0) return 0; // equal
				return 1; // a bigger
			} else if (baPathLength == 0) { // but abPathLength != 0
				return -1; // a smaller
			} else if (abPathLength == baPathLength) {
				return 0; // equal
			} else if (abPathLength < baPathLength) {
				return -1; // a smaller
			} else { // baPathLength > abPathLength
				return 1;  // a bigger
			}
		}

		private int countPathLength(ComponentInfo a, ComponentInfo b) {
			Set<ComponentInfo> possibleIntermediaries = new HashSet<ComponentInfo>(cis);
			possibleIntermediaries.remove(a);
			possibleIntermediaries.remove(b);
			return countPathLength(a, b, possibleIntermediaries);
		}
		
		private int countPathLength(ComponentInfo a, ComponentInfo b,
				Set<ComponentInfo> possibleIntermediaries)
		{
			if (a.sendTopics() == null || a.sendTopics().length == 0)
				return 0; // this cannot fit with any intermediaries
			if (b.receiveTopics() == null || b.receiveTopics().length == 0)
				return 0; // this cannot fit with any intermediaries
			if (sendFitsReceive(a, b)) {
				return 1; // found direct connection
			}
			Set<ComponentInfo> nextSteps = new HashSet<ComponentInfo>();
			for (ComponentInfo ci : possibleIntermediaries) {
				if (sendFitsReceive(a, ci)) {
					nextSteps.add(ci);
				}
			}
			possibleIntermediaries.removeAll(nextSteps);
			// Now for each of the potential intermediaries,
			// compute the path length recursively
			int shortest = Integer.MAX_VALUE;
			for (ComponentInfo ci : nextSteps) {
				int pathLength = countPathLength(ci, b, possibleIntermediaries);
				if (pathLength > 0 && pathLength < shortest)
					shortest = pathLength;
			}
			if (shortest < Integer.MAX_VALUE) return shortest+1;
			return 0; // no path found
		}
		
		private boolean sendFitsReceive(ComponentInfo a, ComponentInfo b) {
			if (a.sendTopics() == null || b.receiveTopics() == null) return false;
			for (String sendTopic : a.sendTopics()) {
				if (b.canReceive(sendTopic)) return true;
			}
			return false;
		}

		public boolean equals(Object obj)
		{
			return false;
		}
	}
	
}
