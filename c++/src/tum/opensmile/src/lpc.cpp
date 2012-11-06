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

LPC, compute LPC coefficients from wave data (PCM) frames 

*/


#include <lpc.hpp>
#include <math.h>

#define MODULE "cLpc"

SMILECOMPONENT_STATICS(cLpc)

SMILECOMPONENT_REGCOMP(cLpc)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CLPC;
  sdescription = COMPONENT_DESCRIPTION_CLPC;

  // we inherit cVectorProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cVectorProcessor")
  
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("method","This option sets the lpc method to use. Choose between: 'acf' acf (autocorrelation) method with Levinson-Durbin algorithm , 'burg' Burg method (N. Anderson (1978)) ","acf");  
    ct->setField("p","Predictor order (= number of lpc coefficients)",8);
    ct->setField("saveLPCoeff","1 = save LP coefficients to output",1);
    ct->setField("lpGain","1 = save lpc gain (error) in output vector",0);
    ct->setField("saveRefCoeff","1 = save reflection coefficients to output",0);
    ct->setField("residual","1 = compute lpc residual signal and store in output frame",0);
    ct->setField("forwardFilter","1 = apply forward instead of inverse filter when computing residual",0);
    ct->setField("lpSpectrum","1 = compute lp spectrum using 'lpSpecDeltaF' as frequency resolution or 'lpSpecBins' bins",0);
    ct->setField("lpSpecDeltaF","frequency resolution of lp spectrum (only applicable if 'lpSpectrum=1')",10.0);
    ct->setField("lpSpecBins","number of bins to compute lp spectrum for (overrides lpSpecDeltaF) (only applicable if 'lpSpectrum=1')",100,0,0);
  )

  SMILECOMPONENT_MAKEINFO(cLpc);
}

SMILECOMPONENT_CREATE(cLpc)

//-----

cLpc::cLpc(const char *_name) :
  cVectorProcessor(_name),
  p(0),
  saveRefCoeff(0), acf(NULL),
  lpCoeff(NULL), lastLpCoeff(NULL), refCoeff(NULL),
  burgB1(NULL), burgB2(NULL), burgAA(NULL),
  lSpec(NULL), _ip(NULL), _w(NULL), latB(NULL)
{

}

void cLpc::fetchConfig()
{
  cVectorProcessor::fetchConfig();

  const char *met = getStr("method");
  method = 0;
  if (met != NULL) {
    if (!strncasecmp(met,"acf",3)) { method = LPC_METHOD_ACF; }
    //else if (!strncasecmp(met,"acf2",4)) { method = LPC_METHOD_ACF2; }
    else if (!strncasecmp(met,"burg",4)) { method = LPC_METHOD_BURG; }
  }

  p=getInt("p");
  if (p<1) p=1;
  SMILE_IDBG(2,"predictor order p = %i",p); 

  saveLPCoeff=getInt("saveLPCoeff");
  SMILE_IDBG(2,"saveLPCoeff = %i",saveLPCoeff); 

  lpGain=getInt("lpGain");
  SMILE_IDBG(2,"lpGain = %i",lpGain); 

  saveRefCoeff=getInt("saveRefCoeff");
  SMILE_IDBG(2,"saveRefCoeff = %i",saveRefCoeff); 

  residual=getInt("residual");
  SMILE_IDBG(2,"residual = %i",residual); 

  forwardRes=getInt("forwardFilter");
  SMILE_IDBG(2,"forwardRes = %i",forwardRes); 

  lpSpectrum = getInt("lpSpectrum");
  SMILE_IDBG(2,"lpSpectrum = %i",lpSpectrum); 

  lpSpecDeltaF = getDouble("lpSpecDeltaF");
  SMILE_IDBG(2,"lpSpecDeltaF = %f",lpSpecDeltaF); 
  lpSpecBins = getInt("lpSpecBins");
  SMILE_IDBG(2,"lpSpecBins = %i",lpSpecBins); 

  latB = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*p);
  lpCoeff = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*(p+1));
  lastLpCoeff = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*(p));
  refCoeff = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*p);
  lastGain = 0.0;
}

// setup size of output vector (= predictor order p)
int cLpc::setupNamesForField(int i, const char*name, long nEl)
{
  //char *xx; 
  int n=0;

  nInput = nEl;
  inputStart = 0;

  /*
  if ((nameAppend!=NULL)&&(strlen(nameAppend)>0)) {
    xx = myvprint("%s_Lpc%s",name,nameAppend);
  } else { xx = myvprint("%s_LOG",name); }*/
  if (saveLPCoeff) {
    writer->addField( "lpcCoeff", p ); n += p;
  }
  //free(xx);

  if (saveRefCoeff) {
  /*  if ((nameAppend!=NULL)&&(strlen(nameAppend)>0)) {
      xx = myvprint("%s_RMS%s",name,nameAppend);
    } else { xx = myvprint("%s_RMS",name); }*/
    writer->addField( "reflectionCoeff", p ); n += p;
    //free(xx);
  }

  if (lpGain) {
    writer->addField( "lpGain", 1 ); n += 1;
  }

  if (lpSpectrum) {
    writer->addField( "lpSpectrum", lpSpecBins ); n += lpSpecBins;
  }

  if (residual) {
    writer->addField( "lpcResidual", nEl ); n += nEl;
  }

  return n;

  // TODO: add reflection coeffs..
 // return cDataProcessor::setupNamesForField(i,name,p);
}


