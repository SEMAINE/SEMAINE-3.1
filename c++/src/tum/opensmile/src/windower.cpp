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

data windower.
takes frames from one level, applies window function and saves to other level

*/


#include <windower.hpp>
#include <math.h>

#define MODULE "cWindower"



SMILECOMPONENT_STATICS(cWindower)

SMILECOMPONENT_REGCOMP(cWindower)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CWINDOWER;
  sdescription = COMPONENT_DESCRIPTION_CWINDOWER;

  // we inherit cVectorProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cVectorProcessor")
  
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("gain","This option allows you to specify a scaling factor by which window function (which is by default normalised to max. 1) should be multiplied by ",1.0);
    ct->setField("offset","This specifies an offset which will be added to the samples after multiplying with the window function",0.0);
    ct->setField("winFunc","Window function:\n   Hann [Han] (= raised cosine window; use this, if you want to resynthesis from the spectral domain, also use 50% overlap in the framer!),\n   Hamming [Ham],\n   Rectangular [Rec],\n   Gauss [Gau],\n   Sine / Cosine [Sin],\n   Triangular [Tri],\n   Bartlett [Bar],\n   Bartlett-Hann [BaH],\n   Blackmann [Bla],\n   Blackmann-Harris [BlH],\n   Lanczos [Lac]","Han");
    ct->setField("sigma","Standard deviation for the Gaussian window",0.4);
    ct->setField("alpha0","alpha0 for Blackmann(-Harris) / Bartlett-Hann windows (optional!)",0.0,0,0);
    ct->setField("alpha1","alpha1 for Blackmann(-Harris) / Bartlett-Hann windows (optional!)",0.0,0,0);
    ct->setField("alpha2","alpha2 for Blackmann(-Harris) / Bartlett-Hann windows (optional!)",0.0,0,0);
    ct->setField("alpha3","alpha3 for Blackmann-Harris window (optional!)",0.0,0,0);
    ct->setField("alpha","alpha for the Blackmann window",0.16);
  )

  SMILECOMPONENT_MAKEINFO(cWindower);
}

SMILECOMPONENT_CREATE(cWindower)

//-----

cWindower::cWindower(const char *_name) :
  cVectorProcessor(_name),
  dtype(0),
  win(NULL)
{

}

void cWindower::fetchConfig()
{
  cVectorProcessor::fetchConfig();

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


int cWindower::myFinaliseInstance()
{
  int ret = cVectorProcessor::myFinaliseInstance();

  if (ret) {
    frameSizeFrames = reader->getLevelN();
    precomputeWinFunc();
  }

  return ret;
}


void cWindower::precomputeWinFunc()
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

int cWindower::processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi)
{
  if (win != NULL) {
    double *w = win;
    long n;
    for (n=0; n<Ndst; n++) { *dst = (int) round( (double)(*src) * (*(w++)) + offset ); src++; dst++; }
  }
  return 1;
}

// a derived class should override this method, in order to implement the actual processing
int cWindower::processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  if (win != NULL) {
    double *w = win;
    long n;
    for (n=0; n<Ndst; n++) { *dst = *src * (FLOAT_DMEM)(*(w++)) + (FLOAT_DMEM)offset; src++; dst++; }
  }
  return 1;
}


cWindower::~cWindower()
{
  if (win!=NULL) free(win);
}

