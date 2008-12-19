/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.gui.monitor;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Frame;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.event.KeyEvent;
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
import java.util.Vector;
import java.util.Map.Entry;

import javax.jms.JMSException;
import javax.swing.BorderFactory;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JSplitPane;
import javax.swing.JTextPane;
import javax.swing.KeyStroke;
import javax.swing.border.EtchedBorder;
import javax.swing.border.TitledBorder;
import javax.swing.text.BadLocationException;

import org.apache.log4j.BasicConfigurator;
import org.apache.log4j.ConsoleAppender;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.apache.log4j.PatternLayout;
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
import eu.semaine.jms.JMSLogReader;

public class SystemMonitor extends Thread
{
	public static final String ALL_COMPONENTS = "all components";
	private JFrame frame;
	private JGraph graph;
	private JTextPane systemStatus;
	private JTextPane logTextPane;
	private Dimension frameSize = null;
	private static int componentWidth = 120;
	private static int componentHeight = 30;
	private boolean mustUpdateCells = true;
	
	private List<ComponentInfo> sortedComponentList;
	private JComboBox logComponentBox;
	private JComboBox logLevelBox;
	private boolean componentListChanged = false;
	private List<List<Info>> infoGroups;
	private Map<String, TopicInfo> topics;
	private List<DefaultGraphCell> cells;
	private List<DefaultEdge> edges;
	private JMSLogReader logReader;
	private String currentLogComponent;
	private String currentLogLevel;
	
	public SystemMonitor(ComponentInfo[] componentInfos)
	{
		// Sort components:
		if (componentInfos != null)
			sortedComponentList = new ArrayList<ComponentInfo>(Arrays.asList(componentInfos));
		else
			sortedComponentList = new ArrayList<ComponentInfo>();
		topics = new HashMap<String, TopicInfo>();
		cells = new ArrayList<DefaultGraphCell>();
		infoGroups = new LinkedList<List<Info>>();

	}

