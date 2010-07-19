/**
 * Copyright (C) 2010 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.mary;

import javax.jms.JMSException;


/**
 * @author marc
 *
 */
public class QueuingSpeechPreprocessor extends SpeechPreprocessor {
	public QueuingSpeechPreprocessor() throws JMSException {
		super("QueuingSpeechPreprocessor", 
			"semaine.data.action.prepare.function",
			"semaine.data.action.prepare.behaviour",
			"semaine.data.action.prepare.speechpreprocessed");
	}
}
