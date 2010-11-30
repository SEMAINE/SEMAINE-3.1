/* @author Dennis Hofs - Modified for Semaine by Mark ter Maat
 */
// Last modification by: $Author: hofs $
// $Log: XmlErrorHandler.java,v $
// Revision 1.4  2006/07/04 08:23:46  hofs
// no message
//
// Revision 1.3  2006/06/07 12:08:03  hofs
// *** empty log message ***
//
// Revision 1.2  2005/11/10 14:03:54  hofs
// Added javadoc
//
// Revision 1.1  2005/11/08 09:05:12  hofs
// Initial version
//

package eu.semaine.components.dialogue.woz;

import org.xml.sax.ErrorHandler;
import org.xml.sax.SAXParseException;

/**
 * <p>This class implements a generic XML error handler for SAX parse
 * errors.</p>
 * 
 * <p>When an error occurs, it is saved in this class, so it can be retrieved
 * later with {@link #getSaxException() getSaxException()}.</p>
 * 
 * @author Dennis Hofs
 */
public class XmlErrorHandler implements ErrorHandler {
	private SAXParseException exception = null;

	///////////////////////////////////////////////////////////////////////////
	// ErrorHandler
	public void error(SAXParseException exception) {
		if (this.exception == null)
			this.exception = exception;
	}

	public void fatalError(SAXParseException exception) {
		if (this.exception == null)
			this.exception = exception;
	}

	public void warning(SAXParseException exception) {
		if (this.exception == null)
			this.exception = exception;
	}
	//
	///////////////////////////////////////////////////////////////////////////

	/**
	 * <p>Returns the SAX exception. If no SAX exception occurred, this method
	 * returns null. If more than one SAX exception occurred, this method only
	 * return the first one.</p>
	 * 
	 * @return the SAX exception
	 */
	public SAXParseException getSaxException() {
		return exception;
	}
}
