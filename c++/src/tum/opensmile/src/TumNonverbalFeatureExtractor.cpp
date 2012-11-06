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


#include <TumNonverbalFeatureExtractor.h>

#undef MODULE
#define MODULE "TumNonverbalFeatureExtractor.SEMAINEcomponent"

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


    TumNonverbalFeatureExtractor::TumNonverbalFeatureExtractor(cComponentManager *_cMan, cConfigManager *_conf) throw(CMSException) : 
      ComponentForSmile("TumNonverbalFeatureExtractor",_cMan,_conf,true,false)
    {
      if (userinfoSenderInfo.topic != NULL) {
        userinfoSender = new EmmaSender(userinfoSenderInfo.topic, userinfoSenderInfo.componentName );
        senders.push_back(userinfoSender);
      } else {
        // TODO: use semaine exception here...
        SMILE_WRN(1,"userinfoSender.topic == NULL in TumFeatureExtractor, please check semaineCfg section in config file (no emma XML messages with user speaking status and gender will be sent!).");
      }
      
      if (nonverbalSenderInfo.topic != NULL) {
        nonverbalSender = new EmmaSender(nonverbalSenderInfo.topic, nonverbalSenderInfo.componentName );
        senders.push_back(nonverbalSender);
      } else {
        // TODO: use semaine exception here...
        SMILE_WRN(1,"nonverbalSender.topic == NULL in TumFeatureExtractor, please check semaineCfg section in config file (no emma XML messages with non-linguistic vocalisation will be sent!).");
      }



    }



    TumNonverbalFeatureExtractor::~TumNonverbalFeatureExtractor()
    {
      if (cMan != NULL) {
        //cMan->requestAbort();
        //smileThreadJoin( smileMainThread );
      }
    }

    void TumNonverbalFeatureExtractor::customStartIO() throw(CMSException)
    {
      if (cMan == NULL) { SMILE_ERR(1,"componentManager (cMan) is NULL, smileMainThread can not be started!"); }
      else {
        // main openSMILE initialisation
        //cMan->createInstances(0); // 0 = do not read config (we already did that ..)

        //wait for component manager to become ready....
        while (!cMan->isReady()) { smileSleep(100); } // TODO: make the isReady variable in cMan thread safe...!

        /* connect all the feature senders / receivers, etc. */
        setSmileEMMAsender(nonverbalSenderInfo.OSinstanceName, nonverbalSender);
        setSmileEMMAsender(userinfoSenderInfo.OSinstanceName, userinfoSender);
      }
      
    }

    void TumNonverbalFeatureExtractor::react(SEMAINEMessage * m) throw (std::exception) 
    {
    }

    void TumNonverbalFeatureExtractor::act() throw(CMSException)
    {
    }


    } // namespace smile
  } // namespace components
} // namespace semaine


#endif // HAVE_SEMAINEAPI
