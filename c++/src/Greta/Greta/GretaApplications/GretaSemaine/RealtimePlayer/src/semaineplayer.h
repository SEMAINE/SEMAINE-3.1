/*
 *  DummyFeatureExtractor.h
 *  semaine
 *
 *  Created by Marc Schröder on 19.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#ifndef SEMAINE_COMPONENTS_PLAYER_SEMAINEPLAYER_H
#define SEMAINE_COMPONENTS_PLAYER_SEMAINEPLAYER_H

#include "FramesManager.h"
#include "FAPFrame.h"

#include <semaine/config.h>

#include <cms/CMSException.h>

#include <semaine/components/Component.h>
//#include <semaine/cms/message/SEMAINEMessage.h>

using namespace cms;
using namespace semaine::components;
using namespace semaine::cms::sender;
using namespace semaine::cms::receiver;

namespace semaine {
namespace components {
namespace player {

class semaineplayer : public Component
{
public:
	semaineplayer(FramesManager *fm) throw (CMSException);
	virtual ~semaineplayer();
	FramesManager* fm;
	CentralClock pc;

protected:
	virtual void react(SEMAINEMessage * m) throw (CMSException);

private:
	Receiver * fap_bap_Receiver;

};

} // namespace player
} // namespace components
} // namespace semaine


#endif

