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

functionals:
various means, arithmetic, geometric, quadratic, etc.
also number of non-zero values, etc.

*/


#include <functionalMeans.hpp>
#include <math.h>

#define MODULE "cFunctionalMeans"


#define FUNCT_AMEAN     0
#define FUNCT_ABSMEAN   1
#define FUNCT_QMEAN     2
#define FUNCT_NZAMEAN   3
#define FUNCT_NZABSMEAN 4
#define FUNCT_NZQMEAN   5
#define FUNCT_NZGMEAN   6
#define FUNCT_NNZ       7  // number of Non-Zero elements

#define N_FUNCTS  8

#define NAMES     "amean","absmean","qmean","nzamean","nzabsmean","nzqmean","nzgmean","nnz"

const char *meansNames[] = {NAMES};  // change variable name to your clas...

SMILECOMPONENT_STATICS(cFunctionalMeans)

SMILECOMPONENT_REGCOMP(cFunctionalMeans)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CFUNCTIONALMEANS;
  sdescription = COMPONENT_DESCRIPTION_CFUNCTIONALMEANS;

  // configure your component's configType:
  SMILECOMPONENT_CREATE_CONFIGTYPE

  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("amean","1/0=enable/disable output of arithmetic mean",1);
    ct->setField("absmean","1/0=enable/disable output of arithmetic mean of absolute values",1);
    ct->setField("qmean","1/0=enable/disable output of quadratic mean",1);
    ct->setField("nzamean","1/0=enable/disable output of arithmetic mean (of non-zero values only)",1);
    ct->setField("nzabsmean","1/0=enable/disable output of arithmetic mean of absolute values (of non-zero values only)",1);
    ct->setField("nzqmean","1/0=enable/disable output of quadratic mean (of non-zero values only)",1);
    ct->setField("nzgmean","1/0=enable/disable output of geometric mean (of absolute values of non-zero values only)",1);
    ct->setField("nnz","1/0=enable/disable output of number of non-zero values (relative to the input segment length, in seconds, or in frames, see the 'norm' option or the 'masterTimeNorm' option of the cFunctionals parent component)",1); // default norm for compatibility : frames
    ct->setField("norm","This option specifies how this component should normalise times (if it generates output values related to durations): \n   'segment' (or: 'turn') : normalise to the range 0..1, the result is the relative length wrt. to the segment length )\n   'second'  (absolute time in seconds) \n   'frame' (absolute time in number of frames of input level)","frames");
  )

  SMILECOMPONENT_MAKEINFO_NODMEM(cFunctionalMeans);
}

SMILECOMPONENT_CREATE(cFunctionalMeans)

//-----

cFunctionalMeans::cFunctionalMeans(const char *_name) :
  cFunctionalComponent(_name,N_FUNCTS,meansNames)
{
}

void cFunctionalMeans::fetchConfig()
{
  parseTimeNormOption();

  if (getInt("amean")) enab[FUNCT_AMEAN] = 1;
  if (getInt("absmean")) enab[FUNCT_ABSMEAN] = 1;
  if (getInt("qmean")) enab[FUNCT_QMEAN] = 1;
  if (getInt("nzamean")) enab[FUNCT_NZAMEAN] = 1;
  if (getInt("nzabsmean")) enab[FUNCT_NZABSMEAN] = 1;
  if (getInt("nzqmean")) enab[FUNCT_NZQMEAN] = 1;
  if (getInt("nzgmean")) enab[FUNCT_NZGMEAN] = 1;
  if (getInt("nnz")) enab[FUNCT_NNZ] = 1;

  cFunctionalComponent::fetchConfig();
}

long cFunctionalMeans::process(FLOAT_DMEM *in, FLOAT_DMEM *inSorted, FLOAT_DMEM min, FLOAT_DMEM max, FLOAT_DMEM mean, FLOAT_DMEM *out, long Nin, long Nout)
{
  int i;
  if ((Nin>0)&&(out!=NULL)) {

    double tmp=(double)*in;
    double fa = fabs(tmp);

    double absmean = fa;
    double qmean = tmp*tmp;
    long nnz;

    double nzamean;
    double nzabsmean;
    double nzqmean;
    double nzgmean;

    if (tmp!=0.0) {
      nzamean = tmp;
      nzabsmean = fa;
      nzqmean = tmp*tmp;
      nzgmean = log(fa);
      nnz=1;
    } else {
      nzamean = 0.0;
      nzabsmean = 0.0;
      nzqmean = 0.0;
      nzgmean = 0.0;
      nnz=0;
    }
    for (i=1; i<Nin; i++) {
      in++;
      tmp=(double)*in;
      fa = fabs(tmp);
      //      amean += tmp;
      absmean += fa;
      if (tmp!=0.0) {
        nzamean += tmp;
        nzabsmean += fa;
        nzgmean += log(fa);
        tmp *= tmp;
        nzqmean += tmp;
        nnz++;
      }
      qmean += tmp;
    }
    tmp = (double)Nin;
    //    amean = amean / tmp;
    absmean = absmean / tmp;
    qmean = qmean / tmp;

    if (nnz>0) {
      tmp = (double)nnz;
      nzamean = nzamean / tmp;
      nzabsmean = nzabsmean / tmp;
      nzqmean = nzqmean / tmp;
      nzgmean /= tmp; //pow( 1.0/nzgmean, 1.0/tmp );
      nzgmean = exp(nzgmean);
    }


    int n=0;
    if (enab[FUNCT_AMEAN]) out[n++]=(FLOAT_DMEM)mean;
    if (enab[FUNCT_ABSMEAN]) out[n++]=(FLOAT_DMEM)absmean;
    if (enab[FUNCT_QMEAN]) out[n++]=(FLOAT_DMEM)qmean;
    if (enab[FUNCT_NZAMEAN]) out[n++]=(FLOAT_DMEM)nzamean;
    if (enab[FUNCT_NZABSMEAN]) out[n++]=(FLOAT_DMEM)nzabsmean;
    if (enab[FUNCT_NZQMEAN]) out[n++]=(FLOAT_DMEM)nzqmean;
    if (enab[FUNCT_NZGMEAN]) out[n++]=(FLOAT_DMEM)nzgmean;
    if (timeNorm==TIMENORM_FRAMES) {
      if (enab[FUNCT_NNZ]) out[n++]=(FLOAT_DMEM)nnz;
    } else if (timeNorm==TIMENORM_SEGMENT) {
      if (enab[FUNCT_NNZ]) out[n++]=(FLOAT_DMEM)nnz/(FLOAT_DMEM)Nin;
    } else if (timeNorm==TIMENORM_SECONDS) {
      if (enab[FUNCT_NNZ]) out[n++]=(FLOAT_DMEM)nnz/(FLOAT_DMEM)getInputPeriod();
    }
    
    return n;
  }
  return 0;
}

/*
long cFunctionalMeans::process(INT_DMEM *in, INT_DMEM *inSorted, INT_DMEM *out, long Nin, long Nout)
{

  return 0;
}
*/

cFunctionalMeans::~cFunctionalMeans()
{
}

