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

AEC
*/


#ifndef __CSPEEXPREPROCESS_HPP
#define __CSPEEXPREPROCESS_HPP


#include <smileCommon.hpp>
#include <dataProcessor.hpp>

#ifdef HAVE_SPEEXLIB

#include <speex/speex_echo.h>
#include <speex/speex_preprocess.h>

#define COMPONENT_DESCRIPTION_CSPEEXPREPROCESS "adaptive echo cancellation"
#define COMPONENT_NAME_CSPEEXPREPROCESS "cSpeexPreprocess"

#undef class
class DLLEXPORT cSpeexPreprocess : public cDataProcessor {
  private:
    //cDataReader * readerOutp;
    cMatrix * matnew;

    int enableDenoise, enableAGC, enableVAD, enableDereverb, enableEchoCancel;

    SpeexEchoState *echoSt;
    SpeexPreprocessState *ppState;
    int16_t *ref_buf, *echo_buf, *e_buf;

    double TT;
    int frameInput;
    //double blocksizeS;
    //long blocksize;  // size of echo FIR filter

  protected:
    SMILECOMPONENT_STATIC_DECL_PR


    virtual void fetchConfig();
    virtual void mySetEnvironment();

    virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);

    virtual int configureWriter(sDmLevelConfig &c);

   // buffer must include all (# order) past samples
    //virtual int processBuffer(cMatrix *_in, cMatrix *_out, int _pre, int _post );
    
/*
    virtual int setupNamesForField(int i, const char*name, long nEl);
    virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);
*/
    
  public:
    SMILECOMPONENT_STATIC_DECL
    
    cSpeexPreprocess(const char *_name);

    virtual ~cSpeexPreprocess();
};


#endif // HAVE_SPEEXLIB

#endif // __CSPEEXPREPROCESS_HPP
