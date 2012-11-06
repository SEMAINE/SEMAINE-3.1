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

data buffer... simply copies data from one level to the other (which then can of course have different buffer sizes)

*/


#include <buffer.hpp>

#define MODULE "cBuffer"

/*Library:
sComponentInfo * registerMe(cConfigManager *_confman) {
  cDataProcessor::registerComponent(_confman);
}
*/

SMILECOMPONENT_STATICS(cBuffer)

SMILECOMPONENT_REGCOMP(cBuffer)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CBUFFER;
  sdescription = COMPONENT_DESCRIPTION_CBUFFER;

  // we inherit cDataProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataProcessor")
  
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("vecCopy","1 = copy vectors/frames one by one , 0 = copy a window/matrix of size 'copyBuf'",1);
    ct->setField("copyBuf","The size of the copy-at-once buffer in frames, if 'vecCopy'=1",1024);
  )
  
  SMILECOMPONENT_MAKEINFO(cBuffer);
}

SMILECOMPONENT_CREATE(cBuffer)

//-----

cBuffer::cBuffer(const char *_name) :
  cDataProcessor(_name)
{
}

void cBuffer::fetchConfig()
{
  cDataProcessor::fetchConfig();
  vecCopy = getInt("vecCopy");
  copyBuf = getInt("copyBuf");
  if (!vecCopy) {
    if (copyBuf < 1) copyBuf = 1;
  }
}

int cBuffer::myFinaliseInstance()
{
  if (!vecCopy) {
    reader->setupSequentialMatrixReading( copyBuf, copyBuf ); // winsize = pre + post
  }
  return cDataProcessor::myFinaliseInstance();
}


int cBuffer::myTick(long long t)
{
  SMILE_DBG(4,"tick # %i, copying data",t);

  if (vecCopy) {
    // get next frame from dataMemory
    cVector *vec = reader->getNextFrame();
    if (vec != NULL) {
      // save frame to dataMemory
      writer->setNextFrame(vec);
      return 1;
    }
  } else {
    // get next block from dataMemory
    cMatrix *mat = reader->getNextMatrix();
    if (mat != NULL) {
      // save block to dataMemory
      writer->setNextMatrix(mat);
      return 1;
    }
  }
  
  return 0;
}


cBuffer::~cBuffer()
{

}

