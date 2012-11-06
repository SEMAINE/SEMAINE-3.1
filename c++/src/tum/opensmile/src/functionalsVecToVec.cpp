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

statistical functionals  vectorProcessor ... TODOOOOOOOOoo
 
*/



#include <math.h>
#include <dataMemory.hpp>
#include <componentManager.hpp>
#include <functionalsVecToVec.hpp>
#include <functionals.hpp>

#define MODULE "cFunctionalsVecToVec"

/*Library:
sComponentInfo * registerMe(cConfigManager *_confman) {
  cDataProcessor::registerComponent(_confman);
}
*/

#define N_BLOCK_ALLOC 50

SMILECOMPONENT_STATICS(cFunctionalsVecToVec)
int cFunctionalsVecToVec::rAcounter=0;

SMILECOMPONENT_REGCOMP(cFunctionalsVecToVec)
//sComponentInfo * cFunctionalsVecToVec::registerComponent(cConfigManager *_confman)
{
/*
  if (_confman == NULL) return NULL;
  int rA = 0;
  sconfman = _confman;
*/
  SMILECOMPONENT_REGCOMP_INIT
  
  scname = COMPONENT_NAME_CFUNCTIONALSVECTOVEC;
  sdescription = COMPONENT_DESCRIPTION_CFUNCTIONALSVECTOVEC;

  // we inherit cVectorProcessor configType and extend it:
//use _compman to find cFunctionalXXXXX component types...
// however, we return rA=1 twice, to allow cFunctionalXXXX to register!
  // add corresponding config sub-types to our config type here....
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cVectorProcessor")

/*
  ConfigType *ct = new ConfigType( *(sconfman->getTypeObj("cWinToVecProcessor")) , scname );
  if (ct == NULL) {
    SMILE_WRN(4,"cWinToVecProcessor config Type not found!");
    rA=1;
  }
*/

  char *funclist=NULL;

  SMILECOMPONENT_IFNOTREGAGAIN_BEGIN
//  if (rA!=1) {

    if (rAcounter < 2) {
      rA=1;
      rAcounter++;
      SMILECOMPONENT_MAKEINFO(cFunctionalsVecToVec);
    }

    if (_compman != NULL) {
  
      int nTp = _compman->getNtypes();
      int i,j=0;
      for (i=0; i<nTp; i++) {
        const char * tp = _compman->getComponentType(i,1);
        if (tp!=NULL) {
          if (!strncmp(tp,"cFunctional",11)&&strncmp(tp,COMPONENT_NAME_CFUNCTIONALS,COMPONENT_NAME_CFUNCTIONALS_LENGTH)) {
             // find beginning "cFunctional" but not our own type (cFunctinals)
            const char *fn = tp+11;
            j++;
            if (funclist != NULL) {
              char * tmp = funclist;
              funclist = myvprint("%s      %i.\t%s \t\t%s\n",funclist,j,fn,_compman->getComponentDescr(i));
              free(tmp);
            } else {
              funclist = myvprint("     (#) \t(name)    \t\t(description)\n      %i.\t%s \t\t%s\n",j,fn,_compman->getComponentDescr(i));
            }
            // add config type
            char *x = myvprint("functional sub-config of type %s",tp);
            ct->setField(fn,x,sconfman->getTypeObj(tp),NO_ARRAY,DONT_FREE);
            free(x);
          }
        }
      }
    
    } else { // cannot proceed without component manager!
      rA=1;
      SMILECOMPONENT_MAKEINFO(cFunctionalsVecToVec);
    }

    char *x = myvprint("Array that lists the enabled functionals\n    The following functionals are available (Names are CASE-SENSITIVE!):\n%s",funclist);
    ct->setField("functionalsEnabled",x,(const char*)NULL, ARRAY_TYPE);
    free(x);
    free(funclist);

  SMILECOMPONENT_IFNOTREGAGAIN_END

  SMILECOMPONENT_MAKEINFO(cFunctionalsVecToVec);
}

