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

vector Processor :  (abstract class only)
       specialised dataProcessor, which takes one frame as input an produces one frame as output
       however, each array field is processed individually as a vector
       for each field the output dimension can be set in derived components

*/


#include <vectorProcessor.hpp>

#define MODULE "cVectorProcessor"


SMILECOMPONENT_STATICS(cVectorProcessor)

SMILECOMPONENT_REGCOMP(cVectorProcessor)
{
  SMILECOMPONENT_REGCOMP_INIT

  scname = COMPONENT_NAME_CVECTORPROCESSOR;
  sdescription = COMPONENT_DESCRIPTION_CVECTORPROCESSOR;

  // we inherit cDataProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataProcessor")

  SMILECOMPONENT_IFNOTREGAGAIN( {}
    ct->setField("processArrayFields","1 = process each array field as one vector individually (and produce one output for each input array field). Only array fields (i.e. fields with more than one element) are processed if this is set. / 0 = process complete input frame as one vector, ignoring field/element structure",1);
  )

  SMILECOMPONENT_MAKEINFO(cVectorProcessor);
}

SMILECOMPONENT_CREATE_ABSTRACT(cVectorProcessor)

//-----

cVectorProcessor::cVectorProcessor(const char *_name) :
  cDataProcessor(_name),
  Nfo(0), No(0),
  Nfi(0), Ni(0),
  fNi(NULL),
  fNo(NULL),
  vecO(NULL),
  confBs(NULL),
  fconf(NULL),
  fconfInv(NULL),
  Nfconf(0),
  processArrayFields(1)
{

}

void cVectorProcessor::fetchConfig()
{
  cDataProcessor::fetchConfig();

  processArrayFields = getInt("processArrayFields");
  SMILE_IDBG(2,"processArrayFields = %i",processArrayFields);
}

/*
int cVectorProcessor::myConfigureInstance()
{
  int ret=1;
  ret *= cDataProcessor::myConfigureInstance();
  return ret;
}
*/

/*
int cVectorProcessor::configureWriter(const sDmLevelConfig *c)
{

  // you must return 1, in order to indicate configure success (0 indicated failure)
  return 1;
}

*/

int cVectorProcessor::addFconf(long bs, int field) // return value is index of assigned configuration
{
  int i;
  if (bs<=0) return -1;
  for (i=0; i<Nfi; i++) {
    if ((confBs[i] == bs)||(confBs[i]==0)) {
      confBs[i] = bs;
      fconfInv[i] = field;
      fconf[field] = i;
      if (i>=Nfconf) Nfconf = i+1;
      return i;
    }
  }
  return -1;
}


// this method should be overridden by a derived class in order to configure the output fields
/*
int cVectorProcessor::setupNamesForField(int i, const char*name, long nEl)
{
  return cDataProcessor::setupNamesForField(i,name,nEl);
//  return nEl;
}
*/

