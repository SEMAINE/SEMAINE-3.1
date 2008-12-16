/*******************************************************************************
 * feaTUM, fast, efficient audio feature extractor by TUM
 * Copyright (C) 2008  Florian Eyben, Martin Woellmer
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
 *******************************************************************************/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

This class manages the temporary features ringbuffer, that is used by the feature
extractors to gain access to feature data of past frames. This avoids multiple
internal storage and recomputation of various shared feature data such as FFT
coefficients.

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
#ifndef __LLDEX_H
#define __LLDEX_H

// which extractors to enable at compile time.... (not yet completely supported..)
#define FT_LLD_ENERGY          
#define FT_LLD_FFT             
#define FT_LLD_PITCH
#define FT_LLD_TIME
#define FT_LLD_LPC
#define FT_LLD_MFCC
#define FT_LLD_MFCCZ
#define FT_LLD_CEPSTRUM
#define FT_LLD_CEPSTRALF0
#define FT_LLD_SPECTRAL
//#define FT_LLD_VQ

#include "input-framer.h"

typedef struct LLDexElement sLLDexElement;
typedef sLLDexElement *pLLDexElement;

typedef struct {
  pLLDexElement **history;   // [frame id/level][ringbuff index]
  pLLDexElement *current;  // [frame id/level]
  int *historyRbIdx;  // history Ringbuffer index for each id
  int *nHistory;      // history size for each id
  int nIDs;            // number of ids
} sLLDex;
typedef sLLDex * pLLDex;

#define MANDATORY_LLDEX_HEADER  int (*freeLLDex)(pLLDexElement);  int flush_status;      // this line is mandatory in all LLDex structs
typedef struct {
   MANDATORY_LLDEX_HEADER        // this line is mandatory in all LLDex structs
} sLLDexGeneric;
typedef sLLDexGeneric * pLLDexGeneric;


/********************************************************/
// include all LLD features here:
#ifdef FT_LLD_ENERGY
#include "LLDenergy.h"
#endif

#ifdef FT_LLD_FFT
#include "LLDfft.h"
#endif

#ifdef FT_LLD_PITCH
#include "LLDpitch.h"
#endif

#ifdef FT_LLD_MFCC
#include "LLDmfcc.h"
#endif

#ifdef FT_LLD_MFCCZ
#include "LLDmfccz.h"
#endif

#ifdef FT_LLD_TIME
#include "LLDtime.h"
#endif

#ifdef FT_LLD_CEPSTRUM
#include "LLDcepstrum.h"
#endif

#ifdef FT_LLD_CEPSTRALF0
#include "LLDcepstralF0.h"
#endif

#ifdef FT_LLD_LPC
#include "LLDlpc.h"
#endif

#ifdef FT_LLD_SPECTRAL
#include "LLDspectral.h"
#endif

#ifdef FT_LLD_VQ
#include "LLDvq.h"
#endif

// ...more....


struct LLDexElementGeneric {
  int              pcmAutoFree;
  FLOAT_TYPE_TIME  timestamp;
  pPcmBuffer       pcm;
  // user defined features below here.....:
  pLLDexGeneric    el1;
} ;
typedef struct LLDexElementGeneric sLLDexElementGeneric;
typedef sLLDexElementGeneric *pLLDexElementGeneric;

// make sure the header parts of the above genric struct and the one below are equal!
struct LLDexElement {
  int              pcmAutoFree;
  FLOAT_TYPE_TIME  timestamp;
  pPcmBuffer       pcm;
  // user defined features below here.....:
#ifdef FT_LLD_ENERGY
  pLLDexEnergy     energy; 
#endif

#ifdef FT_LLD_FFT
  pLLDexFFT        fft;
#endif

#ifdef FT_LLD_PITCH
  pLLDexPitch      pitch;
#endif

#ifdef FT_LLD_MFCC
  pLLDexMfcc       mfcc;
#endif

#ifdef FT_LLD_MFCCZ
  pLLDexMfccz       mfccz;
#endif

#ifdef FT_LLD_TIME
  pLLDexTime       time;
#endif

#ifdef FT_LLD_CEPSTRUM
  pLLDexCepstrum   cepstrum;
#endif

#ifdef FT_LLD_CEPSTRALF0
  pLLDexCepstralF0   cepstralF0;
#endif
  
#ifdef FT_LLD_LPC
  pLLDexLpc       lpc;
#endif

#ifdef FT_LLD_SPECTRAL
  pLLDexSpectral  spectral;
#endif

#ifdef FT_LLD_VQ
  pLLDexVq  vq;
#endif

  // ... more ...
  
  /*  pFtZeroCrossings zeroCrossings;
  pLldPitch         pitch;
  pLldFormants      formants;
  pLldMFCC          mfcc;
  //...
  */
} ;

/********************************************************/


// ????  size of nHistory?? or unused??
#define LLDEX_FLUSHED 10



// constructor: create and initialise ftex struct
// if framer != NULL, nIDs is ignored, otherwise it is used instead of the nClients value from framer
// nHistory is a pointer to an array of size nIDs holding the number of frames to keep for each level
pLLDex LLDex_create( pLLDex obj, pInputFramer framer, int nIDs, int *nHistory ); 

// add a frame for id/level  and return corresponding pFtexElement
pLLDexElement LLDex_addFrame( pLLDex obj, int level, pPcmBuffer pcm );

// nBack == 0 : current element (= last element that was added)
// nBack == -1 :  element at t - 1
// ...
pLLDexElement LLDex_getHistory( pLLDex obj, int id, int nBack );

// completely destroy LLDex obj
pLLDex LLDex_destroy( pLLDex obj );
pLLDex LLDex_destroyData( pLLDex obj );

// low-level functions:
             
// create an LLDex element, copy pcmBuffer and store in LLDex object
pLLDexElement LLDex_createElement( pPcmBuffer b );

// free element an all associated data within
pLLDexElement LLDex_destroyElement( pLLDexElement obj );



#endif // #ifndef __LLDEX_H
