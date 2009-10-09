/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.gui.monitor;

import org.jgraph.event.GraphSelectionEvent;
import org.jgraph.event.GraphSelectionListener;
import org.jgraph.graph.DefaultGraphCell;

public class InfoGraphSelectionListener implements GraphSelectionListener
{
	
	public void valueChanged(GraphSelectionEvent e) {
		Object[] cells = e.getCells();
		for (int i=0; i<cells.length; i++) {
			if (e.isAddedCell(i)) {
				Object userObject = ((DefaultGraphCell)cells[i]).getUserObject();
				if (userObject instanceof ComponentInfo) {
					ComponentInfo ci = (ComponentInfo) userObject;
					InfoDialog dialog = ci.getDialog();
					if (dialog == null) {
						dialog = new InfoDialog(ci.toString());
						ci.setDialog(dialog);
					}
					dialog.setText(ci.getInfo());
					dialog.setVisible(true);
				} else if (userObject instanceof TopicInfo) {
					TopicInfo ti = (TopicInfo) userObject;
					InfoDialog dialog = ti.getDialog();
					if (dialog == null) {
						dialog = new InfoDialog(ti.getName());
						ti.setDialog(dialog);
					}
					dialog.setText(ti.getInfo());
					dialog.setVisible(true);
				}
			} else { // selection removed from cell
				Object userObject = ((DefaultGraphCell)cells[i]).getUserObject();
				if (userObject != null && userObject instanceof Info) {
					Info inf = (Info) userObject;
					InfoDialog dialog = inf.getDialog();
					if (dialog != null)
						dialog.setVisible(false);
				}
			}
		}
	}
}