#if 0
/* lpc via acf */
FLOAT_DMEM cLpc::calcLpcAcf(FLOAT_DMEM * acf, int _p, FLOAT_DMEM *lpc, FLOAT_DMEM *refl)
{
  int i,j;
  if (lpc == NULL) return -1.0;

  FLOAT_DMEM k;
  FLOAT_DMEM error = acf[0];

    if ((acf[0] == 0.0)||(acf[0] == -0.0)) {
      for (i=0; i < _p; i++) lpc[i] = 0.0;
      return 0.0;
    }
    if (method == LPC_METHOD_ACF2) {

      lpc[0] = 1.0; lpc[1] = - acf[1] / acf[0];
      if (refl != NULL) refl[0] = lpc[1];

      error += acf[1] * lpc[1];
      int errF = 1;

      for (i = 2; i <= _p; i++)
      {
        FLOAT_DMEM s = 0;
        for (j = 1; j <= i; j++) {
          s += acf[i-j+1] * lpc[j-1];
        }


        FLOAT_DMEM rc_i=0.0;
        if ((error != 0.0)&&(errF)) {
          rc_i = - s / error;

          for (j = 2; j <= i/2+1; j++)
          {
            FLOAT_DMEM at = lpc[j-1] + rc_i * lpc[i-j+1];
            lpc[i-j+1] += rc_i * lpc[j-1];
            lpc[j-1] = at;
          }

        } 
        if (refl != NULL) refl[i-1] = rc_i;
        lpc[i] = rc_i; error += rc_i * s;
        if (error < 0.0) errF=0;
      }

      for (i=0; i<_p; i++) lpc[i] = lpc[i+1];

    } else if (method == LPC_METHOD_ACF1) {

      for (i = 0; i < _p; i++) {

        /* compute this iteration's reflection coefficient k_m */
        FLOAT_DMEM rr = -(acf[i+1]);
        for (j=0; j < i; j++) {
          rr -= lpc[j] * acf[i-j]; 
        }
        if (error == 0.0) k=0.0;      
        else k = rr/(error);  

        // TODO: save refl. coeff. 
        if (refl != NULL) {
          refl[i] = k;
        }

        /* update LPC coeffs. and error sum */ // ????? check this code
        lpc[i] = k;
        for (j = 0; j < i/2; j++) {
          FLOAT_DMEM tmp  = lpc[j];
          lpc[j]     += k * lpc[i-1-j];
          lpc[i-1-j] += k * tmp;
        }
        if (i & 1) lpc[j] += lpc[j] * k;

        error -= (k*k)*(error); 
        //   printf("err(%i): %f\n",i,error);
      }

    }

  } 
  
  return error;
}

/* autoCorrelation on FLOAT_DMEM array, return FLOAT_DMEM* array with ac coeffs, caller must free the returned array
   on error, NULL is returned */
void cLpc::autoCorrF(const FLOAT_DMEM *x, FLOAT_DMEM *acf, int n, int lag)
{
  int i,j;
/*
  for (i=1;i<=lag;i++){
    acf[i-1] = 0.0;
    for(j=1; j <= n-i+1; j++) {
      acf[i-1] += x[j-1] * x[j+i-2];
    }
    printf("acf(%i) = %f\n",i-1,acf[i-1]);
  }*/

  while (lag) {
    acf[--lag] = 0.0;
    for (i=lag; i < n; i++) acf[lag] += x[i] * x[i-lag];
  }
}
#endif


// return value: gain
FLOAT_DMEM cLpc::calcLpc(const FLOAT_DMEM *x, long Nsrc, FLOAT_DMEM * lpc, long nCoeff, FLOAT_DMEM *refl)
{
  FLOAT_DMEM gain = 0.0;
  if (method == LPC_METHOD_ACF) {
    if (acf == NULL) acf = (FLOAT_DMEM *)malloc(sizeof(FLOAT_DMEM)*(nCoeff+1));
    smileDsp_autoCorr(x, Nsrc, acf, nCoeff+1);
    smileDsp_calcLpcAcf(acf, lpc, nCoeff, &gain, refl);
  } else if (method == LPC_METHOD_BURG) {
    smileDsp_calcLpcBurg(x, Nsrc, lpc, nCoeff, &gain, &burgB1, &burgB2, &burgAA);
    if (refl != NULL) SMILE_IWRN(1,"computation of reflection coefficients with Burg's LPC method is not yet implemented!");
  }
  return gain;
}

int cLpc::processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi)
{
  // not yet implemented
  return 0;
}

