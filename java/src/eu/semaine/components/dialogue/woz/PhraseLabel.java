/* @author Dennis Hofs - Modified for Semaine by Mark ter Maat
 */
// Last modification by: $Author: hofs $
// $Log: PhraseLabel.java,v $
// Revision 1.5  2006/08/29 11:07:48  hofs
// Updates for Java 6
//
// Revision 1.4  2005/11/24 13:16:15  hofs
// *** empty log message ***
//
// Revision 1.3  2005/11/09 12:52:26  hofs
// Added counters
//
// Revision 1.2  2005/11/09 09:45:06  hofs
// Improved layout
//
// Revision 1.1  2005/11/08 09:03:42  hofs
// Initial version
//
// Revision 1.4  2005/10/06 15:18:45  hofs
// Click when playing has no effect
//
// Revision 1.3  2005/10/06 12:35:03  hofs
// Supports WavPlayer
//
// Revision 1.2  2005/09/29 15:12:56  hofs
// *** empty log message ***
//
// Revision 1.1  2005/09/29 11:26:37  hofs
// *** empty log message ***
//

package eu.semaine.components.dialogue.woz;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Cursor;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Rectangle;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;

public class PhraseLabel extends JComponent {
	private static final long serialVersionUID = 0;

	private String text;
	private String wavfile;
	private Color background = Color.WHITE;
	private boolean playing = false;
	private boolean hyperlink = false;
	private JLabel countLabel;
	private JTextArea textArea;
	private JPanel mainPanel;
	private int count = 0;
	private Object updateMonitor = new Object();
	private Object listenerMonitor = new Object();
	private List<PhraseLabelListener> listeners = new ArrayList<PhraseLabelListener>();

	public PhraseLabel(SalProject.PhraseString text) {
		this.text = text.getPhrase();
		this.wavfile = text.getWavFile();
		Cursor cursor = new Cursor(Cursor.HAND_CURSOR);
		MouseListener listener = new LabelMouseListener();
		mainPanel = new JPanel(new BorderLayout(8,8));
		mainPanel.setBackground(Color.WHITE);
		mainPanel.setCursor(cursor);
		mainPanel.addMouseListener(listener);
		countLabel = new JLabel();
		countLabel.setBackground(Color.WHITE);
		countLabel.setForeground(Color.BLACK);
		countLabel.setFont(new Font("SansSerif",Font.PLAIN,12));
		countLabel.setVerticalAlignment(JLabel.TOP);
		countLabel.setCursor(cursor);
		countLabel.setOpaque(true);
		countLabel.addMouseListener(listener);
		countLabel.setBorder(BorderFactory.createEmptyBorder(0,4,0,4));
		textArea = new JTextArea();
		textArea.setText(this.text);
		textArea.setEditable(false);
		textArea.setLineWrap(true);
		textArea.setWrapStyleWord(true);
		textArea.setCursor(cursor);
		textArea.setFont(new Font("SansSerif",Font.PLAIN,12));
		textArea.addMouseListener(listener);
		updateCountLabel();
		mainPanel.add(countLabel,BorderLayout.WEST);
		mainPanel.add(textArea,BorderLayout.CENTER);
		setLayout(new BorderLayout());
		add(mainPanel,BorderLayout.CENTER);
	}
	
	public String getText() {
		return text;
	}
	
	public String getWavFile() {
		return wavfile;
	}
	
	public void addPhraseLabelListener(PhraseLabelListener l) {
		synchronized (listenerMonitor) {
			if (!listeners.contains(l))
				listeners.add(l);
		}
	}
	
	public void removePhraseLabelListener(PhraseLabelListener l) {
		synchronized (listenerMonitor) {
			listeners.remove(l);
		}
	}
	
	private void notifyPhraseClicked() {
		synchronized (listenerMonitor) {
			for (PhraseLabelListener l : listeners) {
				l.phraseClicked(this);
			}
		}
	}

	public void setBackground(Color c) {
		synchronized (updateMonitor) {
			background = c;
			updateColours();
		}
	}
	
	public void setSelectionColor(Color c) {
		synchronized (updateMonitor) {
			textArea.setSelectionColor(c);
		}
	}
	
	public void setSelectedTextColor(Color c) {
		synchronized (updateMonitor) {
			textArea.setSelectedTextColor(c);
		}
	}
	
	private void updateCountLabel() {
		countLabel.setText(Integer.toString(count));
		updateColours();
	}

	private void doPlay() {
		count++;
		updateCountLabel();
		playing = true;
		updateColours();
		notifyPhraseClicked();
	}

	private void setHyperLink(boolean b) {
		hyperlink = b;
		updateColours();
	}

	private void updateColours() {
		Color bg = playing ? Color.BLUE : background;
		super.setBackground(bg);
		textArea.setBackground(bg);
		mainPanel.setBackground(bg);
		if (count > 0) {
			countLabel.setBackground(Color.BLUE);
			countLabel.setForeground(Color.WHITE);
		} else {
			countLabel.setBackground(bg);
			countLabel.setForeground(bg);
		}
		if (playing)
			textArea.setForeground(Color.WHITE);
		else if (hyperlink)
			textArea.setForeground(Color.BLUE);
		else
			textArea.setForeground(Color.BLACK);
	}

	public void playerStopped() {
		synchronized (updateMonitor) {
			playing = false;
			updateColours();
		}
	}
	
	private class LabelMouseListener extends MouseAdapter {
		public void mouseEntered(MouseEvent e) {
			synchronized (updateMonitor) {
				setHyperLink(true);
			}
		}
		public void mouseExited(MouseEvent e) {
			synchronized (updateMonitor) {
				setHyperLink(false);
			}
		}
		public void mouseClicked(MouseEvent e) {
			synchronized (updateMonitor) {
				if (!playing)
					doPlay();
			}
		}
	}
	
	protected void paintBorder(Graphics g) {
		super.paintBorder(g);
		Rectangle rect = g.getClipBounds();
		Color bg = playing ? Color.BLUE : background;
		g.setColor(bg);
		g.fillRect(rect.x,rect.y,rect.width,rect.height);
	}
}
