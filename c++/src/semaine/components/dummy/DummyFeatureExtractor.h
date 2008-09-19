/*
 *  DummyFeatureExtractor.h
 *  semaine
 *
 *  Created by Marc Schröder on 19.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#ifndef SEMAINE_COMPONENTS_DUMMY_DUMMYFEATUREEXTRACTOR_H
#define SEMAINE_COMPONENTS_DUMMY_DUMMYFEATUREEXTRACTOR_H

#include <semaine/config.h>

#include <cms/CMSException.h>

#include <semaine/components/Component.h>
#include <semaine/cms/sender/FeatureSender.h>

using namespace cms;
using namespace semaine::components;
using namespace semaine::cms::sender;

namespace semaine {
namespace components {
namespace dummy {

class DummyFeatureExtractor : public Component
{
public:
	DummyFeatureExtractor() throw (CMSException);
	virtual ~DummyFeatureExtractor();

protected:
	virtual void act() throw (CMSException);

private:
	FeatureSender * featureSender;
};

} // namespace dummy
} // namespace components
} // namespace semaine


#endif