int cLpc::processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  long myN = Ndst;
  if (residual) myN = Ndst - Nsrc;

  long expectedN = 0;
  if (saveRefCoeff) expectedN += p;
  if (saveLPCoeff) expectedN += p;
  if (lpGain) expectedN += 1;
  if (lpSpectrum) expectedN += lpSpecBins;
  if (myN != expectedN) {
    SMILE_IWRN(1,"Ndst(-Nsrc) (=%i) <> expected value (%i) ! something is wrong.. the program might crash!",myN,expectedN);
  }

  if (p<0) {
    SMILE_IWRN(1,"p<0! something is wrong...");
    p=0;
  }

  long i;
  FLOAT_DMEM *dst0 = dst;
  FLOAT_DMEM gain = 0.0;

  //for (i=0; i<Nsrc; i++) {
    //lpc[i] = src[lpcCoeffIdx+i];
  //}printf("----\n");

  if (saveRefCoeff) {
    //calcLpc(const FLOAT_DMEM *x, long Nsrc, FLOAT_DMEM * lpc, long nCoeff, FLOAT_DMEM *refl) 
    gain = calcLpc(src, Nsrc, lpCoeff, p, refCoeff);
    if (saveLPCoeff) {
      for (i=0; i<p; i++) {
        dst[i] = lpCoeff[i];
        dst[i+p] = refCoeff[i];
      }
      dst += 2*p;
    } else {
      for (i=0; i<p; i++) {
        dst[i] = refCoeff[i];
      }
      dst += p;
    }    
  } else {
    if (saveLPCoeff || residual || lpSpectrum || lpGain) {
      gain = calcLpc(src, Nsrc, lpCoeff, p, refCoeff);
      //gain = calcLpc(acf, p, lpCoeff, NULL);
    }
    if (saveLPCoeff) {
      for (i=0; i<p; i++) {
        dst[i] = lpCoeff[i];
      }
      dst += p;
    }
  }

  if (lpGain) {
    //printf("gain: %f\n",gain);
    dst[0] = gain;
    dst++;
  }

  if (lpSpectrum) {
    /*
      we compute the lp spectrum by zero padding and fft of the lp coefficients
      the number of 0's we pad with determines our frequency resolution
    */
     // config parameters: lpSpecDeltaF & lpSpecBins (N or -1 for = nLpc)
    //double fftN = (1.0/T) / lpSpecDeltaF;
    if (lSpec == NULL) lSpec = (FLOAT_TYPE_FFT*)malloc(sizeof(FLOAT_TYPE_FFT) * lpSpecBins * 2);

    // create padded vector
    for (i=0; i<lpSpecBins*2; i++) {
      lSpec[i] = 0.0;
    }
    lSpec[0] = 1.0;
    for (i=1; i<=p; i++) {
      lSpec[i] = (FLOAT_TYPE_FFT)lpCoeff[i-1];
    }

    // transform
    if (_ip==NULL) _ip = (int *)calloc(1,sizeof(int)*(lpSpecBins*2+2));
    if (_w==NULL) _w = (FLOAT_TYPE_FFT *)calloc(1,sizeof(FLOAT_TYPE_FFT)*(lpSpecBins*2*5)/4+2);
    //perform FFT
    rdft(lpSpecBins*2, 1, lSpec, _ip, _w);

    // compute magnitude
    int n=0;
    *(dst++) = (FLOAT_DMEM)fabs( lSpec[0] ); /* DC part */
    for (i=2; i<(lpSpecBins-1)*2; i += 2) {
      // save in output vector
      *(dst++) = (FLOAT_DMEM)sqrt( lSpec[i]*lSpec[i] + lSpec[i+1]*lSpec[i+1] );
    }
    *(dst++) = (FLOAT_DMEM)fabs( lSpec[1] ); /* Nyquist freq. */
  }

  if (residual) {
    if (forwardRes) { // apply forward LPC filter (recursive)
      for (i=0; i<Nsrc; i++) {
       dst[i] = smileDsp_invLattice(refCoeff, latB, p, src[i]);
      }
    } else { // apply inverse LPC filter (this yields the actual residual)
      // alternative: lattice filter with reflection coefficients:
      for (i=0; i<Nsrc; i++) {
        dst[i] = smileDsp_lattice(refCoeff, latB, p, src[i], NULL);
      }
    }

    lastGain = gain;
  }

  return 1;
}


cLpc::~cLpc()
{
  if (acf != NULL) free(acf);
  if (lpCoeff != NULL) free(lpCoeff);
  if (lastLpCoeff != NULL) free(lastLpCoeff);
  if (refCoeff != NULL) free(refCoeff);
  if (lSpec != NULL) free(lSpec);
  if (latB != NULL) free(latB);
  if (burgB1 != NULL) free(burgB1);
  if (burgB2 != NULL) free(burgB2);
  if (burgAA != NULL) free(burgAA);
  if (_ip!=NULL) free(_ip);
  if (_w!=NULL) free(_w);
}

