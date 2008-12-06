package eu.semaine.gui.monitor;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Toolkit;
import java.awt.geom.Point2D;
import java.awt.geom.Rectangle2D;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Map.Entry;

import javax.swing.BorderFactory;
import javax.swing.JFrame;
import javax.swing.JScrollPane;

import org.jgraph.JGraph;
import org.jgraph.event.GraphSelectionEvent;
import org.jgraph.event.GraphSelectionListener;
import org.jgraph.graph.DefaultEdge;
import org.jgraph.graph.DefaultGraphCell;
import org.jgraph.graph.DefaultGraphModel;
import org.jgraph.graph.GraphConstants;
import org.jgraph.graph.GraphLayoutCache;
import org.jgraph.graph.GraphModel;

import eu.semaine.components.Component;

public class SystemMonitor extends Thread
{
	private JFrame frame;
	private JGraph graph;
	private Dimension frameSize = null;
	private static int componentWidth = 120;
	private static int componentHeight = 30;
	
	private List<ComponentInfo> sortedComponentList;
	private boolean componentListChanged = false;
	private List<List<Info>> infoGroups;
	private Map<String, TopicInfo> topics;
	private List<DefaultGraphCell> cells;
	private List<DefaultEdge> edges;
	
	public SystemMonitor(ComponentInfo[] componentInfos)
	{
		// Construct Model and Graph
		GraphModel model = new DefaultGraphModel();
		graph = new JGraph(model);
		graph.setEditable(false);
		graph.setMoveable(true);
		graph.setConnectable(false);
		graph.setDisconnectable(false);
		graph.setSelectionEnabled(true);

		graph.addGraphSelectionListener(new InfoGraphSelectionListener());

		GraphLayoutCache view = graph.getGraphLayoutCache();
		view.setFactory(new MyCellViewFactory());
		
		frame = new JFrame();
		frame.getContentPane().add(new JScrollPane(graph));
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setSize(Toolkit.getDefaultToolkit().getScreenSize());

		// Sort components:
		if (componentInfos != null)
			sortedComponentList = new ArrayList<ComponentInfo>(Arrays.asList(componentInfos));
		else
			sortedComponentList = new ArrayList<ComponentInfo>();
		topics = new HashMap<String, TopicInfo>();
		cells = new ArrayList<DefaultGraphCell>();
		infoGroups = new LinkedList<List<Info>>();
		updateCells();
				
		frame.setVisible(true);
		redraw();

	}
	
	public void addComponentInfo(ComponentInfo ci)
	{
		sortedComponentList.add(ci);
		componentListChanged = true;
	}
	
	private void redraw()
	{
		assert cells != null;
		Map<DefaultGraphCell, Map<Object,Object>> allChanges = new HashMap<DefaultGraphCell, Map<Object,Object>>();
		boolean mustUpdateCells = false;
		boolean mustLayoutCells = false;
		Dimension newFrameSize = frame.getSize();
		if (!newFrameSize.equals(frameSize)) {
			frameSize = newFrameSize;
			mustLayoutCells = true;
		}
		if (componentListChanged) {
			mustUpdateCells = true;
			mustLayoutCells = true;
			componentListChanged = false;
		}

		for (DefaultGraphCell cell : cells) {
			Object userObject = cell.getUserObject();
			Info inf = null;
			if (userObject != null && userObject instanceof Info) {
				inf = (Info) userObject;
			}
			if (inf != null && inf.needsUpdate()) {
				Map<Object,Object> attributes = allChanges.get(cell);
				if (attributes == null) {
					attributes = new HashMap<Object, Object>();
					allChanges.put(cell, attributes);
				}
				if (inf instanceof ComponentInfo) {
					ComponentInfo compInf = (ComponentInfo) inf;
					GraphConstants.setBackground(attributes, compInf.getColor());
					if (compInf.topicsChanged()) {
						mustUpdateCells = true;
						mustLayoutCells = true;
					}
				} else if (inf instanceof TopicInfo) {
					TopicInfo topicInf = (TopicInfo) inf;
					GraphConstants.setBackground(attributes, topicInf.getColor());
				}
				inf.setChanged(false);
			}
		}
		if (mustUpdateCells) {
			updateCells();
		}
		if (mustLayoutCells) {
			if (edges != null) {
				graph.getGraphLayoutCache().remove(edges.toArray());
				edges = null;
			}
			layoutCells(allChanges);
		}
		if (edges != null) {
			for (DefaultEdge edge : edges) {
				Object userObject = edge.getUserObject();
				if (userObject == null) continue;
				assert userObject instanceof ConnectionInfo;
				ConnectionInfo connInf = (ConnectionInfo) userObject;
				if (connInf.needsUpdate()) {
					Map<Object,Object> attributes = allChanges.get(edge);
					if (attributes == null) {
						attributes = new HashMap<Object, Object>();
						allChanges.put(edge, attributes);
					}
					GraphConstants.setLineWidth(attributes, connInf.getLineWidth());
					connInf.setChanged(false);
				}
			}
		}
		if (!allChanges.isEmpty()) {
			graph.getGraphLayoutCache().edit(allChanges);
			
			if (mustLayoutCells) {
				createAllArrows();
			}
		}
		
	}
	

