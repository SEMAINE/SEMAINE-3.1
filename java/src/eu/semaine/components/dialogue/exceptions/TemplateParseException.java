package eu.semaine.components.dialogue.exceptions;

/**
 * This class is used for exceptions that arise during the parsing of Templates
 * 
 * @author Mark ter Maat
 * @version 0.1
 */

public class TemplateParseException extends Exception
{
	/**
	 * Creates a new TemplateParseException
	 */
	public TemplateParseException()
	{
		super();
	}
	
	/**
	 * Creates a new TemplateParseException with an error-message.
	 * @param err - the error-message
	 */
	public TemplateParseException( String err )
	{
		super(err);
	}
}
