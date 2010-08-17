package eu.semaine.components.dialogue.behaviourselection;

import eu.semaine.components.dialogue.informationstate.InformationState;
import eu.semaine.components.dialogue.test.DMLogger;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;

import javax.xml.parsers.ParserConfigurationException;

import org.xml.sax.SAXException;

import eu.semaine.components.dialogue.exceptions.TemplateRunException;

import eu.semaine.components.dialogue.behaviourselection.template.Template;
import eu.semaine.components.dialogue.behaviourselection.template.TemplateState;
import eu.semaine.components.dialogue.behaviourselection.template.behaviours.Behaviour;
import eu.semaine.components.dialogue.behaviourselection.template.effects.Effect;
import eu.semaine.components.dialogue.behaviourselection.template.effects.Update;

/**
 * The TemplateController, as the name suggests, controlls Templates.
 * It is given 1 or more TemplateFiles, uses this to create a list of Templates, and when asked (with a given InformationState),
 * it checks all Templates and selects a Template to execute.
 * 
 * It will only execute 1 Template which has a Behaviour, but Templates without Behaviours (InformationState updates),
 * are always executed.
 * 
 * @author Mark ter Maat
 * @version 0.1
 *
 */

public class TemplateController
{
	/* The TemplateParser to parse Template-files */
	private TemplateParser templateParser = new TemplateParser();
	
	/* A list of classes which contain user-specified functions */
	private ArrayList<Object> functionClasses = new ArrayList<Object>();
	
	/* A list with all Templates it knows */
	private ArrayList<Template> templates = new ArrayList<Template>();
	
	/* A list with recently executed Templates with Behaviour */
	private ArrayList<String> recentRunTemplates = new ArrayList<String>();
	
	/**
	 * Creates a new TemplateController
	 */
	public TemplateController()
	{
	}
	
	/**
	 * Processes the given TemplateFile, asks the parser to Parse it, and adds the returned list of Templates to its own list.
	 * 
	 * @param templateFile - the Template-filename
	 */
	public void processTemplateFile( String templateFile )
	{
		if( templateFile.length() == 0 ) return;
		try {
			templates.addAll(templateParser.parseFile(templateFile));
		}catch( ParserConfigurationException e ) {
			e.printStackTrace();
			// TODO: Handle exception
		}catch( SAXException e ) {
			e.printStackTrace();
			// TODO: Handle exception
		}catch( IOException e ) {
			e.printStackTrace();
			// TODO: Handle exception
		}
	}
	
	/**
	 * Given an InformationState, check all known Templates and select 0, 1, or more to execute.
	 * If no Templates are found with all Preconditions fulfilled, none will be executed.
	 * It a Template has all Preconditions except the Triggers fulfilled and contains a Behaviour, this Behaviour will be prepared.
	 * Only 1 Template with a Behaviour will be executed with each call.
	 * Templates without a Behaviour (IS-updates) are always executed.
	 * 
	 * @param is - the current InformationState.
	 * @return True if a certain Behaviour was executed, False if not. 
	 */
	public TemplateState checkTemplates( InformationState is )
	{
		TemplateState executedTemplateState = null;
		/* Lists of Templates which are only IS-updates, templates that should be prepared, and templates that could be executed */
		ArrayList<TemplateState> ISUpdates = new ArrayList<TemplateState>();
		ArrayList<TemplateState> templatesToPrepare = new ArrayList<TemplateState>();
		ArrayList<TemplateState> templatesToRun = new ArrayList<TemplateState>();
		
		/* Check all Templates */
		for( Template template : templates ) {
			TemplateState state = template.checkTemplate(is);
			if( state.isComparesSatisfied() && state.isIndicatorsSatisfied() ) {
				if( state.isTriggersSatisfied() ) {
					if( state.getEffects().size() > 0 && state.getBehaviour() == null ) {
						ISUpdates.add(state);
					} else {
						templatesToRun.add(state);
					}
				} else if( state.getBehaviour() != null ) {
					templatesToPrepare.add(state);
				}
			}
		}
		
		if( templatesToRun.size() > 0 ) {
			if( templatesToRun.size() == 1 ) {
				/* If there is only 1 Template to run, check for conflicting Effects, apply the Effects, and run the Behaviour */
				TemplateState state = templatesToRun.get(0);
				checkConflictingEffects(state.getEffects(),ISUpdates);
				
				for( Effect effect : state.getEffects() ) {
					try {
						effect.apply(is, this);
					}catch( TemplateRunException e ) {
						System.out.println("Error while applying effect of Template  "+state.getTemplate().getId()+"("+state.getTemplate().getName()+")");
						e.printStackTrace();
					}
				}
				
				try {
					state.getBehaviour().execute(is);
					executedTemplateState = state;
				}catch( TemplateRunException e ) {
					System.out.println("Error while executing behaviour of Template  "+state.getTemplate().getId()+"("+state.getTemplate().getName()+")");
					e.printStackTrace();
				}
				addRecentBehaviour(state);
			} else {
				/* If there are more than 1 Template to run, select one, check for conflicting Effects, apply the Effects, and run the selected Behaviour */
				/* Modify the quality-values based on the history */
				templatesToRun = modifyQualityBasedOnHistory( templatesToRun );
				/* Select the Behaviour with the highest quality-value */
				TemplateState state = getBestTemplate( templatesToRun );
				checkConflictingEffects(state.getEffects(),ISUpdates);
				
				for( Effect effect : state.getEffects() ) {
					try {
						effect.apply(is, this);
					}catch( TemplateRunException e ) {
						System.out.println("Error while applying effect of Template  "+state.getTemplate().getId()+"("+state.getTemplate().getName()+")");
						e.printStackTrace();
					}
				}
				
				try {
					state.getBehaviour().execute(is);
					executedTemplateState = state;
				}catch( TemplateRunException e ) {
					System.out.println("Error while executing behaviour of Template  "+state.getTemplate().getId()+"("+state.getTemplate().getName()+")");
					e.printStackTrace();
				}
				addRecentBehaviour(state);
			}
		} else {
			/* If there are no Behaviours to execute, check for conflicting Effects, apply the Effects, and prepare the Behaviours that can be prepared */
			// Semaine-fix: Only prepare the best one
			if( templatesToPrepare.size() > 0 ) {
				TemplateState bestState = templatesToPrepare.get(0);
				double highestQuality = -1;
				for( TemplateState state : templatesToPrepare ) {
					if( state.getBehaviour().getQuality() > highestQuality ) {
						bestState = state;
						highestQuality = state.getBehaviour().getQuality();
					}
				}
				try {
					bestState.getBehaviour().prepare(is);
				}catch( TemplateRunException e ) {
					System.out.println("Error while preparing behaviour of Template  "+bestState.getTemplate().getId()+"("+bestState.getTemplate().getName()+")");
					e.printStackTrace();
				}
			}
			checkConflictingEffects(new ArrayList<Effect>(),ISUpdates);
		}
		
		/* Apply all Effects of the IS-update Templates */
		for( TemplateState state : ISUpdates ) {
			for( Effect effect : state.getEffects() ) {
				try {
					effect.apply(is, this);
				}catch( TemplateRunException e ) {
					System.out.println("Error while applying effect of Template  "+state.getTemplate().getId()+"("+state.getTemplate().getName()+")");
					e.printStackTrace();
				}
			}
		}
		return executedTemplateState;
	}
	
