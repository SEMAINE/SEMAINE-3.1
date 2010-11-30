package eu.semaine.components.dialogue.behaviourselection.template;

import java.util.ArrayList;
import java.util.Random;

import org.w3c.dom.Element;
import org.w3c.dom.NodeList;

import eu.semaine.components.dialogue.behaviourselection.template.behaviours.Behaviour;
import eu.semaine.components.dialogue.behaviourselection.template.effects.Effect;
import eu.semaine.components.dialogue.behaviourselection.template.effects.Function;
import eu.semaine.components.dialogue.behaviourselection.template.effects.Update;
import eu.semaine.components.dialogue.behaviourselection.template.preconditions.Compare;
import eu.semaine.components.dialogue.behaviourselection.template.preconditions.Indicator;
import eu.semaine.components.dialogue.behaviourselection.template.preconditions.Precondition;
import eu.semaine.components.dialogue.exceptions.TemplateParseException;
import eu.semaine.components.dialogue.informationstate.InformationState;

/**
 * A Template contains all data of a template, and is responsible for checking its preconditions and executing
 * its effects and behaviour.
 * 
 * @version 0.1.1
 * Added support for the removal of IS-elements.
 * 
 * @version 0.1
 * First version
 * 
 * @author Mark ter Maat
 */

public class Template
{	
	/* These are the names of the XML-Elements */
	public static final String E_TEMPLATE = "template";
	public static final String E_PRECONDITIONBLOCK = "preconditions";
	public static final String E_COMPARE = "compare";
	public static final String E_INDICATOR = "indicator";
	public static final String E_INDICATORCOMPARE = "icompare";
	public static final String E_TRIGGER = "trigger";
	public static final String E_EFFECTSBLOCK = "effects";
	public static final String E_UPDATE = "update";
	public static final String E_FUNCTION = "function";
	public static final String E_REMOVE = "remove";
	public static final String E_BEHAVIOURBLOCK = "behaviour";
	public static final String E_ARGUMENT = "argument";
	public static final String E_OPTION = "optional";
	public static final String E_CHOICE = "option";
	public static final String E_CEFFECTSBLOCK = "c_effects";
	public static final String E_CUPDATE = "c_update";
	public static final String E_CFUNCTION = "c_function";
	public static final String E_CREMOVE = "c_remove";
	public static final String E_CBEHAVIOURBLOCK = "c_behaviour";
	public static final String E_CARGUMENT = "c_argument";
	
	/* These are the names of the XML-Attributes */
	public static final String A_ID = "id";
	public static final String A_TEMPLATENAME = "name";
	public static final String A_VALUE1 = "value1";
	public static final String A_COMPARATOR = "comparator";
	public static final String A_VALUE2 = "value2";
	public static final String A_INDICATORSTARTVALUE = "startvalue";
	public static final String A_INDICATORENDVALUE = "endvalue";
	public static final String A_MODIFIER = "modifier";
	public static final String A_EFFECTVARNAME = "name";
	public static final String A_EFFECTVARVALUE = "value";
	public static final String A_BEHAVIOURCLASS = "class";
	public static final String A_QUALITY = "quality";
	public static final String A_ARGUMENTNAME = "name";
	public static final String A_ARGUMENTVALUE = "value";
	public static final String A_PROBABILITY = "probability";
	
	/* These are the names of the XML Comparison-types */
	public static final String COMPARATOR_EQUALS = "equals";
	public static final String COMPARATOR_NOTEQUALS = "not_equals";
	public static final String COMPARATOR_GREATERTHAN = "greater_than";
	public static final String COMPARATOR_GREATEREQUALS = "greater_equals";
	public static final String COMPARATOR_LESSERTHAN = "smaller_than";
	public static final String COMPARATOR_LESSEREQUALS = "smaller_equals";
	public static final String COMPARATOR_EXISTS = "exists";
	public static final String COMPARATOR_NOTEXISTS = "not_exists";
	public static final String COMPARATOR_CONTAINS = "contains";
	public static final String COMPARATOR_NOTCONTAINS = "not_contains";
	
	/* The (unique) id of this template */
	private String id;
	/* The (human-readable) name of this template */
	private String name;
	