	private void updateCells()
	{
		assert cells != null;
		if (sortedComponentList.isEmpty()) return;
		// each time we get here, we expect to know about new topics,
		// so we need to sort again the components:
		Comparator<ComponentInfo> ciComparator = new ComponentInfo.Comparator(
				new HashSet<ComponentInfo>(sortedComponentList)); 
		Collections.sort(sortedComponentList, ciComparator);
		// Group components:
		infoGroups.clear();
		List<Info> currentGroup = new LinkedList<Info>();
		currentGroup.add(sortedComponentList.get(0));
		infoGroups.add(currentGroup);
		for (int i=1, max=sortedComponentList.size(); i<max; i++) {
			assert currentGroup != null;
			assert !currentGroup.isEmpty();
			if (ciComparator.compare((ComponentInfo)currentGroup.get(0), sortedComponentList.get(i))==0) { // equal
				currentGroup.add(sortedComponentList.get(i));
			} else { // not equal
				currentGroup = new LinkedList<Info>();
				currentGroup.add(sortedComponentList.get(i));
				infoGroups.add(currentGroup);
			}
		}
		// add Topics to infoGroups
		Set<TopicInfo> topicsInGraph = new HashSet<TopicInfo>();
		for (int i=0; i<infoGroups.size(); i++) {
			if (! (infoGroups.get(i).get(0) instanceof ComponentInfo))
				continue;
			List<Info> groupTopics = new LinkedList<Info>();
			for (Info inf : infoGroups.get(i)) {
				ComponentInfo ci = (ComponentInfo) inf;
				String[] sendTopics = ci.sendTopics();
				if (sendTopics == null) continue;
				for (String topicName : sendTopics) {
					TopicInfo ti = topics.get(topicName);
					if (ti == null) {
						// a new topic
						ti = new TopicInfo(topicName);
						topics.put(topicName, ti);
					}
					if (!topicsInGraph.contains(ti)) {
						groupTopics.add(ti);
						topicsInGraph.add(ti);
					}
				}
			}
			if (!groupTopics.isEmpty()) {
				infoGroups.add(i+1, groupTopics);
				i++; // skip the newly added group
			}
		}

		// Create cells for components
		List<DefaultGraphCell> newCells = new ArrayList<DefaultGraphCell>();
		for (ComponentInfo ci : sortedComponentList) {
			if (ci.getCell() != null) continue;
			DefaultGraphCell cell = new DefaultGraphCell(ci);
			GraphConstants.setOpaque(cell.getAttributes(), true);
			GraphConstants.setBackground(cell.getAttributes(), ci.getColor());
			GraphConstants.setBorderColor(cell.getAttributes(), Color.black);
			cell.addPort();
			newCells.add(cell);
			ci.setCell(cell);
		}
		// Create cells for topics
		for (TopicInfo ti : topics.values()) {
			if (ti.getCell() != null) continue;
			DefaultGraphCell cell = new DefaultGraphCell(ti);
			GraphConstants.setBorder(cell.getAttributes(), BorderFactory.createRaisedBevelBorder());
			GraphConstants.setBackground(cell.getAttributes(), ti.getColor());
			GraphConstants.setOpaque(cell.getAttributes(), true);
			cell.addPort();
			newCells.add(cell);
			ti.setCell(cell);
		}
		cells.addAll(newCells);
		graph.getGraphLayoutCache().insert(newCells.toArray());

	}
	
