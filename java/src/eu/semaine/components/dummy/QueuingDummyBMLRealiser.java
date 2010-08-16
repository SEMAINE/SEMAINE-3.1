/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.dummy;

import javax.jms.JMSException;

/**
 * @author marc
 *
 */
public class QueuingDummyBMLRealiser extends DummyBMLRealiser {
	public QueuingDummyBMLRealiser() throws JMSException 
	{
		super("QueuingDummyBMLRealiser",
				"semaine.data.synthesis.prepare",
				"semaine.data.synthesis.lowlevel.video.FAP",
				"semaine.data.synthesis.lowlevel.command",
				false);
	}

}