	/* The preconditions, Effects, and Behaviour of this Template */
	private ArrayList<Precondition> compares = new ArrayList<Precondition>();
	private ArrayList<Indicator> indicators = new ArrayList<Indicator>();
	private ArrayList<Precondition> triggers = new ArrayList<Precondition>();
	private ArrayList<Effect> effects = new ArrayList<Effect>();
	private Behaviour behaviour = null;
	private ArrayList<ArrayList<Choice>> optional = new ArrayList<ArrayList<Choice>>();
	
	/* A random-generator */
	private Random random = new Random();
	
	/**
	 * Creates a new Template with the given ID and name.
	 * 
	 * @param id - the ID of the new Template
	 * @param name - the name of the new Template
	 */
	public Template( String id, String name )
	{
		this.id = id;
		if( name != null ) {
			this.name = name;
		} else {
			this.name = "";
		}
	}
	
	/**
	 * Adds the given Precondition to the list of Compares of this Template
	 * @param p - the new Compare
	 */
	public void addCompare( Precondition p )
	{
		compares.add(p);
	}
	
	/**
	 * Adds the given Precondition to the list of Indicators of this Template
	 * @param p - the new Indicator
	 */
	public void addIndicator( Indicator i )
	{
		indicators.add(i);
	}
	
	/**
	 * Adds the given Precondition to the list of Triggers of this Template
	 * @param p - the new Trigger
	 */
	public void addTrigger( Precondition p )
	{
		triggers.add(p);
	}
	
	/**
	 * Adds the given Effects to the list of Effects of this Template
	 * @param p - the new Effect
	 */
	public void addEffect( Effect e)
	{
		effects.add(e);
	}
	
	/**
	 * Sets the Behaviour this Template
	 * @param p - the new Behaviour
	 */
	public void setBehaviour( Behaviour b)
	{
		behaviour = b;
	}
	
	/**
	 * Adds a list of Choices to the Template
	 * @param options - the list of choices
	 */
	public void addOptions( ArrayList<ArrayList<Choice>> options )
	{
		optional = options;
	}
	
	/**
	 * @return the id
	 */
	public String getId() {
		return id;
	}

	/**
	 * @return the name
	 */
	public String getName() {
		return name;
	}
	
	/**
	 * This method uses the given InformationState to check the current state of this template.
	 * This State contains which kind of preconditions are fulfilled (compares, indicators, and triggers) 
	 * and keeps track of the total number of non-fulfilled preconditions, 
	 * 
	 * @param is - the Current InformationState
	 * @return
	 */
	public TemplateState checkTemplate( InformationState is )
	{
		/* Initializing variables */
		TemplateState resultState;
		boolean comparesSatisfied = true;;
		boolean indicatorsSatisfied = true;
		boolean triggersSatisfied = true;
		int nrMissing = 0;
		
		/* Check if all Compares are fulfilled */
		for( Precondition c : compares ) {
			boolean result = c.isValid(is);
			if( !result ) {
				comparesSatisfied = false;
				nrMissing++;
			}
		}
		
		/* Check if all Indicators are fulfilled */
		for( Precondition c : indicators ) {
			boolean result = c.isValid(is);
			if( !result ) {
				indicatorsSatisfied = false;
				nrMissing++;
			}
		}
		
		/* Check if all Triggers are fulfilled */
		for( Precondition c : triggers ) {
			boolean result = c.isValid(is);
			if( !result ) {
				triggersSatisfied = false;
				nrMissing++;
			}
		}
		
		/* Create a new TemplateState */
		resultState = new TemplateState(this,comparesSatisfied,indicatorsSatisfied,triggersSatisfied,(compares.size()+indicators.size()+triggers.size()),nrMissing);
		
		/* Determine which choice-blocks are chosen, and make a list of all Effects that have to be executed. */
		ArrayList<Effect> currEffects = new ArrayList<Effect>();
		Behaviour currBehaviour = null;
		
		if( nrMissing == 0 ) {
			for( ArrayList<Choice> option : optional ) {
				double chance = random.nextDouble();
				for( Choice choice : option ) {
					chance = chance - choice.chance;
					if( chance < 0 ) {
						currEffects.addAll(choice.effects);
						if( choice.behaviour != null ) {
							currBehaviour = choice.behaviour;
							if( currBehaviour != null ) {
								System.out.println("Overwriting Behaviour in template  "+id+"("+name+")");
							}
						}
						break;
					}
				}
			}
		}
		
		currEffects.addAll(effects);
		if( behaviour != null  ) {
			if( currBehaviour != null ) {
				System.out.println("Overwriting Behaviour in template  "+id+"("+name+")");
			}
			currBehaviour = behaviour;
		}
		
		/* Put the list with all Effects and the Behaviour in the TemplateState, and return it */
		resultState.setEffects(currEffects);
		if( currBehaviour != null ) {
			resultState.setBehaviour(currBehaviour);
		}
		
		return resultState;
	}
	
