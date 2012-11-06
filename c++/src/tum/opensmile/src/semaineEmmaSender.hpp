/*F******************************************************************************
 *
 * openSMILE - open Speech and Music Interpretation by Large-space Extraction
 *       the open-source Munich Audio Feature Extraction Toolkit
 * Copyright (C) 2008-2009  Florian Eyben, Martin Woellmer, Bjoern Schuller
 *
 *
 * Institute for Human-Machine Communication
 * Technische Universitaet Muenchen (TUM)
 * D-80333 Munich, Germany
 *
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
 *
 ******************************************************************************E*/


/*  openSMILE component:

the SEMAINE Emma message sender for openSMILE


TODO: move to SEMAINExtract project ... ?

*/


#ifndef __CSEMAINEEMMASENDER_HPP
#define __CSEMAINEEMMASENDER_HPP

#include <smileCommon.hpp>

#ifdef HAVE_SEMAINEAPI

#include <smileComponent.hpp>
#ifndef _MSC_VER
#include <semaine/config.h>
#endif
#include <cms/CMSException.h>
#include <semaine/components/Component.h>
#include <semaine/cms/sender/EmmaSender.h>

#include <semaine/util/XMLTool.h>
#include <semaine/datatypes/xml/EMMA.h>
#include <semaine/datatypes/xml/EmotionML.h>
#include <semaine/datatypes/xml/SemaineML.h>

#define COMPONENT_DESCRIPTION_CSEMAINEEMMASENDER "the SEMAINE Emma message sender for openSMILE"
#define COMPONENT_NAME_CSEMAINEEMMASENDER "cSemaineEmmaSender"

using namespace semaine::cms::sender;
using namespace semaine::util;
using namespace semaine::datatypes::xml;
using namespace XERCES_CPP_NAMESPACE;

#define XERCESC_NS XERCES_CPP_NAMESPACE

/*
#define N_GENDER_HISTORY 14
#define GENDER_M  1
#define GENDER_F  2
*/

#undef class
class cSemaineEmmaSender : public cSmileComponent {
  private:
    EmmaSender *emmaSender;
    MetaMessenger *meta; 

    // gender majority vote
    /*
    int gender[N_GENDER_HISTORY];
    int gPtr;
*/

    //void sendArousalC( cComponentMessage *_msg );
    //void sendValenceC( cComponentMessage *_msg );
    //void sendInterestC( cComponentMessage *_msg );
    //void sendPowerC( cComponentMessage *_msg );
    //void sendIntensityC( cComponentMessage *_msg );
    //void sendAnticipationC( cComponentMessage *_msg );
    //void sendSingleEmotionDim( cComponentMessage *_msg, const char * set, std::string dim );
    /*
    void sendDimensionFSRE( cComponentMessage *_msg, std::string dimensionStr )
    {
      sendDimension( _msg , dimensionStr, EmotionML::VOC_FSRE_DIMENSION_DEFINITION );
    }
*/

//    void sendDimension( cComponentMessage *_msg, std::string dimensionStr, std::string dimensionVoc );
    //void sendSpeakingStatus( cComponentMessage *_msg, int status );
    //void sendGender( cComponentMessage *_msg );
    //void sendPitchDirection( cComponentMessage *_msg );
    //void sendKeywords( cComponentMessage *_msg );

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig() {}
    /*
    virtual void mySetEnvironment();
    virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    */
    virtual int myTick(long long t);
    //virtual int processComponentMessage( cComponentMessage *_msg );

    // get smileTime from _msg, convert to semaine time by getting current time from smile and semaine and observing the difference
    long long smileTimeToSemaineTime( double smileTime ) ;
    void sendDocument( XERCESC_NS::DOMDocument * document );
    
    EmmaSender * getEmmaSender() { return emmaSender; }

  public:
    SMILECOMPONENT_STATIC_DECL

    cSemaineEmmaSender(const char *_name);
  
    // this method sets the semaine emma sender to use... do not call it when the system is running, only call it during setup!!
    void setEmmaSender(EmmaSender *_sender, MetaMessenger *_meta=NULL) { emmaSender = _sender; meta=_meta; }

    virtual ~cSemaineEmmaSender();
};


#endif // HAVE_SEMAINEAPI

#endif // __CSEMAINEEMMASENDER_HPP
