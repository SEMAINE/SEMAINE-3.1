/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package eu.semaine.gui.monitor;


import java.awt.Color;
import java.awt.Image;
import java.awt.Toolkit;
import java.io.StringWriter;
import java.net.URL;
import java.util.HashMap;
import java.util.Map;
import java.util.StringTokenizer;

import javax.swing.JTextPane;
import javax.swing.text.AttributeSet;
import javax.swing.text.BadLocationException;
import javax.swing.text.MutableAttributeSet;
import javax.swing.text.SimpleAttributeSet;
import javax.swing.text.StyleConstants;
import javax.swing.text.StyledDocument;

import org.apache.log4j.AppenderSkeleton;
import org.apache.log4j.Layout;
import org.apache.log4j.Level;
import org.apache.log4j.helpers.QuietWriter;
import org.apache.log4j.spi.LoggingEvent;


/**
 * <b>Experimental</b> TextPaneAppender. <br>
 *
 *
 * Created: Sat Feb 26 18:50:27 2000 <br>
 *
 * @author Sven Reimers
 */

public class TextPaneAppender extends AppenderSkeleton {

	JTextPane textpane;
	StyledDocument doc;
	StringWriter sw;
	QuietWriter qw;
	Map<Level, MutableAttributeSet> attributes;

	private String label;

	private boolean fancy;

//	final String LABEL_OPTION = "Label";
//	final String COLOR_OPTION_FATAL = "Color.Emerg";
//	final String COLOR_OPTION_ERROR = "Color.Error";
//	final String COLOR_OPTION_WARN = "Color.Warn";
//	final String COLOR_OPTION_INFO = "Color.Info";
//	final String COLOR_OPTION_DEBUG = "Color.Debug";
//	final String COLOR_OPTION_BACKGROUND = "Color.Background";
//	final String FANCY_OPTION = "Fancy";
//	final String FONT_NAME_OPTION = "Font.Name";
//	final String FONT_SIZE_OPTION = "Font.Size";

	public static Image loadIcon ( String path ) {
		Image img = null;
		try {
			URL url = ClassLoader.getSystemResource(path);
			img = (Image) (Toolkit.getDefaultToolkit()).getImage(url);
		} catch (Exception e) {
			System.out.println("Exception occured: " + e.getMessage() + 
					" - " + e );   
		}	
		return (img);
	}

	public TextPaneAppender(Layout layout, String name, JTextPane textPane) {
		this();
		this.layout = layout;
		this.name = name;
		setTextPane(textPane);
		createAttributes();
	}

	public TextPaneAppender() {
		super();
		setTextPane(new JTextPane());
		createAttributes();
		this.label="";
		this.sw = new StringWriter();
		this.qw = new QuietWriter(sw, errorHandler);
		this.fancy =true;
	}

	public
	void close() {

	}

	private void createAttributes() {	
		Level prio[] = new Level[] { Level.ERROR, Level.WARN, Level.INFO, Level.DEBUG};

		attributes = new HashMap<Level, MutableAttributeSet>();
		for (int i=0; i<prio.length;i++) {
			MutableAttributeSet att = new SimpleAttributeSet();
			attributes.put(prio[i], att);
			StyleConstants.setFontSize(att,14);
		}
		StyleConstants.setForeground((MutableAttributeSet)attributes.get(Level.ERROR),Color.red);
		StyleConstants.setForeground((MutableAttributeSet)attributes.get(Level.WARN),Color.orange);
		StyleConstants.setForeground((MutableAttributeSet)attributes.get(Level.INFO),Color.gray);
		StyleConstants.setForeground((MutableAttributeSet)attributes.get(Level.DEBUG),Color.black);
	}


