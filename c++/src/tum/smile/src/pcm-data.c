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

This files contains functions for managing pcm data buffers

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#define MODULE "pcm-buffer"
#include "featum_common.h"

#include <stdio.h>
#include <stdlib.h>
#ifndef __WINDOWS
#include <string.h>  // for memcpy function
#endif 
#include <math.h>
#include <inttypes.h>
#if FLOAT_TYPE == double
#define ROUND round
#else
#define ROUND roundf
#endif
extern double
     round(double x);
extern float
     roundf(float x);
     
#include "featum_util.h"
#include "pcm-data.h"

#undef FUNCTION

// unpack 3-byte signed 24-bit little endian samples into a int32_t
inline int32_t unpack24i_le(sample24_t * x) 
{
  return ( (int32_t)(x->x[0]) + (int32_t)(x->x[1]*(1<<8)) + (int32_t)(x->x[2]*(1<<16)) );
}

// warning: this function will clip samples, if they are out of the 24-bit range!
inline sample24_t pack24i_le(int32_t x) 
{
  sample24_t p;
  p.x[0] = (uint8_t)( x & 0xFF);
  p.x[1] = (uint8_t)( x & 0xFF00);
  p.x[2] = (uint8_t)( x & 0xFF0000);
  return p;
}

// add b to a, save in a 
inline add_sample24i_le( sample24_t *a, sample24_t *b )
{
  int32_t bu = unpack24i_le(b);
  int32_t au = unpack24i_le(a);
  *a = pack24i_le(au+bu);
}

// add b to a, save in a 
inline add_sample24i_le_32( sample24_t *a, int32_t b )
{
  int32_t au = unpack24i_le(a);
  *a = pack24i_le(au+b);
}

/********* pcmConversion *************/

/*
typedef struct {
  int nSrc, nDst;
  float *routing;  // matrix with nSrc rows and nDst columns
} sPcmConversion;
typedef sPcmConversion * pPcmConversion;
*/

pPcmConversion pcmConversion_create(pPcmConversion obj, unsigned nSrc, unsigned nDst, unsigned intmatrix)
#define FUNCTION "pcmConversion_create"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    if (obj->routing) { free(obj->routing); obj->routing = NULL; }
    if (obj->routing_int) { free(obj->routing_int); obj->routing_int = NULL; }
  }
  if (obj == NULL) {
    obj = (pPcmConversion) calloc(1,sizeof(sPcmConversion));   
  } else {
    memzero(obj,sizeof(sPcmConversion));       
  }
  if (obj != NULL) {
    if (intmatrix) {
      obj->routing_int = (int *)calloc(1,sizeof(int)*nSrc*nDst);
      #ifdef COMPILE_CHECKS
      if (obj->routing_int == NULL) { 
        FEATUM_FATAL_ERR(0,"Error allocating memory in pcmConversion_create");
        free(obj); _FUNCTION_RETURN_( NULL ); 
      }
      #endif
      obj->intmatrix_scale = 1;
    } else {
      obj->routing = (float *)calloc(1,sizeof(float)*nSrc*nDst);
      #ifdef COMPILE_CHECKS
      if (obj->routing == NULL) { 
        FEATUM_FATAL_ERR(0,"Error allocating memory in pcmConversion_create");
        free(obj); _FUNCTION_RETURN_( NULL );
      }
      #endif
    }
    obj->nSrc = nSrc;
    obj->nDst = nDst;
    obj->intmatrix = intmatrix;
  }
  _FUNCTION_RETURN_( obj );
}
#undef FUNCTION

#ifdef OPTIMIZE_SIZE
void pcmConversion_setRoute(pPcmConversion obj, unsigned from, unsigned to, float weight)
#else
inline void pcmConversion_setRoute(pPcmConversion obj, unsigned from, unsigned to, float weight)
#endif
#define FUNCTION "pcmConversion_setRoute"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    #ifdef COMPILE_CHECKS
    if (from > obj->nSrc) { from = 0; weight = 0.0; }
    if (to > obj->nDst) { to = 0; weight = 0.0; }
    #endif

    if (obj->intmatrix) {
      if (obj->routing_int == NULL) _FUNCTION_RETURNV_;
      obj->routing_int[obj->nDst*from + to] = (int)weight;
                        
    } else {
      if (obj->routing == NULL) _FUNCTION_RETURNV_;
      obj->routing[obj->nDst*from + to] = weight;
    }
  }
  _FUNCTION_RETURNV_
}
#undef FUNCTION

#ifdef OPTIMIZE_SIZE
float pcmConversion_getRoute(pPcmConversion obj, unsigned from, unsigned to)
#else
inline float pcmConversion_getRoute(pPcmConversion obj, unsigned from, unsigned to)
#endif
#define FUNCTION "pcmConversion_getRoute"
{_FUNCTION_ENTER_
  #ifdef COMPILE_CHECKS_LOOPS
  if ((obj != NULL)&&(obj->routing != NULL)) {
  #endif
  if (obj->intmatrix) {
    // this is only a hack to avoid malfunction... you should always (for performance reasons...) call getRoute_int directly when working with int routing matricies!
    int res = pcmConversion_getRoute_int(obj, from, to);
    _FUNCTION_RETURN_((float)res);
  } else {
    if (obj->direct) { 
      if (from >= obj->nSrc) _FUNCTION_RETURN_( 0.0 );
      _FUNCTION_RETURN_( (float)((int)(from == to)) );
    }
    #ifdef COMPILE_CHECKS_LOOPS
    if (from >= obj->nSrc) { _FUNCTION_RETURN_( 0.0 ); }
    if (to >= obj->nDst) { _FUNCTION_RETURN_( 0.0 ); }
    if (obj->routing == NULL) { _FUNCTION_RETURN_( 0.0 ); }
    #endif
    FEATUM_DEBUG(12,"obj: %i, obj->routing: %i",(long)obj, (long)(obj->routing));
    FEATUM_DEBUG(12,"from: %i, to: %i  (obj->nDst = %i)",from,to,obj->nDst);
    _FUNCTION_RETURN_( obj->routing[obj->nDst*from + to] );
  }
  #ifdef COMPILE_CHECKS_LOOPS
  } 
  #endif
  _FUNCTION_RETURN_( 0.0 );
}
#undef FUNCTION

#ifdef OPTIMIZE_SIZE
int pcmConversion_getRoute_int(pPcmConversion obj, unsigned from, unsigned to)
#else
inline int pcmConversion_getRoute_int(pPcmConversion obj, unsigned from, unsigned to)
#endif
#define FUNCTION "pcmConversion_getRoute_int"
{_FUNCTION_ENTER_
  #ifdef COMPILE_CHECKS_LOOPS
  if ((obj != NULL)&&(obj->intmatrix)&&(obj->routing_int != NULL)) {
  #else
  if (obj->intmatrix) {
  #endif
    if (obj->direct) { 
      if (from >= obj->nSrc) _FUNCTION_RETURN_( 0 );
      _FUNCTION_RETURN_( (int)(from == to)*obj->intmatrix_scale );
    }
    #ifdef COMPILE_CHECKS
    if (from >= obj->nSrc) { _FUNCTION_RETURN_( 0 ); }
    if (to >= obj->nDst) { _FUNCTION_RETURN_( 0 ); }
    if (obj->routing_int == NULL) _FUNCTION_RETURN_( 0 );
    #endif
    FEATUM_DEBUG(9,"obj: %i, obj->routing_int: %i",(long)obj, (long)(obj->routing_int));
    FEATUM_DEBUG(9,"from: %i, to: %i  (obj->nDst = %i)",from,to,obj->nDst);
    _FUNCTION_RETURN_( obj->routing_int[obj->nDst*from + to] );
  } 
  _FUNCTION_RETURN_( 0 );
}
#undef FUNCTION

void pcmConversion_clearMatrix(pPcmConversion obj)
#define FUNCTION "pcmConversion_clearMatrix"
{_FUNCTION_ENTER_
  int i;
  if (obj != NULL) {
    if (obj->routing != NULL) 
      memzero(obj->routing, obj->nSrc*obj->nDst*sizeof(float));    
    if (obj->routing_int != NULL) 
      memzero(obj->routing, obj->nSrc*obj->nDst*sizeof(int));    

  }     
  _FUNCTION_RETURNV_
}
#undef FUNCTION


pPcmConversion pcmConversion_destroyData(pPcmConversion obj)
#define FUNCTION "pcmConversion_destroyData"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    if (obj->routing != NULL) free(obj->routing);
    if (obj->routing_int != NULL) free(obj->routing_int);
  }
  _FUNCTION_RETURN_( NULL );
}
#undef FUNCTION


pPcmConversion pcmConversion_destroy(pPcmConversion obj)
#define FUNCTION "pcmConversion_destroy"
{_FUNCTION_ENTER_
  pcmConversion_destroyData(obj);
  if (obj != NULL) {
    free(obj);
  }
  _FUNCTION_RETURN_( NULL );
}
#undef FUNCTION


//********************* pcmBuffer ******************************************


// nBPS: refers to actual size of a packed sample in byte  (not necessarily bits/8)
// bits: refers to the actual resolution in bits of a sample as grabbed from the ADC


// standard audio paramaters to always pass:  nChan, sampleType, sampleRate, ( memOrganisation, byte order )


int pcmSampleTypeToNBPS( int sampleType )
#define FUNCTION "pcmSampleTypeToNBPS"
{
    switch(sampleType) {
      case SAMPLETYPE_I8:  return sizeof(SAMPLE8i);
      case SAMPLETYPE_I16: return sizeof(SAMPLE16i);
      case SAMPLETYPE_I24: return sizeof(SAMPLE24i);
      case SAMPLETYPE_I32: return sizeof(SAMPLE32i);
      case SAMPLETYPE_FLOAT: return sizeof(SAMPLEFLOAT);
      case SAMPLETYPE_DOUBLE: return sizeof(SAMPLEDOUBLE);
      case SAMPLETYPE_FP:  return sizeof(SAMPLEFP);
      default:
        // not implemented!
        FEATUM_ERR_FATAL(1,"Sample type %i not implemented\n",sampleType);
        return 0;
        break;
    }   
}
#undef FUNCTION

int pcmSampleTypeToBits( int sampleType )
#define FUNCTION "pcmSampleTypeToBits"
{

    switch(sampleType) {
      case SAMPLETYPE_I8:  return 8;
      case SAMPLETYPE_I16: return 16;
      case SAMPLETYPE_I24: return 24;
      case SAMPLETYPE_I32: return 32;
      case SAMPLETYPE_FLOAT: return 32;
      case SAMPLETYPE_DOUBLE: return 64;
      case SAMPLETYPE_FP:  return sizeof(SAMPLEFP)/8;
      default:
        // not implemented!
        FEATUM_ERR_FATAL(1,"Sample type %i not implemented\n",sampleType);
        return 0;
        break;
    }   
}
#undef FUNCTION

int pcmBitsToSampleType( int bits, int byteOrder, int preferFloat )
#define FUNCTION "pcmBitsToSampleType"
{
// TODO: extend sampletypes by byteorder...
(void)byteOrder;
    switch(bits) {
      case 8:  return SAMPLETYPE_I8;
      case 16: return SAMPLETYPE_I16;
      case 24: return SAMPLETYPE_I24;
      case 32: {
           if (!preferFloat) return SAMPLETYPE_I32;
           else {
             if ((sizeof(SAMPLEFP)*8) == 32) return SAMPLETYPE_FP;
             else return SAMPLETYPE_FLOAT;
           }
      }
      case 64: {
           if ((sizeof(SAMPLEFP)*8) == 64) return SAMPLETYPE_FP;
           else return SAMPLETYPE_DOUBLE;
      }
      default:
        // not implemented!
        FEATUM_ERR_FATAL(1," %i bits sampleType not implemented\n",bits);
        return 0;
        break;
    }   
}
#undef FUNCTION

/* main allocator function for all types 
   see pcm-data.h for constants for sampleType and memOrganisation
*/
pPcmBuffer pcmBufferAllocate( pPcmBuffer b, LONG_IDX nBlocks, int nChan, 
                               int sampleType, int memOrganisation, long int sampleRate)
#define FUNCTION "pcmBufferAllocate"
{_FUNCTION_ENTER_
  int alloced = 0;
  if (b == NULL) {
    b = (pPcmBuffer) calloc(1,sizeof(sPcmBuffer));   alloced =1;
  } else {
    memzero(b,sizeof(sPcmBuffer));       
  }
  if (b != NULL) {
    if (nChan < 1) { if (alloced) free(b); _FUNCTION_RETURN_( NULL ); }
    if (!alloced) if ((b->data)&&(!(b->alienData))) free(b->data);
    b->data = NULL;
    b->nBlocks = 0;    // this is the actual number of blocks with data in them!!
    b->nChan = nChan;
    b->sampleRate = sampleRate;
    b->sampleType = sampleType;
    b->nBPS = pcmSampleTypeToNBPS(sampleType);
    b->memOrga = memOrganisation;    
    if (memOrganisation == MEMORGA_INTERLV) {
      b->blockSize = nChan * b->nBPS;
      b->chanSize = b->nBPS;
      b->nBytesAlloc = nBlocks * b->blockSize;
    } else if (memOrganisation == MEMORGA_SEPCH) {
      b->chanSize = nBlocks * b->nBPS;
      b->blockSize = b->nBPS;
      b->nBytesAlloc = nChan * b->chanSize;
    }


    b->data = (pPcmBuffer) calloc(1,b->nBytesAlloc);
    if (b->data != NULL) {
      b->nBlocksAlloc = nBlocks;
    } else {
      b->nBlocksAlloc = 0;
      b->nBytesAlloc = 0;
    }
    b->alienData = 0;
  }
  _FUNCTION_RETURN_( b );
}
#undef FUNCTION

