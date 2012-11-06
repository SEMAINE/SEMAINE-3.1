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

functionals: rise/fall times, up/down-level times

28/10/2009:
 added duration feature (thanks to Dino Seppi)

*/


#include <functionalTimes.hpp>

#define MODULE "cFunctionalTimes"



#define FUNCT_UPLEVELTIME25     0
#define FUNCT_DOWNLEVELTIME25   1
#define FUNCT_UPLEVELTIME50     2
#define FUNCT_DOWNLEVELTIME50   3
#define FUNCT_UPLEVELTIME75     4
#define FUNCT_DOWNLEVELTIME75   5
#define FUNCT_UPLEVELTIME90     6
#define FUNCT_DOWNLEVELTIME90   7
#define FUNCT_RISETIME      8
#define FUNCT_FALLTIME      9
#define FUNCT_LEFTCTIME     10
#define FUNCT_RIGHTCTIME    11
#define FUNCT_DURATION      12
#define FUNCT_UPLEVELTIME        13
#define FUNCT_DOWNLEVELTIME      14


#define N_FUNCTS  15

#define NAMES     "upleveltime25","downleveltime25","upleveltime50","downleveltime50","upleveltime75","downleveltime75","upleveltime90","downleveltime90","risetime","falltime","leftctime","rightctime","duration","upleveltime","downleveltime"
#define IDX_VAR_FUNCTS 13  // start of uptime, downtime, arrays of variable length

const char *timesNames[] = {NAMES};  // change variable name to your clas...

SMILECOMPONENT_STATICS(cFunctionalTimes)

SMILECOMPONENT_REGCOMP(cFunctionalTimes)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CFUNCTIONALTIMES;
  sdescription = COMPONENT_DESCRIPTION_CFUNCTIONALTIMES;

  // configure your component's configType:
  SMILECOMPONENT_CREATE_CONFIGTYPE
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("upleveltime25","(1/0=yes/no) compute time where signal is above 0.25*range",1);
    ct->setField("downleveltime25","(1/0=yes/no) compute time where signal is below 0.25*range",1);
    ct->setField("upleveltime50","(1/0=yes/no) compute time where signal is above 0.50*range",1);
    ct->setField("downleveltime50","(1/0=yes/no) compute time where signal is below 0.50*range",1);
    ct->setField("upleveltime75","(1/0=yes/no) compute time where signal is above 0.75*range",1);
    ct->setField("downleveltime75","(1/0=yes/no) compute time where signal is below 0.75*range",1);
    ct->setField("upleveltime90","(1/0=yes/no) compute time where signal is above 0.90*range",1);
    ct->setField("downleveltime90","(1/0=yes/no) compute time where signal is below 0.90*range",1);
    ct->setField("risetime","(1/0=yes/no) compute time where signal is rising",1);
    ct->setField("falltime","(1/0=yes/no) compute time where signal is falling",1);
    ct->setField("leftctime","(1/0=yes/no) compute time where signal has left curvature",1);
    ct->setField("rightctime","(1/0=yes/no) compute time where signal has right curvature",1);
    ct->setField("duration","(1/0=yes/no) compute duration time, in frames (or seconds, if (time)norm==seconds)",1);
    ct->setField("upleveltime","compute time where signal is above X*range : upleveltime[n]=X",0.9,ARRAY_TYPE);
    ct->setField("downleveltime","compute time where signal is below X*range : downleveltime[n]=X",0.9,ARRAY_TYPE);
    ct->setField("norm","This option specifies how this component should normalise times (if it generates output values related to durations): \n   'segment' (or: 'turn') : normalise to the range 0..1, the result is the relative length wrt. to the segment length )\n   'second'  (absolute time in seconds) \n   'frame' (absolute time in number of frames of input level)","segment",0,0);
    ct->setField("buggySecNorm","If set to 1, enables the old (prior to version 1.0.0 , 07 May 2010) second normalisation code which erroneously divides by the number of input frames. The default is kept at 1 (enabled) in order to not break compatibility with old configuration files, however you are strongly encouraged to change this to 0 in any new configuration you write in order to get the times in actual (bug-free) seconds!",1);
  )
  
  SMILECOMPONENT_MAKEINFO_NODMEM(cFunctionalTimes);
}

