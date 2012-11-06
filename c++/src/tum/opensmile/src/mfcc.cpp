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


/*  openSMILE component: cMfcc

computes Mel-Frequency-Cepstral Coefficients (MFCC) from Mel-Spectrum

*/


#include <mfcc.hpp>

#define MODULE "cMfcc"


SMILECOMPONENT_STATICS(cMfcc)

SMILECOMPONENT_REGCOMP(cMfcc)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CMFCC;
  sdescription = COMPONENT_DESCRIPTION_CMFCC;

  // we inherit cVectorProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cVectorProcessor")

  SMILECOMPONENT_IFNOTREGAGAIN(
    //ct->setField("nameAppend", NULL, ");
    ct->setField("firstMfcc","The first MFCC to compute",1);
    ct->setField("lastMfcc","The last MFCC to compute",12);
    ct->setField("nMfcc","Use this option to specify the number of MFCC, instead of specifying lastMfcc",12);
    ct->setField("melfloor","The minimum value allowed for melspectra when taking the log spectrum (this parameter will be forced to 1.0 when htkcompatible=1)",0.00000001); 
    ct->setField("cepLifter","Parameter for cepstral 'liftering', set this to 0.0 to disable cepstral liftering",22.0);
    ct->setField("htkcompatible","1 = append the 0-th coefficient at the end instead of placing it as the first element of the output vector",1);
  )

  SMILECOMPONENT_MAKEINFO(cMfcc);
}

SMILECOMPONENT_CREATE(cMfcc)

//-----

cMfcc::cMfcc(const char *_name) :
  cVectorProcessor(_name),
  costable(NULL),
  sintable(NULL),
  firstMfcc(1),
  lastMfcc(12),
  htkcompatible(0)
{

}

void cMfcc::fetchConfig()
{
  cVectorProcessor::fetchConfig();
  
  firstMfcc = getInt("firstMfcc");
  SMILE_IDBG(2,"firstMfcc = %i",firstMfcc);
  lastMfcc = getInt("lastMfcc");
  melfloor = (FLOAT_DMEM)getDouble("melfloor");
  SMILE_IDBG(2,"melfloor = %f",melfloor);
  cepLifter = (FLOAT_DMEM)getDouble("cepLifter");
  SMILE_IDBG(2,"cepLifter = %f",cepLifter);

  if (!isSet("lastMfcc")&&isSet("nMfcc")) {
    nMfcc = getInt("nMfcc");
    lastMfcc = firstMfcc + nMfcc - 1;
  } else {
    nMfcc = lastMfcc - firstMfcc + 1;
  }
  SMILE_IDBG(2,"lastMfcc = %i",lastMfcc);
  SMILE_IDBG(2,"nMfcc = %i",nMfcc);

  htkcompatible = getInt("htkcompatible");
  if (htkcompatible) {
    SMILE_IDBG(2,"HTK compatible output is enabled");
	melfloor = 1.0;
  }

  
//  htkcompatible = getInt("htkcompatible");
//  if (htkcompatible) SMILE_DBG(3,"HTK compatible output is enabled");
}

/*
int cMfcc::myConfigureInstance()
{
  int ret=1;
  ret *= cVectorProcessor::myConfigureInstance();
  if (ret == 0) return 0;

  
  return ret;
}
*/

int cMfcc::dataProcessorCustomFinalise()
{
  //  Nfi = reader->getLevelNf();

  // allocate for multiple configurations..
  if (sintable == NULL) sintable = (FLOAT_DMEM**)multiConfAlloc();
  if (costable == NULL) costable = (FLOAT_DMEM**)multiConfAlloc();

  return cVectorProcessor::dataProcessorCustomFinalise();
}

/*
int cMfcc::configureWriter(const sDmLevelConfig *c)
{
  if (c==NULL) return 0;
  
  // you must return 1, in order to indicate configure success (0 indicated failure)
  return 1;
}
*/

