/*
 *  ASR.h
 *  semaine
 *
 *  Created by Martin Wöllmer on 13.11.08
 */

#ifndef SEMAINE_COMPONENTS_ASR_ASR_H
#define SEMAINE_COMPONENTS_ASR_ASR_H

#include <semaine/config.h>

#include <cms/CMSException.h>

#include <semaine/components/Component.h>
#include <semaine/cms/sender/EmmaSender.h>

#include <tum/asr/ATKLib/AMonitor.h>
#include <tum/asr/ATKLib/ASource.h>
#include <tum/asr/ATKLib/ACode.h>
#include <tum/asr/ATKLib/ARec.h>

using namespace cms;
using namespace semaine::components;
using namespace semaine::cms::sender;

namespace semaine {
namespace components {
namespace asr {

class ASR : public Component
{
public:
	ASR() throw (CMSException);
	virtual ~ASR();

protected:
	virtual void act() throw (CMSException);
	virtual void customStartIO() throw (CMSException);

private:
	EmmaSender * emmaSender;
	APacket * p;
	ABuffer * auChan;
	ABuffer * feChan;
	ABuffer * ansChan;
	ARMan * rman;
	ASource * ain;
	ACode * acode;
	ARec * arec;
	AHmms * hset;
	ADict * dict;
	AGram * gram;
	ResourceGroup *main;
	ANGram * ngram;
	AMonitor * amon;
};

} // namespace asr
} // namespace components
} // namespace semaine


#endif

