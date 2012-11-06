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


/*  openSMILE component: combFilter

general comb filter bank

output : filtered signal

to aggregate filter energy, use a cFunctionalMean (within cFunctionals) component
or a cIncrementalFunctional(mean) component

*/


#include <combFilter.hpp>
#include <math.h>

#define MODULE "cCombFilter"


SMILECOMPONENT_STATICS(cCombFilter)

SMILECOMPONENT_REGCOMP(cCombFilter)
{
  SMILECOMPONENT_REGCOMP_INIT
    scname = COMPONENT_NAME_CCOMBFILTER;
  sdescription = COMPONENT_DESCRIPTION_CCOMBFILTER;

  // we inherit cWindowProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cWindowProcessor")
    SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("nameAppend", NULL, "comb");
  ct->setField("gains","Gain 'alpha' of each comb filter in the bank. The number of elements must match the number of elements in the 'delays' or 'delays_frames' array. The gain values must be in the range 0..1, excluding the extremes 0 and 1.", 0.0, ARRAY_TYPE);
  ct->setField("delays","Delay 'tau' of each comb filter in the bank (unit: seconds). The number of elements must match the number of elements in the 'gains' array. The unit of the delay is specified in seconds.", 0.0, ARRAY_TYPE);
  ct->setField("delays_frames","Delay 'tau' of each comb filter in the bank (unit: input level frames). The number of elements must match the number of elements in the 'gains' array. The unit of the delay is specified in seconds.", 0.0, ARRAY_TYPE);
  ct->setField("fullBank", "1 = Instead of creating a filter bank from the parameters given in 'gains' and 'delays'/'delays_frames', create a continuous filter bank with delays from 'bankStartDelay(_frames)' to 'bankEndDelay(_frames)' and constant gain 'bankGain'. ", 0);
  ct->setField("bankGain", "Gain 'alpha' of all comb filters in filter bank created with 'fullBank=1'.", 0.7);
  ct->setField("bankStartDelay", "Delay 'tau' of first (shortest delay) comb filter in the bank, unit: seconds.", 0.0);
  ct->setField("bankStartDelay_frames", "Delay 'tau' of first (shortest delay) comb filter in the bank, unit: input level frames.", 0.0);
  ct->setField("bankStartDelay", "Delay 'tau' of first (shortest delay) comb filter in the bank, unit: seconds.", 0.0);
  ct->setField("bankEndDelay_frames", "Delay 'tau' of first (shortest delay) comb filter in the bank, unit: input level frames.", 0.0);

  ct->setField("filterType","All filters in the bank (currently) must be of the same type. Supported types are: 'iir' for an infinite impulse response comb filter, 'fir' for a finite impulse response comb filter (1st order).", "iir");
  //ct->setField("firLength","If 'filterType=fir' this option sets the number of past intervals to include in 
  //ct->setField("smaWin","The size of the moving average window. A larger window means more smoothing.",3);
  ct->setField("blocksize", NULL ,1); // <-- increase blocksize for better performance at the cost of higher latency
  //ct->setField("", NULL, "comb");
  )
    SMILECOMPONENT_MAKEINFO(cCombFilter);
}

SMILECOMPONENT_CREATE(cCombFilter)

//-----

cCombFilter::cCombFilter(const char *_name) :
cWindowProcessor(_name),
filters(NULL), nFilters(0)
{
}


void cCombFilter::fetchConfig()
{
  cWindowProcessor::fetchConfig();
}