	private void createAllArrows()
	{
		List<DefaultEdge> newEdges = new ArrayList<DefaultEdge>();
		// Create cells for arrows
		for (ComponentInfo ci : sortedComponentList) {
			String[] sendTopics = ci.sendTopics();
			if (sendTopics != null) {
				for (String topicName : sendTopics) {
					TopicInfo ti = topics.get(topicName);
					assert ti != null;
					ConnectionInfo arrow = addArrow(newEdges, ci.getCell(), ti.getCell());
					ti.sendingComponents().put(ci.toString(), arrow);
				}
			}
			String[] receiveTopics = ci.receiveTopics();
			if (receiveTopics != null) {
				// receiving can use wildcards, so we need to go through all topics
				for (String topicName : topics.keySet()) {
					if (ci.canReceive(topicName)) {
						TopicInfo ti = topics.get(topicName);
						assert ti != null;
						ConnectionInfo arrow = addArrow(newEdges, ti.getCell(), ci.getCell());
						ti.receivingComponents().put(ci.toString(), arrow);
					}
				}
			}
		}
		graph.getGraphLayoutCache().insert(newEdges.toArray());
		if (edges != null) {
			edges.addAll(newEdges);
		} else {
			edges = newEdges;
		}

	}
	
	private ConnectionInfo addArrow(List<DefaultEdge> someEdges, DefaultGraphCell source, DefaultGraphCell target)
	{
		if (someEdges == null || source == null || target == null)
			throw new NullPointerException("Cannot add arrow for null parameters");
		DefaultEdge blackArrow = new DefaultEdge();
		blackArrow.setSource(source.getChildAt(0));
		blackArrow.setTarget(target.getChildAt(0));
		int arrow = GraphConstants.ARROW_CLASSIC;
		GraphConstants.setLineEnd(blackArrow.getAttributes(), arrow);
		GraphConstants.setEndFill(blackArrow.getAttributes(), true);
		GraphConstants.setSelectable(blackArrow.getAttributes(), false);
		
		ConnectionInfo arrowInfo = new ConnectionInfo();
		DefaultEdge blueArrow = new DefaultEdge(arrowInfo);
		blueArrow.setSource(source.getChildAt(0));
		blueArrow.setTarget(target.getChildAt(0));
		GraphConstants.setLineEnd(blueArrow.getAttributes(), arrow);
		GraphConstants.setEndFill(blueArrow.getAttributes(), true);
		GraphConstants.setLineColor(blueArrow.getAttributes(), new Color(150, 150, 255));
		GraphConstants.setSelectable(blueArrow.getAttributes(), false);
		someEdges.add(blueArrow);
		someEdges.add(blackArrow);
		arrowInfo.setCell(blueArrow);
		
		return arrowInfo;
	}
	
	
	private void layoutCells(Map<DefaultGraphCell, Map<Object,Object>> allChanges)
	{
		int numGroups = infoGroups.size();
		if (numGroups == 0) return;
		int maxX = frameSize.width;
		int maxY = frameSize.height-30;
		Rectangle2D.Double[][] coords = new Rectangle2D.Double[numGroups][];
		
		layoutLeftMidRight(maxX, maxY, numGroups, coords);
		//layoutHalfCircle(maxX, maxY, numGroups, coords);
		
		for (int i=0; i<numGroups; i++) {
			int numInGroup = infoGroups.get(i).size();
			for (int j=0; j<numInGroup; j++) {
				DefaultGraphCell cell = infoGroups.get(i).get(j).getCell();
				if (cell != null) {
					Map<Object,Object> attributes = allChanges.get(cell);
					if (attributes == null) {
						attributes = new HashMap<Object, Object>();
						allChanges.put(cell, attributes);
					}
					GraphConstants.setBounds(attributes, coords[i][j]);
				}
			}
		}
		
	}

