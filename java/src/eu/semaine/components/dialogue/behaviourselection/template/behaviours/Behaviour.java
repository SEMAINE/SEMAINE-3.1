package eu.semaine.components.dialogue.behaviourselection.template.behaviours;

import eu.semaine.components.dialogue.informationstate.InformationState;

import java.util.ArrayList;

import org.w3c.dom.Element;
import org.w3c.dom.NodeList;

import eu.semaine.components.dialogue.exceptions.TemplateParseException;
import eu.semaine.components.dialogue.exceptions.TemplateRunException;

import eu.semaine.components.dialogue.actionproposers.RandomUtteranceActionProposer;
import eu.semaine.components.dialogue.actionproposers.UtteranceActionProposer;
import eu.semaine.components.dialogue.behaviourselection.behaviours.BehaviourClass;
import eu.semaine.components.dialogue.behaviourselection.template.Template;
import eu.semaine.components.dialogue.behaviourselection.template.value.AbstractValue;
import eu.semaine.components.dialogue.behaviourselection.template.value.Value;

/**
 * This class contains information about a Behaviour.
 * 
 * @author Mark ter Maat
 * @version 0.1
 *
 */

public class Behaviour
{
	/* The name of the BehaviourClass */
	private String behaviourClassName;
	
	/* The quality-value of this Behaviour */
	private double quality;

	/* The BehaviourClass that is used for this Behaviour */
	private BehaviourClass behaviour;
	/* The argument-names and values to supply the BehaviourClass when called */
	private ArrayList<String> argNames = new ArrayList<String>();
	private ArrayList<AbstractValue> argValues = new ArrayList<AbstractValue>();

	/**
	 * Creates a new Behaviour with the given BehaviourClass-name and quality-value.
	 * 
	 * @param behaviourClassName - the name of the corresponding BehaviourClass
	 * @param quality - the quality-value of this Behaviour
	 * @throws TemplateParseException
	 */
	public Behaviour( String behaviourClassName, String quality ) throws TemplateParseException
	{
		this.behaviourClassName = behaviourClassName;
		behaviour = getBehaviourClass(behaviourClassName);
		try {
			this.quality = Double.parseDouble(quality);
		}catch(NumberFormatException e) {
			throw new TemplateParseException("Quality-number '"+quality+"' could not be parsed to a Double.");
		}
	}

	/**
	 * Adds an argument with the given name and value to the list of arguments of this Behaviour.
	 * 
	 * @param name - the name of the new argument
	 * @param value - the value of the new argument
	 * @throws TemplateParseException
	 */
	public void addArg( String name, String value ) throws TemplateParseException
	{
		argNames.add(name);
		argValues.add(new AbstractValue(value));
	}

	/**
	 * Given the name of the BehaviourClass, tries to find the Class-file of this class, and returns a new instance.
	 * 
	 * @param name - the name of the BehaviourClass to find.
	 * @return a new instance of this BehaviourClass.<
	 * @throws TemplateParseException
	 */
	public BehaviourClass getBehaviourClass( String name )
	{
		if( name.equals("ResponseActionProposer") ) {
			BehaviourClass c = UtteranceActionProposer.getMyClass();
			if( c != null ) {
				return c;
			} else {
				c = RandomUtteranceActionProposer.getMyClass();
				if( c != null ) {
					return c;
				} else {
					return null;
				}
			}
		} else {
			return null;
		}
	}

	/**
	 * Given the current InformationState, calculate the current values of the arguments, and call
	 * the execute() method of the BehaviourClass with the list of arguments.
	 * 
	 * @param is - the current InformationState
	 * @throws TemplateRunException
	 */
	public void execute( InformationState is ) throws TemplateRunException
	{
		ArrayList<String> values = new ArrayList<String>();
		for( AbstractValue av : argValues) {
			Value v = av.getValue(is);
			values.add(v.toString());
		}
		if( behaviour == null ) behaviour = getBehaviourClass(behaviourClassName);
		behaviour.execute(is,(ArrayList<String>)argNames.clone(), (ArrayList<String>)values.clone());
	}
	
	/**
	 * Given the current InformationState, calculate the current values of the arguments, and call
	 * the prepare() method of the BehaviourClass with the list of arguments.
	 * 
	 * @param is - the current InformationState
	 * @throws TemplateRunException
	 */
	public void prepare( InformationState is ) throws TemplateRunException
	{
		ArrayList<String> values = new ArrayList<String>();
		for( AbstractValue av : argValues) {
			Value v = av.getValue(is);
			values.add(v.toString());
		}
		if( behaviour == null ) behaviour = getBehaviourClass(behaviourClassName);
		behaviour.prepare(is,(ArrayList<String>)argNames.clone(), (ArrayList<String>)values.clone());
	}
	
	/**
	 * Returns the quality-value of this Behaviour
	 * @return quality
	 */
	public double getQuality()
	{
		return quality;
	}

	/**
	 * @return the argNames
	 */
	public ArrayList<String> getArgNames() {
		return argNames;
	}

	/**
	 * @return the argValues
	 */
	public ArrayList<AbstractValue> getArgValues() {
		return argValues;
	}

	/**
	 * Given the DOM Behaviour-Element, returns the Behaviour that fits the XML.
	 * 
	 * @param behaviourElement - the DOM Element of this Behaviour
	 * @return the Behaviour
	 * @throws TemplateParseException
	 */
	public static Behaviour parseBehaviour( Element behaviourElement, boolean inChoiceElement ) throws TemplateParseException
	{
		if( !behaviourElement.hasAttribute(Template.A_BEHAVIOURCLASS) ) {
			throw new TemplateParseException("Missing Behaviourclass in Behaviour-Element.");
		}
		String behaviourClass = behaviourElement.getAttribute(Template.A_BEHAVIOURCLASS);
		String quality = behaviourElement.getAttribute(Template.A_QUALITY);
		if( quality.length() == 0 ) {
			quality = "0.5";
		}
		Behaviour behaviour = new Behaviour(behaviourClass,quality);

		NodeList argList;
		if(inChoiceElement) {
			argList = behaviourElement.getElementsByTagName(Template.E_CARGUMENT);
		} else {
			argList = behaviourElement.getElementsByTagName(Template.E_ARGUMENT);
		}
		for( int i=0; i<argList.getLength(); i++ ) {
			Element argElement = (Element)argList.item(i);
			if( !argElement.hasAttribute(Template.A_ARGUMENTNAME) && !argElement.hasAttribute(Template.A_ARGUMENTVALUE) ) {
				throw new TemplateParseException("Missing Name or Value attribute in Behaviour-argument.");
			}
			behaviour.addArg(argElement.getAttribute(Template.A_ARGUMENTNAME), argElement.getAttribute(Template.A_ARGUMENTVALUE));
		}
		return behaviour;
	}
}
