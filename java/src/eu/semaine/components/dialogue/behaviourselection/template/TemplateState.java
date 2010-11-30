package eu.semaine.components.dialogue.behaviourselection.template;

import java.util.ArrayList;

import eu.semaine.components.dialogue.behaviourselection.template.behaviours.Behaviour;
import eu.semaine.components.dialogue.behaviourselection.template.effects.Effect;

/**
 * A TemplateState stores the current State of a Template.
 * It knows if all Compares, Indicators and Triggers are satisfied.
 * It knows the total number of Preconditions, and how many preconditions have not been satisfied.
 * And it stores a list of all Effects and the Behaviour that has to be executed when the Template is selected.
 * 
 * @author Mark ter Maat
 * @version 0.1
 */

public class TemplateState
{
	/* Stores which Preconditions have been satisfied */
	private boolean comparesSatisfied;
	private boolean indicatorsSatisfied;
	private boolean triggersSatisfied;
	
	/* Stores the total number of preconditions, and the number of unsatisfied preconditions */
	private int totalNrOfPreconditions = 0;
	private int nrOfMissingPreconditions = 0;
	
	/* Stores the corresponding Template, a list of all Effects, the Behaviour, and the quality of the Behaviour. */
	private Template template;
	private ArrayList<Effect> effects = new ArrayList<Effect>();
	private Behaviour behaviour = null;
	private double behaviourQuality;
	
	/**
	 * Creates a new TemplateState with the given parameters.
	 * 
	 * @param template - the original Template
	 * @param comparesSatisfied - if all Compares have been satisfied
	 * @param indicatorsSatisfied - if all Indicators have been satisfied
	 * @param triggersSatisfied - if all Triggers have been satisfied
	 * @param totalNrOfPreconditions - the total number of Preconditions
	 * @param nrOfMissingPreconditions - the number of unsatisfied Preconditions
	 */
	public TemplateState(Template template, boolean comparesSatisfied, 
			boolean indicatorsSatisfied, boolean triggersSatisfied,
			int totalNrOfPreconditions, int nrOfMissingPreconditions) {
		this.template = template;
		this.comparesSatisfied = comparesSatisfied;
		this.indicatorsSatisfied = indicatorsSatisfied;
		this.triggersSatisfied = triggersSatisfied;
		this.totalNrOfPreconditions = totalNrOfPreconditions;
		this.nrOfMissingPreconditions = nrOfMissingPreconditions;
	}
	
	/**
	 * @return the comparesSatisfied
	 */
	public boolean isComparesSatisfied() {
		return comparesSatisfied;
	}

	/**
	 * @return the indicatorsSatisfied
	 */
	public boolean isIndicatorsSatisfied() {
		return indicatorsSatisfied;
	}

	/**
	 * @return the triggersSatisfied
	 */
	public boolean isTriggersSatisfied() {
		return triggersSatisfied;
	}

	/**
	 * @return the totalNrOfPreconditions
	 */
	public int getTotalNrOfPreconditions() {
		return totalNrOfPreconditions;
	}

	/**
	 * @return the nrOfMissingPreconditions
	 */
	public int getNrOfMissingPreconditions() {
		return nrOfMissingPreconditions;
	}

	/**
	 * @return the effects
	 */
	public ArrayList<Effect> getEffects() {
		return effects;
	}

	/**
	 * @param effects the effects to set
	 */
	public void setEffects(ArrayList<Effect> effects) {
		this.effects = effects;
	}

	/**
	 * @return the behaviour
	 */
	public Behaviour getBehaviour() {
		return behaviour;
	}

	/**
	 * @param behaviour the behaviour to set
	 */
	public void setBehaviour(Behaviour behaviour) {
		this.behaviour = behaviour;
		behaviourQuality = behaviour.getQuality();
	}
	
	/**
	 * @param d - the quality to set
	 */
	public void setQuality( double d )
	{
		behaviourQuality = d;
	}
	
	/**
	 * @return the quality
	 */
	public double getQuality()
	{
		return behaviourQuality;
	}
	
	/**
	 * @return the Template
	 */
	public Template getTemplate()
	{
		return template;
	}
	
	
}
