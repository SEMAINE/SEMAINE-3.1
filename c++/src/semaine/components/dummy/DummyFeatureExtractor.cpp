/*
 *  DummyFeatureExtractor.cpp
 *  semaine
 *
 *  Created by Marc Schröder on 19.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#include "DummyFeatureExtractor.h"

#include <cstdlib>
#include <sstream>

namespace semaine {
namespace components {
namespace dummy {

DummyFeatureExtractor::DummyFeatureExtractor() throw(CMSException) :
	Component("DummyFeatureExtractor")
{
	int period = 10; // ms
	featureSender = new FeatureSender("semaine.data.analysis.dummy", "", getName(), period);
	waitingTime = period;
	senders.push_back(featureSender);
	// A scalable list of features -- for load testing
	int num = 100;
	std::vector<std::string> dummyFeatureNames(num);
	for (int i=0; i<num; i++) {
		std::ostringstream namestream;
		namestream << "feature.name.of.feature" << i;
		dummyFeatureNames[i] = namestream.str();
	}
	featureSender->setFeatureNames(dummyFeatureNames);
	featureSender->setTimeToLive(100); // discard messages after 100 ms
}

DummyFeatureExtractor::~DummyFeatureExtractor()
{
	delete featureSender;
}

void DummyFeatureExtractor::customStartIO() throw(CMSException)
{
  printf("customStartIO HERE!!\n");
}

void DummyFeatureExtractor::act() throw(CMSException)
{
	int n = featureSender->getFeatureNames().size();
	std::vector<float> features(n);
	for (int i=0; i<n; i++) {
		features[i] = float(rand()) / RAND_MAX; // a random number between 0 and 1
	}
	//std::ostringstream msg;
	//msg << "sending feature vector with " << features.size() << " features.";
	//log->debug(msg.str());
	featureSender->sendFeatureVector(features, meta.getTime());
}


} // namespace dummy
} // namespace components
} // namespace semaine
