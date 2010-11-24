/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.dummy;

import java.util.Random;

import javax.jms.JMSException;

import eu.semaine.components.Component;
import eu.semaine.jms.sender.FeatureSender;

/**
 * @author marc
 *
 */
public class DummyFeatureExtractor extends Component 
{
	private FeatureSender featureSender;
	/**
	 * @param componentName
	 * @throws JMSException
	 */
	public DummyFeatureExtractor() throws JMSException 
	{
		super("DummyFeatureExtractor", true, false);
		int period = 10; // ms
		featureSender = new FeatureSender("semaine.data.analysis.dummy", "", getName(), period);
		waitingTime = period;
		senders.add(featureSender); // so it can be started etc
		/*featureSender.setFeatureNames(new String[] {
			"f0.level",
			"f0.range",
			"energy.db",
			"optical.flow",
			"facialfeaturepoint1.x"
		});*/
		// A scalable list of features -- for load testing 
		String[] dummyFeatureNames = new String[100];
		for (int i=0; i<dummyFeatureNames.length; i++) {
			dummyFeatureNames[i] = "feature.name.of.feature"+i;
		}
		featureSender.setFeatureNames(dummyFeatureNames);
	}

	@Override
	protected void act()
	throws JMSException
	{
		float[] features = new float[featureSender.getFeatureNames().length];
		Random random = new Random();
		for (int i=0; i<features.length; i++) {
			features[i] = random.nextFloat();
		}
		//log.debug("sending feature vector with "+features.length+" features");
		featureSender.sendFeatureVector(features, meta.getTime());
	}
}
