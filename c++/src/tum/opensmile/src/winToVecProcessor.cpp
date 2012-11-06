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

reads in windows and outputs vectors (frames)

*/


#include <winToVecProcessor.hpp>

#define MODULE "cWinToVecProcessor"


SMILECOMPONENT_STATICS(cWinToVecProcessor)

SMILECOMPONENT_REGCOMP(cWinToVecProcessor)
{
  SMILECOMPONENT_REGCOMP_INIT
  
  scname = COMPONENT_NAME_CWINTOVECPROCESSOR;
  sdescription = COMPONENT_DESCRIPTION_CWINTOVECPROCESSOR;

  // we inherit cDataProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataProcessor");
  
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->disableField("blocksize");
    ct->disableField("blocksizeR");
    ct->disableField("blocksizeW");
    ct->disableField("blocksize_sec");
    ct->disableField("blocksizeR_sec");
    ct->disableField("blocksizeW_sec");

    ct->setField("frameMode","Specifies how to create frames: 'fixed' (fixed frame size, given via 'frameSize' option), 'full' (create one frame at the end of the input only), 'variable' (via message), 'list' (in config file or external text file, see frameList and frameListFile options, UNIMPLEMENTED)","fixed");
    ct->setField("frameListFile","Filename of a file with a list of frame intervals to load (text file with a comma separted list of intervals: 1-10,11-20 , etc., if no interval is specified, i.e. no - is found then consecutive frames with the given number being the frame length are assumed; first index is 0; use the suffix \"s\" after the numbers to specify intervals in seconds (e.g. 0s-2.5s); use an 'E' instead of a number for 'end of sequence')",(const char*) NULL);
    ct->setField("frameList","The list of frame intervals specified directly in the configuration file (comma separted list of intervals: 1-10,11-20 , etc., if no interval is specified, i.e. no - is found then consecutive frames with the given number being the frame length are assumed; first index is 0; use the suffix \"s\" after the numbers to specify intervals in seconds (e.g. 0s-2.5s); use an 'E' instead of a number for 'end of sequence')",(const char*) NULL);

    ct->setField("frameSize","The frame size in seconds (0.0 = full input, same as frameMode=full)",0.025);
    ct->setField("frameStep","The frame step (frame sampling period) in seconds (0.0 = set to the same value as 'frameSize')",0.0);
    ct->setField("frameSizeFrames","The frame size in input level frames (=samples for a pcm/wave input level) (overrides frameSize, if set and > 0)",0,0,0);
    ct->setField("frameStepFrames","The frame step in input level frames (=samples for a pcm/wave input level) (overrides frameStep, if set and > 0)",0,0,0);
    ct->setField("frameCenter","The frame center in seconds, i.e. where frames are sampled (0=left)",0,0,0);
    ct->setField("frameCenterFrames","The frame sampling center in input level frames (overrides frameCenter, if set), (0=left)",0,0,0);
    ct->setField("frameCenterSpecial","The frame sampling center (overrides the other frameCenter options, if set). The available special frame sampling points as strings are: 'mid' = middle (first frame from -frameSize/2 to frameSize/2), 'left' = sample at the beginning of the frame (first frame from 0 to frameSize), 'right' = sample at the end of the frame (first frame from -frameSize to 0)","left");
    ct->setField("noPostEOIprocessing","1 = do not process incomplete windows at the end of the input",1);
//    ct->setField("frameBorderList","array list of frame borders (in seconds), if frameMode==list",(const char*)NULL, ARRAY_TYPE);
//    ct->setField("frameList","array list of frame start/end times (in seconds) (specifiy as: '0.3-1.7 ; 0.9-2.1', for example), if frameMode==list (use either this OR frameBorderList)",(const char*)NULL, ARRAY_TYPE);
  )

  SMILECOMPONENT_MAKEINFO(cWinToVecProcessor);
}

SMILECOMPONENT_CREATE(cWinToVecProcessor)

//-----

