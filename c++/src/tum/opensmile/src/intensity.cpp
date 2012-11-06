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


/*  openSMILE component: cIntensity

compute simplified intensity according to :
 Andreas Kieﬂling - Extraktion und Klassifikation prosodischer Merkmale in der automatischen Sprachverarbeitung
 Pg. 156-157

(08/2009) Written by Florian Eyben

*/


#include <intensity.hpp>

#define MODULE "cIntensity"

SMILECOMPONENT_STATICS(cIntensity)

SMILECOMPONENT_REGCOMP(cIntensity)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CINTENSITY;
  sdescription = COMPONENT_DESCRIPTION_CINTENSITY;

  // we inherit cVectorProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cVectorProcessor")
  
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("nameAppend",NULL,(const char *)NULL);
    ct->setField("intensity","1 = enable the output of intensity I (mean of squared input values multiplied by a Hamming window)",1);
    ct->setField("loudness","1 = enable the output of loudness L : L = (I/I0)^0.3 ; I0 = 0.000001 (for sample values normalised to the range -1..1)",0);
  )
  SMILECOMPONENT_MAKEINFO(cIntensity);
}

SMILECOMPONENT_CREATE(cIntensity)

//-----

cIntensity::cIntensity(const char *_name) :
  cVectorProcessor(_name),
  hamWin(NULL),
  I0(1.0),
  nWin(0), winSum(0.0)
{

}

void cIntensity::fetchConfig()
{
  cVectorProcessor::fetchConfig();

  intensity = getInt("intensity");
  SMILE_IDBG(2,"intensity = %i\n",intensity);
  loudness = getInt("loudness");
  SMILE_IDBG(2,"loudness = %i\n",loudness);

}

/*
int cIntensity::myConfigureInstance()
{
  int ret=1;
  ret *= cVectorProcessor::myConfigureInstance();
  if (ret == 0) return 0;

//...


  return ret;
}
*/

/*
int cIntensity::configureWriter(const sDmLevelConfig *c)
{

  // you must return 1, in order to indicate configure success (0 indicated failure)
  return 1;
}

*/

int cIntensity::setupNamesForField(int i, const char*name, long nEl)
{
  int n=0;
  long j;

  if (intensity) { addNameAppendField(name, "intensity", 1); n++; }
  if (loudness) { addNameAppendField(name, "loudness", 1); n++; }

  // Create hamming window:
  hamWin = smileDsp_winHam( nEl );
  if (hamWin != NULL) nWin = nEl;
  for (j=0; j<nEl; j++) {
    winSum += hamWin[j];
  }
  if (winSum <= 0.0) winSum = 1.0;

  // I0 constant for 60 dB and max. amplitude = 1.0:  I0 = 10^-6
  I0 = (double)0.000001;

  return n;
}


/*
int cIntensity::myFinaliseInstance()
{
  int ret=1;
  ret *= cVectorProcessor::myFinaliseInstance();
//.....
  return ret;
}
*/

/*
// a derived class should override this method, in order to implement the actual processing
int cIntensity::processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  // do domething to data in *src, save result to *dst
  // NOTE: *src and *dst may be the same...
  
  return 0;
}
*/

// a derived class should override this method, in order to implement the actual processing
int cIntensity::processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  // do domething to data in *src, save result to *dst
  // NOTE: *src and *dst may be the same...
  if (Nsrc == 0) return 0;
  if ((hamWin == NULL)||(nWin<=0)) return 0;

  long i;
  double Im=0.0;

  long safeN = MIN(Nsrc,MIN(nWin,Ndst));
  for (i=0; i<safeN; i++) {
    Im += hamWin[i] * (double)src[i]*(double)src[i];       
  }
  Im /= winSum;

  long n=0;
  if (intensity) { dst[n++] = (FLOAT_DMEM)Im; }
  if (loudness) { dst[n++] = (FLOAT_DMEM)pow( Im/I0 , 0.3 ); }
  
  return n;
}

cIntensity::~cIntensity()
{
  if (hamWin != NULL) free(hamWin);
}

