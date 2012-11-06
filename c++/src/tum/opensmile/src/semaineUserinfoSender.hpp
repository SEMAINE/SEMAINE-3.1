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

The SEMAINE keyword sender

*/


#ifndef __CSEMAINEUSERINFOSENDER_HPP
#define __CSEMAINEUSERINFOSENDER_HPP

#include <smileCommon.hpp>

#ifdef HAVE_SEMAINEAPI

#include <smileComponent.hpp>
#include <semaineEmmaSender.hpp>

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

#define COMPONENT_DESCRIPTION_CSEMAINEUSERINFOSENDER "the SEMAINE Emma message sender for sending keywords (ASR output)"
#define COMPONENT_NAME_CSEMAINEUSERINFOSENDER "cSemaineUserinfoSender"

using namespace semaine::cms::sender;
using namespace semaine::util;
using namespace semaine::datatypes::xml;
using namespace XERCES_CPP_NAMESPACE;

#define XERCESC_NS XERCES_CPP_NAMESPACE


#define N_GENDER_HISTORY 14
#define GENDER_M  1
#define GENDER_F  2


#undef class
class cSemaineUserinfoSender : public cSemaineEmmaSender {
  private:
    // gender majority vote
    int gender[N_GENDER_HISTORY];
    int gPtr;

    void sendSpeakingStatus( cComponentMessage *_msg, int status );
    void sendGender( cComponentMessage *_msg );
    void sendPitchDirection( cComponentMessage *_msg );

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig() {}
    virtual int processComponentMessage( cComponentMessage *_msg );

   
  public:
    SMILECOMPONENT_STATIC_DECL

    cSemaineUserinfoSender(const char *_name);
    virtual ~cSemaineUserinfoSender() {}

};


#endif // HAVE_SEMAINEAPI

#endif // __CSEMAINEUSERINFOSENDER_HPP
