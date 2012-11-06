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

OBSOLETE component!!!

data windower.
takes data from one element of one level and creates "frames",
also applies window function

*/


#include <math.h>
#include <windowframer.hpp>

#define MODULE "cWindowFramer"

/*Library:
sComponentInfo * registerMe(cConfigManager *_confman) {
  cDataProcessor::registerComponent(_confman);
}
*/


SMILECOMPONENT_STATICS(cWindowFramer)

SMILECOMPONENT_REGCOMP(cWindowFramer)
//sComponentInfo * cWindowFramer::registerComponent(cConfigManager *_confman)
{
  SMILECOMPONENT_REGCOMP_INIT
/*
  if (_confman == NULL) return NULL;
  int rA = 0;
  sconfman = _confman;
*/
  scname = COMPONENT_NAME_CWINDOWFRAMER;
  sdescription = COMPONENT_DESCRIPTION_CWINDOWFRAMER;

  // we inherit cWindower configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cWindower");
/*
  ConfigType *ct = new ConfigType( *(sconfman->getTypeObj("cWindower")) , scname );
  if (ct == NULL) {
    SMILE_WRN(4,"cWindower config Type not found!");
    rA=1;
  }*/
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("frameSize","frame size in seconds",0.025);
    ct->setField("frameStep","frame step (period) in seconds",0.010);
    ct->setField("frameSizeFrames","frame size in frames (overrides frameSize, if set and > 0)",0);
    ct->setField("frameStepFrames","frame step in frames (overrides frameSize, if set and > 0)",0);
  )
/*
    ct->setField("gain","gain by which samples should be multiplied",1.0);
    ct->setField("offset","offset to add to samples",0.0);
    ct->setField("winFunc","window function:\n   Hann [Han],\n   Hamming [Ham],\n   Rectangular [Rec],\n   Gauss [Gau],\n   Sine / Cosine [Sin],\n   Triangular [Tri],\n   Bartlett [Bar],\n   Bartlett-Hann [BaH],\n   Blackmann [Bla],\n   Blackmann-Harris [BlH],\n   Lanczos [Lac]","Han");
    ct->setField("sigma","stddev for Gauss window",0.4);
    ct->setField("alpha0","alpha0 for Blackmann(-Harris) / Bartlett-Hann windows (optional!)",0.0);
    ct->setField("alpha1","alpha1 for Blackmann(-Harris) / Bartlett-Hann windows (optional!)",0.0);
    ct->setField("alpha2","alpha2 for Blackmann(-Harris) / Bartlett-Hann windows (optional!)",0.0);
    ct->setField("alpha3","alpha2 for Blackmann-Harris window (optional!)",0.0);
    ct->setField("alpha","alpha for Blackmann windows",0.16);
*/

  /*
    ConfigInstance *Tdflt = new ConfigInstance( scname, ct, 1 );
    sconfman->registerType(Tdflt);
  } */

  SMILECOMPONENT_MAKEINFO(cWindowFramer);
}

SMILECOMPONENT_CREATE(cWindowFramer)

//-----

cWindowFramer::cWindowFramer(const char *_name) :
  cDataProcessor(_name),
  fsfGiven(0),
  fstfGiven(0),
  frameSizeFrames(0),
  frameStepFrames(0),
  frameSize(0.0),
  frameStep(0.0),
  dtype(0),
  win(NULL)
{

}