SMILECOMPONENT_CREATE(cFunctionalsVecToVec)

//-----

cFunctionalsVecToVec::cFunctionalsVecToVec(const char *_name) :
  cVectorProcessor(_name),
  functTp(NULL),
  functI(NULL),
  functN(NULL),
  functTpI(NULL),
  functObj(NULL),
  nFunctTpAlloc(0),
  nFunctTp(0)
{

}

int cFunctionalsVecToVec::myConfigureInstance()
{
  int i,j;

  cComponentManager *_compman = getCompMan();
  if (_compman != NULL) {
    int nTp = _compman->getNtypes();
    nFunctTp = 0;
    for (i=0; i<nTp; i++) {
      const char * tp = _compman->getComponentType(i,1);
      if (tp!=NULL) {
        if (!strncmp(tp,"cFunctional",11)&&strcmp(tp,COMPONENT_NAME_CFUNCTIONALSVECTOVEC)) {
           // find beginning "cFunctional" but not our own type (cFunctinals)
          const char *fn = tp+11;
          if (nFunctTpAlloc == nFunctTp) { // realloc:
            functTp = (char **)crealloc( functTp, sizeof(char*)*(nFunctTpAlloc+N_BLOCK_ALLOC), nFunctTpAlloc );
            functTpI = (int *)crealloc( functTpI, sizeof(int)*(nFunctTpAlloc+N_BLOCK_ALLOC), nFunctTpAlloc );
            functI = (int *)crealloc( functI, sizeof(int)*(nFunctTpAlloc+N_BLOCK_ALLOC), nFunctTpAlloc );
            functN = (int *)crealloc( functN, sizeof(int)*(nFunctTpAlloc+N_BLOCK_ALLOC), nFunctTpAlloc );
            functObj = (cFunctionalComponent **)crealloc( functObj, sizeof(cFunctionalComponent *)*(nFunctTpAlloc+N_BLOCK_ALLOC), nFunctTpAlloc );
            nFunctTpAlloc += N_BLOCK_ALLOC;
          }
          functTp[nFunctTp] = strdup(fn);
          functTpI[nFunctTp] = i;
          nFunctTp++;
        }
      }
    }
  }
  SMILE_DBG(2,"(inst '%s') found %i cFunctionalXXXX component types.",getInstName(),nFunctTp);

  // fetch enabled functionals list
  nFunctionalsEnabled = getArraySize("functionalsEnabled");
  nFunctValues = 0;
  requireSorted = 0;
  for (i=0; i<nFunctionalsEnabled; i++) {
    const char *fname = getStr_f(myvprint("functionalsEnabled[%i]",i));
    char *tpname = myvprint("cFunctional%s",fname);
    for (j=0; j<nFunctTp; j++) {
      if (!strcmp(functTp[j],fname)) {
        functI[i] = j;
        break;
      }
    }
// TODO: find duplicates in functionalsEnabled Array!!!

    if (j<nFunctTp) {
      // and create corresponding component instances...
        SMILE_DBG(3,"(inst '%s') creating Functional object 'cFunctional%s'.",fname);
        char *_tmp = myvprint("%s.%s",getInstName(),fname);
        cFunctionalComponent * tmp = (cFunctionalComponent *)(_compman->createComponent)(_tmp,tpname);
        free(_tmp);
        if (tmp==NULL) OUT_OF_MEMORY;
        tmp->setComponentEnvironment(_compman, -1, this);
        functN[i] = tmp->getNoutputValues();
        requireSorted += tmp->getRequireSorted();
        nFunctValues += functN[i];
        functObj[i] = tmp;
        //functTp[i]  = strdup(fname);
    } else {
      SMILE_ERR(1,"(inst '%s') Functional object '%s' specified in 'functionalsEnabled' array, however no type 'cFunctional%s' exists!",getInstName(),fname,fname);
      functObj[i] = NULL;
      functN[i] = 0;
      free(tpname);
      return 0;
      //functTp[i]  = NULL;
    }
    free(tpname);
  }
  if (requireSorted) {
    SMILE_IDBG(2,"%i Functional components require sorted data.",requireSorted);
  }

  return cVectorProcessor::myConfigureInstance();

}

