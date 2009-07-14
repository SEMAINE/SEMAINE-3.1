package eu.semaine.components.dialogue.datastructures;

public class AgentUtterance
{
	private String type;
	private String utterance;
	private long time;
	
	public AgentUtterance( String type, String utterance )
	{
		this.type = type;
		this.utterance = utterance;
	}

	/**
	 * @return the type
	 */
	public String getType() {
		return type;
	}

	/**
	 * @param type the type to set
	 */
	public void setType(String type) {
		this.type = type;
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
	
	
}
