/* @author Dennis Hofs - Modified for Semaine by Mark ter Maat
 */
// Last modification by: $Author: hofs $
// $Log: PhrasesPanel.java,v $
// Revision 1.4  2006/08/29 11:07:49  hofs
// Updates for Java 6
//
// Revision 1.3  2005/11/24 13:16:15  hofs
// *** empty log message ***
//
// Revision 1.2  2005/11/09 12:52:26  hofs
// Added counters
//
// Revision 1.1  2005/11/09 09:45:06  hofs
// Improved layout
//

package eu.semaine.components.dialogue.woz;

import java.awt.Color;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Rectangle;
import java.util.ArrayList;
import java.util.List;

import javax.swing.BorderFactory;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JViewport;
import javax.swing.Scrollable;
import javax.swing.SwingConstants;

public class PhrasesPanel extends JPanel implements Scrollable {
	private static final long serialVersionUID = 0;

	private ColumnLayout layout;
	private List<PhraseLabelListener> listeners = new ArrayList<PhraseLabelListener>();
	private List<PhraseLabel> labels = new ArrayList<PhraseLabel>();
	
	public PhrasesPanel(int cols) {
		setBackground(Color.WHITE);
		layout = new ColumnLayout(cols,8,16);
		setLayout(layout);
	}
	
	public void addPhraseLabelListener(PhraseLabelListener l) {
		if (!listeners.contains(l)) {
			listeners.add(l);
			for (PhraseLabel label : labels) {
				label.addPhraseLabelListener(l);
			}
		}
	}
	
	public void removePhraseLabelListener(PhraseLabelListener l) {
		listeners.remove(l);
		for (PhraseLabel label : labels) {
			label.removePhraseLabelListener(l);
		}
	}
	
	public void addPhrases(String title, List<SalProject.PhraseString> list) {
		JLabel titleLabel = new JLabel(title);
		titleLabel.setOpaque(true);
		titleLabel.setBackground(Color.BLACK);
		titleLabel.setForeground(Color.WHITE);
		titleLabel.setFont(new Font("SansSerif",Font.BOLD,12));
		titleLabel.setBorder(BorderFactory.createEmptyBorder(2,8,2,8));
		add(titleLabel);
		layout.startNewGroup();
		layout.addLayoutComponent(titleLabel);
		Color gray = new Color(220,220,255);
		Color white = Color.WHITE;
		Color currentBg = white;
		for (SalProject.PhraseString phrase : list) {
			PhraseLabel label = new PhraseLabel(phrase);
			for (PhraseLabelListener l : listeners) {
				label.addPhraseLabelListener(l);
			}
			label.setOpaque(true);
			label.setBorder(BorderFactory.createEmptyBorder(2,0,2,8));
			label.setBackground(currentBg);
			label.setSelectionColor(currentBg);
			label.setSelectedTextColor(Color.BLUE);
			if (currentBg == white)
				currentBg = gray;
			else
				currentBg = white;
			add(label);
			layout.addLayoutComponent(label);
		}
	}
	
	public Dimension getPreferredSize() {
		Dimension dim = layout.preferredLayoutSize(getWidth());
		Container parent = getParent();
		if (parent instanceof JViewport && parent.getComponentCount() == 1 &&
				dim.height < parent.getHeight()) {
			dim.height = parent.getHeight();
		}
		return dim;
	}

	public Dimension getPreferredScrollableViewportSize() {
		return getPreferredSize();
	}

	public int getScrollableUnitIncrement(Rectangle visibleRect, int orientation, int direction) {
		return 20;
	}

	public int getScrollableBlockIncrement(Rectangle visibleRect, int orientation, int direction) {
		if (orientation == SwingConstants.VERTICAL)
			return visibleRect.height-20;
		else
			return visibleRect.width-20;
	}

	public boolean getScrollableTracksViewportWidth() {
		return true;
	}

	public boolean getScrollableTracksViewportHeight() {
		return false;
	}
}