cWinToVecProcessor::cWinToVecProcessor(const char *_name) :
  cDataProcessor(_name),
  wholeMatrixMode(0), processFieldsInMatrixMode(0),
  //outputPeriod(0.0),
  fsfGiven(0),
  fstfGiven(0),
  frameSizeFrames(0),
  frameStepFrames(0),
  frameCenterFrames(0),
  frameSize(0.0),
  frameStep(0.0),
  frameCenter(0.0),
  pre(0),
  dtype(0),
  matBuf(NULL), matBufN(0), matBufNalloc(0),
  tmpRow(NULL),
  tmpVec(NULL),
  lastText(NULL), lastCustom(NULL),
  tmpFrameF(NULL),
  tmpFrameI(NULL),
  noPostEOIprocessing(0),
  nQ(0),
  frameMode(FRAMEMODE_FIXED),
  ivSec(NULL), ivFrames(NULL)
{
}

void cWinToVecProcessor::fetchConfig()
{
  cDataProcessor::fetchConfig();

  if (isSet("frameSizeFrames")) {
    frameSizeFrames = getInt("frameSizeFrames");
    if (frameSizeFrames != 0)  fsfGiven = 1;
    SMILE_DBG(2,"frameSizeFrames = %i",frameSizeFrames);
  } else {
    frameSize = getDouble("frameSize");
    SMILE_DBG(2,"frameSize = %f s",frameSize);
  }
  if (isSet("frameStepFrames")) {
    frameStepFrames = getInt("frameStepFrames");
    if (frameStepFrames != 0) fstfGiven = 1;
    SMILE_DBG(2,"frameStepFrames = %i",frameStepFrames);
  } else {
    frameStep = getDouble("frameStep");
    SMILE_DBG(2,"frameStep = %f s",frameStep);
  }

  noPostEOIprocessing = getInt("noPostEOIprocessing");
  if (noPostEOIprocessing) { SMILE_IDBG(2,"not processing incomplete frames at end of input"); }
  
  const char *tmp = getStr("frameMode");
  if (tmp != NULL) {
    if (!strncmp(tmp,"fix",3)) { frameMode = FRAMEMODE_FIXED; }
    else if (!strncmp(tmp,"var",3)) { frameMode = FRAMEMODE_VAR; }
    else if (!strncmp(tmp,"ful",3)) { frameMode = FRAMEMODE_FULL; }
    else if (!strncmp(tmp,"msg",3)) { frameMode = FRAMEMODE_VAR; }
    else if (!strncmp(tmp,"meta",4)) { frameMode = FRAMEMODE_META; }
    else if (!strncmp(tmp,"list",4)) { frameMode = FRAMEMODE_LIST; }
    else if (!strncmp(tmp,"message",7)) { frameMode = FRAMEMODE_VAR; }
  }
  SMILE_IDBG(2,"frameMode = %i\n",frameMode);

  if (frameMode == FRAMEMODE_LIST) {
    const char *flf=NULL;
    char * fl = NULL;
    if (isSet("frameListFile")) { // prefer file over config file list
      flf = getStr("frameListFile");
      SMILE_IMSG(3,"using frameList from external file: '%s'");
      if (isSet("frameList")) {
        SMILE_IWRN(1,"frameListFile overrides option frameList! option frameList will be ignored!");
      }
      if (flf != NULL) { // open file and read frame list (only 1st line!)
        FILE *f = fopen(flf,"r");
        size_t n;
        getline(&fl,&n,f);
        fclose(f);
      } else { // display error message, shouldn't happen
        SMILE_IERR(1,"cannot open frameListFile 'NULL'");
        COMP_ERR("aborting");
      }
    } else {
      if (isSet("frameList")) {
        // get frame list
        const char * _fl = getStr("frameList");
        if (_fl != NULL) {
          fl = strdup(_fl);
        } else {
          SMILE_IERR(1,"frameList string is 'NULL'");
          COMP_ERR("aborting");
        }
      } else {
        SMILE_IERR(1,"you have selected frameMode=list, but you have specified neither frameList nor frameListFile options! One of these is required!");
        COMP_ERR("aborting");
      }
    }
    if (fl != NULL) { // if frameList has been loaded
      // parse frame list string
      // 1(s)-20(s),...,999-E <- intervals
      // 1,2,1,1,   <- lengths

      // do a quick count of commas to determine number of intervals
      char * c = NULL;
      char * s = fl;
      int nIvs = 1;
      do {
        c = strchr(s,',');
        if (c != NULL) {
          nIvs++;
          s = c+1;
        }
      } while (c!=NULL);

      // allocate arrays
      ivSec = (double*)calloc(1,sizeof(double)*nIvs*2);
      ivFrames = (long*)calloc(1,sizeof(long)*nIvs*2);

      // find commas, ignore spaces
      s = fl;
      int l;
      int isIv = -1; 
      do {
        c = strchr(s,',');
        if (c != NULL) {
          *c = 0;
        }
        l = (int)strlen(s);
        // process string s, which contains one token only by now
        // check for "-" in string -> interval
        char *x = strchr(s,'-');
        if (x!=NULL) { // interval
          if (isIv == -1) isIv = 1;
          else {
            if (isIv != 1) {
              SMILE_IWRN(2,"mixing intervals and continuous frames (lenghts) may lead to unpredictable results if not used deliberately and with care! (see frameList or frameListFile option : substring string  = '%s')",s);
            }
          }


        } else {
          if (isIv == -1) isIv = 0;
          else {
            if (isIv != 0) {
              SMILE_IWRN(2,"mixing intervals and continuous frames (lenghts) may lead to unpredictable results if not used deliberately and with care! (see frameList or frameListFile option : substring string  = '%s')",s);
            }
          }

        }
        s = c+1;
      } while (c != NULL);


      // free string
      free(fl);
    }

    // TODO: get frame array list
    SMILE_IERR(1,"FRAMEMODE_LIST not yet implemented!! See winToVecProcessor.cpp!");
  }
/*
  outputPeriod = getDouble("outputPeriod");
  if (outputPeriod <= 0.0) outputPeriod = 0.1;
  SMILE_DBG(2,"outputPeriod = %f s",outputPeriod);
*/

/*
  if (isSet("outputBuffersize")) {
    outputBuffersize = getInt("outputBuffersize");
    SMILE_DBG(2,"outputBuffersize = %i frames",outputBuffersize);
  }*/
  
}

