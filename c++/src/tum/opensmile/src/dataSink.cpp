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

dataSink
write data to data memory...

*/


#include <dataSink.hpp>

#define MODULE "cDataSink"

SMILECOMPONENT_STATICS(cDataSink)

SMILECOMPONENT_REGCOMP(cDataSink)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CDATASINK;
  sdescription = COMPONENT_DESCRIPTION_CDATASINK;

  // configure your component's configType:
  SMILECOMPONENT_CREATE_CONFIGTYPE
  if (ct->setField("reader", "The configuration of the cDataReader subcomponent, which handles the dataMemory interface for reading of input",
                  sconfman->getTypeObj("cDataReader"), NO_ARRAY, DONT_FREE) == -1) {
     rA=1; // if subtype not yet found, request , re-register in the next iteration
  }

  
  ct->setField("blocksize", "The size of the data blocks to read at once, in frames (overwrites blocksize_sec, if set)", 0,0,0);
  ct->setField("blocksizeR", "The size of the data blocks to read at once, in frames (this overwrites blocksize and blocksize_sec!) (this option is provided for compatibility only... it is exactly the same as 'blocksize')", 0,0,0);
  ct->setField("blocksize_sec", "The size of the data blocks to read at once, in seconds", 0);
  ct->setField("blocksizeR_sec", "The size of the data blocks to read at once, in seconds (this overwrites blocksize_sec!) (this option is provided for compatibility only... it is exactly the same as 'blocksize')", 0,0,0);


  // TODO: derived class dataSinkChunk (receives turn start/end messages), -> dataSinkChunkFile (writes data to files)

  SMILECOMPONENT_IFNOTREGAGAIN( {} )

  SMILECOMPONENT_MAKEINFO_ABSTRACT(cDataSink);
}

SMILECOMPONENT_CREATE_ABSTRACT(cDataSink)



//-----

cDataSink::cDataSink(const char *_name) :
  cSmileComponent(_name),
  reader(NULL),
  blocksizeR(1),
  blocksizeR_sec(-1.0)
{
  char *tmp = myvprint("%s.reader",getInstName());
  reader = (cDataReader *)(cDataReader::create(tmp));
  if (reader == NULL) {
    COMP_ERR("Error creating dataReader '%s'",tmp);
  }
  if (tmp != NULL) free(tmp);
}

void cDataSink::mySetEnvironment()
{
  reader->setComponentEnvironment(getCompMan(), -1, this);
}

void cDataSink::fetchConfig()
{
  //blocksizeR_sec = -1.0;
  //if (isSet("blocksize_sec")) {
  blocksizeR_sec = getDouble("blocksize_sec");
  //}
  if ( (blocksizeR_sec <= 0.0) || (isSet("blocksizeR_sec")) ) {
    blocksizeR_sec = getDouble("blocksizeR_sec");
  }
  
  SMILE_IDBG(2,"blocksizeR (sec.) = %f",blocksizeR_sec);

  //blocksizeR = -1;
  //if (isSet("blocksize")) {
  blocksizeR = getInt("blocksize");
  //}
  if ( (blocksizeR <= 0) || (isSet("blocksizeR")) ) {
    blocksizeR = getInt("blocksizeR");
  }
}

int cDataSink::myRegisterInstance(int *runMe)
{
  int ret = reader->registerInstance();
  if ((ret)&&(runMe!=NULL)) {
    // call runMe config hook
    *runMe = runMeConfig();
  }
  return ret;
}

int cDataSink::configureReader() 
{ 
  reader->setBlocksize(blocksizeR);
  return 1; 
}

int cDataSink::myConfigureInstance()
{
  int ret = reader->configureInstance();
  if (ret) {
    // convert blocksize options, so all options are accessible, if possible:
    // 1. blocksize values in frames override those in seconds:
    // 2. now do the inverse...
    double TT = reader->getLevelT();
    if (blocksizeR > 0) {
      blocksizeR_sec = (double)blocksizeR * TT;
    } else if ((blocksizeR_sec > 0.0)&&(TT != 0.0)) {
      blocksizeR = (long) ceil (blocksizeR_sec / TT);
    } else {
      SMILE_IDBG(3,"using fallback blocksize of 1, because blocksize or blocksize_sec was not set in config!");
      blocksizeR = 1;
    }
    if (!configureReader()) {
      SMILE_IERR(1,"configureReader() returned 0 (failure)!");
      return 0;  
    }
  }
  return ret;
}

int cDataSink::myFinaliseInstance()
{
  return reader->finaliseInstance();
}

cDataSink::~cDataSink()
{
  if (reader != NULL) { delete reader; }
}

