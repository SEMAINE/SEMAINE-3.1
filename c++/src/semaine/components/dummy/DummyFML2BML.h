/*
 *  DummyFML2BML.h
 *  semaine
 *
 *  Created by Marc Schröder on 19.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#ifndef SEMAINE_COMPONENTS_DUMMY_DUMMYFML2BML_H
#define SEMAINE_COMPONENTS_DUMMY_DUMMYFML2BML_H

#include <semaine/config.h>

#include <cms/CMSException.h>

#include <semaine/components/Component.h>
#include <semaine/cms/sender/BMLSender.h>
#include <semaine/cms/receiver/FMLReceiver.h>
#include <semaine/cms/receiver/BMLReceiver.h>


using namespace cms;
using namespace semaine::components;
using namespace semaine::cms::sender;
using namespace semaine::cms::receiver;

namespace semaine {
namespace components {
namespace dummy {

class DummyFML2BML : public Component
{
public:
	DummyFML2BML() throw (CMSException);
	virtual ~DummyFML2BML();

protected:
	virtual void react(SEMAINEMessage * m) throw (CMSException);

private:
	FMLReceiver * fmlReceiver;
	BMLReceiver * bmlReceiver;
	BMLSender * bmlSender;
};

} // namespace dummy
} // namespace components
} // namespace semaine


#endif

