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

No class, but only
Functions for simple processing and analysis of pcmBuffers, for example:
          computing energy
          zero-crossings,
          resample
          mixing
          gain
          pan
          ...
          
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
#ifndef __PCM_PROCESS_H
#define __PCM_PROCESS_H

#include "pcm-data.h"

#define WIN_NONE    0
#define WIN_HAMMING 1
#define WIN_HANNING 2

// set up hamming window coefficients for pcmBuffer b
// set recomp = 1 to force recomputing coefficients, even if they have been allocated already
//  (e.g. when chaning the type of the windowing function
void pcmProcess_windowingSetup( pPcmBuffer b, int recomp, int type );

// apply a windowing function to pcmBuffer b
// the function coefficients must be set up first by calling pcmProcess_windowingSetup
void pcmProcess_doWindowing( pPcmBuffer b );

// apply a simple first order pre emphasis filter to the sample data
// *status contains a pointer to an array with nChan elements, which contain the value 
//   of the last sample of the last frame processed for each channel
// preEmphasis is a value from 0.0 to 1.0
void pcmProcess_preEmphasis( pPcmBuffer b, FLOAT_TYPE preEmphasis, WORK_SAMPLE *status ) ;

FLOAT_TYPE pcmProcess_energy( pPcmBuffer p );
double pcmProcess_logEnergy( pPcmBuffer p );


//#define USE_SLOW_DFT   1     // for datasizes != power of 2

#ifdef USE_SLOW_DFT
// returns only complex data:
int pcmProcess_fft( pPcmBuffer pcm, FLOAT_TYPE_FFT *complex_interleaved, FLOAT_TYPE_FFT *ip, FLOAT_TYPE_FFT *w );
// returns only magnitudes and phase:
int pcmProcess_fft_magphase( pPcmBuffer pcm, FLOAT_TYPE_FFT *magnitudes, FLOAT_TYPE_FFT * phases, FLOAT_TYPE_FFT *ip, FLOAT_TYPE_FFT *w );
// returns magnitude, phase and complex number data:
int pcmProcess_fft_All( pPcmBuffer pcm, FLOAT_TYPE_FFT *complex, FLOAT_TYPE_FFT *magnitudes, FLOAT_TYPE_FFT * phases, FLOAT_TYPE_FFT *ip, FLOAT_TYPE_FFT *w );
#else // fft4g code...  datasize must be a power of 2
// returns only complex data:
int pcmProcess_fft( pPcmBuffer pcm, FLOAT_TYPE_FFT *complex_interleaved, int *ip, double *w );
// returns only magnitudes and phase:
int pcmProcess_fft_magphase( pPcmBuffer pcm, FLOAT_TYPE_FFT *magnitudes, FLOAT_TYPE_FFT * phases, int *ip, double *w );
// returns magnitude, phase and complex number data:
int pcmProcess_fft_All( pPcmBuffer pcm, FLOAT_TYPE_FFT *complex, FLOAT_TYPE_FFT *magnitudes, FLOAT_TYPE_FFT * phases, int *ip, double *w );
#endif

#endif // #ifndef __PCM_PROCESS_H
