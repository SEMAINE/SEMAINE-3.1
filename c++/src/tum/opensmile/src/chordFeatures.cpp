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

Chord features (computed from chroma features). 
This component was written by Benedikt Gollan (TUM), 2009.

*/



#include <chordFeatures.hpp>

#define MODULE "cChordFeatures"

SMILECOMPONENT_STATICS(cChordFeatures)

SMILECOMPONENT_REGCOMP(cChordFeatures)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CCHORDFEATURES;
  sdescription = COMPONENT_DESCRIPTION_CCHORDFEATURES;

  // we inherit cVectorProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cVectorProcessor")

  // if the inherited config type was found, we register our configuration variables
  SMILECOMPONENT_IFNOTREGAGAIN( {} // <- this is only to avoid compiler warnings...
    // name append has a special role: it is defined in cDataProcessor, and can be overwritten here:
	// if you set description to NULL, the existing description will be used, thus the following call can
	// be used to update the default value:
    ct->setField("nameAppend",NULL,"chordbasedfeature");

    // this is an example for adding an integer option:
	//ct->setField("inverse","1 = perform inverse FFT",0);
  )

  // The configType gets automatically registered with the config manger by the SMILECOMPONENT_IFNOTREGAGAIN macro
  
  // we now create out sComponentInfo, including name, description, success status, etc. and return that
  SMILECOMPONENT_MAKEINFO(cChordFeatures);
}

SMILECOMPONENT_CREATE(cChordFeatures)

//-----

cChordFeatures::cChordFeatures(const char *_name) :
  cVectorProcessor(_name)
{

}

void cChordFeatures::fetchConfig()
{
  cVectorProcessor::fetchConfig();
  for (int i=0;i<12;i++)
  {
  chromasaves[i]=0;
  }
  
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
int cChordFeatures::configureWriter(const sDmLevelConfig *c)
{

  // you must return 1, in order to indicate configure success (0 indicated failure)
  return 1;
}
*/


// optional

int cChordFeatures::setupNamesForField(int i, const char*name, long nEl)
{
  //namesAreSet=1;
  //int n = 0;
  //nChroma = nEl;
  //addNameAppendField(name,"myName",nEl); n+=nEl;
  //...
  //return n; 
  return cVectorProcessor::setupNamesForField(i,name,48);
  
}


// use this to allocate data like filter coefficient arrays, etc...
void cChordFeatures::configureField(int idxi, long __N, long nOut)
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
//int cChordFeatures::processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
//{
  // do domething to data in *src, save result to *dst
  // NOTE: *src and *dst may be the same...
 
  //return 1;
//}

// a derived class should override this method, in order to implement the actual processing
int cChordFeatures::processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  // do domething to data in *src, save result to *dst
  // NOTE: *src and *dst may be the same...
  
  
  float chroma[12];
  int i,j;
  float power=0;
  j=12;
  FLOAT_DMEM *ChordFeatures = dst;
int n=0;
  
   for (i=12;i<24;i++)  //get Chroma values from src
  {
      chroma[n]=src[i];//-chromasaves[i];
      ChordFeatures[n]=src[i];//-chromasaves[i];
	  //chromasaves[i]=src[i];
	  n++;
  }

   for (i=0;i<12;i++) //Chords Feature  48<j<59
  {  
      ChordFeatures[j]=3*chroma[i]+chroma[(i+2)%12]+2*chroma[(i+4)%12]+chroma[(i+5)%12]+2*chroma[(i+7)%12]+2*chroma[(i+9)%12]+chroma[(i+11)%12];     
      power+=ChordFeatures[j]*ChordFeatures[j];
      j++;
  }
  power=sqrt(power);
  for (j=12;j<24;j++) //Normalization of Chords
  {
      ChordFeatures[j]=ChordFeatures[j]/power;      
  }
  power=0;

  j=24;
  for (i=0;i<12;i++) //PTR Major  
  {  
      ChordFeatures[j]=(FLOAT_DMEM)( 6.6*chroma[i]+2.2*chroma[(i+1)%12]+3.5*chroma[(i+2)%12]+2.4*chroma[(i+3)%12]+4.8*chroma[(i+4)%12]+4.2*chroma[(i+5)%12]+2.6*chroma[(i+6)%12]+5.5*chroma[(i+7)%12]+2*chroma[(i+8)%12]+3.8*chroma[(i+9)%12]+2.4*chroma[(i+10)%12]+3.3*chroma[(i+11)%12] );     
      power+=ChordFeatures[j]*ChordFeatures[j];
      j++;
  }
  power=sqrt(power);
  for (j=24;j<36;j++) //Normalization of PTR Major
  {
      ChordFeatures[j]=ChordFeatures[j]/power;      
  }
  power=0;

  j=36;  
  for (i=0;i<12;i++) //PTR Minor  120<j<131
  {  
      ChordFeatures[j]=(FLOAT_DMEM)( 6.5*chroma[i]+2.7*chroma[(i+1)%12]+3*chroma[(i+2)%12]+5.2*chroma[(i+3)%12]+2.8*chroma[(i+4)%12]+3.3*chroma[(i+5)%12]+2.6*chroma[(i+6)%12]+4.8*chroma[(i+7)%12]+4.2*chroma[(i+8)%12]+3*chroma[(i+9)%12]+3.5*chroma[(i+10)%12]+3*chroma[(i+11)%12] );     
      power+=ChordFeatures[j]*ChordFeatures[j];      
      j++;
  }
  power=sqrt(power);
  for (j=36;j<48;j++) //Normalization of PTR Minor
  {
      ChordFeatures[j]=ChordFeatures[j]/power;      
  }
  power=0;

  /*for (i=0;i<48;i++)
  {

  }*/

 // *dst=ChordFeatures[0];  //????Syntax
  return 1;

}

cChordFeatures::~cChordFeatures()
{
}

