package eu.semaine.components.dialogue.behaviourselection.template.preconditions;

import eu.semaine.components.dialogue.informationstate.InformationState;

/**
 * This interface defines a Precondition of a template. This can either be a Compare or an Indicator.
 * 
 * @author Mark ter Maat
 * @version 0.1
 * 
 */

public abstract class Precondition
{
	/**
	 * Given the current InformationState, checks if this Precondition is valid.
	 * 
	 * @param is - the current InformationState
	 * @return 	- true if the precondition is valid
	 * 			- false if the precondition is not valid, or if an error occurred.
	 */
	public abstract boolean isValid( InformationState is );
}
