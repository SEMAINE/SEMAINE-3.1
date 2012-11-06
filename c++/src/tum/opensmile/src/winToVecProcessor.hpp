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

reads in windows and outputs vectors (frames)

*/


#ifndef __CWINTOVECPROCESSOR_HPP
#define __CWINTOVECPROCESSOR_HPP

#include <smileCommon.hpp>
#include <dataProcessor.hpp>

#define COMPONENT_DESCRIPTION_CWINTOVECPROCESSOR "reads input windows, outputs frame(s)"
#define COMPONENT_NAME_CWINTOVECPROCESSOR "cWinToVecProcessor"

#define FRAME_MSG_QUE_SIZE 10
#define MATBUF_ALLOC_STEP  200

#define FRAMEMODE_FIXED 0 
#define FRAMEMODE_FULL  1
#define FRAMEMODE_VAR   2
#define FRAMEMODE_LIST  3
#define FRAMEMODE_META  4


#undef class
class DLLEXPORT cWinToVecProcessor : public cDataProcessor {
  private:
    int   frameMode;
    int   fsfGiven;   // flag that indicates whether frameSizeFrame, etc. was specified directly (to override frameSize in seconds)
    int   fstfGiven;   // flag that indicates whether frameStepFrame, etc. was specified directly (to override frameStep in seconds)
    int   dtype;     // data type (DMEM_FLOAT, DMEM_INT)
    int   noPostEOIprocessing;
    int   nIntervals;  // number of intervals for frameMode = list
    double *ivSec; //interleaved array : start/end in seconds (frameList, frameMode = list)
    long *ivFrames; //interleaved array : start/end in frames (frameList, frameMode = list)

    long Ni, Nfi;
    long No; //, Nfo;
    long Mult;
    long matBufN; // number of valid elements in matbuf
    long matBufNalloc; // backup of matbuf->nT
    char * lastText, *lastCustom;
    double inputPeriod;
    
    cMatrix *matBuf;
    cMatrix *tmpRow;
    cVector *tmpVec;
    FLOAT_DMEM *tmpFrameF;
    INT_DMEM *tmpFrameI;

    //mapping of field indicies to config indicies: (size of these array is maximum possible size: Nfi)
    int Nfconf;
    int *fconf, *fconfInv;
    long *confBs;  // blocksize for configurations

    int addFconf(long bs, int field); // return value is index of assigned configuration
    
    // message memory:
    int nQ; // number of frames quequed
    double Qstart[FRAME_MSG_QUE_SIZE]; // start time array
    double Qend[FRAME_MSG_QUE_SIZE];   // end time array
    int Qflag[FRAME_MSG_QUE_SIZE];     // final frame flag (turn end..)
    int getNextFrameData(double *start, double *end, int *flag);
    int peakNextFrameData(double *start, double *end, int *flag);
    int clearNextFrameData();
    int queNextFrameData(double start, double end, int flag);
    void addVecToBuf(cVector *ve);

  protected:
    int   wholeMatrixMode; // this is 0 by default, but child classes can overwrite it, if they implement the doProcessMatrix method instead of the doProcess method
    int   processFieldsInMatrixMode; // <- this variable is set from the config by the child class, the config option must also be populated by the child class; by default the value is 0

    double frameSize, frameStep, frameCenter;
    long  frameSizeFrames, frameStepFrames, frameCenterFrames, pre;

    SMILECOMPONENT_STATIC_DECL_PR
    double getInputPeriod(){return inputPeriod;}
    long getNi() { return Ni; } // number of elements
    long getNfi() { return Nfi; } // number of fields
    long getNf() { return Ni; } // return whatever will be processed (Ni: winToVec, Nfi, vecProc.)
    
    // (input) field configuration, may be used in setupNamesForField
    int getFconf(int field) { return fconf[field]; } // caller must check for return value -1 (= no conf available for this field)
    void multiConfFree( void * x );
    void *multiConfAlloc() {
      return calloc(1,sizeof(void*)*getNf());
    }
    
    virtual void fetchConfig();
    //virtual int myFinaliseInstance();
    //virtual int myConfigureInstance();
    virtual int dataProcessorCustomFinalise();
    virtual int myTick(long long t);

    // this must return the multiplier, i.e. the vector size returned for each input element (e.g. number of functionals, etc.)
    virtual int getMultiplier();
    
    // get the number of outputs given a number of inputs, 
    // this is used for wholeMatrixMode, where the quantitative relation between inputs and outputs may be non-linear
    // this replaces getMultiplier, which is used in the standard row-wise processing mode
    virtual long getNoutputs(long nIn);

    virtual int configureWriter(sDmLevelConfig &c);
    virtual int setupNamesForElement(int idxi, const char*name, long nEl);
    //virtual int setupNamesForField(int idxi, const char*name, long nEl);
    virtual int doProcessMatrix(int i, cMatrix *in, FLOAT_DMEM *out, long nOut);
    virtual int doProcessMatrix(int i, cMatrix *in, INT_DMEM *out, long nOut);
    virtual int doProcess(int i, cMatrix *row, FLOAT_DMEM*x);
    virtual int doProcess(int i, cMatrix *row, INT_DMEM*x);
    virtual int doFlush(int i, FLOAT_DMEM*x);
    virtual int doFlush(int i, INT_DMEM*x);

    virtual int processComponentMessage( cComponentMessage *_msg );

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cWinToVecProcessor(const char *_name);

    virtual ~cWinToVecProcessor();
};




#endif // __CWINTOVECPROCESSOR_HPP
