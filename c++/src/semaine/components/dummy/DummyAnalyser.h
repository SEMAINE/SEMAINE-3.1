/*
 *  DummyFeatureExtractor.h
 *  semaine
 *
 *  Created by Marc Schröder on 19.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#ifndef SEMAINE_COMPONENTS_DUMMY_DUMMYANALYSER_H
#define SEMAINE_COMPONENTS_DUMMY_DUMMYANALYSER_H

#include <semaine/config.h>

#include <cms/CMSException.h>

#include <semaine/components/Component.h>
#include <semaine/cms/sender/EmmaSender.h>
#include <semaine/cms/receiver/FeatureReceiver.h>
//#include <semaine/cms/receiver/DialogStateReceiver.h>

using namespace cms;
using namespace semaine::components;
using namespace semaine::cms::sender;
using namespace semaine::cms::receiver;

namespace semaine {
namespace components {
namespace dummy {

class DummyAnalyser : public Component
{
public:
	DummyAnalyser() throw (CMSException);
	virtual ~DummyAnalyser();

protected:
	virtual void react(SEMAINEMessage * m) throw (CMSException);

private:
	FeatureReceiver * featureReceiver;
	EmmaSender * emmaSender;
	//DialogStateReceiver * dialogStateReceiver;
	bool userIsSpeaking;
};

} // namespace dummy
} // namespace components
} // namespace semaine


#endif

