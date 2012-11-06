/*******************************************************************************
* openSMILE
*  - open Speech and Music Interpretation by Large-space Extraction -
* Copyright (C) 2008  Florian Eyben, Martin Woellmer, Bjoern Schuller
* 
* Institute for Human-Machine Communication
* Technische Universitaet Muenchen (TUM)
* D-80333 Munich, Germany
*
* If you use openSMILE or any code from openSMILE in your research work,
* you are kindly asked to acknowledge the use of openSMILE in your publications.
* See the file CITING.txt for details.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/


/*
*  TumOpenSMILE.cpp
*  semaine
*  
*/
#include <smileCommon.hpp>

#ifdef HAVE_SEMAINEAPI


#include <TumInterestDetector.h>

#undef MODULE
#define MODULE "TumInterestDetector.SEMAINEcomponent"

#include <cstdlib>
#include <sstream>

#include <semaine/util/XMLTool.h>
#include <semaine/datatypes/xml/EMMA.h>
#include <semaine/datatypes/xml/EmotionML.h>
#include <semaine/datatypes/xml/SemaineML.h>

using namespace semaine::util;
using namespace semaine::datatypes::xml;
using namespace XERCES_CPP_NAMESPACE;

namespace semaine {
  namespace components {
    namespace smile {


    TumInterestDetector::TumInterestDetector(cComponentManager *_cMan, cConfigManager *_conf) throw(CMSException) : 
      ComponentForSmile("TumInterestDetector",_cMan,_conf,true,false)
    {
      if (emotionSenderInfo.topic != NULL) {
        emotionSender = new EmmaSender(emotionSenderInfo.topic, emotionSenderInfo.componentName );
        senders.push_back(emotionSender);
      } else {
        // TODO: use semaine exception here...
        SMILE_WRN(1,"emotionSender.topic == NULL in TumFeatureExtractor, please check semaineCfg section in config file (no emma XML messages with dimensional affect recognition results will be sent!).");
      }

      // Marc, 21 Dec 08: Deactivated, because it leads to dropped messages when the ASR
      // is on a different machine where the clock time is not the same.
      //featureSender->setTimeToLive(100); // discard messages after 100 ms

    }




    TumInterestDetector::~TumInterestDetector()
    {
      if (cMan != NULL) {
        //cMan->requestAbort();
        //smileThreadJoin( smileMainThread );
      }
    }

    void TumInterestDetector::customStartIO() throw(CMSException)
    {
      if (cMan == NULL) { SMILE_ERR(1,"componentManager (cMan) is NULL, smileMainThread can not be started!"); }
      else {
        // main openSMILE initialisation
        //cMan->createInstances(0); // 0 = do not read config (we already did that ..)

        // wait for component manager to become ready....
        while (!cMan->isReady()) { smileSleep(100); } // TODO: make the isReady variable in cMan thread safe...!

        /* connect all the feature senders / receivers, etc. */
        // NOTE: The emotion analyser already sets this.... (can we override it??)
        setSmileEMMAsender(emotionSenderInfo.OSinstanceName, emotionSender);

      }
      
    }

    void TumInterestDetector::react(SEMAINEMessage * m) throw (std::exception) 
    {
  
    }

    void TumInterestDetector::act() throw(CMSException)
    {
  
    }


    } // namespace smile
  } // namespace components
} // namespace semaine


#endif // HAVE_SEMAINEAPI