int cMfcc::setupNamesForField(int i, const char*name, long nEl)
{
  // compute sin/cos tables:
  initTables(nEl,getFconf(i));

  name="mfcc";
  if ((nameAppend != NULL)&&(strlen(nameAppend)>0)) {
    addNameAppendField(name,nameAppend,nMfcc,firstMfcc);
  } else {
    writer->addField( name, nMfcc, firstMfcc);
  }
  return nMfcc;
  //return cVectorProcessor::setupNamesForField(i,"mfcc",nMfcc);
}


// blocksize is size of mspec block (=nBands)
int cMfcc::initTables( long blocksize, int idxc )
{
  int i,m;
  FLOAT_DMEM *_costable = costable[idxc];
  FLOAT_DMEM *_sintable = sintable[idxc];
  
  if (_costable != NULL) free(_costable);
  _costable = (FLOAT_DMEM *)malloc(sizeof(FLOAT_DMEM)*blocksize*nMfcc);
  if (_costable == NULL) OUT_OF_MEMORY;

  double fnM = (double)(blocksize);
  for (i=firstMfcc; i <= lastMfcc; i++) {
    double fi = (double)i;
    for (m=0; m<blocksize; m++) {
      _costable[m+(i-firstMfcc)*blocksize] = (FLOAT_DMEM)cos((double)M_PI*(fi/fnM) * ( (double)(m) + (double)0.5) );
    }
  }

  if (_sintable != NULL) free(_sintable);
  _sintable = (FLOAT_DMEM *)malloc(sizeof(FLOAT_DMEM)*nMfcc);
  if (_sintable == NULL) OUT_OF_MEMORY;

  if (cepLifter > 0.0) {
    for (i=firstMfcc; i <= lastMfcc; i++) {
      _sintable[i-firstMfcc] = ((FLOAT_DMEM)1.0 + cepLifter/(FLOAT_DMEM)2.0 * sin((FLOAT_DMEM)M_PI*((FLOAT_DMEM)(i))/cepLifter));
    }
  } else {
    for (i=firstMfcc; i <= lastMfcc; i++) {
      _sintable[i-firstMfcc] = 1.0;
    }
  }
  
  costable[idxc] = _costable;
  sintable[idxc] = _sintable;
  
  return 1;
}


/*
int cMfcc::myFinaliseInstance()
{
  int ret;
  ret = cVectorProcessor::myFinaliseInstance();
  return ret;
}
*/

// a derived class should override this method, in order to implement the actual processing
/*
int cMfcc::processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  // do domething to data in *src, save result to *dst
  // NOTE: *src and *dst may be the same...
  
  return 1;
}
*/

// a derived class should override this method, in order to implement the actual processing
int cMfcc::processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  int i,m;
  idxi = getFconf(idxi);
  FLOAT_DMEM *_costable = costable[idxi];
  FLOAT_DMEM *_sintable = sintable[idxi];

  FLOAT_DMEM *_src;
  _src = (FLOAT_DMEM*)malloc(sizeof(FLOAT_DMEM)*Nsrc);
  if (_src==NULL) OUT_OF_MEMORY;
  
  // compute log mel spectrum
  for (i=0; i<Nsrc; i++) {
    if (src[i] < melfloor) _src[i] = log(melfloor);
    else _src[i] = (FLOAT_DMEM)log(src[i]);
  }

  // compute dct of mel data & do cepstral liftering:
  FLOAT_DMEM factor = (FLOAT_DMEM)sqrt((double)2.0/(double)(Nsrc));
  for (i=firstMfcc; i <= lastMfcc; i++) {
    int i0 = i-firstMfcc;
    FLOAT_DMEM * outc = dst+i0;  // = outp + (i-obj->firstMFCC);
    if (htkcompatible && (firstMfcc==0)) {
      if (i==lastMfcc) { i0 = 0; }
      else { i0 += 1; }
    }
    *outc = 0.0;
    for (m=0; m<Nsrc; m++) {
      *outc += _src[m] * _costable[m + i0*Nsrc];
    }
    //*outc *= factor;   // use this line, if you want unliftered mfcc
    // do cepstral liftering:
    *outc *= _sintable[i0] * factor;
  }

  free(_src);
  
  return 1;
}

cMfcc::~cMfcc()
{
  multiConfFree(costable);
  multiConfFree(sintable);
}