	public void append(LoggingEvent event) {
		String text = this.layout.format(event);
		String trace="";
		for (int i=0; i< sw.getBuffer().length(); i++) {
			if (sw.getBuffer().charAt(i)=='\t')
				sw.getBuffer().replace(i,i+1,"        ");
		}
		trace = sw.toString();
		sw.getBuffer().delete(0,sw.getBuffer().length());
		try {
			doc.insertString(doc.getLength(),text+trace,
					(MutableAttributeSet)attributes.get(event.getLevel()));
		}	
		catch (BadLocationException badex) {
			System.err.println(badex);
		}	
		textpane.setCaretPosition(doc.getLength());
	}

	public
	JTextPane getTextPane() {
		return textpane;
	}

	private
	static
	Color parseColor (String v) {
		StringTokenizer st = new StringTokenizer(v,",");
		int val[] = {255,255,255,255};
		int i=0;
		while (st.hasMoreTokens()) {
			val[i]=Integer.parseInt(st.nextToken());
			i++;
		}
		return new Color(val[0],val[1],val[2],val[3]);
	}

	private
	static
	String colorToString(Color c) {
		// alpha component emitted only if not default (255)
		String res = ""+c.getRed()+","+c.getGreen()+","+c.getBlue();
		return c.getAlpha() >= 255 ? res : res + ","+c.getAlpha();
	}

	public
	void setLayout(Layout layout) {
		this.layout=layout;
	}

	public
	void setName(String name) {
		this.name = name;
	}


	public
	void setTextPane(JTextPane textpane) {
		this.textpane=textpane;
		textpane.setEditable(false);
		textpane.setBackground(Color.WHITE);
		this.doc=textpane.getStyledDocument();
	}

	private
	void setColor(Level p, String v) {
		StyleConstants.setForeground(
				(MutableAttributeSet)attributes.get(p),parseColor(v));	
	}

	private
	String getColor(Level p) {
		Color c =  StyleConstants.getForeground(
				(MutableAttributeSet)attributes.get(p));
		return c == null ? null : colorToString(c);
	}

	/////////////////////////////////////////////////////////////////////
	// option setters and getters

	public
	void setLabel(String label) {
		this.label = label;
	}
	public
	String getLabel() {
		return label;
	}

	public
	void setColorEmerg(String color) {
		setColor(Level.FATAL, color);
	}
	public
	String getColorEmerg() {
		return getColor(Level.FATAL);
	}

	public
	void setColorError(String color) {
		setColor(Level.ERROR, color);
	}
	public
	String getColorError() {
		return getColor(Level.ERROR);
	}

	public
	void setColorWarn(String color) {
		setColor(Level.WARN, color);
	}
	public
	String getColorWarn() {
		return getColor(Level.WARN);
	}

	public
	void setColorInfo(String color) {
		setColor(Level.INFO, color);
	}
	public
	String getColorInfo() {
		return getColor(Level.INFO);
	}

	public
	void setColorDebug(String color) {
		setColor(Level.DEBUG, color);
	}
	public
	String getColorDebug() {
		return getColor(Level.DEBUG);
	}

	public
	void setColorBackground(String color) {
		textpane.setBackground(parseColor(color));
	}
	public
	String getColorBackground() {
		return colorToString(textpane.getBackground());
	}

	public
	void setFancy(boolean fancy) {
		this.fancy = fancy;
	}
	public
	boolean getFancy() {
		return fancy;
	}

	public
	void setFontSize(int size) {
		for (MutableAttributeSet mas : attributes.values()) {
			StyleConstants.setFontSize(mas,size);
		}
	}

	public
	int getFontSize() {
		AttributeSet attrSet = (AttributeSet) attributes.get(Level.INFO);
		return StyleConstants.getFontSize(attrSet);
	}

	public
	void setFontName(String name) {
		for (MutableAttributeSet mas : attributes.values()) {
			StyleConstants.setFontFamily(mas,name);
		}
	}

	public
	String getFontName() {
		AttributeSet attrSet = (AttributeSet) attributes.get(Level.INFO);
		return StyleConstants.getFontFamily(attrSet);
	}

	public
	boolean requiresLayout() {
		return true;
	}
} // TextPaneAppender



