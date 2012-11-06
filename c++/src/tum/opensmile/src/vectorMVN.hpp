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

This component extends the base class cVectorTransform and implements mean/variance normalisation

*/


#ifndef __CVECTORMVN_HPP
#define __CVECTORMVN_HPP

#include <smileCommon.hpp>
#include <vectorTransform.hpp>
#include <math.h>

#define COMPONENT_DESCRIPTION_CVECTORMVN "This component extends the base class cVectorTransform and implements mean/variance normalisation. You can use this component to perform on-line cepstral mean normalisation. See cFullinputMean for off-line cepstral mean normalisation."
#define COMPONENT_NAME_CVECTORMVN "cVectorMVN"

/* we define some transform type IDs, other will be defined in child classes */
//#define TRFTYPE_CMN     10    /* mean normalisation, mean vector only */
//#define TRFTYPE_MVN     20    /* mean variance normalisation, mean vector + stddev vector */
//#define TRFTYPE_UNDEF    0    /* undefined, or custom type */

#define STDDEV_FLOOR  0.0000001


#undef class
class DLLEXPORT cVectorMVN : public cVectorTransform {
  private:
    FLOAT_DMEM specFloor;
    int spectralFlooring, subtractMeans;
    int meanEnable;
    int stdEnable, normEnable;
    int htkLogEnorm;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR
      
    /**** refactoring hooks ****/
    /* allocate the memory for vectors and userData, initialise header
       the only field that has been set is head.vecSize ! 
       This function is only called in modes analysis and incremental if no data was loaded */
    virtual void allocTransformData(struct sTfData *tf, int Ndst, int idxi);
    /* For mode == ANALYSIS or TRANSFORMATION, this functions allows for a final processing step
       at the end of input and just before the transformation data is saved to file */
    //virtual void computeFinalTransformData(struct sTfData *tf, int idxi) {}

    /* this will be called BEFORE the transform will be reset to initial values (at turn beginning/end) 
       you may modify the initial values here or the new values, 
       if you return 1 then no further changes to tf will be done,
       if you return 0 then tf0 will be copied to tf after running this function */
    //virtual int transformResetNotify(struct sTfData *tf, struct sTfData *tf0) { return 0; }

    /* Do the actual transformation (do NOT use this to calculate parameters!) 
       This function will only be called if not in ANALYSIS mode 
       Please return the number of output samples (0, if you haven't produced output) */
    virtual int transformDataFloat(const struct sTfData * tf, const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

    //virtual int transformDataInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);

    /* Update transform parameters incrementally
       This function will only be called if not in TRANSFORMATIONs mode 
       *buf is a pointer to a buffer if updateMethod is fixedBuffer */
    // return value: 0: no update was performed , 1: successfull update
    virtual int updateTransformFloatExp(struct sTfData * tf, const FLOAT_DMEM *src, int idxi);
    virtual int updateTransformFloatBuf(struct sTfData * tf, const FLOAT_DMEM *src, FLOAT_DMEM *buf, long Nbuf, long wrPtr, int idxi);
    virtual int updateTransformFloatAvg(struct sTfData * tf, const FLOAT_DMEM *src, int idxi);
    virtual int updateTransformFloatAvgI(struct sTfData * tf, const FLOAT_DMEM *src, FLOAT_DMEM *buf, long * bufferNframes, long Nbuf, long wrPtr, int idxi);

    /* generic method, default version will select one of the above methods,
       overwrite to implement your own update strategy ('usr' option) */
    //virtual int updateTransformFloat(struct sTfData * tf, const FLOAT_DMEM *src, FLOAT_DMEM *buf, long * bufferNframes, long Nbuf, int idxi);

/////////////////////////////////////////////
    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    //virtual int myFinaliseInstance();
    //virtual int myTick(long long t);

    //virtual int configureWriter(sDmLevelConfig &c);

    //virtual int processComponentMessage( cComponentMessage *_msg );
    //virtual int setupNamesForField(int i, const char*name, long nEl);
    //virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    //virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    //virtual int flushVectorFloat(FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cVectorMVN(const char *_name);

    virtual ~cVectorMVN();
};




#endif // __CVECTORMVN_HPP
