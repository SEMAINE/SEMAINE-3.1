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
 
 /*  IMPLEMENTATION of the class <GENoutput>
     See the corresponding header file (.h) for documentation!  */

#define MODULE "GENoutput"
 
/*



*/

 
 // this file contains the function definitions and the private class data

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "featum_common.h"
#include "featum_util.h"       //( optional )
#include "GENoutput.h"

#undef FUNCTION     // use undef only if you define it below for every function


pGenOutput genOutput_create( pGenOutput obj, pFeatureMemory mem )
#define FUNCTION "genOutput_create"
{_FUNCTION_ENTER_

  if (mem == NULL) {
    FEATUM_ERROR(2,"parameter mem == NULL : featureMemory object is required!");
    _FUNCTION_RETURN_(0);
  }

  if (obj == NULL) {
    obj = (pGenOutput)calloc(1,sizeof(sGenOutput));
  } else {
    memzero(obj,sizeof(sGenOutput));
  }
  
  if (obj == NULL) {
    FEATUM_ERROR(0,"Error allocating memory!");
    _FUNCTION_RETURN_(NULL);        
  }

  obj->mem = mem;
  obj->vIdxSaved = -1;
  
  _FUNCTION_RETURN_(obj);
}
#undef FUNCTION 

int genOutput_setHeaderCallback( pGenOutput obj, int (*_writeHeader)( pGenOutput ) )
#define FUNCTION "genOutput_setHeaderCallback"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    obj->writeHeader = _writeHeader;
    // the warning displayed for the above line is OK and can be ignored...
    _FUNCTION_RETURN_(1);
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 


/*
// configure or change the current output file  (append=1 : append to file instead of overwriting)
int csvOutput_changeOutputFile( pCsvOutput obj, char *filename, int append )
#define FUNCTION "csvOutput_changeOutputFile"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    obj->filename = filename;
    if (obj->filedesc != NULL) {
      fclose(obj->filedesc); obj->filedesc = NULL;
    }

//    memzero(&(obj->header),sizeof(sCSVheader));
    if (!append) {
      obj->filedesc = fopen(filename,"w");
      if (obj->filedesc == NULL) {
        FEATUM_ERROR(0,"Error opening output feature file %s (append=%i)",filename,append);
        _FUNCTION_RETURN_(0);
      }   
      obj->fileIdx_frames = 0;
      // write header:
      //fwrite(&header, sizeof(sCSVheader), 1, obj->filedesc);
      // -> note here....  need more info
      
      _FUNCTION_RETURN_(1);
    } else {  // check if vectorsize is the same AND determine current file position
      int ret = csvOutput_reopenOutputFile(obj);
      _FUNCTION_RETURN_(ret);
    }
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 
*/


/*
selection file format: (ascii)

type       (str | idx)
[numStrs   if type=str]
idx OR string
idx OR string
...

*/
#define mAx(x,y) (((x)>(y))?(x):(y))
#define mIn(x,y) (((x)<(y))?(x):(y))