	private void layoutLeftMidRight(int maxX, int maxY, int numGroups,
			Rectangle2D.Double[][] coords) {
		int numLeft = (int) Math.ceil(numGroups/3.);
		int numRight = numLeft;
		int numMid = numGroups - numLeft - numRight;
		int avgSpaceX = (maxX - (numMid+2) * componentWidth) / (numMid+2);
		int avgSpaceY = (2 * maxY - (numLeft+numRight+2) * componentHeight) 
			/ (numLeft+numRight);
		int leftX = avgSpaceX/2;
		int rightX = leftX + (numMid+1)*(avgSpaceX+componentWidth);
		int leftXMid = leftX + componentWidth + avgSpaceX;
		int stepsLeft = numLeft;
		if (numMid == numLeft) {
			stepsLeft = numLeft+1;
			avgSpaceX = (maxX - (numMid+1) * componentWidth) / (numMid+1);
			avgSpaceY = (2 * maxY - (numLeft+numRight+2) * componentHeight) 
				/ (numLeft+numRight+2);
			leftXMid = componentWidth/2 + avgSpaceX;
		}
		int midY = maxY + avgSpaceY/2 - stepsLeft*(avgSpaceY+componentHeight);
		int stepsRight = stepsLeft;
		for (int i=0; i<numLeft; i++) {
			int numInGroup = infoGroups.get(i).size();
			coords[i] = new Rectangle2D.Double[numInGroup];
			int avgSpaceXInGroup = (maxX/3-leftX-numInGroup*componentWidth)/numInGroup;
			for (int j=0; j<numInGroup; j++) {
				coords[i][j] = new Rectangle2D.Double(
					leftX + j * (componentWidth+avgSpaceXInGroup),
					maxY + avgSpaceY/2 - (i+1)*(avgSpaceY+componentHeight),
					componentWidth,
					componentHeight);
			}
		}
		for (int i=0; i<numMid; i++) {
			int numInGroup = infoGroups.get(numLeft+i).size();
			coords[numLeft+i] = new Rectangle2D.Double[numInGroup];
			int avgSpaceYInGroup = (maxY/3 - midY - numInGroup*componentHeight)/numInGroup;
			for (int j=0; j<numInGroup; j++) {
				coords[numLeft+i][j] = new Rectangle2D.Double(
					leftXMid+i*(avgSpaceX+componentWidth),
					midY+j * (componentHeight+avgSpaceYInGroup),
					componentWidth,
					componentHeight);
			}
		}
		for (int i=0; i<numRight; i++) {
			int numInGroup = infoGroups.get(numLeft+numMid+i).size();
			coords[numLeft+numMid+i] = new Rectangle2D.Double[numInGroup];
			int avgSpaceXInGroup = (maxX/3-leftX-numInGroup*componentWidth)/numInGroup;
			for (int j=0; j<numInGroup; j++) {
				coords[numLeft+numMid+i][j] = new Rectangle2D.Double(
					rightX- j * (componentWidth+avgSpaceXInGroup),
					midY+(i+stepsRight-numRight)*(avgSpaceY+componentHeight),
					componentWidth,
					componentHeight);
			}
		}
	}
	
	/**
	 * Arrange coordinates on a half circle.
	 * @param maxX
	 * @param maxY
	 * @param numGroups
	 * @param coords
	 */
	private void layoutHalfCircle(int maxX, int maxY, int numGroups,
			Rectangle2D.Double[][] coords)
	{
		double angleDelta = Math.PI/(numGroups-1);
		double radiusX = maxX*0.4;
		double radiusY = maxY* 0.8;
		double spanX = maxX*0.2;
		double spanY = maxY*0.3;
		Point2D center = new Point2D.Double(maxX/2, maxY - componentHeight/2);
		for (int i=0; i<infoGroups.size(); i++) {
			List<Info> group = infoGroups.get(i);
			double angle = i*angleDelta;
			int numInGroup = group.size();
			coords[i] = new Rectangle2D.Double[numInGroup];
			double outerRadiusX = numInGroup == 1 ? radiusX : radiusX + spanX/2;
			double outerRadiusY = numInGroup == 1 ? radiusY : radiusY + spanY/2;
			double deltaX = numInGroup == 1 ? 0 : spanX / (numInGroup-1);
			double deltaY = numInGroup == 1 ? 0 : spanY / (numInGroup-1);
			for (int j=0; j<numInGroup; j++) {
				Point2D compCenter = toLocation(angle, outerRadiusX-j*deltaX, outerRadiusY-j*deltaY, center);
				coords[i][j] = new Rectangle2D.Double(compCenter.getX() - componentWidth/2,
						compCenter.getY() - componentHeight/2,
						componentWidth, componentHeight);
			}
		}
	}
	
	Point2D toLocation(double angle, double radiusX, double radiusY, Point2D center)
	{
		double deltaX = -Math.cos(angle) * radiusX;
		double deltaY = Math.sin(angle) * radiusY;
		return new Point2D.Double(center.getX()+deltaX, center.getY()-deltaY);
	}
	
