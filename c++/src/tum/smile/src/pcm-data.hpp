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

This files contains typedefs for pcm samples and buffers of various sample formats

All internal data should be in HOST byte order.
Any function or class that reads in data from a file or stream should
  check if a conversion is necessary and convert the byte order if so.

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
#ifndef __PCM_DATA_HPP
#define __PCM_DATA_HPP

extern "C" {
  #include "pcm-data.h"
  #include "pcm-process.h"
}
#include "wave-common.hpp"

/***************************/

class cPcmConversion {

 public:
   sPcmConversion _data;
        
        cPcmConversion (pPcmConversion obj, int nSrc, int nDst, int intmatrix) {
          pcmConversion_create( &_data, nSrc, nDst, intmatrix );
        }
        cPcmConversion (pPcmConversion obj, int nSrc, int nDst) {
          pcmConversion_create( &_data, nSrc, nDst, 0 );
        }
        
        void setRoute(pPcmConversion obj, int from, int to, float weight) {
          pcmConversion_setRoute( &_data, from, to, weight );
        }

        float operator() (unsigned from, unsigned to) {
          return pcmConversion_getRoute( &_data, from, to );
        }
        
        /*
        float getRoute(int from, int to) {
          return pcmConversion_getRoute( &_data, from, to );
        }
        */        
        int getRoute_int(unsigned from, unsigned to) {
          return pcmConversion_getRoute_int( &_data, from, to );
        }
        
        
        void clearMatrix() {
          pcmConversion_clearMatrix( &_data );     
        }
        
        ~cPcmConversion() { pcmConversion_destroyData( &_data ); }
};

/**********************************************/





class cPcmBuffer {
  // allocate the uData structure and entry <entryNo>
  //int pcmBuffer_uDataAlloc(pPcmBuffer b, int entryNo, int size);
  // faster version of the above with less checking overhead
  //int pcmBuffer_uDataAllocf(pPcmBuffer b, int entryNo, int size);
  // free an entry in the uData struct
  //int pcmBuffer_uDataFree(pPcmBuffer b, int entryNo);

      
  public:
    sPcmBuffer _data; 

    cPcmBuffer(LONG_IDX nBlocks, int nChan, 
                        int sampleType, int memOrganisation, long int sampleRate) 
    {  
      pcmBufferAllocate( &_data, nBlocks, nChan, 
                        sampleType, memOrganisation, sampleRate);
    }
    
    // if aliendata is specified, the memory for the sample buffer is not allocated, 
    // the memory pointed to by aliendata is used instead
    cPcmBuffer(LONG_IDX nBlocks, int nChan, 
                        int sampleType, int memOrganisation, long int sampleRate, void *aliendata) 
    {  
      pcmBufferMakeWrapper( &_data, nBlocks, nChan, 
                        sampleType, memOrganisation, sampleRate, aliendata);
    }  
    
    
    // reset number of data-samples in buffer to zero
    inline void clear() { pcmBuffer_clear( &_data ); }

    inline LONG_IDX getNBlocks() { return _data.nBlocks; }
    
    // write zeros to buffer data and reset number of samples in buffer to zero
    inline int wipe() { return pcmBuffer_wipe( &_data ); }
    
    // fill all samples with zeros, and set nBlocks to nBlocks given as parameter
    inline LONG_IDX zeroBuf(LONG_IDX nBlocks) { return pcmBuffer_zeroBuf( &_data, nBlocks ); }
    
    // fill all samples with zeros, and set nBlocks to nBlocksAlloc
    inline LONG_IDX zeroBuf() { return pcmBuffer_zeroBuf( &_data, -1 ); }
            
    // clear (set to zero) data of channel <c> in pcm buffer <obj>
    // is must be in the range of 0...nChan-1
    inline void chanZero( int c ) { pcmBuffer_chanZero( &_data, c ); }

    // copy from buffer src (start to end) into dst starting at "skip" (in dst)
    // all conversions between different buffer formats will be performed if
    // neccessary
    inline int copyTo(cPcmBuffer &dst, int start, int end, int skip) {
      return pcmBuffer_copy(&_data, &(dst._data), start, end, skip);
    }
    inline int copyTo(cPcmBuffer &dst) {  // copy full buffer
      return pcmBuffer_copy(&_data, &(dst._data), 0, -1, 0);
    }

    // copy from buffer src (start to start+len) into dst starting at "skip" (in dst)
    // all conversions between different buffer formats will be performed if
    // neccessary
    // this function has the ability to specify a channel routing matrix <pPcmConversion conv>
    inline int copyTo_ext(cPcmBuffer &dst, int start, int len, int skip, cPcmConversion &conv)  {
      return pcmBuffer_copy_ext(&_data, &(dst._data), start, len, skip, &(conv._data));
    }

