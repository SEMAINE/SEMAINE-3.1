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

This files contains typedefs for pcm samples and buffers of various sample formats

All internal data should be in HOST byte order.
Any function or class that reads in data from a file or stream should
  check if a conversion is necessary and convert the byte order if so.

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
#ifndef __PCM_DATA_H
#define __PCM_DATA_H

#include <inttypes.h>
#include "featum_common.h"

typedef struct {
  int8_t x[3];
} sample24_t;
        
#define SAMPLE8i  int8_t
#define SAMPLE16i int16_t
#define SAMPLE24i sample24_t
#define SAMPLE32i int32_t
//#ifdef  SAMPLEFP_ISFLOAT
#define SAMPLEFLOAT   float
#define SAMPLEDOUBLE  double
//#else
#define SAMPLEFP  FLOAT_TYPE
//#endif

#define SAMPLETYPE_I8   1  // 8 bit integer signed samples
#define SAMPLETYPE_I16  2  // 16 bit integer signed samples
#define SAMPLETYPE_I24  3  // 24 bit integer signed samples
#define SAMPLETYPE_I32  4  // 32 bit integer signed samples

#define SAMPLETYPE_FLOAT   10  // floating point samples
#define SAMPLETYPE_DOUBLE  11  // floating point (double) samples
#define SAMPLETYPE_FP      15  // use SAMPLEFP

// sample type that will be used for all computations (mainly pcm-process module)
#define WORK_SAMPLETYPE   SAMPLETYPE_FP
#define WORK_SAMPLE       SAMPLEFP
#define WORK_SAMPLE_SIZE  sizeof(WORK_SAMPLE)
#define WORK_SAMPLE_RANGE 1.0

#define MEMORGA_INTERLV 0  // channels interleaved (like in wave file)
#define MEMORGA_SEPCH   1  // channels seperated as sequential arrays

// struct to pass advanced options for channels conversions
// => channel routing and mixing matrix nSrc x nDst entries
typedef struct {
  unsigned direct;   // direct copy, do not consider matrix
  unsigned intmatrix; // use integer matrix instead of float matrix
  int intmatrix_scale;  // for future use..
  unsigned nSrc, nDst;
  float *routing; // matrix with nSrc rows and nDst columns
  int *routing_int;  // same as above, however only 1 and 0 integers
} sPcmConversion;
typedef sPcmConversion * pPcmConversion;

pPcmConversion pcmConversion_create(pPcmConversion obj, unsigned nSrc, unsigned nDst, unsigned intmatrix);
void pcmConversion_setRoute(pPcmConversion obj, unsigned from, unsigned to, float weight);
//#ifdef OPTIMIZE_SIZE
int pcmConversion_getRoute_int(pPcmConversion obj, unsigned from, unsigned to);
float pcmConversion_getRoute(pPcmConversion obj, unsigned from, unsigned to);
/*
#else
inline int pcmConversion_getRoute_int(pPcmConversion obj, unsigned from, unsigned to);
inline float pcmConversion_getRoute(pPcmConversion obj, unsigned from, unsigned to);
#endif
*/
void pcmConversion_clearMatrix(pPcmConversion obj);
pPcmConversion pcmConversion_destroy(pPcmConversion obj);
pPcmConversion pcmConversion_destroyData(pPcmConversion obj);

typedef void* pPcmSample;

#define nPCMUSERDATAENTRIES 5
#define uDataWINDOW 0
#define uDataUNASSIGNED1 1
// ... more to come ...

typedef struct {
  int entrySize[nPCMUSERDATAENTRIES];
  void *entry[nPCMUSERDATAENTRIES];
} sPcmUserData;
typedef sPcmUserData *pPcmUserData;
        
/***** the most important struct ****/
typedef struct {
  LONG_IDX nBlocks,nBlocksAlloc; // number of Blocks and Allocated Blocks
         // For sequential data "nBlocks" refers to the number of samples per channel
  LONG_IDX nBytesAlloc;
  LONG_IDX timestampSamples;
  long int sampleRate;
  FLOAT_TYPE_TIME timestamp;  // important!
  int nChan;
  int nBPS;  // actual bytes per sample (one channel)
  int sampleType;  // abstract version of nBPS
  int blockSize;  // for interleaved data
  int chanSize;   // for sequential data
  int memOrga;
  pPcmUserData  uData;  // stuff like windowing coefficients etc.
  int alienData;     // indicate whether *data has been allocated elsewhere and should not be freed by pcmBufferFree
  pPcmSample data; 
} sPcmBuffer;
typedef sPcmBuffer* pPcmBuffer;

FLOAT_TYPE energy( pPcmBuffer b );

int pcmSampleTypeToNBPS( int sampleType );
int pcmSampleTypeToBits( int sampleType );
int pcmBitsToSampleType( int bits, int byteOrder, int preferFloat );

pPcmBuffer pcmBufferAllocate( pPcmBuffer b, LONG_IDX nBlocks, int nChan, 
                              int sampleType, int memOrganisation, long int sampleRate);

// create a pcmBuffer object as wrapper for a raw pcm data pointer
//   this function sets the alienData flag
pPcmBuffer pcmBufferMakeWrapper( pPcmBuffer b, LONG_IDX nBlocks, int nChan, 
                               int sampleType, int memOrganisation, long int sampleRate, void *data);
                               
// allocate the uData structure and entry <entryNo>
int pcmBuffer_uDataAlloc(pPcmBuffer b, int entryNo, int size);
// faster version of the above with less checking overhead
int pcmBuffer_uDataAllocf(pPcmBuffer b, int entryNo, int size);
// free an entry in the uData struct
int pcmBuffer_uDataFree(pPcmBuffer b, int entryNo);

