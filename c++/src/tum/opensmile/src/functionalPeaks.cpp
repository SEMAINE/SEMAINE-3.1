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

functionals: number of peaks and various measures associated with peaks

*/


#include <functionalPeaks.hpp>

#define MODULE "cFunctionalPeaks"


#define FUNCT_NUMPEAKS         0  // number of peaks
#define FUNCT_MEANPEAKDIST     1  // mean distance between peaks
#define FUNCT_PEAKMEAN         2  // arithmetic mean of peaks
#define FUNCT_PEAKMEANMEANDIST 3  // aritmetic mean of peaks - aritmetic mean

#define N_FUNCTS  4

#define NAMES     "numPeaks","meanPeakDist","peakMean","peakMeanMeanDist"

const char *peaksNames[] = {NAMES};  

SMILECOMPONENT_STATICS(cFunctionalPeaks)

SMILECOMPONENT_REGCOMP(cFunctionalPeaks)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CFUNCTIONALPEAKS;
  sdescription = COMPONENT_DESCRIPTION_CFUNCTIONALPEAKS;

  // configure your component's configType:
  SMILECOMPONENT_CREATE_CONFIGTYPE
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("numPeaks","1/0=enable/disable output of number of peaks [output name: numPeaks]",1);
    ct->setField("meanPeakDist","1/0=enable/disable output of mean distance between peaks (relative to the input segment length, in seconds, or in frames, see the 'norm' option or the 'masterTimeNorm' option of the cFunctionals parent component) [output name: meanPeakDist]",1);
    ct->setField("peakMean","1/0=enable/disable output of arithmetic mean of peaks [output name: peakMean]",1);
    ct->setField("peakMeanMeanDist","1/0=enable/disable output of aritmetic mean of peaks - aritmetic mean of all values [output name: peakMeanMeanDist]",1);
    ct->setField("norm","This option specifies how this component should normalise times (if it generates output values related to durations): \n   'segment' (or: 'turn') : normalise to the range 0..1, the result is the relative length wrt. to the segment length )\n   'second'  (absolute time in seconds) \n   'frame' (absolute time in number of frames of input level)","frames");
    ct->setField("overlapFlag","1/0=yes/no frames overlap (i.e. compute peaks locally only)",1,0,0);
    //TODO: unified time norm handling for all functionals components: frame, seconds, turn !
  )

  SMILECOMPONENT_MAKEINFO_NODMEM(cFunctionalPeaks);
}

SMILECOMPONENT_CREATE(cFunctionalPeaks)

//-----

cFunctionalPeaks::cFunctionalPeaks(const char *_name) :
  cFunctionalComponent(_name,N_FUNCTS,peaksNames),
  lastlastVal(0.0),
  lastVal(0.0),
  overlapFlag(0)
{
}

void cFunctionalPeaks::fetchConfig()
{
  parseTimeNormOption();

  if (getInt("numPeaks")) enab[FUNCT_NUMPEAKS] = 1;
  if (getInt("meanPeakDist")) enab[FUNCT_MEANPEAKDIST] = 1;
  if (getInt("peakMean")) enab[FUNCT_PEAKMEAN] = 1;
  if (getInt("peakMeanMeanDist")) enab[FUNCT_PEAKMEANMEANDIST] = 1;
  
  overlapFlag = getInt("overlapFlag");

  cFunctionalComponent::fetchConfig();
}

long cFunctionalPeaks::process(FLOAT_DMEM *in, FLOAT_DMEM *inSorted, FLOAT_DMEM *out, long Nin, long Nout)
{
  int i;
  if ((Nin>0)&&(out!=NULL)) {
    FLOAT_DMEM max = *in;
    FLOAT_DMEM min = *in;
    FLOAT_DMEM mean = *in;

    FLOAT_DMEM peakDist = (FLOAT_DMEM)0.0;
    long nPeakDist = 0;
    FLOAT_DMEM peakMean = (FLOAT_DMEM)0.0;
    long nPeaks = 0;

    FLOAT_DMEM lastMin=(FLOAT_DMEM)0.0; // in[0];
    FLOAT_DMEM lastMax=(FLOAT_DMEM)0.0; //in[0];
    long curmaxPos=0, lastmaxPos=-1;

    int peakflag = 0;  

    // range, min, max, mean
    for (i=1; i<Nin; i++) {
      if (in[i]<min) min=in[i];
      if (in[i]>max) max=in[i];
      mean += in[i];
    }
    mean /= (FLOAT_DMEM)Nin;
    FLOAT_DMEM range = max-min;

    if (overlapFlag) i=2;
    else i=0;

    if (overlapFlag) {
      lastlastVal=in[0];
      lastVal=in[1];
    }

    // advanced peak detection
    for (; i<Nin; i++) {
      // first, find ALL peaks:
      if ((lastlastVal < lastVal)&&(lastVal > in[i])) { // max
        if (!peakflag) lastMax = in[i];
        else { if (in[i] > lastMax) { lastMax = in[i]; curmaxPos = i; } }

        if (lastMax - lastMin > 0.11*range) { 
          peakflag = 1; curmaxPos = i;
        }

      } else {
        if ((lastlastVal > lastVal)&&(lastVal < in[i])) { // min
          lastMin = in[i];
        } 
      }

      // detect peak only, if x[i] falls below lastmax - 0.09*range
      if ((peakflag)&&( (in[i] < lastMax-0.09*range) || (i==Nin-1)) ) {
        nPeaks++;
        peakMean += lastMax;
        if (lastmaxPos >= 0) {
          peakDist += (FLOAT_DMEM)(curmaxPos-lastmaxPos);
          nPeakDist++;
        }
        lastmaxPos = curmaxPos;
        peakflag = 0;
      }

      lastlastVal = lastVal;
      lastVal = in[i];
    }


    int n=0;
    if (enab[FUNCT_NUMPEAKS]) out[n++]=(FLOAT_DMEM)nPeaks;

    if (nPeakDist > 0.0) {
      peakDist /= (FLOAT_DMEM)nPeakDist;
    } else {
      peakDist = (FLOAT_DMEM)(Nin+1);
    }
    if (timeNorm==TIMENORM_SECONDS) {
      peakDist *= (FLOAT_DMEM)getInputPeriod();
    } else if (timeNorm==TIMENORM_SEGMENT) {
      peakDist /= (FLOAT_DMEM)Nin;
    }
    if (enab[FUNCT_MEANPEAKDIST]) out[n++]=peakDist;

    if (nPeaks > 0.0) {
      peakMean /= (FLOAT_DMEM)nPeaks;
    } else {
      peakMean = (FLOAT_DMEM)0.0;
    }
    if (enab[FUNCT_PEAKMEAN]) out[n++] = peakMean;

    if (enab[FUNCT_PEAKMEANMEANDIST]) out[n++]=peakMean-mean;

    return n;
  }
  return 0;
}

/*
long cFunctionalPeaks::process(INT_DMEM *in, INT_DMEM *inSorted, INT_DMEM *out, long Nin, long Nout)
{

  return 0;
}
*/

cFunctionalPeaks::~cFunctionalPeaks()
{
}

