/*******************************************************************************
 * openSMILE
 *  - open Speech and Music Interpretation by Large-space Extraction -
 * Copyright (C) 2008  Florian Eyben, Martin Woellmer, Bjoern Schuller
 * 
 * Institute for Human-Machine Communication
 * Technische Universitaet Muenchen (TUM)
 * D-80333 Munich, Germany
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
 *******************************************************************************/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

global config variables for LLD extractors

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#include "featum_common.h"

#include <stdio.h>
#include "LLDs.h"


//**** define the total number of implemented llds here: ******
// this must match the number of elements in the sLLDs LLD = {} struct below!!
//#define TOTAL_NUM_LLDS 11
// --> moved to another file

/****************************************************************************/

#ifdef FT_LLD_ENERGY
char * lld_energy_ftMem_names[] = {
     "rmsEnergy",
     "logEnergy"
     };
int ft_lld_energy = 0;
#endif


#ifdef FT_LLD_TIME
char * lld_time_ftMem_names[] = {
     "zcr",
     "dc",
     "min",
     "max"
     };
int ft_lld_time = 0;
#endif

#ifdef FT_LLD_FFT
char * lld_fft_ftMem_names[] = {"fftbin"};
char * lld_fftLong_ftMem_names[] = {"fftLongBin"};
int ft_lld_fft = 0;
int ft_lld_fftLong = 0;
#endif

#ifdef FT_LLD_PITCH
char * lld_pitch_ftMem_names[] = {
     "F0freq",
     "F0strength",
     "F0quality"
     };
int ft_lld_pitch = 0;
#endif

#ifdef FT_LLD_CEPSTRUM
char * lld_cepstrum_ftMem_names[] = { "cepstrumSample" };
int ft_lld_cepstrum = 0;
#endif

#ifdef FT_LLD_CEPSTRALF0
char * lld_cepstralF0_ftMem_names[] = {      
     "cepsF0freq",
     "cepsF0strength"
     };
int ft_lld_cepstralF0 = 0;
#endif

#ifdef FT_LLD_MFCC
#define FT_LLD_MFCC_nVal     0      // number of values/names in ft. mem  (0=dynamic size)
char * lld_mfcc_ftMem_names[] = { "mfcc" };
int ft_lld_mfcc = 0;
#endif

#ifdef FT_LLD_MFCCZ
#define FT_LLD_MFCCZ_nVal     0      // number of values/names in ft. mem  (0=dynamic size)
char * lld_mfccz_ftMem_names[] = { "mfccz" };
int ft_lld_mfccz = 0;
#endif

#ifdef FT_LLD_LPC
char * lld_lpc_ftMem_names[] = { "lpcA" };
int ft_lld_lpc = 0;
#endif


#ifdef FT_LLD_SPECTRAL
char * lld_spectral_ftMem_names[] = {      
     "spectralEn0_250",    // energy from 0 250 hz
     "spectralEn0_650",    // energy from 0-650 hz
     "spectralEn250_650",  // energy from 250-650 hz
     "spectralEn1000_4000",    // energy fom 1kHz to 4kHz
     "spectralRollOff10",   // rollOff
     "spectralRollOff25",   // rollOff
     "spectralRollOff50",   // rollOff
     "spectralRollOff75",   // rollOff
     "spectralRollOff90",   // rollOff
     "spectralCentroid",   // centroid
     "spectralFlux",   // flux
     "spectralMaxPos",  // normalised pos of maximum
     "spectralMinPos",  // normalised pos of minimum
     };
int ft_lld_spectral = 0;
#endif


// not yet implemented: 
#ifdef FT_LLD_VQ
char * lld_vq_ftMem_names[] = {      
     "hnr",
     "jitter",
     "shimmer"
     };
int ft_lld_vq = 0;
#endif

/*
dynamic compile time extractor management:
  FT_N_ELEM and thus nExtractors and nFtElems, etc. are always constant at the maximum value
  the actual size of the LLD struct is also constant. Extractors that are disabled via #define 
  at compile time get NULL entries in the LLD struct
TODO:  during LLDs_create only those that do not have NULL structs are copied (check for NULL in name)
  Then only those activated will be attempted to be called, flushed, etc. 
*/


// all features defined here:
#ifndef LLD_DFLT_ENABLE
  #define LLD_DFLT_ENABLE 0
