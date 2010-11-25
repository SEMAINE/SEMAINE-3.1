/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.gui.monitor;

import java.awt.Color;

public class ConnectionInfo extends Info
{
	private static final int MAXWIDTH = 4;
	private static final int MINWIDTH = 0;
	private static final int STEPSIZE = 2;
	private static final int STEPTIME = 200; // in ms

	private int width;
	private long nextStepDue;
	private boolean isShowingActive = false;
	
	public ConnectionInfo()
	{
		super(null);
		width = MINWIDTH;
		nextStepDue = 0;
	}
	
	public void setActive()
	{
		long time = System.currentTimeMillis();
		if (isShowingActive) {
			// this one is already showing, push back next one:
			nextStepDue = time+STEPTIME;
		} else {
			width = MAXWIDTH;
			nextStepDue = time; // now!
		}
	}


	public int getLineWidth()
	{
		if (width == MAXWIDTH) {
			isShowingActive = true;
		} else {
			isShowingActive = false;
		}
		return width;
	}

	@Override
	public synchronized boolean needsUpdate()
	{
		if (width > MINWIDTH) {
			long currentTime = System.currentTimeMillis();
			if (currentTime >= nextStepDue) {
				nextStepDue += STEPTIME;
				width-= STEPSIZE;
				return true;
			}
		}
		return false;
	}

}
