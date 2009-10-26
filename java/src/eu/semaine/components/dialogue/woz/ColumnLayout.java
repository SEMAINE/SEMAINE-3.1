/* @author Dennis Hofs - Modified for Semaine by Mark ter Maat
 */
// Last modification by: $Author: hofs $
// $Log: ColumnLayout.java,v $
// Revision 1.2  2005/11/24 13:16:14  hofs
// *** empty log message ***
//
// Revision 1.1  2005/11/09 09:45:06  hofs
// Improved layout
//

package eu.semaine.components.dialogue.woz;

import java.awt.Component;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.LayoutManager;
import java.util.*;

/**
 * <p>A column layout can layout groups of components in one or more columns.
 * When this layout is set to a container, components should be added to the
 * container and this layout. A component is added to the layout with {@link
 * #addLayoutComponent(Component) addLayoutComponent()}. Groups of components
 * are created by separating one group from another with {@link
 * #startNewGroup() startNewGroup()}.</p>
 * 
 * <p>At construction a number of columns should be specified. That is the
 * maximum number of columns. If all components can be layed out in less
 * columns, only that number of columns will be used. It is always tried to
 * keep groups together in one column. If there is no way to keep groups
 * together and lay out all components in the available space, groups may
 * be split over different columns. If a group has more than one component,
 * the group will never be split after the first component (considering that
 * it may be a group header).</p>
 * 
 * <p>To determine the minimum required height to lay out all components in an
 * area with a certain width, call {@link #preferredLayoutSize(int)
 * preferredLayoutSize()}.</p>
 */
public class ColumnLayout implements LayoutManager {
	private int cols;
	private int colSpace;
	private int groupSpace;
	private List<List<Component>> groups;
	private List<Component> currentGroup;
	
	/**
	 * <p>Constructs a new column layout. Components will be added in the
	 * specified number of columns or less (if all components fit in less
	 * columns).</p>
	 * 
	 * @param cols the maximum number of columns
	 * @param colSpace the horizontal space between two columns (in pixels)
	 * @param groupSpace the vertical space between two groups (in pixels)
	 */
	public ColumnLayout(int cols, int colSpace, int groupSpace) {
		this.cols = cols;
		this.colSpace = colSpace;
		this.groupSpace = groupSpace;
		groups = new ArrayList<List<Component>>();
		currentGroup = new ArrayList<Component>();
		groups.add(currentGroup);
	}
	
	/**
	 * <p>Starts a new group of components. This closes the current group,
	 * so components that are {@linkplain #addLayoutComponent(Component) added}
	 * after calling this method, will in the new group. If the current group
	 * is empty, this method has no effect. After construction the layout
	 * starts with an empty current group, so it is not important whether this
	 * method is called before the first group.</p>
	 */
	public void startNewGroup() {
		if (currentGroup.size() == 0)
			return;
		currentGroup = new ArrayList<Component>();
		groups.add(currentGroup);
	}
	
	/**
	 * <p>Adds a component to this layout. See {@link #startNewGroup()
	 * startNewGroup()} for grouping components.</p>
	 * 
	 * @param comp the component
	 */
	public void addLayoutComponent(Component comp) {
		currentGroup.add(comp);
	}
	
	/**
	 * <p>The same as {@link #addLayoutComponent(Component)
	 * addLayoutComponent(comp)}.</p>
	 * 
	 * @param comp the component
	 */
	public void addLayoutComponent(String name, Component comp) {
		addLayoutComponent(comp);
	}
	
	/**
	 * <p>Removes the specified component from this layout.</p>
	 * 
	 * @param comp the component
	 */
	public void removeLayoutComponent(Component comp) {
		int i = 0;
		while (i < groups.size()) {
			List<Component> group = groups.get(i);
			if (group.contains(comp))
				group.remove(comp);
			if (group.size() == 0 && i < groups.size()-1) {
				groups.remove(i);
			} else {
				i++;
			}
		}
	}
	
	/**
	 * <p>Returns the minimum size of a container with this layout. Not all
	 * components may fit in this size.</p>
	 * 
	 * @param parent the container
	 * @return the minimum size
	 */
	public Dimension minimumLayoutSize(Container parent) {
		return new Dimension(100*cols+(cols-1)*colSpace,100);
	}
	
