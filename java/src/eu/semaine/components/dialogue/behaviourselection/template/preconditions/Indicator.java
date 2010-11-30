package eu.semaine.components.dialogue.behaviourselection.template.preconditions;

import java.util.ArrayList;

import org.w3c.dom.Element;
import org.w3c.dom.NodeList;

import eu.semaine.components.dialogue.behaviourselection.template.Template;
import eu.semaine.components.dialogue.behaviourselection.template.value.AbstractValue;
import eu.semaine.components.dialogue.behaviourselection.template.value.Value;
import eu.semaine.components.dialogue.exceptions.TemplateParseException;
import eu.semaine.components.dialogue.exceptions.TemplateRunException;
import eu.semaine.components.dialogue.informationstate.InformationState;

/**
 * The Indicator is a type of Precondition, and checks if there is enough proof (enough indication) for a certain feature.
 * This proof is calculated by checking a number of Compares+, and each check can modify the value of the proof.
 * 
 * @author Mark ter Maat
 * @version 0.1
 *
 */

public class Indicator extends Precondition
{
	/* An indicator has a starting-value for the proof, a target-value for this proof, and a comparator */
	private AbstractValue abstractStartValue;
	private AbstractValue abstractEndValue;
	private String comparator;
	
	/* The list of Compares of this Indicator */
	public ArrayList<ICompare> indicatorCompares = new ArrayList<ICompare>();
	
	/**
	 * Creates a new Indicator with the given values and comparator.
	 * 
	 * @param startValue - the starting-value of the proof
	 * @param endValue - the target-value of the proof
	 * @param comparator - the comparator
	 * @throws TemplateParseException
	 */
	public Indicator( String startValue, String endValue, String comparator ) throws TemplateParseException
	{
		if( startValue != null && endValue != null && comparator != null ) {
			this.abstractStartValue = new AbstractValue(startValue);
			this.abstractEndValue = new AbstractValue(endValue);
			this.comparator = comparator;
		} else {
			throw new TemplateParseException("Incorrect number of arguments of Indicator.");
		}
	}
	
	/**
	 * Adds a Compare to the list of Compares.
	 * @param compare
	 */
	public void addCompare( ICompare compare )
	{
		indicatorCompares.add(compare);
	}
	
	/**
	 * Given the current InformationState, checks if this indicator is valid.
	 * 
	 * It does this by calculating a proof-value. The starting-value is given by abstractStartValue.
	 * Then it goes through all Compares, and for each Compare that is valid it will apply the modifier 
	 * of that compare (for example '+3') to the proof-value. In the end it will use the Comparator
	 * to verify if the proof-value and the given end-value match.
	 */
	public boolean isValid( InformationState is )
	{
		/* Get the Value of startValue and endValue */
		Value startValue, endValue;
		try {
			startValue = abstractStartValue.getValue(is);
			endValue = abstractEndValue.getValue(is);
		} catch( TemplateRunException e ) {
			e.printStackTrace();
			return false;
		}
		
		/* Checks if the types are comparable, or return 'false' */
		if( (startValue.getType() != Value.Type.Integer && startValue.getType() != Value.Type.Double)
				|| (endValue.getType() != Value.Type.Integer && endValue.getType() != Value.Type.Double)) {
			return false;
		}
		
		/* Get the numerical value of startValue and endValue */
		double startV, endV;
		if( startValue.getType() == Value.Type.Integer ) {
			startV = startValue.getIntegerValue().doubleValue();
		} else {
			startV = startValue.getDoubleValue();
		}
		if( endValue.getType() == Value.Type.Integer ) {
			endV = endValue.getIntegerValue().doubleValue();
		} else {
			endV = endValue.getDoubleValue();
		}
		
		/* Check all Compares of this indicator, and modify the proof-value accordingly. */
		for( ICompare compare : indicatorCompares ) {
			if( compare.isValid(is) ) {
				try {
					startV = calc( startV, compare.getModifier() );
				} catch( TemplateRunException e ) {
					e.printStackTrace();
					return false;
				}
			}
		}
		
		/* given the comparator, check if the current proof-value matches the endValue */
		if( comparator.equals(Template.COMPARATOR_EQUALS) ) {
			return (startV == endV);
		} else if(comparator.equals(Template.COMPARATOR_NOTEQUALS)) {
			return (startV != endV);
		} else if(comparator.equals(Template.COMPARATOR_GREATERTHAN)) {
			return (startV > endV);
		} else if(comparator.equals(Template.COMPARATOR_GREATEREQUALS)) {
			return (startV >= endV);
		} else if(comparator.equals(Template.COMPARATOR_LESSERTHAN)) {
			return (startV < endV);
		} else if(comparator.equals(Template.COMPARATOR_LESSEREQUALS)) {
			return (startV <= endV);
		} else {
			try {
				throw new TemplateRunException("Illegal comparator in Indicator.");
			} catch( TemplateRunException e ) {
				e.printStackTrace();
				return false;
			}
		}
	}
	
	/**
	 * Given the current proof-value and a modifier, calculate the new proof-value and return this.
	 * 
	 * @param d - the current proof-value
	 * @param modifier - a string-representation of the modifier.
	 * @return the new proof-value
	 * @throws TemplateRunException
	 */
	public double calc( double d, String modifier ) throws TemplateRunException
	{
		char sign = modifier.charAt(0);
		double modValue;
		try {
			modValue = Double.parseDouble(modifier.substring(1));
		}catch( NumberFormatException e ) {
			throw new TemplateRunException("Modifier of Indicator-comparison does not contain a correct value after the arithmetic operator.");
		}
		if( sign != '+' && sign != '-' && sign != '*' && sign != '/' ) {
			throw new TemplateRunException("Modifier of Indicator-comparison does not contain a correct arithmetic operator.");
		}
		if( sign == '+' ) {
			return d + modValue;
		} else if( sign == '-' ) {
			return d - modValue;
		} else if( sign == '*' ) {
			return d * modValue;
		} else { // sign == '/'
			return d / modValue;
		}
	}
	
	/**
	 * Given the DOM Indicator-Element, returns the Precondition that fits the XML.
	 * This class creates a new Indicator, parses all containing Compares and adds them to the Indicator.
	 * 
	 * @param compareElement - the DOM Element of this Indicator
	 * @return the Precondition of the Indicator
	 * @throws TemplateParseException
	 */
	public static Indicator parseIndicator( Element indicatorElement ) throws TemplateParseException
	{
		if( !indicatorElement.hasAttribute(Template.A_INDICATORSTARTVALUE) || !indicatorElement.hasAttribute(Template.A_INDICATORENDVALUE) || !indicatorElement.hasAttribute(Template.A_COMPARATOR) ) {
			throw new TemplateParseException("Missing attributes of Indicator element.");
		}
		Indicator indicator = new Indicator(indicatorElement.getAttribute(Template.A_INDICATORSTARTVALUE),indicatorElement.getAttribute(Template.A_INDICATORENDVALUE),indicatorElement.getAttribute(Template.A_COMPARATOR) );
		
		NodeList icompareList = indicatorElement.getElementsByTagName(Template.E_INDICATORCOMPARE);
		for( int i=0; i<icompareList.getLength(); i++ ) {
			Element icompare = (Element)icompareList.item(i);
			indicator.addCompare(ICompare.parseICompare(icompare));
		}
		
		return indicator;
	}
}