int cWinToVecProcessor::addFconf(long bs, int field) // return value is index of assigned configuration
{
  int i;
  if (bs<=0) return -1;
  for (i=0; i<Nfi; i++) {
    if ((confBs[i] == bs)||(confBs[i]==0)) {
      confBs[i] = bs;
      fconfInv[i] = field;
      fconf[field] = i;
      if (i>=Nfconf) Nfconf = i+1;
      return i;
    }
  }
  return -1;
}

void cWinToVecProcessor::multiConfFree( void *x )
{
  int i;
  void **y = (void **)x;
  if (y!=NULL) {
    for (i=0; i<getNf(); i++) {
      if (y[i] != NULL) free(y[i]);
    }
    free(y);
  }
}

int cWinToVecProcessor::configureWriter(sDmLevelConfig &c)
{
  SMILE_DBG(2,"reader period = %f",c.T);

  if ((!fsfGiven)&&(c.T!=0.0)) frameSizeFrames = (long)round(frameSize / c.T);
  else frameSize = ((double)frameSizeFrames) * c.T;
  if (frameStep == 0.0) frameStep = frameSize;
  if (!fstfGiven) {
    if (c.T != 0.0)
      frameStepFrames = (long)round(frameStep / c.T);
    else
      frameStepFrames = frameSizeFrames;
  } else frameStep = ((double)frameStepFrames) * c.T;
  if (frameStepFrames == 0) frameStepFrames = frameSizeFrames;

  
  SMILE_DBG(2,"computed frameSizeFrames = %i",frameSizeFrames);
  SMILE_DBG(2,"computed frameStepFrames = %i",frameStepFrames);

  if (isSet("frameCenterSpecial")) {
    const char * fc = getStr("frameCenterSpecial");
    if (fc!=NULL) {
      frameCenterFrames = 0;
      if (!strncasecmp(fc,"mi",2)) frameCenter=frameSize/2.0;
      else if (!strncasecmp(fc,"ce",2)) frameCenter=frameSize/2.0;
      else if (!strncasecmp(fc,"le",2)) frameCenter=0.0;
      else if (!strncasecmp(fc,"ri",2)) frameCenterFrames=frameSizeFrames-1;
      else frameCenterFrames=0; // frameSizeFrames-1;
    }
    if (frameCenterFrames == 0) {
      if (c.T != 0.0) frameCenterFrames = (long)round(frameCenter / c.T);
      else frameCenterFrames = (long)round(frameCenter);
    }
  } else {
    if (isSet("frameCenterFrames")) {
      frameCenterFrames = getInt("frameCenterFrames");
      frameCenter = (frameCenterFrames * c.T); // + frameSize/2.0;
      //frameCenterFrames += frameSizeFrames/2;
    } else {
      frameCenter = getDouble("frameCenter");
      //frameCenter += frameSize/2.0;
      if (c.T != 0.0) frameCenterFrames = (long)round(frameCenter / c.T);
      else frameCenterFrames = (long)round(frameCenter);
    }
  }

  SMILE_DBG(2,"frameCenter = %f",frameCenter);
  SMILE_DBG(2,"frameCenterFrames = %i",frameCenterFrames);
  if (frameMode == FRAMEMODE_FULL) {
    pre = 0;
  } else {
    pre = -frameCenterFrames;
  }
  
  if (frameStep == 0.0) {
    if (frameMode == FRAMEMODE_FIXED) frameMode = FRAMEMODE_FULL;
  }

  if ((frameMode == FRAMEMODE_FULL)||(frameMode==FRAMEMODE_VAR)||(frameMode==FRAMEMODE_LIST)||(frameMode==FRAMEMODE_META)) {
    frameStep = 0.0;
    frameSize = 0.0;
    frameStepFrames = 0;
    frameSizeFrames = 0;
  }

  if (frameMode == FRAMEMODE_FULL) { // full input mode
    if (isSet("noPostEOIprocessing") && (noPostEOIprocessing == 1)) { 
      SMILE_IWRN(1,"default user defined option noPostEOIprocessing=1 forced to 0 due to frameStep==0.0 (full input mode)");
    }
    noPostEOIprocessing = 0; // change default...
  }

  // we adjust the buffersize proportionally 
  if ((frameStepFrames==0)||(frameSizeFrames==0)) {
    c.nT = 2;
  } else {
    c.nT /= frameStepFrames;
  }
  SMILE_DBG(2,"using buffersize = %i frames",c.nT);

  // we overwrite the reader period with the recently computed writer period...
  c.T = frameStep;
  c.frameSizeSec = frameSize;
  dtype = c.type; // INPUT type ??
  c.type = DMEM_FLOAT;

  if ((frameMode != FRAMEMODE_VAR)&&(frameMode != FRAMEMODE_META))
    reader->setupSequentialMatrixReading(frameStepFrames, frameSizeFrames, pre);

  // this is now handled by cDataProcessor myConfigureInstance
  //writer->setConfig( 1, len, frameStep, 0.0, frameSize, 0, DMEM_FLOAT);

  // we must return 1, in order to indicate configure success (0 indicates failure)
  return 1;
}


