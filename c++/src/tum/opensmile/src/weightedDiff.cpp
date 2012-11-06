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


/*  openSMILE component: weighted differential

computes a weighted differential as used and described in :

Björn Schuller, Florian Eyben, Gerhard Rigoll: 
"Fast and Robust Meter and Tempo Recognition for the Automatic Discrimination of Ballroom Dance Styles", 
Proc. ICASSP 2007, IEEE, Vol. I, pp. 217-220, ISBN 1-4244-0728-1, Honolulu, Hawaii, USA, 15.-20.04.2007.
*/



#include <weightedDiff.hpp>
//#include <math.h>

#define MODULE "cWeightedDiff"


SMILECOMPONENT_STATICS(cWeightedDiff)

SMILECOMPONENT_REGCOMP(cWeightedDiff)
{
  SMILECOMPONENT_REGCOMP_INIT
    scname = COMPONENT_NAME_CWEIGHTEDDIFF;
  sdescription = COMPONENT_DESCRIPTION_CWEIGHTEDDIFF;

  // we inherit cWindowProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cWindowProcessor")
    SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("nameAppend", NULL, "wsDiff");
  ct->setField("leftwin","The left context window for smoothing, in frames (this overwrites leftwin_sec, if set)",10,0,0);
  ct->setField("leftwin_sec","The left context window for smoothing, in seconds (this will be rounded to the nearest number of frames)",0.1);
  ct->setField("rightwin","The right context window for smoothing or weighting (see doRightWeight option), in frames (this overwrites rightwin_sec, if set)",20,0,0);
  ct->setField("rightwin_sec","right context window for smoothing or weighting (see doRightWeight option), in seconds",0.2);
  ct->setField("doRightWeight","1 = use right mean for weighting / -1 = use left mean for weighting / 0 = use right mean for differential only if rightwin > 0",1);
  ct->setField("blocksize", NULL , 1);
  )
    SMILECOMPONENT_MAKEINFO(cWeightedDiff);
}

SMILECOMPONENT_CREATE(cWeightedDiff)

//-----

cWeightedDiff::cWeightedDiff(const char *_name) :
cWindowProcessor(_name),
lSum(NULL), rSum(NULL),
lastL(NULL), nL(NULL), nR(NULL)
{
}


void cWeightedDiff::fetchConfig()
{
  cWindowProcessor::fetchConfig();

  doRightWeight = getInt("doRightWeight");
  SMILE_DBG(2,"doRightWeight = %i",doRightWeight);
}

/*
int cWeightedDiff::setupNamesForField(int i, const char*name, long nEl)
{
char *tmp = myvprint("%s_de",name);
writer->addField( name, nEl );
return nEl;
}
*/

int cWeightedDiff::configureWriter(sDmLevelConfig &c)
{
  double T = reader->getLevelT();

  if (isSet("leftwin")) {
    leftwin = getInt("leftwin");
    SMILE_DBG(2,"leftwin = %i",leftwin);
  } else {
    double leftwin_sec = getDouble("leftwin_sec");
    SMILE_DBG(2,"leftwin_sec = %f",leftwin_sec);
    if (T != 0.0) {
      leftwin = (int)ceil(leftwin_sec/T);
    } else {
      leftwin = (int)ceil(T);
    }
    SMILE_DBG(2,"leftwin = %i",leftwin);
  }

  if (isSet("rightwin")) {
    rightwin = getInt("rightwin");
    SMILE_DBG(2,"rightwin = %i",rightwin);
  } else {
    double rightwin_sec = getDouble("rightwin_sec");
    SMILE_DBG(2,"rightwin_sec = %f",rightwin_sec);
    if (T != 0.0) {
      rightwin = (int)ceil(rightwin_sec/T);
    } else {
      rightwin = (int)ceil(T);
    }
    SMILE_DBG(2,"rightwin = %i",rightwin);
  }


  if ((leftwin < 1)&&(rightwin<1)) {
    SMILE_IERR(1,"leftwin must be >= 1 (if rightwin < 1) ! (setting to 1)");
    leftwin = 1;
  }
  if (rightwin < 0) {
    SMILE_IWRN(1,"rightwin must be >= 0 ! (setting to 0)");
    rightwin = 0;
  }

  setWindow(leftwin,rightwin);

  // allocate variables:
  long Ni = reader->getLevelN();
//  long i;
  lSum = (double*)calloc(1,sizeof(double)*Ni);
  rSum = (double*)calloc(1,sizeof(double)*Ni);
  lastL = (double*)calloc(1,sizeof(double)*Ni);
  nL = (double*)calloc(1,sizeof(double)*Ni);
  nR = (double*)calloc(1,sizeof(double)*Ni);

  return cWindowProcessor::configureWriter(c);
}

// order is the amount of memory (overlap) that will be present in _in
// buf will have nT timesteps, however also order negative indicies (i.e. you may access a negative array index up to -order!)
int cWeightedDiff::processBuffer(cMatrix *_in, cMatrix *_out, int _pre, int _post, int rowGlobal )
{
  long n;
  int i;
  if (_in->type!=DMEM_FLOAT) COMP_ERR("dataType (%i) != DMEM_FLOAT not yet supported!",_in->type);
  FLOAT_DMEM *x=_in->dataF;
  FLOAT_DMEM *y=_out->dataF;

  /* TODO: FIXME: rSum, lSum, etc. variables need to be individual PER ROW!
  */

  double *_nR = nR+rowGlobal;
  double *_nL = nL+rowGlobal;
  double *_rSum = rSum+rowGlobal;
  double *_lSum = lSum+rowGlobal;
  double *_lastL = lastL+rowGlobal;

  for (n=0; n<_out->nT; n++) {

    FLOAT_DMEM rmean=0, lmean=0, diff=0;

    if (rightwin > 0) { // compute right window mean

      if (*_nR < rightwin) {
        for (i=1; i<=rightwin; i++) {
          *_rSum += (double)x[n+i];
          *_nR += 1.0;
        }
      } else {
        *_rSum = *_rSum - (double)x[n] + (double)x[n+rightwin];
      }

      rmean = (FLOAT_DMEM)( *_rSum / *_nR );
    } 

    if (leftwin > 0) { // compute left window mean

      if (*_nL < leftwin) {
        for (i=1; i<=leftwin; i++) {
          *_lSum += (double)x[n-i];
          *_nL += 1.0;
        }
        *_lastL = (double)x[n-leftwin];
      } else {
        *_lSum = *_lSum - *_lastL + (double)x[n-1];
        *_lastL = (double)x[n-leftwin];
      }

      lmean = (FLOAT_DMEM)( *_lSum / *_nL );
    } 

    if (doRightWeight==1) {
      diff = (x[n] - lmean)*rmean;
    } else if (doRightWeight==-1) {
      diff = (x[n] - rmean)*lmean;
    } else {
      if ((rightwin > 0)&&(leftwin > 0)) {
        diff = (x[n] - (FLOAT_DMEM)0.5*(lmean+rmean));
      } else if (rightwin > 0) {
        diff = (x[n] - rmean);
      } else if (leftwin > 0) {
        diff = (x[n] - lmean);
      }
    }


    y[n] = diff;

  }
  return 1;
}


cWeightedDiff::~cWeightedDiff()
{
  if (lSum != NULL) free(lSum);
  if (rSum != NULL) free(rSum);
  if (lastL != NULL) free(lastL);
  if (nL != NULL) free(nL);
  if (nR != NULL) free(nR);
}

