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

Chroma-based features (computed from chroma features). 

This code was originally written by Benedikt Gollan (TUM), 2007.
It was integrated into openSMILE in 2009 by Benedikt Gollan.


*/



#include <chromaFeatures.hpp>

#define MODULE "cChromaFeatures"

SMILECOMPONENT_STATICS(cChromaFeatures)

SMILECOMPONENT_REGCOMP(cChromaFeatures)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CCHROMAFEATURES;
  sdescription = COMPONENT_DESCRIPTION_CCHROMAFEATURES;

  // we inherit cVectorProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cVectorProcessor")

  // if the inherited config type was found, we register our configuration variables
  SMILECOMPONENT_IFNOTREGAGAIN( {} // <- this is only to avoid compiler warnings...
    // name append has a special role: it is defined in cDataProcessor, and can be overwritten here:
	// if you set description to NULL, the existing description will be used, thus the following call can
	// be used to update the default value:
    ct->setField("nameAppend",NULL,"chromabasedfeature");

    // this is an example for adding an integer option:
	//ct->setField("inverse","1 = perform inverse FFT",0);
  )

  // The configType gets automatically registered with the config manger by the SMILECOMPONENT_IFNOTREGAGAIN macro
  
  // we now create out sComponentInfo, including name, description, success status, etc. and return that
  SMILECOMPONENT_MAKEINFO(cChromaFeatures);
}

SMILECOMPONENT_CREATE(cChromaFeatures)

//-----

cChromaFeatures::cChromaFeatures(const char *_name) :
  cVectorProcessor(_name)
{

}

void cChromaFeatures::fetchConfig()
{
  cVectorProcessor::fetchConfig();
  
  //TODO get Chromavector
  
  //chroma = ...
  
/*
  inverse = getInt("inverse");
  if (inverse) {
    SMILE_DBG(2,"transformFFT set for inverse FFT",inverse);
    inverse = 1;  // sign of exponent
  } else {
    inverse = -1; // sign of exponent
  }
  */
}

/*
int cExampleVectorProcessor::myConfigureInstance()
{
  int ret=1;
  ret *= cVectorProcessor::myConfigureInstance();
  if (ret == 0) return 0;

//...


  return ret;
}
*/

/*
int cChromaFeatures::configureWriter(const sDmLevelConfig *c)
{

  // you must return 1, in order to indicate configure success (0 indicated failure)
  return 1;
}
*/


// optional

int cChromaFeatures::setupNamesForField(int i, const char*name, long nEl)
{
  //namesAreSet=1;
  //int n = 0;
  //nChroma = nEl;
  //addNameAppendField(name,"myName",nEl); n+=nEl;
  //...
  //return n; 
  return cVectorProcessor::setupNamesForField(i,name,156);
  
}


// use this to allocate data like filter coefficient arrays, etc...
void cChromaFeatures::configureField(int idxi, long __N, long nOut)
{
//     const sDmLevelConfig *c = reader->getLevelConfig();
// idxc = getFconf(idxi);
}



/*
int cExampleVectorProcessor::myFinaliseInstance()
{
  int ret=1;
  ret *= cVectorProcessor::myFinaliseInstance();
//.....
  return ret;
}
*/

// a derived class should override this method, in order to implement the actual processing
//int cChromaFeatures::processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
//{
  // do domething to data in *src, save result to *dst
  // NOTE: *src and *dst may be the same...
 
  //return 1;
//}

