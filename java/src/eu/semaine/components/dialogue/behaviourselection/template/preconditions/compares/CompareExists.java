package eu.semaine.components.dialogue.behaviourselection.template.preconditions.compares;

import eu.semaine.components.dialogue.behaviourselection.template.preconditions.Compare;
import eu.semaine.components.dialogue.behaviourselection.template.value.Value;
import eu.semaine.components.dialogue.exceptions.TemplateParseException;
import eu.semaine.components.dialogue.exceptions.TemplateRunException;
import eu.semaine.components.dialogue.informationstate.InformationState;

/**
 * A CompareExists checks if a value currently exists in the InformationState.
 * 
 * @author Mark ter Maat
 * @version 0.1
 *
 */

public class CompareExists extends Compare
{
	/**
	 * Creates a new CompareEquals with the given value.
	 * 
	 * @param value1
	 * @throws TemplateParseException
	 */
	public CompareExists( String value1 ) throws TemplateParseException
	{
		super( value1, null, Compare.Comparator.exists );
	}
	
	/**
	 * Given the current InformationState, checks if the value exists.
	 * 
	 * @param is - the current InformationState
	 * @returns	true  - if the value exists
	 * 			false - if the value does not exist
	 */
	public boolean isValid( InformationState is )
	{	
		/* Get the current Values of the 2 values. */
		Value value;
		try {
			value = abstractValue1.getValue(is);
		} catch( TemplateRunException e ) {
			e.printStackTrace();
			return false;
		}
		
		/* If the Value is not null, than it exists. */
		if( value != null ) {
			return true;
		} else {
			return false;
		}
	}
}