	/**
	 * <p>Returns the preferred size of a container with the specified with.
	 * The returned size will have the same width. It is the minimum size
	 * required to lay out all components.</p>
	 * 
	 * @param width the width
	 * @return the preferred size
	 */
	public Dimension preferredLayoutSize(int width) {
		List<Column> cs = layoutComponents(width,cols);
		int height = 0;
		for (Column c : cs) {
			if (c.getHeight() > height)
				height = c.getHeight();
		}
		return new Dimension(width,height);
	}
	
	/**
	 * <p>Returns the preferred size for the specified container. The returned
	 * size will have the same width as the container. It is the minimum size
	 * required to lay out all components.</p>
	 * 
	 * @param parent the container
	 * @return the preferred size
	 */
	public Dimension preferredLayoutSize(Container parent) {
		return preferredLayoutSize(parent.getWidth());
	}
	
	/**
	 * <p>Lays out all components in the specified container. This method will
	 * first try to keep the groups together, but if not all components fit in
	 * the container that way, groups may be split over different columns.</p>
	 * 
	 * @param parent the container
	 */
	public void layoutContainer(Container parent) {
		List<Column> cs = null;
		for (int i = 1; cs == null && i <= cols; i++) {
			cs = layoutGroups(parent,i);
		}
		if (cs == null) {
			cs = layoutComponents(parent.getWidth(),cols);
		}
		int colWidth = (parent.getWidth()-colSpace*(cs.size()-1))/cs.size();
		for (int i = 0; i < cs.size(); i++) {
			Column col = cs.get(i);
			int left = (colWidth+colSpace)*i;
			int top = 0;
			for (Component comp : col.getComponents()) {
				if (comp != null) {
					Dimension dim = comp.getPreferredSize();
					comp.setBounds(left,top,colWidth,dim.height);
					top += dim.height;
				} else {
					top += groupSpace;
				}
			}
		}
	}
	
	/**
	 * <p>Determines how all components should be layed out in an area with
	 * the specified width, in the specified maximum number of columns. This
	 * method does not try to keep groups together, but will keep the first
	 * and second component in a group together.</p>
	 * 
	 * <p>This method returns a list of columns, which can be used to determine
	 * the required height to lay out all components, or to actually do the
	 * layout.</p>
	 * 
	 * @param width the width of the area in which the components should be
	 * layed out
	 * @param cols the maximum number of columns
	 * @return a list of columns
	 */
	private List<Column> layoutComponents(int width, int cols) {
		int colWidth = (width-colSpace*(cols-1))/cols;
		for (List<Component> group : groups) {
			for (Component comp : group) {
				Dimension dim = comp.getPreferredSize();
				comp.setSize(colWidth,dim.height);
			}
		}
		int totalHeight = 0;
		for (List<Component> group : groups) {
			if (totalHeight > 0 && group.size() > 0)
				totalHeight += groupSpace;
			for (Component comp : group) {
				totalHeight += comp.getPreferredSize().height;
			}
		}
		int avgColHeight = totalHeight/cols;
		List<Column> result = new ArrayList<Column>();
		Column current = new Column();
		for (List<Component> group : groups) {
			if (current.getHeight() > 0)
				current.addSeparator();
			for (Component comp : group) {
				int height = comp.getPreferredSize().height;
				if (result.size()+1 < cols && current.getHeight() + height >
				avgColHeight && current.getHeight() > 0) {
					Component orphan = current.finishOrphan();
					result.add(current);
					current = new Column();
					if (orphan != null)
						current.addComponent(orphan);
					current.addComponent(comp);
				} else {
					current.addComponent(comp);
				}
			}
		}
		if (current.getComponents().size() > 0) {
			current.finish();
			result.add(current);
		}
		if (result.size() < cols)
			return layoutComponents(width,cols-1);
		else
			return result;
	}
	