void cWindowFramer::fetchConfig()
{
  cDataProcessor::fetchConfig();

  if (isSet("frameSizeFrames")) {
    frameSizeFrames = getInt("frameSizeFrames");
    fsfGiven = 1;
    SMILE_DBG(3,"frameSizeFrames = %i",frameSizeFrames);
  } else {
    frameSize = getDouble("frameSize");
    SMILE_DBG(3,"frameSize = %f s",frameSize);
  }
  if (isSet("frameStepFrames")) {
    frameStepFrames = getInt("frameStepFrames");
    fstfGiven = 1;
    SMILE_DBG(3,"frameStepFrames = %i",frameStepFrames);
  } else {
    frameStep = getDouble("frameStep");
    SMILE_DBG(3,"frameStep = %f s",frameStep);
  }
  offset=getDouble("offset");
  gain=getDouble("gain");

  const char *winF = getStr("winFunc");
  if ((!strcmp(winF,"Han"))||(!strcmp(winF,"han"))||(!strcmp(winF,"Hanning"))||(!strcmp(winF,"hanning"))||(!strcmp(winF,"hann"))||(!strcmp(winF,"Hann")))
    { winFunc = WINF_HANNING; }
  else if ((!strcmp(winF,"Ham"))||(!strcmp(winF,"ham"))||(!strcmp(winF,"Hamming"))||(!strcmp(winF,"hamming")))
    { winFunc = WINF_HAMMING; }
  else if ((!strcmp(winF,"Rec"))||(!strcmp(winF,"rec"))||(!strcmp(winF,"Rectangular"))||(!strcmp(winF,"rectangular"))||(!strcmp(winF,"none"))||(!strcmp(winF,"None")))
    { winFunc = WINF_RECTANGLE; }
  else if ((!strcmp(winF,"Gau"))||(!strcmp(winF,"gau"))||(!strcmp(winF,"Gauss"))||(!strcmp(winF,"gauss"))||(!strcmp(winF,"Gaussian"))||(!strcmp(winF,"gaussian")))
    { winFunc = WINF_GAUSS; }
  else if ((!strcmp(winF,"Sin"))||(!strcmp(winF,"sin"))||(!strcmp(winF,"Sine"))||(!strcmp(winF,"sine"))||(!strcmp(winF,"cosine"))||(!strcmp(winF,"Cosine"))||(!strcmp(winF,"Cos"))||(!strcmp(winF,"cos")))
    { winFunc = WINF_SINE; }
  else if ((!strcmp(winF,"Tri"))||(!strcmp(winF,"tri"))||(!strcmp(winF,"Triangle"))||(!strcmp(winF,"triangle")))
    { winFunc = WINF_TRIANGLE; }
  else if ((!strcmp(winF,"Bla"))||(!strcmp(winF,"bla"))||(!strcmp(winF,"Blackman"))||(!strcmp(winF,"blackman")))
    { winFunc = WINF_BLACKMAN; }
  else if ((!strcmp(winF,"BlH"))||(!strcmp(winF,"blh"))||(!strcmp(winF,"Blackman-Harris"))||(!strcmp(winF,"blackman-harris")))
    { winFunc = WINF_BLACKHARR; }
  else if ((!strcmp(winF,"Bar"))||(!strcmp(winF,"bar"))||(!strcmp(winF,"Bartlett"))||(!strcmp(winF,"bartlett")))
    { winFunc = WINF_BARTLETT; }
  else if ((!strcmp(winF,"BaH"))||(!strcmp(winF,"bah"))||(!strcmp(winF,"Bartlett-Hann"))||(!strcmp(winF,"bartlett-hann"))||(!strcmp(winF,"Bartlett-Hanning"))||(!strcmp(winF,"bartlett-hanning")))
    { winFunc = WINF_BARTHANN; }
  else if ((!strcmp(winF,"Lac"))||(!strcmp(winF,"lac"))||(!strcmp(winF,"Lanczos"))||(!strcmp(winF,"lanczos")))
    { winFunc = WINF_LANCZOS; }
  else {
    SMILE_ERR(1,"unkown window function '%s' specified in config file! setting window function to 'rectangular' (none)!",winF);
    winFunc = WINF_RECTANGLE;
  }
  
  if (winFunc == WINF_GAUSS) sigma = getDouble("sigma");
  
  if (winFunc == WINF_BLACKMAN) {
    if (isSet("alpha0") && isSet("alpha1") && isSet("alpha2")) {
      alpha0 = getDouble("alpha0");
      alpha1 = getDouble("alpha1");
      alpha2 = getDouble("alpha2");
    } else {
      alpha = getDouble("alpha");
      alpha0 = (1.0-alpha)*0.5;
      alpha1 = 0.5;
      alpha2 = alpha*0.5;
    }
  }
  if (winFunc == WINF_BLACKHARR) {
    if (isSet("alpha0")) alpha0 = getDouble("alpha0");
    else alpha0 = 0.35875;
    if (isSet("alpha1")) alpha1 = getDouble("alpha1");
    else alpha1 = 0.48829;
    if (isSet("alpha2")) alpha2 = getDouble("alpha2");
    else alpha2 = 0.14128;
    if (isSet("alpha3")) alpha3 = getDouble("alpha3");
    else alpha3 = 0.01168;
  }
  if (winFunc == WINF_BARTHANN) {
    if (isSet("alpha0")) alpha0 = getDouble("alpha0");
    else alpha0 = 0.62;
    if (isSet("alpha1")) alpha1 = getDouble("alpha1");
    else alpha1 = 0.48;
    if (isSet("alpha2")) alpha2 = getDouble("alpha2");
    else alpha2 = 0.38;
  }

}


int cWindowFramer::configureWriter(const sDmLevelConfig *c)
{
  SMILE_DBG(3,"reader period = %f",c->T);
  if (!fsfGiven) frameSizeFrames = (long)round(frameSize / c->T);
  else frameSize = ((double)frameSizeFrames) * c->T;
  if (!fstfGiven) frameStepFrames = (long)round(frameStep / c->T);
  else frameStep = ((double)frameStepFrames) * c->T;
  SMILE_DBG(4,"computed frameSizeFrames = %i",frameSizeFrames);
  SMILE_DBG(4,"computed frameStepFrames = %i",frameStepFrames);

  long len = c->nT / frameStepFrames;
  if (len < 2) len = 2;
// TODO: overwrite only frameStep
  writer->setConfig( 1, len, frameStep, 0.0, frameSize, 0, DMEM_FLOAT);

// TODO: overwrite only frameStep
//  writer->setConfig( 1, c->nT / frameStepFrames, frameStep, 0.0, 0, DMEM_FLOAT);

  dtype = c->type;
  // you must return 1, in order to indicate configure success (0 indicated failure)
  return 1;
}


