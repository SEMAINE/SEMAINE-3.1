/* @author Dennis Hofs - Modified for Semaine by Mark ter Maat
 */
// Last modification by: $Author: hofs $
// $Log: BoxPanel.java,v $
// Revision 1.3  2006/08/29 11:07:48  hofs
// Updates for Java 6
//
// Revision 1.2  2006/02/23 13:32:25  hofs
// Media socket updates
//
// Revision 1.1  2005/11/24 13:15:36  hofs
// Initial version
//

package eu.semaine.components.dialogue.woz;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.Insets;
import java.awt.LayoutManager;
import java.awt.Rectangle;
import java.util.HashMap;
import java.util.Map;

import javax.swing.BorderFactory;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.Scrollable;

public class BoxPanel extends JPanel {
	private static final long serialVersionUID = 0;
	
	private JScrollPane scroller;
	private JPanel panel;
	private Map<String,JPanel> boxes = new HashMap<String,JPanel>();
	
	public BoxPanel() {
		panel = new ContentPanel();
		panel.setBorder(BorderFactory.createEmptyBorder(8,8,8,8));
		scroller = new JScrollPane(panel);
		scroller.setBorder(BorderFactory.createEmptyBorder());
		setLayout(new BorderLayout());
		add(scroller,BorderLayout.CENTER);
	}
	
	public Dimension getMinimumSize() {
		return new Dimension(panel.getMinimumSize().width,0);
	}
	
	public void addBox(String name, String title, Component comp) {
		JPanel box = new JPanel(new BorderLayout(8,8));
		box.setBackground(Color.WHITE);
		box.setBorder(BorderFactory.createCompoundBorder(
				BorderFactory.createEtchedBorder(),
				BorderFactory.createEmptyBorder(8,8,8,8)));
		box.add(new JLabel(title),BorderLayout.NORTH);
		if (comp != null)
			box.add(comp,BorderLayout.SOUTH);
		boxes.put(name,box);
		panel.add(box);
	}
	
	public void setBoxVisible(String name, boolean visible) {
		JPanel box = boxes.get(name);
		box.setVisible(visible);
	}
	
	public void setBoxComponent(String name, Component comp) {
		JPanel box = boxes.get(name);
		if (box.getComponentCount() == 2)
			box.remove(1);
		box.add(comp,BorderLayout.SOUTH);
		box.revalidate();
	}
	
	private class ContentPanel extends JPanel implements Scrollable {
		private static final long serialVersionUID = 0;

		public ContentPanel() {
			super();
			setLayout(new BoxLayout(8));
		}
		
		public Dimension getPreferredScrollableViewportSize() {
			return getPreferredSize();
		}

		public int getScrollableUnitIncrement(Rectangle visibleRect, int orientation, int direction) {
			return 10;
		}

		public int getScrollableBlockIncrement(Rectangle visibleRect, int orientation, int direction) {
			return visibleRect.height-10;
		}

		public boolean getScrollableTracksViewportWidth() {
			return true;
		}

		public boolean getScrollableTracksViewportHeight() {
			return false;
		}
		
		private class BoxLayout implements LayoutManager {
			private int space;
			
			public BoxLayout(int space) {
				this.space = space;
			}
			
			public void addLayoutComponent(String name, Component comp) {
			}

			public void removeLayoutComponent(Component comp) {
			}

			public Dimension preferredLayoutSize(Container parent) {
				Insets border = null;
				if (parent instanceof JComponent) {
					border = ((JComponent)parent).getBorder().getBorderInsets(
								parent);
				}
				int parentWidth = parent.getWidth();
				if (border != null) {
					parentWidth -= border.left + border.right;
				}
				Component[] comps = parent.getComponents();
				int maxWidth = 0;
				for (Component comp : comps) {
					if (comp.isVisible()) {
						comp.setSize(parentWidth,0);
						int width = comp.getPreferredSize().width;
						if (width > maxWidth)
							maxWidth = width;
					}
				}
				int totalHeight = 0;
				for (Component comp : comps) {
					if (comp.isVisible()) {
						comp.setSize(maxWidth,0);
						int height = comp.getPreferredSize().height;
						if (totalHeight > 0 && height > 0)
							totalHeight += space;
						totalHeight += height;
					}
				}
				if (border != null) {
					maxWidth += border.left + border.right;
					totalHeight += border.top + border.bottom;
				}
				return new Dimension(maxWidth,totalHeight);
			}

			public Dimension minimumLayoutSize(Container parent) {
				Insets border = null;
				if (parent instanceof JComponent) {
					border = ((JComponent)parent).getBorder().getBorderInsets(
								parent);
				}
				Component[] comps = parent.getComponents();
				int maxWidth = 0;
				for (Component comp : comps) {
					if (comp.isVisible()) {
						int width = comp.getMinimumSize().width;
						if (width > maxWidth)
							maxWidth = width;
					}
				}
				int totalHeight = 0;
				for (Component comp : comps) {
					if (comp.isVisible()) {
						int height = comp.getMinimumSize().height;
						if (totalHeight > 0 && height > 0)
							totalHeight += space;
						totalHeight += height;
					}
				}
				if (border != null) {
					maxWidth += border.left + border.right;
					totalHeight += border.top + border.bottom;
				}
				return new Dimension(maxWidth,totalHeight);
			}

			public void layoutContainer(Container parent) {
				Insets border = null;
				if (parent instanceof JComponent) {
					border = ((JComponent)parent).getBorder().getBorderInsets(
								parent);
				}
				Component[] comps = parent.getComponents();
				int top = 0;
				int left = 0;
				int width = parent.getWidth();
				if (border != null) {
					left += border.left;
					top += border.top;
					width -= border.left + border.right;
				}
				for (Component comp : comps) {
					if (comp.isVisible()) {
						comp.setSize(width,0);
						int height = comp.getPreferredSize().height;
						comp.setBounds(left,top,width,height);
						top += height;
						if (height > 0)
							top += space;
					}
				}
			}
		}
	}
}
