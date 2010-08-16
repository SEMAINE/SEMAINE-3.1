package eu.semaine.components.dialogue.behaviourselection.template.preconditions.compares;

import eu.semaine.components.dialogue.behaviourselection.template.preconditions.Compare;
import eu.semaine.components.dialogue.behaviourselection.template.value.Value;
import eu.semaine.components.dialogue.exceptions.TemplateParseException;
import eu.semaine.components.dialogue.exceptions.TemplateRunException;
import eu.semaine.components.dialogue.informationstate.InformationState;

/**
 * A CompareGreaterThan checks if the first value is greater than the second value.
 * Only compares Integers and Doubles, and returns true if the first value is greater than the second value, 
 * and false if this is not the case or if the two values have incomparable types.
 * 
 * @author Mark ter Maat
 * @version 0.1
 *
 */

public class CompareGreaterThan extends Compare
{
	/**
	 * Creates a new CompareGreaterThan with the given values.
	 * 
	 * @param value1
	 * @param value2
	 * @throws TemplateParseException
	 */
	public CompareGreaterThan( String value1, String value2 ) throws TemplateParseException
	{
		super(value1, value2, Compare.Comparator.greater_than);
	}
	
	/**
	 * Given the current InformationState, checks if the the first value is greater than the second value.
	 * 
	 * @param is - the current InformationState
	 * @returns	true  - if the first value is greater than the second value
	 * 			false - if the second value is greater or equals to the first, or of incomparable types 
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
			return false;
		}
		
		/* Compare the values based on the types of the Values */
		if( value1.getType() == Value.Type.Integer && value2.getType() == Value.Type.Integer ) {
			return( value1.getIntegerValue() > value2.getIntegerValue() );
		}
		if( value1.getType() == Value.Type.Integer && value2.getType() == Value.Type.Double ) {
			return ( new Double(value1.getIntegerValue()) > value2.getDoubleValue() );
		}
		if( value1.getType() == Value.Type.Double && value2.getType() == Value.Type.Integer ) {
			return ( value1.getDoubleValue() > new Double(value2.getIntegerValue()) );
		}
		if( value1.getType() == Value.Type.Double && value2.getType() == Value.Type.Double ) {
			return ( value1.getDoubleValue() > value2.getDoubleValue() );
		}
		return false;
	}
}
