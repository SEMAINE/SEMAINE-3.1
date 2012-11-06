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


*/


#ifndef __DATA_SOURCE_HPP
#define __DATA_SOURCE_HPP

#include <smileCommon.hpp>
#include <smileComponent.hpp>
#include <dataWriter.hpp>

#define COMPONENT_DESCRIPTION_CDATASOURCE "This is a base class for components, which write data to dataMemory, but do not read from it."
#define COMPONENT_NAME_CDATASOURCE "cDataSource"

#undef class
class DLLEXPORT cDataSource : public cSmileComponent {
  private:

  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    cDataWriter *writer;
    cVector     *vec;    // current vector to be written
    cMatrix     *mat;    // current matrix to be written
    double buffersize_sec; /* buffersize of write level, as requested by config file (in seconds)*/
    double blocksizeW_sec; /* blocksizes for block processing (in seconds)*/
    long buffersize; /* buffersize of write level, as requested by config file (in frames)*/
    long blocksizeW; /* blocksizes for block processing (in frames)*/
    double period; /* period, as set by config */

    int namesAreSet;

    virtual void fetchConfig();
    virtual void mySetEnvironment();
    virtual int myRegisterInstance(int *runMe=NULL);
    virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    void allocVec(int n);
    void allocMat(int n, int t);


    // *runMe return value for component manager : 0, don't call my tick of this component, 1, call myTick
    virtual int runMeConfig() { return 1; }

    virtual int configureWriter(sDmLevelConfig &c) { return 1; }
    
    virtual int setupNewNames(long nEl=0) { return 1; }

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cDataSource(const char *_name);
    virtual ~cDataSource();
};




#endif // __DATA_SOURCE_HPP