// create a pcmBuffer object as wrapper for a raw pcm data pointer
//   this function sets the alienData flag
pPcmBuffer pcmBufferMakeWrapper( pPcmBuffer b, LONG_IDX nBlocks, int nChan, 
                               int sampleType, int memOrganisation, long int sampleRate, void *data)
#define FUNCTION "pcmBufferMakeWrapper"
#ifndef USE_PORTAUDIO
{_FUNCTION_ENTER_
#else
{
#endif
  if (b == NULL) {
    b = (pPcmBuffer) calloc(1,sizeof(sPcmBuffer));   
  }
  if (b != NULL) {
    if (nChan < 1) { free(b); _FUNCTION_RETURN_( NULL ); }
    if ((b->data)&&(!(b->alienData))) free(b->data);
    b->data = data;
    b->nBlocks = nBlocks;    // this is the actual number of blocks with data in them!!
    b->nChan = nChan;
    b->sampleRate = sampleRate;
    b->sampleType = sampleType;
    b->nBPS = pcmSampleTypeToNBPS(sampleType);
    b->memOrga = memOrganisation;    
    if (memOrganisation == MEMORGA_INTERLV) {
      b->blockSize = nChan * b->nBPS;
      b->chanSize = b->nBPS;
      b->nBytesAlloc = nBlocks * b->blockSize;
    } else if (memOrganisation == MEMORGA_SEPCH) {
      b->chanSize = nBlocks * b->nBPS;
      b->blockSize = b->nBPS;
      b->nBytesAlloc = nChan * b->chanSize;
    }

    if (b->data != NULL) {
      b->nBlocksAlloc = nBlocks;
    } else {
      b->nBlocksAlloc = 0;
      b->nBytesAlloc = 0;
    }
    b->alienData = 1;
  }
  #ifdef USE_PORTAUDIO
  return b;
  #else
  _FUNCTION_RETURN_( b );
  #endif
}
#undef FUNCTION

int pcmBuffer_uDataAlloc(pPcmBuffer b, int entryNo, int size)
#define FUNCTION "pcmBuffer_uDataAlloc"
{_FUNCTION_ENTER_
  
  if (b == NULL) _FUNCTION_RETURN_(0);
  if (b->uData == NULL) { 
    b->uData = (pPcmUserData)calloc(1,sizeof(pPcmUserData)); 
    if (b->uData == NULL) _FUNCTION_RETURN_(0);
  }
   
  #ifdef COMPILE_CHECKS
  if ( (entryNo < 0) || (entryNo > nPCMUSERDATAENTRIES)) _FUNCTION_RETURN_(0);
  if (size <= 0) _FUNCTION_RETURN_(0);
  #endif
 
  if (b->uData->entry[entryNo] == NULL) {
     b->uData->entry[entryNo] = calloc(1,size);
     b->uData->entrySize[entryNo] = size;
     _FUNCTION_RETURN_(1);
  }
                    
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION


int pcmBuffer_uDataFree(pPcmBuffer b, int entryNo)
#define FUNCTION "pcmBuffer_uDataFree"
{_FUNCTION_ENTER_
  
  if (b == NULL) _FUNCTION_RETURN_(0);
  if (b->uData == NULL)  _FUNCTION_RETURN_(0);
   
  #ifdef COMPILE_CHECKS
  if ( (entryNo < 0) || (entryNo > nPCMUSERDATAENTRIES)) _FUNCTION_RETURN_(0);
  #endif
 
  if (b->uData->entry[entryNo] != NULL) {
     free(b->uData->entry[entryNo]);
     b->uData->entrySize[entryNo] = 0;
     _FUNCTION_RETURN_(1);
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION

// faster version
inline int pcmBuffer_uDataAllocf(pPcmBuffer b, int entryNo, int size)
#define FUNCTION "pcmBuffer_uDataAllocf"
{_FUNCTION_ENTER_
  
  if (b->uData == NULL) { 
    b->uData = (pPcmUserData)calloc(1,sizeof(sPcmUserData)); 
  }

  if (b->uData->entry[entryNo] == NULL) {
     b->uData->entry[entryNo] = calloc(1,size);
     b->uData->entrySize[entryNo] = size;
     _FUNCTION_RETURN_(1);
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION

// reset number of data-samples in buffer to zero
int pcmBuffer_clear(pPcmBuffer b)
#define FUNCTION "pcmBuffer_clear"
{_FUNCTION_ENTER_
  if (b!= NULL) {
    b->nBlocks = 0;
    _FUNCTION_RETURN_( 1 );
  } else {
    _FUNCTION_RETURN_( 0 ); 
  }
}
#undef FUNCTION

// write zeros to buffer data and reset number of samples in buffer to zero
int pcmBuffer_wipe(pPcmBuffer b)
#define FUNCTION "pcmBuffer_wipe"
{_FUNCTION_ENTER_
  if (b!= NULL) {
    b->nBlocks = 0;
    if (b->data) memzero(b->data,b->nBytesAlloc);
    _FUNCTION_RETURN_( 1 );
  } else {
    _FUNCTION_RETURN_( 0 );       
  }
}
#undef FUNCTION

LONG_IDX pcmBuffer_zeroBuf(pPcmBuffer b, LONG_IDX nBlocks)
#define FUNCTION "pcmBuffer_zeroBuf"
{_FUNCTION_ENTER_
  if (b!= NULL) {
    if ((nBlocks >= 0)&&(nBlocks < b->nBlocksAlloc)) 
      b->nBlocks = nBlocks;
    else 
      b->nBlocks = b->nBlocksAlloc;
    if (b->data) memzero(b->data,b->nBytesAlloc);
    _FUNCTION_RETURN_( b->nBlocks );
  } else {
    _FUNCTION_RETURN_( 0 );       
  }
}
#undef FUNCTION


// set end to -1 to copy full buffer src into dst
int pcmBuffer_copy(pPcmBuffer src, pPcmBuffer dst, int start, int end, int skip)
#define FUNCTION "pcmBuffer_copy"
#ifdef USE_PORTAUDIO
{
#else
{_FUNCTION_ENTER_
#endif
  int i; LONG_IDX ret;
  if (dst == NULL) _FUNCTION_RETURN_( 0 );
  if (src == NULL) _FUNCTION_RETURN_( 0 );
  #ifdef COMPILE_CHECKS
  #endif
  
  if (end < 0) end = src->nBlocks;
  
  pPcmConversion conv = NULL;
  
  // apply default routings:
  if (src->nChan > dst->nChan) { 
//  if (src->nChan <= dst->nChan) { // direct copy, zero remaining
     //conv = pcmConversion_create(conv, src->nChan, dst->nChan, 0);
     //conv->direct = 1;
//  } else {
     conv = pcmConversion_create(conv, src->nChan, dst->nChan, 0);
     if (conv == NULL) _FUNCTION_RETURN_( 0 );
     if ((src->nChan % 2 == 0) && (dst->nChan % 2 == 0) && (src->nChan>>1 == dst->nChan)) { // pairwise mixdown
       for (i=0; i<dst->nChan; i++) {
         pcmConversion_setRoute(conv, i*2, i, 0.5);              
         pcmConversion_setRoute(conv, i*2+1, i, 0.5);
       }
     } else if (dst->nChan == 1) { // mono mixdown
       float weight = 1.0/(float)src->nChan;
       for (i=0; i<src->nChan; i++) {
         pcmConversion_setRoute(conv, i, 0, weight);    
       }            
     } else {
       conv->direct = 1;       
     }  
     ret = pcmBuffer_copy_ext(src,dst,start,end-start,skip,conv);
     conv = pcmConversion_destroy(conv);
  } else { // direct copy, zero remaining
    ret = pcmBuffer_copy_ext(src,dst,start,end-start,skip,NULL);
  }
  _FUNCTION_RETURN_( ret );
}
#undef FUNCTION

/*
  clear #len data samples of one channel in the sample buffer pointed to by *buffer
  if chanSize is given, interleaved format is assumed,
  else if chanSize == 0, continuous data is assumed (SEPCH memOrga)
*/
void chanZero(void *buffer, LONG_IDX len, int blockSize, int chanSize) 
#define FUNCTION "chanZero"
#ifdef USE_PORTAUDIO
{
#else
{_FUNCTION_ENTER_
#endif
  if (chanSize == 0) {
    memzero(buffer,len*blockSize);
  } else {
    void *buf0 = buffer;
    for ( ; buffer < buf0 + len*blockSize; buffer += blockSize ) {
      memzero(buffer,chanSize);
    }         
  }
  #ifndef USE_PORTAUDIO
  _FUNCTION_RETURNV_
  #endif
}
#undef FUNCTION

// clear (set to zero) data of channel <c> in pcm buffer <obj>
// is must be in the range of 0...nChan-1
int pcmBuffer_chanZero(pPcmBuffer obj, int c) 
#define FUNCTION "pcmBuffer_chanZero"
#ifdef USE_PORTAUDIO
{
#else
{_FUNCTION_ENTER_
#endif
  if (obj != NULL) {
    #ifdef COMPILE_CHECKS
    if (obj->data == NULL) _FUNCTION_RETURN_( 0 );
    if (obj->nChan <= c) _FUNCTION_RETURN_( 0 );
    if (c < 0) _FUNCTION_RETURN_( 0 );
    #endif
    if (obj->memOrga == MEMORGA_INTERLV) 
      chanZero(obj->data + c*obj->chanSize,obj->nBlocksAlloc,obj->blockSize,obj->chanSize);
    else if (obj->memOrga == MEMORGA_SEPCH) 
      chanZero(obj->data + c*obj->chanSize,obj->nBlocksAlloc,obj->blockSize,0);
    else {
      #ifndef USE_PORTAUDIO
      _FUNCTION_RETURN_( 0 );
      #else
      return 0;
      #endif
    }
    #ifndef USE_PORTAUDIO
    _FUNCTION_RETURN_( 1 );
    #else
    return 0;
    #endif
  }
  #ifndef USE_PORTAUDIO
  _FUNCTION_RETURN_( 0 );
  #else
  return 0;
  #endif
}
#undef FUNCTION

// TODO:   control struct for e.g. channel conversions, where channels can be selected , etc.
//
// TODO:::: all checks if Channels, BPS, etc. of buffers is equal
//           if NOT: todo: conversions!!
//
// copy from buffer src (start to start+len) into dst starting at "skip" (in dst)
// all conversions between different buffer formats will be performed if
// neccessary
int pcmBuffer_copy_ext(pPcmBuffer src, pPcmBuffer dst, int start, int len, int skip, pPcmConversion conv)
#define FUNCTION "pcmBuffer_copy_ext"
#ifdef USE_PORTAUDIO
{
#else
{_FUNCTION_ENTER_
#endif
    
  int i,j,n;
  LONG_IDX nCopied = 0;
  
  #ifdef COMPILE_CHECKS
  if (dst == NULL) _FUNCTION_RETURN_( 0 );
  if (dst->data == NULL) _FUNCTION_RETURN_( 0 );
  if (src == NULL) _FUNCTION_RETURN_( 0 );
  if (len == 0) _FUNCTION_RETURN_( 0 );
  if (skip >= dst->nBlocksAlloc) _FUNCTION_RETURN_( 0 );  
  if (dst->nBlocks < skip) {
    FEATUM_WARNING(5,"dst->nBlocks < skip encountered!");
    //return 0; // ??
  }
  #endif
  if (len < 0) len = src->nBlocks; //_FUNCTION_RETURN_( 0 );

  //if (start+len > src->nBlocks) return 0; // OR copy less?? i.e. len = src->nBlocks - start; //??
  if (start+len > src->nBlocks) {
    #ifndef USE_PORTAUDIO
    FEATUM_DEBUG(8,"truncating copy length (less data available than requested) %i < %i",src->nBlocks - start,len);
    #endif
    len = src->nBlocks - start; //??
  }
  if (start < 0) start = 0;
  if (len + skip > dst->nBlocksAlloc) {
    #ifndef USE_PORTAUDIO
    FEATUM_DEBUG(8,"truncating copy length (less space available in target buffer than requested) %i - %i < %i",dst->nBlocksAlloc, skip,len);
    #endif
    len = dst->nBlocksAlloc -skip;
  }

/*
  if ((conv == NULL)||(conv->direct)) {  // do not route/mix channels
    if (src->nBPS == dst->nBPS) { // direct copy, no BPS (sampleType) conversion
   
    } else { // BPS conversion, direct channel copy
    
    }
  } else { // channel matrix routing (incl. BPS if applicable)
  
  }  
 */
  int silence = 0;
  if (src->data == NULL) silence = 1;
  
  if ((conv == NULL)||(conv->direct)) {  // do not route/mix channels
    if ((src->nBPS == dst->nBPS)&&(src->sampleType == dst->sampleType)) { // direct copy, no BPS (sampleType) and no channel conversion

      if (dst->memOrga == MEMORGA_SEPCH) {
        if (src->memOrga == MEMORGA_SEPCH) {

            i=0;
            if (!silence) {
              for (; i < min_i(dst->nChan,src->nChan); i++) {
                memcpy( dst->data+(skip*dst->blockSize) + i*dst->chanSize, src->data+(start*src->blockSize) + i*src->chanSize, len*src->blockSize );
              }
            }
            for ( ; i<dst->nChan; i++) { // fill excess channels with zeros
              memzero( dst->data+(skip*dst->blockSize) + i*dst->chanSize, len*dst->blockSize );
            }
            
        } else if (src->memOrga == MEMORGA_INTERLV) {
          void *psrc, *pdst; 
          i=0;
          if (!silence) {
            for (; i < min_i(dst->nChan,src->nChan); i++) {
              psrc = src->data + start*src->blockSize + i*src->chanSize;
              pdst = dst->data + skip*dst->blockSize + i*dst->chanSize;
              for(n=0; n < len; n++) {
                memcpy( pdst, psrc, dst->blockSize );
                psrc += src->blockSize;
                pdst += dst->blockSize;
              }
            }
          }
          for ( ; i<dst->nChan; i++) { // fill excess channels with zeros
            memzero( dst->data+(skip*dst->blockSize) + i*dst->chanSize, len*dst->blockSize );
          }
        }
      } else if (dst->memOrga == MEMORGA_INTERLV) {
        if (src->memOrga == MEMORGA_SEPCH) {

          void *psrc, *pdst;
          if (!silence) {
            for (i=0; i < min_i(dst->nChan,src->nChan); i++) {
              psrc = src->data + start*src->blockSize + i*src->chanSize;
              pdst = dst->data + skip*dst->blockSize + i*dst->chanSize;
              for(n=0; n < len; n++) {
                memcpy( pdst, psrc, dst->blockSize );
                psrc += src->blockSize;
                pdst += dst->blockSize;
              }
            }
            if (dst->nChan > src->nChan) {
              pdst = dst->data+(skip*dst->blockSize) + src->nChan*dst->chanSize;
              chanZero(pdst, len, dst->blockSize, (dst->nChan-src->nChan)*dst->chanSize);
            }  

          } else {
            chanZero(dst->data+(skip*dst->blockSize), len, dst->blockSize, (dst->nChan)*dst->chanSize);
          }
          
                         
        } else if (src->memOrga == MEMORGA_INTERLV) {
//               printf("SAME MEMO,BPS,TYPE....\n");
          if (dst->nChan == src->nChan) { // quick method
//               printf("SAME chan....\n");
            if (!silence) {
//                                         printf("->no silence, 1:1 copy .... bs: %i , bps: %i, chan: %i\n\n",src->blockSize,src->nBPS,src->nChan);
              memcpy( dst->data+(skip*dst->blockSize), src->data+(start*src->blockSize), len*src->blockSize );
            } else {
              memzero( dst->data+(skip*dst->blockSize), len*src->blockSize );
            }
          } else {
            void *pdst = dst->data+(skip*dst->blockSize);
            void *psrc = src->data+(start*src->blockSize);
            if (!silence) {
              for (n=0; n< len; n++) {
                memcpy( pdst,  psrc, src->chanSize * min_i(src->nChan, dst->nChan) );
                pdst += dst->blockSize;
                psrc += src->blockSize;
              }     
              if (dst->nChan > src->nChan) {
                pdst = dst->data+(skip*dst->blockSize) + src->nChan*dst->chanSize;
                chanZero(pdst, len, dst->blockSize, (dst->nChan-src->nChan)*dst->chanSize);
                /*for (n=0; n< len; n++) {
                  memzero( pdst, (dst->nChan-src->nChan) * dst->chanSize);
                  pdst += dst->blockSize;
                }*/
              }
            } // end if (!silence)
            else {
              chanZero(pdst, len, dst->blockSize, (dst->nChan)*dst->chanSize);
            }
          }               

        }                                      
      }
      dst->nBlocks = len+skip;
      nCopied = len;
   
    } else { // sampleType and BPS conversion, direct channel copy
   
          void *pdst, *psrc, *pdst0, *psrc0, *psrc1;
          int from;
          for (i=0; i < dst->nChan; i++) {
            pdst0 = dst->data+(skip*dst->blockSize) + i*dst->chanSize; pdst = pdst0;
            psrc0 = src->data+(start*src->blockSize) + i*src->chanSize;

            if (( i >= src->nChan)||(silence)) {  // no source channel available, zero the output channel
              //printf("i: %i, srcCh: %i, dstCh: %i\n",i,src->nChan, dst->nChan);
              chanZero(pdst0,len,dst->blockSize,dst->chanSize); // TODO: ... this only works for memorga_interlv!!
              
            } else {
                   
            switch(src->sampleType) {
            case SAMPLETYPE_I8:
              switch (dst->sampleType) {
              case SAMPLETYPE_I16: // src=8bit , target = 16bit 
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { 
                    *(SAMPLE16i *)pdst = (SAMPLE16i) (*(SAMPLE8i *)psrc) << 8;
                    pdst += dst->blockSize;
                  }  
                  break;
              case SAMPLETYPE_I24: // src=8bit , target = 24bit 
                  FEATUM_ERR_FATAL(1,"24-bit wave not yet implemented!");
                  _FUNCTION_RETURN_( 0 );
                  break;
              case SAMPLETYPE_FP: // src=8bit , target = FP(32bit)
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { 
                    *(SAMPLEFP *)pdst = (SAMPLEFP) (*(SAMPLE8i *)psrc) / 127.0;
                    pdst += dst->blockSize;
                  }  
                  break;
              }
              break;

            case SAMPLETYPE_I16:
              switch (dst->sampleType) {
              case SAMPLETYPE_I8: // src=16bit , target = 8bit 
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { 
                    *(SAMPLE8i *)pdst = (SAMPLE8i) (*(SAMPLE16i *)psrc >> 8);
                    pdst += dst->blockSize;
                  }  
                  break;
              case SAMPLETYPE_I24: // src=16bit , target = 24bit 
                  FEATUM_ERR_FATAL(1,"24-bit wave not yet implemented!");
                  _FUNCTION_RETURN_( 0 );
                  break;
              case SAMPLETYPE_FP: // src=16bit , target = FP(32bit)
                  #ifndef USE_PORTAUDIO
                  FEATUM_DEBUG(12,"(%i) [len:%i]: 16b to FP conversion  bs %i cs %i -> bs %i , cs %i ", i,len,src->blockSize, src->chanSize,dst->blockSize, dst->chanSize);
                  #endif
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { 
                    *(SAMPLEFP *)pdst = ((SAMPLEFP) (*(SAMPLE16i *)psrc) / 32767.0);
                    pdst += dst->blockSize;
                  }  
                  break;
              }
              break;

            case SAMPLETYPE_I24:
                  FEATUM_ERR_FATAL(1,"24-bit wave not yet implemented!");
                  _FUNCTION_RETURN_( 0 );
                  break;

            case SAMPLETYPE_FP:
              switch (dst->sampleType) {
              case SAMPLETYPE_I8: // src=FP(32bit) , target = 8bit 
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { 
                    *(SAMPLE8i *)pdst = (SAMPLE8i) ROUND(*(SAMPLEFP *)psrc * 127.0);
                    pdst += dst->blockSize;
                  }  
                  break;
              case SAMPLETYPE_I16: // src=FP(32bit) , target = 16bit
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { 
                    *(SAMPLE16i *)pdst = (SAMPLE16i) ROUND(*(SAMPLEFP *)psrc * 32767.0);
                    pdst += dst->blockSize;
                  }  
                  break;
              case SAMPLETYPE_I24: // src=FP(32bit) , target = 24bit
                  FEATUM_ERR_FATAL(1,"24-bit wave not yet implemented!");
                  _FUNCTION_RETURN_( 0 );
                  break;
              }
              break;
              

            } // END switch(src->sampleType)
            } // END else...
            
          } // END for()...
          dst->nBlocks = len+skip;
          #ifndef USE_PORTAUDIO
          FEATUM_DEBUG(11,"dst->nBlocks----> %i  (skip=%i)",dst->nBlocks,skip);
          #endif
          nCopied = len;
           
    } // END else (bps conversion)
  } else { // channel matrix routing (incl. BPS if applicable)

       if (silence) {
         FEATUM_WARNING(0,"silence is not yet implemented for the routing functions!!!!\n");
         _FUNCTION_RETURN_(0);
       }
       
          void *pdst, *psrc, *pdst0, *psrc0, *psrc1;
          int from;
          for (i=0; i < dst->nChan; i++) {
            pdst0 = dst->data+(skip*dst->blockSize) + i*dst->chanSize; pdst = pdst0;
            psrc0 = src->data+(start*src->blockSize); // + i*src->chanSize;
            
//          for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { 
            chanZero(pdst0,len,dst->blockSize,0); 
//          }
          
            switch(src->sampleType) {  
            case SAMPLETYPE_I8:
              switch (dst->sampleType) {
              case SAMPLETYPE_I8: // src=8bit , target = 8bit 
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { psrc1 = psrc; 
                    for ( from = 0; psrc1 < psrc + src->nChan * src->chanSize; psrc1 += src->chanSize) {
                       *(SAMPLE8i *)pdst += (SAMPLE8i)roundf( (float)(*(SAMPLE8i *)psrc1) * pcmConversion_getRoute(conv, from++, i) );
                    } pdst += dst->blockSize;
                  }  
                  break;
              case SAMPLETYPE_I16: // src=8bit , target = 16bit 
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { psrc1 = psrc; 
                    for ( from = 0; psrc1 < psrc + src->nChan * src->chanSize; psrc1 += src->chanSize) {
                       *(SAMPLE16i *)pdst += (SAMPLE16i)roundf( (float)(*(SAMPLE8i *)psrc1) * 256.0 * pcmConversion_getRoute(conv, from++, i) );
                    } pdst += dst->blockSize;
                  }  
                  break;
              case SAMPLETYPE_I24: // src=8bit , target = 24bit 
                  FEATUM_ERR_FATAL(1,"24-bit wave not yet implemented!");
                  _FUNCTION_RETURN_( 0 );
                  break;
              case SAMPLETYPE_FP: // src=8bit , target = 32bit (float / SAMPLEFP)
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { psrc1 = psrc; 
                    for ( from = 0; psrc1 < psrc + src->nChan * src->chanSize; psrc1 += src->chanSize) {
                       *(SAMPLEFP *)pdst += (SAMPLEFP)(*(SAMPLE8i *)psrc1) / 127.0 * (SAMPLEFP)pcmConversion_getRoute(conv, from++, i);
                    } pdst += dst->blockSize;
                  }  
                  break;
              #ifndef OPTIMIZE_SIZE
              default:
                FEATUM_ERR_FATAL(0," Illegal sampleType conversion! Destination sampleType not supported: %i",dst->sampleType);
                dst->nBlocks = 0;
                _FUNCTION_RETURN_( 0 );
              #endif  

              }
              break;
              
            case SAMPLETYPE_I16:
              switch (dst->sampleType) {
              case SAMPLETYPE_I8: // src=16bit , target = 8bit 
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { psrc1 = psrc; 
                    for ( from = 0; psrc1 < psrc + src->nChan * src->chanSize; psrc1 += src->chanSize) {
                       *(SAMPLE8i *)pdst += (SAMPLE8i)round( (float)(*(SAMPLE16i *)psrc1) * pcmConversion_getRoute(conv, from++, i) / 256.0 );
                    } pdst += dst->blockSize;
                  }  
                  break;
              case SAMPLETYPE_I16: // src=16bit , target = 16bit 
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { psrc1 = psrc; 
                    for ( from = 0; psrc1 < psrc + src->nChan * src->chanSize; psrc1 += src->chanSize) {
                       *(SAMPLE16i *)pdst += (SAMPLE16i)round( (float)(*(SAMPLE16i *)psrc1) * pcmConversion_getRoute(conv, from++, i) );
                    } pdst += dst->blockSize;
                  }  
                  break;
              case SAMPLETYPE_I24: // src=16bit , target = 24bit 
                  FEATUM_ERR_FATAL(1,"24-bit wave not yet implemented!");
                  _FUNCTION_RETURN_( 0 );
                  break;
              case SAMPLETYPE_FP: // src=16bit , target = 32bit (float / SAMPLEFP)
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { psrc1 = psrc; 
                    for ( from = 0; psrc1 < psrc + src->nChan * src->chanSize; psrc1 += src->chanSize) {
                       *(SAMPLEFP *)pdst += (SAMPLEFP)(*(SAMPLE16i *)psrc1) / 32767.0 * (SAMPLEFP)pcmConversion_getRoute(conv, from++, i);
                    } pdst += dst->blockSize;
                  }  
                  break;
              #ifndef OPTIMIZE_SIZE
              default:
                FEATUM_ERR_FATAL(0,"Illegal sampleType conversion! Destination sampleType not supported: %i",dst->sampleType);
                dst->nBlocks = 0;
                _FUNCTION_RETURN_( 0 );
              #endif  
              }
              break;
              
            case SAMPLETYPE_I24:
//                 FEATUM_DEBUG(4,"24: src type:  %i    dst type: %i",src->sampleType, dst->sampleType);
              switch (dst->sampleType) {
              case SAMPLETYPE_I32: // src=INT(24bit) , target = 32bit int (incl. rescaling!)
//                 FEATUM_DEBUG(4,"24->32:  src type:  %i    dst type: %i",src->sampleType, dst->sampleType);
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { psrc1 = psrc; 
                    FEATUM_DEBUG(12,"psrc %i, psrc0 %i, psrc1 %i, pdst %i, src->blockSize %i",(unsigned long)psrc,(unsigned long)psrc0,(unsigned long)psrc1,(unsigned long)pdst,src->blockSize);
                    for ( from = 0; psrc1 < psrc + src->nChan * src->chanSize; psrc1 += src->chanSize) {
                       *(SAMPLE32i *)pdst += (SAMPLE32i)round(  (double)unpack24i_le((SAMPLE24i *)psrc1) * (double)pcmConversion_getRoute(conv, from++, i) * 256.0);
                    } pdst += dst->blockSize;
                  }  
                  break;
              case SAMPLETYPE_I24: // src=INT(24bit) , target = INT(24bit)
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { psrc1 = psrc; 
                    FEATUM_DEBUG(12,"psrc %i, psrc0 %i, psrc1 %i, pdst %i, src->blockSize %i",(unsigned long)psrc,(unsigned long)psrc0,(unsigned long)psrc1,(unsigned long)pdst,src->blockSize);
                    for ( from = 0; psrc1 < psrc + src->nChan * src->chanSize; psrc1 += src->chanSize) {
                       add_sample24i_le_32( (SAMPLE24i *)pdst, (SAMPLE32i)round((double)unpack24i_le((SAMPLE24i *)psrc1) * (double)pcmConversion_getRoute(conv, from++, i) ) );
                       //*(SAMPLE32i *)pdst += (SAMPLE32i)round(  (double)unpack24i_le((SAMPLE24i *)psrc1) * (double)pcmConversion_getRoute(conv, from++, i) * 256.0);
                    } pdst += dst->blockSize;
                  }  
                  break;

              default:
                  FEATUM_ERR_FATAL(1,"24-bit wave not yet implemented for given conversion destination format!");
                  _FUNCTION_RETURN_( 0 );
                  break;
              }
              break;

            case SAMPLETYPE_I32:
//                 FEATUM_DEBUG(4,"32: src type:  %i    dst type: %i",src->sampleType, dst->sampleType);
              switch (dst->sampleType) {
              case SAMPLETYPE_I24: // src=INT(32bit) , target = 24-bit int (no rescaling?)
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { psrc1 = psrc; 
                    for ( from = 0; psrc1 < psrc + src->nChan * src->chanSize; psrc1 += src->chanSize) {
                       add_sample24i_le_32( (SAMPLE24i *)pdst, (SAMPLE32i)round((double)(*(SAMPLE32i *)psrc1) * (double)pcmConversion_getRoute(conv, from++, i) / 256.0) );
                    } pdst += dst->blockSize;
                  }  
                  break;
              default:
                  FEATUM_ERR_FATAL(1,"32-bit wave not yet implemented for given conversion destination format!");
                  _FUNCTION_RETURN_( 0 );
                  break;
              }
              break;

            case SAMPLETYPE_FP:
              switch (dst->sampleType) {
              case SAMPLETYPE_I8: // src=FP(32bit) , target = 8bit          
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { psrc1 = psrc; 
                    for ( from = 0; psrc1 < psrc + src->nChan * src->chanSize; psrc1 += src->chanSize) {
                       *(SAMPLE8i *)pdst += (SAMPLE8i)ROUND(  *(SAMPLEFP *)psrc1 * (SAMPLEFP)pcmConversion_getRoute(conv, from++, i) * 127.0 );
                    } pdst += dst->blockSize;
                  }  
                  break;
              case SAMPLETYPE_I16: // src=FP(32bit) , target = 16bit          
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { psrc1 = psrc; 
                    for ( from = 0; psrc1 < psrc + src->nChan * src->chanSize; psrc1 += src->chanSize) {
                       *(SAMPLE16i *)pdst += (SAMPLE16i)ROUND(  *(SAMPLEFP *)psrc1 * (SAMPLEFP)pcmConversion_getRoute(conv, from++, i) * 32767.0 );
                    } pdst += dst->blockSize;
                  }  
                  break;
              case SAMPLETYPE_I24: // src=FP(32bit) , target = 24bit          
                  FEATUM_ERR_FATAL(1,"24-bit wave not yet implemented!");
                  _FUNCTION_RETURN_( 0 );
                  break;
              case SAMPLETYPE_FP: // src=FP(32bit) , target = FP(32bit)         
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { psrc1 = psrc; 
                    for ( from = 0; psrc1 < psrc + src->nChan * src->chanSize; psrc1 += src->chanSize) {
                       *(SAMPLEFP *)pdst += *(SAMPLEFP *)psrc1 * (SAMPLEFP)pcmConversion_getRoute(conv, from++, i);
                    } pdst += dst->blockSize;
                  }  
                  break;
              #ifndef OPTIMIZE_SIZE
              default:
                FEATUM_ERR_FATAL(0,"Illegal sampleType conversion! Destination sampleType not supported: %i",dst->sampleType);
                dst->nBlocks = 0;
                _FUNCTION_RETURN_( 0 );
              #endif  

              }
              break;

            #ifndef OPTIMIZE_SIZE
            default:
              FEATUM_ERR_FATAL(0,"Illegal sampleType conversion! Source sampleType not supported: %i",src->sampleType);
              dst->nBlocks = 0;
              _FUNCTION_RETURN_( 0 );
            #endif  
                                 
          } // END  switch(src->sampleType)
        } // END for() [dst chan]

     dst->nBlocks = len+skip;
     nCopied = len;
  }  // END else [channel matrix routing]

  
/*       
  if (dst->memOrga == MEMORGA_SEPCH) {
    if (src->memOrga == MEMORGA_SEPCH) {
      
      if ((conv == NULL)||(conv->direct)) {  // do not route/mix channels
        if (src->nBPS == dst->nBPS) { // direct copy, no BPS (sampleType) conversion
          for (i=0; i < min_i(dst->nChan,src->nChan); i++) {
            memcpy( dst->data+(skip*dst->blockSize) + i*dst->chanSize, src->data+(start*src->blockSize) + i*src->chanSize, len*src->blockSize );
          }
          for ( ; i<dst->nChan; i++) { // fill excess channels with zeros
            memzero( dst->data+(start*dst->blockSize) + i*dst->chanSize, len*dst->blockSize );
          }
          dst->nBlocks = len+skip;
          nCopied = len;
           
        } else { // BPS conversion, direct channel copy
          void *pdst, *psrc, *pdst0, *psrc0, *psrc1;
          int from;
          for (i=0; i < dst->nChan; i++) {
            pdst0 = dst->data+(skip*dst->blockSize) + i*dst->chanSize; pdst = pdst0:
            psrc0 = src->data+(start*src->blockSize) + i*src->chanSize;

            if ( i >= src->nChan) {  // no source channel available, zero the output channel
              chanZero(pdst0,len,dst->blockSize,0); 
              
            } else {
                   
            switch(src->sampleType) {
            case SAMPLETYPE_I8:
              switch (dst->sampleType) {
              case SAMPLETYPE_I16: // src=8bit , target = 16bit 
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { 
                    *(SAMPLE16i *)pdst = (SAMPLE16i) (*(SAMPLE8i *)psrc) << 8;
                    pdst += dst->blockSize;
                  }  
                  break;
              case SAMPLETYPE_I24: // src=8bit , target = 24bit 
                  FEATUM_ERR_FATAL(1,"24-bit wave not yet implemented!");
                  return 0;
                  break;
              case SAMPLETYPE_FP: // src=8bit , target = FP(32bit)
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { 
                    *(SAMPLEFP *)pdst = (SAMPLEFP) (*(SAMPLE8i *)psrc) / 127.0;
                    pdst += dst->blockSize;
                  }  
                  break;
              }
              break;

            case SAMPLETYPE_I16:
              switch (dst->sampleType) {
              case SAMPLETYPE_I8: // src=16bit , target = 8bit 
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { 
                    *(SAMPLE8i *)pdst = (SAMPLE8i) (*(SAMPLE16i *)psrc >> 8);
                    pdst += dst->blockSize;
                  }  
                  break;
              case SAMPLETYPE_I24: // src=16bit , target = 24bit 
                  FEATUM_ERR_FATAL(1,"24-bit wave not yet implemented!");
                  return 0;
                  break;
              case SAMPLETYPE_FP: // src=16bit , target = FP(32bit)
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { 
                    *(SAMPLEFP *)pdst = (SAMPLEFP) (*(SAMPLE16i *)psrc) / 32767.0;
                    pdst += dst->blockSize;
                  }  
                  break;
              }
              break;

            case SAMPLETYPE_I24:
                  FEATUM_ERR_FATAL(1,"24-bit wave not yet implemented!");
                  return 0;
                  break;

            case SAMPLETYPE_FP:
              switch (dst->sampleType) {
              case SAMPLETYPE_I8: // src=FP(32bit) , target = 8bit 
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { 
                    *(SAMPLE8i *)pdst = (SAMPLE8i) round(*(SAMPLEFP *)psrc * 127.0);
                    pdst += dst->blockSize;
                  }  
                  break;
              case SAMPLETYPE_16: // src=FP(32bit) , target = 16bit
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { 
                    *(SAMPLE16i *)pdst = (SAMPLE16i) round(*(SAMPLEFP *)psrc * 32767.0);
                    pdst += dst->blockSize;
                  }  
                  break;
              case SAMPLETYPE_I24: // src=FP(32bit) , target = 24bit
                  FEATUM_ERR_FATAL(1,"24-bit wave not yet implemented!");
                  return 0;
                  break;
              }
              break;
              

            } // END switch(src->sampleType)
            }
            
          } // END for()...
          dst->nBlocks = len+skip;
          nCopied = len;
           
        } // END else (bps conversion)

      } else { // channel matrix routing (incl. BPS if applicable)

          void *pdst, *psrc, *pdst0, *psrc0, *psrc1;
          int from;
          for (i=0; i < dst->nChan; i++) {
            pdst0 = dst->data+(skip*dst->blockSize) + i*dst->chanSize; pdst = pdst0:
            psrc0 = src->data+(start*src->blockSize); // + i*src->chanSize;
            
//          for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { 
            chanZero(pdst0,len,dst->blockSize,0); 
//          }
          
            switch(src->sampleType) {  
            case SAMPLETYPE_I8:
              switch (dst->sampleType) {
              case SAMPLETYPE_I8: // src=8bit , target = 8bit 
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { *psrc1 = psrc; 
                    for ( from = 0; psrc1 < psrc + src->nChan * src->chanSize; psrc1 += src->chanSize) {
                       *(SAMPLE8i *)pdst += (SAMPLE8i)round( (float)(*(SAMPLE8i *)psrc1) * pcmConversion_getRoute(conv, from++, i) );
                    } pdst += dst->blockSize;
                  }  
                  break;
              case SAMPLETYPE_I16: // src=8bit , target = 16bit 
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { *psrc1 = psrc; 
                    for ( from = 0; psrc1 < psrc + src->nChan * src->chanSize; psrc1 += src->chanSize) {
                       *(SAMPLE16i *)pdst += (SAMPLE16i)round( (float)(*(SAMPLE8i *)psrc1) * 256.0 * pcmConversion_getRoute(conv, from++, i) );
                    } pdst += dst->blockSize;
                  }  
                  break;
              case SAMPLETYPE_I24: // src=8bit , target = 24bit 
                  FEATUM_ERR_FATAL(1,"24-bit wave not yet implemented!");
                  return 0;
                  break;
              case SAMPLETYPE_FP: // src=8bit , target = 32bit (float / SAMPLEFP)
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { *psrc1 = psrc; 
                    for ( from = 0; psrc1 < psrc + src->nChan * src->chanSize; psrc1 += src->chanSize) {
                       *(SAMPLEFP *)pdst += (SAMPLEFP)(*(SAMPLE8i *)psrc1) / 127.0 * (SAMPLEFP)pcmConversion_getRoute(conv, from++, i);
                    } pdst += dst->blockSize;
                  }  
                  break;
              #ifndef OPTIMIZE_SIZE
              default:
                FEATUM_ERR_FATAL(0,"_copy: Illegal sampleType conversion! Destination sampleType not supported: %i",dst->sampleType);
                dst->nBlocks = 0;
                return 0;
              #endif  

              }
              break;
              
            case SAMPLETYPE_I16:
              switch (dst->sampleType) {
              case SAMPLETYPE_I8: // src=16bit , target = 8bit 
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { *psrc1 = psrc; 
                    for ( from = 0; psrc1 < psrc + src->nChan * src->chanSize; psrc1 += src->chanSize) {
                       *(SAMPLE8i *)pdst += (SAMPLE8i)round( (float)(*(SAMPLE16i *)psrc1) * pcmConversion_getRoute(conv, from++, i) / 256.0 );
                    } pdst += dst->blockSize;
                  }  
                  break;
              case SAMPLETYPE_I16: // src=16bit , target = 16bit 
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { *psrc1 = psrc; 
                    for ( from = 0; psrc1 < psrc + src->nChan * src->chanSize; psrc1 += src->chanSize) {
                       *(SAMPLE16i *)pdst += (SAMPLE16i)round( (float)(*(SAMPLE16i *)psrc1) * pcmConversion_getRoute(conv, from++, i) );
                    } pdst += dst->blockSize;
                  }  
                  break;
              case SAMPLETYPE_I24: // src=16bit , target = 24bit 
                  FEATUM_ERR_FATAL(1,"24-bit wave not yet implemented!");
                  return 0;
                  break;
              case SAMPLETYPE_FP: // src=16bit , target = 32bit (float / SAMPLEFP)
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { *psrc1 = psrc; 
                    for ( from = 0; psrc1 < psrc + src->nChan * src->chanSize; psrc1 += src->chanSize) {
                       *(SAMPLEFP *)pdst += (SAMPLEFP)(*(SAMPLE16i *)psrc1) / 32767.0 * (SAMPLEFP)pcmConversion_getRoute(conv, from++, i);
                    } pdst += dst->blockSize;
                  }  
                  break;
              #ifndef OPTIMIZE_SIZE
              default:
                FEATUM_ERR_FATAL(0,"_copy: Illegal sampleType conversion! Destination sampleType not supported: %i",dst->sampleType);
                dst->nBlocks = 0;
                return 0;
              #endif  
              }
              break;
              
            case SAMPLETYPE_I24:
                  FEATUM_ERR_FATAL(1,"24-bit wave not yet implemented!");
                  return 0;
                  break;

            case SAMPLETYPE_FP:
              switch (dst->sampleType) {
              case SAMPLETYPE_I8: // src=FP(32bit) , target = 8bit          
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { *psrc1 = psrc; 
                    for ( from = 0; psrc1 < psrc + src->nChan * src->chanSize; psrc1 += src->chanSize) {
                       *(SAMPLE8i *)pdst += (SAMPLE8i)round(  *(SAMPLEFP *)psrc1 * (SAMPLEFP)pcmConversion_getRoute(conv, from++, i) * 127.0 );
                    } pdst += dst->blockSize;
                  }  
                  break;
              case SAMPLETYPE_I16: // src=FP(32bit) , target = 16bit          
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { *psrc1 = psrc; 
                    for ( from = 0; psrc1 < psrc + src->nChan * src->chanSize; psrc1 += src->chanSize) {
                       *(SAMPLE16i *)pdst += (SAMPLE16i)round(  *(SAMPLEFP *)psrc1 * (SAMPLEFP)pcmConversion_getRoute(conv, from++, i) * 32767.0 );
                    } pdst += dst->blockSize;
                  }  
                  break;
              case SAMPLETYPE_I24: // src=FP(32bit) , target = 24bit          
                  FEATUM_ERR_FATAL(1,"24-bit wave not yet implemented!");
                  return 0;
                  break;
              case SAMPLETYPE_FP: // src=FP(32bit) , target = FP(32bit)         
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { *psrc1 = psrc; 
                    for ( from = 0; psrc1 < psrc + src->nChan * src->chanSize; psrc1 += src->chanSize) {
                       *(SAMPLEFP *)pdst += *(SAMPLEFP *)psrc1 * (SAMPLEFP)pcmConversion_getRoute(conv, from++, i);
                    } pdst += dst->blockSize;
                  }  
                  break;
              #ifndef OPTIMIZE_SIZE
              default:
                FEATUM_ERR_FATAL(0,"_copy: Illegal sampleType conversion! Destination sampleType not supported: %i",dst->sampleType);
                dst->nBlocks = 0;
                return 0;
              #endif  

              }
              break;

            #ifndef OPTIMIZE_SIZE
            default:
              FEATUM_ERR_FATAL(0,"_copy: Illegal sampleType conversion! Source sampleType not supported: %i",src->sampleType);
              dst->nBlocks = 0;
              return 0;
            #endif  
                                 
          } // END  switch(src->sampleType)
        } // END for() [dst chan]
        
      }  // END else [channel matrix routing]

    } else if (src->memOrga == MEMORGA_INTERLV) {  //************** conversions from INTERLV TO SEPCH********************
    
      if ((conv == NULL)||(conv->direct)) {  // do not route/mix channels
        if (src->nBPS == dst->nBPS) { // direct copy, no BPS (sampleType) conversion
          void *psrc, *pdst;
          for (i=0; i < min_i(dst->nChan,src->nChan); i++) {
            psrc = src->data + start*src->blockSize + i*src->chanSize;
            pdst = dst->data + skip*dst->blockSize + i*dst->chanSize
            for(n=0; n < len; n++) {
              memcpy( pdst, psrc, dst->blockSize );
              psrc += src->blockSize;
              pdst += dst->blockSize;
            }
          }
          for ( ; i<dst->nChan; i++) { // fill excess channels with zeros
            memzero( dst->data+(start*dst->blockSize) + i*dst->chanSize, len*dst->blockSize );
          }
          dst->nBlocks = len+skip;
          nCopied = len;
           
        } else { // BPS conversion, direct channel copy
          void *pdst, *psrc, *pdst0, *psrc0, *psrc1;
          int from;
          for (i=0; i < dst->nChan; i++) {
            pdst0 = dst->data+(skip*dst->blockSize) + i*dst->chanSize; pdst = pdst0:
            psrc0 = src->data+(start*src->blockSize) + i*src->chanSize;

            if ( i >= src->nChan) {  // no source channel available, zero the output channel
              chanZero(pdst0,len,dst->blockSize,0); 
              
            } else {
                   
            switch(src->sampleType) {
            case SAMPLETYPE_I8:
              switch (dst->sampleType) {
              case SAMPLETYPE_I16: // src=8bit , target = 16bit 
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { 
                    *(SAMPLE16i *)pdst = (SAMPLE16i) (*(SAMPLE8i *)psrc) << 8;
                    pdst += dst->blockSize;
                  }  
                  break;
              case SAMPLETYPE_I24: // src=8bit , target = 24bit 
                  FEATUM_ERR_FATAL(1,"24-bit wave not yet implemented!");
                  return 0;
                  break;
              case SAMPLETYPE_FP: // src=8bit , target = FP(32bit)
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { 
                    *(SAMPLEFP *)pdst = (SAMPLEFP) (*(SAMPLE8i *)psrc) / 127.0;
                    pdst += dst->blockSize;
                  }  
                  break;
              }
              break;

            case SAMPLETYPE_I16:
              switch (dst->sampleType) {
              case SAMPLETYPE_I8: // src=16bit , target = 8bit 
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { 
                    *(SAMPLE8i *)pdst = (SAMPLE8i) (*(SAMPLE16i *)psrc >> 8);
                    pdst += dst->blockSize;
                  }  
                  break;
              case SAMPLETYPE_I24: // src=16bit , target = 24bit 
                  FEATUM_ERR_FATAL(1,"24-bit wave not yet implemented!");
                  return 0;
                  break;
              case SAMPLETYPE_FP: // src=16bit , target = FP(32bit)
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { 
                    *(SAMPLEFP *)pdst = (SAMPLEFP) (*(SAMPLE16i *)psrc) / 32767.0;
                    pdst += dst->blockSize;
                  }  
                  break;
              }
              break;

            case SAMPLETYPE_I24:
                  FEATUM_ERR_FATAL(1,"24-bit wave not yet implemented!");
                  return 0;
                  break;

            case SAMPLETYPE_FP:
              switch (dst->sampleType) {
              case SAMPLETYPE_I8: // src=FP(32bit) , target = 8bit 
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { 
                    *(SAMPLE8i *)pdst = (SAMPLE8i) round(*(SAMPLEFP *)psrc * 127.0);
                    pdst += dst->blockSize;
                  }  
                  break;
              case SAMPLETYPE_16: // src=FP(32bit) , target = 16bit
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { 
                    *(SAMPLE16i *)pdst = (SAMPLE16i) round(*(SAMPLEFP *)psrc * 32767.0);
                    pdst += dst->blockSize;
                  }  
                  break;
              case SAMPLETYPE_I24: // src=FP(32bit) , target = 24bit
                  FEATUM_ERR_FATAL(1,"24-bit wave not yet implemented!");
                  return 0;
                  break;
              }
              break;
              

            } // END switch(src->sampleType)
            }
            
          } // END for()...
          dst->nBlocks = len+skip;
          nCopied = len;
           
        } // END else (bps conversion)
              
      } else { // channel matrix routing (incl. BPS and memOrga conversion)
             
          void *pdst, *psrc, *pdst0, *psrc0, *psrc1;
          int from;
          for (i=0; i < dst->nChan; i++) {
            pdst0 = dst->data+(skip*dst->blockSize) + i*dst->chanSize; pdst = pdst0:
            psrc0 = src->data+(start*src->blockSize); // + i*src->chanSize;
            
//          for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { 
            chanZero(pdst0,len,dst->blockSize,1);
//            memzero(pdst0,len*dst->blockSize); 
//          }
          
            switch(src->sampleType) {  
            case SAMPLETYPE_I8:
              switch (dst->sampleType) {
              case SAMPLETYPE_I8: // src=8bit , target = 8bit 
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { *psrc1 = psrc; 
                    for ( from = 0; psrc1 < psrc + src->nChan * src->chanSize; psrc1 += src->chanSize) {
                       *(SAMPLE8i *)pdst += (SAMPLE8i)round( (float)(*(SAMPLE8i *)psrc1) * pcmConversion_getRoute(conv, from++, i) );
                    } pdst += dst->blockSize;
                  }  
                  break;
              case SAMPLETYPE_I16: // src=8bit , target = 16bit 
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { *psrc1 = psrc; 
                    for ( from = 0; psrc1 < psrc + src->nChan * src->chanSize; psrc1 += src->chanSize) {
                       *(SAMPLE16i *)pdst += (SAMPLE16i)round( (float)(*(SAMPLE8i *)psrc1) * 256.0 * pcmConversion_getRoute(conv, from++, i) );
                    } pdst += dst->blockSize;
                  }  
                  break;
              case SAMPLETYPE_I24: // src=8bit , target = 24bit 
                  FEATUM_ERR_FATAL(1,"24-bit wave not yet implemented!");
                  return 0;
                  break;
              case SAMPLETYPE_FP: // src=8bit , target = 32bit (float / SAMPLEFP)
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { *psrc1 = psrc; 
                    for ( from = 0; psrc1 < psrc + src->nChan * src->chanSize; psrc1 += src->chanSize) {
                       *(SAMPLEFP *)pdst += (SAMPLEFP)(*(SAMPLE8i *)psrc1) / 127.0 * (SAMPLEFP)pcmConversion_getRoute(conv, from++, i);
                    } pdst += dst->blockSize;
                  }  
                  break;
              #ifndef OPTIMIZE_SIZE
              default:
                FEATUM_ERR_FATAL(0,"_copy: Illegal sampleType conversion! Destination sampleType not supported: %i",dst->sampleType);
                dst->nBlocks = 0;
                return 0;
              #endif  

              }
              break;
              
            case SAMPLETYPE_I16:
              switch (dst->sampleType) {
              case SAMPLETYPE_I8: // src=16bit , target = 8bit 
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { *psrc1 = psrc; 
                    for ( from = 0; psrc1 < psrc + src->nChan * src->chanSize; psrc1 += src->chanSize) {
                       *(SAMPLE8i *)pdst += (SAMPLE8i)round( (float)(*(SAMPLE16i *)psrc1) * pcmConversion_getRoute(conv, from++, i) / 256.0 );
                    } pdst += dst->blockSize;
                  }  
                  break;
              case SAMPLETYPE_I16: // src=16bit , target = 16bit 
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { *psrc1 = psrc; 
                    for ( from = 0; psrc1 < psrc + src->nChan * src->chanSize; psrc1 += src->chanSize) {
                       *(SAMPLE16i *)pdst += (SAMPLE16i)round( (float)(*(SAMPLE16i *)psrc1) * pcmConversion_getRoute(conv, from++, i) );
                    } pdst += dst->blockSize;
                  }  
                  break;
              case SAMPLETYPE_I24: // src=16bit , target = 24bit 
                  FEATUM_ERR_FATAL(1,"24-bit wave not yet implemented!");
                  return 0;
                  break;
              case SAMPLETYPE_FP: // src=16bit , target = 32bit (float / SAMPLEFP)
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { *psrc1 = psrc; 
                    for ( from = 0; psrc1 < psrc + src->nChan * src->chanSize; psrc1 += src->chanSize) {
                       *(SAMPLEFP *)pdst += (SAMPLEFP)(*(SAMPLE16i *)psrc1) / 32767.0 * (SAMPLEFP)pcmConversion_getRoute(conv, from++, i);
                    } pdst += dst->blockSize;
                  }  
                  break;
              #ifndef OPTIMIZE_SIZE
              default:
                FEATUM_ERR_FATAL(0,"_copy: Illegal sampleType conversion! Destination sampleType not supported: %i",dst->sampleType);
                dst->nBlocks = 0;
                return 0;
              #endif  
              }
              break;
              
            case SAMPLETYPE_I24:
                  FEATUM_ERR_FATAL(1,"24-bit wave not yet implemented!");
                  return 0;
                  break;

            case SAMPLETYPE_FP:
              switch (dst->sampleType) {
              case SAMPLETYPE_I8: // src=FP(32bit) , target = 8bit          
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { *psrc1 = psrc; 
                    for ( from = 0; psrc1 < psrc + src->nChan * src->chanSize; psrc1 += src->chanSize) {
                       *(SAMPLE8i *)pdst += (SAMPLE8i)round(  *(SAMPLEFP *)psrc1 * (SAMPLEFP)pcmConversion_getRoute(conv, from++, i) * 127.0 );
                    } pdst += dst->blockSize;
                  }  
                  break;
              case SAMPLETYPE_I16: // src=FP(32bit) , target = 16bit          
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { *psrc1 = psrc; 
                    for ( from = 0; psrc1 < psrc + src->nChan * src->chanSize; psrc1 += src->chanSize) {
                       *(SAMPLE16i *)pdst += (SAMPLE16i)round(  *(SAMPLEFP *)psrc1 * (SAMPLEFP)pcmConversion_getRoute(conv, from++, i) * 32767.0 );
                    } pdst += dst->blockSize;
                  }  
                  break;
              case SAMPLETYPE_I24: // src=FP(32bit) , target = 24bit          
                  FEATUM_ERR_FATAL(1,"24-bit wave not yet implemented!");
                  return 0;
                  break;
              case SAMPLETYPE_FP: // src=FP(32bit) , target = FP(32bit)         
                  for ( psrc = psrc0; psrc < psrc0 + len*src->blockSize; psrc += src->blockSize) { *psrc1 = psrc; 
                    for ( from = 0; psrc1 < psrc + src->nChan * src->chanSize; psrc1 += src->chanSize) {
                       *(SAMPLEFP *)pdst += *(SAMPLEFP *)psrc1 * (SAMPLEFP)pcmConversion_getRoute(conv, from++, i);
                    } pdst += dst->blockSize;
                  }  
                  break;
              #ifndef OPTIMIZE_SIZE
              default:
                FEATUM_ERR_FATAL(0,"_copy: Illegal sampleType conversion! Destination sampleType not supported: %i",dst->sampleType);
                dst->nBlocks = 0;
                return 0;
              #endif  

              }
              break;

            #ifndef OPTIMIZE_SIZE
            default:
              FEATUM_ERR_FATAL(0,"_copy: Illegal sampleType conversion! Source sampleType not supported: %i",src->sampleType);
              dst->nBlocks = 0;
              return 0;
            #endif  
                                 
          } // END  switch(src->sampleType)
        } // END for() [dst chan]
        
      }  // END else [channel matrix routing]
    }
  } else if (dst->memOrga == MEMORGA_INTERLV) {
    if (src->memOrga == MEMORGA_SEPCH) {  // conversions from INTERLV to SEPCH
      
      if ((conv == NULL)||(conv->direct)) {  // do not route/mix channels

      }
      
    } else if (src->memOrga == MEMORGA_INTERLV) {
     
           
    }
  }




*/




/*** old code***/    

/*
  if (dst->memOrga == src->memOrga) {
    
    if (dst->nChan == src->nChan) {
                   
      if (dst->nBPS == src->nBPS) {  
        // treat different memorgas differently
        // apart from this, all is the same, just copy
        if (dst->memOrga == MEMORGA_INTERLV) {
          memcpy( dst->data+(skip*dst->blockSize), src->data+(start*src->blockSize), len*src->blockSize );
          dst->nBlocks = len+skip;
          nCopied = len;
        } else if (dst->memOrga == MEMORGA_SEPCH) {
          for (i=0; i<dst->nChan; i++) {
            memcpy( dst->data+(skip*dst->blockSize) + i*dst->chanSize, src->data+(start*src->blockSize) + i*src->chanSize, len*src->blockSize );
          }
          dst->nBlocks = len+skip;
          nCopied = len;
        }
        
      } else {
        // convert BPS, 8bit <-> 16bit <-> 24bit <-> FP (32bit)       
           //        dst->nBlocks = min_l(src->nBlocks,dst->nBlocks);
        void *tmpdst = (void *)dst->data+(skip*dst->blockSize);
        void *tmpsrc = (void *)src->data+(start*src->blockSize);
        dst->nBlocks = end-start+skip;
        nCopied = end-start;
        
        switch ((src->sampleType << 8) + (dst->sampleType)) {
          case (SAMPLETYPE_I8 << 8)+SAMPLETYPE_I16: // src=8bit , target = 16bit 
            for (i=0; i < (end-start)*dst->nChan; i++) {
              *(SAMPLE16i *)tmpdst = (SAMPLE16i)( *(SAMPLE8i *)tmpsrc ) << 8;
              tmpdst += sizeof(SAMPLE16i); tmpsrc += sizeof(SAMPLE8i);
            }
            break;
          case (SAMPLETYPE_I8 << 8)+SAMPLETYPE_I24: // src=8bit , target = 24bit 
            FEATUM_ERR_FATAL(1,"24-bit wave not yet implemented!");
            return 0;
            break;
          case (SAMPLETYPE_I8 << 8)+SAMPLETYPE_FP: // src=8bit , target = 32bit (float / SAMPLEFP)
            for (i=0; i < (end-start)*dst->nChan; i++) {
              *(SAMPLEFP *)tmpdst = (SAMPLEFP)( *(SAMPLE8i *)tmpsrc ) / 127.0;
              tmpdst += sizeof(SAMPLEFP); tmpsrc += sizeof(SAMPLE8i);
            }
            break;

          case (SAMPLETYPE_I16 << 8)+SAMPLETYPE_I8: // src=16bit , target = 8bit 
            for (i=0; i < (end-start)*dst->nChan; i++) {
              *(SAMPLE8i *)tmpdst = (SAMPLE8i)( *(SAMPLE16i *)tmpsrc >> 8 );
              tmpdst += sizeof(SAMPLE8i); tmpsrc += sizeof(SAMPLE16i);
            }
            break;
          case (SAMPLETYPE_I16 << 8)+SAMPLETYPE_I16: // src=16bit , target = 24bit 
            FEATUM_ERR_FATAL(1,"24-bit wave not yet implemented!");
            return 0;
            break;
          case (SAMPLETYPE_I16 << 8)+SAMPLETYPE_FP: // src=16bit , target = 32bit (float / SAMPLEFP)
            for (i=0; i < (end-start)*dst->nChan; i++) {
              *(SAMPLEFP *)tmpdst = (SAMPLEFP)( *(SAMPLE16i *)tmpsrc ) / 32767.0;
              tmpdst += sizeof(SAMPLEFP); tmpsrc += sizeof(SAMPLE16i);
            }
            break;

          case (SAMPLETYPE_I24 << 8)+SAMPLETYPE_I8: // src=24bit , target = 8bit
            FEATUM_ERR_FATAL(1,"24-bit wave not yet implemented!");
            return 0;
            break;
          case (SAMPLETYPE_I24 << 8)+SAMPLETYPE_I16: // src=24bit , target = 16bit 
            FEATUM_ERR_FATAL(1,"24-bit wave not yet implemented!");
            return 0;
            break;
          case (SAMPLETYPE_I24 << 8)+SAMPLETYPE_FP: // src=24bit , target = 32bit (float) 
            FEATUM_ERR_FATAL(1,"24-bit wave not yet implemented!");
            return 0;
            break;

          case (SAMPLETYPE_FP << 8)+SAMPLETYPE_I8: // src=32bit , target = 8bit 
            for (i=0; i < (end-start)*dst->nChan; i++) {
              *(SAMPLE8i *)tmpdst = (SAMPLE8i)( *(SAMPLEFP *)tmpsrc * 127.0 );
              tmpdst += sizeof(SAMPLE8i); tmpsrc += sizeof(SAMPLEFP);
            }
            break;
          case (SAMPLETYPE_FP << 8)+SAMPLETYPE_I16: // src=32bit , target = 16bit 
            for (i=0; i < (end-start)*dst->nChan; i++) {
              *(SAMPLE16i *)tmpdst = (SAMPLE16i)( *(SAMPLEFP *)tmpsrc * 32767.0 );
              tmpdst += sizeof(SAMPLE16i); tmpsrc += sizeof(SAMPLEFP);
            }
            break;
          case (SAMPLETYPE_FP << 8)+SAMPLETYPE_I24: // src=32bit , target = 24bit
            FEATUM_ERR_FATAL(1,"24-bit wave not yet implemented!");
            return 0;
            break;
            
          default:
            FEATUM_ERR_FATAL(0,"Illegal BPS conversion in copy_buffer!");
            dst->nBlocks = 0;
            return 0;
            break;
        }
      }

    } else {  
      // convert number of channels
      // if dst > src, simply zero, if dst < source, remove
      // if dst < src && dst == 1 : mono mixdown

      if (dst->nBPS == src->nBPS) {  
        // convert only number of channels  , treat memorgas separately!
        
        switch( dst->memOrga ) {
          case MEMORGA_INTERLV :
               
               break;
          case MEMORGA_SEPCH :
               if (dst->nChan > src->nChan) { // zero unused channels
               
               } else { // copy only dst channels
               
               }
               break;
          default:               
            FEATUM_ERR_FATAL(1,"_copy: MemOrga %i not implemented!",dst->memOrga);
            return 0;
        }
        FEATUM_ERR_FATAL(1,"Conversion between multiple channels is not yet implemented!");
        return 0;
      */  
/*        switch(dst->nBPS) {
          case 1:
            for (i=0; i < dst->nBlocks * dst->nChan; i++) {
             ((SAMPLE8 *)dst->data)[i] = (SAMPLE8)( ((SAMPLE8 *)src->data)[i] ) << 8;
            }
            break;
          case 2:
               
        }*/
/*        
      } else {
        // convert BPS, 8bit <-> 16bit <-> 24bit <-> FP (32bit)       
        
        FEATUM_ERR_FATAL(1,"Conversion between multiple channels and BPS is not yet implemented!");
        return 0;

             
      }
      
    } 
    
    
  } else {
    // change memOrga from INTERLV to SEQ... complicated...
    
    if (dst->nChan == src->nChan) {
                   
      if (dst->nBPS == src->nBPS) {  
        // all is the same, just copy, but change memOrga
        dst->nBlocks = end-start+skip;
        nCopied = end-start;
        
        for (i=0; i < src->nChan; i++) {
//          FEATUM_DEBUG(20,"copy chan i = %i, start = %i,  end = %i, skip = %i",i, start,end,skip);
          void * tmpsrc = (void *)src->data+(start*src->blockSize + i*src->chanSize);
//          FEATUM_DEBUG(20,"src cs %i , dst cs %i, s nBPS = %i, d nBPS = %i" ,src->blockSize,dst->blockSize,src->nBPS,dst->nBPS);
          void * tmpdst = (void *)dst->data+(skip *dst->blockSize + i*dst->chanSize);
          for (j=0; j< end-start; j++) {
              //FEATUM_DEBUG(20," copy j = %i",j);
            memcpy( tmpdst, tmpsrc , src->nBPS ); 
            tmpsrc += src->blockSize;
            tmpdst += dst->blockSize;
          }    
        }
      
      } else {
        // convert BPS and memOrga, 8bit <-> 16bit <-> 24bit <-> FP (32bit)       
        FEATUM_ERR_FATAL(1,"Conversion between memOrga and BPS is not yet implemented!");
        return 0;
             
      }

    } else {  
      // convert number of channels
      // if dst > src, simply zero, if dst < source, remove
      // if dst < src && dst == 1 : mono mixdown

      if (dst->nBPS == src->nBPS) {  
        FEATUM_ERR_FATAL(1,"Conversion between multiple channels and memOrga is not yet implemented!");
        return 0;
      
      } else {
        // convert BPS, 8bit <-> 16bit <-> 24bit <-> FP (32bit)       
        FEATUM_ERR_FATAL(1,"Conversion between multiple channels, BPS and memOrga is not yet implemented!");
        return 0;
      }
      
    } 
    
  } 
  */

  // ????
  dst->timestampSamples = src->timestampSamples;
  dst->timestamp = src->timestamp;

  
  if (dst->nBlocks > dst->nBlocksAlloc) dst->nBlocks = dst->nBlocksAlloc;
  #ifdef USE_PORTAUDIO
  return nCopied;
  #else
  _FUNCTION_RETURN_( nCopied );
  #endif

}
#undef FUNCTION 


// creates a copy of the buffer <src>, allocate a new object for the new buffer
pPcmBuffer pcmBuffer_duplicate( pPcmBuffer src ) 
#define FUNCTION "pcmBuffer_duplicate"
{_FUNCTION_ENTER_
  pPcmBuffer dst = NULL;
  if (src != NULL) {
    dst = pcmBufferAllocate(dst, src->nBlocksAlloc, src->nChan, src->sampleType, src->memOrga, src->sampleRate);   

    if (dst != NULL) {
      // copy parameters
      dst->nBlocks = src->nBlocks; 
      dst->timestampSamples = src->timestampSamples;
      dst->timestamp = src->timestamp;
      //dst->sampleRate = src->sampleRate;
      
      // copy pcm data
      if (src->data != NULL) 
        memcpy(dst->data, src->data, dst->nBytesAlloc);
/*      else 
        memzero(dst->data, dst->nBytesAlloc);*/
      
      //clone uData struct, if allocated
      if (src->uData) {
        int i;
        for (i=0; i < nPCMUSERDATAENTRIES; i++) {
          if ((src->uData->entry[i] != NULL)&&(src->uData->entrySize[i] > 0)) 
            pcmBuffer_uDataAllocf(dst, i, src->uData->entrySize[i]);
        }
      } 
    }
    

  } 
  _FUNCTION_RETURN_(dst);         
}
#undef FUNCTION 


// creates a copy of the buffer <src>, allocate a new object for the new buffer
void pcmBuffer_duplicateData( pPcmBuffer src, pPcmBuffer dst ) 
#define FUNCTION "pcmBuffer_duplicateData"
{_FUNCTION_ENTER_
  if ((src != NULL)&&(dst != NULL)) {
      // copy parameters
      dst->nBlocks = src->nBlocks; 
      dst->timestampSamples = src->timestampSamples;
      dst->timestamp = src->timestamp;
      //dst->sampleRate = src->sampleRate;
      
      // copy pcm data
      if (src->data != NULL) 
        memcpy(dst->data, src->data, dst->nBytesAlloc);
      else 
        memzero(dst->data, dst->nBytesAlloc);
      
      //clone uData struct, if allocated
      if (src->uData) {
        int i;
        for (i=0; i < nPCMUSERDATAENTRIES; i++) {
          if ((src->uData->entry[i] != NULL)&&(src->uData->entrySize[i] > 0)) 
            pcmBuffer_uDataAllocf(dst, i, src->uData->entrySize[i]);
        }
      } 
  } 
  _FUNCTION_RETURNV_
}
#undef FUNCTION 


// creates a copy of the buffer <src>, allocate a new object for the new buffer with different size, pad with zeros if new size is larger
pPcmBuffer pcmBuffer_duplicateResize( pPcmBuffer src, LONG_IDX nBlocksDst ) 
#define FUNCTION "pcmBuffer_duplicateResize"
{_FUNCTION_ENTER_
  pPcmBuffer dst = NULL;
  if (src != NULL) {
    dst = pcmBufferAllocate(dst, nBlocksDst, src->nChan, src->sampleType, src->memOrga, src->sampleRate);   

    if (dst != NULL) {
      // copy parameters
      dst->nBlocks = src->nBlocks; 
      dst->timestampSamples = src->timestampSamples;
      dst->timestamp = src->timestamp;
      
      // copy pcm data
      if (src->data != NULL) 
        if (src->memOrga == MEMORGA_INTERLV)
          memcpy(dst->data, src->data, MINIMUM(dst->nBytesAlloc,src->nBytesAlloc));
        else if (src->memOrga == MEMORGA_SEPCH)
          // TODO.... this is broken for more than the first cahnnel... add for(ch=...) memcpy...
          memcpy(dst->data, src->data, MINIMUM(dst->nBytesAlloc,src->nBlocks*src->nBPS));

      
      //clone uData struct, if allocated
      if (src->uData) {
        int i;
        for (i=0; i < nPCMUSERDATAENTRIES; i++) {
          if ((src->uData->entry[i] != NULL)&&(src->uData->entrySize[i] > 0)) 
            pcmBuffer_uDataAllocf(dst, i, src->uData->entrySize[i]);
        }
      } 
    }
  } 
  _FUNCTION_RETURN_(dst);         
}
#undef FUNCTION 


// free a pcmBuffer object an all associated data
pPcmBuffer pcmBufferFree( pPcmBuffer b, int noObjFree ) 
#define FUNCTION "pcmBufferFree"
{_FUNCTION_ENTER_
  int i;
  
  if (b == NULL) _FUNCTION_RETURN_( NULL );
  if ((b->data)&&(!(b->alienData))) free(b->data);
  if (b->uData) {
    for (i=0; i<nPCMUSERDATAENTRIES; i++) {
      if (b->uData->entry[i] != NULL) 
        free(b->uData->entry[i]);
    }
    free(b->uData);
  }
                

  if (!noObjFree) {
    free(b);
    _FUNCTION_RETURN_( NULL );
  } else {
    b->nBlocksAlloc = 0;
    b->nBlocks = 0;
    b->data = NULL;
    b->uData = NULL;
    _FUNCTION_RETURN_( b );
  }
}
#undef FUNCTION

/***** pcm ringbuffer ****/
pPcmRingBuffer pcmRingBuffer_create( pPcmRingBuffer obj, LONG_IDX size, int nChan, int sampleType, int memOrganisation, long int sampleRate )
#define FUNCTION "pcmRingBuffer_create"
{_FUNCTION_ENTER_
  if (obj == NULL) {
    obj = (pPcmRingBuffer) calloc(1,sizeof(sPcmRingBuffer));   
  } else {
    memzero(obj,sizeof(sPcmRingBuffer));       
  }
  if (obj != NULL) {
    if (obj->data) obj->data = pcmBufferFree(obj->data,0);
    obj->data = pcmBufferAllocate( obj->data, size, nChan, sampleType, memOrganisation, sampleRate );
    if (obj->data == NULL) { free(obj); _FUNCTION_RETURN_( NULL ); }
    obj->bsize = size;
    obj->rdidx = 0;
    obj->wridx = 0;        
  }
  _FUNCTION_RETURN_( obj );
}
#undef FUNCTION

// read a pcmBuffer from current location 
LONG_IDX pcmRingBuffer_read( pPcmRingBuffer obj, pPcmBuffer data )
#define FUNCTION "pcmRingBuffer_read"
{_FUNCTION_ENTER_
  LONG_IDX tocopy,nleft,rdidxactual,startidx;
  if (obj != NULL) {
    tocopy = data->nBlocksAlloc;
    rdidxactual = obj->rdidx % obj->bsize;
    startidx = obj->rdidx;
    if (obj->wridx - obj->rdidx < tocopy) {
      FEATUM_WARNING(5,"more data was requested than available in ringbuffer! truncating!");
      tocopy = obj->wridx - obj->rdidx;
    }
    // one step or two step copy?
    nleft = obj->bsize - rdidxactual;
    if (nleft >= tocopy) { // one step copy
      obj->rdidx += pcmBuffer_copy(obj->data, data, rdidxactual, rdidxactual+tocopy, 0);
    } else { // two step copy
      obj->rdidx += pcmBuffer_copy(obj->data, data, rdidxactual, rdidxactual+nleft, 0);
      obj->rdidx += pcmBuffer_copy(obj->data, data, 0, tocopy - data->nBlocks, data->nBlocks);
    }
    if (data != NULL) { data->timestampSamples = startidx; }
    _FUNCTION_RETURN_( tocopy );
//    pcmBuffer_copy(pPcmBuffer src, pPcmBuffer dst, int start, int end, int skip);
  }
  _FUNCTION_RETURN_( 0 );         
}
#undef FUNCTION

// read a pcmBuffer from current location , increasing rdidx only by stepsize
LONG_IDX pcmRingBuffer_readStep( pPcmRingBuffer obj, pPcmBuffer data, int stepsize )
#define FUNCTION "pcmRingBuffer_readStep"
{_FUNCTION_ENTER_
  LONG_IDX tocopy,nleft,rdidxactual;
  if (obj != NULL) {
    #ifdef COMPILE_CHECKS
    if (stepsize < 0) _FUNCTION_RETURN_( 0 );
    #endif
    tocopy = data->nBlocksAlloc;
    rdidxactual = obj->rdidx % obj->bsize;
    if (obj->wridx - obj->rdidx < tocopy) {
      FEATUM_WARNING(5,"more data was requested than available in ringbuffer! truncating!");
      tocopy = obj->wridx - obj->rdidx;
    }
    // one step or two step copy?
    nleft = obj->bsize - rdidxactual;
    if (nleft >= tocopy) { // one step copy
      pcmBuffer_copy(obj->data, data, rdidxactual, rdidxactual+tocopy, 0);
    } else { // two step copy
      pcmBuffer_copy(obj->data, data, rdidxactual, rdidxactual+nleft, 0);
      pcmBuffer_copy(obj->data, data, 0, tocopy - data->nBlocks, data->nBlocks);
    }
    if (data != NULL) { data->timestampSamples = obj->rdidx; }
    obj->rdidx += stepsize;
    _FUNCTION_RETURN_( tocopy );
//    pcmBuffer_copy(pPcmBuffer src, pPcmBuffer dst, int start, int end, int skip);
  }
  _FUNCTION_RETURN_( 0 );
}
#undef FUNCTION

// read at random location (must be present in ringbuffer, however!), do not update indicies:
LONG_IDX pcmRingBuffer_readRandom( pPcmRingBuffer obj, pPcmBuffer data, LONG_IDX index, int isRelative )
#define FUNCTION "pcmRingBuffer_readRandom"
{_FUNCTION_ENTER_
  LONG_IDX tocopy,nleft,rdidxactual;
  LONG_IDX rdidx;
  
  if (obj != NULL) {
    tocopy = data->nBlocksAlloc;
    if (isRelative) rdidx = obj->rdidx + index;
    else rdidx = index;
    // check if rdidx is within bounds:
    if ((obj->wridx - rdidx > obj->bsize) || (rdidx >= obj->wridx)) {
      FEATUM_FATAL_ERROR(0,"supplied ringbuffer read index is out of ringbuffer bounds!");
      _FUNCTION_RETURN_( 0 );
    }             
    rdidxactual = rdidx % obj->bsize;
    if (obj->wridx - rdidx < tocopy) {
      FEATUM_WARNING(4,"more data was requested than available in ringbuffer! truncating!");
      tocopy = obj->wridx - rdidx;
    }
    // one step or two step copy?
    nleft = obj->bsize - rdidxactual;
    if (nleft >= tocopy) { // one step copy
      pcmBuffer_copy(obj->data, data, rdidxactual, rdidxactual+tocopy, 0);
    } else { // two step copy
      pcmBuffer_copy(obj->data, data, rdidxactual, rdidxactual+nleft, 0);
      pcmBuffer_copy(obj->data, data, 0, tocopy - data->nBlocks, data->nBlocks);
    }
    if (data != NULL) { data->timestampSamples = rdidx; }
    
    _FUNCTION_RETURN_( tocopy );
//    pcmBuffer_copy(pPcmBuffer src, pPcmBuffer dst, int start, int end, int skip);
  }
  _FUNCTION_RETURN_( 0 );
}
#undef FUNCTION

// write a pcmBuffer at current write location:
LONG_IDX pcmRingBuffer_write( pPcmRingBuffer obj, pPcmBuffer data )
#define FUNCTION "pcmRingBuffer_write"
{_FUNCTION_ENTER_
  LONG_IDX tocopy,nleft,wridxactual;
  if (obj != NULL) {
    if (data == NULL) _FUNCTION_RETURN_( 0 );
    tocopy = data->nBlocks;
    if (obj->bsize - (obj->wridx - obj->rdidx) < tocopy) {
      FEATUM_WARNING(3,"more data to be written than free space in ringbuffer! truncating!");
      tocopy = obj->bsize - (obj->wridx - obj->rdidx);
    }
    wridxactual = obj->wridx % obj->bsize;
    // one step or two step copy?
    nleft = obj->bsize - wridxactual;
    if (nleft >= tocopy) { // one step copy
      obj->wridx += pcmBuffer_copy(data, obj->data, 0, tocopy, wridxactual);
      obj->data->nBlocks = obj->data->nBlocksAlloc;
    } else { // two step copy
      obj->wridx += pcmBuffer_copy(data, obj->data, 0, nleft, wridxactual);
      //obj->data->nBlocks = obj->data->nBlocksAlloc;
      obj->wridx += pcmBuffer_copy(data, obj->data, nleft, tocopy, 0);
      obj->data->nBlocks = obj->data->nBlocksAlloc;
    }
    _FUNCTION_RETURN_( tocopy );
  }     
  _FUNCTION_RETURN_( 0 );
}
#undef FUNCTION

// modulo divide given index and all internal indicies, to prevent absolute index overflows!
LONG_IDX pcmRingBuffer_modIndex( pPcmRingBuffer obj, LONG_IDX index )
#define FUNCTION "pcmRingBuffer_modIndex"
{_FUNCTION_ENTER_
  if (obj != NULL) {
     obj->rdidx %= obj->bsize;
     obj->wridx %= obj->bsize;
     _FUNCTION_RETURN_( index % obj->bsize );
  } 
  _FUNCTION_RETURN_( index );
}
#undef FUNCTION
         

// return number of data samples available in buffer:
#ifdef OPTIMIZE_SIZE
LONG_IDX pcmRingBuffer_toread( pPcmRingBuffer obj )
#else
inline LONG_IDX pcmRingBuffer_toread( pPcmRingBuffer obj )
#endif
#define FUNCTION "pcmRingBuffer_toread"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    _FUNCTION_RETURN_( obj->wridx - obj->rdidx );
  }
  _FUNCTION_RETURN_( 0 );
}
#undef FUNCTION

// return free space in buffer for writes:
#ifdef OPTIMIZE_SIZE
LONG_IDX pcmRingBuffer_freespace( pPcmRingBuffer obj )
#else
inline LONG_IDX pcmRingBuffer_freespace( pPcmRingBuffer obj )
#endif
#define FUNCTION "pcmRingBuffer_freespace"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    _FUNCTION_RETURN_( obj->bsize - (obj->wridx - obj->rdidx) );
  }
  _FUNCTION_RETURN_( 0 );
}
#undef FUNCTION


pPcmRingBuffer pcmRingBuffer_destroy( pPcmRingBuffer obj )
#define FUNCTION "pcmRingBuffer_destroy"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    pcmBufferFree(obj->data,0);
    free(obj);
  }
  _FUNCTION_RETURN_( NULL );
}
#undef FUNCTION

pPcmRingBuffer pcmRingBuffer_destroyData( pPcmRingBuffer obj )
#define FUNCTION "pcmRingBuffer_destroyData"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    obj->data = pcmBufferFree(obj->data,0); 
  }
  _FUNCTION_RETURN_( NULL );
}
#undef FUNCTION



/***** multi-read (MR) ringbuffer ******/
               
pPcmRingBufferMR pcmRingBufferMR_create( pPcmRingBufferMR obj, LONG_IDX size, int nMR, int nChan, int sampleType, int memOrganisation, long int sampleRate )
#define FUNCTION "pcmRingBufferMR_create"
{_FUNCTION_ENTER_
  if (obj == NULL) {
    obj = (pPcmRingBufferMR) calloc(1,sizeof(sPcmRingBufferMR));   
  } else {
    memzero(obj, sizeof(sPcmRingBufferMR));
  }
  if (obj != NULL) {
    if (obj->data) obj->data = pcmBufferFree(obj->data,0);
    if (obj->rdidx) free(obj->rdidx);
    obj->rdidx = (LONG_IDX *)calloc(1,sizeof(LONG_IDX)*nMR);
    obj->data = pcmBufferAllocate( obj->data, size, nChan, sampleType, memOrganisation, sampleRate );
    if (obj->data == NULL) { free(obj); _FUNCTION_RETURN_( NULL ); }
    obj->bsize = size;
    obj->wridx = 0;
    obj->nMR = nMR;
  }
  _FUNCTION_RETURN_( obj );
}
#undef FUNCTION

// read a pcmBuffer from current location 
LONG_IDX pcmRingBufferMR_read( pPcmRingBufferMR obj, int id, pPcmBuffer data )
#define FUNCTION "pcmRingBufferMR_read"
{_FUNCTION_ENTER_
  LONG_IDX tocopy,nleft,rdidxactual,startidx;
  if (obj != NULL) {
    #ifdef COMPILE_CHECKS
    if ((id < 0)||(id >= obj->nMR))  _FUNCTION_RETURN_( 0 );
    if (obj->data == NULL) _FUNCTION_RETURN_( 0 );
    if (obj->rdidx == NULL) _FUNCTION_RETURN_( 0 );
    #endif
    tocopy = data->nBlocksAlloc;
    startidx = obj->rdidx[id];
    rdidxactual = obj->rdidx[id] % obj->bsize;
    if (obj->wridx - obj->rdidx[id] < tocopy) {
      FEATUM_WARNING(5,"more data was requested than available in ringbuffer! truncating!");
      tocopy = obj->wridx - obj->rdidx[id];
    }
    // one step or two step copy?
    nleft = obj->bsize - rdidxactual;
    if (nleft >= tocopy) { // one step copy
      obj->rdidx[id] += pcmBuffer_copy(obj->data, data, rdidxactual, rdidxactual+tocopy, 0);
    } else { // two step copy
      obj->rdidx[id] += pcmBuffer_copy(obj->data, data, rdidxactual, rdidxactual+nleft, 0);
      obj->rdidx[id] += pcmBuffer_copy(obj->data, data, 0, tocopy - data->nBlocks, data->nBlocks);
    }
    if (data != NULL) { data->timestampSamples = startidx; }
    _FUNCTION_RETURN_( tocopy );
  }
  _FUNCTION_RETURN_( 0 );
}
#undef FUNCTION

// read a pcmBuffer from current location, increasing rdidx only by stepsize
LONG_IDX pcmRingBufferMR_readStep( pPcmRingBufferMR obj, int id, pPcmBuffer data, LONG_IDX stepsize )
#define FUNCTION "pcmRingBufferMR_readStep"
{_FUNCTION_ENTER_
  LONG_IDX tocopy,nleft,rdidxactual;
  if (obj != NULL) {
    #ifdef COMPILE_CHECKS
    if ((id < 0)||(id >= obj->nMR)) _FUNCTION_RETURN_( 0 );
    if (obj->data == NULL) _FUNCTION_RETURN_( 0 );
    if (obj->rdidx == NULL) _FUNCTION_RETURN_( 0 );
    if (stepsize < 0) _FUNCTION_RETURN_( 0 );
    #endif
    tocopy = data->nBlocksAlloc;
    rdidxactual = obj->rdidx[id] % obj->bsize;
    if (obj->wridx - obj->rdidx[id] < tocopy) {
      FEATUM_WARNING(5,"more data was requested than available in ringbuffer! truncating!");
      tocopy = obj->wridx - obj->rdidx[id];
    }
    // one step or two step copy?
    nleft = obj->bsize - rdidxactual;
    if (nleft >= tocopy) { // one step copy
       pcmBuffer_copy(obj->data, data, rdidxactual, rdidxactual+tocopy, 0);
    } else { // two step copy
      pcmBuffer_copy(obj->data, data, rdidxactual, rdidxactual+nleft, 0);
      pcmBuffer_copy(obj->data, data, 0, tocopy - data->nBlocks, data->nBlocks);
    }
    if (data != NULL) { data->timestampSamples = obj->rdidx[id]; }
    obj->rdidx[id] += stepsize;
    _FUNCTION_RETURN_( tocopy );
  }
  _FUNCTION_RETURN_( 0 );
}
#undef FUNCTION

// read at random location (must be present in ringbuffer, however!), do not update indicies:
LONG_IDX pcmRingBufferMR_readRandom( pPcmRingBufferMR obj, pPcmBuffer data, LONG_IDX index )
#define FUNCTION "pcmRingBufferMR_readRandom"
{_FUNCTION_ENTER_
  LONG_IDX tocopy,nleft,rdidxactual;
  LONG_IDX rdidx;
  
  if (obj != NULL) {
    tocopy = data->nBlocksAlloc;
    rdidx = index;
    // check if rdidx is within bounds:
    if ((obj->wridx - rdidx > obj->bsize) || (rdidx >= obj->wridx)) {
      FEATUM_FATAL_ERROR(0,"supplied ringbuffer read index is out of ringbuffer bounds!");
      _FUNCTION_RETURN_( 0 );
    }             
    rdidxactual = rdidx % obj->bsize;
    if (obj->wridx - rdidx < tocopy) {
      FEATUM_WARNING(4,"more data was requested than available in ringbuffer! truncating!");
      tocopy = obj->wridx - rdidx;
    }
    // one step or two step copy?
    nleft = obj->bsize - rdidxactual;
    if (nleft >= tocopy) { // one step copy
      pcmBuffer_copy(obj->data, data, rdidxactual, rdidxactual+tocopy, 0);
    } else { // two step copy
      pcmBuffer_copy(obj->data, data, rdidxactual, rdidxactual+nleft, 0);
      pcmBuffer_copy(obj->data, data, 0, tocopy - data->nBlocks, data->nBlocks);
    }
    if (data != NULL) { data->timestampSamples = index; }
    _FUNCTION_RETURN_( tocopy );
  }
  _FUNCTION_RETURN_( 0 ); 
}
#undef FUNCTION

#ifdef OPTIMIZE_SIZE
LONG_IDX pcmRingBufferMR_minRdIdx( pPcmRingBufferMR obj )
#else
inline LONG_IDX pcmRingBufferMR_minRdIdx( pPcmRingBufferMR obj )
#endif
#define FUNCTION "pcmRingBufferMR_minRdIdx"
{_FUNCTION_ENTER_
  _FUNCTION_RETURN_( min_lv(obj->rdidx, obj->nMR) );
}
#undef FUNCTION

#ifdef OPTIMIZE_SIZE
LONG_IDX pcmRingBufferMR_maxRdIdx( pPcmRingBufferMR obj )
#else
inline LONG_IDX pcmRingBufferMR_maxRdIdx( pPcmRingBufferMR obj )
#endif
#define FUNCTION "pcmRingBufferMR_maxRdIdx"
{_FUNCTION_ENTER_
  _FUNCTION_RETURN_( max_lv(obj->rdidx, obj->nMR) );
}
#undef FUNCTION

// write a pcmBuffer at current write location:
LONG_IDX pcmRingBufferMR_write( pPcmRingBufferMR obj, pPcmBuffer data )
#define FUNCTION "pcmRingBufferMR_write"
{_FUNCTION_ENTER_
  LONG_IDX tocopy,nleft,wridxactual,minrdidx;
  if (obj != NULL) {
    if (data == NULL) _FUNCTION_RETURN_( 0 );
    tocopy = data->nBlocks;
    minrdidx = pcmRingBufferMR_minRdIdx(obj);
    if (obj->bsize - (obj->wridx - minrdidx) < tocopy) {
      FEATUM_WARNING(3,"more data to be written than free space in ringbuffer! truncating!");
      tocopy = obj->bsize - (obj->wridx - minrdidx);
    }
    wridxactual = obj->wridx % obj->bsize;
    // one step or two step copy?
    nleft = obj->bsize - wridxactual;
    if (nleft >= tocopy) { // one step copy
      obj->wridx += pcmBuffer_copy(data, obj->data, 0, tocopy, wridxactual);
      obj->data->nBlocks = obj->data->nBlocksAlloc;
    } else { // two step copy
      obj->wridx += pcmBuffer_copy(data, obj->data, 0, nleft, wridxactual);
      //obj->data->nBlocks = obj->data->nBlocksAlloc;
      obj->wridx += pcmBuffer_copy(data, obj->data, nleft, tocopy, 0);
      obj->data->nBlocks = obj->data->nBlocksAlloc;
    }
    _FUNCTION_RETURN_( tocopy );
  }     
  _FUNCTION_RETURN_( 0 );
}
#undef FUNCTION

// modulo divide given index and all internal indicies, to prevent absolute index overflows!
LONG_IDX pcmRingBufferMR_modIndex( pPcmRingBufferMR obj, LONG_IDX index )
#define FUNCTION "pcmRingBufferMR_modIndex"
{_FUNCTION_ENTER_
  int i;
  if (obj != NULL) {
     #ifdef COMPILE_CHECKS
     if (obj->rdidx != NULL) {
       for (i=0; i<obj->nMR; i++) 
         obj->rdidx[i] %= obj->bsize;
     }
     #else
     for (i=0; i<obj->nMR; i++) 
       obj->rdidx[i] %= obj->bsize;
     #endif
     obj->wridx %= obj->bsize;
     _FUNCTION_RETURN_( index % obj->bsize );
  } 
  _FUNCTION_RETURN_( index );
}
#undef FUNCTION
         

// return number of data samples available in buffer:
#ifdef OPTIMIZE_SIZE
LONG_IDX pcmRingBufferMR_toread( pPcmRingBufferMR obj, int id )
#else
inline LONG_IDX pcmRingBufferMR_toread( pPcmRingBufferMR obj, int id )
#endif
#define FUNCTION "pcmRingBufferMR_toread"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    #ifdef COMPILE_CHECKS
    if (obj->rdidx == NULL) _FUNCTION_RETURN_( 0 );
    if ( (id<0)||(id >= obj->nMR)) _FUNCTION_RETURN_( 0 );
    #endif
    _FUNCTION_RETURN_( obj->wridx - obj->rdidx[id] );
  }
  _FUNCTION_RETURN_( 0 );
}
#undef FUNCTION

