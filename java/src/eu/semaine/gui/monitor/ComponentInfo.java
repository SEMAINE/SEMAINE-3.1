/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.gui.monitor;

import java.awt.Color;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Random;
import java.util.Set;

import eu.semaine.components.Component;

public class ComponentInfo extends Info
{
	protected static Set<String> topicsToIgnoreWhenSorting;
	
	/**
	 * Remember a list of topics that should be ignored when sorting the components in the GUI.
	 * @param topics
	 */
	public static void setTopicsToIgnoreWhenSorting(Set<String> topics) {
		topicsToIgnoreWhenSorting = topics;
	}
	
	
	private String name;
	private String[] receiveTopics;
	private String[] sendTopics;
	private boolean isInput;
	private boolean isOutput;
	private Component.State state;
	private String stateDetails;
	private boolean topicsChanged;
	private long lastSeenAlive;
	private long avgActTime = -1;
	private long avgReactTime = -1;
	private long avgTransmitTime = -1;
	private int messagesReceived = -1;
	
	public ComponentInfo(String name, String[] receiveTopics, String[] sendTopics,
			boolean isInput, boolean isOutput)
	{
		this.name = name;
		this.receiveTopics = receiveTopics != null ? receiveTopics.clone() : null;
		this.sendTopics = sendTopics != null ? sendTopics.clone() : null;
		this.isInput = isInput;
		this.isOutput = isOutput;
		this.state = null;
		this.stateDetails = null;
		this.lastSeenAlive = 0;
	}
	
	public boolean isInput()
	{
		return isInput;
	}
	
	public void setIsInput(boolean b)
	{
		isInput = b;
		setTopicsChanged(true);
	}
	
	public boolean isOutput()
	{
		return isOutput;
	}
	
	public void setIsOutput(boolean b)
	{
		isOutput = b;
		setTopicsChanged(true);
	}
	
	public long lastSeenAlive()
	{
		return lastSeenAlive;
	}
	
	public void setLastSeenAlive(long time)
	{
		lastSeenAlive = time;
		if (dialog != null) {
			dialog.setText(getInfo());
		}
	}

	public String[] receiveTopics()
	{
		return receiveTopics;
	}
	
	public String[] sendTopics()
	{
		return sendTopics;
	}
	
	/**
	 * Indicate whether the given topic name is a callback topic.
	 * This implementation considers any topic to be a callback topic
	 * if the topicName contains the substring "callback".
	 * @param topicName
	 * @return
	 */
	public boolean isCallbackTopic(String topicName)
	{
		return topicName != null && topicName.contains("callback");
	}

	/**
	 * Indicate whether the given topic name is a data topic.
	 * This implementation considers any topic to be a data topic
	 * if the topicName contains the substring "data".
	 * @param topicName
	 * @return
	 */
	public boolean isDataTopic(String topicName)
	{
		return topicName != null && topicName.contains("data");
	}
	
	/**
	 * For the given topic, provide a topic info, including in particular the topic type.
	 * This must succeed if topicName is one of our send topics,
	 * and may or may not succeed otherwise.
	 * @param topicName
	 * @return a TopicInfo object corresponding to the given topicName if possible, null otherwise.
	 */
	public TopicInfo getTopicInfo(String topicName) {
		if (topicName == null) {
			return null;
		}
		TopicInfo.TopicType type = null;
		if (isDataTopic(topicName)) {
			type = TopicInfo.TopicType.Data;
		} else if (isCallbackTopic(topicName)) {
			type = TopicInfo.TopicType.Callback;
		}
		return new TopicInfo(topicName, type);
		
	}
	

	public synchronized Component.State getState()
	{
		return state;
	}

	public synchronized void setState(Component.State newState)
	{
		setState(newState, null);
	}
	
	public synchronized void setState(Component.State newState, String details)
	{
		if (newState != null && newState.equals(state) ||
				newState == null && state == null) {
			// state unchanged
			return;
		} 
		// else state changed
		state = newState;
		stateDetails = details;
		setChanged(true);
		if (dialog != null) {
			dialog.setText(getInfo());
		}
	}

	
	public String getStateDetails()
	{
		return stateDetails;
	}
	
