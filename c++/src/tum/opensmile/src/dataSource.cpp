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

dataSource
write data to data memory...

*/


#include <dataSource.hpp>

#define MODULE "cDataSource"


SMILECOMPONENT_STATICS(cDataSource)

SMILECOMPONENT_REGCOMP(cDataSource)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CDATASOURCE;
  sdescription = COMPONENT_DESCRIPTION_CDATASOURCE;

  // configure your component's configType:
  SMILECOMPONENT_CREATE_CONFIGTYPE
  if (ct->setField("writer", "The configuration of the cDataWriter subcomponent, which handles the dataMemory interface for data output",
                  sconfman->getTypeObj("cDataWriter"), NO_ARRAY, DONT_FREE) == -1) {
     rA=1; // if subtype not yet found, request , re-register in the next iteration
  }

  ct->setField("buffersize", "The buffer size for the output level in frames (this overwrites buffersize_sec)", 0,0,0);
  ct->setField("buffersize_sec", "The buffer size for the output level in seconds", 0.0);
  ct->setField("blocksize", "A size of data blocks to write at once, in frames (same as blocksizeW for source only components, this overwrites blocksize_sec, if set)", 0,0,0);
  ct->setField("blocksizeW", "The size of data blocks to write in frames (this overwrites blocksize and blocksize_sec, if it is set) (this option is provided for compatibility only... it is exactly the same as 'blocksize')", 0,0,0);
  ct->setField("blocksize_sec", "The size of data blocks to write at once, in seconds", 0.0);
  ct->setField("blocksizeW_sec", "The size of data blocks to write at once, in seconds (this option overwrites blocksize_sec!) (this option is provided for compatibility only... it is exactly the same as 'blocksize')", 0.0,0,0);
  ct->setField("period", "(optional) The period of the input frames, if it cannot be determined from the input file format. (if set and != 0, this will overwrite any automatically set values, e.g. from samplerate etc.)", 0.0,0,0);

  SMILECOMPONENT_IFNOTREGAGAIN( {} )
             // TODO: buffersize config.... (remove in wavesource..)
             
  SMILECOMPONENT_MAKEINFO_ABSTRACT(cDataSource);
}

SMILECOMPONENT_CREATE_ABSTRACT(cDataSource)

//-----

cDataSource::cDataSource(const char *_name) :
  cSmileComponent(_name),
  writer(NULL),
  vec(NULL),
  mat(NULL),
  blocksizeW(1),
  blocksizeW_sec(0.0),
  buffersize(10),
  buffersize_sec(0.0),
  period(0.0),
  namesAreSet(0)
{
  char *tmp = myvprint("%s.writer",getInstName());
  writer = (cDataWriter *)(cDataWriter::create(tmp));
  if (writer == NULL) {
    COMP_ERR("Error creating dataWriter '%s'",tmp);
  }
  if (tmp!=NULL) free(tmp);
}

void cDataSource::mySetEnvironment()
{
  writer->setComponentEnvironment(getCompMan(), -1, this);
}

void cDataSource::allocVec(int n)
{
  if (vec != NULL) { delete vec; }
  vec = new cVector(n);
}

void cDataSource::allocMat(int n, int t)
{
  if (mat != NULL) { delete mat; }
  mat = new cMatrix(n,t);
}

void cDataSource::fetchConfig()
{
  buffersize_sec = getDouble("buffersize_sec");
  SMILE_IDBG(2,"buffersize (sec.) = %f",buffersize_sec);

  buffersize = getInt("buffersize");
  SMILE_IDBG(2,"buffersize (frames.) = %i",buffersize);

//  blocksizeW_sec = -1.0;
//  if (isSet("blocksize_sec")) {
  blocksizeW_sec = getDouble("blocksize_sec");
//  }
  if ( (blocksizeW_sec <= 0.0) || (isSet("blocksizeW_sec")) ) {
    blocksizeW_sec = getDouble("blocksizeW_sec");
  }
  
  SMILE_IDBG(2,"blocksizeW (sec.) = %f",blocksizeW_sec);

  //blocksizeW = -1;
  //if (isSet("blocksize")) {
  blocksizeW = getInt("blocksize");
  //}
  if ( (blocksizeW <= 0) || (isSet("blocksizeW")) ) {
    blocksizeW = getInt("blocksizeW");
  }

  SMILE_IDBG(2,"blocksizeW (frames, from config only) = %i",blocksizeW);

  period = getDouble("period");
}

