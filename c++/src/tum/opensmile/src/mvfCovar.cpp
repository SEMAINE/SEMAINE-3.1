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

Mvf = Multivariate Functional

Mvf Covar: Mutlivariate Covariance
 
*/


#include <mvfCovar.hpp>

#define MODULE "cMvfCovar"


SMILECOMPONENT_STATICS(cMvfCovar)

SMILECOMPONENT_REGCOMP(cMvfCovar)
{
  SMILECOMPONENT_REGCOMP_INIT

  scname = COMPONENT_NAME_CMVFCOVAR;
  sdescription = COMPONENT_DESCRIPTION_CMVFCOVAR;

  // we inherit cWinToVecProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cWinToVecProcessor")
  
  SMILECOMPONENT_IFNOTREGAGAIN( 
  // TODO: move this to the cWinToVecProcessor class, and call it something different to avoid confusion with the option in cVectorProcessor
    ct->setField("processFieldsInMatrixMode", "If you set this to 0, then the whole input vector will treated as a single field, i.e. processed at once. If it is set to 1 (default) then each field will be processed individually, i.e. you will get a covariance matrix as output for each field.",1);
  )
  
  SMILECOMPONENT_MAKEINFO(cMvfCovar);
}

SMILECOMPONENT_CREATE(cMvfCovar)

//-----

cMvfCovar::cMvfCovar(const char *_name) :
  cWinToVecProcessor(_name),
  means(NULL)
{
  wholeMatrixMode = 1; // enable wholeMatrixMode in cWinToVecProcessor parent class
}

void cMvfCovar::fetchConfig() 
{
  cWinToVecProcessor::fetchConfig();

  if (wholeMatrixMode) {
    processFieldsInMatrixMode = getInt("processFieldsInMatrixMode"); // NOTE: the variable is defined in the cWinToVecProcessor class
  }
}

long cMvfCovar::getNoutputs(long nIn) 
{
  // the covariance matrix is a square matrix...
  //return nIn*nIn; //<-- full matrix
  return (nIn*(nIn+1))/2; // <-- upper triangle only
}

// get index of covariance element in vector of only upper triangle elements, given the size of the cov. matrix and the coordinates in the matrix
// i = row, j = col.
long covTrIdx(long i, long j, long _N) 
{
  long n; long ret=0;
  for (n=0; n<i; n++) {
    ret += _N-n;
  }
  return ret + j - i;
}

int cMvfCovar::doProcessMatrix(int idxi, cMatrix *in, FLOAT_DMEM *out, long nOut)
{
  // very simple algorihtm and code here...
  // we compute the covariance of the matrix in in, where the rows represent the elements we compute the variances for and the columns contain the timesteps
  long n; long i,j;
  long N = in->N;

  if (getNoutputs(N) != nOut) {
    SMILE_IWRN(1,"nOut [%i] != getNoutputs(%i) [%i] in doProcessMatrix, seems like a bug in the code, or some bogus configuration",nOut,N,getNoutputs(N));
  }

  // first, compute means 
  if (means == NULL) {
    means = (FLOAT_DMEM*)malloc(sizeof(FLOAT_DMEM)*N);
  }
  for (i=0; i<N; i++) {
    means[i] = 0.0;
    for (n=0; n < in->nT; n++) {
      means[i] += in->dataF[i+N*n];
    }
    means[i] /= (FLOAT_DMEM)(in->nT);
  }

  // next, compute (co-)variances
  FLOAT_DMEM cov; long idx = 0;
  for (n=0; n<in->nT; n++) {
    for (i=0; i<N; i++) {
      //long idx = covTrIdx(i,i,N);
      for (j=i; j<N; j++) {
        cov = (in->dataF[n*N + i] - means[i]) * (in->dataF[n*N + j] - means[j]);
        out[idx++] += cov;
      }
    }
  }

  //normalise
  for (i=0; i<nOut; i++) {
    out[i] /= (FLOAT_DMEM)(in->nT);
  }

  // *out contains the upper triangle of the covariance matrix as a vector, where the matrix is read rowwise (1st row first, N elements, 2nd row second N-1 elements, etc; the first element in each "row" thus is always the diagonal element) 
  return 1;
}


cMvfCovar::~cMvfCovar()
{
  if (means != NULL) free(means);
}

