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


*/


#include <fullinputMean.hpp>

#define MODULE "cFullinputMean"

SMILECOMPONENT_STATICS(cFullinputMean)

SMILECOMPONENT_REGCOMP(cFullinputMean)
{
  SMILECOMPONENT_REGCOMP_INIT

  scname = COMPONENT_NAME_CFULLINPUTMEAN;
  sdescription = COMPONENT_DESCRIPTION_CFULLINPUTMEAN;

  // we inherit cVectorProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataProcessor")
  
    SMILECOMPONENT_IFNOTREGAGAIN( {}
    //ct->setField("expandFields", "expand fields to single elements, i.e. each field in the output will correspond to exactly one element in the input [not yet implemented]", 0);
    //ct->setField("htkcompatible", "1 = HTK compatible mean subtraction (this ignores (i.e. does not subtract the mean of) the 0th MFCC or energy field (and its deltas) specified by the option 'idx0')", 0);
    //ct->setField("idx0", "if > 0, index of field to ignore for mean subtraction (attention: 1st field is 0)", 0);
    ct->setField("htkLogEnorm","performs HTK compatible energy normalisation on all input fields instead of the default action of mean subtraction. The energy normalisation subtracts the maximum value of each value in the sequence and adds 1.",0);
  )

  SMILECOMPONENT_MAKEINFO(cFullinputMean);
}

SMILECOMPONENT_CREATE(cFullinputMean)

//-----

cFullinputMean::cFullinputMean(const char *_name) :
  cDataProcessor(_name),
  means(NULL),
  nMeans(0),
  flag(0), eNormMode(0)
{
}

void cFullinputMean::fetchConfig()
{
  cDataProcessor::fetchConfig();

  eNormMode = (int)getInt("htkLogEnorm");
}

int cFullinputMean::myTick(long long t)
{
  long i;
  if (isEOI()) {
    if (means == NULL) {
      SMILE_IWRN(1,"sequence too short, cannot compute statistics (mean or max value)!");
      long N = reader->getLevelN();
      means = new cVector( N, DMEM_FLOAT );
      for (i=0; i<N; i++) {
        means->dataF[i] = 0;
      }
      nMeans = 1;
    }

    if (flag==0) { 
      reader->setCurR(0); flag = 1; 
      if (!eNormMode) {
        FLOAT_DMEM nM = (FLOAT_DMEM)nMeans;
        if (nM <= 0.0) nM = 1.0;
        for (i=0; i<means->N; i++) {
          means->dataF[i] /= nM;
        }
      }
    }
    cVector *vec = reader->getNextFrame();
    if (vec!= NULL) {
      /*if ((htkcompatible)&&(idx0>0)) {
      for (i=0; i<means->N; i++) {
      if ((i==0)||(i%idx0!=0)) {
      vec->dataF[i] -= means->dataF[i];
      }
      }
      } else {*/
      if (eNormMode) {
        for (i=0; i<means->N; i++) {
          vec->dataF[i] -= means->dataF[i] - (FLOAT_DMEM)1.0;  // x - max + 1 
        }
      } else {
        for (i=0; i<means->N; i++) {
          vec->dataF[i] -= means->dataF[i];
        }
      }
      writer->setNextFrame(vec);
      return 1;
    } 
  } else {
    // compute means, do not write data
    cVector *vec = reader->getNextFrame();
    if (vec != NULL) {
      if (vec->type != DMEM_FLOAT) { COMP_ERR("only float data-type is supported by cFullinputMean!"); }

      if (means == NULL) {
        means = new cVector( vec->N, vec->type );
        for (i=0; i<vec->N; i++) {
          means->dataF[i] = vec->dataF[i];
        }
        nMeans = 1;
      } else {
        if (eNormMode) {
          for (i=0; i<vec->N; i++) {
            if (vec->dataF[i] > means->dataF[i]) means->dataF[i] = vec->dataF[i];
          }
        } else {
          for (i=0; i<vec->N; i++) {
            means->dataF[i] += vec->dataF[i];
          }
          nMeans++;
        }
      }
      return 1;
    }
    
  } 
  return 0;
}


cFullinputMean::~cFullinputMean()
{
  if (means != NULL) delete means;
}

