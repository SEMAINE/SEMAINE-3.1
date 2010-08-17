package eu.semaine.components.dialogue.behaviourselection.template.preconditions.compares;

import eu.semaine.components.dialogue.behaviourselection.template.preconditions.Compare;
import eu.semaine.components.dialogue.behaviourselection.template.value.Value;
import eu.semaine.components.dialogue.exceptions.TemplateParseException;
import eu.semaine.components.dialogue.exceptions.TemplateRunException;
import eu.semaine.components.dialogue.informationstate.InformationState;
import eu.semaine.components.dialogue.informationstate.List;
import eu.semaine.components.dialogue.test.DMLogger;

/**
 * A CompareContains checks if the first value (which has to be a list) contains the second value.
 * Returns true if the list contains the value, or false if the list does not contain the value or if the types do not match.
 * 
 * @author Mark ter Maat
 * @version 0.1
 *
 */

public class CompareContains extends Compare 
{
	/**
	 * Creates a new CompareContains with the given values.
	 * 
	 * @param value1 - should be a list in the InformationState
	 * @param value2
	 * @throws TemplateParseException
	 */
	public CompareContains( String value1, String value2 ) throws TemplateParseException
	{
		super(value1, value2, Compare.Comparator.contains);
	}
	
	/**
	 * Given the current InformationState, checks if the first value is a list, and if 
	 * it contains the second value.
	 * 
	 * @param is - the current InformationState
	 * @returns	true  - if the second value exists in the list
	 * 			false - if the second value does not exists in the list, or if the types are incomparable 
	 */
	public boolean isValid( InformationState is )
	{
		/* Get the current Values of the 2 values. */
		Value value1, value2;
		try {
			value1 = abstractValue1.getValue(is);
			value2 = abstractValue2.getValue(is);
		} catch( TemplateRunException e ) {
			e.printStackTrace();
			return false;
		}
		
		if( value1 == null ) DMLogger.getLogger().logMissingISParameter(abstractValue1.getValueString());
		if( value2 == null ) DMLogger.getLogger().logMissingISParameter(abstractValue2.getValueString());
		
		/* Checks if the first value is a list */
		if( value1 != null && value1.getType() == Value.Type.List ) {
			List list = value1.getListValue();
			
			/* Based on the type, checks if the list contains the required value */
			if( value2.getType() == Value.Type.String ) {
				return list.contains(value2.getStringValue());
			} else if( value2.getType() == Value.Type.Integer ) {
				return list.contains(value2.getIntegerValue());
			} else if( value2.getType() == Value.Type.Double ) {
				return list.contains(value2.getDoubleValue());
			} else {
				/* The types of the list and the value are incomparable */
				return false;
			}
		} else {
			/* The first value is not a list */
			return false;
		}
	}
}