	/**
	 * Given the DOM Template-Element, returns the Template that fits the XML.
	 * This class creates a new Template and returns this.
	 * 
	 * @param templateElement - the DOM Element of this Template
	 * @return the Template
	 * @throws TemplateParseException
	 */
	public static Template parseTemplateElement( Element templateElement ) throws TemplateParseException
	{
		/* Make a new template with an ID and a name*/
		Template template;
		if( templateElement.hasAttribute(A_ID) ) {
			template = new Template( templateElement.getAttribute(A_ID), templateElement.getAttribute(A_TEMPLATENAME) );
		} else {
			throw new TemplateParseException("Template found without an 'id'-element");
		}
		
		/* +++++ Preconditions +++++ */
		NodeList preconditionBlockList = templateElement.getElementsByTagName(E_PRECONDITIONBLOCK);
		if( preconditionBlockList.getLength() > 1 ) {
			throw new TemplateParseException("Template '"+template.getId()+"' has more than 1 <preconditions> element.");
		}
		Element preconditionBlock = (Element)preconditionBlockList.item(0) ;
		
		/* Parse Compares */
		NodeList preconditionList = preconditionBlock.getElementsByTagName(E_COMPARE);
		for( int i=0; i<preconditionList.getLength(); i++ ) {
			Element preconditionElement = ((Element)(preconditionList.item(i)));
			template.addCompare( Compare.parsePrecondition(preconditionElement) );
		}
		
		/* Parse Indicators */
		NodeList indicatorList = preconditionBlock.getElementsByTagName(E_INDICATOR);
		for( int i=0; i<indicatorList.getLength(); i++ ) {
			Element indicatorElement = ((Element)(indicatorList.item(i)));
			template.addIndicator( Indicator.parseIndicator(indicatorElement) );
		}
		
		/* Parse Triggers */
		NodeList triggerList = preconditionBlock.getElementsByTagName(E_TRIGGER);
		for( int i=0; i<triggerList.getLength(); i++ ) {
			Element triggerElement = ((Element)(triggerList.item(i)));
			template.addTrigger( Compare.parsePrecondition(triggerElement) );
		}
		
		/* +++++ Effects +++++ */
		NodeList effectsBlockList = templateElement.getElementsByTagName(E_EFFECTSBLOCK);
		if( effectsBlockList.getLength() > 1 ) {
			throw new TemplateParseException("Template '"+template.getId()+"' has more than 1 <effects> element.");
		}
		Element effectsBlock = (Element)effectsBlockList.item(0) ;
		
		if( effectsBlock != null ) {
			/* Parse Updates */
			NodeList updateList = effectsBlock.getElementsByTagName(E_UPDATE);
			for( int i=0; i<updateList.getLength(); i++ ) {
				Element updateElement = (Element)updateList.item(i);
				template.addEffect(Update.parseUpdate(updateElement));
			}
			
			/* Parse Updates */
			NodeList removeList = effectsBlock.getElementsByTagName(E_REMOVE);
			for( int i=0; i<removeList.getLength(); i++ ) {
				Element removeElement = (Element)removeList.item(i);
				template.addEffect(Update.parseUpdate(removeElement));
			}
			
			/* Parse Functions */
			NodeList functionList = effectsBlock.getElementsByTagName(E_FUNCTION);
			for( int i=0; i<functionList.getLength(); i++ ) {
				Element functionElement = (Element)functionList.item(i);
				template.addEffect(Function.parseFunction(functionElement,false));
			}
		}
		
		/* +++++ Behaviours +++++ */
		NodeList behaviourBlockList = templateElement.getElementsByTagName(E_BEHAVIOURBLOCK);
		if( behaviourBlockList.getLength() > 1 ) {
			throw new TemplateParseException("Template '"+template.getId()+"' has more than 1 <behaviour> element.");
		}
		Element behaviourElement = (Element)behaviourBlockList.item(0) ;
		if( behaviourElement != null ) {
			template.setBehaviour(Behaviour.parseBehaviour(behaviourElement,false));
		}
		
		/* +++++ Optional Behaviour +++++ */
		NodeList optionBlockList = templateElement.getElementsByTagName(E_OPTION);;
		ArrayList<ArrayList<Choice>> options = new ArrayList<ArrayList<Choice>>();
		for( int i=0; i<optionBlockList.getLength(); i++ ) {
			ArrayList<Choice> choices = new ArrayList<Choice>();
			
			Element optionBlock = (Element)optionBlockList.item(i);
			NodeList choiceElementList = optionBlock.getElementsByTagName(E_CHOICE);
			for( int j=0; j<choiceElementList.getLength(); j++ ) {
				Element choiceElement = (Element)choiceElementList.item(j);
				if( !choiceElement.hasAttribute(A_PROBABILITY) ) {
					throw new TemplateParseException("Template '"+template.getId()+"' is missing a probability in a Choice-element");
				}
				Choice choice = new Choice(choiceElement.getAttribute(A_PROBABILITY));
				
				/* +++++ Effects +++++ */
				NodeList ceffectsBlockList = choiceElement.getElementsByTagName(E_CEFFECTSBLOCK);
				if( ceffectsBlockList.getLength() > 1 ) {
					throw new TemplateParseException("Template '"+template.getId()+"' has more than 1 <c_effects> element.");
				}
				Element ceffectsBlock = (Element)ceffectsBlockList.item(0) ;
				
				if( ceffectsBlock != null ) {
					/* Parse Updates */
					NodeList cupdateList = ceffectsBlock.getElementsByTagName(E_CUPDATE);
					for( int k=0; k<cupdateList.getLength(); k++ ) {
						Element cupdateElement = (Element)cupdateList.item(k);
						choice.addEffect(Update.parseUpdate(cupdateElement));
						// --> template.addEffect(Update.parseUpdate(cupdateElement));
					}
					
					/* Parse Removals */
					NodeList cremoveList = ceffectsBlock.getElementsByTagName(E_CREMOVE);
					for( int k=0; k<cremoveList.getLength(); k++ ) {
						Element curemoveElement = (Element)cremoveList.item(k);
						choice.addEffect(Update.parseUpdate(curemoveElement));
						// --> template.addEffect(Update.parseUpdate(cupdateElement));
					}
					
					/* Parse Functions */
					NodeList cfunctionList = ceffectsBlock.getElementsByTagName(E_CFUNCTION);
					for( int k=0; k<cfunctionList.getLength(); k++ ) {
						Element cfunctionElement = (Element)cfunctionList.item(k);
						choice.addEffect(Function.parseFunction(cfunctionElement,true));
						// --> template.addFunction(Function.parseFunction(functionElement));
					}
				}
				
				/* +++++ Behaviours +++++ */
				NodeList cbehaviourBlockList = choiceElement.getElementsByTagName(E_CBEHAVIOURBLOCK);
				if( cbehaviourBlockList.getLength() > 1 ) {
					throw new TemplateParseException("Template '"+template.getId()+"' has more than 1 <c_behaviour> element.");
				}
				Element cbehaviourElement = (Element)cbehaviourBlockList.item(0) ;
				choice.addBehaviour(Behaviour.parseBehaviour(cbehaviourElement,true));
				// --> template.addBehaviour(Behaviour.parseBehaviour(behaviourElement));
				
				choices.add(choice);
			}
			options.add(choices);
		}
		template.addOptions(options);
		
		return template;
	}
	
	/**
	 * The subclass Choice keeps track of a Choice-element, which can have a list of Effects, a Behaviour, and a chance.
	 * 
	 * @author mark
	 * @version 0.1
	 */
	public static class Choice
	{
		public double chance;
		public ArrayList<Effect> effects = new ArrayList<Effect>();
		public Behaviour behaviour = null;
		
		public Choice( String chance ) throws TemplateParseException
		{
			try {
				this.chance = Double.parseDouble(chance);
			}catch( NumberFormatException e ) {
				throw new TemplateParseException("Probability '"+chance+"' could not be converted to a Double");
			}
		}
		
		public void addEffect( Effect e )
		{
			effects.add(e);
		}
		
		public void addBehaviour( Behaviour b )
		{
			behaviour = b;
		}
	}
}
