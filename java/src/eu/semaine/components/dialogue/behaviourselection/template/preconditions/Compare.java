package eu.semaine.components.dialogue.behaviourselection.template.preconditions;

import org.w3c.dom.Element;

import eu.semaine.components.dialogue.behaviourselection.template.Template;
import eu.semaine.components.dialogue.behaviourselection.template.preconditions.compares.CompareContains;
import eu.semaine.components.dialogue.behaviourselection.template.preconditions.compares.CompareEquals;
import eu.semaine.components.dialogue.behaviourselection.template.preconditions.compares.CompareExists;
import eu.semaine.components.dialogue.behaviourselection.template.preconditions.compares.CompareGreaterEquals;
import eu.semaine.components.dialogue.behaviourselection.template.preconditions.compares.CompareGreaterThan;
import eu.semaine.components.dialogue.behaviourselection.template.preconditions.compares.CompareLesserEquals;
import eu.semaine.components.dialogue.behaviourselection.template.preconditions.compares.CompareLesserThan;
import eu.semaine.components.dialogue.behaviourselection.template.preconditions.compares.CompareNotContains;
import eu.semaine.components.dialogue.behaviourselection.template.preconditions.compares.CompareNotEquals;
import eu.semaine.components.dialogue.behaviourselection.template.preconditions.compares.CompareNotExists;
import eu.semaine.components.dialogue.behaviourselection.template.value.AbstractValue;
import eu.semaine.components.dialogue.exceptions.TemplateParseException;
import eu.semaine.components.dialogue.informationstate.InformationState;

/**
 * This is an abstract class of a Compare. Depending on what kind of comparison has to be made, a different subclass should be used.
 * 
 * @author Mark ter Maat
 * @version 0.1
 *
 */

public abstract class Compare extends Precondition
{
	/* The different comparators that can be used in a Compare */
	public enum Comparator {equals, not_equals, lesser_than, greater_than, lesser_equals, greater_equals, exists, not_exists, contains, not_contains };
	
	/* A Compare has 1 or 2 values, and a comparator. */
	protected AbstractValue abstractValue1;
	protected AbstractValue abstractValue2;
	protected Comparator comparator;
	
	/**
	 * Creates a new Compare with the given values and comparator.
	 * 
	 * @param value1 - the first value of the Compare
	 * @param value2 - the second value of the Compare (could be null)
	 * @param comparator - the comparator
	 * @throws TemplateParseException
	 */
	public Compare( String value1, String value2, Comparator comparator ) throws TemplateParseException
	{
		if( value1 != null ) this.abstractValue1 = new AbstractValue(value1);
		if( value2 != null ) this.abstractValue2 = new AbstractValue(value2);
		this.comparator = comparator;
	}
	
	/**
	 * Given the current InformationState, checks if this Precondition is valid.
	 * 
	 * @param is - the current InformationState
	 * @return 	- true if the precondition is valid
	 * 			- false if the precondition is not valid, or if an error occurred.
	 */
	public abstract boolean isValid( InformationState is );
	