SMILECOMPONENT_CREATE(cFunctionalTimes)

//-----

cFunctionalTimes::cFunctionalTimes(const char *_name) :
  cFunctionalComponent(_name,N_FUNCTS,timesNames),
  ultime(NULL),
  dltime(NULL),
  tmpstr(NULL),
  varFctIdx(0)
{
}

void cFunctionalTimes::fetchConfig()
{
  parseTimeNormOption();
  
  buggySecNorm = getInt("buggySecNorm");

  if (getInt("upleveltime25")) enab[FUNCT_UPLEVELTIME25] = 1;
  if (getInt("downleveltime25")) enab[FUNCT_DOWNLEVELTIME25] = 1;
  if (getInt("upleveltime50")) enab[FUNCT_UPLEVELTIME50] = 1;
  if (getInt("downleveltime50")) enab[FUNCT_DOWNLEVELTIME50] = 1;
  if (getInt("upleveltime75")) enab[FUNCT_UPLEVELTIME75] = 1;
  if (getInt("downleveltime75")) enab[FUNCT_DOWNLEVELTIME75] = 1;
  if (getInt("upleveltime90")) enab[FUNCT_UPLEVELTIME90] = 1;
  if (getInt("downleveltime90")) enab[FUNCT_DOWNLEVELTIME90] = 1;
  if (getInt("risetime")) enab[FUNCT_RISETIME] = 1;
  if (getInt("falltime")) enab[FUNCT_FALLTIME] = 1;
  if (getInt("leftctime")) enab[FUNCT_LEFTCTIME] = 1;
  if (getInt("rightctime")) enab[FUNCT_RIGHTCTIME] = 1;
  if (getInt("duration")) enab[FUNCT_DURATION] = 1;


  int i;
  nUltime = getArraySize("upleveltime");
  nDltime = getArraySize("downleveltime");
  if (nUltime > 0) {
    enab[FUNCT_UPLEVELTIME] = 1;
    ultime = (double*)calloc(1,sizeof(double)*nUltime);

    // load upleveltime info
    for (i=0; i<nUltime; i++) {
      ultime[i] = getDouble_f(myvprint("upleveltime[%i]",i));
      if (ultime[i] < 0.0) {
        SMILE_WRN(2,"(inst '%s') upleveltime[%i] is out of range [0..1] : %f (clipping to 0.0)",getInstName(),i,ultime[i]);
        ultime[i] = 0.0;
      }
      if (ultime[i] > 1.0) {
        SMILE_WRN(2,"(inst '%s') upleveltime[%i] is out of range [0..1] : %f (clipping to 1.0)",getInstName(),i,ultime[i]);
        ultime[i] = 1.0;
      }
    }
  }
  if (nDltime > 0) {
    enab[FUNCT_DOWNLEVELTIME] = 1;
    dltime = (double*)calloc(1,sizeof(double)*nDltime);

    // load upleveltime info
    for (i=0; i<nDltime; i++) {
      dltime[i] = getDouble_f(myvprint("downleveltime[%i]",i));
      if (dltime[i] < 0.0) {
        SMILE_WRN(2,"(inst '%s') upleveltime[%i] is out of range [0..1] : %f (clipping to 0.0)",getInstName(),i,dltime[i]);
        dltime[i] = 0.0;
      }
      if (dltime[i] > 1.0) {
        SMILE_WRN(2,"(inst '%s') downleveltime[%i] is out of range [0..1] : %f (clipping to 1.0)",getInstName(),i,dltime[i]);
        dltime[i] = 1.0;
      }
    }
  }

  cFunctionalComponent::fetchConfig();
  if (enab[FUNCT_UPLEVELTIME]) nEnab += nUltime-1;
  if (enab[FUNCT_DOWNLEVELTIME]) nEnab += nDltime-1;
  
  // compute new var index:
  varFctIdx = 0;
  for (i=0; i<IDX_VAR_FUNCTS; i++) {
    if (enab[i]) varFctIdx++;
  }
}

