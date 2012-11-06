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

This component computes pitch via the Harmonic Product Spectrum method.
As input it requires FFT magnitude data. 
Note that the type of input data is not checked, thus be careful when writing your configuration files!

*/



#include <pitchHpsLinf.hpp>

#define MODULE "cPitchHpsLinf"


SMILECOMPONENT_STATICS(cPitchHpsLinf)

SMILECOMPONENT_REGCOMP(cPitchHpsLinf)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CPITCHHPSLINF;
  sdescription = COMPONENT_DESCRIPTION_CPITCHHPSLINF;

  // we inherit cVectorProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cVectorProcessor")

  // if the inherited config type was found, we register our configuration variables
  SMILECOMPONENT_IFNOTREGAGAIN( {} // <- this is only to avoid compiler warnings...
    // name append has a special role: it is defined in cDataProcessor, and can be overwritten here:
	  // if you set description to NULL, the existing description will be used, thus the following call can
  	// be used to update the default value:
    //ct->setField("nameAppend",NULL,"processed");
    ct->setField("maxPitch","Maximum detectable pitch", 650.0);  
    ct->setField("minPitch","Minimum detectable pitch", 30.0);  
    ct->setField("nHarmonics","Number of scaled spectra to multiply, i.e. maximum scaling factor. Should not be larger than the excpected minimum number of harmonics in the signal!", 3);

    ct->setField("F0raw","1/0 = on/off: raw F0 candidate without thresholding in unvoiced segments",1);
    ct->setField("HNR","1/0 = on/off: HNR as: log( hpsVec[F0_idx] / hpsVecMean )",0);
    ct->setField("hpsMean","1/0 = on/off: arithmetic mean of raw hps vector",0);
    ct->setField("hpsVec","1/0 = on/off: dump full raw hps vector (= result of the multiplication of the scaled magnitude spectra)",0);

    ct->setField("voicingCutoff","voicing Prob. (here: HNR) threshold for pitch detection [>1.0]",1.5);
    ct->setField("octaveCorrection","enable simple rule based octave correction, which chooses the lower frequency peak, despite its lower amplitude if it seems reasonable (i.e. if its amplitude is at least 1/nHarmonics * maxPeak)",0);
  	ct->setField("processArrayFields",NULL,0);
  )

  // The configType gets automatically registered with the config manger by the SMILECOMPONENT_IFNOTREGAGAIN macro
  
  // we now create out sComponentInfo, including name, description, success status, etc. and return that
  SMILECOMPONENT_MAKEINFO(cPitchHpsLinf);
}

SMILECOMPONENT_CREATE(cPitchHpsLinf)

//-----

cPitchHpsLinf::cPitchHpsLinf(const char *_name) :
  cVectorProcessor(_name),
  fsSec(-1.0),
//  lastPitch(0.0),
//  lastlastPitch(0.0),
//  glMeanPitch(0.0),
  onsFlag(0),
  hps(NULL), hss(NULL)
{

}

void cPitchHpsLinf::fetchConfig()
{
  cVectorProcessor::fetchConfig();
  
  F0raw = getInt("F0raw");
  SMILE_IDBG(2,"F0raw = %i",F0raw);
  HNR = getInt("HNR");
  SMILE_IDBG(2,"HNR = %i",HNR);
  hpsVecMean = getInt("hpsVecMean");
  SMILE_IDBG(2,"hpsVecMean = %i",hpsVecMean);
  hpsVec = getInt("hpsVec");
  SMILE_IDBG(2,"hpsVec = %i",hpsVec);

  voicingCutoff = getDouble("voicingCutoff");

  octaveCorrection = getInt("octaveCorrection");
  SMILE_IDBG(2,"octaveCorrection = %i",octaveCorrection);

//  if (voicingCutoff > 1.0) voicingCutoff = 1.0;
  if (voicingCutoff < 0.0) voicingCutoff = 0.0;
  SMILE_IDBG(2,"voicingCutoff = %i",voicingCutoff);


  maxPitch = getDouble("maxPitch");
  if (maxPitch < 0.0) maxPitch = 0.0;
  SMILE_IDBG(2,"maxPitch = %lf",maxPitch);

  minPitch = getDouble("minPitch");
  if (minPitch < 0.0) minPitch = 0.0;
  if (minPitch > maxPitch) minPitch = maxPitch;
  SMILE_IDBG(2,"minPitch = %lf",minPitch);

  nHarmonics = getInt("nHarmonics");
  if (nHarmonics < 1) nHarmonics = 1;
  SMILE_IDBG(2,"nHarmonics = %i",nHarmonics);
}


