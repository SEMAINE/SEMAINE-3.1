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

functionals: linear and quadratic regression coefficients

*/


#include <functionalRegression.hpp>

#define MODULE "cFunctionalRegression"


#define FUNCT_LINREGC1     0
#define FUNCT_LINREGC2     1
#define FUNCT_LINREGERRA   2
#define FUNCT_LINREGERRQ   3
#define FUNCT_QREGC1       4
#define FUNCT_QREGC2       5
#define FUNCT_QREGC3       6
#define FUNCT_QREGERRA     7
#define FUNCT_QREGERRQ     8
#define FUNCT_CENTROID     9

#define N_FUNCTS  10

#define NAMES  "linregc1","linregc2","linregerrA","linregerrQ","qregc1","qregc2","qregc3","qregerrA","qregerrQ","centroid"

const char *regressionNames[] = {NAMES};  // change variable name to your clas...

SMILECOMPONENT_STATICS(cFunctionalRegression)

SMILECOMPONENT_REGCOMP(cFunctionalRegression)
{
  SMILECOMPONENT_REGCOMP_INIT
  
  scname = COMPONENT_NAME_CFUNCTIONALREGRESSION;
  sdescription = COMPONENT_DESCRIPTION_CFUNCTIONALREGRESSION;

  // configure your component's configType:
  SMILECOMPONENT_CREATE_CONFIGTYPE
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("linregc1","1/0=enable/disable output of slope m (linear regression line)",1);
    ct->setField("linregc2","1/0=enable/disable output of offset t (linear regression line)",1);
    ct->setField("linregerrA","1/0=enable/disable output of linear error between contour and linear regression line",1);
    ct->setField("linregerrQ","1/0=enable/disable output of quadratic error between contour and linear regression line",1);
    ct->setField("qregc1","1/0=enable/disable output of quadratic regression coefficient 1 (a)",1);
    ct->setField("qregc2","1/0=enable/disable output of quadratic regression coefficient 2 (b)",1);
    ct->setField("qregc3","1/0=enable/disable output of quadratic regression coefficient 3 (c = offset)",1);
    ct->setField("qregerrA","1/0=enable/disable output of linear error between contour and quadratic regression line (parabola)",1);
    ct->setField("qregerrQ","1/0=enable/disable output of quadratic error between contour and quadratic regression line (parabola)",1);
    ct->setField("centroid","1/0=enable/disable output of centroid of contour (this is computed as a by-product of the regression coefficients).",1);
    ct->setField("normRegCoeff","1/0=enable/disable normalisation of regression coefficients. If enabled, the coefficients are scaled (multiplied by the contour length) so that a regression line or parabola approximating the contour can be plotted over an x-axis range from 0 to 1, i.e. this makes the coefficients independent of the contour length (a longer contour with a lower slope will then have the same 'm' (slope) linear regression coefficient as a shorter but steeper slope).",0);
  )

  SMILECOMPONENT_MAKEINFO_NODMEM(cFunctionalRegression);
}

SMILECOMPONENT_CREATE(cFunctionalRegression)

//-----

cFunctionalRegression::cFunctionalRegression(const char *_name) :
  cFunctionalComponent(_name,N_FUNCTS,regressionNames),
  enQreg(0)
{
}

void cFunctionalRegression::fetchConfig()
{
  if (getInt("linregc1")) enab[FUNCT_LINREGC1] = 1;
  if (getInt("linregc2")) enab[FUNCT_LINREGC2] = 1;
  if (getInt("linregerrA")) enab[FUNCT_LINREGERRA] = 1;
  if (getInt("linregerrQ")) enab[FUNCT_LINREGERRQ] = 1;
  if (getInt("qregc1")) { enab[FUNCT_QREGC1] = 1; enQreg=1; }
  if (getInt("qregc2")) { enab[FUNCT_QREGC2] = 1; enQreg=1; }
  if (getInt("qregc3")) { enab[FUNCT_QREGC3] = 1; enQreg=1; }
  if (getInt("qregerrA")) { enab[FUNCT_QREGERRA] = 1; enQreg=1; }
  if (getInt("qregerrQ")) { enab[FUNCT_QREGERRQ] = 1; enQreg=1; }
  if (getInt("centroid")) { enab[FUNCT_CENTROID] = 1; enQreg=1; }
  
  normRegCoeff = getInt("normRegCoeff");

  cFunctionalComponent::fetchConfig();
}

