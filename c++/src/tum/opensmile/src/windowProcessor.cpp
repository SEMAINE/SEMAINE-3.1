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

filter :  (abstract class only)
       linear N-th order filter for single value data streams
       this class processed every element of a frame independently
       derived classes only need to implement the filter algorithm

*/


#include <windowProcessor.hpp>

#define MODULE "cWindowProcessor"

SMILECOMPONENT_STATICS(cWindowProcessor)

SMILECOMPONENT_REGCOMP(cWindowProcessor)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CWINDOWPROCESSOR;
  sdescription = COMPONENT_DESCRIPTION_CWINDOWPROCESSOR;

  // we inherit cDataProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataProcessor");

  SMILECOMPONENT_IFNOTREGAGAIN(
//    ct->setField("pre","number of past frames required",1);
//    ct->setField("post","number of post frames required",1);
    ct->setField("noPostEOIprocessing","1 = do not process incomplete windows at the end of the input",0);
    //ct->setField("blocksize","# of samples to process at once (for computational speed-up)",256);
  )

  SMILECOMPONENT_MAKEINFO(cWindowProcessor);
}

SMILECOMPONENT_CREATE_ABSTRACT(cWindowProcessor)

//-----

cWindowProcessor::cWindowProcessor(const char *_name, int _pre, int _post) :
  cDataProcessor(_name),
  matnew(NULL),
  isFirstFrame(1),
  row(NULL),
  rowout(NULL),
  pre(_pre),
  post(_post),
  winsize(0),
  //blocksize(256),
  noPostEOIprocessing(0),
  multiplier(1)
{
  winsize = _pre + _post;
}

void cWindowProcessor::fetchConfig()
{
  cDataProcessor::fetchConfig();
/*
  blocksize = getInt("blocksize");
  SMILE_DBG(2,"blocksize = %i",blocksize);
  */

  noPostEOIprocessing = getInt("noPostEOIprocessing");
  if (noPostEOIprocessing) { SMILE_IDBG(2,"not processing incomplete frames at end of input"); }
}

void cWindowProcessor::setWindow(int _pre, int _post)
{
  pre = _pre;
  post = _post;
  winsize = pre+post;
}

int cWindowProcessor::configureWriter(sDmLevelConfig &c)
{
  // blocksize should not be larger than c->nT / 2
/*
if (blocksize >= c->nT/2) {
    blocksize = c->nT / 2 - 1;
    SMILE_MSG(3,"auto-adjusting blocksize to %i",blocksize);
  }
  if (blocksize < 1) blocksize = 1;
*/  

  /*
  if (!writer->isManualConfigSet()) {
    long bs;
    if (buffersize > 0) bs = buffersize;
    else bs=c->nT;
    if (bs < 2*blocksize+1) bs=2*blocksize+1;
    writer->setConfig(c->isRb, bs, c->T, c->lenSec, c->frameSizeSec, c->growDyn, c->type, 0);
  } else {
    if (writer->getBuffersize() < 2*blocksize+1) writer->setBuffersize(2*blocksize+1);
  }
*/

  if (blocksizeW != blocksizeR) {
    //blocksizeW = blocksizeR;
    blocksizeW_sec = blocksizeR_sec;
    c.blocksizeWriter = blocksizeR;
  }

  if (winsize < pre+post) winsize = pre+post;
  // TODO: seconds/frames??
  reader->setupSequentialMatrixReading(blocksizeR,blocksizeR+winsize,-pre); // winsize = pre + post

  return 1;
}


// order is the amount of memory (overlap) that will be present in _buf
// buf will have nT timesteps, however also order negative indicies (i.e. you may access a negative array index up to -order!)
int cWindowProcessor::processBuffer(cMatrix *_in, cMatrix *_out, int _pre, int _post )
{

  // return value: 
  //   0: error, myTick will return 0, and no frame will be set
  //   1: ok, next frame will be set
  //   -1: ok, myTick will return 1, but no new frame will be set 
  //          (use this for analysers, for example)
  return 0;
}

// overwrite this method in your derived class if you need to know for which input element (matrix row) 
// processBuffer was called (-> rowGlobal)
int cWindowProcessor::processBuffer(cMatrix *_in, cMatrix *_out, int _pre, int _post, int rowGlobal )
{

  // return value: 
  //   0: error, myTick will return 0, and no frame will be set
  //   1: ok, next frame will be set
  //   -1: ok, myTick will return 1, but no new frame will be set 
  //          (use this for analysers, for example)
  return 0;
}

int cWindowProcessor::dataProcessorCustomFinalise()
{
  Ni = reader->getLevelN();
  return 1;
}

/*
int cWindowProcessor::myFinaliseInstance()
{
  int ret=1;
  ret *= reader->finaliseInstance();
  if (!ret) return ret;

  
  //ok? TODO: blocksize < order????  more or less neg start offset... zero pad, => i.e. ignMissingBegin option in reader!!
  return cDataProcessor::myFinaliseInstance();
}
*/

int cWindowProcessor::myTick(long long t)
{
  SMILE_IDBG(4,"tick # %i, running window processor",t);
  if ((isEOI())&&(noPostEOIprocessing)) return 0;

  int ret = 1;
  
  if (!(writer->checkWrite(blocksizeW))) return 0;

  // get next block from dataMemory
  cMatrix *mat = reader->getNextMatrix();
  // TODO: if blocksize< order!! also check if we need to increase the read counter!
  if (mat != NULL) {
    int i,toSet=0;
    if (matnew == NULL) matnew = new cMatrix(mat->N, mat->nT-winsize,mat->type);
      // TODO: support multiplier for N output rows for each input row!
    if (rowout == NULL) rowout = new cMatrix(1,mat->nT-winsize, mat->type);
    if (row == NULL) row = new cMatrix(1,mat->nT, mat->type);
    for (i=0; i<mat->N; i++)  {
      // get matrix row...
      cMatrix *rowr = mat->getRow(i,row);
      if (rowr==NULL) COMP_ERR("cWindowProcessor::myTick : Error getting row %i from matrix! (return obj = NULL!)",i);
      if (row->dataF != NULL) row->dataF += pre;
      if (row->dataI != NULL) row->dataI += pre;
      row->nT -= winsize;
      toSet = processBuffer(row, rowout, pre, post);
      if (toSet == 0) toSet = processBuffer(row, rowout, pre, post, i);
      if (!toSet) ret=0;
      // copy row back into new matrix... ( NO overlap!)
      if (toSet==1) matnew->setRow(i,rowout); // TODO
      if (row->dataF != NULL) row->dataF -= pre;
      if (row->dataI != NULL) row->dataI -= pre;
      row->nT += winsize;
    }
    // set next matrix...
    if (toSet==1)  {
      mat->tmeta += pre; // TODO::: skip "order" elements of tmeta array ..ok?
      matnew->tmetaReplace(mat->tmeta); 
      mat->tmeta -= pre;
      writer->setNextMatrix(matnew);
    }
  } else {
//         printf("WINPROC '%s' mat==NULL tickNr=%i EOI=%i\n",getInstName(),t,isEOI());
    return 0;
  }

  isFirstFrame = 0;
  
  return ret;

}



cWindowProcessor::~cWindowProcessor()
{
  if (row != NULL) delete row;
  if (rowout != NULL) delete rowout;
  if (matnew != NULL) delete matnew;
}