	/**
	 * <p>Determines how all components should be layed out in the specified
	 * container, in the specified maximum number of columns. Groups of
	 * components will be kept together. If not all components fit in the
	 * container, this method returns null. In that case call {@link
	 * #layoutComponents(int, int) layoutComponents()}, which does not try to
	 * keep groups together.</p>
	 * 
	 * @param parent the container
	 * @param cols the maximum number of columns
	 * @return a list of columns
	 */
	private List<Column> layoutGroups(Container parent, int cols) {
		int colWidth = (parent.getWidth()-colSpace*(cols-1))/cols;
		for (List<Component> group : groups) {
			for (Component comp : group) {
				Dimension dim = comp.getPreferredSize();
				comp.setSize(colWidth,dim.height);
			}
		}
		int maxColHeight = parent.getHeight();
		List<Column> result = new ArrayList<Column>();
		Column current = new Column();
		for (List<Component> group : groups) {
			if (current.getHeight() > 0) {
				current.addSeparator();
			}
			int groupHeight = 0;
			for (Component comp : group) {
				groupHeight += comp.getPreferredSize().height;
			}
			if (current.getHeight() + groupHeight > maxColHeight) {
				if (groupHeight > maxColHeight || result.size()+1 >= cols)
					return null;
				current.finish();
				result.add(current);
				current = new Column();
			}
			for (Component comp : group) {
				current.addComponent(comp);
			}
		}
		if (current.getComponents().size() > 0) {
			current.finish();
			result.add(current);
		}
		if (result.size() < cols)
			return layoutGroups(parent,cols-1);
		else
			return result;
	}
	
	/**
	 * <p>This class represents a column specification. It contains the
	 * components and possible group separators to be layed out in a
	 * column.</p>
	 * 
	 * <p>Components and separators are added to the column with {@link
	 * #addComponent(Component) addComponent()} or {@link #addSeparator()
	 * addSeparator()}. When no more components will be added to the column,
	 * call {@link #finish() finish()} or {@link #finishOrphan()
	 * finishOrphan()}.</p>
	 */
	private class Column {
		private List<Component> comps;
		private int height;
		
		/**
		 * <p>Constructs a new empty column.</p>
		 */
		public Column() {
			comps = new ArrayList<Component>();
			height = 0;
		}
		
		/**
		 * <p>Adds a group separator to this column.</p>
		 */
		public void addSeparator() {
			comps.add(null);
			height += groupSpace;
		}
		
		/**
		 * <p>Adds a component to this column.</p>
		 * 
		 * @param comp a component
		 */
		public void addComponent(Component comp) {
			comps.add(comp);
			height += comp.getPreferredSize().height;
		}
		
		/**
		 * <p>Finishes this column. If there is a separator at the end of this
		 * column, it will be removed. This method should be called when no
		 * more components will be added to the column and there will not be
		 * columns.</p>
		 */
		public void finish() {
			if (comps.size() > 0 && comps.get(comps.size()-1) == null) {
				comps.remove(comps.size()-1);
				height -= groupSpace;
			}
		}
		
		/**
		 * <p>Finishes this column. If there is a separator plus a single
		 * component at the end of this column, it is assumed that the single
		 * component is the first component of a group with more than one
		 * component. Both the separator and the component are removed and
		 * the component is returned, so it can be added to the next
		 * column. If there is only a separator at the end of this column, it
		 * will be removed. This method should be called when no more
		 * components will be added to the column, but there will be more
		 * columns.</p>
		 * 
		 * @return an orphan component or null
		 */
		public Component finishOrphan() {
			if (comps.size() > 0 && comps.get(comps.size()-1) == null) {
				comps.remove(comps.size()-1);
				height -= groupSpace;
				return null;
			} else if (comps.size() > 1 && comps.get(comps.size()-2) == null) {
				Component comp = comps.remove(comps.size()-1);
				height -= comp.getPreferredSize().height;
				comps.remove(comps.size()-1);
				height -= groupSpace;
				return comp;
			} else {
				return null;
			}
		}
		
		/**
		 * <p>Returns the list of components in this column. The list may
		 * contain null values, which should be interpreted as group
		 * separators.</p>
		 * 
		 * @return a list with components and null values
		 */
		public List<Component> getComponents() {
			return comps;
		}
		
		/**
		 * <p>Returns the total height of all components and group separators
		 * in this column.</p>
		 * 
		 * @return the column height
		 */
		public int getHeight() {
			return height;
		}
	}
}
