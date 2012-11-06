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

Input log2 scale spectrum

*/



#include <pitchShs.hpp>

#define MODULE "cPitchShs"


SMILECOMPONENT_STATICS(cPitchShs)

SMILECOMPONENT_REGCOMP(cPitchShs)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CPITCHSHS;
  sdescription = COMPONENT_DESCRIPTION_CPITCHSHS;

  // we inherit cPitchBase configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cPitchBase")

  // if the inherited config type was found, we register our configuration variables
  SMILECOMPONENT_IFNOTREGAGAIN( {} // <- this is only to avoid compiler warnings...
    // name append has a special role: it is defined in cDataProcessor, and can be overwritten here:
	  // if you set description to NULL, the existing description will be used, thus the following call can
  	// be used to update the default value:
    //ct->setField("nameAppend",NULL,"processed");

    // add custom config here...
    ct->setField("inputFieldSearch",NULL,"Mag_logScale");
    ct->setField("nHarmonics","Number of harmonics to consider for subharmonic sampling (feasible values: 5-15)",15);
    ct->setField("compressionFactor","The factor for successive compression of sub-harmonics",0.85);
    ct->setField("voicingCutoff",NULL,0.75);
    ct->setField("octaveCorrection","1 = enable low-level octave correction tuned for the SHS algorithm (will affect F0C1, voicingC1 and F0raw output fields) [EXPERIMENTAL! MAY BREAK CORRECT PITCH DETECTION!]",0);
  )

  // The configType gets automatically registered with the config manger by the SMILECOMPONENT_IFNOTREGAGAIN macro
  
  // we now create out sComponentInfo, including name, description, success status, etc. and return that
  SMILECOMPONENT_MAKEINFO(cPitchShs);
}

SMILECOMPONENT_CREATE(cPitchShs)

//-----

cPitchShs::cPitchShs(const char *_name) :
  cPitchBase(_name),
    SS(NULL), Fmap(NULL)
{

}

void cPitchShs::fetchConfig()
{
  cPitchBase::fetchConfig();

  // fetch custom config here...
  nHarmonics = getInt("nHarmonics");
  SMILE_IDBG(2,"nHarmonics = %i\n",nHarmonics);

  compressionFactor = (FLOAT_DMEM)getDouble("compressionFactor");
  SMILE_IDBG(2,"compressionFactor = %f\n",compressionFactor);

}

int cPitchShs::setupNewNames(long nEl)
{
  int n = cPitchBase::setupNewNames(nEl);
  // set up custom names here:

  cVectorMeta *mdata = reader->getLevelMetaDataPtr();
  FLOAT_DMEM _fmint, _fmaxt, _fmin;
  if (mdata != NULL) {
    _fmin = mdata->fData[0];
    //_fmax = mdata->fData[1];
    nOctaves = mdata->fData[2];
    nPointsPerOctave = mdata->fData[3];
    _fmint = mdata->fData[4];
    _fmaxt = mdata->fData[5];
    if (nOctaves == 0.0) {
      SMILE_IERR(1,"cannot read valid 'nOctaves' from input level meta data, please check if the input is a log(2) scale spectrum from a cSpecScale component!");
      COMP_ERR("aborting!");
    }
  }

  // check for octave scaling:
  base = exp( log((double)_fmin)/(double)_fmint );
  if (fabs(base-2.0) < 0.00001) {
   // oct scale ok
    base = 2.0;
  } else {
   // warn: not oct scale, adjust base internally... untested!
    SMILE_IWRN(1,"log base is not 2.0 (no octave scale spectrum)! Untested behaviour! (base = %f, _fmin %f, _fmint %f)",base,_fmin,_fmint);
  }

  Fmint = _fmint;
  Fstept = (_fmaxt-_fmint)/(FLOAT_DMEM)(nInput-1);

/*  // build frequency mapping for log spectral axis: (obsolete!?)
  Fmap = (FLOAT_DMEM*)malloc(sizeof(FLOAT_DMEM)*nInput);
  long i; 
  for (i=0; i<nInput; i++) {
    Fmap[i] = exp(f*log(base)); // we assume octave scaling here!!
    f += fstep;
  }*/

  // allocate array for sum spectrum
  SS = (FLOAT_DMEM*)malloc(sizeof(FLOAT_DMEM)*nInput);

  return n;
}

