/**
 * Copyright (C) 2008 University of Twente, HMI. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.dialogue.datastructures;

import java.util.ArrayList;

/**
 * This class serves as a container for conversation model elements.
 * It includes a response and a list of all required and all optional
 * features that are linked to this response.
 * 
 * @author Mark tM
 * @version 0.1 - dummy class
 *
 */

public class ContextTemplate 
{
	public final static int POPPY = 1;
	public final static int OBADIAH = 2;
	public final static int PRUDENCE = 3;
	public final static int SPIKE = 4;
	
	public final static int HAPPY = 5;
	public final static int SAD = 6;
	public final static int ANGRY = 7;
	
	public final static int POSITIVE = 8;
	public final static int NEGATIVE = 9;
	
	public final static int AGREE = 10;
	public final static int DISAGREE = 11;
	public final static int ABOUT_OTHER_PEOPLE = 12;
	public final static int ABOUT_OTHER_CHARACTER = 13;
	public final static int ABOUT_CURRENT_CHARACTER = 14;
	public final static int ABOUT_OWN_FEELINGS = 15;
	public final static int PRAGMATIC = 16;
	
	public final static int TALK_ABOUT_SELF = 17;
	
	public final static int PAST_EVENT = 18;
	public final static int FUTURE_EVENT = 19;
	public final static int PAST_ACTION = 20;
	public final static int FUTURE_ACTION = 21;
	
	public final static int LAUGH = 22;
	
	/* The response */
	private String reaction;
	
	/* The features that are required for this response */
	private ArrayList<Integer> requiredFeatures = new ArrayList<Integer>();
	
	/* The features that are optional for this response */
	private ArrayList<Integer> extraFeatures = new ArrayList<Integer>();
	
	/**
	 * Creates a new ContextTemplate with the given reaction
	 * @param reaction
	 */
	public ContextTemplate( String reaction )
	{
		this.reaction = reaction;
	}
	
	/**
	 * Adds a required feature to this template
	 * @param feature
	 */
	public void addRequiredFeature( int feature )
	{
		if( ! requiredFeatures.contains( feature ) ) {
			requiredFeatures.add( feature );
		}
	}
	
	/**
	 * Adds an optional feature to this template
	 * @param feature
	 */
	public void addExtraFeature( int feature )
	{
		if( ! extraFeatures.contains( feature ) ) {
			extraFeatures.add( feature );
		}
	}
	
	/**
	 * @return the reaction of this template
	 */
	public String getReaction()
	{
		return reaction;
	}
	
	/**
	 * @return the required features of this template
	 */
	public ArrayList<Integer> getRequiredFeatures()
	{
		return requiredFeatures;
	}
	
	/**
	 * @return the optional features of this template
	 */
	public ArrayList<Integer> getExtraFeatures()
	{
		return extraFeatures;
	}
	
	/**
	 * Sets a new response
	 * @param reaction
	 */
	public void setNewReaction( String reaction )
	{
		this.reaction = reaction;
	}
	
	/**
	 * Creates a String representation of this template
	 */
	public String toString()
	{
		String result = "";
		for( Integer i : requiredFeatures ) {
			result += feature2String(i) + " ";
		}
		
		return result;
	}
	
	/**
	 * Overrides the Object-equals method to compare to ContextTemplates with each other
	 * @param template
	 * @return
	 */
	public boolean equals( ContextTemplate template )
	{
		if( requiredFeatures.size() == template.getRequiredFeatures().size() && 
			extraFeatures.size() == template.getExtraFeatures().size() ) {
			for( Integer i : template.getRequiredFeatures() ) {
				if( ! requiredFeatures.contains(i) ) {
					return false;
				}
			}
			return true;
		}
		return false;
	}
	
	/**
	 * Returns a String representation of the given feature-integer
	 * @param i
	 * @return
	 */
	public String feature2String( Integer i ) 
	{
		switch(i) {
			case 1 : return "POPPY";
			case 2 : return "OBADIAH";
			case 3 : return "PRUDENCE";
			case 4 : return "SPIKE";
			case 5 : return "HAPPY";
			case 6 : return "SAD";
			case 7 : return "ANGRY";
			case 8 : return "POSITIVE";
			case 9 : return "NEGATIVE";
			case 10 : return "AGREE";
			case 11 : return "DISAGREE";
			case 12 : return "ABOUT_OTHER_PEOPLE";
			case 13 : return "ABOUT_OTHER_CHARACTER";
			case 14 : return "ABOUT_CURRENT_CHARACTER";
			case 15 : return "ABOUT_OWN_FEELINGS";
			case 16 : return "PRAGMATIC";
			case 17 : return "TALK_ABOUT_SELF";
			case 18 : return "PAST_EVENT";
			case 19 : return "FUTURE_EVENT";
			case 20 : return "PAST_ACTION";
			case 21 : return "FUTURE_ACTION";
			case 22 : return "LAUGH";
			default: return "";
		}
	}
}
