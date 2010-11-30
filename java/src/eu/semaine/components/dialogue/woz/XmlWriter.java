/* @author Dennis Hofs - Modified for Semaine by Mark ter Maat
 */
// Last modification by: $Author: hofs $
// $Log: XmlWriter.java,v $
// Revision 1.4  2006/07/04 08:23:46  hofs
// no message
//
// Revision 1.3  2006/03/22 10:29:47  hofs
// Updated javadoc
//
// Revision 1.2  2005/11/10 14:03:54  hofs
// Added javadoc
//
// Revision 1.1  2005/11/08 09:05:12  hofs
// Initial version
//

package eu.semaine.components.dialogue.woz;

import java.nio.ByteBuffer;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.List;

/**
 * <p>An XML writer facilitates the generation of an XML document. It contains
 * convenient methods to write XML elements. At construction it initializes an
 * internal string buffer and adds the following XML header:</p>
 * 
 * <p><pre>&lt;?xml version="1.0" encoding="UTF-8"?&gt;</pre></p>
 * 
 * <p>The append methods append XML code to the internal buffer. The writer
 * automatically takes care of formatting with indentation of subelements. When
 * all elements have been written, the complete document can be obtained with
 * {@link #getBytes() getBytes()} or {@link #toString() toString()}. The
 * returned bytes will contain the string encoded in UTF-8. If you get the
 * string and want to write it to a stream later, make sure that you encode it
 * in UTF-8 (see the header!)</p>
 * 
 * <p>The XML writer automatically escapes special characters with XML entities
 * in text nodes and attribute values.</p>
 * 
 * @author Dennis Hofs
 */
public class XmlWriter {
	private List<String> elemStack;
	private StringBuffer xml;
	private String newline;
	private String indent;
	private NodeType lastNode;
	
	private enum NodeType {
		START_TAG,
		END_TAG,
		TEXT
	}
	
	/**
	 * <p>Constructs a new XML writer.</p>
	 */
	public XmlWriter() {
		elemStack = new ArrayList<String>();
		newline = System.getProperty("line.separator");
		indent = "   ";
		xml = new StringBuffer("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
		lastNode = NodeType.END_TAG;
	}
	
	/**
	 * <p>Appends <i>n</i> indentations, where <i>n</i> is the number of
	 * currently open elements (whose start tag was written but end tag was
	 * not written yet).</p>
	 */
	private void appendIndent() {
		for (int i = 0; i < elemStack.size(); i++) {
			xml.append(indent);
		}
	}
	
	/**
	 * <p>Appends a start tag of an element with the specified name:
	 * "<code>&lt;name&gt;</code>".</p>
	 * 
	 * @param name the element name
	 */
	public void appendSTag(String name) {
		if (lastNode != NodeType.TEXT) {
			xml.append(newline);
			appendIndent();
		}
		xml.append("<" + name + ">");
		elemStack.add(0,name);
		lastNode = NodeType.START_TAG;
	}
	
	/**
	 * <p>Appends an open start tag of an element with the specified name:
	 * "<code>&lt;name</code>". This can be used to write a start tag or
	 * empty element with attributes. After calling this method, append the
	 * attributes with {@link #appendAttribute(String, String)
	 * appendAttribute()}. Finally close the start tag or element with
	 * {@link #appendCloseSTag() appendCloseSTag()} or {@link
	 * #appendCloseEmptyTag() appendCloseEmptyTag()}.</p>
	 * 
	 * @param name the element name
	 */
	public void appendOpenSTag(String name) {
		if (lastNode != NodeType.TEXT) {
			xml.append(newline);
			appendIndent();
		}
		xml.append("<" + name);
		elemStack.add(0,name);
		lastNode = NodeType.START_TAG;
	}
	
	/**
	 * <p>Closes an open start tag by appending "<code>&gt;</code>". This can
	 * be used after {@link #appendOpenSTag(String) appendOpenSTag()}.</p>
	 */
	public void appendCloseSTag() {
		xml.append(">");
	}
	
	/**
	 * <p>Appends an empty element with the specified name:
	 * "<code>&lt;name/&gt;</code>".</p>
	 * 
	 * @param name the element name
	 */
	public void appendEmptyTag(String name) {
		if (lastNode != NodeType.TEXT) {
			xml.append(newline);
			appendIndent();
		}
		xml.append("<" + name + "/>");
		lastNode = NodeType.END_TAG;
	}
	
	/**
	 * <p>Closes an empty element by appending "<code>/&gt;</code>". This can
	 * be used after {@link #appendOpenSTag(String) appendOpenSTag()}.</p>
	 */
	public void appendCloseEmptyTag() {
		xml.append("/>");
		elemStack.remove(0);
		lastNode = NodeType.END_TAG;
	}
	
	/**
	 * <p>Appends an attribute to an open start tag:
	 * "<code>&nbsp;name="value"</code>". This can be used after {@link
	 * #appendOpenSTag(String) appendOpenSTag()}.</p>
	 * 
	 * @param name the attribute name
	 * @param val the attribute value
	 */
	public void appendAttribute(String name, String val) {
		xml.append(" " + name + "=\"");
		appendString(val);
		xml.append("\"");
	}
	
	/**
	 * <p>Escapes special characters in the specified string and then appends
	 * the string.</p>
	 * 
	 * @param s the string
	 */
	private void appendString(String s) {
		s = s.replaceAll("&","&amp;");
		s = s.replaceAll("<","&lt;");
		s = s.replaceAll(">","&gt;");
		s = s.replaceAll("\"","&quot;");
		s = s.replaceAll("'","&apos;");
		xml.append(s);
	}
	
	/**
	 * <p>Appends a text node.</p>
	 * 
	 * @param s the text
	 */
	public void appendText(String s) {
		appendString(s);
		lastNode = NodeType.TEXT;
	}

	/**
	 * <p>Appends the end tag of the current element:
	 * "<code>&lt;/name&gt;</code>".</p>
	 */
	public void appendETag() {
		String name = elemStack.remove(0);
		if (lastNode == NodeType.END_TAG) {
			xml.append(newline);
			appendIndent();
		}
		xml.append("</" + name + ">");
		lastNode = NodeType.END_TAG;
	}
	
	/**
	 * <p>Returns the XML code. If the code is later written to a stream, make
	 * sure that it is encoded in UTF-8.</p>
	 * 
	 * @return the XML code
	 */
	public String toString() {
		return xml.toString();
	}
	
	/**
	 * <p>Returns the XML code in a UTF-8 encoded byte array to be written to
	 * a stream.</p>
	 * 
	 * @return the XML code
	 */
	public byte[] getBytes() {
		Charset charset = Charset.forName("UTF-8");
		ByteBuffer bb = charset.encode(toString());
		byte[] result = new byte[bb.remaining()];
		bb.get(result);
		return result;
	}
}
