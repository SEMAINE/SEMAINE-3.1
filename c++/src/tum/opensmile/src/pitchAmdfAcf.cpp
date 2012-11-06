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



#include <pitchAmdfAcf.hpp>

#define MODULE "cPitchAmdfAcf"


SMILECOMPONENT_STATICS(cPitchAmdfAcf)

SMILECOMPONENT_REGCOMP(cPitchAmdfAcf)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CPITCHAMDFACF;
  sdescription = COMPONENT_DESCRIPTION_CPITCHAMDFACF;

  // we inherit cVectorProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cVectorProcessor")

  // if the inherited config type was found, we register our configuration variables
  SMILECOMPONENT_IFNOTREGAGAIN( {} // <- this is only to avoid compiler warnings...
    // name append has a special role: it is defined in cDataProcessor, and can be overwritten here:
	  // if you set description to NULL, the existing description will be used, thus the following call can
  	// be used to update the default value:
    //ct->setField("nameAppend",NULL,"processed");
    ct->setField("maxPitch","Maximum detectable pitch", 450.0);  
    ct->setField("minPitch","Minimum detectable pitch", 60.0);  

    ct->setField("F0raw","1/0 = on/off: raw F0 candidate without thresholding in unvoiced segments",1);
    ct->setField("HNR","1/0 = on/off: HNR as: log( hpsVec[F0_idx] / hpsVecMean )",0);
    ct->setField("hpsMean","1/0 = on/off: arithmetic mean of raw hps vector",0);
    ct->setField("hpsVec","1/0 = on/off: dump full raw hps vector (= result of the multiplication of the scaled magnitude spectra)",0);

    ct->setField("voicingCutoff","voicing Prob. (here: HNR) threshold for pitch detection [>1.0]",1.5);
  	ct->setField("processArrayFields",NULL,0);
  )

  // The configType gets automatically registered with the config manger by the SMILECOMPONENT_IFNOTREGAGAIN macro
  
  // we now create out sComponentInfo, including name, description, success status, etc. and return that
  SMILECOMPONENT_MAKEINFO(cPitchAmdfAcf);
}

SMILECOMPONENT_CREATE(cPitchAmdfAcf)

//-----

cPitchAmdfAcf::cPitchAmdfAcf(const char *_name) :
  cVectorProcessor(_name),
  fsSec(-1.0),
//  lastPitch(0.0),
//  lastlastPitch(0.0),
//  glMeanPitch(0.0),
  onsFlag(0),
  work(NULL), workB(NULL)
{

}

void cPitchAmdfAcf::fetchConfig()
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
}


int cPitchAmdfAcf::setupNewNames(long nEl)
{
  if (fsSec == -1.0) {
    const sDmLevelConfig *c = reader->getLevelConfig();
    fsSec = (float)(c->frameSizeSec);
  }

/*  const FrameMetaInfo * fmeta = reader->getFrameMetaInfo();
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
*/
  int n=0;
  if (F0raw) { writer->addField( "AmdfAcfF0raw", 1 ); n++; }
  if (HNR) { writer->addField( "AmdfAcfHNR", 1 ); n++;}
  if (hpsVec) { 
    writer->addField( "hpsVec", nEl ); n += nEl;
  }
  namesAreSet = 1;
  return n;
}



long cPitchAmdfAcf::hpsPitchPeak(const FLOAT_DMEM *_hps, long _N, long start, long end)
{
  if (end > _N) end = _N;
  if (end < 0) return -1;
  if (start > _N) start = _N-1;
  if (start < 0) start = 0;
  
  long i;
  long idx=start;
  FLOAT_DMEM max = _hps[start];
  for (i=start+1; i<end; i++) {
    if (_hps[i] > max) {
      max = _hps[i];
      idx = i;
    }
  }
  return idx;
}

// a derived class should override this method, in order to implement the actual processing
int cPitchAmdfAcf::processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  // we assume we have amdf as input...
  double _N = (double)(Nsrc); 
  double Tsamp = fsSec/(_N*2.0);

  if (work == NULL) {
  //  hss = (FLOAT_DMEM *)malloc(sizeof(FLOAT_DMEM) * nMag);
    work = (FLOAT_DMEM *)malloc(sizeof(FLOAT_DMEM) * Nsrc);
    workB = (char *)malloc(sizeof(char) * Nsrc);
  }

  long i,j;

  for (i=0; i<Nsrc; i++) {
    work[i] = src[i];
  }

  // normalise amdf vector (?)
  smileMath_vectorNormMax(work, Nsrc, 0, 1);

  //quantise amdf with only 1 bit
  for (i=0; i<Nsrc; i++) {
    if (work[i] < 0.4) workB[i] = 1;
    else workB[i] = 0;
  }

  // do acf on 1-bit array
  for (i=0; i<Nsrc; i++) { // <- shift
    char sum=0;
    for (j=i; j<Nsrc; j++) { // <- mult & sum
      sum += workB[j] * workB[j-i];
    }
    // copy back to 'work' array
    work[i] = (FLOAT_DMEM)sum;
  }

  long maxI = (long)ceil(1.0 / (minPitch * Tsamp));
  long minI = (long)floor(1.0 / (maxPitch * Tsamp));
  FLOAT_DMEM F0 = 0.0;
  FLOAT_DMEM hnr = 0.0;

  // ???
    //smileMath_vectorNormMax(work, Nsrc, 0, 1);

  long idx = hpsPitchPeak(work, Nsrc/2, minI, maxI);
if (idx > minI) {
    // compute pitch frequency from peak index
    F0 = 1.f / ( (FLOAT_DMEM)idx * (FLOAT_DMEM)Tsamp );
}


  long n=0;
  if (F0raw) dst[n++] = F0;
  if (HNR) dst[n++] = hnr;
  if (hpsVec) {
    long i;
    for (i=0; i<Nsrc; i++) {
      dst[n++] = work[i];
    }
  }
  return n;
}

cPitchAmdfAcf::~cPitchAmdfAcf()
{
  if (workB != NULL) free(workB);
  if (work != NULL) free(work);
}