	public void run()
	{
		// Give the GUI some time to set up properly before sending edit requests
		try {
			Thread.sleep(1000);
		} catch (InterruptedException ie) {}
		while (true) {
			try {
				Thread.sleep(100);
			} catch (InterruptedException ie) {}
			redraw();
		}
	}
	
	
	public ComponentInfo getComponentInfo(String componentName)
	{
		for(ComponentInfo ci : sortedComponentList) {
			if (ci.toString().equals(componentName)) {
				return ci;
			}
		}
		return null;
	}

	public TopicInfo getTopicInfo(String topicName)
	{
		return topics.get(topicName);
	}
	
	
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		ComponentInfo[] cis = new ComponentInfo[] {
				new ComponentInfo("speech synthesis", 
						new String[] {"semaine.data.synthesis.plan"},
						new String[] {"semaine.data.synthesis.plan.speechtimings", "semaine.data.lowlevel.audio"},
						false, false),
				new ComponentInfo("audio features",
						null,
						null,
						//new String[] {"semaine.data.analysis.audio"},
						true, false),
				new ComponentInfo("ASR",
						new String[] {"semaine.data.analysis.audio"},
						new String[] {"semaine.data.state.user.emma"},
						false, false),
				new ComponentInfo("turn taking interpreter",
						new String[] {"semaine.data.state.user.behaviour", "semaine.data.state.user.emma"},
						new String[] {"semaine.data.state.user.behaviour"},
						false, false),
				new ComponentInfo("user utterance interpreter",
						new String[] {"semaine.data.state.user.behaviour", "semaine.data.state.user.emma", "semaine.data.state.user.intention"},
						new String[] {"semaine.data.state.user.intention", "semaine.data.state.agent.intention"},
						false, false),
				new ComponentInfo("agent utterance proposer",
						new String[] {"semaine.data.state.agent.intention"},
						new String[] {"semaine.data.action.candidate.function"},
						false, false),
				new ComponentInfo("emotion detection", 
						new String[] {"semaine.data.analysis.audio"},
						new String[] {"semaine.data.state.user.emma"},
						false, false),
				new ComponentInfo("mimicry action proposer",
						new String[] {"semaine.data.state.user.behaviour"},
						new String[] {"semaine.data.action.candidate.behaviour"},
						false, false),
				new ComponentInfo("backchannel action proposer",
						new String[] {"semaine.data.state.user.emma"},
						new String[] {"semaine.data.action.candidate.behaviour"},
						false, false),
				new ComponentInfo("action selection",
						new String[] {"semaine.data.action.candidate.>"},
						new String[] {"semaine.data.action.selected.function", "semaine.data.action.selected.behaviour"},
						false, false),
				new ComponentInfo("Greta player", 
						new String[] {"semaine.data.lowlevel.video", "semaine.data.lowlevel.audio"},
						null, 
						false, true),
				new ComponentInfo("FML2BML", 
						new String[] {"semaine.data.action.selected.function.speechpreprocessed"},
						new String[] {"semaine.data.synthesis.plan"},
						false, false),
				new ComponentInfo("speech preprocessing", 
						new String[] {"semaine.data.action.selected.function"},
						new String[] {"semaine.data.action.selected.function.speechpreprocessed"},
						false, false),
				new ComponentInfo("BML realiser", 
						new String[] {"semaine.data.synthesis.plan.speechtimings"},
						new String[] {"semaine.data.lowlevel.video"},
						false, false)
		};
		SystemMonitor mon = new SystemMonitor(cis);
		mon.start();
		for (int i=0; i<cis.length; i++) {
			try {
				Thread.sleep(100);
			} catch (InterruptedException ie) {}
			cis[i].setState(Component.State.ready);
		}
		ComponentInfo audioFeatures = mon.getComponentInfo("audio features");
		audioFeatures.setSendTopics("semaine.data.analysis.audio");
		try {
			Thread.sleep(3000);
		} catch (InterruptedException ie) {}
		//mon.addComponentInfo();
		TopicInfo ti = mon.topics.get("semaine.data.state.user.emma");
		for (int i=0; i<1000; i++) {
			ti.addMessage("Test message "+i, "emotion detection");
			try {
				Thread.sleep(1000);
			} catch (InterruptedException ie) {}
			
		}
	}

}
