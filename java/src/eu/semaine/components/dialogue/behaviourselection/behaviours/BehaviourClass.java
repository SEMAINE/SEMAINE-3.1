package eu.semaine.components.dialogue.behaviourselection.behaviours;

import java.util.ArrayList;

import eu.semaine.components.dialogue.informationstate.InformationState;

public interface BehaviourClass
{	
	public abstract void execute( InformationState is, ArrayList<String> argNames, ArrayList<String> argValues );
	public abstract boolean prepare( InformationState is, ArrayList<String> argNames, ArrayList<String> argValues );
}
