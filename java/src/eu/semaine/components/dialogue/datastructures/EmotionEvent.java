package eu.semaine.components.dialogue.datastructures;

public class EmotionEvent
{
	public final static int VALENCE = 0;
	public final static int AROUSAL = 1;
	public final static int INTEREST = 2;
	
	private long time;
	private long duration;
	private int type;
	private float intensity;
	
	public EmotionEvent( long time, long duration, int type, float intensity )
	{
		this.time = time;
		this.duration = duration;
		this.type = type;
		this.intensity = intensity;
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
	 * @return the duration
	 */
	public long getDuration() {
		return duration;
	}

	/**
	 * @param duration the duration to set
	 */
	public void setDuration(long duration) {
		this.duration = duration;
	}

	/**
	 * @return the type
	 */
	public int getType() {
		return type;
	}

	/**
	 * @param type the type to set
	 */
	public void setType(int type) {
		this.type = type;
	}

	/**
	 * @return the intensity
	 */
	public float getIntensity() {
		return intensity;
	}

	/**
	 * @param intensity the intensity to set
	 */
	public void setIntensity(float intensity) {
		this.intensity = intensity;
	}
	
	
}