const char* cFunctionalTimes::getValueName(long i)
{
  if (i<varFctIdx) {
    return cFunctionalComponent::getValueName(i);
  }
  if (i>=varFctIdx) {
    int j=varFctIdx;
    int dl=0;
    // check, if up- or downleveltime is referenced:
    i -= varFctIdx;
    if (i>=nUltime) { j++; i-= nUltime; dl = 1; }
    const char *n = cFunctionalComponent::getValueName(j);
    // append [i]
    if (tmpstr != NULL) free(tmpstr);
    if (!dl) {
      tmpstr = myvprint("%s%.1f",n,ultime[i]*100.0);
    } else {
      tmpstr = myvprint("%s%.1f",n,dltime[i]*100.0);
    }
    return tmpstr;
  }
  return "UNDEF";
}

long cFunctionalTimes::process(FLOAT_DMEM *in, FLOAT_DMEM *inSorted,  FLOAT_DMEM min, FLOAT_DMEM max, FLOAT_DMEM mean, FLOAT_DMEM *out, long Nin, long Nout)
{
  if ((Nin>0)&&(out!=NULL)) {
    int n=0;

    FLOAT_DMEM *i0 = in;
    FLOAT_DMEM *iE = in+Nin;
    FLOAT_DMEM Nind = (FLOAT_DMEM)Nin;

    FLOAT_DMEM Norm, Norm1, Norm2;
    Norm=Nind; Norm1=Nind-(FLOAT_DMEM)1.0; Norm2=Nind-(FLOAT_DMEM)2.0;

    FLOAT_DMEM _T = 1.0; 
    if (timeNorm==NORM_SECOND) {
      _T = (FLOAT_DMEM)getInputPeriod();
                           
      if (_T != 0.0) {
        if (buggySecNorm) {//OLD mode!!: 
          Norm /= _T; Norm1 /= _T; Norm2 /= _T;
        } else {
          Norm = (FLOAT_DMEM)(1.0)/_T;
          Norm1 /= Nind*_T;
          Norm2 /= Nind*_T;
        }
      }
    }
    if (timeNorm==NORM_FRAME) {
      Norm = 1.0; Norm1 /= Nind; Norm2 /= Nind;
    }

    
    FLOAT_DMEM range = max-min;
    FLOAT_DMEM l25, l50, l75, l90;
    l25 = (FLOAT_DMEM)0.25*range+min;
    l50 = (FLOAT_DMEM)0.50*range+min;
    l75 = (FLOAT_DMEM)0.75*range+min;
    l90 = (FLOAT_DMEM)0.90*range+min;
    long n25=0, n50=0, n75=0, n90=0;
    long nR=0, nF=0, nLC=0, nRC=0;
    
    // first pass: predefined ul/dl times AND rise/fall, etc.
    if ((enab[FUNCT_UPLEVELTIME25])||(enab[FUNCT_DOWNLEVELTIME25]) ||
        (enab[FUNCT_UPLEVELTIME50])||(enab[FUNCT_DOWNLEVELTIME50]) ||
        (enab[FUNCT_UPLEVELTIME75])||(enab[FUNCT_DOWNLEVELTIME75]) ||
        (enab[FUNCT_UPLEVELTIME90])||(enab[FUNCT_DOWNLEVELTIME90])) {
      while (in<iE) {
        if (*in <= l25) n25++;
        if (*in <= l50) n50++;
        if (*in <= l75) n75++;
        if (*(in++) <= l90) n90++;
      }
      in = i0;
    }
    if ((enab[FUNCT_RISETIME])||(enab[FUNCT_FALLTIME])) {
      while (++in<iE) {
//      for (i=1; i<Nin; i++) {
        if (*(in-1) < *in) nR++;      // rise
        else if (*(in-1) > *in) nF++; // fall
//        in++;
      }
      in = i0;
    }
    if ((enab[FUNCT_LEFTCTIME])||(enab[FUNCT_RIGHTCTIME])) {
      FLOAT_DMEM a1,a2;
      while (++in<iE-1) {
//      for (i=1; i<Nin-1; i++) {
        a1 = *(in)-*(in-1);
        a2 = *(in+1)-*(in);
        if ( a2 < a1 ) nRC++;      // right curve
        else if ( a1 < a2) nLC++;  // left curve
//        in++;
      }
      in = i0;
    }

    if (enab[FUNCT_UPLEVELTIME25]) out[n++]=((FLOAT_DMEM)(Nin-n25))/Norm;
    if (enab[FUNCT_DOWNLEVELTIME25]) out[n++]=((FLOAT_DMEM)(n25))/Norm;
    if (enab[FUNCT_UPLEVELTIME50]) out[n++]=((FLOAT_DMEM)(Nin-n50))/Norm;
    if (enab[FUNCT_DOWNLEVELTIME50]) out[n++]=((FLOAT_DMEM)(n50))/Norm;
    if (enab[FUNCT_UPLEVELTIME75]) out[n++]=((FLOAT_DMEM)(Nin-n75))/Norm;
    if (enab[FUNCT_DOWNLEVELTIME75]) out[n++]=((FLOAT_DMEM)(n75))/Norm;
    if (enab[FUNCT_UPLEVELTIME90]) out[n++]=((FLOAT_DMEM)(Nin-n90))/Norm;
    if (enab[FUNCT_DOWNLEVELTIME90]) out[n++]=((FLOAT_DMEM)(n90))/Norm;

    if (Norm1 != 0.0) {
      if (enab[FUNCT_RISETIME]) out[n++]=((FLOAT_DMEM)nR)/Norm1;
      if (enab[FUNCT_FALLTIME]) out[n++]=((FLOAT_DMEM)nF)/Norm1;
    } else {
      if (enab[FUNCT_RISETIME]) out[n++]=0.0;
      if (enab[FUNCT_FALLTIME]) out[n++]=0.0;
    }
    if (Norm2 != 0.0) {
      if (enab[FUNCT_LEFTCTIME]) out[n++]=((FLOAT_DMEM)nLC)/Norm2;
      if (enab[FUNCT_RIGHTCTIME]) out[n++]=((FLOAT_DMEM)nRC)/Norm2;
    } else {
      if (enab[FUNCT_LEFTCTIME]) out[n++]=0.0;
      if (enab[FUNCT_RIGHTCTIME]) out[n++]=0.0;
    }

    if (enab[FUNCT_DURATION]) {
      if (timeNorm==NORM_SECOND) {
        out[n++]=((FLOAT_DMEM)(Nin)*_T);
      } else {
        out[n++]=((FLOAT_DMEM)(Nin));
      }
    }

    // second pass, user defined times
    int j;
    if (enab[FUNCT_UPLEVELTIME]) {
      for (j=0; j<nUltime; j++) {
        FLOAT_DMEM lX = (FLOAT_DMEM)(ultime[j]*range+min);
        long nX=0;
        while (in<iE) if (*(in++) > lX) nX++;
        in = i0;
        out[n++] = ((FLOAT_DMEM)(nX))/Norm;
      }
    }
    if (enab[FUNCT_DOWNLEVELTIME]) {
      for (j=0; j<nDltime; j++) {
        FLOAT_DMEM lX = (FLOAT_DMEM)(dltime[j]*range+min);
        long nX=0;
        while (in<iE) if (*(in++) <= lX) nX++;
        in = i0;
        out[n++] = ((FLOAT_DMEM)(nX))/Norm;
      }
    }
    
    return n;
  }
  return 0;
}

/*
long cFunctionalTimes::process(INT_DMEM *in, INT_DMEM *inSorted, INT_DMEM *out, long Nin, long Nout)
{

  return 0;
}
*/

cFunctionalTimes::~cFunctionalTimes()
{
  if(ultime!=NULL) free(ultime);
  if(dltime!=NULL) free(dltime);
}