// overwrite in descendant class, return number of elements that are output for each input element
 // NOTE: this is rather a setupNamesForElement!!
int cWinToVecProcessor::setupNamesForElement(int idxi, const char*name, long nEl)
{
  if (wholeMatrixMode) {
    return setupNamesForField(idxi,name,getNoutputs(nEl));
  } else {
    return setupNamesForField(idxi,name,getMultiplier());
  }
}

// this must return the multiplier, i.e. the vector size returned for each input element (e.g. number of functionals, etc.)
int cWinToVecProcessor::getMultiplier()
{
  return 1;
}

// get the number of outputs given a number of inputs, 
// this is used for wholeMatrixMode, where the quantitative relation between inputs and outputs may be non-linear
// this replaces getMultiplier, which is used in the standard row-wise processing mode
long cWinToVecProcessor::getNoutputs(long nIn) 
{
  // some default behaviour... override this in your derived class
  return nIn;
}

// OBSOLETE
/*
int cWinToVecProcessor::myConfigureInstance()
{
  int ret = cDataProcessor::myConfigureInstance();

  return ret;
}
*/

int cWinToVecProcessor::dataProcessorCustomFinalise() 
{
  Ni = reader->getLevelN();
  Nfi = reader->getLevelNf();
  No = 0;
  //  Nfo = Ni;
  inputPeriod = reader->getLevelT();

  int i,n;

  long e=0;
  for (i=0; i<Nfi; i++) {
    int __N=0;
    const char *tmp = reader->getFieldName(i,&__N);
    if (tmp == NULL) { SMILE_ERR(1,"reader->getFieldName(%i) failed (return value = NULL)!",i); return 0; }
    if (wholeMatrixMode) {

      No += setupNamesForElement(i, tmp, 1);

    } else {
      if (__N > 1) {

        for (n=0; n<__N; n++) {
          char *na = reader->getElementName(e);
          No += setupNamesForElement(e++, na, 1);
          free(na);
        }

      } else {
        No += setupNamesForElement(e++, tmp, 1);
      }
    }
  }

  namesAreSet = 1;

  Mult = getMultiplier();
  if (Mult*Ni != No) COMP_ERR("Mult not constant (as returned by setupNamesForField! This is not allowed! Mult*Ni=%i <> No=%i",Mult*Ni,No);
  if (tmpFrameF==NULL) tmpFrameF=(FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*Mult);
  if (tmpFrameI==NULL) tmpFrameI=(INT_DMEM*)calloc(1,sizeof(INT_DMEM)*Mult);

  return 1;
}

