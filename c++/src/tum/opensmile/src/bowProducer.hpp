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


#ifndef __CBOWPRODUCER_HPP
#define __CBOWPRODUCER_HPP

#include <smileCommon.hpp>
#include <dataSource.hpp>

#define COMPONENT_DESCRIPTION_CBOWPRODUCER "This component produces a bag-of-words vector from the keyword spotter result message."
#define COMPONENT_NAME_CBOWPRODUCER "cBowProducer"

#undef class
class DLLEXPORT cBowProducer : public cDataSource {
  private:
    const char *kwList;
    const char *prefix;
    int count;
    int dataFlag;
    int kwListPrefixFilter;

    int numKw;
    char ** keywords;
    
    int loadKwList();
    int buildBoW( cComponentMessage *_msg );

  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    //virtual int myFinaliseInstance();
    virtual int processComponentMessage( cComponentMessage *_msg );
    virtual int myTick(long long t);

    virtual int configureWriter(sDmLevelConfig &c);
    virtual int setupNewNames(long nEl);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cBowProducer(const char *_name);

    virtual ~cBowProducer();
};




#endif // __CBOWPRODUCER_HPP
