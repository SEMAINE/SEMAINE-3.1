package eu.semaine.components.dialogue.behaviourselection.template.effects;

import eu.semaine.components.dialogue.informationstate.InformationState;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.ArrayList;

import org.w3c.dom.Element;
import org.w3c.dom.NodeList;

import eu.semaine.components.dialogue.exceptions.TemplateParseException;
import eu.semaine.components.dialogue.exceptions.TemplateRunException;

import eu.semaine.components.dialogue.behaviourselection.TemplateController;
import eu.semaine.components.dialogue.behaviourselection.template.Template;
import eu.semaine.components.dialogue.behaviourselection.template.value.AbstractValue;
import eu.semaine.components.dialogue.behaviourselection.template.value.Value;

/**
 * A Function is a type of Effect. When applied, it will search for a method with the correct name and number of arguments and execute this.
 * 
 * @author Mark ter Maat
 * @version 0.1
 */

public class Function extends Effect
{
	/* The name of the method to execute */
	private String name;
	/* The list of arguments to supply the method */
	private ArrayList<AbstractValue> arguments = new ArrayList<AbstractValue>();
	
	/**
	 * Creates a new Function with the given name and arguments
	 * 
	 * @param name - the name of the method to execute
	 * @param arguments - a list of arguments, divided by comma's.
	 * @throws TemplateParseException
	 */
	public Function( String name ) throws TemplateParseException
	{
		this.name = name;
	}
	
	/**
	 * Adds an argument with the given value to the list of arguments of this Function.
	 * 
	 * @param value - the value of the new argument
	 * @throws TemplateParseException
	 */
	public void addArg( String value ) throws TemplateParseException
	{
		arguments.add(new AbstractValue(value));
	}
	
	/**
	 * Given the current InformationState, and the TemplateController, retrieve a list of classes which
	 * contain user-specified functions from the TemplateController, search these classes for a method
	 * with the correct name and number of arguments, and execute this function the the current values
	 * of the arguments.
	 * 
	 * @param is - the current InformationState
	 * @param controller - the TemplateController than manages the Template containing this Update.
	 * @throws TemplateRunException
	 */
	public void apply( InformationState is, TemplateController tc ) throws TemplateRunException
	{
		/* Get a list of all classes containing user-specified functions */
		ArrayList<Object> functionClasses = tc.getFunctionClasses();
		
		for( Object object : functionClasses ) {
			Class c = object.getClass();
			
			for( Method method : c.getMethods() ) {
				/* Check all methods of all classes */
				if( method.getName().equals(name) ) {
					/* Make a list with the current values of the arguments */
					ArrayList<Value> argValues = new ArrayList<Value>();
					for( AbstractValue av : arguments ) {
						argValues.add(av.getValue(is));
					}
					String[] args = new String[argValues.size()];
					for( int i=0; i<argValues.size(); i++ ) {
						args[i] = argValues.get(i).toString();
					}
					
					/* Execute the function with the current values of the arguments */
					try {
						method.invoke(object, args);
						return;
					}catch( IllegalAccessException e ) {
						throw new TemplateRunException("Error while calling function '"+name+"'.");
					}catch( InvocationTargetException e ) {
						throw new TemplateRunException("Error while calling function '"+name+"'.");
					}catch( IllegalArgumentException e ) {
						throw new TemplateRunException("Error while calling function '"+name+"'.");
					}
				}
			}
		}
		throw new TemplateRunException("Could not find function '"+name+"'.");
	}
	
	/**
	 * Given the DOM Function-Element, returns the Effect that fits the XML.
	 * This class creates a new Function and returns this.
	 * 
	 * @param functionElement - the DOM Element of this Function
	 * @return the Effect of the Function
	 * @throws TemplateParseException
	 */
	public static Function parseFunction( Element functionElement, boolean inChoiceElement ) throws TemplateParseException
	{
		if( !functionElement.hasAttribute(Template.A_EFFECTVARNAME) && !functionElement.hasAttribute(Template.A_EFFECTVARVALUE) ) {
			throw new TemplateParseException("Missing Name or Value attribute in Function-rule.");
		}
		Function function = new Function( functionElement.getAttribute(Template.A_EFFECTVARNAME) );
		
		NodeList argList;
		if(inChoiceElement) {
			argList = functionElement.getElementsByTagName(Template.E_CARGUMENT);
		} else {
			argList = functionElement.getElementsByTagName(Template.E_ARGUMENT);
		}
		for( int i=0; i<argList.getLength(); i++ ) {
			Element argElement = (Element)argList.item(i);
			if( !argElement.hasAttribute(Template.A_ARGUMENTVALUE) ) {
				throw new TemplateParseException("Missing Value attribute in Function-argument.");
			}
			function.addArg(argElement.getAttribute(Template.A_ARGUMENTVALUE));
		}
		
		return function;
	}
}