int cWindowFramer::myFinaliseInstance()
{
  // get reader names, append _win(_winf) to it (winf is optional)

  int _Nf = reader->getLevelNf();
  int i,n;
  for (i=0; i<_Nf; i++) {
    int __N=0;
    const char *tmp = reader->getFieldName(i,&__N);
    if (tmp == NULL) { SMILE_ERR(1,"reader->getFieldName(%i) failed (return value = NULL)!",i); return 0; }
    if (__N > 1) {
      for (n=0; n<__N; n++) {
        char *xx = myvprint("%s%i_win",tmp,n);  // TODO: append name of winFunc, if option in config file is set
        writer->addField( xx, frameSizeFrames );
        free(xx);
      }
    } else {
      char *xx = myvprint("%s_win",tmp);  // TODO: append name of winFunc, if option in config file is set
      writer->addField( xx, frameSizeFrames );
      free(xx);
    }
  }
  namesAreSet = 1;

  precomputeWinFunc();
  
  reader->setupSequentialMatrixReading(frameStepFrames, frameSizeFrames);
  return cDataProcessor::myFinaliseInstance();
}


void cWindowFramer::precomputeWinFunc()
{
  if (!isConfigured()) return;
  long i;
  if (win != NULL) free(win);
  switch(winFunc) {
    case WINF_RECTANGLE: win = smileDsp_winRec(frameSizeFrames); break;
    case WINF_HANNING:   win = smileDsp_winHan(frameSizeFrames); break;
    case WINF_HAMMING:   win = smileDsp_winHam(frameSizeFrames); break;
    case WINF_TRIANGLE:  win = smileDsp_winTri(frameSizeFrames); break;
    case WINF_BARTLETT:  win = smileDsp_winBar(frameSizeFrames); break;
    case WINF_SINE:      win = smileDsp_winSin(frameSizeFrames); break;
    case WINF_GAUSS:     win = smileDsp_winGau(frameSizeFrames,sigma); break;
    case WINF_BLACKMAN:  win = smileDsp_winBla(frameSizeFrames,alpha0,alpha1,alpha2); break;
    case WINF_BLACKHARR: win = smileDsp_winBlH(frameSizeFrames,alpha0,alpha1,alpha2,alpha3); break;
    case WINF_BARTHANN:  win = smileDsp_winBaH(frameSizeFrames,alpha0,alpha1,alpha2); break;
    case WINF_LANCZOS:   win = smileDsp_winLac(frameSizeFrames); break;
    default: SMILE_ERR(1,"unknown window function ID (%i) !",winFunc); win=NULL;
  }
  if ((win != NULL)&&(gain!=1.0)) { // apply gain
    for (i=0; i<frameSizeFrames; i++) {
      win[i] *= gain;
    }
  }
  
}

int cWindowFramer::myTick(long long t)
{
  SMILE_DBG(4,"tick # %i, processing ....",t);

  if (!(writer->checkWrite(1))) return 0;

  // get next frame from dataMemory
  cMatrix *mat = reader->getNextMatrix();
  if (mat == NULL) { return 0; } // currently no data available

  // transpose mat
  mat->transpose();

  // now apply winF to each component (now column...) of mat..
  
  if (win != NULL) {
  if ((winFunc != WINF_RECTANGLE)||(offset!=0.0)||(gain!=1.0)) {

  long i,n;
  FLOAT_DMEM *o;
  INT_DMEM   *oi;
  double *w;
  
  if (dtype == DMEM_FLOAT) {
    for (i=0; i<mat->nT; i++) {
      o = mat->dataF + (i*mat->N);
      w = win;
      for (n=0; n<mat->N; n++) { *o = *o * (FLOAT_DMEM)(*(w++)) + (FLOAT_DMEM)offset; o++; }
//      for (n=0; n<mat->N; n++) { *o = (FLOAT_DMEM)(*(w++)); o++; }
    }
  } else if (dtype == DMEM_INT) {
    for (i=0; i<mat->nT; i++) {
      oi = mat->dataI + (i*mat->N);
      w = win;
      for (n=0; n<mat->N; n++) { *oi = (int) round( (double)(*oi) * (*(w++)) + offset ); oi++; }
    }
  }
  
  } // else frame copy only, no window function or offset/gain etc.
  } else {SMILE_WRN(2,"win == NULL");}

  //  then run reshape() to convert mat to vector with 1 column
  const cVector *vec = mat->expand();

  // save to dataMemory
  writer->setNextFrame(vec);
  
  return 1;
}


cWindowFramer::~cWindowFramer()
{
  if (win!=NULL) free(win);
}

