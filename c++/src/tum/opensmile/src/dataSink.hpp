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

dataSink

*/


#ifndef __DATA_SINK_HPP
#define __DATA_SINK_HPP

#include <smileCommon.hpp>
#include <smileComponent.hpp>
#include <dataReader.hpp>

#define COMPONENT_DESCRIPTION_CDATASINK "This is a base class for components reading from (and not writing to) the dataMemory and dumping/passing data to external entities."
#define COMPONENT_NAME_CDATASINK "cDataSink"

#undef class
class DLLEXPORT cDataSink : public cSmileComponent {
  private:

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    long blocksizeR;
    double blocksizeR_sec;

    cDataReader *reader;
    virtual void fetchConfig();
    virtual void mySetEnvironment();
    virtual int myRegisterInstance(int *runMe=NULL);
    virtual int myConfigureInstance();
    virtual int myFinaliseInstance();

    // *runMe return value for component manager : 0, don't call my tick of this component, 1, call myTick
    virtual int runMeConfig() { return 1; }

    // configure the reader, i.e. set blocksize requests etc.
    virtual int configureReader();

  public:
    SMILECOMPONENT_STATIC_DECL

    cDataSink(const char *_name);
    virtual ~cDataSink();
};




#endif // __DATA_SINK_HPP