int cVectorProcessor::dataProcessorCustomFinalise()
{
  if (namesAreSet) return 1;

  int i;

  Nfi = reader->getLevelNf();
  if (!processArrayFields) {
    //    Nfi=1; // ????
    Ni = reader->getLevelN(); // number of elements
  }

  fNi = (long*)calloc(1,sizeof(long)*Nfi); // input
  fNo = (long*)calloc(1,sizeof(long)*Nfi); // output  (Nfo <= Nfi, always!)

  fconf = (int *)malloc(sizeof(int)*Nfi);
  fconfInv = (int *)malloc(sizeof(int)*Nfi);
  confBs = (long *)calloc(1,sizeof(long)*Nfi);

  if (!processArrayFields) {

    int __N=0;
    //const char *tmp = reader->getFieldName(0,&__N);
    fconf[0] = -1;
    addFconf(Ni,0);

    if (!namesAreSet) fNo[0] = setupNewNames(Ni);

    if (!namesAreSet) {
      fNo[0] = 0; 
      //SMILE_IMSG(1,"XXXX array Nfi=%i Ni=%i\n",Nfi,Ni);
      i=0;
     // for (i=0; i<Nfi; i++) {  // OK, changed Nfi to 1 on this line.. hope this breaks nothing else.. ( TODO::: this is NOT OK... why do we run this for each input field?? (names...?) )
        int arrNameOffset=0;
        const char *tmp = reader->getFieldName(i,&__N,&arrNameOffset);
        long nOut = setupNamesForField(i,tmp,Ni);
        if (nOut==Ni) {
          // set arrNameOffset
          writer->setArrNameOffset(arrNameOffset);
        }
        fNo[0] += nOut; //fNo[0] += __N;
     // }
    }
    else { if (fNo[0]==0) fNo[0] = Ni; }
    No = fNo[0];
//old:    Nfo=Nfi;  // ?????
//new:
    Nfo=1;

    fNi[0] = Ni;
    Nfi=1; // ????

    configureField(0,Ni,No);

  } else {

    for (i=0; i<Nfi; i++) { fconf[i] = -1; }
    //
    for (i=0; i<Nfi; i++) {
      int __N=0;
      int arrNameOffset=0;
      const char *tmp = reader->getFieldName(i,&__N,&arrNameOffset);
      if (tmp == NULL) { SMILE_ERR(1,"reader->getFieldName(%i) failed (return value = NULL)!",i); return 0; }
      fNi[i] = __N; Ni += __N;
      if (__N > 1) {
        addFconf(__N,i);
        if (!namesAreSet) {
          fNo[Nfo] = setupNamesForField(i,tmp,__N);
          if (fNo[Nfo]==__N) {
            // set arrNameOffset
            writer->setArrNameOffset(arrNameOffset);
          }
        }
        else fNo[Nfo] = __N; // fNo[i] = __N;
        No += fNo[Nfo];
        configureField(i,__N,fNo[Nfo]);
        Nfo++;
      } else {
        SMILE_IWRN(1,"ignoring single (non-array) field '%s'!",tmp);
      }
    }

  }

  namesAreSet = 1;
  return 1;
}

/*
int cVectorProcessor::myFinaliseInstance()
{
  int ret=1;
  ret *= reader->finaliseInstance();
  if (!ret) return ret;

 

  for (i=0; i<Nfconf; i++) {
    //      configureField(fconfInv[i],fNi[fconfInv[i]]);
    configureField(fconfInv[i],confBs[i]);
  }

  
  return cDataProcessor::myFinaliseInstance();
}

*/

// a derived class should override this method, in order to implement the actual processing
int cVectorProcessor::processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  //memcpy(dst,src, MIN(Nsrc,Ndst));
  SMILE_ERR(1,"component '%s' (type '%s') does not support data type DMEM_FLOAT (yet) !",getInstName(),getTypeName());
  return 1;
}

// a derived class should override this method, in order to implement the actual processing
int cVectorProcessor::processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  //memcpy(dst,src, MIN(Nsrc,Ndst));
  SMILE_ERR(1,"component '%s' (type '%s') does not support data type DMEM_INT (yet) !",getInstName(),getTypeName());
  return 1;
}

// a derived class should override this method, in order to implement the actual processing
int cVectorProcessor::flushVectorFloat(FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  //memcpy(dst,src, MIN(Nsrc,Ndst));
  //SMILE_ERR(1,"component '%s' (type '%s') does not support data type DMEM_FLOAT (yet) !",getInstName(),getTypeName());
  return 0;
}

// a derived class should override this method, in order to implement the actual processing
int cVectorProcessor::flushVectorInt(INT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  //memcpy(dst,src, MIN(Nsrc,Ndst));
  //SMILE_ERR(1,"component '%s' (type '%s') does not support data type DMEM_INT (yet) !",getInstName(),getTypeName());
  return 0;
}

