package eu.semaine.components.dialogue.datastructures;

public class AgentSpokenUtterance
{
	private AgentUtterance utterance;
	private int newCharacter = -1;
	private long time;
	private double quality = 0d;
	
	public AgentSpokenUtterance( AgentUtterance utterance )
	{
		this.utterance = utterance;
	}
	
	public AgentSpokenUtterance( int character )
	{
		this.newCharacter = character;
	}

	/**
	 * @return the type
	 */
	public AgentUtterance getUtterance() {
		return utterance;
	}

	/**
	 * @param utterance the utterance to set
	 */
	public void setUtterance(AgentUtterance utterance) {
		this.utterance = utterance;
	}

	/**
	 * @return the time
	 */
	public long getTime() {
		return time;
	}

	/**
	 * @param time the time to set
	 */
	public void setTime(long time) {
		this.time = time;
	}

	/**
	 * @return the newCharacter
	 */
	public int getNewCharacter() {
		return newCharacter;
	}

	/**
	 * @param newCharacter the newCharacter to set
	 */
	public void setNewCharacter(int newCharacter) {
		this.newCharacter = newCharacter;
	}

	/**
	 * @return the quality
	 */
	public double getQuality() {
		return quality;
	}

	/**
	 * @param quality the quality to set
	 */
	public void setQuality(double quality) {
		this.quality = quality;
	}
	
	
}
