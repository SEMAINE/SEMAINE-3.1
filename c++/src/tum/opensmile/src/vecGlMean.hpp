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

compute gliding mean of input vectors and subtract it from the vectors
(used for cepstral mean subtraction (CMS), for example)

OBSOLETE class, use cVectorMVN instead

*/


#ifndef __CVECGLMEAN_HPP
#define __CVECGLMEAN_HPP

#include <smileCommon.hpp>
#include <vectorProcessor.hpp>
#include <math.h>

#define COMPONENT_DESCRIPTION_CVECGLMEAN "(This is the old component for cepstral mean subtraction. It has been replaced by the cVectorMVN component, which is more powerful.) This component computes gliding mean of input vectors and subtracts it from the vectors (use for Cepstral Mean Subtraction, for example)."
#define COMPONENT_NAME_CVECGLMEAN "cVecGlMean"

/* available mean update methods */
#define UPDMETHOD_NUL   0  /* only inital values */
#define UPDMETHOD_EXP   1  /* exponentially decaying mean */
#define UPDMETHOD_FIX   2  /* fixed buffer */
#define UPDMETHOD_AVG   3  /* weighted sum: incremental average and w. inital mean */
#define UPDMETHOD_AVGI  4  /* weighted sum: incremental average and w. inital mean (incl. mean update) */


#undef class
class DLLEXPORT cVecGlMean : public cVectorProcessor {
  private:
    const char * initFile;
    FLOAT_DMEM alpha, weight;
    FLOAT_DMEM * imeans; /* initial means */
    FLOAT_DMEM * means; /* current means */
    long nMeans;

    FLOAT_DMEM * buffer; /* ring-buffer for sums or values (depending on update method) */
    long *bufferNframes; /* ring-buffer of number of frames (update method 'avg'), same size as buffer, same r/w pointers */
    long nAvgBuffer; /* currently present number of elements in above ring buffer */
    long wPtr, rPtr; /* write and read pointers, real indicies */
    long nFrames;

    int updateMethod;
    int turnOnlyNormalise;
    int invertTurn;
    int resetOnTurn;
    int turnOnlyUpdate;

    int zeroThIsLast; // flag that indicates wheter last coeff in initFile is loaded into means[0]
    int skipNfirst;

    double fixedBuffer;
    long fixedBufferFrames;

    int isTurn, resetMeans;
    const char * turnStartMessage;  // name of turnStartMessage
    const char * turnEndMessage;    // name of turnEndMessage

   // cDataReader *auxReader;

    int loadInitCmsFile();
    
  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    //virtual int myTick(long long t);

    virtual int configureWriter(sDmLevelConfig &c);

    virtual int processComponentMessage( cComponentMessage *_msg );
    //virtual int setupNamesForField(int i, const char*name, long nEl);
    //virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cVecGlMean(const char *_name);

    virtual ~cVecGlMean();
};




#endif // __CVECGLMEAN_HPP