	public synchronized void setReceiveTopics(String... newReceiveTopics) {
		if (newReceiveTopics == null) receiveTopics = null;
		else
			receiveTopics = newReceiveTopics.clone();
		setTopicsChanged(true);
	}
	
	public synchronized void setSendTopics(String... newSendTopics) {
		if (newSendTopics == null) sendTopics = null;
		else
			sendTopics = newSendTopics.clone();
		setTopicsChanged(true);
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
		case starting: return new Color(255, 200, 100);
		case ready: return new Color(50, 255, 50);
		case stopped: return Color.blue;
		case stalled: return new Color(255, 100, 255);
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
	
	public void setAverageActTime(long millis)
	{
		avgActTime = millis;
	}
	
	public long getAverageActTime()
	{
		return avgActTime;
	}
	
	public void setAverageReactTime(long millis)
	{
		avgReactTime = millis;
	}
	
	public long getAverageReactTime()
	{
		return avgReactTime;
	}

	public void setAverageTransmitTime(long millis)
	{
		avgTransmitTime = millis;
	}
	
	public long getAverageTransmitTime()
	{
		return avgTransmitTime;
	}
	
	public void setTotalMessagesReceived(int number)
	{
		messagesReceived = number;
	}
	
	public int getTotalMessagesReceived()
	{
		return messagesReceived;
	}

	public String toString() { return name; }

	public String getInfo()
	{
		StringBuilder sb = new StringBuilder();
		sb.append("Component name: ").append(name).append("\n");
		sb.append("State: ").append(state);
		if (stateDetails != null)
			sb.append(" (").append(stateDetails).append(")");
		sb.append("\n");
		sb.append("Last seen alive: ").append(lastSeenAlive).append("\n");
		if (avgActTime != -1) {
			sb.append("Average time spent in act(): ").append(avgActTime).append(" ms").append("\n");
		}
		if (avgReactTime != -1) {
			sb.append("Average time spent in react(): ").append(avgReactTime).append(" ms").append("\n");
		}
		if (avgTransmitTime != -1) {
			sb.append("Average travel time of incoming messages (ignoring clock differences!): ").append(avgTransmitTime).append(" ms").append("\n");
		}
		if (messagesReceived != -1) {
			sb.append("Total messages received: ").append(messagesReceived).append("\n");
		}
		if (receiveTopics != null && receiveTopics.length > 0) {
			sb.append("Receiving from topics:\n");
			for (String t : receiveTopics) {
				sb.append("    ").append(t).append("\n");
			}
		}
		if (sendTopics != null && sendTopics.length > 0) {
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
	 * sending/receiving topic properties -- taking only data topics into account
	 * (and not, e.g., callback topics).
	 * @author marc
	 *
	 */
	public static class SimpleComparator implements java.util.Comparator<ComponentInfo>
	{
		private Set<ComponentInfo> cis;
		public SimpleComparator(Set<ComponentInfo> allCIs)
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
			if (b.isInput()) {
				return 1; // b smaller
			}
			if (a.isOutput()) {
				if (b.isOutput()) return 0;
				return 1; // a bigger
			}
			if (b.isOutput()) {
				return -1; // b bigger
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
				if (!a.isDataTopic(sendTopic)) {
					continue;
				}
				if (b.canReceive(sendTopic)) {
					return true;
				}
			}
			return false;
		}

		public boolean equals(Object obj)
		{
			return false;
		}
	}
	
	/**
	 * Helper to sort CompareInfo objects so that input components are first,
	 * output components last, and the others in an order consistent with 
	 * sending/receiving topic properties -- taking only data topics into account
	 * (and not, e.g., callback topics).
	 * 
	 * This implementation counts the number of connections from a to b and from b to a;
	 * the one that is higher indicates the right order.
	 * @author marc
	 *
	 */
	public static class CountingComparator implements java.util.Comparator<ComponentInfo>
	{
		private Set<ComponentInfo> cis;
		public CountingComparator(Set<ComponentInfo> allCIs)
		{
			this.cis = allCIs;
		}
		
		public int compare(ComponentInfo a, ComponentInfo b)
		{
			if (a.equals(b)) {
				//System.out.println(a.name + " is equal to "+b.name+" because they are actually equal");
				return 0; // equal
			}
			if (a.isInput()) {
				if (b.isInput()) {
					//System.out.println(a.name + " is equal to "+b.name+" because both are input components");
					return 0; // equal
				}
				//System.out.println(a.name + " is before "+b.name+" because it is an input component");
				return -1; // a smaller
			}
			if (b.isInput()) {
				//System.out.println(b.name + " is before "+a.name+" because it is an input component");
				return 1; // b smaller
			}
			if (a.isOutput()) {
				if (b.isOutput()) {
					//System.out.println(a.name + " is equal to "+b.name+" because both are output components");
					return 0;
				}
				//System.out.println(a.name + " is after "+b.name+" because it is an output component");
				return 1; // a bigger
			}
			if (b.isOutput()) {
				//System.out.println(b.name + " is after "+a.name+" because it is an output component");
				return -1; // b bigger
			}
			// Compare sendTopics and receiveTopics. Basic strategy:
			// 1. count number of connections from a to b, and number of connections from b to a.
			// 2. if #ab > #ba, then a < b; if #ab < #ba, then a > b;
			// 3. if #ab == #ba, then both are equal.
			int numABPaths = countPaths(a, b);
			int numBAPaths = countPaths(b, a);
			if (numABPaths > numBAPaths) {
				//System.out.println(a.name + " is before "+b.name+" because AB="+numABPaths+" > BA="+numBAPaths);
				return -1; // a smaller
			} else if (numABPaths < numBAPaths) {
				//System.out.println(b.name + " is before "+a.name+" because AB="+numBAPaths+" > BA="+numABPaths);
				return 1; // a bigger
			} else { // ==
				//System.out.println(a.name + " is equal to "+b.name+" because AB="+numABPaths+" = BA");
				return 0;
			}
		}

		private int countPaths(ComponentInfo a, ComponentInfo b) {
			Set<ComponentInfo> possibleIntermediaries = new HashSet<ComponentInfo>(cis);
			possibleIntermediaries.remove(a);
			possibleIntermediaries.remove(b);
			return countPaths(a, b, possibleIntermediaries);
		}
		
		private int countPaths(ComponentInfo a, ComponentInfo b,
				Set<ComponentInfo> possibleIntermediaries)
		{
			if (a.sendTopics() == null || a.sendTopics().length == 0)
				return 0; // this cannot fit with any intermediaries
			if (b.receiveTopics() == null || b.receiveTopics().length == 0)
				return 0; // this cannot fit with any intermediaries
			int totalPaths = countDirectConnections(a, b);
			Set<ComponentInfo> nextSteps = new HashSet<ComponentInfo>();
			for (ComponentInfo ci : possibleIntermediaries) {
				if (countDirectConnections(a, ci) > 0) {
					nextSteps.add(ci);
				}
			}
			Set<ComponentInfo> nextIntermediaries = new HashSet<ComponentInfo>(possibleIntermediaries);
			nextIntermediaries.removeAll(nextSteps);

			for (ComponentInfo ci : nextSteps) {
				int numA2CI = countDirectConnections(a, ci);
				int numFromCiToB = countPaths(ci, b, nextIntermediaries);
				if (numFromCiToB > 0) {
					totalPaths += numA2CI * numFromCiToB;
				}
			}
			return totalPaths;
		}
		
		private int countDirectConnections(ComponentInfo a, ComponentInfo b) {
			int num = 0;
			for (String sendTopic : a.sendTopics()) {
				// ignore it if it's not a data topic or if user requested to hide or ignore it:
				if (!a.isDataTopic(sendTopic) || topicsToIgnoreWhenSorting.contains(sendTopic)) {
					continue;
				}
				if (b.canReceive(sendTopic)) {
					num++;
				}
			}
			return num;
		}

		public boolean equals(Object obj)
		{
			return false;
		}
	}
	
	/**
	 * Helper to sort CompareInfo objects so that input components are first,
	 * output components last, and the others in an order consistent with 
	 * sending/receiving topic properties -- taking only data topics into account
	 * (and not, e.g., callback topics).
	 * 
	 * This implementation uses a voting mechanism for determining the order of components,
	 * taking output-input relationships between components as partial ordering requests.
	 * 
	 *  In detail, the algorithm proceeds as follows.
	 *  <ol>
	 *  <li>
	 *  If component i publishes to a data Topic and component j subscribes to that Topic,
	 *  this is taken as a partial ordering request (POR);
	 *  </li>
	 *  <li>
	 *  In an iterative loop, PORs vote for components to either stay where they are, or move left or right.
	 *  For a POR involving components i and j, the POR will vote for both to stay where they are if the 
	 *  current position of component j is to the right of component i; otherwise, it will vote for both 
	 *  to move such that they get closer to that configuration. 
	 *  </li>
	 *  <li>
	 *  After taking all PORs into account, the votes for each component are analysed as follows. The net requests 
	 *  to move left or right are computed by substraction; only if the absolute value of this difference is larger
	 *  than the votes to stay in place will the component move in the appropriate direction. A move is effected
	 *  by increasing the position of the component by 1 to move right, and by decreasing it by 1 to move left. 
	 *  </li>
	 *  <li>
	 *  The algorithm stops when all components stay in place.
	 *  </li>
	 *  </ol>
	 *  <b>Doesn't work properly. Leaving code just in case some time I want to build on top of this.</b>
	 * @author marc
	 *
	 */
	@Deprecated
	public static class VoteComparator implements java.util.Comparator<ComponentInfo>
	{
		private ComponentInfo[] components;
		private Map<ComponentInfo, Integer> componentIndices;
		private int[] positions;
		private boolean[] isFixed;
		
		public VoteComparator(Set<ComponentInfo> allCIs, final Set<String> topicsToHide)
		{
			this.components = (ComponentInfo[]) allCIs.toArray(new ComponentInfo[0]);
			this.componentIndices = new HashMap<ComponentInfo, Integer>();
			this.positions = new int[components.length];
			this.isFixed = new boolean[components.length];
			
			System.out.println("Starting with components in the following order:");
			for (int i=0; i<components.length; i++) {
				System.out.println(i+" "+components[i].name);
				componentIndices.put(components[i], i); // for faster lookup in compare()
				if (components[i].isInput()) {
					positions[i] = 0;
					isFixed[i] = true;
				} else if (components[i].isOutput()) {
					positions[i] = components.length - 1;
					isFixed[i] = true;
				} else {
					positions[i] = i;
					isFixed[i] = false;
				}
			}
			// each POR consists of the index numbers of the components involved in a
			// publish-subscribe relationship.
			List<int[]> partialOrderingRequests = new ArrayList<int[]>();
			for (int i=0; i<components.length; i++) {
				String[] sendTopics = components[i].sendTopics();
				if (sendTopics == null) {
					continue;
				}
				for (String sendTopic : sendTopics) {
					if (topicsToHide.contains(sendTopic) || !components[i].isDataTopic(sendTopic)) {
						continue;
					}
					for (int j=0; j<components.length; j++) {
						if (i == j) {
							continue;
						}
						if (components[j].canReceive(sendTopic)) {
							// i provides direct input to j, so j should immediately follow i
							partialOrderingRequests.add(new int[] {i, j});
						}
					}
				}
			}
			// Now move component positions according to votes until there are no more changes:
			boolean isChanged = true;
			int maxIterations = 20; // just to make sure we don't ever loop infinitely
			int iteration = 0;
			int votesForFixedComponents = 10;
			Random random = new Random();
			while (isChanged && iteration++ < maxIterations) {
				isChanged = false;
				int[][] votes = new int[components.length][2]; // 0: votes to stay in place; 1: votes to move left (<0) or right (>0)
				for (int[] por : partialOrderingRequests) {
					int i = por[0];
					int j = por[1];
					if (isFixed[i] && isFixed[j]) {
						continue;
					}
					if (positions[i] + 1 == positions[j]) {
						System.out.println(components[i].name+" and "+components[j].name+" are in right order");
						if (isFixed[i]) {
							System.out.println(components[i].name+" votes for "+components[j].name+" to stay in place");
							// vote to stay in place
							votes[j][0] += votesForFixedComponents;
						} else if (isFixed[j]) {
							System.out.println(components[j].name+" votes for "+components[i].name+" to stay in place");
							// vote to stay in place on both
							votes[i][0] += votesForFixedComponents;
						}
					} else if (positions[i] < positions[j]) {
						System.out.print("POR "+components[i].name+"->"+components[j].name+" votes for ");
						// vote to move component i right and component j left
						if (isFixed[i]) {
							System.out.println(components[j].name+" left by "+votesForFixedComponents);
							votes[j][1] -= votesForFixedComponents;
						} else if (isFixed[j]) {
							System.out.println(components[i].name+" right by "+votesForFixedComponents);
							votes[i][1] += votesForFixedComponents;
						} else {
							System.out.println(components[i].name+" right");
							votes[i][1]++;
							System.out.println(components[j].name+" left");
							votes[j][1]--;
/*							// flip a coin
							if (random.nextBoolean()) {
								System.out.println(components[i].name+" right");
								votes[i][1]++;
							} else {
								System.out.println(components[j].name+" left");
								votes[j][1]--;
							}
	*/					}
					} else { // positions[i] >= positions[j]
						System.out.print("POR "+components[i].name+"->"+components[j].name+" votes for ");
						// vote to move component i left and component j right
						System.out.println(components[i].name+" left");
						votes[i][1]--;
						System.out.println(components[j].name+" right");
						votes[j][1]++;
/*						// flip a coin
						if (random.nextBoolean()) {
							System.out.println(components[i].name+" left");
							votes[i][1]--;
						} else {
							System.out.println(components[j].name+" right");
							votes[j][1]++;
						}
	*/				}
				}
				System.out.println("Iteration "+iteration);
				// Now, move only the mobile components:
				for (int i=0; i<components.length; i++) {
					System.out.println("Component "+i+" ("+components[i].name+"):"+(isFixed[i]?" fixed":"")+" position="+positions[i]+", stay="+votes[i][0]+", move="+votes[i][1]);
					if (isFixed[i]) {
						continue;
					}
					if (votes[i][0] >= Math.abs(votes[i][1])) {
						// votes to stay in place win
						// if we stay and we are a neighbour of a fixed component, we become a fixed component
						for (int j=0; j<components.length; j++) {
							if (isFixed[j] && (positions[i] + 1 == positions[j] || positions[i] - 1 == positions[j])) {
								isFixed[i] = true;
								//System.out.println("Fixing component "+components[i].name+" because it stays a neighbour of "+components[j].name);
								break;
							}
						}
						continue;
					}
					// votes to move win
					if (votes[i][1] > 0) { // move right
						positions[i]++;
						//System.out.println("Moving component "+ components[i].name+ " right (-> "+positions[i]+")");
					} else { // move left
						positions[i]--;
						//System.out.println("Moving component "+ components[i].name+ " left (-> "+positions[i]+")");
					}
					isChanged = true;
				}
			}
			System.out.println("Sorting completed after "+iteration+" iterations");
			
		}
		
		public int compare(ComponentInfo a, ComponentInfo b)
		{
			if (a.equals(b)) return 0; // equal
			if (a.isInput()) {
				if (b.isInput()) return 0; // equal
				return -1; // a smaller
			}
			if (b.isInput()) {
				return 1; // b smaller
			}
			if (a.isOutput()) {
				if (b.isOutput()) return 0;
				return 1; // a bigger
			}
			if (b.isOutput()) {
				return -1; // b bigger
			}
			// Find index position of a and b:
			int ia = componentIndices.get(a);
			int ib = componentIndices.get(b);
			Integer pa = new Integer(positions[ia]);
			Integer pb = new Integer(positions[ib]);
			return pa.compareTo(pb);
		}

		public boolean equals(Object obj)
		{
			return false;
		}
	}
}
