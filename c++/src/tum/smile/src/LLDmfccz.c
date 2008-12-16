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
 
 /*  On-line Cepstral Mean Subtraction of MFCC  */

#define MODULE "LLDmfccz"
#include "featum_common.h"

// this file contains the function definitions and the private class data

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "featum_util.h"       //( optional )
#include "feature-memory.h"
#include "LLDs.h"

#undef FUNCTION     // use undef only if you define it below for every function


// default constructor 
pLLDmfccz LLDmfccz_create( pLLDmfccz obj )
#define FUNCTION "LLDmfccz_create"
{_FUNCTION_ENTER_
  if (obj == NULL) {
    obj = (pLLDmfccz)calloc(1,sizeof(sLLDmfccz));
    if (obj == NULL) {
      FEATUM_ERROR(0,"Error allocating memory!"); 
    }        
    obj->nMfcc = -1;  // not set yet...
    obj->nVec = 0;
    obj->means = NULL;
    obj->means0 = NULL;
    obj->alpha = DEFAULT_alpha;  
  }
  _FUNCTION_RETURN_( obj );
}
#undef FUNCTION 


int LLDmfccz_configure( pLLDmfccz obj, FLOAT_TYPE alpha, const char * initFile )
#define FUNCTION "LLDmfccz_configure"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    if (alpha > 1.0) alpha = 1.0;
    if (alpha < 0.001) alpha = 0.001;
    obj->alpha = alpha;
    if (initFile != NULL) {
	  // open cepstral mean initialisation file and load values:
	  FILE *f = fopen(initFile,"r");
	  if (f!=NULL) {
  	    int i,n; float v;
        fscanf(f,"<MEAN> %i\n",&n);
	    obj->nMeans0 = n;
	    obj->means0 = (FLOAT_TYPE *)malloc(sizeof(FLOAT_TYPE)*n);
        //printf("nMeans to load: %i\n",n);
	    for (i=1; i<n; i++) {
          if (fscanf(f," %f",&v) > 0) {
            obj->means0[i] = v;
            //printf("v(%i): %f\n",i,v);
          } else {
            obj->means0[i] = 0.0;
            printf("parse error during load of cepstral mean file: %s\n",initFile);
	      }
        }
	 if (fscanf(f," %f",&v) > 0) {
            obj->means0[0] = v;
            //printf("v(%i): %f\n",i,v);
          } else {
            obj->means0[0] = 0.0;
            printf("parse error during load of cepstral mean file: %s\n",initFile);
	      }
	    fclose(f);
      } else {
        FEATUM_ERROR(1,"cannot open CMN initialisation file %s\n",initFile);
      }
    }
    _FUNCTION_RETURN_( 1 );
  }
  _FUNCTION_RETURN_( 0 );
}
#undef FUNCTION 

// flushToMem: copy final data from ftex to feature memory element array
int LLDmfccz_flushToFtMem( int level, pFeatureMemory mem, pLLDex lldex )
#define FUNCTION "LLDmfccz_flushToFtMem"
{_FUNCTION_ENTER_
 
  if (lldex == NULL)  _FUNCTION_RETURN_(0);
  #ifdef COMPILE_CHECKS_LOOPS
  if (level >= lldex->nIDs) _FUNCTION_RETURN_(0);
  if (lldex->current[level] == NULL) _FUNCTION_RETURN_(0);
  #endif
  if (lldex->current[level]->mfccz == NULL) _FUNCTION_RETURN_(0);  // extractor failed, do not flush, because there is no data
  if (lldex->current[level]->mfccz->flush_status == LLDEX_FLUSHED) _FUNCTION_RETURN_(2);
  
  FLOAT_TYPE_FTMEM *values 
    = featureMemory_getValuesForWrite_seq( mem, level, ft_lld_mfccz, 
                                           lldex->current[level]->mfccz->nMFCC, 0, 
                                           lldex->current[level]->timestamp );

  if (values != NULL) {
    int i;

	// flush the normalised mfcc to the feature memory
    FLOAT_TYPE *mfccarr = lldex->current[level]->mfccz->mfccz;
    for (i=0; i< lldex->current[level]->mfccz->nMFCC; i++) {
      values[i] = (FLOAT_TYPE_FTMEM)(mfccarr[i]);
    }   
    lldex->current[level]->mfccz->flush_status = LLDEX_FLUSHED;

    _FUNCTION_RETURN_(1);
  }

  _FUNCTION_RETURN_(0);
}
#undef FUNCTION


