/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.gui.monitor;

import javax.swing.JFrame;
import javax.swing.JScrollPane;
import javax.swing.JTextPane;
import javax.swing.text.BadLocationException;
import javax.swing.text.Document;

public class InfoDialog extends JFrame
{
	JTextPane textPane;
	
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

	public void setText(String text)
	{
		textPane.setText(text);
		textPane.setCaretPosition(textPane.getDocument().getLength());
	}
	
	public void appendText(String text)
	{
		Document doc = textPane.getDocument();
		boolean doScrollToEnd = false; // should we scroll to the (new) end?
		if (textPane.getCaretPosition() == doc.getLength()) {
			// scroll only if the current caret was at the end
			doScrollToEnd = true;
		}
		try {
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