int cDataSource::myRegisterInstance(int *runMe)
{
  int ret = writer->registerInstance();
  if ((ret)&&(runMe!=NULL)) {
    // call runMe config hook
    *runMe = runMeConfig();
  }
  return ret;
}

int cDataSource::myConfigureInstance()
{
// TODO: set writer config
// if (!writer->isManualConfigSet()) {
//     writer->setConfig(1,2*buffersize,T, 0.0, 0.0 , 0, DMEM_FLOAT);  // lenSec = (double)(2*buffersize)*T
// }
  sDmLevelConfig c2;

  c2.T = period;
  c2.blocksizeWriter = -1;
  c2.basePeriod = -1.0;

  // provide a hook, to allow a derived component to modify the writer config we have obtained from the parent level:
  int ret = configureWriter(c2);
  // possible return values (others will be ignored): 
  // -1 : configureWriter has overwritten c->nT value for the buffersize, myConfigureInstance will not touch nT !
  // 0 : failure, myConfigure must exit with code 0
  // 1 : default / success
  if (!ret) {
    SMILE_IERR(1,"configureWriter() returned 0 (failure)!");
    return 0;  
  }

  // since this component is a source, it sets the base period
  // if it was not set by configureWriter, we have to set it to T here! 
  if (c2.basePeriod == -1.0) 
    c2.basePeriod = c2.T;

  // convert blocksize options, so all options are accessible, if possible:
  // 1. blocksize values in frames override those in seconds:
  // 2. now do the inverse...
  if (blocksizeW > 0) {
    blocksizeW_sec = (double)blocksizeW * c2.T;
  } else if ((blocksizeW_sec > 0.0)&&(c2.T != 0.0)) {
    blocksizeW = (long) ceil (blocksizeW_sec / c2.T);
  } else {
    SMILE_IDBG(3,"using fallback blocksize of 1, because blocksize or blocksize_sec was not set in config!");
    blocksizeW = 1;
  }

  if (c2.blocksizeWriter == -1) { // if configureWriter has not modified the blocksize ...
    // get writer blocksize from "blocksizeW" config options
    c2.blocksizeWriter = blocksizeW;
  } else {
    blocksizeW = c2.blocksizeWriter;
  }

  if (ret!=-1) {
    if (buffersize > 0) {
      c2.nT = buffersize;
    } else {
      if (buffersize_sec > 0.0) {
        if (c2.T != 0.0) {
          c2.nT = (long)ceil(buffersize_sec/c2.T);
        } else {
          c2.nT = (long)ceil(buffersize_sec);
        }
      } else {
        SMILE_IDBG(3,"using default buffersize 100 because buffersize or buffersize_sec was not set in config!");
        c2.nT = 100;
      }
    }
  }

  writer->setConfig(c2,0);

  return writer->configureInstance();
}

int cDataSource::myFinaliseInstance()
{
  // hook, to allow component to set data element names:
  if (!setupNewNames(0)) {
    SMILE_IERR(1,"setupNewNames() returned 0 (failure)!");
    return 0;
  }

  return writer->finaliseInstance();
}

/*
int cDataSource::myTick(long long t)
{

}
*/

cDataSource::~cDataSource()
{
  if (writer != NULL) { delete writer; }
  if (vec != NULL) { delete vec; }
  if (mat != NULL) { delete mat; }
}

