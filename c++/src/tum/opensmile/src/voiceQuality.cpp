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

Voice Quality (Jitter & Shimmer)

Inputs required: pcm frame data (UN-windowed!), F0 final / raw, (optional voicing Prob.) 

*/



#include <voiceQuality.hpp>

#define MODULE "cVoiceQuality"


SMILECOMPONENT_STATICS(cVoiceQuality)

SMILECOMPONENT_REGCOMP(cVoiceQuality)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CVOICEQUALITY;
  sdescription = COMPONENT_DESCRIPTION_CVOICEQUALITY;

  // we inherit cVectorProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cVectorProcessor")

  // if the inherited config type was found, we register our configuration variables
  SMILECOMPONENT_IFNOTREGAGAIN( {} // <- this is only to avoid compiler warnings...
    // name append has a special role: it is defined in cDataProcessor, and can be overwritten here:
	  // if you set description to NULL, the existing description will be used, thus the following call can
  	// be used to update the default value:
    //ct->setField("nameAppend",NULL,"processed");

    ct->setField("F0field","part of the name of the F0 input field (change e.g. to F0raw to use raw pitch instead of smoothed pitch)","F0final");
    ct->setField("pcmField","full name of pcm (wave) data input field. Note: the data must not be windowed!","pcm");
    // periodTolerance   (max jitter)
    // nPeriods     (# of periods to cache)
    // TODO: period memory

    // new data
    ct->setField("jitter","compute Jitter",0);
    ct->setField("shimmer","compute Shimmer",0);
    
    ct->setField("processArrayFields",NULL,0);
  )

  // The configType gets automatically registered with the config manger by the SMILECOMPONENT_IFNOTREGAGAIN macro
  
  // we now create out sComponentInfo, including name, description, success status, etc. and return that
  SMILECOMPONENT_MAKEINFO(cVoiceQuality);
}

SMILECOMPONENT_CREATE(cVoiceQuality)

//-----

cVoiceQuality::cVoiceQuality(const char *_name) :
  cVectorProcessor(_name),
    inData(NULL)
{

}

void cVoiceQuality::fetchConfig()
{
  cVectorProcessor::fetchConfig();

  jitter = getInt("jitter");
  SMILE_IDBG(2,"jitter = %i",jitter);

  shimmer = getInt("shimmer");
  SMILE_IDBG(2,"shimmer = %i",shimmer);
}


int cVoiceQuality::setupNewNames(long nEl)
{

  int n=0;
    //TODO: usw addNameAppendField to preserve base name
  
  if (jitter) { writer->addField("jitter", 1); n++; } 
  if (shimmer) { writer->addField("shimmer", 1); n++; } 

  // find input fields:
  findInputField(getStr("pcmField"),1);
  F0field = findField(getStr("F0field"));
  if (F0field < 0) {
    SMILE_IERR(1,"F0 input field F0field='%s' not found! This field is required, please check the name!",getStr("F0field"));
  }

  // getJitter will store the period lengths in this array
  periods = (long)malloc(1,sizeof(long)*(nInputs+1));

  baseT = getBasePeriod();

  namesAreSet = 1;
  return n;
}

/* T0 as period length of fundamental frequency in frames (float) */
FLOAT_DMEM getJitter(FLOAT_DMEM *pcm, long N, FLOAT_DMEM T0, long *periods)
{
  long i,j;
  long curPos = 0;
  long nextPos = (long)round(T0);

  long periodVar = (long)ceil(T0*0.2);  // <-- TODDO: make this a parameter

  // waveform matching: cross correlation of periods .. slow!
  for (j=0; j<periodVar; j++) {
    for (i=curPos; i<nextPos; i++) {

    }
  }
}

// a derived class should override this method, in order to implement the actual processing
int cVoiceQuality::processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  long i, j;
  long n=0;

  FLOAT_DMEM Jitter = 0.0;
  FLOAT_DMEM Shimmer = 0.0;

  // check for F0:
  if (F0field >= 0) {
    FLOAT_DMEM F0 = src[F0field];
    if (F0 > 0.0) {
      if (!getInputFieldData(src,Nsrc,&inData)) return 0;
      // pcm data is now in inData
      Jitter = getJitter(inData,nInput,(1.0/F0)/baseT,periods);
      Shimmer = getShimmer(inData,nInput,periods);
    } // else // jitter / shimmer will be 0 for unvoiced frames
  } else { return 0; }

  if (jitter) dst[n++] = Jitter;
  if (shimmer) dst[n++] = Shimmer;

  // TODO: cross frame jitter/shimmer!

  return n;
}

cVoiceQuality::~cVoiceQuality()
{
  if (inData != NULL) free(inData);
  
}