long cFunctionalRegression::process(FLOAT_DMEM *in, FLOAT_DMEM *inSorted, FLOAT_DMEM min, FLOAT_DMEM max, FLOAT_DMEM mean, FLOAT_DMEM *out, long Nin, long Nout)
{
  if ((Nin>0)&&(out!=NULL)) {
    //compute centroid
    FLOAT_DMEM *iE=in+Nin, *i0=in;
    double Nind = (double)Nin;
    
    double num=0.0, num2=0.0;
    double tmp,asum=mean*Nind;
    double ii=0.0;
    while (in<iE) {
//      asum += *(in);
      tmp = (double)(*(in++)) * ii;
      num += tmp;
      tmp *= ii;
      ii += 1.0;
      num2 += tmp;
    }

    double centroid;
    if (asum != 0.0)
      centroid = num / ( asum * Nind);
    else
      centroid = 0.0;

    in=i0;

    
    double m=0.0,t=0.0,leq=0.0,lea=0.0;
    double a=0.0,b=0.0,c=0.0,qeq=0.0,qea=0.0;
    double S1,S2,S3,S4;
    if (Nin > 1) {
      // LINEAR REGRESSION:
/*
      S1 = (Nind-1.0)*Nind/2.0;  // sum of all i=0..N-1
      S2 = (Nind-1.0)*Nind*(2.0*Nind-1.0)/6.0; // sum of all i^2 for i=0..N-1
                                              // num: if sum of y_i*i for all i=0..N-1
      t = ( asum - num*S1/S2) / ( Nind - S1*S1/S2 );
      m = ( num - t * S1 ) / S2;
*/ // optimised computation:
      double NNm1 = (Nind)*(Nind-(double)1.0);

      S1 = NNm1/(double)2.0;  // sum of all i=0..N-1
      S2 = NNm1*((double)2.0*Nind-(double)1.0)/(double)6.0; // sum of all i^2 for i=0..N-1

      double S1dS2 = S1/S2;
      double tmp = ( Nind - S1*S1dS2 );
      if (tmp == 0.0) t = 0.0;
      else t = ( asum - num*S1dS2) / tmp;
      m = ( num - t * S1 ) / S2;

      S3 = S1*S1;
      double Nind1 = Nind-(double)1.0;
      S4 = S2 * ((double)3.0*(Nind1*Nind1 + Nind1)-(double)1.0) / (double)5.0;

      // QUADRATIC REGRESSION:
      if (enQreg) {

        double det;
        double S3S3 = S3*S3;
        double S2S2 = S2*S2;
        double S1S2 = S1*S2;
        double S1S1 = S3;
        det = S4*S2*Nind + (double)2.0*S3*S1S2 - S2S2*S2 - S3S3*Nind - S1S1*S4;

        if (det != 0.0) {
          a = ( (S2*Nind - S1S1)*num2 + (S1S2 - S3*Nind)*num + (S3*S1 - S2S2)*asum ) / det;
          b = ( (S1S2 - S3*Nind)*num2 + (S4*Nind - S2S2)*num + (S3*S2 - S4*S1)*asum ) / det;
          c = ( (S3*S1 - S2S2)*num2 + (S3*S2-S4*S1)*num + (S4*S2 - S3S3)*asum ) / det;
        } else {
          a=0.0; b=0.0; c=0.0;
        }

      }
//    printf("nind:%f  S1=%f,  S2=%f  S3=%f  S4=%f  num2=%f  num=%f  asum=%f t=%f\n",Nind,S1,S2,S3,S4,num2,num,asum,t);
    } else {
      m = 0; t=c=*in;
      a = 0.0; b=0.0;
    }
    
    // linear regression error:
    ii=0.0; double e;
    while (in<iE) {
      e = (double)(*(in++)) - (m*ii + t);
      lea += fabs(e);
      ii += 1.0;
      leq += e*e;
    }
    in=i0;

    // quadratic regresssion error:
    if (enQreg) {
      ii=0.0; double e;
      while (in<iE) {
        e = (double)(*(in++)) - (a*ii*ii + b*ii + c);
        qea += fabs(e);
        ii += 1.0;
        qeq += e*e;
      }
      in=i0;
    }

    int n=0;
    
    if (normRegCoeff) {
      m *= Nind-1.0;
      // TODO: a,b
      a *= (Nind-1.0)*(Nind-1.0);
      b *= Nind-1.0;
    }

    // security checks:
    if (!finite(m)) m = 0.0;
    if (!finite(t)) t = 0.0;
    if (!finite(lea/Nind)) lea = 0.0;
    if (!finite(leq/Nind)) leq = 0.0;
    if (!finite(a)) a = 0.0;
    if (!finite(b)) b = 0.0;
    if (!finite(c)) c = 0.0;
    if (!finite(qea/Nind)) qea = 0.0;
    if (!finite(qeq/Nind)) qeq = 0.0;
    if (!finite(centroid)) centroid = 0.0;

    // save values:
    if (enab[FUNCT_LINREGC1]) out[n++]=(FLOAT_DMEM)m;
    if (enab[FUNCT_LINREGC2]) out[n++]=(FLOAT_DMEM)t;
    if (enab[FUNCT_LINREGERRA]) out[n++]=(FLOAT_DMEM)(lea/Nind);
    if (enab[FUNCT_LINREGERRQ]) out[n++]=(FLOAT_DMEM)(leq/Nind);

    if (enab[FUNCT_QREGC1]) out[n++]=(FLOAT_DMEM)a;
    if (enab[FUNCT_QREGC2]) out[n++]=(FLOAT_DMEM)b;
    if (enab[FUNCT_QREGC3]) out[n++]=(FLOAT_DMEM)c;
    if (enab[FUNCT_QREGERRA]) out[n++]=(FLOAT_DMEM)qea;
    if (enab[FUNCT_QREGERRQ]) out[n++]=(FLOAT_DMEM)qeq;

    if (enab[FUNCT_CENTROID]) out[n++]=(FLOAT_DMEM)centroid;

    return n;
  }
  return 0;
}

/*
long cFunctionalRegression::process(INT_DMEM *in, INT_DMEM *inSorted, INT_DMEM *out, long Nin, long Nout)
{

  return 0;
}
*/

cFunctionalRegression::~cFunctionalRegression()
{
}