int cPitchShs::pitchDetect(FLOAT_DMEM * _inData, long _N, double _fsSec, double _baseT, FLOAT_DMEM *_f0cand, FLOAT_DMEM *_candVoice, FLOAT_DMEM *_candScore, long _nCandidates)
{
  /* subharmonic summation; shift spectra by octaves and add */

  int nCand = 0;
  long i,j;

  if (nOctaves == 0.0) return -1;
  
  for (j=0; j < _N; j++) {
    SS[j] = _inData[j];
  }
  FLOAT_DMEM _scale = compressionFactor;
  
  for (i=2; i < nHarmonics+1; i++) {
    long shift = (long)floor ((double)nPointsPerOctave * smileMath_log2(i));
    for (j=shift; j < _N; j++) {
      SS[j-shift] += _inData[j] * _scale;
    }
    _scale *= compressionFactor;
  }
  for (j=0; j < _N; j++) {
    SS[j] /= (FLOAT_DMEM)nHarmonics;
    if (SS[j] < 0) SS[j] = 0.0;
  }

  // peak candidate picking & computation of SS vector mean
  _candScore[0] = 0.0;
  double ssMean = (double)SS[0];
  for (i=1; i<_N-1; i++) {
    if ( (SS[i-1] < SS[i]) && (SS[i] > SS[i+1]) // <- peak detection
         && ((SS[i] > _candScore[0])||(_candScore[0]==0.0)) ) { // is max. peak or first peak? 
      // shift candScores and f0cand (=indicies)
      for (j=_nCandidates-1; j>0; j--) {
        _candScore[j] = _candScore[j-1];
        _f0cand[j] = _f0cand[j-1];
      }
      _f0cand[0] = (FLOAT_DMEM)i;
      _candScore[0] = SS[i];
      if (nCand<_nCandidates) nCand++;
    }
    ssMean += (double)SS[i];
  }
  ssMean = (ssMean+(double)SS[i])/(double)_N;

  // convert peak candidate frequencies and compute voicing prob.
  for (i=0; i<nCand; i++) {
    long j = (long)_f0cand[i];
    // parabolic peak interpolation:
    FLOAT_DMEM f1 = _f0cand[i]*Fstept + Fmint;
    FLOAT_DMEM f2 = (_f0cand[i]+(FLOAT_DMEM)1.0)*Fstept + Fmint;
    FLOAT_DMEM f0 = (_f0cand[i]-(FLOAT_DMEM)1.0)*Fstept + Fmint;
    double sc=0;
    double fx = smileMath_quadFrom3pts((double)f0, (double)SS[j-1], (double)f1, (double)SS[j], (double)f2, (double)SS[j+1], &sc, NULL);
    // convert log(2) frequency scale to lin frequency scale (Hz):
    _f0cand[i] = (FLOAT_DMEM)exp(fx*log(base));
    _candScore[i] = (FLOAT_DMEM)sc;
    if ((sc > 0.0)&&(sc>ssMean)) {
      _candVoice[i] = (FLOAT_DMEM)( 1.0 - ssMean/sc );
    } else {
      _candVoice[i] = 0.0;
    }
  }

  // octave correction of first candidate:
  if (octaveCorrection) {
    /*
     algo: prefer lower candidate, if voicing prob of lower candidate approx. voicing prob of first candidate (or > voicing cutoff)
     and if score of lower candidate > ( 1/((nHarmonics-1)*compressionFactor) )*score of cand[0]
     */
    for (i=0; i<nCand; i++) {
      if ( (_f0cand[i] < _f0cand[0])&&(_f0cand[i] > 0) && ((_candVoice[i] > voicingCutoff)||(_candVoice[i]>=0.9*voicingCutoff)) && (_candScore[i] > ((1.0/(FLOAT_DMEM)(nHarmonics-1)*compressionFactor))*_candScore[0]) ) {
        // then swap:
        FLOAT_DMEM tmp;
        tmp = _f0cand[0];
        _f0cand[0] = _f0cand[i]; 
        _f0cand[i] = tmp;
        tmp = _candVoice[0];
        _candVoice[0] = _candVoice[i]; 
        _candVoice[i] = tmp;
        tmp = _candScore[0];
        _candScore[0] = _candScore[i]; 
        _candScore[i] = tmp;
      }
    }
  }

  // return actual number of candidates on success (-1 on failure...)
  return nCand;
}

int cPitchShs::addCustomOutputs(FLOAT_DMEM *dstCur, long NdstLeft)
{
  // to be implemented by child class

  // return the number of custom outputs that were added..
  return 0;
}


cPitchShs::~cPitchShs()
{
  if (SS != NULL) free(SS);
  if (Fmap != NULL) free(Fmap);
}