/*
int cWinToVecProcessor::myFinaliseInstance()
{

  // get reader names, append _win(_winf) to it (winf is optional)
  int ret=1;
  ret *= reader->finaliseInstance();
  if (!ret) return ret;

  Ni = reader->getLevelN();
  Nfi = reader->getLevelNf();
  No = 0;
//  Nfo = Ni;
  inputPeriod = reader->getLevelT();

  int i,n;

  long e=0;
  for (i=0; i<Nfi; i++) {
    int __N=0;
    const char *tmp = reader->getFieldName(i,&__N);
    if (tmp == NULL) { SMILE_ERR(1,"reader->getFieldName(%i) failed (return value = NULL)!",i); return 0; }
    if (__N > 1) {

      for (n=0; n<__N; n++) {
		    char *na = reader->getElementName(e);
        No += setupNamesForField(e++, na, 1);
        free(na);
      }

    } else {
      No += setupNamesForField(e++, tmp, 1);
    }
  }

  namesAreSet = 1;
  Mult = getMultiplier();
  if (Mult*Ni != No) COMP_ERR("Mult not constant (as returned by setupNamesForField! This is not allowed! Mult*Ni=%i <> No=%i",Mult*Ni,No);
  if (tmpFrameF==NULL) tmpFrameF=(FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*Mult);
  if (tmpFrameI==NULL) tmpFrameI=(INT_DMEM*)calloc(1,sizeof(INT_DMEM)*Mult);

  if (frameMode != FRAMEMODE_VAR) 
    reader->setupSequentialMatrixReading(frameStepFrames, frameSizeFrames, pre);

  return cDataProcessor::myFinaliseInstance();
  
}
*/

int cWinToVecProcessor::doProcessMatrix(int idxi, cMatrix *in, FLOAT_DMEM *out, long nOut)
{
  SMILE_IERR(1,"doProcessMatrix (type FLOAT_DMEM) is not implemented in this component, however for some reason the 'wholeMatrixMode' variable was set to 1...!");
  return 0;
}

int cWinToVecProcessor::doProcessMatrix(int idxi, cMatrix *in, INT_DMEM *out, long nOut)
{
  SMILE_IERR(1,"doProcessMatrix (type INT_DMEM) is not implemented in this component, however for some reason the 'wholeMatrixMode' variable was set to 1...!");
  return 0;
}

// idxi is index of input element
// row is the input row
// y is the output vector (part) for the input row
int cWinToVecProcessor::doProcess(int idxi, cMatrix *row, FLOAT_DMEM*y)
{
   /* int i;
    for (i=0;i<row->nT; i++) {
      printf("[%i] %f\n",i,row->dataF[i]);
    }*/
  SMILE_IERR(1,"dataType FLOAT_DMEM not yet supported!");
  //return getMultiplier();
  return 0;
  // return the number of actually set components in y!!
  // return 0 on failue
  // return -1 if you don't want to set a new output frame...
}

