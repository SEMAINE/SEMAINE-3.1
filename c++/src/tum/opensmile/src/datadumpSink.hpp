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

ARFF file output (for WEKA)

*/


#ifndef __CDATADUMPSINK_HPP
#define __CDATADUMPSINK_HPP

#include <smileCommon.hpp>
#include <dataSink.hpp>

#define COMPONENT_DESCRIPTION_CDATADUMPSINK "This component writes dataMemory data to a raw binary file (e.g. for matlab import). The binary file consits of 32-bit float values representing the data values, concattenated frame by frame along the time axis. The first two float values in the file resemble the file header, an thus indicate the dimension of the matrix (1: size of frames, 2: number of frames in file). The total file size in bytes is thus <size of frames>x<number of frames>x4 + 2."
#define COMPONENT_NAME_CDATADUMPSINK "cDatadumpSink"

#undef class
class DLLEXPORT cDatadumpSink : public cDataSink {
  private:
    FILE * filehandle;
    const char *filename;
    int lag;
    int append;
    long nVec,vecSize;
    
    void writeHeader();
    
  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);


  public:
    //static sComponentInfo * registerComponent(cConfigManager *_confman);
    //static cSmileComponent * create(const char *_instname);
    SMILECOMPONENT_STATIC_DECL
    
    cDatadumpSink(const char *_name);

    virtual ~cDatadumpSink();
};




#endif // __CDATADUMPSINK_HPP
