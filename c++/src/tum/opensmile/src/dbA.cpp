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


/*  openSMILE component: dbA

applies dbX weighting to fft magnitudes

*/


#include <dbA.hpp>

#define MODULE "cDbA"

/*Library:
sComponentInfo * registerMe(cConfigManager *_confman) {
  cDataProcessor::registerComponent(_confman);
}
*/

SMILECOMPONENT_STATICS(cDbA)

SMILECOMPONENT_REGCOMP(cDbA)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CDBA;
  sdescription = COMPONENT_DESCRIPTION_CDBA;

  // we inherit cVectorProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cVectorProcessor")
  SMILECOMPONENT_IFNOTREGAGAIN(
//    ct->setField("nameAppend", NULL, (const);
    ct->setField("curve","1 character, which specifies the type of the curve to use (supported: A ; soon supported: B,C) [NOT YET FULLY IMPLEMENTED, ONLY A is supported]",'A');
    ct->setField("usePower","1 = square the input magnitudes before multiplying with the dX weighting function (the output will then be a dBX weighted power spectrum)",1);
  )
  SMILECOMPONENT_MAKEINFO(cDbA);
}

SMILECOMPONENT_CREATE(cDbA)

//-----

cDbA::cDbA(const char *_name) :
  cVectorProcessor(_name),
  curve(0),
  filterCoeffs(NULL)
{

}

void cDbA::fetchConfig()
{
  cVectorProcessor::fetchConfig();
  
  char c = getChar("curve");
  SMILE_DBG(2,"curve = %c",c);
  if ((c=='A')||(c=='a')) curve = CURVE_DBA;
  if ((c=='B')||(c=='b')) curve = CURVE_DBB;
  if ((c=='C')||(c=='c')) curve = CURVE_DBC;

  usePower = getInt("usePower");
  if (usePower) { SMILE_DBG(2,"using power spectrum"); }
}

/*
int cDbA::myConfigureInstance()
{
  int ret=1;
  ret *= cVectorProcessor::myConfigureInstance();
  if (ret == 0) return 0;

  return ret;
}
*/

/*
int cDbA::configureWriter(const sDmLevelConfig *c)
{
  if (c==NULL) return 0;
  
  // you must return 1, in order to indicate configure success (0 indicated failure)
  return 1;
}
*/

int cDbA::dataProcessorCustomFinalise()
{
  // allocate for multiple configurations..
  if (filterCoeffs == NULL) filterCoeffs = (FLOAT_DMEM**)multiConfAlloc();
  
  return cVectorProcessor::dataProcessorCustomFinalise();
}

int cDbA::setupNamesForField(int i, const char*name, long nEl)
{
  if ((strstr(name,"phase") == NULL)&&(strstr(name,"Phase") == NULL)) {
    const sDmLevelConfig *c = reader->getLevelConfig();
    computeFilters(nEl, c->frameSizeSec, getFconf(i));

    return cVectorProcessor::setupNamesForField(i,name,nEl);
  } else { return 0; }
}


void computeDBA(FLOAT_DMEM *x, long blocksize, FLOAT_DMEM F0)
{
  FLOAT_DMEM curF = 0.0;
  FLOAT_DMEM tmp,cf2;
  int i;
  
  for (i=0; i < blocksize; i++)
  {
    cf2 = (FLOAT_DMEM)pow(curF,(FLOAT_DMEM)2.0);
    tmp = (FLOAT_DMEM)( pow(12200.0,2.0) * pow(cf2,(FLOAT_DMEM)2.0) ) /  ( (cf2+(FLOAT_DMEM)pow(20.6,2.0)) * (cf2+(FLOAT_DMEM)pow(12200.0,2.0))  );
    tmp /=  (FLOAT_DMEM)( sqrt(cf2+pow(107.7,2.0)) * sqrt(cf2+pow(737.9,2.0)) );
      //obj->db[i] = 10.0*log(ddummy) + 2.0;
    *(x++) = (FLOAT_DMEM) pow (10.0,( (10.0*log(tmp) + 2.0) / 10.0));
      //obj->db[i] = fdummy;
    curF += F0;
  }
}

// blocksize is size of fft block, _T is period of fft frames
// sampling period is reconstructed by: _T/((blocksize-1)*2)
int cDbA::computeFilters( long blocksize, double frameSizeSec, int idxc )
{
  FLOAT_DMEM *_filterCoeffs = filterCoeffs[idxc];
  FLOAT_DMEM F0 = (FLOAT_DMEM)(1.0/frameSizeSec);

  if (_filterCoeffs != NULL) { free(_filterCoeffs);   }
  _filterCoeffs = (FLOAT_DMEM *)malloc(sizeof(FLOAT_DMEM) * blocksize);

  // dbA::
  computeDBA(_filterCoeffs, blocksize, F0);
  // TODO::: support dbB, dbC, etc.

  filterCoeffs[idxc] = _filterCoeffs;
  return 1;
}


/*
int cDbA::myFinaliseInstance()
{
  int ret;
  ret = cVectorProcessor::myFinaliseInstance();
  return ret;
}
*/

// a derived class should override this method, in order to implement the actual processing
/*
int cDbA::processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  // do domething to data in *src, save result to *dst
  // NOTE: *src and *dst may be the same...
  
  return 1;
}
*/

// a derived class should override this method, in order to implement the actual processing
int cDbA::processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  int n;
  FLOAT_DMEM *db = filterCoeffs[getFconf(idxi)];

  // do the dbX filtering by multiplying with the filters and summing up
  bzero(dst, Ndst*sizeof(FLOAT_DMEM));

  // copy & square the fft magnitude
  FLOAT_DMEM *_src;
  if (usePower) {
    _src = (FLOAT_DMEM*)malloc(sizeof(FLOAT_DMEM)*Nsrc);
    if (src==NULL) OUT_OF_MEMORY;
    for (n=0; n<Nsrc; n++) {
      _src[n] = src[n]*src[n];
    }
    src = _src;
  }

  for (n=0; n<Ndst; n++) {
    *(dst++) = *(src++) * *(db++);
  }

  if ((usePower)&&(_src!=NULL)) free((void *)_src);

  return 1;
}

cDbA::~cDbA()
{
  multiConfFree(filterCoeffs);
}

