/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.gui.monitor;

import java.awt.Color;
import java.awt.Toolkit;
import java.awt.geom.Rectangle2D;
import java.util.HashMap;
import java.util.Map;

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

public class JGraphTest {

	/**
	 * @param args
	 */
	public static void main(String[] args) throws Exception 
	{
		// Construct Model and Graph
		GraphModel model = new DefaultGraphModel();
		JGraph graph = new JGraph(model);
		GraphLayoutCache view = graph.getGraphLayoutCache();
		view.setFactory(new MyCellViewFactory());
		graph.addGraphSelectionListener(new GraphSelectionListener() {
			public void valueChanged(GraphSelectionEvent e) {
				Object[] cells = e.getCells();
				for (int i=0; i<cells.length; i++) {
					if (e.isAddedCell(i)) {
						Object userObject = ((DefaultGraphCell)cells[i]).getUserObject();
						if (userObject instanceof ComponentInfo) {
							ComponentInfo ci = (ComponentInfo) userObject;
						} else if (userObject instanceof TopicInfo) {
							TopicInfo ti = (TopicInfo) userObject;
						}
					} else { // selection removed from cell
					}
				}
			}
		});
		graph.setEditable(false);
		graph.setMoveable(false);
		graph.setConnectable(false);
		graph.setDisconnectable(false);
		graph.setSelectionEnabled(true);
		// Control-drag should clone selection
		//graph.setCloneable(true);

		// Enable edit without final RETURN keystroke
		graph.setInvokesStopCellEditing(true);

		// When over a cell, jump to its default port (we only have one, anyway)
		graph.setJumpToDefaultPort(true);

		DefaultGraphCell hello = new DefaultGraphCell(new ComponentInfo("Component", null, null, false, false));
		GraphConstants.setBounds(hello.getAttributes(), new Rectangle2D.Double(10, 10, 120,30));
		GraphConstants.setOpaque(hello.getAttributes(), true);
		GraphConstants.setBorderColor(hello.getAttributes(), Color.black);
		hello.addPort();
		
		DefaultGraphCell world = new DefaultGraphCell(new TopicInfo("Topic"));
		GraphConstants.setBounds(world.getAttributes(), new Rectangle2D.Double(100, 100, 80,20));
		GraphConstants.setBorder(world.getAttributes(), BorderFactory.createRaisedBevelBorder());
		GraphConstants.setBackground(world.getAttributes(), new Color(100, 255, 100));
		GraphConstants.setOpaque(world.getAttributes(), true);
		world.addPort();

		DefaultEdge edge = new DefaultEdge();
		edge.setSource(hello.getChildAt(0));
		edge.setTarget(world.getChildAt(0));
		int arrow = GraphConstants.ARROW_CLASSIC;
		GraphConstants.setLineEnd(edge.getAttributes(), arrow);
		GraphConstants.setEndFill(edge.getAttributes(), true);
		GraphConstants.setSelectable(edge.getAttributes(), false);
		
		DefaultEdge edge2 = new DefaultEdge();
		edge2.setSource(hello.getChildAt(0));
		edge2.setTarget(world.getChildAt(0));
		GraphConstants.setLineEnd(edge2.getAttributes(), arrow);
		GraphConstants.setEndFill(edge2.getAttributes(), true);
		GraphConstants.setLineColor(edge2.getAttributes(), new Color(150, 150, 255));
		GraphConstants.setSelectable(edge2.getAttributes(), false);

		DefaultGraphCell[] cells = new DefaultGraphCell[] {hello, world, edge2, edge};
		view.insert(cells);

		
		
		// Show in Frame
		JFrame frame = new JFrame();
		frame.getContentPane().add(new JScrollPane(graph));
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setSize(Toolkit.getDefaultToolkit().getScreenSize());
		frame.setVisible(true);
		
		
		while(true) {
			Map<Object,Object> nested = new HashMap<Object, Object>();
			Map<Object, Object> helloAtt = new HashMap<Object, Object>();
			nested.put(hello, helloAtt);
			for (int delta = 55, min=200; delta >= 0; delta -= 5) {
				GraphConstants.setBackground(helloAtt, new Color(min+delta, min+delta, min-2*delta));
				view.edit(nested);
				Thread.sleep(100);
			}
			nested.remove(hello);
			Map<Object, Object> edge2Att = new HashMap<Object, Object>();
			nested.put(edge2, edge2Att);
			for (int width = 3, min=0; width >= min; width--) {
				GraphConstants.setLineWidth(edge2Att, width);
				view.edit(nested);
				Thread.sleep(100);
			}
			Thread.sleep(2000);
		}
		
	}

	

}
