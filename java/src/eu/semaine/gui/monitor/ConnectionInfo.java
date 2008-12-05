package eu.semaine.gui.monitor;

import java.awt.Color;

public class ConnectionInfo extends Info
{
	private static final int MAXWIDTH = 4;
	private static final int MINWIDTH = 0;
	private static final int STEPTIME = 100; // in ms

	private int width;
	private long nextStepDue;
	
	public ConnectionInfo()
	{
		width = MINWIDTH;
		nextStepDue = 0;
	}
	
	public void setActive()
	{
		long time = System.currentTimeMillis();
		if (width == MAXWIDTH && nextStepDue > time) {
			// this one is already showing, push back next one:
			nextStepDue = time+STEPTIME;
		} else {
			width = MAXWIDTH;
			nextStepDue = time; // now!
		}
	}


	public int getLineWidth()
	{
		return width;
	}

	@Override
	public synchronized boolean needsUpdate()
	{
		if (width > MINWIDTH) {
			long currentTime = System.currentTimeMillis();
			if (currentTime >= nextStepDue) {
				nextStepDue += STEPTIME;
				width--;
				return true;
			}
		}
		return false;
	}


	public String toString() { return null; }
}