// reset number of data-samples in buffer to zero
int pcmBuffer_clear(pPcmBuffer b);
// write zeros to buffer data and reset number of samples in buffer to zero
int pcmBuffer_wipe(pPcmBuffer b);
LONG_IDX pcmBuffer_zeroBuf(pPcmBuffer b, LONG_IDX nBlocks);

// clear (set to zero) data of channel <c> in pcm buffer <obj>
// is must be in the range of 0...nChan-1
int pcmBuffer_chanZero(pPcmBuffer obj, int c);

// copy from buffer src (start to end) into dst starting at "skip" (in dst)
// all conversions between different buffer formats will be performed if
// neccessary
int pcmBuffer_copy(pPcmBuffer src, pPcmBuffer dst, int start, int end, int skip);

// copy from buffer src (start to start+len) into dst starting at "skip" (in dst)
// all conversions between different buffer formats will be performed if
// neccessary
// this function has the ability to specify a channel routing matrix <pPcmConversion conv>
int pcmBuffer_copy_ext(pPcmBuffer src, pPcmBuffer dst, int start, int len, int skip, pPcmConversion conv);

// creates a copy of the buffer <src>, allocate a new object for the new buffer
pPcmBuffer pcmBuffer_duplicate( pPcmBuffer src );
void pcmBuffer_duplicateData( pPcmBuffer src, pPcmBuffer dst );

// creates a copy of the buffer <src>, allocate a new object for the new buffer with different size, pad with zeros if new size is larger
pPcmBuffer pcmBuffer_duplicateResize( pPcmBuffer src, LONG_IDX nBlocksDst );

/* free buffer object if noObjFree==0, else free only internal data 
    -> this option (1) is important when passing a pointer like this: &(wavebuffer)
*/
pPcmBuffer pcmBufferFree( pPcmBuffer b, int noObjFree );


/***** pcm ringbuffer ****/
typedef struct {
  pPcmBuffer data;
  LONG_IDX  bsize; // actual size
  LONG_IDX  rdidx, wridx;
} sPcmRingBuffer;
typedef sPcmRingBuffer* pPcmRingBuffer;

pPcmRingBuffer pcmRingBuffer_create( pPcmRingBuffer obj, LONG_IDX size, int nChan, int sampleType, int memOrganisation, long int sampleRate );

// read a pcmBuffer from current location 
LONG_IDX pcmRingBuffer_read( pPcmRingBuffer obj, pPcmBuffer data );

// read at random location (must be present in ringbuffer, however!):
LONG_IDX pcmRingBuffer_readRandom( pPcmRingBuffer obj, pPcmBuffer data, LONG_IDX index, int isRelative );

// modulo divide given index and all internal indicies, to prevent absolute index overflows!
LONG_IDX pcmRingBuffer_modIndex( pPcmRingBuffer obj, LONG_IDX index );

// write a pcmBuffer at current write location:
LONG_IDX pcmRingBuffer_write( pPcmRingBuffer obj, pPcmBuffer data );

// return number of data samples available in buffer:
LONG_IDX pcmRingBuffer_toread( pPcmRingBuffer obj );
// return free space in buffer for writes:
LONG_IDX pcmRingBuffer_freespace( pPcmRingBuffer obj );

pPcmRingBuffer pcmRingBuffer_destroy( pPcmRingBuffer obj );
pPcmRingBuffer pcmRingBuffer_destroyData( pPcmRingBuffer obj );


/***** pcm ringbuffer, multi-read (MR) ****/
typedef struct {
  pPcmBuffer data;
  int nMR;
  LONG_IDX  bsize; // actual size
  LONG_IDX  *rdidx;
  LONG_IDX  wridx;
} sPcmRingBufferMR;
typedef sPcmRingBufferMR* pPcmRingBufferMR;

pPcmRingBufferMR pcmRingBufferMR_create( pPcmRingBufferMR obj, LONG_IDX size, int nMR, int nChan, int sampleType, int memOrganisation, long int sampleRate );

// read a pcmBuffer from current location 
LONG_IDX pcmRingBufferMR_read( pPcmRingBufferMR obj, int id, pPcmBuffer data );

// read a pcmBuffer from current location, increasing rdidx only by stepsize
LONG_IDX pcmRingBufferMR_readStep( pPcmRingBufferMR obj, int id, pPcmBuffer data, LONG_IDX stepsize );

// read at random location (must be present in ringbuffer, however!):
LONG_IDX pcmRingBufferMR_readRandom( pPcmRingBufferMR obj, pPcmBuffer data, LONG_IDX index );

// modulo divide given index and all internal indicies, to prevent absolute index overflows!
LONG_IDX pcmRingBufferMR_modIndex( pPcmRingBufferMR obj, LONG_IDX index );

// write a pcmBuffer at current write location:
LONG_IDX pcmRingBufferMR_write( pPcmRingBufferMR obj, pPcmBuffer data );

LONG_IDX pcmRingBufferMR_minRdIdx( pPcmRingBufferMR obj );
LONG_IDX pcmRingBufferMR_maxRdIdx( pPcmRingBufferMR obj );

// return number of data samples available in buffer:
LONG_IDX pcmRingBufferMR_toread( pPcmRingBufferMR obj, int id );
// return free space in buffer for writes:
LONG_IDX pcmRingBufferMR_freespace( pPcmRingBufferMR obj );

pPcmRingBufferMR pcmRingBufferMR_destroy( pPcmRingBufferMR obj );
pPcmRingBufferMR pcmRingBufferMR_destroyData( pPcmRingBufferMR obj );

#endif // #ifndef __MY_FILE_NAME_H
