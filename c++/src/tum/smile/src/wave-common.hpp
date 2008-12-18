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


#ifndef __WAVE_COMMON_HPP
#define __WAVE_COMMON_HPP

extern "C" {
       #include "wave-common.h"
       #include "featum_util.h"
}

// TODO: use this class everywhere, instead of passing all parameters separately
class cWaveParameters {

  public:
   sWaveParameters param;

   cWaveParameters(const sWaveParameters & p) : param(p) { }
   cWaveParameters(int _sampleRate, int _nChan, int _bits, int _memOrga, int _byteOrder, int _useFloat) { 
     memzero(&param,sizeof(param));
     sampleRate(_sampleRate);
     nChan(_nChan);
     memOrga(_memOrga);
     setFormat(_bits, _useFloat, _byteOrder);
   }
   cWaveParameters(int _sampleRate, int _nChan, int _bits, int _memOrga, int _byteOrder) { 
     memzero(&param,sizeof(param));
     sampleRate(_sampleRate);
     nChan(_nChan);
     memOrga(_memOrga);
     setFormat(_bits, 0, _byteOrder);
   }
   cWaveParameters(int _sampleRate, int _nChan, int _bits, int _memOrga) { 
     memzero(&param,sizeof(param));
     sampleRate(_sampleRate);
     nChan(_nChan);
     memOrga(_memOrga);
     setFormat(_bits, 0, BYTEORDER_LE);
   }
/*   cWaveParameters(int _sampleRate, int _nChan, int _bits) { 
     memzero(&param,sizeof(param));
     sampleRate(_sampleRate);
     nChan(_nChan);
     memOrga(MEMORGA_INTERLV);
     setFormat(_bits, 0, BYTEORDER_LE);
   }*/
   cWaveParameters(int _sampleRate, int _nChan, int _sampleType) { 
     memzero(&param,sizeof(param));
     sampleType(_sampleType);
     nChan(_nChan);
     memOrga(MEMORGA_INTERLV);
   }

   void sampleType(int t) { param.sampleType = t; }
   int sampleType() const { return param.sampleType; }
   int nBPS() const {
     return pcmSampleTypeToNBPS( param.sampleType );
   }
   int nBits() const {
     return pcmSampleTypeToBits( param.sampleType );
   }
   int setFormat(int bits, int preferFloat, int byteOrder) {
     param.sampleType = pcmBitsToSampleType( bits, byteOrder, preferFloat );       
   }
   int setFormat(int bits) {
     param.sampleType = pcmBitsToSampleType( bits, BYTEORDER_LE, 0 );
   }

   void memOrga(int m) { param.memOrga = m; }
   int memOrga() const { return param.memOrga; }

   void nChan(int c) {  param.nChan = c; }
   int nChan() const { return param.nChan; }

   void sampleRate(long s)  { param.sampleRate = s; }
   long sampleRate() const { return param.sampleRate; }
   
   ~cWaveParameters() {};
};


#endif  // __WAVE_COMMON_HPP