// extractor:  do the main extraction , this function is called once per frame
// if data from multiple past frames is required, it is accessible via the lldex object
int LLDmfccz_extractor( pLLDmfccz obj, pLLDex lldex, int level )
#define FUNCTION "LLDmfccz_extractor"
{_FUNCTION_ENTER_
  int i,m;
  if (lldex != NULL) {
    #ifdef COMPILE_CHECKS_LOOPS
    if (level >= lldex->nIDs) _FUNCTION_RETURN_(0);
    if (level < 0) _FUNCTION_RETURN_(0);
    if (lldex->current[level] == NULL) _FUNCTION_RETURN_(0);
    #endif

    pLLDexMfcc currentMfcc = lldex->current[level]->mfcc;
    if (currentMfcc == NULL) {
      FEATUM_ERROR(9,"mfcc data in lldex struct is NULL, but required for mfccz!");
      _FUNCTION_RETURN_(0);
    }
    int N = currentMfcc->nMFCC;

    if (obj->nMfcc == -1) {  // initialize...
      obj->nMfcc = N;
      obj->means = (FLOAT_TYPE *)calloc(1,sizeof(FLOAT_TYPE)*N);
      // load initial mean values ...
      if ( (obj->nMeans0 > 0) && (obj->means0 != NULL) ) {
        if (obj->nMeans0 < N) {
		  for (i=0; i<obj->nMeans0; i++) { obj->means[i] = obj->means0[i]; }
		  for ( ; i<N; i++) { obj->means[i] = 0.0; }
        } else {
		  for (i=0; i<obj->nMeans0; i++) { obj->means[i] = obj->means0[i]; 
		  //printf("(%i), means=%f, means0=%f\n",i,obj->means[i],obj->means0[i]);
		  }
	    }
	  }
    }
    
    pLLDexMfccz currentMfccz = lldex->current[level]->mfccz;
    if (currentMfccz == NULL) {
       LLDmfccz_createLLDex( lldex->current[level] );
       currentMfccz = lldex->current[level]->mfccz;
    }
    
    if (currentMfccz->nMFCC == 0) {
       currentMfccz->nMFCC = N;
       currentMfccz->mfccz = (FLOAT_TYPE *) malloc(sizeof(FLOAT_TYPE)*N);
    }
    
    // do the CMN, based on the current mean cepstral vector
	int i;
	FLOAT_TYPE *out = currentMfccz->mfccz;
	FLOAT_TYPE *in  = currentMfcc->mfcc;
	for (i=0; i<N; i++) {
	  out[i] = 	in[i] - obj->means[i];
//	  if (obj->nVec == 0) 
  	    //printf("(%i), means=%f, in=%f out=%f\n",i,obj->means[i],in[i],out[i]);
	}


	// update the CMN:
    if (obj->nVec > 12) {
      FLOAT_TYPE a = obj->alpha;
	  for (i=0; i<N; i++) {
        obj->means[i] = a * (obj->means[i] - in[i]) + in[i];
      }	   		  
    } else {
      // count number of vector processed (from beginning)
      obj->nVec++;    
    }

    _FUNCTION_RETURN_(1);
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION


// create LLDex object for this extractor
int LLDmfccz_createLLDex( pLLDexElement obj )
#define FUNCTION "LLDmfccz_createLLDex"
{_FUNCTION_ENTER_
  pLLDexMfccz p = NULL;
  int ret = 0;
  
  if (obj != NULL) {
    if (obj->mfccz != NULL) LLDmfccz_freeLLDex(obj);          
    obj->mfccz = NULL;
    p = (pLLDexMfccz) calloc(1,sizeof(sLLDexMfccz));
    if (p != NULL) {
      p->freeLLDex = &LLDmfccz_freeLLDex;
      ret = 1;
    }
    obj->mfccz = p;
  }
  _FUNCTION_RETURN_(ret);
}
#undef FUNCTION


// free the lldex object l and return NULL
int LLDmfccz_freeLLDex( pLLDexElement obj )
#define FUNCTION "LLDmfccz_freeLLDex"
{_FUNCTION_ENTER_

  if (obj != NULL) {
    if (obj->mfccz != NULL) {
      if (obj->mfccz->mfccz != NULL) free(obj->mfccz->mfccz);
      free(obj->mfccz); 
      obj->mfccz = NULL;
      _FUNCTION_RETURN_(1);
    }   
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION


// destroy the LLDmfccz feature extractor object
pLLDmfccz LLDmfccz_destroy( pLLDmfccz obj )
#define FUNCTION "LLDmfccz_destroy"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    if (obj->means != NULL) { free(obj->means); }
    if (obj->means0 != NULL) { free(obj->means0); }
    free(obj);
  }
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION



