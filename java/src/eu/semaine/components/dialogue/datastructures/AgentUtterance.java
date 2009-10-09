package eu.semaine.components.dialogue.datastructures;

import java.util.ArrayList;
import java.util.HashMap;

public class AgentUtterance
{
	private String utterance;
	private String category;
	
	private HashMap<ArrayList<String>,AgentUtterance> linkingUtterances = new HashMap<ArrayList<String>,AgentUtterance>();
	private AgentUtterance elaborationUtterance;
	
	private ArrayList<String> features;
	
	public AgentUtterance( String utterance, String category )
	{
		this.utterance = utterance;
		this.category = category;
	}
	
	public ArrayList<AgentUtterance> getLinkingUtterances( ArrayList<String> features )
	{
		ArrayList<AgentUtterance> responses = new ArrayList<AgentUtterance>();
		
		for( ArrayList<String> preconditions : linkingUtterances.keySet() ) {
			boolean valid = true;
			for( String pre : preconditions ) {
				if( !features.contains(pre) ) {
					valid = false;
				}
			}
			if( valid ) {
				responses.add( linkingUtterances.get(preconditions) );
			}
		}
		
		return responses;
	}

	/**
	 * @return the utterance
	 */
	public String getUtterance() {
		return utterance;
	}

	/**
	 * @param utterance the utterance to set
	 */
	public void setUtterance(String utterance) {
		this.utterance = utterance;
	}

	/**
	 * @return the category
	 */
	public String getCategory() {
		return category;
	}

	/**
	 * @param category the category to set
	 */
	public void setCategory(String category) {
		this.category = category;
	}

	/**
	 * @return the linkingUtterances
	 */
	public HashMap<ArrayList<String>, AgentUtterance> getLinkingUtterances() {
		return linkingUtterances;
	}

	/**
	 * @param linkingUtterances the linkingUtterances to set
	 */
	public void setLinkingUtterances(
			HashMap<ArrayList<String>, AgentUtterance> linkingUtterances) {
		this.linkingUtterances = linkingUtterances;
	}

	/**
	 * @return the elaborationUtterance
	 */
	public AgentUtterance getElaborationUtterance() {
		return elaborationUtterance;
	}

	/**
	 * @param elaborationUtterance the elaborationUtterance to set
	 */
	public void setElaborationUtterance(AgentUtterance elaborationUtterance) {
		this.elaborationUtterance = elaborationUtterance;
	}

	/**
	 * @return the features
	 */
	public ArrayList<String> getFeatures() {
		return features;
	}

	/**
	 * @param features the features to set
	 */
	public void setFeatures(ArrayList<String> features) {
		this.features = features;
	}
	
	
}
