package eu.semaine.components.dialogue.exceptions;

/**
 * This class is used for exceptions that arise during the verification and execution of Templates
 * 
 * @author Mark ter Maat
 * @version 0.1
 */

public class TemplateRunException extends Exception
{
	/**
	 * Creates a new TemplateRunException
	 */
	public TemplateRunException()
	{
		super();
	}
	
	/**
	 * Creates a new TemplateRunException with an error-message.
	 * @param err - the error-message
	 */
	public TemplateRunException( String err )
	{
		super(err);
	}
}
