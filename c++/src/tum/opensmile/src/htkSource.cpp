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


/*  openSMILE component: cHtkSource
-----------------------------------

HTK Source:

Reads data from an HTK parameter file.

-----------------------------------

11/2009 - Written by Florian Eyben
*/

#include <htkSource.hpp>
#define MODULE "cHtkSource"

/*Library:
sComponentInfo * registerMe(cConfigManager *_confman) {
  cDataSource::registerComponent(_confman);
}
*/
#define N_ALLOC_BLOCK 50

SMILECOMPONENT_STATICS(cHtkSource)

SMILECOMPONENT_REGCOMP(cHtkSource)
{
  SMILECOMPONENT_REGCOMP_INIT

  scname = COMPONENT_NAME_CHTKSOURCE;
  sdescription = COMPONENT_DESCRIPTION_CHTKSOURCE;

  // we inherit cDataSource configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataSource")
  
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("filename","HTK parameter file to read","input.htk");
    ct->setField("featureName","The name of the array-field which is to be created in the data memory output level for the data array read from the HTK file","htkpara");
    ct->setField("blocksize", "The size of data blocks to write at once (to data memory) in frames", 10);
//    ct->setField("featureNames","array of feature names to apply (must match the vector size in the HTK parameter file!)","htkpara",ARRAY_TYPE);
  )

  SMILECOMPONENT_MAKEINFO(cHtkSource);
}

SMILECOMPONENT_CREATE(cHtkSource)

//-----

cHtkSource::cHtkSource(const char *_name) :
  cDataSource(_name),
  eof(0),
  featureName(NULL),
  tmpvec(NULL)
{
  vax = smileHtk_IsVAXOrder();
}

void cHtkSource::fetchConfig()
{
  cDataSource::fetchConfig();
  
  filename = getStr("filename");
  SMILE_IDBG(2,"filename = '%s'",filename);
  featureName = getStr("featureName");
  SMILE_IDBG(2,"featureName = '%s'",featureName);
}


int cHtkSource::myConfigureInstance()
{
  int ret = 1;
  filehandle = fopen(filename, "rb");
  if (filehandle == NULL) {
    COMP_ERR("Error opening file '%s' for reading (component instance '%s', type '%s')",filename, getInstName(), getTypeName());
  }

  if (!readHeader()) {
    ret = 0;
  } else {
    ret *= cDataSource::myConfigureInstance();
  }

  if (ret == 0) {
    fclose(filehandle); filehandle = NULL;
  }
  return ret;
}

/*
int cHtkSource::readHeader()
{
  if (filehandle==NULL) return 0;
  if (!fread(&head, sizeof(sHTKheader), 1, filehandle)) {
    SMILE_IERR(1,"error reading header from file '%s'",filename);
    return 0;
  }
  prepareHeader(&head); // convert to host byte order
  return 1;
}
*/

int cHtkSource::configureWriter(sDmLevelConfig &c)
{
  c.T = ( (double)head.samplePeriod ) * 0.000000100;  // convert HTK 100ns units..
  return 1;
}

int cHtkSource::setupNewNames(long nEl)
{
  N = head.sampleSize/sizeof(float);
  writer->addField(featureName,N);

  allocVec(N);
  tmpvec = (float *)malloc(sizeof(float)*N);

  namesAreSet=1;
  return 1;
}

int cHtkSource::myFinaliseInstance()
{
  int ret = cDataSource::myFinaliseInstance();
  return ret;
}


int cHtkSource::myTick(long long t)
{
  if (isEOI()) return 0;

  SMILE_IDBG(4,"tick # %i, reading value vector from HTK parameter file",t);
  if (eof) {
    SMILE_IDBG(4,"EOF, no more data to read");
    return 0;
  }

  long n;
  for (n=0; n<blocksizeW; n++) {

    // check if there is enough space in the data memory
    if (!(writer->checkWrite(1))) return 0;


    if (fread(tmpvec, head.sampleSize, 1, filehandle)) {
      long i;
      if (vax) {
        for (i=0; i<vec->N; i++) {
          smileHtk_SwapFloat ( (tmpvec+i) );
          vec->dataF[i] = (FLOAT_DMEM)tmpvec[i];
        }
      } else {
        for (i=0; i<vec->N; i++) {
          vec->dataF[i] = (FLOAT_DMEM)tmpvec[i];
        }
      }
    } else {
      // EOF ??
      eof = 1;
    } 


    if (!eof) {
      writer->setNextFrame(vec);
      return 1;
    } else {
      return 0;
    }
  }
  return 0;
}


cHtkSource::~cHtkSource()
{
  if (filehandle!=NULL) fclose(filehandle);
  if (tmpvec != NULL) free(tmpvec);
}
