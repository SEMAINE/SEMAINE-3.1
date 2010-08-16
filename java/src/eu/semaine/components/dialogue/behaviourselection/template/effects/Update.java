package eu.semaine.components.dialogue.behaviourselection.template.effects;

import org.w3c.dom.Element;

import eu.semaine.components.dialogue.informationstate.InformationState;
import eu.semaine.components.dialogue.informationstate.Item;
import eu.semaine.components.dialogue.exceptions.TemplateParseException;
import eu.semaine.components.dialogue.exceptions.TemplateRunException;
import eu.semaine.components.dialogue.behaviourselection.TemplateController;
import eu.semaine.components.dialogue.behaviourselection.template.Template;
import eu.semaine.components.dialogue.behaviourselection.template.value.AbstractValue;
import eu.semaine.components.dialogue.behaviourselection.template.value.Value;

/**
 * An Update is a type of Effect. When applied, it will modify the a value in InformationState.
 * 
 * @version 0.1.1
 * Added support for the removal of IS-elements.
 * 
 * @version 0.1
 * First version
 * 
 * @author Mark ter Maat
 *
 */

public class Update extends Effect
{
	/* The name of the IS-value to modify. */
	private String name;
	/* The new value */
	private AbstractValue abstractValue;
	
	/**
	 * Creates a new Update with the given variable name and value.
	 * 
	 * @param name - the name of the IS-value to modify.
	 * @param valueString - the new value of the IS-value
	 * @throws TemplateParseException
	 */
	public Update( String name, String valueString ) throws TemplateParseException
	{
		this.name = name;
		if( valueString != null && valueString.length() > 0 ) {
			this.abstractValue = new AbstractValue(valueString);
		} else {
			abstractValue = null;
		}
	}
	
	/**
	 * Given the current InformationState, and the TemplateController, find the value with the correct 
	 * name in the IS and modify it accordingly.
	 * 
	 * @param is - the current InformationState
	 * @param controller - the TemplateController than manages the Template containing this Update.
	 * @throws TemplateRunException
	 */
	public void apply( InformationState is, TemplateController tc ) throws TemplateRunException
	{
		if( abstractValue == null ) {
			is.remove(name);
		} else {
		
			/* Find the IS-item with the correct name */
//			Item item = is.getValueOfPath(name);
//			if( item == null ) {
//				throw new TemplateRunException("Unknown IS-variable '"+name+"' in Update.");
//			}
			
			/* Calculate the new Value */
			Value value = abstractValue.getValue(is);
			if( value == null ) {
				throw new TemplateRunException("Error calculating new Value of '"+name+"'.");
			}
			
			Item.Type type = is.getTypeOfPath(name);
			if( type == null ) {
				if( value.getType() == Value.Type.Double ) {
					is.set(name, value.getDoubleValue());
				} else if( value.getType() == Value.Type.Integer ) {
					is.set(name, value.getIntegerValue());
				} else if( value.getType() == Value.Type.String ) {
					is.set(name, value.getStringValue());
				}
				return;
			}
			/* Verify that the types of the new Value and the IS-value match */
			if( !type.toString().equals(value.getType().toString()) ) {
				if( (type.toString().equals("Integer") && value.getType().toString().equals("Double")) ||
					(type.toString().equals("Double") && value.getType().toString().equals("Integer"))	) {
					// Nothing wrong.
				} else {
					throw new TemplateRunException("Non-matching types of value of '"+name+"'.");
				}
			}
			
			/* Set the new value */
			if( type == Item.Type.String ) {
				is.set(name, value.getStringValue());
				//item.setStringValue(value.getStringValue());
			} else if( type == Item.Type.Double ) {
				if( value.getType() == Value.Type.Double ) {
					is.set(name, value.getDoubleValue());
					//item.setDoubleValue(value.getDoubleValue());
				} else if( value.getType() == Value.Type.Integer ) {
					is.set(name, value.getIntegerValue());
					//item.setDoubleValue(new Double(value.getIntegerValue()));
				}
			} else if( type == Item.Type.Integer ) {
				if( value.getType() == Value.Type.Integer ) {
					is.set(name, value.getIntegerValue());
					//item.setIntegerValue(value.getIntegerValue());
				} else if( value.getType() == Value.Type.Double ) {
					is.set(name, value.getDoubleValue());
					//item.setIntegerValue(new Double(value.getDoubleValue()).intValue());
				}
				
			} else {
				throw new TemplateRunException("Non-atomic value of update.");
			}
		}
	}
	
	/**
	 * Returns the name of variable that has to be changed.
	 * @return name
	 */
	public String getName()
	{
		return name;
	}
	
	/**
	 * Given the DOM Update-Element, returns the Effect that fits the XML.
	 * This class creates a new Update and returns this.
	 * 
	 * @param updateElement - the DOM Element of this Update
	 * @return the Effect of the Update
	 * @throws TemplateParseException
	 */
	public static Effect parseUpdate( Element updateElement ) throws TemplateParseException
	{
		if( !updateElement.hasAttribute(Template.A_EFFECTVARNAME) ) {
			throw new TemplateParseException("Missing Name attribute in Update-rule.");
		}
		Update update = new Update( updateElement.getAttribute(Template.A_EFFECTVARNAME), updateElement.getAttribute(Template.A_EFFECTVARVALUE) );
		return update;
	} 
}
