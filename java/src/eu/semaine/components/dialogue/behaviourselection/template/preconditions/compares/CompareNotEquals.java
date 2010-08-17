package eu.semaine.components.dialogue.behaviourselection.template.preconditions.compares;

import eu.semaine.components.dialogue.behaviourselection.template.preconditions.Compare;
import eu.semaine.components.dialogue.behaviourselection.template.value.Value;
import eu.semaine.components.dialogue.exceptions.TemplateParseException;
import eu.semaine.components.dialogue.exceptions.TemplateRunException;
import eu.semaine.components.dialogue.informationstate.InformationState;
import eu.semaine.components.dialogue.test.DMLogger;

/**
 * A CompareNotEquals checks if two values are not equal. Returns true if they are not equal, or false if they are or if they are of incomparable types.
 * 
 * @author Mark ter Maat
 * @version 0.1
 *
 */

public class CompareNotEquals extends Compare
{
	/**
	 * Creates a new CompareNotEquals with the given values.
	 * 
	 * @param value1
	 * @param value2
	 * @throws TemplateParseException
	 */
	public CompareNotEquals( String value1, String value2 ) throws TemplateParseException
	{
		super(value1, value2, Compare.Comparator.not_equals);
	}
	
	/**
	 * Given the current InformationState, checks if the 2 values are not equal.
	 * 
	 * @param is - the current InformationState
	 * @returns	true  - if the 2 values are not equal
	 * 			false - if the 2 values are equal or of incomparable types 
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
		
		/* If one of the Values is null, return false; */
		if( value1 == null || value2 == null ) {
			if( value1 == null ) DMLogger.getLogger().logMissingISParameter(abstractValue1.getValueString());
			if( value2 == null ) DMLogger.getLogger().logMissingISParameter(abstractValue2.getValueString());
			return false;
		}
		
		/* Check for equality based on the types of the Values */
		if( value1.getType() == Value.Type.String && value2.getType() == Value.Type.String ) {
			return ( ! value1.getStringValue().equals(value2.getStringValue()));
		}
		if( value1.getType() == Value.Type.Integer && value2.getType() == Value.Type.Integer ) {
			return ( ! value1.getIntegerValue().equals(value2.getIntegerValue()));
		}
		if( value1.getType() == Value.Type.Integer && value2.getType() == Value.Type.Double ) {
			return ( ! new Double(value1.getIntegerValue()).equals(value2.getDoubleValue()) );
		}
		if( value1.getType() == Value.Type.Double && value2.getType() == Value.Type.Integer ) {
			return ( ! value1.getDoubleValue().equals(new Double(value2.getIntegerValue())) );
		}
		if( value1.getType() == Value.Type.Double && value2.getType() == Value.Type.Double ) {
			return ( ! value1.getDoubleValue().equals(value2.getDoubleValue()) );
		}
		
		/* If this point is reached then the 2 values are of incomparable types, so return false.*/
		return false;
	}
}