// a derived class should override this method, in order to implement the actual processing
int cChromaFeatures::processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  // do domething to data in *src, save result to *dst
  // NOTE: *src and *dst may be the same...
  
  float chroma[12];
  int i,j;
  float power=0;
  j=12;
  FLOAT_DMEM *chromafeatures = dst;
  for (i=0;i<12;i++)  //get Chroma values from src
  {
      chroma[i]=src[i];
      chromafeatures[i]=src[i];

  }
  for (i=0;i<12;i++) //Scale Feature 12<j<23
  {  
      chromafeatures[j]=chroma[i]+chroma[(i+2)%12]+chroma[(i+4)%12]+chroma[(i+5)%12]+chroma[(i+7)%12]+chroma[(i+9)%12]+chroma[(i+11)%12];      
      power+=chromafeatures[j]*chromafeatures[j];
      j++;

  }
  power=sqrt(power);
  for (j=12;j<24;j++) //Normalization of Scale
  {
      chromafeatures[j]=chromafeatures[j]/power;      
  }
  power=0;
  
  for (j=24;j<36;j++) //Scale Dominant Feature  24<=j<=35
  {
      chromafeatures[j]=chromafeatures[j-12]+chromafeatures[(j+7)%12+12];      
      power+=chromafeatures[j]*chromafeatures[j];
        }
  power=sqrt(power);
  for (j=24;j<36;j++) //Normalization of Scale Dominant
  {
      chromafeatures[j]=chromafeatures[j]/power; 
  }
  power=0;
  
  for (j=36;j<48;j++) //Scale Cadence Feature  36<j<47
  {
      chromafeatures[j]=chromafeatures[j-24]+chromafeatures[(j+5)%12+12]+chromafeatures[(j+7)%12+12];      
      power+=chromafeatures[j]*chromafeatures[j];
  }
  power=sqrt(power);
  for (j=36;j<48;j++) //Normalization of Scale Cadence
  {
      chromafeatures[j]=chromafeatures[j]/power;      
  }
  power=0;


  j=48;
  for (i=0;i<12;i++) //Chords Feature  48<j<59
  {  
      chromafeatures[j]=3*chroma[i]+chroma[(i+2)%12]+2*chroma[(i+4)%12]+chroma[(i+5)%12]+2*chroma[(i+7)%12]+2*chroma[(i+9)%12]+chroma[(i+11)%12];     
      power+=chromafeatures[j]*chromafeatures[j];
      j++;
  }
  power=sqrt(power);
  for (j=48;j<60;j++) //Normalization of Chords
  {
      chromafeatures[j]=chromafeatures[j]/power;      
  }
  power=0;

  for (j=60;j<72;j++) //Chords Dominant Feature  60<j<71
  {
      chromafeatures[j]=chromafeatures[j-12]+chromafeatures[(j+7)%12+48];      
      power+=chromafeatures[j]*chromafeatures[j];
  }
  power=sqrt(power);
  for (j=60;j<72;j++) //Normalization of Chords Dominant
  {
      chromafeatures[j]=chromafeatures[j]/power;      
  }
  power=0;

  for (j=72;j<84;j++) //Chords Cadence Feature  72<j<83
  {
      chromafeatures[j]=chromafeatures[j-24]+chromafeatures[(j+5)%12+48]+chromafeatures[(j+7)%12+48];      
      power+=chromafeatures[j]*chromafeatures[j];
  }
  power=sqrt(power);
  for (j=72;j<84;j++) //Normalization of Chords Cadence
  {
      chromafeatures[j]=chromafeatures[j]/power;      
  }
  power=0;

  j=84;
  for (i=0;i<12;i++) //PTR Major  84<j<95
  {  
      chromafeatures[j]=(FLOAT_DMEM)6.6*chroma[i]+(FLOAT_DMEM)2.2*chroma[(i+1)%12]+(FLOAT_DMEM)3.5*chroma[(i+2)%12]+(FLOAT_DMEM)2.4*chroma[(i+3)%12]+(FLOAT_DMEM)4.8*chroma[(i+4)%12]+(FLOAT_DMEM)4.2*chroma[(i+5)%12]+(FLOAT_DMEM)2.6*chroma[(i+6)%12]+(FLOAT_DMEM)5.5*chroma[(i+7)%12]+(FLOAT_DMEM)2.0*chroma[(i+8)%12]+(FLOAT_DMEM)3.8*chroma[(i+9)%12]+(FLOAT_DMEM)2.4*chroma[(i+10)%12]+(FLOAT_DMEM)3.3*chroma[(i+11)%12];     
      power+=chromafeatures[j]*chromafeatures[j];
      j++;
  }
  power=sqrt(power);
  for (j=84;j<96;j++) //Normalization of PTR Major
  {
      chromafeatures[j]=chromafeatures[j]/power;      
  }
  power=0;

  for (j=96;j<108;j++) //PTR Major Dominant Feature  96<j<107
  {
      chromafeatures[j]=chromafeatures[j-12]+chromafeatures[(j+7)%12+84];      
      power+=chromafeatures[j]*chromafeatures[j];
  }
  power=sqrt(power);
  for (j=96;j<108;j++) //Normalization of PTR Major Dominant
  {
      chromafeatures[j]=chromafeatures[j]/power;      
  }
  power=0;
  
  for (j=108;j<120;j++) //PTR Major Cadence Feature  108<j<119
  {
      chromafeatures[j]=chromafeatures[j-24]+chromafeatures[(j+5)%12+84]+chromafeatures[(j+7)%12+84];      
      power+=chromafeatures[j]*chromafeatures[j];
  }
  power=sqrt(power);
  for (j=108;j<120;j++) //Normalization of PTR Major Cadence
  {
      chromafeatures[j]=chromafeatures[j]/power;      
  }
  power=0;

  j=120;  
  for (i=0;i<12;i++) //PTR Minor  120<j<131
  {  
      chromafeatures[j]=(FLOAT_DMEM)6.5*chroma[i]+(FLOAT_DMEM)2.7*chroma[(i+1)%12]+(FLOAT_DMEM)3.0*chroma[(i+2)%12]+(FLOAT_DMEM)5.2*chroma[(i+3)%12]+(FLOAT_DMEM)2.8*chroma[(i+4)%12]+(FLOAT_DMEM)3.3*chroma[(i+5)%12]+(FLOAT_DMEM)2.6*chroma[(i+6)%12]+(FLOAT_DMEM)4.8*chroma[(i+7)%12]+(FLOAT_DMEM)4.2*chroma[(i+8)%12]+(FLOAT_DMEM)3.0*chroma[(i+9)%12]+(FLOAT_DMEM)3.5*chroma[(i+10)%12]+(FLOAT_DMEM)3.0*chroma[(i+11)%12];     
      power+=chromafeatures[j]*chromafeatures[j];      
      j++;
  }
  power=sqrt(power);
  for (j=120;j<132;j++) //Normalization of PTR Minor
  {
      chromafeatures[j]=chromafeatures[j]/power;      
  }
  power=0;
  
  for (j=132;j<144;j++) //PTR Minor Dominant Feature  132<j<143
  {
      chromafeatures[j]=chromafeatures[j-12]+chromafeatures[(j+7)%12+120];      
      power+=chromafeatures[j]*chromafeatures[j];
  }
  power=sqrt(power);
  for (j=132;j<144;j++) //Normalization of PTR Minor Dominant
  {
      chromafeatures[j]=chromafeatures[j]/power;      
  }
  power=0;
  
  for (j=144;j<156;j++) //PTR Major Cadence Feature  144<j<155
  {
      chromafeatures[j]=chromafeatures[j-24]+chromafeatures[(j+5)%12+120]+chromafeatures[(j+7)%12+120];      
      power+=chromafeatures[j]*chromafeatures[j];
  }
  power=sqrt(power);
  for (j=144;j<156;j++) //Normalization of PTR Minor Cadence
  {
      chromafeatures[j]=chromafeatures[j]/power;      
  }
  power=0;

 // *dst=chromafeatures[0];  //????Syntax
  return 1;

}

cChromaFeatures::~cChromaFeatures()
{
}

