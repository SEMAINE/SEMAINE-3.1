package eu.semaine.components.dialogue.behaviourselection.template.effects;

import eu.semaine.components.dialogue.behaviourselection.TemplateController;
import eu.semaine.components.dialogue.exceptions.TemplateRunException;
import eu.semaine.components.dialogue.informationstate.InformationState;

/**
 * This is an abstract class of an Effect. Depending on what kind of effect is required, a different subclass should be used.
 * 
 * @author Mark ter Maat
 * @version 0.1
 *
 */

public abstract class Effect
{
	/**
	 * Given the current InformationState, and the TemplateController, apply the effects.
	 * 
	 * @param is
	 * @param controller
	 * @throws TemplateRunException
	 */
	public abstract void apply( InformationState is, TemplateController controller ) throws TemplateRunException;
}
