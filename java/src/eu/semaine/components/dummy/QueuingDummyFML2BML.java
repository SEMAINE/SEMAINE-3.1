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
public class QueuingDummyFML2BML extends DummyVisualFML2BML {
	public QueuingDummyFML2BML() throws JMSException 
	{
		super("QueuingDummyFML2BML",
			"semaine.data.action.prepare.speechpreprocessed",
			"semaine.data.synthesis.prepare");
	}

}
