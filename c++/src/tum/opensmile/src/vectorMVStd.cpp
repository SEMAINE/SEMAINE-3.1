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

concatenates vectors from multiple levels and copy to another level

*/


#include <vectorMVStd.hpp>

#define MODULE "cVectorMVStd"

SMILECOMPONENT_STATICS(cVectorMVStd)

SMILECOMPONENT_REGCOMP(cVectorMVStd)
{
  SMILECOMPONENT_REGCOMP_INIT

  scname = COMPONENT_NAME_CVECTORMVSTD;
  sdescription = COMPONENT_DESCRIPTION_CVECTORMVSTD;

  // we inherit cVectorProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cVectorProcessor")
  
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("meanstdFile", "file to load mean and stddev values from", (const char*)NULL);
    //ct->setField("meanstdFile", "file to load mean and stddev values from", (const char*)NULL);
  )

  SMILECOMPONENT_MAKEINFO(cVectorMVStd);
}

SMILECOMPONENT_CREATE(cVectorMVStd)

//-----

cVectorMVStd::cVectorMVStd(const char *_name) :
  cVectorProcessor(_name),
  stddev(NULL),
  means(NULL),
  nMV(0)
{
}

int cVectorMVStd::myFinaliseInstance()
{
  int ret = cVectorProcessor::myFinaliseInstance();

  if (ret) {
    // open and load init file
    const char *f = getStr("meanstdFile");
    if (f != NULL) {
      FILE *nd = fopen(f,"rb");
      // get file size:
      fseek(nd, 0L, SEEK_END);
      long len = ftell(nd);
      fseek(nd, 0L, SEEK_SET);
      nMV = len / ( sizeof(double) * 2 );
      means = (double*)malloc( sizeof(double) * nMV );
      stddev = (double*)malloc( sizeof(double) * nMV );
      if (nd != NULL) {
        fread(means, sizeof(double)*nMV, 1, nd);
        fread(stddev, sizeof(double)*nMV, 1, nd);
        int i;
        for (i=0; i<nMV; i++) {
          if (stddev[i] <= 0.0) {
            stddev[i] = 0.0000001;
            SMILE_IWRN(2,"some values of stddev in '%s' are <= 0.0 !",f);
          }
        }
        fclose(nd);
      } else {
        SMILE_IERR(1,"ERROR loading mean/stddev data from file '%s'",f);
      }

    }
  }
  return ret;
}

int cVectorMVStd::processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  int i;
  for (i=0; i<MIN(nMV,MIN(Nsrc,Ndst)); i++)  {
    dst[i] = (src[i] - (INT_DMEM)means[i]) / (INT_DMEM)stddev[i];
  }
  return 1;
}

int cVectorMVStd::processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  int i;
  for (i=0; i<MIN(nMV,MIN(Nsrc,Ndst)); i++)  {
    dst[i] = (src[i] - (FLOAT_DMEM)means[i]) / (FLOAT_DMEM)stddev[i];
  }
  return 1;
}

cVectorMVStd::~cVectorMVStd()
{
  if (means!=NULL) free(means);
  if (stddev!=NULL) free(stddev);
}

