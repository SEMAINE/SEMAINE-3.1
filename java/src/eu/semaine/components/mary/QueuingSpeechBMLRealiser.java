/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.mary;

import javax.jms.JMSException;

/**
 * @author marc
 *
 */
public class QueuingSpeechBMLRealiser extends SpeechBMLRealiser {

	public QueuingSpeechBMLRealiser() throws JMSException 
	{
		super("QueuingSpeechBMLRealiser",
				"semaine.data.synthesis.prepare.plan",
				"semaine.data.synthesis.prepare.speechtimings",
				"semaine.data.synthesis.lowlevel.audio");
	}
}
