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


/*  openSMILE component: cMaxIndex

computes Mel-Frequency-Cepstral Coefficients (MFCC) from Mel-Spectrum

*/


#include <maxIndex.hpp>
#include <set>

#define MODULE "cMaxIndex"


SMILECOMPONENT_STATICS(cMaxIndex)

SMILECOMPONENT_REGCOMP(cMaxIndex)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CMAXINDEX;
  sdescription = COMPONENT_DESCRIPTION_CMAXINDEX;

  // we inherit cVectorProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cVectorProcessor")

  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("nameAppend", NULL, "maxIndex");
    // TODO: Thresholding?
    ct->setField("nIndices", "The number of indices to compute", 1);
    ct->setField("minFeature", "Start with this feature in computing the maximum index (1 = first feature)", 1);
    ct->setField("maxFeature", "End with this feature in computing the maximum index (0 = last feature)", 0);
    ct->setField("randNoise", "If > 0, add random noise with given range (+-'randNoise') to index values.", 0.0);
  )

  SMILECOMPONENT_MAKEINFO(cMaxIndex);
}

SMILECOMPONENT_CREATE(cMaxIndex)

//-----

cMaxIndex::cMaxIndex(const char *_name) :
  cVectorProcessor(_name),
  nIndices(1),
  minFeature(1),
  maxFeature(0)
{

}

void cMaxIndex::fetchConfig()
{
  cVectorProcessor::fetchConfig();
  
  nIndices = getInt("nIndices");
  minFeature = getInt("minFeature");
  maxFeature = getInt("maxFeature");
  noise = (FLOAT_DMEM)getDouble("randNoise");
}

/*
int cMaxIndex::myConfigureInstance()
{
  int ret=1;
  ret *= cVectorProcessor::myConfigureInstance();
  if (ret == 0) return 0;

  
  return ret;
}
*/

/*int cMaxIndex::dataProcessorCustomFinalise()
{
  //  Nfi = reader->getLevelNf();

  // allocate for multiple configurations..
  if (sintable == NULL) sintable = (FLOAT_DMEM**)multiConfAlloc();
  if (costable == NULL) costable = (FLOAT_DMEM**)multiConfAlloc();

  return cVectorProcessor::dataProcessorCustomFinalise();
}*/

/*
int cMaxIndex::configureWriter(const sDmLevelConfig *c)
{
  if (c==NULL) return 0;
  
  // you must return 1, in order to indicate configure success (0 indicated failure)
  return 1;
}
*/

int cMaxIndex::setupNamesForField(int i, const char*name, long nEl)
{
  // XXX: include min/maxFeature parameters?
  //name="maxIndex";
  if ((nameAppend != NULL)&&(strlen(nameAppend)>0)) {
    addNameAppendField(name,nameAppend,nIndices,0);
  } else {
    writer->addField( "maxIndex", nIndices, 0);
  }
  return nIndices;
}


/*
int cMaxIndex::myFinaliseInstance()
{
  int ret;
  ret = cVectorProcessor::myFinaliseInstance();
  return ret;
}
*/

// a derived class should override this method, in order to implement the actual processing
/*
int cMaxIndex::processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  // do domething to data in *src, save result to *dst
  // NOTE: *src and *dst may be the same...
  
  return 1;
}
*/

// a derived class should override this method, in order to implement the actual processing
int cMaxIndex::processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  int i,k;
  int last;

  if (maxFeature == 0) last = (int)Nsrc;
  else last = maxFeature;

  SMILE_IDBG(2,"maxFeature = %i",maxFeature);
  std::set<int> indices;
  for (k = 0; k < nIndices; ++k) {
    double max = 0.0;
    int maxIndex = 1;
    for (i = minFeature; i <= last; ++i) {
      if (indices.find(i) == indices.end()) {
        double elem = fabs(src[i-1]);
        if (elem > max) {
          max = elem;
          maxIndex = i;
        }
      }
    }
    indices.insert(maxIndex);
    FLOAT_DMEM x = (FLOAT_DMEM)maxIndex;
    if (noise > 0.0) {
      x += (FLOAT_DMEM)(rand()/RAND_MAX) * noise;
    }
    dst[k] = x;
  }
  return 1;
}

cMaxIndex::~cMaxIndex()
{
}

