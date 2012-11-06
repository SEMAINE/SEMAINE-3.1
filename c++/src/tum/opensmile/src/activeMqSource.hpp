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

example dataSource
writes data to data memory...

*/


#ifndef __CACTIVEMQSOURCE_HPP
#define __CACTIVEMQSOURCE_HPP

#include <smileCommon.hpp>
#undef class

#ifdef HAVE_SEMAINEAPI

//#ifdef HAVE_SEMAINE_CONFIG_H  // <- this causes problems in linux
#ifndef _MSC_VER
#include <semaine/config.h>
#endif

#include <cms/CMSException.h>
#include <semaine/components/Component.h>
#include <semaine/cms/receiver/FeatureReceiver.h>

#include <dataSource.hpp>

#define COMPONENT_DESCRIPTION_CACTIVEMQSOURCE "dataSource which reads data vectors from the SEMAINE ActiveMQ server"
#define COMPONENT_NAME_CACTIVEMQSOURCE "cActiveMqSource"

class cActiveMqSource : public cDataSource {
  private:
    int nValues;
    float fStep;
    const char *vadRecv;

    MetaMessenger *meta; 
    int lag; int warned;
    int running;

    cVector *vec;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);
    
    virtual int setupNewNames(long nEl);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cActiveMqSource(const char *_name);
    cDataWriter * getWriter() { return writer; }

    cVector *getVec() { return vec; }

    // this method sets the semaine feature sender to use... do not call it when the system is running, only call it during setup!!
    //void setFeatureReceiver(FeatureReceiver *_receiver, MetaMessenger *_meta=NULL) { featureReceiver = _receiver; meta=_meta; }

    void waveFileReceive(void *data, long long N);

    int isRunning() { return running; }

    virtual ~cActiveMqSource();
};


#endif // HAVE_SEMAINEAPI

#endif // __CACTIVEMQSOURCE_HPP