// return free space in buffer for writes:
#ifdef OPTIMIZE_SIZE
LONG_IDX pcmRingBufferMR_freespace( pPcmRingBufferMR obj )
#else
inline LONG_IDX pcmRingBufferMR_freespace( pPcmRingBufferMR obj )
#endif
#define FUNCTION "pcmRingBufferMR_freespace"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    #ifdef COMPILE_CHECKS
    if (obj->rdidx == NULL) _FUNCTION_RETURN_( 0 );
    #endif
    #ifdef DEBUG_SHOW_FUNCTION_ENTRY
    LONG_IDX ret = obj->bsize - (obj->wridx - pcmRingBufferMR_minRdIdx(obj));
    _FUNCTION_RETURN_( ret );
    #else
    return obj->bsize - (obj->wridx - pcmRingBufferMR_minRdIdx(obj));
    #endif
  }
  _FUNCTION_RETURN_( 0 );
}
#undef FUNCTION


pPcmRingBufferMR pcmRingBufferMR_destroy( pPcmRingBufferMR obj )
#define FUNCTION "pcmRingBufferMR_destroy"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    pcmBufferFree(obj->data,0);
    if (obj->rdidx != NULL) free(obj->rdidx);
    free(obj);
  }
  _FUNCTION_RETURN_( NULL );
}
#undef FUNCTION

pPcmRingBufferMR pcmRingBufferMR_destroyData( pPcmRingBufferMR obj )
#define FUNCTION "pcmRingBufferMR_destroyData"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    pcmBufferFree(obj->data,0); obj->data = NULL;
    if (obj->rdidx != NULL) { free(obj->rdidx); obj->rdidx = NULL; }
  }
  _FUNCTION_RETURN_( NULL );
}
#undef FUNCTION




/** internal (private) functions ***/


// ...