#endif
#ifndef LLD_LEVEL0
  #define LLD_LEVEL0 0
#endif
sLLDs_const LLD = {
        TOTAL_NUM_LLDS, 0, NULL, NULL,
        NULL, 1, 1,
      { 
        #ifdef FT_LLD_ENERGY
        {LLD_DFLT_ENABLE,0,LLD_LEVEL0,"energy",
         lld_energy_ftMem_names, FT_LLD_ENERGY_nVal, 0, &ft_lld_energy,
         &LLDenergy_flushToFtMem, &LLDenergy_extractor, 
         &LLDenergy_createLLDex, &LLDenergy_freeLLDex, 
         &LLDenergy_create, &LLDenergy_destroy, NULL,
         0,{"","","","",""}},
        #else
        {0,0,0,NULL,NULL,0,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,{"","","","",""}},
        #endif

        #ifdef FT_LLD_TIME
        {LLD_DFLT_ENABLE,0,LLD_LEVEL0,"time",
         lld_time_ftMem_names, FT_LLD_TIME_nVal, 0, &ft_lld_time,
         &LLDtime_flushToFtMem, &LLDtime_extractor, 
         &LLDtime_createLLDex, &LLDtime_freeLLDex, 
         &LLDtime_create, &LLDtime_destroy, NULL,
         0,{"","","","",""}},
        #else
        {0,0,0,NULL,NULL,0,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,{"","","","",""}},
        #endif
        
        #ifdef FT_LLD_FFT
        {LLD_DFLT_ENABLE,0,LLD_LEVEL0,"fft",
         lld_fft_ftMem_names, FT_LLD_FFT_nVal, 0, &ft_lld_fft,    // nVal == 0: dynamic number of values, only on name configured as basename
         /*&LLDfft_flushToFtMem*/ NULL, &LLDfft_extractor,   
         &LLDfft_createLLDex, &LLDfft_freeLLDex, 
         &LLDfft_create, &LLDfft_destroy, NULL,
         0,{"","","","",""}},
        #else
        {0,0,0,NULL,NULL,0,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,{"","","","",""}},        
        #endif

        #ifdef FT_LLD_FFT
        {LLD_DFLT_ENABLE,0,LLD_LEVEL1,"fft",
         lld_fftLong_ftMem_names, FT_LLD_FFT_nVal, 0, &ft_lld_fftLong,    // nVal == 0: dynamic number of values, only on name configured as basename
         &LLDfft_flushToFtMem, &LLDfft_extractor,
         &LLDfft_createLLDex, &LLDfft_freeLLDex, 
         &LLDfft_create, &LLDfft_destroy, NULL,
         0,{"","","","",""}},
        #else
        {0,0,0,NULL,NULL,0,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,{"","","","",""}},        
        #endif

        #ifdef FT_LLD_PITCH
        {LLD_DFLT_ENABLE,1,LLD_LEVEL0,"pitch", 
         lld_pitch_ftMem_names, FT_LLD_PITCH_nVal, 0, &ft_lld_pitch,    // nVal == 0: dynamic number of values, only on name configured as basename
         &LLDpitch_flushToFtMem, &LLDpitch_extractor,
         &LLDpitch_createLLDex, &LLDpitch_freeLLDex, 
         &LLDpitch_create, &LLDpitch_destroy, NULL,
         1,{"fft","","","",""}},
        #else
        {0,0,0,NULL,NULL,0,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,{"","","","",""}},
        #endif

        #ifdef FT_LLD_CEPSTRUM
        {LLD_DFLT_ENABLE,1,LLD_LEVEL0,"cepstrum", 
         lld_cepstrum_ftMem_names, FT_LLD_CEPSTRUM_nVal, 0,&ft_lld_cepstrum,    // nVal == 0: dynamic number of values, only on name configured as basename
         &LLDcepstrum_flushToFtMem, &LLDcepstrum_extractor,
         &LLDcepstrum_createLLDex, &LLDcepstrum_freeLLDex, 
         &LLDcepstrum_create, &LLDcepstrum_destroy, NULL,
         1,{"fft","","","",""}},
        #else
        {0,0,0,NULL,NULL,0,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,{"","","","",""}},
        #endif

        #ifdef FT_LLD_CEPSTRALF0
        {LLD_DFLT_ENABLE,1,LLD_LEVEL0,"cepstralF0", 
         lld_cepstralF0_ftMem_names, FT_LLD_CEPSTRALF0_nVal, 0,&ft_lld_cepstralF0,    // nVal == 0: dynamic number of values, only on name configured as basename
         &LLDcepstralF0_flushToFtMem, &LLDcepstralF0_extractor,
         &LLDcepstralF0_createLLDex, &LLDcepstralF0_freeLLDex, 
         &LLDcepstralF0_create, &LLDcepstralF0_destroy, NULL,
         2,{"fft","cepstrum","","",""}},
        #else
        {0,0,0,NULL,NULL,0,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,{"","","","",""}},
        #endif
        
        #ifdef FT_LLD_MFCC
        {LLD_DFLT_ENABLE,1,LLD_LEVEL0,"mfcc", 
         lld_mfcc_ftMem_names, FT_LLD_MFCC_nVal, 1, &ft_lld_mfcc,    // nVal == 0: dynamic number of values, only on name configured as basename
         &LLDmfcc_flushToFtMem, &LLDmfcc_extractor,
         &LLDmfcc_createLLDex, &LLDmfcc_freeLLDex, 
         &LLDmfcc_create, &LLDmfcc_destroy, NULL,
         1,{"fft","","","",""}},
        #else
        {0,0,0,NULL,NULL,0,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,{"","","","",""}},
        #endif

        #ifdef FT_LLD_MFCCZ
        {LLD_DFLT_ENABLE,1,LLD_LEVEL0,"mfccz", 
         lld_mfccz_ftMem_names, FT_LLD_MFCCZ_nVal, 1, &ft_lld_mfccz,    // nVal == 0: dynamic number of values, only on name configured as basename
         &LLDmfccz_flushToFtMem, &LLDmfccz_extractor,
         &LLDmfccz_createLLDex, &LLDmfccz_freeLLDex, 
         &LLDmfccz_create, &LLDmfccz_destroy, NULL,
         2,{"fft","mfcc","","",""}},
        #else
        {0,0,0,NULL,NULL,0,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,{"","","","",""}},
        #endif

        #ifdef FT_LLD_LPC
        {LLD_DFLT_ENABLE,1,LLD_LEVEL0,"lpc", 
         lld_lpc_ftMem_names, FT_LLD_LPC_nVal, 0,&ft_lld_lpc,    // nVal == 0: dynamic number of values, only on name configured as basename
         &LLDlpc_flushToFtMem, &LLDlpc_extractor,
         &LLDlpc_createLLDex, &LLDlpc_freeLLDex, 
         &LLDlpc_create, &LLDlpc_destroy, NULL,
         0,{"","","","",""}},
        #else
        {0,0,0,NULL,NULL,0,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,{"","","","",""}},
        #endif

        #ifdef FT_LLD_SPECTRAL
        {LLD_DFLT_ENABLE,1,LLD_LEVEL0,"spectral", 
         lld_spectral_ftMem_names, FT_LLD_SPECTRAL_nVal, 0,&ft_lld_spectral,    // nVal == 0: dynamic number of values, only on name configured as basename
         &LLDspectral_flushToFtMem, &LLDspectral_extractor,
         &LLDspectral_createLLDex, &LLDspectral_freeLLDex, 
         &LLDspectral_create, &LLDspectral_destroy, NULL,
         1,{"fft","","","",""}},
        #else
        {0,0,0,NULL,NULL,0,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,{"","","","",""}},
        #endif

        #ifdef FT_LLD_VQ
        {LLD_DFLT_ENABLE,1,LLD_LEVEL0,"vq", 
         lld_vq_ftMem_names, FT_LLD_VQ_nVal, 0,&ft_lld_vq,    // nVal == 0: dynamic number of values, only on name configured as basename
         &LLDvq_flushToFtMem, &LLDvq_extractor,
         &LLDvq_createLLDex, &LLDvq_freeLLDex, 
         &LLDvq_create, &LLDvq_destroy, NULL,
         1,{"fft","","","",""}},
        #else
        {0,0,0,NULL,NULL,0,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,{"","","","",""}},
        #endif
        
        {}
          }};
 
/****************************************************************************/ 
