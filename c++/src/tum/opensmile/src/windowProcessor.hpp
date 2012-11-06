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

filter :  (abstract class only)
       linear N-th order filter for single value data streams
       this class processed every element of a frame independently
       derived classes only need to implement the filter algorithm

*/


#ifndef __CWINDOWPROCESSOR_HPP
#define __CWINDOWPROCESSOR_HPP

#include <smileCommon.hpp>
#include <dataProcessor.hpp>

#define COMPONENT_DESCRIPTION_CWINDOWPROCESSOR "filter dataProcessor, filters each element in a dataMemory level independently"
#define COMPONENT_NAME_CWINDOWPROCESSOR "cWindowProcessor"

#undef class
class DLLEXPORT cWindowProcessor : public cDataProcessor {
  private:
    //int blocksize;         // block size for filter (speed up purpose only)
    long Ni;
    int isFirstFrame;
    int pre, post, winsize;
    int noPostEOIprocessing;
    
    cMatrix * matnew;
    cMatrix * rowout;
    cMatrix * row;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    int multiplier;

    void setWindow(int _pre, int _post);
    int firstFrame() { return isFirstFrame; }

    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    //virtual int myFinaliseInstance();
    virtual int myTick(long long t);

    virtual int configureWriter(sDmLevelConfig &c);


   // buffer must include all (# order) past samples
    virtual int processBuffer(cMatrix *_in, cMatrix *_out,  int _pre, int _post );
    virtual int processBuffer(cMatrix *_in, cMatrix *_out,  int _pre, int _post, int rowGlobal );
    virtual int dataProcessorCustomFinalise();

/*
    virtual int setupNamesForField(int i, const char*name, long nEl);
    virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);
*/
    
  public:
    SMILECOMPONENT_STATIC_DECL
    
    cWindowProcessor(const char *_name, int _pre=0, int _post=0);

    virtual ~cWindowProcessor();
};




#endif // __CWINDOWPROCESSOR_HPP