int genOutput_loadSelection( pGenOutput obj, const char *selFile )
#define FUNCTION "genOutput_loadSelection"
{_FUNCTION_ENTER_
  if ((obj != NULL)&&(selFile != NULL)) {
    FILE *f = fopen(selFile,"r");
    if (f== NULL) {
      FEATUM_ERROR(1,"error opening feature selection file %s for reading!",selFile);
      _FUNCTION_RETURN_(0);        
    }
    // read first line to determine filetype:
    char line[MAX_FTNAME_LENGTH+1];
    unsigned nStr=0;
    fgets( line, 5, f);
    line[3] = 0;
    if (!strcmp(line,"str")) { // string list
      obj->outputSelType = 2;
      FEATUM_DEBUG(5,"reading string list of features");
      fscanf( f, "%u\n", &nStr);
      obj->outputSelStr.n = nStr;
      obj->outputSelStr.names = (char **)calloc(1,sizeof(char *)*nStr);
      int i=0; line[0] = 0;
      while(fgets(line,MAX_FTNAME_LENGTH,f) != NULL) {
        if (strlen( line ) > 1) { 
          line[strlen( line ) - 1] = 0;
          obj->outputSelStr.names[i++] = strdup(line);
        }
      }
      FEATUM_DEBUG(5,"enabled %i features",i);
      fclose(f);
      _FUNCTION_RETURN_(1);      
    } else if (!strcmp(line,"idx")) { // index list
      obj->outputSelType = 1;
      FEATUM_DEBUG(5,"reading index list of features");
      unsigned idx=0; int i;
      // pass1: parse for max index
      while(fscanf(f,"%u\n",&idx) == 1)
        obj->outputSelIdx.nFull = mAx(obj->outputSelIdx.nFull, idx);
      FEATUM_DEBUG(5,"max index in selection file was found to be %i",obj->outputSelIdx.nFull);
      obj->outputSelIdx.nFull++;
      obj->outputSelIdx.enabled = (int *)calloc(1,sizeof(int)*obj->outputSelIdx.nFull);
      rewind( f );
      fgets(line, 5, f); // skip header line;
      // pass2: parse for max index
      i=0;
      while(fscanf(f,"%u\n",&idx) == 1) {
        obj->outputSelIdx.enabled[idx] = 1; // enable feature "idx"
        i++;
      }
      obj->outputSelIdx.nSel = i;
      FEATUM_DEBUG(5,"enabled %i features",i);
      fclose(f);
      _FUNCTION_RETURN_(1);
    } else { // bogus file...
      FEATUM_ERROR(1,"error parsing file %s. bogus header! expected 'str' or 'idx' at beginning. found '%s'.",line);
      fclose( f );
      _FUNCTION_RETURN_(0); 
    }
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION


// build next output vector at position vIdxSaved+1
int genOutput_buildVector( pGenOutput obj, pOutputVector vec )
#define FUNCTION "genOutput_buildVector"
{_FUNCTION_ENTER_
  if ((obj != NULL)&&(vec != NULL)) {
    vec->filled=0;
    if (obj->configured < 2) { // not yet configured...
      if (!genOutput_buildVectorDefinition(obj)) _FUNCTION_RETURN_(0);
    }
    // now everything is configured and the names are set up...

    // check if memory in output vector is allocated    
    if (vec->data == NULL) {
      vec->data = (FLOAT_TYPE_OUTPUT *)calloc(1, obj->outputDef.n * sizeof(FLOAT_TYPE_OUTPUT));
      if (vec->data  == NULL) {
        FEATUM_ERROR(0,"Error allocating memory!");
        _FUNCTION_RETURN_(0);
      }
      vec->n = obj->outputDef.n;
      vec->metadata = &(obj->outputDef);
      vec->vIdx = -1;
    } else {
      if (vec->n != obj->outputDef.n) {
        FEATUM_ERROR_FATAL(1,"output vector size changed! this is not yet supported..");
        _FUNCTION_RETURN_(0);             
      }
      memzero(vec->data, obj->outputDef.n * sizeof(FLOAT_TYPE_OUTPUT));
    }
    
    // save frame at index obj->idx_save;
    LONG_IDX frameIndex = obj->vIdxSaved+1;
    if (vec->vIdx == frameIndex) // frame was built already...
      _FUNCTION_RETURN_(1);
      //printf("building frame idx=%i\n",frameIndex);

    int index = 0; int rindex = 0;
    int i,j; int error = 0;
    
    for (i=0; i<obj->nEls; i++) {
      int level = obj->els[i].level;    
      int el = obj->els[i].el;
      FEATUM_DEBUG(10,"calling getEl: frameIndex = %i, el=%i, level=%i",frameIndex,el,level);
      pFtMemElement e = featureMemory_getElement( obj->mem, level, frameIndex, el );
      if ((e != NULL)&&(e->filled)) {
        
        int start=obj->els[i].nStart; int end;
        if (obj->els[i].nEnd < 0) end = e->nValues-1;
        else end = obj->els[i].nEnd;
        
        for(j=start; j<=end; j++) {
/***DBG***/ //printf("%s, %i : %f (%i) [l:%i el:%i]\n",obj->outputDef.names[index],index, e->values[j],j,level,el);
          if (obj->outputSelType > 0) {
            if (obj->outputSelIdx.enabled[index]) {
              vec->data[rindex] = (FLOAT_TYPE_OUTPUT)(e->values[j]);
              rindex++; 
            }
          } else vec->data[index] = (FLOAT_TYPE_OUTPUT)(e->values[j]);
          index++;      
        }
        
      } else {
        //memzero(vector, obj->outputVectorDimensionality * sizeof(float) );       
        FEATUM_WARNING(5,"e == NULL or not filled: no data to save  (e=%i)",e);
        error = 1;
      }
//      printf("rindex:%i [i=%i]\n",rindex,i);
      if (obj->outputSelType > 0) {
        if ((rindex > obj->outputDef.n)) { 
          i= obj->nEls+1; error=1;
          FEATUM_ERROR(2,"rindex (%i) exceeds outputVectorDimensionality (%i)!", rindex, obj->outputDef.n);
        }   // something went wrong...
                           
      } else {
        if ((index > obj->outputDef.n) || ((index == obj->outputDef.n) && (i<obj->nEls-1)) ) { 
          i= obj->nEls+1; error=1;
          FEATUM_ERROR(2,"index (%i) exceeds outputVectorDimensionality (%i)!", index, obj->outputDef.n);
        }   // something went wrong...
      }
    }

    if ((i == obj->nEls+1)||(error)) { _FUNCTION_RETURN_(0); }
    
    //    FEATUM_DEBUG(0,"obj->outputVectorDimensionality = %i",obj->outputVectorDimensionality);
    vec->vIdx = frameIndex;
    vec->timestamp = featureMemory_vIdxToTime(obj->mem, obj->els[0].level, frameIndex);
    if (vec->timestamp < 0) vec->timestamp = 0.0;
    FEATUM_DEBUG(5,"timestamp of vIdx %i  : %f",frameIndex,vec->timestamp);
    vec->duration = 0.0; // TODO....

    vec->filled = 1;
    _FUNCTION_RETURN_(1);
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION

    // genOutput_applySelection(obj);
int genOutput_applySelection( pGenOutput obj )
#define FUNCTION "genOutput_applySelection"
{_FUNCTION_ENTER_
  int i,j,N=0;
  
  if (obj != NULL) {
    if (obj->outputSelType == 1) {  // index list
      if (obj->outputDef.namesFull != NULL) {
         FEATUM_DEBUG(6,"applying index list feature selection...");
         obj->outputDef.names = (char **)calloc(1,sizeof(char *)*obj->outputSelIdx.nSel);
         for (i=0; i<obj->outputDef.nFull; i++) {
           if ((i<obj->outputSelIdx.nFull)&&(obj->outputSelIdx.enabled[i])) { 
             obj->outputDef.names[N++] = strdup(obj->outputDef.namesFull[i]);
           }
         }
         if (N==0) {
           FEATUM_ERROR(1,"No features enabled! Ignoring feature selection");
           obj->outputDef.n = obj->outputDef.nFull;        
           free(obj->outputDef.names);  
           obj->outputDef.names = obj->outputDef.namesFull;
           obj->outputSelIdx.nFull = 0;           
         } else {
           obj->outputDef.n = N;
           obj->outputVec.n = N;
         }
         FEATUM_DEBUG(7,"DONE : applying index list feature selection.");
      }     
      
      _FUNCTION_RETURN_(1);
    } else
    if (obj->outputSelType == 2) {  // string list
      if (obj->outputDef.namesFull != NULL) {
         FEATUM_DEBUG(6,"applying string list feature selection...");
         obj->outputDef.names = (char **)calloc(1,sizeof(char *)*obj->outputSelStr.n);
         if (obj->outputSelIdx.enabled) free(obj->outputSelIdx.enabled);
         obj->outputSelIdx.enabled = (int *)calloc(1,sizeof(int)*obj->outputDef.nFull);
         for (i=0; i<  obj->outputDef.nFull; i++) {
             for (j=0; j<obj->outputSelStr.n; j++) {
               if (!strcmp(obj->outputDef.namesFull[i], obj->outputSelStr.names[j])) {
                 obj->outputDef.names[N++] = strdup(obj->outputDef.namesFull[i]);
                 obj->outputSelIdx.enabled[i] = 1;
                 break;                                     
               }
             }             
         }
         if (N==0) {
           FEATUM_ERROR(1,"No matching features! Ignoring feature selection");
           obj->outputDef.n = obj->outputDef.nFull;        
           free(obj->outputDef.names);  
           obj->outputDef.names = obj->outputDef.namesFull;
           obj->outputSelIdx.nFull = 0;
         } else {
           obj->outputDef.n = N;
           obj->outputVec.n = N;
           // we must now build the index mapping:
           obj->outputSelIdx.nFull = obj->outputDef.nFull;
           obj->outputSelIdx.nSel = N;
           
         }
         FEATUM_DEBUG(7,"DONE : applying string list feature selection.");
      }     
      _FUNCTION_RETURN_(1);
    }
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION


// build output vector definition (names, dimensionality, etc.)
int genOutput_buildVectorDefinition( pGenOutput obj )
#define FUNCTION "genOutput_buildVectorDefinition"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    if (obj->configured < 1) {
      FEATUM_WARNING(0,"output not yet configured, call _configure(...) method first!");
      _FUNCTION_RETURN_(0);                                    
    }

    // a) compute dimensionality, check if possible for all elements, and updata nValues in ElIDs structs

    int i;
    long dim = 0;
    float frameStep = 0.0;
    FEATUM_DEBUG(8,"nEls = %i",obj->nEls);

    for (i=0; i<obj->nEls; i++) {
      int level = obj->els[i].level;    
      int el = obj->els[i].el;
      if (obj->mem->level != NULL) {
        pFtMemElement elem = featureMemory_getElement(obj->mem, level, obj->vIdxSaved+1, el); //obj->mem->level[level].curIdx

        if (obj->els[i].enabled) {
        if ((elem == NULL)||(elem->filled == 0)) {
            // computing of dimensionality not yet possible... (no element available on this level)
            obj->vIdxSaved = -1;
            FEATUM_WARNING(3,"elem == NULL or filled==0, idx_save: %i, level %i, el %i, elem=%i",obj->vIdxSaved,level,el,(long)elem);
            _FUNCTION_RETURN_(0); 
        }

        if (obj->vIdxSaved+1 > obj->mem->level[level].allFilledIdx) 
          obj->vIdxSaved = obj->mem->level[level].allFilledIdx-1;  /// ???
        
        if (i>0) { 
          if (frameStep != obj->mem->level[level].T) {
            FEATUM_ERROR(0,"combining asynchronus levels (different frameSteps) in one output is not yet supported!");
            _FUNCTION_RETURN_(0); 
          }
        } else { frameStep = obj->mem->level[level].T; }
        FEATUM_DEBUG(3,"frameStep = %f",frameStep);
        
        obj->els[i].nValues = elem->nValues;
        if (obj->els[i].nEnd < 0) {  // -1 : full length
            FEATUM_DEBUG(10,"obj->mem->level[level].ft[ci][el]->nValues = %i",elem->nValues);
            obj->els[i].nEnd = elem->nValues-1;
            if (obj->els[i].nEnd < obj->els[i].nStart) {
              obj->els[i].enabled = 0;                     
            }
        } else {
            if (obj->els[i].nEnd >= elem->nValues) {
              obj->els[i].nEnd = elem->nValues-1; // limit length to nValues!
              if (obj->els[i].nEnd < obj->els[i].nStart) {
                obj->els[i].enabled = 0;                     
              }
            }
        }
        

        dim += obj->els[i].nEnd - obj->els[i].nStart + 1;
        } // end if(enabled...)

        
      } // end if (... != NULL) 
    } // end for(...)
    
    obj->samplePeriod = frameStep;
    obj->outputDef.n = dim;
    obj->outputDef.nFull = dim;
    obj->outputVec.n = dim;
    
    FEATUM_DEBUG(3,"frameStep set to %f, dim  = %i",frameStep,dim);
 
   // b) allocate char* array for n elements
   //     and copy names (getFeatureName)  into it
    obj->outputDef.names = (char **)calloc(1,sizeof(char*) * dim);
    int N=0; int n;
    for (i=0; i<obj->nEls; i++) {
      int level = obj->els[i].level;    
      int el = obj->els[i].el;
      
      for (n=obj->els[i].nStart; n<=obj->els[i].nEnd; n++) {
        char *tmp = featureMemory_getFeatureName( obj->mem, level, el, n );
        int len = strlen(tmp)+2;
        if (len > obj->outputDef.maxNameLength) obj->outputDef.maxNameLength = len;
        if (N<dim) obj->outputDef.names[N] = strdup(tmp);
        N++; if (N>dim) { FEATUM_ERROR_FATAL(1,"N (%i) > dim (%i) !",N,dim); }
      }
    } // end for ( i= ...) 
    obj->outputDef.namesFull = obj->outputDef.names;  
    
    obj->configured = 2;
    
    // call callback to build and write custom stream/file header
     //DONE: // TODO::: do this somewhere else... or load feature selection right here....
             // even better:: load it somewhere, but if it was loaded, apply it right here!! :)
    genOutput_applySelection(obj);
    
    if (obj->writeHeader != NULL) {
      (obj->writeHeader)(obj);
    }
    _FUNCTION_RETURN_(1);
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION


//configure the elements and levels to be saved (i.e. configure output feature vector)
int genOutput_configure( pGenOutput obj, pElID els, int nEls )
#define FUNCTION "genOutput_configure"
{_FUNCTION_ENTER_
// TODO: 
  if (obj != NULL) {
    #ifdef COMPILE_CHECKS
    if (els == NULL) {
      FEATUM_WARNING(0,"function called with parameter els == NULL : this makes no sense!");
      _FUNCTION_RETURN_(0);
    }
    if (nEls <= 0) {
      FEATUM_WARNING(0,"parameter nEls <= 0 : must have at least one source element!");
      _FUNCTION_RETURN_(0);
    }
    #endif
    if (obj->els != NULL) { free(obj->els); obj->els = NULL; }
    obj->els = (pElID) malloc(sizeof(sElID)*nEls);
    if (obj->els == NULL) {
      FEATUM_ERROR(0,"Error allocating memory");
      _FUNCTION_RETURN_(0);
    }    
    memcpy(obj->els, els, nEls * sizeof(sElID) );
    obj->nEls = nEls;
    obj->vIdxSaved = -1;
    obj->configured = 1;
    
    int i;
    for (i=0; i<nEls; i++) {
      obj->els[i].enabled = 1; // TODO: ???
      if (obj->els[i].nEnd != -1) {
        if (obj->els[i].n > 0) obj->els[i].nEnd = obj->els[i].nStart + obj->els[i].n;
        if (obj->els[i].nStart > obj->els[i].nEnd) {
          FEATUM_WARNING(4,"nStart (%i) > nEnd (%i) in sElID # %i ",obj->els[i].nStart,obj->els[i].nEnd,i);
          obj->els[i].enabled = 0; obj->els[i].nStart = 0; obj->els[i].nEnd = 0;
        }
      }
    }
    
    _FUNCTION_RETURN_(1);
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 




// save all frames up to last available
// returns 1 if at least 1 frame was saved successfully and no erros occured
// 0 is returned upon errors (even if frames were saved...) and if no frame to save
// was available
int genOutput_saveAll( pGenOutput obj, int (*_writeFrame)( pGenOutput, pOutputVector ) )
#define FUNCTION "genOutput_saveAll"
{_FUNCTION_ENTER_
  if (obj != NULL) {

    int status = 0;  // will be set to 1 if at least one frame was saved successfully
    int ret=1; 
    while (ret) {
      FEATUM_DEBUG(7,"attempting to save frame with index obj->vIdxSaved+1 = %i (obj->writePtr_vec=%i)",obj->vIdxSaved+1,obj->writePtr_vec);

      int ret = genOutput_buildVector( obj, &(obj->outputVec) );
      if (ret == 0) {
        FEATUM_WARNING(9,"this frame is not available"); break;
      }
      FEATUM_DEBUG(7,"outputVectorDimensionality = %i",obj->outputDef.n);
    
      if (obj->outputDef.n <= 0) { ret=0; status=0; break; } // no frame to save...?
      
      // put code to save frame here....
      if (_writeFrame != NULL) {
        int r = _writeFrame(obj, &(obj->outputVec));
    
        if (r) {
          FEATUM_DEBUG(7,"the frame was saved");
          obj->vIdxSaved++;
          obj->writePtr_vec++;
          status=1;
        } else {
          FEATUM_ERROR(4,"error saving frame %i",obj->vIdxSaved+1);
          ret=0; status=0; break;
        }
      }

    } // end (while...)

    _FUNCTION_RETURN_(status);
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 


// save next frame
int genOutput_saveFrame( pGenOutput obj, int (*_writeFrame)( pGenOutput, pOutputVector ) )
#define FUNCTION "genOutput_saveFrame"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    FEATUM_DEBUG(7,"attempting to save frame with index obj->vIdxSaved = %i (obj->writePtr_vec=%i)",obj->vIdxSaved,obj->writePtr_vec);

    int ret = genOutput_buildVector( obj, &(obj->outputVec) );

    if (ret == 0) {
        FEATUM_ERROR(4,"Cannot compute dimensionality of output vector (yet)!"); 
        _FUNCTION_RETURN_(0);        
    }
    FEATUM_DEBUG(7,"outputVectorDimensionality = %i",obj->outputDef.n);
    
    if (obj->outputDef.n <= 0) { _FUNCTION_RETURN_(0); } // no frame to save...?
    
    // put code to save frame here....
    if (_writeFrame != NULL) {
      int ret = _writeFrame(obj, &(obj->outputVec) );
    
      if (ret) {
        FEATUM_DEBUG(7,"the frame was saved");
        obj->vIdxSaved++;
        obj->writePtr_vec++;
        _FUNCTION_RETURN_(1);
      } else {
        FEATUM_ERROR(4,"error saving frame %i",obj->vIdxSaved+1);
      }
    }

  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 

int genOutput_setIdxToLastFrame( pGenOutput obj ) 
#define FUNCTION "genOutput_setIdxToLastFrame"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    LONG_IDX af = -1;
    LONG_IDX afic;
    int i;
    
    // find globally minimal allFilledIdx
    for (i=0; i<obj->nEls; i++) {
      int level = obj->els[i].level;    
      afic = featureMemory_getSpecialIndex(obj->mem, level, VIDX_CURRENT_ALLFILLED);
      if ((afic < af)||(af==-1)) af = afic;
    }
    obj->vIdxSaved = af-1;
    if (obj->vIdxSaved < -1) obj->vIdxSaved = -1;
    
    _FUNCTION_RETURN_(1);
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 



// save next frame
pOutputVector genOutput_getFrame( pGenOutput obj )
#define FUNCTION "genOutput_getFrame"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    FEATUM_DEBUG(7,"attempting to get frame with index obj->vIdxSaved = %i (obj->writePtr_vec=%i)",obj->vIdxSaved,obj->writePtr_vec);

    int ret = genOutput_buildVector( obj, &(obj->outputVec) );

    if (ret == 0) {
        FEATUM_ERROR(4,"Cannot compute dimensionality of output vector (yet)!"); 
        _FUNCTION_RETURN_(NULL);        
    }
    FEATUM_DEBUG(7,"outputVectorDimensionality = %i",obj->outputDef.n);
    
    if (obj->outputDef.n <= 0) { _FUNCTION_RETURN_(NULL); } // no frame to save...?
    
    // put code to save frame here....
    obj->vIdxSaved++;
    _FUNCTION_RETURN_( &(obj->outputVec) );


  }
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION 

// destructor for generic output class
// call this from the derived output class's destructor after freeing data from 
// the derived class...
pGenOutput genOutput_destroyData( pGenOutput obj )
#define FUNCTION "genOutput_destroyData"
{_FUNCTION_ENTER_

  if (obj != NULL) {
     
    if (obj->streamName != NULL) { free(obj->streamName); obj->streamName = NULL; }
    if (obj->headerString != NULL) { free(obj->headerString); obj->headerString = NULL; }
    if (obj->header != NULL) { free(obj->header); obj->header = NULL; }
    if (obj->els != NULL) { free(obj->els); obj->els = NULL; }
    int n;
    if (obj->outputDef.names == obj->outputDef.namesFull) obj->outputDef.namesFull = NULL;
    if (obj->outputDef.names != NULL) {
      for (n=0; n<obj->outputDef.n; n++) {
        if (obj->outputDef.names[n] != NULL) free(obj->outputDef.names[n]);
      }
      free(obj->outputDef.names); obj->outputDef.names = NULL;
    }
    if (obj->outputDef.namesFull != NULL) {
      for (n=0; n<obj->outputDef.nFull; n++) {
        if (obj->outputDef.namesFull[n] != NULL) free(obj->outputDef.namesFull[n]);
      }
      free(obj->outputDef.namesFull); obj->outputDef.namesFull = NULL;
    }
    if (obj->outputSelIdx.enabled != NULL) { 
      free(obj->outputSelIdx.enabled); obj->outputSelIdx.enabled = NULL;
      obj->outputSelIdx.nSel = 0;
      obj->outputSelIdx.nFull = 0;
    }
    if (obj->outputSelStr.names != NULL) {
      for(n=0; n<obj->outputSelStr.n; n++) {
        if (obj->outputSelStr.names[n] != NULL) free(  obj->outputSelStr.names[n]);
      }                             
      free(  obj->outputSelStr.names );
      obj->outputSelStr.names = NULL; obj->outputSelStr.n = 0;
    }
    if (obj->outputVec.data != NULL) { 
      free(obj->outputVec.data); obj->outputVec.data = NULL; 
    }
    if (obj->filedesc != NULL) { fclose(obj->filedesc); obj->filedesc = NULL; }
  }
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION 

pGenOutput genOutput_destroy( pGenOutput obj )
#define FUNCTION "genOutput_destroy"
{_FUNCTION_ENTER_

  genOutput_destroyData( obj );
  if (obj != NULL) {
    free(obj);
  }
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION 



/**************** helper functions ************************/

inline sElID outputObject( int level, int el, int nStart, int nEnd, int n, int enabled )
{
  sElID ret;
  ret.level = level;
  ret.el = el;
  ret.nStart = nStart;
  ret.nEnd = nEnd;
  ret.n = n;
  ret.enabled = enabled;  
  return ret;
}

inline sElID outputObjectFull( int level, int el )
{
  sElID ret;
  ret.level = level;
  ret.el = el;
  ret.nStart = 0;
  ret.nEnd = -1;
  ret.n = 0;
  ret.enabled = 1;  
  return ret;
}
