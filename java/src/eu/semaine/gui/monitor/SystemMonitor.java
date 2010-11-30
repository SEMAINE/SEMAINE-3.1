/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.gui.monitor;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
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
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;

import javax.jms.JMSException;
import javax.swing.BorderFactory;
import javax.swing.JCheckBoxMenuItem;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JSplitPane;
import javax.swing.JTextPane;
import javax.swing.KeyStroke;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;
import javax.swing.border.EtchedBorder;
import javax.swing.border.TitledBorder;
import javax.swing.text.BadLocationException;

import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.apache.log4j.PatternLayout;
import org.jgraph.JGraph;
import org.jgraph.graph.DefaultEdge;
import org.jgraph.graph.DefaultGraphCell;
import org.jgraph.graph.DefaultGraphModel;
import org.jgraph.graph.GraphConstants;
import org.jgraph.graph.GraphLayoutCache;
import org.jgraph.graph.GraphModel;

import eu.semaine.components.Component;
import eu.semaine.jms.JMSLogReader;
import eu.semaine.util.FuzzySort;
import eu.semaine.util.FuzzySort.FuzzySortable;
import eu.semaine.util.FuzzySort.FuzzySortableRelation;

public class SystemMonitor extends Thread {
	//// Static stuff
	public static final String ALL_COMPONENTS = "all components";
	private static int componentWidth = 120;
	private static int componentHeight = 30;
	
	/**
	 * Get the width of a component
	 * @return the width of a component, in pixels
	 */
	public static int getComponentWidth() {
		return componentWidth;
	}
	
	/**
	 * Get the font used to label the components and topics
	 * @return a font object as will be used for labelling the vertices.
	 */
	public static Font getLabelFont() {
		Font defaultFont = UIManager.getDefaults().getFont("Label.font");
		Font font = defaultFont.deriveFont(11f);
		return font;
	}

	// Non-static stuff
	private JFrame frame;
	private JSplitPane splitPane;
	private JGraph graph;
	private boolean showingGraph = false;

	private JTextPane systemStatus;
	private JTextPane logTextPane;
	private Dimension frameSize = null;
	
	
	private boolean mustUpdateCells = true;

	private List<ComponentInfo> sortedComponentList;
	private JComboBox logComponentBox;
	private JComboBox logLevelBox;
	private boolean componentListChanged = false;
	private List<List<Info>> infoGroups;
	private List<TopicInfo> callbackTopics;
	private Map<String, TopicInfo> topics;
	private List<DefaultGraphCell> cells;
	private List<DefaultEdge> edges;
	private List<DefaultEdge> edgesWithUpdates;
	private JMSLogReader logReader;
	private String currentLogComponent;
	private String currentLogLevel;
	private final Set<String> topicsToHide;

	public SystemMonitor(ComponentInfo[] componentInfos,
			Set<String> topicsToHide) {
		// Sort components:
		if (componentInfos != null)
			sortedComponentList = new ArrayList<ComponentInfo>(Arrays
					.asList(componentInfos));
		else
			sortedComponentList = new ArrayList<ComponentInfo>();
		topics = new HashMap<String, TopicInfo>();
		cells = new ArrayList<DefaultGraphCell>();
		infoGroups = new LinkedList<List<Info>>();
		callbackTopics = new LinkedList<TopicInfo>();
		this.topicsToHide = topicsToHide;
	}