int cWinToVecProcessor::doProcess(int idxi, cMatrix *row, INT_DMEM*y)
{
  SMILE_IERR(1,"dataType INT_DMEM not yet supported!");
  //return getMultiplier();
  return 0;
  // return the number of actually set components in y!!
  // return 0 on failue
  // return -1 if you don't want to set a new output frame...
}

int cWinToVecProcessor::doFlush(int idxi, FLOAT_DMEM*y)
{
  //SMILE_IERR(1,"dataType FLOAT_DMEM not yet supported!");
  //return getMultiplier();
  return 0;
  // return the number of actually set components in y!!
  // return 0 on failue
  // return -1 if you don't want to set a new output frame...
}

int cWinToVecProcessor::doFlush(int idxi, INT_DMEM*y)
{
  //SMILE_IERR(1,"dataType INT_DMEM not yet supported!");
  //return getMultiplier();
  return 0;
  // return the number of actually set components in y!!
  // return 0 on failue
  // return -1 if you don't want to set a new output frame...
}

// get data for next segment begin/end from frame message memory (if FRAMEMODE_VAR)
int cWinToVecProcessor::getNextFrameData(double *start, double *end, int *flag)
{
  if ((start!=NULL)&&(end!=NULL)) {
    lockMessageMemory();
    if (nQ > 0) {
      *start = Qstart[0];
      *end = Qend[0];
      if (flag != NULL) *flag = Qflag[0];
      int i;
      for (i=0; i<nQ-1; i++) {
        Qstart[i] = Qstart[i+1];
        Qend[i] = Qend[i+1];
        Qflag[i] = Qflag[i+1];
      }
      nQ--;
      unlockMessageMemory();
      return 1;
    } else {
      *start = -1.0;
      *end = -1.0;
      if (flag != NULL) *flag = -1;
      unlockMessageMemory();
      return 0;
    }
  }
  return 0;
}

int cWinToVecProcessor::peakNextFrameData(double *start, double *end, int *flag)
{
  if ((start!=NULL)&&(end!=NULL)) {
    lockMessageMemory();
    if (nQ > 0) {
      *start = Qstart[0];
      *end = Qend[0];
      if (flag != NULL) *flag = Qflag[0];
      unlockMessageMemory();
      return 1;
    } else {
      *start = -1.0;
      *end = -1.0;
      if (flag != NULL) *flag = -1;
      unlockMessageMemory();
      return 0;
    }
  }
  return 0;
}

int cWinToVecProcessor::clearNextFrameData()
{
  lockMessageMemory();
  if (nQ > 0) {
    int i;
    for (i=0; i<nQ-1; i++) {
      Qstart[i] = Qstart[i+1];
      Qend[i] = Qend[i+1];
      Qflag[i] = Qflag[i+1];
    }
    nQ--;
    unlockMessageMemory();
    return 1;
  }
  unlockMessageMemory();
  return 0;
}

int cWinToVecProcessor::queNextFrameData(double start, double end, int flag)
{
  if (nQ < FRAME_MSG_QUE_SIZE) {
    Qstart[nQ] = start;
    Qend[nQ] = end;
    Qflag[nQ++] = flag;
    return 1;
  }
  return 0;
}

int cWinToVecProcessor::processComponentMessage( cComponentMessage *_msg )
{
  if (isMessageType(_msg,"turnFrameTime")) {
    return queNextFrameData(_msg->floatData[0], _msg->floatData[1], _msg->intData[0]);
  }
  return 0;
}

void cWinToVecProcessor::addVecToBuf(cVector *ve) 
{
  // add the current vector to the buffer...
  if (matBuf == NULL) {
    matBuf = new cMatrix(ve->N, MATBUF_ALLOC_STEP);
  }
  if (matBuf->nT <= matBufN) {
    matBuf->resize(matBufN + MATBUF_ALLOC_STEP);
  }
  long i;
  for (i=0; i<ve->N; i++) {
    matBuf->dataF[i+matBufN*matBuf->N] = ve->dataF[i];
  }
  matBufN++;
}