    // creates a copy of the buffer <src>, allocate a new object for the new buffer
    inline cPcmBuffer * duplicate() {
//           PcmBuffer(LONG_IDX nBlocks, int nChan, 
  //                      int sampleType, int memOrganisation, long int sampleRate) 
      cPcmBuffer * buf = new cPcmBuffer(_data.nBlocks, _data.nChan, _data.sampleType, _data.memOrga, _data.sampleRate );
      pcmBuffer_duplicateData( &_data, &(buf->_data) );
      return buf;
    }
    
    FLOAT_TYPE rmsEnergy() {
      return pcmProcess_energy( &_data );           
    }
                          
    ~cPcmBuffer() { pcmBufferFree( &_data, 1 ); }
};



/***** pcm ringbuffer ****/


class cPcmRingBuffer {
  sPcmRingBuffer _data;
  
  public:

    cPcmRingBuffer(LONG_IDX size, int nChan, int sampleType, int memOrganisation, long int sampleRate) {
      pcmRingBuffer_create( &_data, size, nChan, sampleType, memOrganisation, sampleRate );
    }

    // read a pcmBuffer from current location 
    inline LONG_IDX read ( cPcmBuffer &b ) {
      return pcmRingBuffer_read( &_data, &(b._data) );
    }

    // read at random location (must be present in ringbuffer, however!):
    inline LONG_IDX readRandom(cPcmBuffer &data, LONG_IDX index, int isRelative ) {
      return pcmRingBuffer_readRandom( &_data, &(data._data), index, isRelative );
    }

    // modulo divide given index and all internal indicies, to prevent absolute index overflows!
    inline LONG_IDX modIndex( LONG_IDX index ) {
      return pcmRingBuffer_modIndex( &_data, index );
    }

    // write a pcmBuffer at current write location:
    inline LONG_IDX write(cPcmBuffer &data) {
      return pcmRingBuffer_write( &_data, &(data._data) );
    }

    // return number of data samples available in buffer:
    inline LONG_IDX toread() {
      return pcmRingBuffer_toread( &_data );
    }
    // return free space in buffer for writes:
    inline LONG_IDX freespace() {
      return pcmRingBuffer_freespace( &_data );
    }

    ~cPcmRingBuffer() { pcmRingBuffer_destroyData( &_data ); };
};

/***** pcm ringbuffer, multi-read (MR) ****/

class cPcmRingBufferMR {
  sPcmRingBufferMR _data;
  
  public:

    cPcmRingBufferMR(LONG_IDX size, int MR, int nChan, int sampleType, int memOrganisation, long int sampleRate) {
      pcmRingBufferMR_create( &_data, size, MR, nChan, sampleType, memOrganisation, sampleRate );
    }

    // read a pcmBuffer from current location 
    inline LONG_IDX read ( cPcmBuffer &b, int id ) {
      return pcmRingBufferMR_read( &_data, id, &(b._data) );
    }

    // read a pcmBuffer from current location, increasing rdidx only by stepsize
    inline LONG_IDX readStep ( cPcmBuffer &b, int id, LONG_IDX stepsize ) {
      return pcmRingBufferMR_readStep( &_data, id, &(b._data), stepsize );
    }

    // read at random location (must be present in ringbuffer, however!):
    inline LONG_IDX readRandom(cPcmBuffer &data, LONG_IDX index ) {
      return pcmRingBufferMR_readRandom( &_data, &(data._data), index );
    }

    inline LONG_IDX minRdIdx() {
      return pcmRingBufferMR_minRdIdx( &_data );
    }

    inline LONG_IDX maxRdIdx() {
      return pcmRingBufferMR_maxRdIdx( &_data );
    }

    // modulo divide given index and all internal indicies, to prevent absolute index overflows!
    inline LONG_IDX modIndex( LONG_IDX index ) {
      return pcmRingBufferMR_modIndex( &_data, index );
    }

    // write a pcmBuffer at current write location:
    inline LONG_IDX write(cPcmBuffer &data) {
      return pcmRingBufferMR_write( &_data, &(data._data) );
    }

    // return number of data samples available in buffer:
    inline LONG_IDX toread(int id) {
      return pcmRingBufferMR_toread( &_data, id );
    }
    // return free space in buffer for writes:
    inline LONG_IDX freespace() {
      return pcmRingBufferMR_freespace( &_data );
    }

    ~cPcmRingBufferMR() { pcmRingBufferMR_destroyData( &_data ); };
};

#endif // #ifndef __PCM_DATA_HPP