int cCombFilter::configureWriter(const sDmLevelConfig *c)
{
  long maxDelay=0;

  // read config and create the filter bank
  int fullBank = getInt("fullBank");
  const char * stype = getStr("filterType");
  int ftype;
  if (!strncmp(stype,"iir",3)) {
    ftype = COMB_IIR;
  } else if (!strncmp(stype,"fir",3)) {
    ftype = COMB_FIR;
  }
  double T = reader->getLevelT();

  int sizeGains = getArraySize("gains");
  int sizeDelays = getArraySize("delays");
  int sizeDelaysFrames = getArraySize("delays_frames");

  if ( (!fullBank) && (sizeGains > 0) && ((sizeDelays>0)||(sizeDelaysFrames>0)) ) {
    // create custom filter bank
    int tmp = MAX(sizeDelays, sizeDelaysFrames);
    if (tmp != sizeGains) {
      SMILE_IWRN(1,"Something is incorrect in your config! The number of elements in the 'gains' array (%i) differs from the number of elements in the 'delays' or 'delays_frames' array (%i). Will only use the smaller number of elements as number of filters.",  sizeGains , tmp);
    }
    nFilters = MIN(tmp, sizeGains);
    filters = (combFilter **)calloc(1,sizeof(combFilter *)*nFilters);

    long n;
    for (n=0; n<nFilters ; n++) {
      long d;
      if (sizeDelaysFrames > 0) {
        d = getInt_f(myvprint("delays_frames[%i]",n));
      } else {
        double _d = getDouble_f(myvprint("delays[%i]",n));
        if (T > 0.0) d = (long)(_d/T);
        else d = (long)(_d);
      }

      if (d < 1) {
        SMILE_IERR(1,"delay[%i] must be >= 1 frame, auto-setting to 1 frame.",n);
        d = 1;
      }
      else if (d < 1) {
        SMILE_IERR(1,"delay[%i] must be >= 1 frame, auto-setting to 1 frame.",n);
        d = 1;
      }

      filters[n] = new combFilter((FLOAT_DMEM)getDouble_f(myvprint("gains[%i]",n)), 
                                  d, ftype);  

      if (d > maxDelay) maxDelay = d;
    }

  } else {
    // create continuous filter bank over a given delay range
    FLOAT_DMEM gain = (FLOAT_DMEM)getDouble("bankGain");
    long tau0, tau1;
    if (isSet("bankStartDelay_frames")) {
      tau0 = getInt("bankStartDelay_frames");
    } else {
      double tmp = getDouble("bankStartDelay");
      if (T > 0.0) tau0 = (long)(tmp/T);
      else tau0 = (long)(tmp);
    }
    if (isSet("bankEndDelay_frames")) {
      tau1 = getInt("bankEndDelay_frames");
    } else {
      double tmp = getDouble("bankEndDelay");
      if (T>0.0) tau1 = (long)(tmp/T);
      else tau1 = (long)(tmp);
    }

    // check the input
    if (tau0 < 1) {
      SMILE_IERR(1,"bankStartDelay must be >= 1 frame, auto-setting to 1 frame.");
      tau0 = 1;
    }
    else if (tau1 < 1) {
      SMILE_IERR(1,"bankEndDelay must be >= 1 frame, auto-setting to 1 frame.");
      tau1 = 1;
    }

    if (tau0 > tau1) {
      SMILE_IERR(1,"bankStartDelay must be <= bankEndDelay, auto-setting bankStartDelay=bankEndDelay (the filter bank will consist of only 1 comv filter).");
      tau0 = tau1;
    }

    if (gain < 0.0) {
      SMILE_IERR(1,"bankGain must be >= 0.0! auto setting to 0.0.");
      gain = 0.0;
    }
    if (gain > 1.0) {
      SMILE_IERR(1,"bankGain must be <= 1.0! auto setting to 1.0.");
      gain = 1.0;
    }

    // actually create the bank:
    nFilters = tau1 - tau0 + 1;
    filters = (combFilter **)calloc(1,sizeof(combFilter *)*nFilters);

    long n;
    for (n=0; n<nFilters ; n++) {
      filters[n] = new combFilter(gain, tau0+n, ftype, reader->getLevelN());  
    }

    maxDelay = tau1;
  }

  setWindow(maxDelay,0);

  return 1;
}

int cCombFilter::setupNamesForField(int i, const char*name, long nEl)
{
  // the configureWriter function is called before this functions is called (at least it should be, otherwise nFilters will not be initialised)
  if (nFilters == 0) {
    SMILE_IERR(1, "nFilters = 0 in setupNamesForField. Something is wrong with your config, the filterbank will create no output, openSMILE will not run like this.");
    return 0;
  }
  if (nEl > 1) {
    char *name2; long n;
    for (n=0; n<nEl; n++) {
      name2 = myvprint("%s[%i]",name,n);
      addNameAppendField(name2, nameAppend, nFilters);
      free(name2);
    }
  } else {
    if (nEl > 0) addNameAppendField(name, nameAppend, nFilters);
  }
  return nFilters*nEl;
}

// order is the amount of memory (overlap) that will be present in _in
// buf will have nT timesteps, however also order negative indicies (i.e. you may access a negative array index up to -order!)
int cCombFilter::processBuffer(cMatrix *_in, cMatrix *_out, int _pre, int _post, int rowGlobal )
{
  long n;
  long i;
  if (_in->type!=DMEM_FLOAT) COMP_ERR("dataType (%i) != DMEM_FLOAT not yet supported!",_in->type);
  FLOAT_DMEM *x=_in->dataF;
  FLOAT_DMEM *y=_out->dataF;

  
  for (i=0; i<_in->N; i++) {
    for (n=0; n<nFilters; n++) {
      filters[n]->process(x+i, y+i*nFilters, _in->nT, _in->N, _out->N);
    }
  }

  return 1;
}


cCombFilter::~cCombFilter()
{
  // free the filter bank
  if (filters != NULL) {
    int n;
    for (n=0; n<nFilters; n++) {
      if (filters[n] != NULL) delete filters[n];
    }
    delete filters;
  }
}

