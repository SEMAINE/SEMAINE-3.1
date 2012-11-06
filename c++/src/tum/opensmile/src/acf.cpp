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

TODO:
Autocorrelation Function (ACF)

simple windowed ACF

(further TODO: continuous ACF (overlap add OR time domain?)

*/

#include <acf.hpp>

#define MODULE "cAcf"

/*Library:
sComponentInfo * registerMe(cConfigManager *_confman) {
cDataProcessor::registerComponent(_confman);
}
*/

SMILECOMPONENT_STATICS(cAcf)

SMILECOMPONENT_REGCOMP(cAcf)
//sComponentInfo * cAcf::registerComponent(cConfigManager *_confman)
{
  SMILECOMPONENT_REGCOMP_INIT
    /*  if (_confman == NULL) return NULL;
    int rA = 0;
    sconfman = _confman;*/

    scname = COMPONENT_NAME_CACF;
  sdescription = COMPONENT_DESCRIPTION_CACF;

  // we inherit cVectorProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cVectorProcessor")
    /*
    ConfigType *ct = new ConfigType( *(sconfman->getTypeObj("cVectorProcessor")) , scname );
    if (ct == NULL) {
    SMILE_WRN(4,"cVectorProcessor config Type not found!");
    rA=1;
    }*/
    SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("nameAppend", NULL, "acf");
    ct->setField("usePower","= square input values; you must leave this at default 1, when using fft magnitude as input",1);
    ct->setField("cepstrum","1 = compute the cepstrum instead of the standard ACF. This applies a log() to the magnitudes bevore transforming from the spectral domain back to the time domain. You might want to set 'nameAppend=cepstrum' when using this option.",0);
  )
    /*    ConfigInstance *Tdflt = new ConfigInstance( scname, ct, 1 );
    sconfman->registerType(Tdflt);
    } */

    SMILECOMPONENT_MAKEINFO(cAcf);
}

SMILECOMPONENT_CREATE(cAcf)

//-----

cAcf::cAcf(const char *_name) :
cVectorProcessor(_name),
usePower(0), cepstrum(0),
data(NULL),
w(NULL),
ip(NULL)
{

}

void cAcf::fetchConfig()
{
  cVectorProcessor::fetchConfig();

  usePower = getInt("usePower");
  if (usePower) { SMILE_IDBG(2,"using power spectrum"); }
  cepstrum = getInt("cepstrum");
  if (cepstrum) { SMILE_IDBG(2,"computing cesptrum instead of ACF"); }
}

/*
int cAcf::myConfigureInstance()
{
int ret=1;
ret *= cVectorProcessor::myConfigureInstance();
if (ret == 0) return 0;

return ret;
}
*/

/*
int cAcf::configureWriter(const sDmLevelConfig *c)
{
if (c==NULL) return 0;

// you must return 1, in order to indicate configure success (0 indicated failure)
return 1;
}
*/

/*
void cAcf::configureField(int idxi, long __N, long nOut)
{
// compute filters:   // TODO:: compute filters for each FIELD (however, only if fields are of different blocksize!)
const sDmLevelConfig *c = reader->getLevelConfig();
computeFilters(__N, c->frameSizeSec, getFconf(idxi));
}
*/


int cAcf::setupNamesForField(int i, const char*name, long nEl)
{
  if (cepstrum) {
    return cVectorProcessor::setupNamesForField(i,name,(nEl-1));
  } else {
    return cVectorProcessor::setupNamesForField(i,name,(nEl-1)); 
  }
}

int cAcf::dataProcessorCustomFinalise()
{
  /*
  if (ip!=NULL) { multiConfFree(ip); ip=NULL; }
  if (w!=NULL) { multiConfFree(w); w = NULL; }
  if (data!=NULL) { multiConfFree(w); data = NULL; }
  */
  if (ip == NULL) ip = (int**)multiConfAlloc();
  if (w == NULL) w = (FLOAT_TYPE_FFT**)multiConfAlloc();
  if (data == NULL) data = (FLOAT_TYPE_FFT**)multiConfAlloc();

  return cVectorProcessor::dataProcessorCustomFinalise();
}

/*
int cAcf::myFinaliseInstance()
{
  int ret;

  ret = cVectorProcessor::myFinaliseInstance();
  return ret;
}
*/

// a derived class should override this method, in order to implement the actual processing
/*
int cAcf::processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
// do domething to data in *src, save result to *dst
// NOTE: *src and *dst may be the same...

return 1;
}
*/

// a derived class should override this method, in order to implement the actual processing
int cAcf::processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  long i,n;
  idxi=getFconf(idxi);
  FLOAT_TYPE_FFT *_data = data[idxi];
  int *_ip = ip[idxi];
  FLOAT_TYPE_FFT *_w = w[idxi];


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

  long N = (Nsrc-1)*2;
  // check for power of 2!!
  if (!smileMath_isPowerOf2(N)) {
    SMILE_IERR(1,"(Nsrc-1)*2 = %i is not a power of 2, this is required for acf!! make sure the input data really is fft magnitude data!",N);
    return 0;
  }

  // data preparation for inverse fft:
  if (_data == NULL) {
    _data = (FLOAT_TYPE_FFT*)malloc(sizeof(FLOAT_TYPE_FFT)*N);
  }
  if (_ip==NULL) _ip = (int *)calloc(1,sizeof(int)*(N+2));
  if (_w==NULL) _w = (FLOAT_TYPE_FFT *)calloc(1,sizeof(FLOAT_TYPE_FFT)*(N*5)/4+2);

  _data[0] = (FLOAT_TYPE_FFT)(src[0]);
  _data[1] = (FLOAT_TYPE_FFT)(src[Nsrc-1]);
  if (cepstrum) {
    for (i=2; i<N-1; i += 2) {
      if (src[i>>1] > 0.0) 
        _data[i] = (FLOAT_TYPE_FFT)log(src[i>>1]);
      else
        _data[i] = 0.0;
      _data[i+1] = 0.0;
    }
  } else {
    for (i=2; i<N-1; i += 2) {
      _data[i] = (FLOAT_TYPE_FFT)(src[i>>1]);
      _data[i+1] = 0.0;
    }
  }

  // inverse fft
  rdft(N, -1, _data, _ip, _w);

  // copy magnitude to output array
  //dst[0] = (FLOAT_DMEM)_data[0];
  /*
  for (i=2; (i<(N-1)/2)&&(i<Ndst*2); i += 2) {
  dst[i>>1] = (FLOAT_DMEM)sqrt( _data[i]*_data[i] + _data[i+1]*_data[i+1]);
  }
  */
  for (i=0; (i<N)&&(i<Ndst); i ++) {
    dst[i] = (FLOAT_DMEM)fabs(_data[i]);
  }

  if ((usePower)&&(_src!=NULL)) free((void *)_src);

  data[idxi] = _data;
  ip[idxi] = _ip;
  w[idxi] = _w;

  return 1;
}

cAcf::~cAcf()
{
  multiConfFree(ip);
  multiConfFree(w);
  multiConfFree(data);
}