int cWinToVecProcessor::myTick(long long t)
{
  SMILE_IDBG(4,"tick # %i, running winToVecProcessor ....",t);
  
  if ((isEOI())&&(noPostEOIprocessing)) {
    if ((noPostEOIprocessing)&&(frameMode == FRAMEMODE_FULL)) {
      SMILE_IWRN(1,"frameMode=full, or frameStep==0: this means processing of full input at End-Of-Input is enabled. However, noPostEOIProcessing is set to 'yes' (1) . This means that no output will be produced!! To solve this, set noPostEOIProcessing=0 in section [%s:%s]",getInstName(),getTypeName());
    }
    return 0;
  }

  if (!(writer->checkWrite(1))) return 0;

  cMatrix *mat=NULL;
  cVector * vec=NULL;
  int isFinal = 0;
  // get next frame from dataMemory
  if (frameMode == FRAMEMODE_META) { 
    vec = reader->getNextFrame();
    if (vec == NULL) return 0;
    // check for arff instance id in vector meta data!
    if ((vec->tmeta->metadata.iData[1] != 1234)||(vec->tmeta->metadata.text == NULL)) {
      SMILE_IERR(1,"using frameMode=meta requires instance ID as string in vector meta data (see the saveInstanceIdInMetadata option in cArffSource!)");
    } else {


      //SMILE_IMSG(1,"metadata.custom = '%s'",vec->tmeta->metadata.custom);
      // check meta info, add frames to buffer... flush if meta info text changes
      if ((lastText!=NULL)&&(strcmp(lastText,vec->tmeta->metadata.text))) {
        // if the strings differ... we generate a new segment
        if (matBuf != NULL) {
          mat = matBuf;
          matBufNalloc = matBuf->nT;
          matBuf->nT = matBufN;
        }
      } else {
        if (lastText == NULL) {
          // save old metadata.text / custom
          lastText = strdup(vec->tmeta->metadata.text);
          if (lastCustom != NULL) free(lastCustom);
          lastCustom = strdup((char*)(vec->tmeta->metadata.custom));
        }
        // add new frame to buffer
        addVecToBuf(vec);
        // no new segment...
        return 0;
      }

    }
  
  } else if (frameMode == FRAMEMODE_VAR) { // OR frameMode == list!

    double start = 0.0;
    double end = 0.0;
    if (peakNextFrameData(&start, &end, &isFinal)) { // TODO: add turn end flag here (isFinal)
      
      SMILE_IDBG(3,"getting frame based on received message: vIdx %i - %i",(long)start - 1, (long)end + 1);
      mat = reader->getMatrix((long)start - 1,(long)(end-start) + 2);
      if (mat != NULL) {
        SMILE_IDBG(2,"successfully got frame based on received message: vIdx %i - %i (nT:%i)",(long)start-1, (long)end+1, mat->nT);
        clearNextFrameData();
      }
    } else {
      return 0;
    }
  } else {
    mat = reader->getNextMatrix();
//mat = NULL;
  }

  if ((mat == NULL)&&(!isEOI())) { return 0; } // currently no data available

  int type;
  if (mat==NULL) {
    const sDmLevelConfig * c = writer->getLevelConfig();
    if (c!=NULL) type = c->type;
    else return 0;
  } else { type=mat->type; }

  if (tmpVec==NULL) tmpVec = new cVector(No,type);
  
  if (frameMode == FRAMEMODE_META) { 
    // set tmeta correctly in tmpVec...      
    if (tmpVec->tmeta->metadata.text != NULL) free(tmpVec->tmeta->metadata.text);
    tmpVec->tmeta->metadata.text = strdup(lastText);
    if (tmpVec->tmeta->metadata.custom != NULL) free(tmpVec->tmeta->metadata.custom);
    tmpVec->tmeta->metadata.custom = strdup(lastCustom);
    tmpVec->tmeta->metadata.customLength = (long)(strlen(lastCustom)+1);
    tmpVec->tmeta->metadata.iData[1] = 1234;
    //TODO: proper tmeta squashing and cloning for correct time-stamps!
    // (problem: after squashing, we must restore to original state for new data in buffer...?)

    // next...
    if (lastText != NULL) free(lastText);
    lastText = strdup(vec->tmeta->metadata.text);
    if (lastCustom != NULL) free(lastCustom);
    lastCustom = strdup((char*)(vec->tmeta->metadata.custom));
  }

  //if ((tmpRow==NULL)&&(mat!=NULL)) tmpRow = mat->getRow(0);

//  printf("vs=%i Nf=%i nn=%i\n",tmpVec->N,Nf,nNotes);
  int i,toSet=1,ret=1;
  if (type == DMEM_FLOAT) {
    if (wholeMatrixMode) { // call the function to process the whole data matrix, producing a single output vector

      if (processFieldsInMatrixMode) {
        FLOAT_DMEM * opVec = tmpVec->dataF;
        for(i=0; i<Nfi; i++) {
          long nout = getNoutputs(mat->fmeta->field[i].N);
          doProcessMatrix(i,mat,opVec,nout);
          opVec += nout;
        }
      } else { 
        doProcessMatrix(0,mat,tmpVec->dataF,tmpVec->N);
      }

    } else { // process each row independently and concattenate output vectors

      for (i=0; i<Ni; i++) {
        long Mu;
        //cMatrix *r=NULL;
        if (mat!=NULL) {
          tmpRow = mat->getRow(i,tmpRow);
          Mu = doProcess(i,tmpRow,tmpFrameF);
        } else {
          Mu = doFlush(i,tmpFrameF);
        }
        if ((Mu > 0)&&(toSet==1)) {
          // copy data into main vector
          Mu = MIN(Mu,Mult);
          memcpy( tmpVec->dataF+i*Mult, tmpFrameF, sizeof(FLOAT_DMEM)*Mu ); // was: *Mult
          if (Mu<Mult)
            memset( tmpVec->dataF+i*Mult+Mu, 0, sizeof(FLOAT_DMEM)*(Mult-Mu) );
        } else { 
          toSet=0;
          if (Mu==0) {
            ret=0;
          }
        }
        //if (r!=NULL) delete r;
      }
    }
  } else if (mat->type == DMEM_INT) {
    if (wholeMatrixMode) { // call the function to process the whole data matrix, producing a single output vector
      if (processFieldsInMatrixMode) {
        for(i=0; i<Nfi; i++) {
          //doProcessMatrix(i,mat,tmpVec); // TODO!
        }
      } else { 
        doProcessMatrix(0,mat,tmpVec->dataI, tmpVec->N);
      }

    } else { // process each row independently and concattenate output vectors


      for (i=0; i<Ni; i++) {
        // TODO: return value... toSet, etc...
        //!!!!!!!!!!!!
        //cMatrix *r = mat->getRow(i);
        doProcess(i,tmpRow,tmpFrameI);
        // copy data into main vector
        memcpy( tmpVec->dataI+i*Mult, tmpFrameI, sizeof(INT_DMEM)*Mult );
      }
    }
  } else { SMILE_ERR(1,"unknown dataType %i (inst: '%s')!!",mat->type,getInstName()); return 0; }
  
  if (toSet == 1) {
    if (frameMode == FRAMEMODE_META) { 
      mat->nT = matBufNalloc;
      matBufN = 0; // (the vector we just added)
      addVecToBuf(vec);
    } else {
    // generate new tmeta from first and last tmeta
    if (mat != NULL) {
      mat->tmetaSquash();
      if (frameCenterFrames>0) {
        mat->tmeta[0].time += frameCenter;
      }
      tmpVec->tmetaReplace(mat->tmeta);  // tmetaClone ??
    } else {
      // ugly TODO: compute correct tmeta...
    }
    }
    tmpVec->tmeta->metadata.ID = 0;
    tmpVec->tmeta->metadata.iData[0] = isFinal;
 
    // save to dataMemory
    writer->setNextFrame(tmpVec);
  }

  return ret;
}


cWinToVecProcessor::~cWinToVecProcessor()
{
  if (tmpFrameF!=NULL) free(tmpFrameF);
  if (tmpFrameI!=NULL) free(tmpFrameI);
  if (ivSec != NULL) free(ivSec);
  if (ivFrames != NULL) free(ivFrames);
  if (tmpVec!=NULL) delete tmpVec;
  if (tmpRow != NULL) delete tmpRow;
  if (matBuf != NULL) delete matBuf;
  if (lastText != NULL) free(lastText);
  if (lastCustom != NULL) free(lastCustom);
}