	private synchronized void setupGUI() {

		frame = new JFrame("SEMAINE System Monitor");
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
		frame.setSize(new Dimension(screenSize.width * 9 / 10, screenSize.height * 9 / 10));
		frame.setExtendedState(frame.getExtendedState() | JFrame.MAXIMIZED_BOTH);

		// Menu
		// Use CTRL key on most platforms
		int magicKey = ActionEvent.CTRL_MASK;
		// but apple key on the Mac:
		if (System.getProperty("os.name").startsWith("Mac")) {
			magicKey = ActionEvent.META_MASK;
		}

		// Create the menu bar.
		JMenuBar menuBar = new JMenuBar();
		JMenu fileMenu = new JMenu("File");
		fileMenu.setMnemonic(KeyEvent.VK_F);
		menuBar.add(fileMenu);
		JMenuItem exitItem = new JMenuItem("Exit", KeyEvent.VK_X);
		exitItem
				.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Q, magicKey));
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
		final JCheckBoxMenuItem toggleGraphItem = new JCheckBoxMenuItem(
				"Show message flow graph", true);
		toggleGraphItem.setMnemonic(KeyEvent.VK_M);
		toggleGraphItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_M,
				magicKey));
		toggleGraphItem.getAccessibleContext().setAccessibleDescription(
				"Toggle message flow graph");
		toggleGraphItem.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ae) {
				SwingUtilities.invokeLater(new Runnable() {
					public void run() {
						if (toggleGraphItem.isSelected()) {
							setupGraph();
							redraw();
						} else {
							hideGraph();
						}
					}
				});
			}
		});
		guiMenu.add(toggleGraphItem);
		frame.setJMenuBar(menuBar);

		JPanel rightSide = new JPanel();
		rightSide.setLayout(new BorderLayout());
		systemStatus = new JTextPane();
		TitledBorder title = BorderFactory.createTitledBorder(BorderFactory
				.createEtchedBorder(EtchedBorder.LOWERED), "System status");
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
		logComponentBox.setMinimumSize(new Dimension(10, 10));// can get really
																// small if
																// needed
		logComponentBox.setMaximumRowCount(20);
		updateLogComponents();
		logConfig.add(logComponentBox, BorderLayout.CENTER);
		logLevelBox = new JComboBox(new String[] { "DEBUG", "INFO", "WARN",
				"ERROR" });
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
		logPane.setBorder(BorderFactory.createTitledBorder(BorderFactory
				.createEmptyBorder(), "Log messages"));
		logPane.setBackground(systemStatus.getBackground());
		rightSide.add(systemStatus, BorderLayout.PAGE_START);
		rightSide.add(logPane, BorderLayout.CENTER);
		splitPane = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, null, rightSide);
		splitPane.setOneTouchExpandable(true);
		frame.getContentPane().add(splitPane);

		// Set up log reader:
		PatternLayout layout = new PatternLayout("%-5p %-10c %m\n");
		Logger.getLogger("semaine.log").addAppender(new TextPaneAppender(layout, "log-appender", logTextPane));
		Logger.getLogger("semaine.log").setLevel(Level.DEBUG);
		setupLogReader();

		// updateCells();
		setupGraph();

		frame.setVisible(true);
		redraw();
	}

	private synchronized void setupGraph() {
		if (graph == null) {
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
		}

		splitPane.setLeftComponent(graph);
		// On some systems, maximise frame will have worked, on others not, so
		// let's try to get a
		// reasonable estimate of our frame width:
		int dividerLocation;
		Dimension frameSize = frame.getSize();
		if (frameSize.width > 0) {
			dividerLocation = frameSize.width * 3 / 4;
		} else {
			dividerLocation = Toolkit.getDefaultToolkit().getScreenSize().width * 3 / 4;
		}
		splitPane.setDividerLocation(dividerLocation);

		showingGraph = true;
	}

	private synchronized void hideGraph() {
		showingGraph = false;
		splitPane.setLeftComponent(null);
	}

	private synchronized boolean isShowingGraph() {
		return showingGraph;
	}

	public synchronized void addComponentInfo(ComponentInfo ci) {
		sortedComponentList.add(ci);
		componentListChanged = true;
	}

	private synchronized void updateLogComponents() {
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

	private synchronized void setupLogReader() {
		String component = (String) logComponentBox.getSelectedItem();
		if (component == null || component.equals(ALL_COMPONENTS))
			component = "*";
		String level = (String) logLevelBox.getSelectedItem();
		if (level == null)
			level = "DEBUG";
		if (component.equals(currentLogComponent)
				&& level.equals(currentLogLevel)) {
			return; // nothing changed
		}

		// empty the log text pane
		SwingUtilities.invokeLater(new Runnable() {
			@Override public void run() {
				try {
					logTextPane.getDocument().remove(0,	logTextPane.getDocument().getLength());
				} catch (BadLocationException ble) {
					ble.printStackTrace();
				}
			}
		});
			
//		System.out.println("Looking for log messages in topic 'semaine.log."
//						+ component + ".*', showing log level " + level
//						+ " and higher");
		String loggerName = (component.equals("*")) ? "semaine.log" : "semaine.log." + component;
		Logger.getLogger(loggerName).setLevel(Level.toLevel(level));
		try {
			if (logReader != null) {
				logReader.stopLogging();
			}
			logReader = new JMSLogReader("semaine.log." + component + ".*");
			currentLogComponent = component;
			currentLogLevel = level;
		} catch (JMSException je) {
			System.err.println("Problem with log reader:");
			je.printStackTrace();
		}
	}

	private synchronized void redraw() {
		if (!isShowingGraph()) {
			return;
		}
		assert cells != null;
		final Map<DefaultGraphCell, Map<Object, Object>> allChanges = new HashMap<DefaultGraphCell, Map<Object, Object>>();
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
				Map<Object, Object> attributes = allChanges.get(cell);
				if (attributes == null) {
					attributes = new HashMap<Object, Object>();
					allChanges.put(cell, attributes);
				}
				if (inf instanceof ComponentInfo) {
					ComponentInfo compInf = (ComponentInfo) inf;
					GraphConstants
							.setBackground(attributes, compInf.getColor());
					if (compInf.topicsChanged()) {
						mustUpdateCells = true;
						mustLayoutCells = true;
					}
				} else if (inf instanceof TopicInfo) {
					TopicInfo topicInf = (TopicInfo) inf;
					GraphConstants.setBackground(attributes, topicInf
							.getColor());
				}
				inf.setChanged(false);
			}
		}
		if (mustUpdateCells) {
			updateCells();
		}
		if (mustLayoutCells) {
			// if (edges != null) {
			// graph.getGraphLayoutCache().remove(edges.toArray());
			// edges = null;
			// }
			layoutCells(allChanges);
		}
		if (edgesWithUpdates != null) {
			for (DefaultEdge edge : edgesWithUpdates) {
				Object userObject = edge.getUserObject();
				if (userObject == null)
					continue;
				assert userObject instanceof ConnectionInfo;
				ConnectionInfo connInf = (ConnectionInfo) userObject;
				if (connInf.needsUpdate()) {
					Map<Object, Object> attributes = allChanges.get(edge);
					if (attributes == null) {
						attributes = new HashMap<Object, Object>();
						allChanges.put(edge, attributes);
					}
					GraphConstants.setLineWidth(attributes, connInf
							.getLineWidth());
					connInf.setChanged(false);
				}
			}
		}
		if (!allChanges.isEmpty()) {
			SwingUtilities.invokeLater(new Runnable() {
				public void run() {
					graph.getGraphLayoutCache().edit(allChanges);
				}
			});

			if (mustLayoutCells) {
				// createAllArrows();
			}
		}
		mustUpdateCells = false;

	}

	private synchronized void updateCells() {
		assert cells != null;
		if (sortedComponentList.isEmpty())
			return;
		// each time we get here, we expect to know about new topics,
		// so we need to sort again the components:

		// Sort by counting comparator:
		/*
		 * Comparator<ComponentInfo> ciComparator = new
		 * ComponentInfo.CountingComparator( new
		 * HashSet<ComponentInfo>(sortedComponentList));
		 * Collections.sort(sortedComponentList, ciComparator);
		 * updateLogComponents(); // Group components: infoGroups.clear();
		 * callbackTopics.clear(); List<Info> currentGroup = new
		 * LinkedList<Info>(); currentGroup.add(sortedComponentList.get(0));
		 * infoGroups.add(currentGroup); for (int i=1,
		 * max=sortedComponentList.size(); i<max; i++) { assert currentGroup !=
		 * null; assert !currentGroup.isEmpty(); if
		 * (ciComparator.compare((ComponentInfo)currentGroup.get(0),
		 * sortedComponentList.get(i))==0) { // equal
		 * currentGroup.add(sortedComponentList.get(i)); } else { // not equal
		 * currentGroup = new LinkedList<Info>();
		 * currentGroup.add(sortedComponentList.get(i));
		 * infoGroups.add(currentGroup); } } addTopicsToInfoGroups();
		 */

		// Sort components by fuzzy sort algorithm:
		List<List<Info>> compGroups = sortComponents(sortedComponentList);
		updateLogComponents();
		infoGroups.clear();
		callbackTopics.clear();
		infoGroups.addAll(compGroups);
		addTopicsToInfoGroups();

		// Create cells for components
		final List<DefaultGraphCell> newCells = new ArrayList<DefaultGraphCell>();
		for (ComponentInfo ci : sortedComponentList) {
			if (ci.getCell() != null)
				continue;
			DefaultGraphCell cell = new DefaultGraphCell(ci);
			GraphConstants.setOpaque(cell.getAttributes(), true);
			GraphConstants.setBackground(cell.getAttributes(), ci.getColor());
			GraphConstants.setBorderColor(cell.getAttributes(), Color.black);
			GraphConstants.setFont(cell.getAttributes(), getLabelFont());
			cell.addPort();
			newCells.add(cell);
			ci.setCell(cell);
		}
		// Create cells for topics
		for (TopicInfo ti : topics.values()) {
			if (ti.getCell() != null)
				continue;
			DefaultGraphCell cell = new DefaultGraphCell(ti);
			GraphConstants.setBorder(cell.getAttributes(), BorderFactory.createRaisedBevelBorder());
			GraphConstants.setBackground(cell.getAttributes(), ti.getColor());
			GraphConstants.setOpaque(cell.getAttributes(), true);
			GraphConstants.setFont(cell.getAttributes(), getLabelFont());
			cell.addPort();
			newCells.add(cell);
			ti.setCell(cell);
		}
		cells.addAll(newCells);
//		if (newCells.size() > 0) {
//			System.out.println("Adding new cells:");
//			for (DefaultGraphCell c : newCells) {
//				System.out.println("    " + c.toString());
//			}
//		}
		SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				graph.getGraphLayoutCache().insert(newCells.toArray());
			}
		});
		createAllArrows();

	}

	/**
	 * 
	 */
	private void addTopicsToInfoGroups() {
		// add Topics to infoGroups
		Set<TopicInfo> topicsInGraph = new HashSet<TopicInfo>();
		for (int i = 0; i < infoGroups.size(); i++) {
			if (!(infoGroups.get(i).get(0) instanceof ComponentInfo))
				continue;
			List<Info> groupTopics = new LinkedList<Info>();
			for (Info inf : infoGroups.get(i)) {
				assert inf instanceof ComponentInfo : "Only component infos expected in info groups at this stage!";
				ComponentInfo ci = (ComponentInfo) inf;
				String[] sendTopics = ci.sendTopics();
				if (sendTopics == null)
					continue;
				for (String topicName : sendTopics) {
					if (topicsToHide.contains(topicName)) {
						continue;
					}
					TopicInfo ti = topics.get(topicName);
					if (ti == null) {
						// a new topic
						ti = ci.getTopicInfo(topicName);
						topics.put(topicName, ti);
					}
					if (!topicsInGraph.contains(ti)) {
						topicsInGraph.add(ti);
						switch (ti.getType()) {
						case Data:
							groupTopics.add(ti);
							break;
						case Callback:
							callbackTopics.add(ti);
							break;
						default:
							// shouldn't occur
						}
					}
				}
			}
			if (!groupTopics.isEmpty()) {
				infoGroups.add(i + 1, groupTopics);
				i++; // skip the newly added group
			}
		}
	}

	/**
	 * Sort the given list of component infos into an ordered list of groups of
	 * components, based on the initial/final status of components and on their
	 * send/receive topics.
	 * 
	 * @param comps
	 *            the list of topics to sort. Must not be null and must not be
	 *            empty. This will also be sorted in place, to reflect the
	 *            iterator of iterators of the return value.
	 * @return a non-empty list of non-empty sets of component infos. the list
	 *         is ordered as best we can from input to output components.
	 */
	private synchronized List<List<Info>> sortComponents(
			List<ComponentInfo> comps) {
		assert comps != null;
		assert !comps.isEmpty();
		// Strategy: for each component, create a FuzzySortable; for any direct
		// connections of components, create a relation for the FuzzySort
		// algorithm.
		Map<ComponentInfo, FuzzySortable> items = new HashMap<ComponentInfo, FuzzySortable>();
		Set<FuzzySortableRelation> relations = new HashSet<FuzzySortableRelation>();
		for (ComponentInfo ci : comps) {
			FuzzySortable f = new FuzzySortable(ci, ci.isInput(), ci.isOutput());
			items.put(ci, f);
		}

		for (ComponentInfo a : comps) {
			for (ComponentInfo b : comps) {
				if (a == b) {
					continue;
				}
				if (a.sendTopics() == null) {
					continue;
				}
				boolean bFollowsA = false;
				for (String sendTopic : a.sendTopics()) {
					// ignore it if it's not a data topic or if user requested
					// to hide or ignore it:
					if (!a.isDataTopic(sendTopic)
							|| ComponentInfo
									.isIgnoreTopicWhenSorting(sendTopic)) {
						continue;
					}
					if (b.canReceive(sendTopic)) {
						bFollowsA = true;
						break;
					}
				}
				if (bFollowsA) {
					relations.add(new FuzzySortableRelation(items.get(a), items
							.get(b)));
				}
			}
		}
//		System.out.println("For " + comps.size() + " components, registered "
//				+ relations.size() + " ordering relations");
		List<List<Info>> result = new ArrayList<List<Info>>();
		if (relations.isEmpty()) { // no relations? all in one!
			List<Info> allInOne = new ArrayList<Info>(comps.size());
			allInOne.addAll(comps);
		} else { // can do a meaningful sort
			List<Set<FuzzySortable>> sortedOrder = FuzzySort.sort(relations,
					true);
			for (Set<FuzzySortable> sfs : sortedOrder) {
				List<Info> sci = new ArrayList<Info>(sfs.size());
				for (FuzzySortable fs : sfs) {
					sci.add((ComponentInfo) fs.getPayload());
				}
				result.add(sci);
			}
			// Only if we actually sorted, update comps:
			// There may be components in comps which are not part of any
			// relation -- these will not be in result,
			// so careful with comps.clear();
			for (List<Info> l : result) {
				for (Info inf : l) {
					ComponentInfo ci = (ComponentInfo) inf;
					comps.remove(ci);
					comps.add(ci);
				}
			}
		}
		return result;
	}

	private synchronized void createAllArrows() {
		final List<DefaultEdge> constantEdges = new ArrayList<DefaultEdge>();
		final List<DefaultEdge> updateableEdges = new ArrayList<DefaultEdge>();
		// Create cells for arrows
		for (ComponentInfo ci : sortedComponentList) {
			String[] sendTopics = ci.sendTopics();
			if (ci.getCell() == null)
				continue;
			if (sendTopics != null) {
				for (String topicName : sendTopics) {
					if (topicsToHide.contains(topicName)) {
						continue;
					}
					TopicInfo ti = topics.get(topicName);
					if (ti != null) {
						assert ti.getCell() != null;
						ConnectionInfo arrow = addArrow(constantEdges,
								updateableEdges, ci.getCell(), ti.getCell());
						ti.sendingComponents().put(ci.toString(), arrow);
					}
				}
			}
			String[] receiveTopics = ci.receiveTopics();
			if (receiveTopics != null) {
				// receiving can use wildcards, so we need to go through all
				// topics
				for (String topicName : topics.keySet()) {
					if (topicsToHide.contains(topicName)) {
						continue;
					}
					if (ci.canReceive(topicName)) {
						TopicInfo ti = topics.get(topicName);
						assert ti != null;
						ConnectionInfo arrow = addArrow(constantEdges,
								updateableEdges, ti.getCell(), ci.getCell());
						ti.receivingComponents().put(ci.toString(), arrow);
					}
				}
			}
		}
		final Object[] removeEdges = edges != null ? edges.toArray() : null;
		// Edges to render:
		if (edges != null) {
			edges.clear();
		} else {
			edges = new ArrayList<DefaultEdge>(constantEdges.size()
					+ updateableEdges.size());
		}
		edges.addAll(updateableEdges);
		edges.addAll(constantEdges);

		// Only some of the edges may have updates:
		if (edgesWithUpdates != null) {
			edgesWithUpdates.clear();
		} else {
			edgesWithUpdates = new ArrayList<DefaultEdge>(updateableEdges
					.size());
		}
		edgesWithUpdates.addAll(updateableEdges);

		SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				if (removeEdges != null) {
					graph.getGraphLayoutCache().remove(removeEdges);
				}
				graph.getGraphLayoutCache().insert(updateableEdges.toArray());
				graph.getGraphLayoutCache().insert(constantEdges.toArray());
			}
		});

	}

	private ConnectionInfo addArrow(List<DefaultEdge> constantEdges,
			List<DefaultEdge> updateableEdges, DefaultGraphCell source,
			DefaultGraphCell target) {
		assert constantEdges != null;
		assert updateableEdges != null;
		assert source != null;
		assert target != null;
		DefaultEdge blackArrow = new DefaultEdge();
		blackArrow.setSource(source.getChildAt(0));
		blackArrow.setTarget(target.getChildAt(0));
		int arrow = GraphConstants.ARROW_CLASSIC;
		GraphConstants.setLineEnd(blackArrow.getAttributes(), arrow);
		GraphConstants.setEndFill(blackArrow.getAttributes(), true);
		GraphConstants.setSelectable(blackArrow.getAttributes(), false);
		constantEdges.add(blackArrow);

		ConnectionInfo arrowInfo = new ConnectionInfo();
		DefaultEdge blueArrow = new DefaultEdge(arrowInfo);
		blueArrow.setSource(source.getChildAt(0));
		blueArrow.setTarget(target.getChildAt(0));
		GraphConstants.setLineEnd(blueArrow.getAttributes(), arrow);
		GraphConstants.setEndFill(blueArrow.getAttributes(), true);
		GraphConstants.setLineColor(blueArrow.getAttributes(), new Color(150,
				150, 255));
		GraphConstants.setSelectable(blueArrow.getAttributes(), false);
		updateableEdges.add(blueArrow);
		arrowInfo.setCell(blueArrow);

		return arrowInfo;
	}

	private void layoutCells(
			Map<DefaultGraphCell, Map<Object, Object>> allChanges) {
		int numGroups = infoGroups.size();
		if (numGroups == 0) {
			return;
		}
		int maxX = frameSize.width;
		int maxY = frameSize.height - 30;
		Point2D.Double[][] coords = new Point2D.Double[numGroups][];

		layoutLeftMidRight(maxX, maxY, numGroups, coords);
		// layoutHalfCircle(maxX, maxY, numGroups, coords);

		for (int i = 0; i < numGroups; i++) {
			int numInGroup = infoGroups.get(i).size();
			for (int j = 0; j < numInGroup; j++) {
				DefaultGraphCell cell = infoGroups.get(i).get(j).getCell();
				if (cell != null) {
					Map<Object, Object> attributes = allChanges.get(cell);
					if (attributes == null) {
						attributes = new HashMap<Object, Object>();
						allChanges.put(cell, attributes);
					}
					Rectangle2D.Double bounds = new Rectangle2D.Double(
							coords[i][j].getX() - componentWidth / 2.,
							coords[i][j].getY() - componentHeight / 2.,
							componentWidth, componentHeight);
					GraphConstants.setBounds(attributes, bounds);
				}
			}
		}

		int numCallbackTopics = callbackTopics.size();
		if (numCallbackTopics == 0) {
			return;
		}
		Point2D.Double[] cbCoords = new Point2D.Double[numCallbackTopics];
		layoutCallbackTopics(maxX, maxY, numCallbackTopics, cbCoords);
		for (int i = 0; i < numCallbackTopics; i++) {
			DefaultGraphCell cell = callbackTopics.get(i).getCell();
			if (cell != null) {
				Map<Object, Object> attributes = allChanges.get(cell);
				if (attributes == null) {
					attributes = new HashMap<Object, Object>();
					allChanges.put(cell, attributes);
				}
				Rectangle2D.Double bounds = new Rectangle2D.Double(cbCoords[i]
						.getX()
						- componentWidth / 2., cbCoords[i].getY()
						- componentHeight / 2., componentWidth, componentHeight);
				GraphConstants.setBounds(attributes, bounds);
			}
		}
	}

	/**
	 * A three-section layout:
	 * 
	 * <pre>
	 * L\M   M M   M/R
	 *  L\M  M M  M/R
	 *   L\M M M M/R
	 * 
	 * L L L     R R R
	 * 
	 * L L L     R R R
	 * </pre>
	 * 
	 * @param maxX
	 * @param maxY
	 * @param numGroups
	 * @param coords
	 */
	private void layoutLeftMidRight(int maxX, int maxY, int numGroups,
			Point2D.Double[][] coords) {
		int numLeft = (int) Math.ceil(numGroups / 3.);
		int numRight = numLeft;
		int numMid = numGroups - numLeft - numRight;

		final double SPACE = 50;
		// Coordinates of the middle of the respective section:
		double leftCenterX = 0.15 * maxX;
		double rightCenterX = 0.85 * maxX;
		double midCenterY = 0.15 * maxY;
		double leftFirstY = maxY - SPACE; // the center point of the lowest (first) component
		double leftLastY = midCenterY + 2 * SPACE; // the center point of the highest left component
		double midFirstX = leftCenterX + 2 * SPACE; // the center point of the first mid component
		double midLastX = rightCenterX - 2 * SPACE; // the center point of the last mid component
		double rightLastY = maxY - SPACE;
		double rightFirstY = numRight > 1 ? midCenterY + 2 * SPACE : rightLastY;
		// width of the respective section:
		double spanX = 0.2 * maxX;
		double spanY = 0.2 * maxY;

		double leftStepY = numLeft > 1 ? (leftFirstY - leftLastY)
				/ (numLeft - 1) : 0;
		for (int i = 0; i < numLeft; i++) {
			int numInGroup = infoGroups.get(i).size();
			// the "crowding factor" is the number by which one would have to
			// divide the components so that they do not overlap anymore
			// -- a crowdingFactor of 1 means "not crowded"
			int crowdingFactor = (int) Math.ceil(numInGroup * componentWidth / (spanY + componentWidth));
			coords[i] = new Point2D.Double[numInGroup];
			double centerY = leftFirstY - i * leftStepY;
			double outerY = centerY;
			double innerY = centerY;
			if (centerY < midCenterY + spanY / 2 + SPACE) { // danger of overlap with mid
				innerY += SPACE;
				outerY = centerY - (innerY - centerY);
			}
			double outerX = numInGroup == 1 ? leftCenterX : leftCenterX - spanX / 2;
			double innerX = numInGroup == 1 ? leftCenterX : leftCenterX + spanX / 2;
			double stepX = numInGroup == 1 ? 0 : (innerX - outerX) / (numInGroup - 1);
			double stepY = numInGroup == 1 ? 0 : (innerY - outerY) / (numInGroup - 1);
			for (int j = 0; j < numInGroup; j++) {
				// in crowded conditions, make more space by shifting some of the items:
				double shift = 0;
				double unitShift = componentHeight * 0.8;
				if (crowdingFactor > 1 && Math.abs(outerY - innerY) < 1.e-9 /* only if otherwise flat */) {
					for (int k=1; k<crowdingFactor; k++) {
						if ((j-k)%crowdingFactor == 0) {
							shift = k * unitShift;
						}
					}
				}
				coords[i][j] = new Point2D.Double(outerX + j * stepX, outerY + j * stepY + shift);
			}
		}
		double midStepX = numMid > 1 ? (midLastX - midFirstX) / (numMid - 1) : 0;
		for (int i = 0; i < numMid; i++) {
			int numInGroup = infoGroups.get(numLeft + i).size();
			coords[numLeft + i] = new Point2D.Double[numInGroup];
			double centerX = midFirstX + i * midStepX;
			double outerX = centerX;
			double innerX = centerX;
			if (centerX < leftCenterX + spanX / 2 + SPACE) { // danger of overlap left
				innerX += SPACE;
				outerX = centerX - (innerX - centerX);
			} else if (centerX > rightCenterX - spanX / 2 - SPACE) { // danger of overlap right
				innerX -= SPACE;
				outerX = centerX + (centerX - innerX);
			}
			double outerY = numInGroup == 1 ? midCenterY : midCenterY - spanY / 2;
			double innerY = numInGroup == 1 ? midCenterY : midCenterY + spanY / 2;
			double stepY = numInGroup == 1 ? 0 : (innerY - outerY) / (numInGroup - 1);
			double stepX = numInGroup == 1 ? 0 : (innerX - outerX) / (numInGroup - 1);
			for (int j = 0; j < numInGroup; j++) {
				coords[numLeft + i][j] = new Point2D.Double(outerX + j * stepX, outerY + j * stepY);
			}
		}
		double rightStepY = numRight > 1 ? (rightLastY - rightFirstY)
				/ (numRight - 1) : 0;
		for (int i = 0; i < numRight; i++) {
			int numInGroup = infoGroups.get(numLeft + numMid + i).size();
			// the "crowding factor" is the number by which one would have to
			// divide the components so that they do not overlap anymore
			// -- a crowdingFactor of 1 means "not crowded"
			int crowdingFactor = (int) Math.ceil(numInGroup * componentWidth / (spanY + componentWidth));
			coords[numLeft + numMid + i] = new Point2D.Double[numInGroup];
			double centerY = rightFirstY + i * rightStepY;
			double outerY = centerY;
			double innerY = centerY;
			if (centerY < midCenterY + spanY / 2 + SPACE) { // danger of overlap with mid
				innerY += SPACE;
				outerY = centerY - (innerY - centerY);
			}
			double outerX = numInGroup == 1 ? rightCenterX : rightCenterX + spanX / 2;
			double innerX = numInGroup == 1 ? rightCenterX : rightCenterX - spanX / 2;
			double stepX = numInGroup == 1 ? 0 : (innerX - outerX) / (numInGroup - 1); // negative in this case
			double stepY = numInGroup == 1 ? 0 : (innerY - outerY) / (numInGroup - 1);
			for (int j = 0; j < numInGroup; j++) {
				// in crowded conditions, make more space by shifting some of the items:
				double shift = 0;
				double unitShift = componentHeight * 0.8;
				if (crowdingFactor > 1 && Math.abs(outerY - innerY) < 1.e-9 /* only if otherwise flat */) {
					for (int k=1; k<crowdingFactor; k++) {
						if ((j-k)%crowdingFactor == 0) {
							shift = k * unitShift;
						}
					}
				}
				coords[numLeft + numMid + i][j] = new Point2D.Double(outerX + j * stepX, outerY + j * stepY + shift);
			}
		}
	}

	/**
	 * Arrange coordinates on a half circle.
	 * 
	 * @param maxX
	 * @param maxY
	 * @param numGroups
	 * @param coords
	 */
	@SuppressWarnings("unused")
	private void layoutHalfCircle(int maxX, int maxY, int numGroups,
			Rectangle2D.Double[][] coords) {
		double angleDelta = Math.PI / (numGroups - 1);
		double radiusX = maxX * 0.4;
		double radiusY = maxY * 0.8;
		double spanX = maxX * 0.2;
		double spanY = maxY * 0.3;
		Point2D center = new Point2D.Double(maxX / 2., maxY - componentHeight / 2.);
		for (int i = 0; i < infoGroups.size(); i++) {
			List<Info> group = infoGroups.get(i);
			double angle = i * angleDelta;
			int numInGroup = group.size();
			coords[i] = new Rectangle2D.Double[numInGroup];
			double outerRadiusX = numInGroup == 1 ? radiusX : radiusX + spanX
					/ 2;
			double outerRadiusY = numInGroup == 1 ? radiusY : radiusY + spanY
					/ 2;
			double deltaX = numInGroup == 1 ? 0 : spanX / (numInGroup - 1);
			double deltaY = numInGroup == 1 ? 0 : spanY / (numInGroup - 1);
			for (int j = 0; j < numInGroup; j++) {
				Point2D compCenter = toLocation(angle, outerRadiusX - j
						* deltaX, outerRadiusY - j * deltaY, center);
				coords[i][j] = new Rectangle2D.Double(compCenter.getX()
						- componentWidth / 2., compCenter.getY()
						- componentHeight / 2., componentWidth, componentHeight);
			}
		}
	}

	private Point2D toLocation(double angle, double radiusX, double radiusY,
			Point2D center) {
		double deltaX = -Math.cos(angle) * radiusX;
		double deltaY = Math.sin(angle) * radiusY;
		return new Point2D.Double(center.getX() + deltaX, center.getY()
				- deltaY);
	}

	private void layoutCallbackTopics(int maxX, int maxY,
			int numCallbackTopics, Point2D.Double[] coords) {
		assert numCallbackTopics > 0;
		final double SPACE = 50;
		double x = 0.5 * maxX; // same X coordinate for all callback topics
		double firstY = 0.5 * maxY;
		double lastY = maxY - SPACE;
		if (numCallbackTopics == 1) {
			double y = firstY + (lastY - firstY) / 2;
			coords[0] = new Point2D.Double(x, y);
		} else { // more than one callback topic
			double stepY = (lastY - firstY) / (numCallbackTopics - 1);
			for (int i = 0; i < numCallbackTopics; i++) {
				double y = firstY + i * stepY;
				coords[i] = new Point2D.Double(x, y);
			}
		}
	}

	public void run() {
		setupGUI();
		// Give the GUI some time to set up properly before sending edit
		// requests
		// try {
		// Thread.sleep(1000);
		// } catch (InterruptedException ie) {}
		while (true) {
			try {
				Thread.sleep(100);
			} catch (InterruptedException ie) {
			}
			try {
				redraw();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}

	public synchronized ComponentInfo getComponentInfo(String componentName) {
		for (ComponentInfo ci : sortedComponentList) {
			if (ci.toString().equals(componentName)) {
				return ci;
			}
		}
		return null;
	}

	public TopicInfo getTopicInfo(String topicName) {
		return topics.get(topicName);
	}

	public void setSystemStatus(final String text) {
		if (systemStatus != null) {
			SwingUtilities.invokeLater(new Runnable() {
				public void run() { systemStatus.setText(text); };
			});
						
		}
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		ComponentInfo[] cis = new ComponentInfo[] {
				new ComponentInfo("speech synthesis",
						new String[] { "semaine.data.synthesis.plan" },
						new String[] {
								"semaine.data.synthesis.plan.speechtimings",
								"semaine.data.lowlevel.audio" }, false, false),
				new ComponentInfo("audio features", null, null,
				// new String[] {"semaine.data.analysis.audio"},
						true, false),
				new ComponentInfo("ASR",
						new String[] { "semaine.data.analysis.audio" },
						new String[] { "semaine.data.state.user.emma" }, false,
						false),
				new ComponentInfo("turn taking interpreter", new String[] {
						"semaine.data.state.user.behaviour",
						"semaine.data.state.user.emma" },
						new String[] { "semaine.data.state.user.behaviour" },
						false, false),
				new ComponentInfo("user utterance interpreter", new String[] {
						"semaine.data.state.user.behaviour",
						"semaine.data.state.user.emma",
						"semaine.data.state.user.intention" }, new String[] {
						"semaine.data.state.user.intention",
						"semaine.data.state.agent.intention" }, false, false),
				new ComponentInfo(
						"agent utterance proposer",
						new String[] { "semaine.data.state.agent.intention" },
						new String[] { "semaine.data.action.candidate.function" },
						false, false),
				new ComponentInfo("emotion detection",
						new String[] { "semaine.data.analysis.audio" },
						new String[] { "semaine.data.state.user.emma" }, false,
						false),
				new ComponentInfo(
						"mimicry action proposer",
						new String[] { "semaine.data.state.user.behaviour" },
						new String[] { "semaine.data.action.candidate.behaviour" },
						false, false),
				new ComponentInfo(
						"backchannel action proposer",
						new String[] { "semaine.data.state.user.emma" },
						new String[] { "semaine.data.action.candidate.behaviour" },
						false, false),
				new ComponentInfo("action selection",
						new String[] { "semaine.data.action.candidate.>" },
						new String[] { "semaine.data.action.selected.function",
								"semaine.data.action.selected.behaviour" },
						false, false),
				new ComponentInfo("Greta player", new String[] {
						"semaine.data.lowlevel.video",
						"semaine.data.lowlevel.audio" }, null, false, true),
				new ComponentInfo(
						"FML2BML",
						new String[] { "semaine.data.action.selected.function.speechpreprocessed" },
						new String[] { "semaine.data.synthesis.plan" }, false,
						false),
				new ComponentInfo(
						"speech preprocessing",
						new String[] { "semaine.data.action.selected.function" },
						new String[] { "semaine.data.action.selected.function.speechpreprocessed" },
						false, false),
				new ComponentInfo(
						"BML realiser",
						new String[] { "semaine.data.synthesis.plan.speechtimings" },
						new String[] { "semaine.data.lowlevel.video" }, false,
						false) };
		SystemMonitor mon = new SystemMonitor(cis, null);
		// mon.start();
		mon.setupGUI();
		for (int i = 0; i < cis.length; i++) {
			try {
				Thread.sleep(100);
			} catch (InterruptedException ie) {
			}
			cis[i].setState(Component.State.ready);
			mon.redraw();
		}
		ComponentInfo audioFeatures = mon.getComponentInfo("audio features");
		audioFeatures.setSendTopics("semaine.data.analysis.audio");
		mon.redraw();
		try {
			Thread.sleep(3000);
		} catch (InterruptedException ie) {
		}
		// mon.addComponentInfo();
		TopicInfo ti = mon.topics.get("semaine.data.state.user.emma");
		for (int i = 0; i < 1000; i++) {
			ti.addMessage("Test message " + i, "emotion detection");
			mon.redraw();
			try {
				Thread.sleep(1000);
			} catch (InterruptedException ie) {
			}

		}
	}

}
