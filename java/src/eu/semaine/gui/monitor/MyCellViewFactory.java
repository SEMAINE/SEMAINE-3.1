package eu.semaine.gui.monitor;

import org.jgraph.graph.DefaultCellViewFactory;
import org.jgraph.graph.DefaultGraphCell;
import org.jgraph.graph.VertexView;


@SuppressWarnings("serial")
public class MyCellViewFactory extends DefaultCellViewFactory
{
	
	protected VertexView createVertexView(Object v)
	{
		VertexView view;
		DefaultGraphCell cell = (DefaultGraphCell) v;
		Object userObject = cell.getUserObject();
		if (userObject instanceof ComponentInfo) {
			view = new JGraphEllipseView(); 
		} else {
			view = new VertexView();
		}
		view.setCell(v);
		return view;
	}
}