	private void setupGraphGUI() {
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
		
		frame = new JFrame("SEMAINE System Monitor");
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
		frame.setSize(new Dimension(screenSize.width*9/10,screenSize.height*9/10));
		frame.setExtendedState(frame.getExtendedState() | JFrame.MAXIMIZED_BOTH);

		// Menu
		//Create the menu bar.
		JMenuBar menuBar = new JMenuBar();
		JMenu fileMenu = new JMenu("File");
		fileMenu.setMnemonic(KeyEvent.VK_F);
		menuBar.add(fileMenu);
		JMenuItem exitItem = new JMenuItem("Exit", KeyEvent.VK_X);
		exitItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Q, ActionEvent.META_MASK));
		exitItem.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ae) {
				System.exit(0);
			}
		});
		fileMenu.add(exitItem);
		JMenu guiMenu = new JMenu("GUI");
		guiMenu.setMnemonic(KeyEvent.VK_G);
		guiMenu.getAccessibleContext().setAccessibleDescription("GUI menu");
		menuBar.add(guiMenu);
		JMenuItem resetItem = new JMenuItem("Reset GUI", KeyEvent.VK_R);
		resetItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_R, ActionEvent.META_MASK));
		resetItem.getAccessibleContext().setAccessibleDescription("Reset the GUI");
		resetItem.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ae) {
				setMustUpdateCells(true);
			}
		});
		guiMenu.add(resetItem);
		frame.setJMenuBar(menuBar);
		
		JPanel rightSide = new JPanel();
		rightSide.setLayout(new BorderLayout());
		systemStatus = new JTextPane();
		TitledBorder title = BorderFactory.createTitledBorder(
				BorderFactory.createEtchedBorder(EtchedBorder.LOWERED),
				"System status");
		title.setTitleJustification(TitledBorder.LEFT);
		systemStatus.setBorder(title);
		systemStatus.setPreferredSize(new Dimension(200, 200));
		systemStatus.setBackground(new Color(230, 230, 230));
		logTextPane = new JTextPane();
		JScrollPane logScrollPane = new JScrollPane(logTextPane);
		JPanel logPane = new JPanel();
		logPane.setLayout(new BorderLayout());
		JPanel logConfig = new JPanel();
		logConfig.setLayout(new BorderLayout());
		logComponentBox = new JComboBox();
		logComponentBox.setMinimumSize(new Dimension(10,10));// can get really small if needed
		updateLogComponents();
		logConfig.add(logComponentBox, BorderLayout.CENTER);
		logLevelBox = new JComboBox(new String[] {"DEBUG", "INFO", "WARN", "ERROR"});
		logLevelBox.setSelectedItem("INFO");
		logConfig.add(logLevelBox, BorderLayout.LINE_END);
		ItemListener logListener = new ItemListener() {
			public void itemStateChanged(ItemEvent ie) {
				setupLogReader();
			}
		};
		logLevelBox.addItemListener(logListener);
		logComponentBox.addItemListener(logListener);
		
		
		logPane.add(logConfig, BorderLayout.PAGE_START);
		logPane.add(logScrollPane, BorderLayout.CENTER);
		logPane.setBorder(BorderFactory.createTitledBorder(
				BorderFactory.createEmptyBorder(),
				"Log messages"));
		logPane.setBackground(systemStatus.getBackground());
		rightSide.add(systemStatus, BorderLayout.PAGE_START);
		rightSide.add(logPane, BorderLayout.CENTER);
		JSplitPane splitPane = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT,
		                           graph, rightSide);
		splitPane.setOneTouchExpandable(true);
		frame.getContentPane().add(splitPane);
		// On some systems, maximise frame will have worked, on others not, so let's try to get a 
		// reasonable estimate of our frame width:
		int dividerLocation;
		Dimension frameSize = frame.getSize();
		if (frameSize.width > 0) {
			dividerLocation = frameSize.width * 3/4;
		} else {
			dividerLocation = Toolkit.getDefaultToolkit().getScreenSize().width*3/4;
		}	
		splitPane.setDividerLocation(dividerLocation);

		// Set up log reader:
		System.setProperty("org.apache.commons.logging.Log", "org.apache.commons.logging.impl.Log4JLogger");
		// Configure log4j
		Logger.getRootLogger().setLevel(Level.DEBUG);
		Logger.getLogger("org.apache").setLevel(Level.INFO);
		PatternLayout layout = new PatternLayout("%-5p %-10c %m\n");
		BasicConfigurator.configure();
		Logger.getLogger("semaine.log").addAppender(new TextPaneAppender(layout, "log-appender", logTextPane));
		Logger.getLogger("semaine.log").setLevel(Level.DEBUG);
		setupLogReader();
		
		//updateCells();
				
		frame.setVisible(true);
		redraw();
	}
	
	public synchronized void addComponentInfo(ComponentInfo ci)
	{
		sortedComponentList.add(ci);
		componentListChanged = true;
	}
	
	private synchronized void updateLogComponents()
	{
		Object selected = logComponentBox.getSelectedItem();
		// avoid sending item changed messages upon remove:
		ItemListener[] ils = logComponentBox.getItemListeners();
		for (ItemListener il : ils) {
			logComponentBox.removeItemListener(il);
		}
		logComponentBox.removeAllItems();
		logComponentBox.addItem(ALL_COMPONENTS);
		for (ComponentInfo ci : sortedComponentList) {
			logComponentBox.addItem(ci.toString());
		}
		// Now, add the item listeners back:
		for (ItemListener il : ils) {
			logComponentBox.addItemListener(il);
		}
		if (selected != null)
			logComponentBox.setSelectedItem(selected);
		else
			logComponentBox.setSelectedItem(ALL_COMPONENTS);
	}
	
	private synchronized void setupLogReader()
	{
		String component = (String) logComponentBox.getSelectedItem();
		if (component == null || component.equals(ALL_COMPONENTS))
			component = "*";
		String level = (String) logLevelBox.getSelectedItem();
		if (level == null) level = "DEBUG";
		if (component.equals(currentLogComponent) && level.equals(currentLogLevel)) {
			return; // nothing changed
		}
		try {
			// empty the log text pane
			logTextPane.getDocument().remove(0, logTextPane.getDocument().getLength());
		} catch (BadLocationException ble) {
			ble.printStackTrace();
		}
		System.out.println("Looking for log messages in topic 'semaine.log."+component+".*', showing log level "+level+" and higher");
		String loggerName = (component.equals("*")) ? "semaine.log" : "semaine.log."+component;
		Logger.getLogger(loggerName).setLevel(Level.toLevel(level));
		try {
			if (logReader != null) {
				logReader.getConnection().stop();
			}
			logReader = new JMSLogReader("semaine.log."+component+".*");
			currentLogComponent = component;
			currentLogLevel = level;
		} catch (JMSException je) {
			System.err.println("Problem with log reader:");
			je.printStackTrace();
		}
	}
	
	private synchronized void setMustUpdateCells(boolean value)
	{
		mustUpdateCells = value;
	}
	
	private synchronized void redraw()
	{
		assert cells != null;
		Map<DefaultGraphCell, Map<Object,Object>> allChanges = new HashMap<DefaultGraphCell, Map<Object,Object>>();
		boolean mustLayoutCells = false;
		Dimension newFrameSize = graph.getSize();
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
			//if (edges != null) {
				//graph.getGraphLayoutCache().remove(edges.toArray());
				//edges = null;
			//}
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
				//createAllArrows();
			}
		}
		mustUpdateCells = false;
		
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
		updateLogComponents();
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
		if (newCells.size() > 0) {
			System.out.println("Adding new cells:");
			for (DefaultGraphCell c : newCells) {
				System.out.println("    "+c.toString());
			}
		}
		graph.getGraphLayoutCache().insert(newCells.toArray());
		createAllArrows();

	}
	
	private void createAllArrows()
	{
		List<DefaultEdge> newEdges = new ArrayList<DefaultEdge>();
		// Create cells for arrows
		for (ComponentInfo ci : sortedComponentList) {
			String[] sendTopics = ci.sendTopics();
			if (ci.getCell() == null) continue;
			if (sendTopics != null) {
				for (String topicName : sendTopics) {
					TopicInfo ti = topics.get(topicName);
					if (ti != null) {
						assert ti.getCell() != null;
						ConnectionInfo arrow = addArrow(newEdges, ci.getCell(), ti.getCell());
						ti.sendingComponents().put(ci.toString(), arrow);
					}
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
		Point2D.Double[][] coords = new Point2D.Double[numGroups][];
		
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
					Rectangle2D.Double bounds = new Rectangle2D.Double(
							coords[i][j].getX()-componentWidth/2,
							coords[i][j].getY()-componentHeight/2,
							componentWidth, componentHeight);
					GraphConstants.setBounds(attributes, bounds);
				}
			}
		}
		
	}

	/**
	 * A three-section layout:
	 * <pre>
	 * L\M   M M   M/R
	 *  L\M  M M  M/R
	 *   L\M M M M/R
	 * 
	 * L L L     R R R
	 * 
	 * L L L     R R R
	 * </pre>
	 * @param maxX
	 * @param maxY
	 * @param numGroups
	 * @param coords
	 */
	private void layoutLeftMidRight(int maxX, int maxY, int numGroups,
			Point2D.Double[][] coords)
	{
		int numLeft = (int) Math.ceil(numGroups/3.);
		int numRight = numLeft;
		int numMid = numGroups - numLeft - numRight;

		double SPACE = 50;
		// Coordinates of the middle of the respective section:
		double leftCenterX = 0.15 * maxX;
		double rightCenterX = 0.85 * maxX;
		double midCenterY = 0.15 * maxY;
		double leftFirstY = maxY - SPACE; // the center point of the lowest (first) component
		double leftLastY = midCenterY + 2*SPACE; // the center point of the highest left component
		double midFirstX = leftCenterX + 2*SPACE; // the center point of the first mid component
		double midLastX = rightCenterX - 2*SPACE; // the center point of the last mid component
		double rightLastY = maxY - SPACE;
		double rightFirstY = numRight > 1 ? midCenterY + 2*SPACE: rightLastY;
		// width of the respective section:
		double spanX = 0.2 * maxX;
		double spanY = 0.2 * maxY;
		
		double leftStepY = numLeft > 1 ? (leftFirstY-leftLastY)/(numLeft-1) : 0;
		for (int i=0; i<numLeft; i++) {
			int numInGroup = infoGroups.get(i).size();
			coords[i] = new Point2D.Double[numInGroup];
			double centerY = leftFirstY - i * leftStepY;
			double outerY = centerY;
			double innerY = centerY;
			if (centerY < midCenterY+spanY/2+SPACE) { // danger of overlap
				innerY += SPACE;
				outerY = centerY - (innerY-centerY);
			}
			double outerX = numInGroup == 1 ? leftCenterX : leftCenterX - spanX / 2;
			double innerX = numInGroup == 1 ? leftCenterX : leftCenterX + spanX / 2;
			double stepX = numInGroup == 1 ? 0 : (innerX-outerX)/(numInGroup-1);
			double stepY = numInGroup == 1 ? 0 : (innerY-outerY)/(numInGroup-1);
			for (int j=0; j<numInGroup; j++) {
				coords[i][j] = new Point2D.Double(
						outerX + j*stepX,
						outerY + j*stepY);
			}
		}
		double midStepX = numMid > 1 ? (midLastX-midFirstX)/(numMid-1) : 0;
		for (int i=0; i<numMid; i++) {
			int numInGroup = infoGroups.get(numLeft+i).size();
			coords[numLeft+i] = new Point2D.Double[numInGroup];
			double centerX = midFirstX + i * midStepX;
			double outerX = centerX;
			double innerX = centerX;
			if ( centerX < leftCenterX+spanX/2+SPACE) { // danger of overlap left
				innerX += SPACE;
				outerX = centerX - (innerX-centerX);
			} else if (centerX > rightCenterX-spanX/2-SPACE) { // danger of overlap right
				innerX -= SPACE;
				outerX = centerX + (centerX-innerX);
			}
			double outerY = numInGroup == 1 ? midCenterY : midCenterY - spanY / 2;
			double innerY = numInGroup == 1 ? midCenterY : midCenterY + spanY / 2;
			double stepY = numInGroup == 1 ? 0 : (innerY-outerY)/(numInGroup-1);
			double stepX = numInGroup == 1 ? 0 : (innerX-outerX)/(numInGroup-1);
			for (int j=0; j<numInGroup; j++) {
				coords[numLeft+i][j] = new Point2D.Double(
						outerX + j*stepX,
						outerY + j*stepY);
			}
		}
		double rightStepY = numRight > 1 ? (rightLastY - rightFirstY)/(numRight-1) : 0;
		for (int i=0; i<numRight; i++) {
			int numInGroup = infoGroups.get(numLeft+numMid+i).size();
			coords[numLeft+numMid+i] = new Point2D.Double[numInGroup];
			double centerY = rightFirstY + i * rightStepY;
			double outerY = centerY;
			double innerY = centerY;
			if (centerY < midCenterY+spanY/2+SPACE) { // danger of overlap
				innerY += SPACE;
				outerY = centerY - (innerY-centerY);
			}
			double outerX = numInGroup == 1 ? rightCenterX : rightCenterX + spanX / 2;
			double innerX = numInGroup == 1 ? rightCenterX : rightCenterX - spanX / 2;
			double stepX = numInGroup == 1 ? 0 : (innerX-outerX)/(numInGroup-1); // negative in this case
			double stepY = numInGroup == 1 ? 0 : (innerY-outerY)/(numInGroup-1);
			for (int j=0; j<numInGroup; j++) {
				coords[numLeft+numMid+i][j] = new Point2D.Double(
						outerX + j*stepX,
						outerY + j*stepY);
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
		setupGraphGUI();
		// Give the GUI some time to set up properly before sending edit requests
		//try {
			//Thread.sleep(1000);
		//} catch (InterruptedException ie) {}
		while (true) {
			try {
				Thread.sleep(100);
			} catch (InterruptedException ie) {}
			try {
				redraw();
			} catch (Exception e) {
				e.printStackTrace();
			}
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
	
	
	public void setSystemStatus(String text)
	{
		if (systemStatus != null)
			systemStatus.setText(text);
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
		//mon.start();
		mon.setupGraphGUI();
		for (int i=0; i<cis.length; i++) {
			try {
				Thread.sleep(100);
			} catch (InterruptedException ie) {}
			cis[i].setState(Component.State.ready);
			mon.redraw();
		}
		ComponentInfo audioFeatures = mon.getComponentInfo("audio features");
		audioFeatures.setSendTopics("semaine.data.analysis.audio");
		mon.redraw();
		try {
			Thread.sleep(3000);
		} catch (InterruptedException ie) {}
		//mon.addComponentInfo();
		TopicInfo ti = mon.topics.get("semaine.data.state.user.emma");
		for (int i=0; i<1000; i++) {
			ti.addMessage("Test message "+i, "emotion detection");
			mon.redraw();
			try {
				Thread.sleep(1000);
			} catch (InterruptedException ie) {}
			
		}
	}

}