	/**
	 * Given the DOM Compare-Element, returns the Precondition that fits the XML.
	 * This class will look at the type of comparator and calls the corresponding subclass.
	 * 
	 * @param compareElement - the DOM Element of this Compare
	 * @return the Precondition of the Compare
	 * @throws TemplateParseException
	 */
	public static Precondition parsePrecondition( Element compareElement ) throws TemplateParseException
	{
		if( compareElement.hasAttribute(Template.A_COMPARATOR) ) {
			String comp = compareElement.getAttribute(Template.A_COMPARATOR);
			
			if( comp.equals(Template.COMPARATOR_EQUALS) ) {
				if( compareElement.hasAttribute(Template.A_VALUE1) && compareElement.hasAttribute(Template.A_VALUE2) ) {
					return new CompareEquals( compareElement.getAttribute(Template.A_VALUE1), compareElement.getAttribute(Template.A_VALUE2) );
				} else {
					throw new TemplateParseException("Wrong number of arguments of a Compare (Equals).");
				}
			}
			if( comp.equals(Template.COMPARATOR_NOTEQUALS) ) {
				if( compareElement.hasAttribute(Template.A_VALUE1) && compareElement.hasAttribute(Template.A_VALUE2) ) {
					return new CompareNotEquals( compareElement.getAttribute(Template.A_VALUE1), compareElement.getAttribute(Template.A_VALUE2) );
				} else {
					throw new TemplateParseException("Wrong number of arguments of a Compare (Not_Equals).");
				}
			}
			if( comp.equals(Template.COMPARATOR_GREATERTHAN) ) {
				if( compareElement.hasAttribute(Template.A_VALUE1) && compareElement.hasAttribute(Template.A_VALUE2) ) {
					return new CompareGreaterThan( compareElement.getAttribute(Template.A_VALUE1), compareElement.getAttribute(Template.A_VALUE2) );
				} else {
					throw new TemplateParseException("Wrong number of arguments of a Compare (Greater_Than).");
				}
			}
			if( comp.equals(Template.COMPARATOR_LESSERTHAN) ) {
				if( compareElement.hasAttribute(Template.A_VALUE1) && compareElement.hasAttribute(Template.A_VALUE2) ) {
					return new CompareLesserThan( compareElement.getAttribute(Template.A_VALUE1), compareElement.getAttribute(Template.A_VALUE2) );
				} else {
					throw new TemplateParseException("Wrong number of arguments of a Compare (Lesser_Than).");
				}
			}
			if( comp.equals(Template.COMPARATOR_GREATEREQUALS) ) {
				if( compareElement.hasAttribute(Template.A_VALUE1) && compareElement.hasAttribute(Template.A_VALUE2) ) {
					return new CompareGreaterEquals( compareElement.getAttribute(Template.A_VALUE1), compareElement.getAttribute(Template.A_VALUE2) );
				} else {
					throw new TemplateParseException("Wrong number of arguments of a Compare (Greater_Equals).");
				}
			}
			if( comp.equals(Template.COMPARATOR_LESSEREQUALS) ) {
				if( compareElement.hasAttribute(Template.A_VALUE1) && compareElement.hasAttribute(Template.A_VALUE2) ) {
					return new CompareLesserEquals( compareElement.getAttribute(Template.A_VALUE1), compareElement.getAttribute(Template.A_VALUE2) );
				} else {
					throw new TemplateParseException("Wrong number of arguments of a Compare (Lesser_Equals).");
				}
			}
			if( comp.equals(Template.COMPARATOR_NOTEQUALS) ) {
				if( compareElement.hasAttribute(Template.A_VALUE1) && compareElement.hasAttribute(Template.A_VALUE2) ) {
					return new CompareNotEquals( compareElement.getAttribute(Template.A_VALUE1), compareElement.getAttribute(Template.A_VALUE2) );
				} else {
					throw new TemplateParseException("Wrong number of arguments of a Compare (Not_Equals).");
				}
			}
			if( comp.equals(Template.COMPARATOR_EXISTS) ) {
				if( compareElement.hasAttribute(Template.A_VALUE1) ) {
					return new CompareExists( compareElement.getAttribute(Template.A_VALUE1) );
				} else {
					throw new TemplateParseException("Wrong number of arguments of a Compare (Exists).");
				}
			}
			if( comp.equals(Template.COMPARATOR_NOTEXISTS) ) {
				if( compareElement.hasAttribute(Template.A_VALUE1) ) {
					return new CompareNotExists( compareElement.getAttribute(Template.A_VALUE1) );
				} else {
					throw new TemplateParseException("Wrong number of arguments of a Compare (Not_Exists).");
				}
			}
			if( comp.equals(Template.COMPARATOR_CONTAINS) ) {
				if( compareElement.hasAttribute(Template.A_VALUE1) && compareElement.hasAttribute(Template.A_VALUE2) ) {
					return new CompareContains( compareElement.getAttribute(Template.A_VALUE1), compareElement.getAttribute(Template.A_VALUE2) );
				} else {
					throw new TemplateParseException("Wrong number of arguments of a Compare (Contains).");
				}
			}
			if( comp.equals(Template.COMPARATOR_NOTCONTAINS) ) {
				if( compareElement.hasAttribute(Template.A_VALUE1) && compareElement.hasAttribute(Template.A_VALUE2) ) {
					return new CompareNotContains( compareElement.getAttribute(Template.A_VALUE1), compareElement.getAttribute(Template.A_VALUE2) );
				} else {
					throw new TemplateParseException("Wrong number of arguments of a Compare (Not_Contains).");
				}
			}
			throw new TemplateParseException("Unknown Comparator used.");
		} else {
			if( compareElement.hasAttribute(Template.A_VALUE1) && compareElement.hasAttribute(Template.A_VALUE2) ) {
				return new CompareEquals( compareElement.getAttribute(Template.A_VALUE1), compareElement.getAttribute(Template.A_VALUE2) );
			} else {
				throw new TemplateParseException("Wrong number of arguments of a Compare.");
			}
		}
	}
}
