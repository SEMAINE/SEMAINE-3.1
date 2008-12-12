/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.gui.monitor;

import org.jgraph.graph.DefaultGraphCell;

public class Info
{
	private DefaultGraphCell cell;
	protected InfoDialog dialog;
	private boolean changed = true;

	
	protected Info() {}
	
	public DefaultGraphCell getCell() {
		return cell;
	}
	
	public void setCell(DefaultGraphCell aCell)
	{
		this.cell = aCell;
	}
	
	public InfoDialog getDialog()
	{
		return dialog;
	}
	
	public void setDialog(InfoDialog aDialog)
	{
		this.dialog = aDialog;
	}
	
	public synchronized boolean needsUpdate()
	{
		return changed;
	}
	
	public synchronized void setChanged(boolean newChanged)
	{
		changed = newChanged;
	}
}
