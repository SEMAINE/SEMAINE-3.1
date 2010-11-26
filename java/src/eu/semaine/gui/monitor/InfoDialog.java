/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.gui.monitor;

import javax.swing.JFrame;
import javax.swing.JScrollPane;
import javax.swing.JTextPane;
import javax.swing.SwingUtilities;
import javax.swing.text.BadLocationException;
import javax.swing.text.Document;

public class InfoDialog extends JFrame
{
	JTextPane textPane;
	private static final int MAX_DOCUMENT_LENGTH = 100000; // num characters to show
	
	public InfoDialog(String title)
	{
		super(title);
		setSize(400, 400);
        java.awt.Dimension screenSize = java.awt.Toolkit.getDefaultToolkit().getScreenSize();
        setLocation((screenSize.width-getWidth())/2,(screenSize.height-getHeight())/2);
		textPane = new JTextPane();
		textPane.setEditable(false);
		getContentPane().add(new JScrollPane(textPane));
	}

	public void setText(final String text)
	{
		if (!SwingUtilities.isEventDispatchThread()) {
			SwingUtilities.invokeLater(new Runnable() {
				@Override public void run() { setText(text); }
			});
			return;
		}
		textPane.setText(text);
		textPane.setCaretPosition(textPane.getDocument().getLength());
	}
	
	public void appendText(final String text)
	{
		if (!SwingUtilities.isEventDispatchThread()) {
			SwingUtilities.invokeLater(new Runnable() {
				@Override public void run() { appendText(text); }
			});
			return;
		}
		Document doc = textPane.getDocument();
		boolean doScrollToEnd = false; // should we scroll to the (new) end?
		if (textPane.getCaretPosition() == doc.getLength()) {
			// scroll only if the current caret was at the end
			doScrollToEnd = true;
		}
		try {
			int len = doc.getLength();
			if (len + text.length() > MAX_DOCUMENT_LENGTH) {
				int maxlen = Math.max(0, MAX_DOCUMENT_LENGTH - text.length());
				// current text must be shortened to maximally maxlen
				String oldText = doc.getText(0, len-maxlen);
				// But cut at line break:
				int cutPos = oldText.lastIndexOf('\n');
				if (cutPos == -1) {
					cutPos = oldText.length(); 
				}
				int selStart = textPane.getSelectionStart();
				int selEnd = textPane.getSelectionEnd();
				doc.remove(0, cutPos);
				if (selStart < selEnd) {
					textPane.setSelectionStart(selStart - cutPos);
					textPane.setSelectionEnd(selEnd - cutPos);
				}
			}
			doc.insertString(doc.getLength(), text, null);
		} catch (BadLocationException e) {
			e.printStackTrace();
			setText(text);
		}
		if (doScrollToEnd) {
			textPane.setCaretPosition(doc.getLength());
		}
	}
}
