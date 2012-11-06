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

#include <smileComponent.hpp>


#define COMPONENT_DESCRIPTION_CSEMAINETEXTOUTPUT "the SEMAINE text output component for openSMILE"
#define COMPONENT_NAME_CSEMAINETEXTOUTPUT "cSemaineTextOutput"

#define N_GENDER_HISTORY 14
#define GENDER_M  1
#define GENDER_F  2

#undef class
class cSemaineTextOutput : public cSmileComponent {
  private:
//    EmmaSender *emmaSender;
//    MetaMessenger *meta; 

    int kw[2];
    const char* outputFileName;
    const char* emoOutputFileName;
    FILE* filehandle;
    FILE* emoFilehandle;
    int append;
    int appendEmo;

    // gender majority vote
    int gender[N_GENDER_HISTORY];
    int gPtr;

    void sendArousalC( cComponentMessage *_msg );
    void sendValenceC( cComponentMessage *_msg );
    void sendInterestC( cComponentMessage *_msg );
    void sendPowerC( cComponentMessage *_msg );
    void sendIntensityC( cComponentMessage *_msg );
    void sendAnticipationC( cComponentMessage *_msg );
    void sendSpeakingStatus( cComponentMessage *_msg, int status );
    void sendGender( cComponentMessage *_msg );
    void sendPitchDirection( cComponentMessage *_msg );
    void sendKeywords( cComponentMessage *_msg );

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    /*
    virtual void mySetEnvironment();
    virtual int myConfigureInstance();
    */
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);
    virtual int processComponentMessage( cComponentMessage *_msg );

    // get smileTime from _msg, convert to semaine time by getting current time from smile and semaine and observing the difference
    //long long smileTimeToSemaineTime( double smileTime ) ;
    //void sendDocument( XERCESC_NS::DOMDocument * document );

  public:
    SMILECOMPONENT_STATIC_DECL

    cSemaineTextOutput(const char *_name);
  
    // this method sets the semaine emma sender to use... do not call it when the system is running, only call it during setup!!
    //void setEmmaSender(EmmaSender *_sender, MetaMessenger *_meta=NULL) { emmaSender = _sender; meta=_meta; }

    virtual ~cSemaineTextOutput();
};


#endif // __CSEMAINETEXTOUTPUT_HPP