int cPitchHpsLinf::setupNewNames(long nEl)
{
  if (fsSec == -1.0) {
    const sDmLevelConfig *c = reader->getLevelConfig();
    fsSec = (float)(c->frameSizeSec);
  }

  const FrameMetaInfo * fmeta = reader->getFrameMetaInfo();
  int ri=0;
  long idx = fmeta->findFieldByPartialName( "Mag" , &ri );
  if (idx < 0) {
    nMag = nEl;
    magStart = 0;
    SMILE_IWRN(2,"FFT magnitude field '*Mag*' not found, defaulting to use full input vector!");
  } else {
    magStart = ri;
    nMag = fmeta->field[idx].N;
    //printf("start=%i nmag=%i\n",magStart,nMag);
  }

  if (nMag+magStart > nEl) nMag = nEl-magStart;
  if (magStart < 0) magStart = 0;

  if (nHarmonics > nMag/2) {
    SMILE_IWRN(2,"nHarmonics (%i) is > nMag/2 (%i), setting nHarmonics = nMag/2 (note: Nsrc is the number of FFT bands in the input)\n",nHarmonics,nMag/2);
    nHarmonics = nMag/2;
  }

  N = nMag/nHarmonics;

  int n=0;
  if (F0raw) { writer->addField( "hpsF0raw", 1 ); n++; }
  if (HNR) { writer->addField( "hpsHNR", 1 ); n++;}
  if (hpsVecMean) { writer->addField( "hpsVecMean", 1 ); n++;}
  if (hpsVec) { 
    writer->addField( "hpsVec", N ); n += N;
  }
  namesAreSet = 1;
  return n;
}



long cPitchHpsLinf::hpsPitchPeak(const FLOAT_DMEM *_hps, long _N, long start, long end)
{
  if (end > _N) end = _N;
  if (end < 0) return -1;
  if (start > _N) start = _N-1;
  if (start < 0) start = 0;
  

  long i;
  long idx=start; long idxL = idx;
  FLOAT_DMEM max = _hps[start]; FLOAT_DMEM maxL = max;
  for (i=start+1; i<end-1; i++) {
    if ((_hps[i] > _hps[i-1]) && (_hps[i] > _hps[i+1])) { // peak detected...
      if (_hps[i] > max) {
        maxL = max; // shift left (= save one lower frequency peak for octave correction)
        idxL = idx; // shift left
        max = _hps[i];
        idx = i;
      }
    }
  }

  // octave correction:
  if (octaveCorrection) {
    long upper = idx/2 + 1;
    long lower = idx/2 - 1;
    if ( (lower <= idxL) && (idxL <= upper) ) {
      if (maxL > sqrt(1.0/(FLOAT_DMEM)nHarmonics) * max) idx = idxL;
    }
  }

  return idx;
}

// a derived class should override this method, in order to implement the actual processing
int cPitchHpsLinf::processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  // we assume we have fft magnitude as input...
  double _N = (double)(Nsrc);  
  double F0fft = 1.0/fsSec;
  double Tsamp = fsSec/_N;

  if (nMag < 0) return 0;

  if (hps == NULL) {
  //  hss = (FLOAT_DMEM *)malloc(sizeof(FLOAT_DMEM) * nMag);
    hps = (FLOAT_DMEM *)malloc(sizeof(FLOAT_DMEM) * N);
  }

  // compute the harmonic product spectrum
  smileDsp_harmonicProductLin(src+magStart, nMag, hps, N, nHarmonics);

  // normalise HPS vector to range [ 0 - 1 ]
  smileMath_vectorNormMax(hps, N, 0, 1);

  // do peak picking:
  FLOAT_DMEM F0 = 0.0;
  FLOAT_DMEM hnr = 0.0;
  FLOAT_DMEM mean = 0.0;
  long minI = (long)floor(minPitch / F0fft);
  long maxI = (long)ceil(maxPitch / F0fft);
  long F0I = hpsPitchPeak(hps, N, minI, maxI);

  if (F0I > minI) {
    // compute pitch frequency from peak index
    F0 = (FLOAT_DMEM)F0I * (FLOAT_DMEM)F0fft;

    // now compute a pseudo HNR by comparing the peak 
    // amplitude with the mean of the HPS spectrum
    mean = smileMath_vectorAMean(hps, N);
    hnr = (FLOAT_DMEM)log( hps[F0I] / mean );
  }

  if (hnr < voicingCutoff) F0 = 0.0;

  long n=0;
  if (F0raw) dst[n++] = F0;
  if (HNR) dst[n++] = hnr;
  if (hpsVecMean) dst[n++] = mean;
  if (hpsVec) {
    long i;
    for (i=0; i<N; i++) {
      dst[n++] = hps[i];
    }
  }
  return n;
}

cPitchHpsLinf::~cPitchHpsLinf()
{
  //if (hss != NULL) free(hss);
  if (hps != NULL) free(hps);
}