	/**
	 * Update the list of recently executed Behaviours, based on the given TemplateState
	 * @param state
	 */
	public void addRecentBehaviour( TemplateState state )
	{
		recentRunTemplates.add(state.getTemplate().getId());
		if( recentRunTemplates.size() > 5 ) {
			recentRunTemplates.remove(0);
		}
	}
	
	/**
	 * Searches all given Effects for conflicting Effects, that is, Effects that modify the exact same variable in the InformationState
	 * 
	 * @param effects1
	 * @param states
	 */
	private void checkConflictingEffects( ArrayList<Effect> effects1, ArrayList<TemplateState> states )
	{
		ArrayList<String> names = new ArrayList<String>();
		for( Effect effect : effects1 ) {
			if( effect instanceof Update ) {
				Update u = (Update)effect;
				String name = u.getName();
				if( !names.contains(name) ) {
					names.add(name);
				} else {
					System.out.println( "Warning, conflicting InformationState updates, may cause possible unexpected behaviour." );
				}
			}
		}
		for( TemplateState state : states ) {
			for( Effect effect : state.getEffects() ) {
				if( effect instanceof Update ) {
					Update u = (Update)effect;
					String name = u.getName();
					if( !names.contains(name) ) {
						names.add(name);
					} else {
						System.out.println( "Warning, conflicting InformationState updates, may cause possible unexpected behaviour." );
					}
				}
			}
		}
	}
	
	/**
	 * Modifies the quality-value of the given TemplateStates based on the behaviour history.
	 * The more recent a certain Behaviour was executed, the more its quality-value will be decreased.
	 * 
	 * @param states
	 * @return
	 */
	public ArrayList<TemplateState> modifyQualityBasedOnHistory( ArrayList<TemplateState> states )
	{
		for( TemplateState state : states ) {
			String id = state.getTemplate().getId();
			if( recentRunTemplates.contains(id) ) {
				int index = recentRunTemplates.indexOf(id);
				state.setQuality(state.getQuality() - ((index+1)*0.1));
			}
		}
		return states;
	}
	
	/**
	 * Returns the TemplateState which includes the Behaviour with the highest quality-value
	 * @param states
	 * @return
	 */
	public TemplateState getBestTemplate( ArrayList<TemplateState> states )
	{
		TemplateState bestState = null;
		double quality = -9999;
		for( TemplateState state : states ) {
			if( state.getQuality() > quality ) {
				quality = state.getQuality();
				bestState = state;
			}
		}
		return bestState;
	}
	
	/**
	 * Adds the given Object to the list of classes that contains user-specified functions.
	 * @param obj
	 */
	public void addFunction( Object obj)
	{
		functionClasses.add(obj);
	}
	
	/**
	 * @return the list of classes tha contain user-specified functions.
	 */
	public ArrayList<Object> getFunctionClasses()
	{
		return functionClasses;
	}
	
	/**
	 * FOR TESTING PURPOSES
	 * @param args
	 */
	public static void main( String args[] )
	{
//		System.out.println(1==1);
//		System.out.println(123.45==123.45);
//		/* Creating Test-IS */
//		InformationState is = new InformationState();
//		is.set("String_Test", "Test123");
//		is.set("Double_Test", 123.45);
//		is.set("Int_Test", 12345);
//		is.set("Record_Test", new InformationState());
//		is.getRecord("Record_Test").set("Record_Double_Test", 123.456);
//		is.set("List_Test",new List());
//		is.getList("List_Test").addItemEnd(123.4567);
//		
//		/* Testing Strings */
//		Compare c1 = new CompareExists("Test123");
//		System.out.println("Testing C1: " + c1.isValid(is));

		File f = new File("templates/templates.xml");
		System.out.println(f.getAbsolutePath());
		ArrayList<File> files = new ArrayList<File>();
		files.add(f);
		TemplateParser p = new TemplateParser();
		//p.readTemplates(files);
	}
}