int cVectorProcessor::myTick(long long t)
{
  SMILE_IDBG(4,"tick # %i, running vector processor",t);

  if (!(writer->checkWrite(1))) return 0;
// printf("'%s' checkwrite ok\n",getInstName());

  // get next frame from dataMemory
  cVector *vec = reader->getNextFrame();
  int i,iO=0,toSet=1,ret=1; int res;

  int type;
  if (vec==NULL) {
    const sDmLevelConfig * c = writer->getLevelConfig();
    if (c!=NULL) type = c->type;
    else return 0;
  } else { 
    customVecProcess(vec);
    type=vec->type; 
  }

  //if (vec != NULL) {
  if (vecO == NULL) vecO = new cVector(No,type);

  if (type == DMEM_FLOAT) {
    FLOAT_DMEM *dFi = NULL;
    if (vec != NULL) {
      dFi = vec->dataF;
    }
    FLOAT_DMEM *dFo = vecO->dataF;
    for (i=0; i<Nfi; i++) {
      if ((fNi[i]<=1) && (processArrayFields)) continue;
      if (vec != NULL) {
        if ((dFo == NULL)||(fNo[iO]<=0)) {
          SMILE_IERR(1,"output field size for field %i is 0 in call to processVectorFloat!\n  Please check if setupNewNames or setupNamesForField returns a number > 0 !!",iO);
          COMP_ERR("aborting here, since this is a serious bug in this component ...");
        }
        res = processVectorFloat(dFi, dFo, fNi[i], fNo[iO], i);
      } else {
        if ((dFo == NULL)||(fNo[iO]<=0)) {
          SMILE_IERR(1,"output field size for field %i is 0 in call to processVectorFloat!\n  Please check if setupNewNames or setupNamesForField returns a number > 0 !!",iO);
          COMP_ERR("aborting here, since this is a serious bug in this component ...");
        }
        res = flushVectorFloat(dFo, fNi[i], fNo[iO], i);
      }
      if (res == 0) ret=0;
      else if (res < 0) toSet=0;
      dFi += fNi[i];
      dFo += fNo[iO];
      iO++;
    }
  } else if (type == DMEM_INT) {
    INT_DMEM *iFi = NULL;
    if (vec != NULL) {
      iFi = vec->dataI;
    }
    INT_DMEM *iFo = vecO->dataI;
    for (i=0; i<Nfi; i++) {
      if (fNi[i]<=1) continue;
      if (vec != NULL) {
        if ((iFo == NULL)||(fNo[iO]<=0)) {
          SMILE_IERR(1,"output field size for field %i is 0 in call to processVectorFloat!\n  Please check if setupNewNames or setupNamesForField returns a number > 0 !!",iO);
          COMP_ERR("aborting here, since this is a serious bug in this component ...");
        }
        res = processVectorInt(iFi, iFo, fNi[i], fNo[iO], i);
      } else {
        if ((iFo == NULL)||(fNo[iO]<=0)) {
          SMILE_IERR(1,"output field size for field %i is 0 in call to processVectorFloat!\n  Please check if setupNewNames or setupNamesForField returns a number > 0 !!",iO);
          COMP_ERR("aborting here, since this is a serious bug in this component ...");
        }
        res = flushVectorInt(iFo, fNi[i], fNo[iO], i);
      }
      if (res == 0) ret=0;
      else if (res < 0) toSet=0;

      iFi += fNi[i];
      iFo += fNo[iO];
      iO++;
    }
  } else {
    SMILE_ERR(1,"unknown data type: vec->type = %i! (instance: '%s')",vec->type,getInstName());
  }

  if (!ret) toSet=0;
  if (toSet) {    
    if (vec != NULL) vecO->tmetaReplace(vec->tmeta);

    // save to dataMemory
    writer->setNextFrame(vecO);
  }

  return ret;
  //}
  //return 0;
}


void cVectorProcessor::multiConfFree( void *x )
{
  int i;
  void **y = (void **)x;
  if (y!=NULL) {
    for (i=0; i<getNf(); i++) {
      if (y[i] != NULL) free(y[i]);
    }
    free(y);
  }
}

cVectorProcessor::~cVectorProcessor()
{
  if (fNi!=NULL)  free(fNi);
  if (fNo!=NULL)  free(fNo);
  if (fconf != NULL) free(fconf);
  if (fconfInv != NULL) free(fconfInv);
  if (confBs != NULL)  free(confBs);
  if (vecO!=NULL) delete vecO;
}