int cFunctionalsVecToVec::setupNamesForField(int idxi, const char*name, long nEl)
{
  // in a winToVecProcessor , nEl should always be 1!
  int i,j;
  for (i=0; i<nFunctionalsEnabled; i++) {
//        printf("sunames %i i=%i fni=%i foi=%i\n",idxi,i,functN[i],(long)functObj[i]);

    if ( (functN[i] > 0) && (functObj[i] != NULL) ) {
      for (j=0; j<functN[i]; j++) {
//          printf("j=%i functN[%i]=%i\n",j,i,functN[i]);
        char * newname = myvprint("%s_%s",name,functObj[i]->getValueName(j));
        writer->addField( newname );
        free( newname );
      }
    }
  }
  return nFunctValues;
}

// this must return the multiplier, i.e. the vector size returned for each input element (e.g. number of functionals, etc.)
//int cFunctionalsVecToVec::getMultiplier()
//{
//  return nFunctValues;
//}


//    virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
//    virtual int

// idxi is index of input element
// row is the input row
// y is the output vector (part) for the input row
//doProcess(int idxi, cMatrix *row, FLOAT_DMEM*y)
int cFunctionalsVecToVec::processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi)

{
  // copy row to matrix... simple memcpy here
  //  memcpy(y,row->dataF,row->nT*sizeof(FLOAT_DMEM));
  // return the number of components in y!!
  int i; int ok=0;
  FLOAT_DMEM * unsorted = (FLOAT_DMEM*)memdup(src,sizeof(FLOAT_DMEM)*Nsrc);
  FLOAT_DMEM * sorted=NULL;
  
  if (requireSorted) {
    sorted = (FLOAT_DMEM*)malloc(sizeof(FLOAT_DMEM)*Nsrc);
    // quicksort:
    memcpy( sorted, unsorted, sizeof(FLOAT_DMEM) * Nsrc );
    // TODO: check for float_dmem::: with #if ...
    #if FLOAT_DMEM_NUM == FLOAT_DMEM_FLOAT
    smileUtil_quickSort_float(sorted, Nsrc);
    #else
    smileUtil_quickSort_double(sorted, Nsrc);
    #endif
  }

  for (i=0; i<nFunctionalsEnabled; i++) {
    if (functObj[i] != NULL) {
      int ret;
      ret = functObj[i]->process( unsorted, sorted, dst+i, Nsrc, functN[i] );
      if (ret < functN[i]) {
        int j;
        for (j=ret; j<functN[i]; j++) dst[j] = 0.0;
      }
      if (ret>0) ok++;
    }
  }

  if (requireSorted) {
    free(sorted);
  }
  
  free(unsorted);
  
  return nFunctValues;
}

/* TODO
int cFunctionalsVecToVec::doProcess(int idxi, cMatrix *row, INT_DMEM*y)
{
  // copy row to matrix... simple memcpy here
  //memcpy(y,row->dataI,row->nT*sizeof(INT_DMEM));
  // return the number of components in y!!
  return nFunctsEnabled;
}
*/

cFunctionalsVecToVec::~cFunctionalsVecToVec()
{
  int i;
  if (functTp != NULL) {
    for (i=0; i<nFunctTpAlloc; i++)
      if (functTp[i] != NULL) free(functTp[i]);
    free(functTp);
  }
  if (functI != NULL) free(functI);
  if (functN != NULL) free(functN);
  if (functTpI != NULL) free(functTpI);
  if (functObj != NULL) {
    for (i=0; i<nFunctTpAlloc; i++)
      if (functObj[i] != NULL) delete(functObj[i]);
    free(functObj);
  }
}

