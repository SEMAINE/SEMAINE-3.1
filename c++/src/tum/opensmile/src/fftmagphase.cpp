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

compute magnitude and phase from complex valued fft output
TODO: support inverse??

*/


#include <fftmagphase.hpp>
#include <math.h>

#define MODULE "cFFTmagphase"

/*Library:
sComponentInfo * registerMe(cConfigManager *_confman) {
  cDataProcessor::registerComponent(_confman);
}
*/

SMILECOMPONENT_STATICS(cFFTmagphase)

SMILECOMPONENT_REGCOMP(cFFTmagphase)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CFFTMAGPHASE;
  sdescription = COMPONENT_DESCRIPTION_CFFTMAGPHASE;

  // we inherit cVectorProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cVectorProcessor")
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("inverse","If set to 1, converts magnitude and phase input data to complex frequency domain data",0);
    ct->setField("magnitude","1/0 = compute magnitude yes/no (or use magnitude as input to inverse transformation, must be enabled for inverse)",1);
    ct->setField("phase","1/0 = compute phase yes/no (or use phase as input to inverse transformation, must be enabled for inverse)",0);
  )

  SMILECOMPONENT_MAKEINFO(cFFTmagphase);
}

SMILECOMPONENT_CREATE(cFFTmagphase)

//-----


cFFTmagphase::cFFTmagphase(const char *_name) :
  cVectorProcessor(_name),
  magnitude(1),
  phase(0)
{

}

void cFFTmagphase::fetchConfig()
{
  cVectorProcessor::fetchConfig();
  
  inverse = getInt("inverse");
  magnitude = getInt("magnitude");
  phase = getInt("phase");
  if ((!magnitude)&&(!phase)) { magnitude = 1; }
  if (magnitude) { SMILE_IDBG(2,"magnitude computation enabled"); }
  if (phase) { SMILE_IDBG(2,"phase computation enabled"); }
  if (inverse) { 
    SMILE_IDBG(2,"inverse mode selected"); 
    if (!(magnitude&&phase)) {
      SMILE_IERR(1,"we need magnitude AND phase as inputs for inverse fftmagphase. Thus you need to provide the phase with the input AND enable the option 'phase' in the config file!");
      COMP_ERR("aborting");
    }
  }
}


int cFFTmagphase::setupNamesForField(int i, const char*name, long nEl)
{
  long newNEl = 0; 
  
  // copy frequency axis information for this field...
  const FrameMetaInfo *c = reader->getFrameMetaInfo();

  if (inverse) {
    addNameAppendFieldAuto(name, "fftcInv", (nEl-1)*2);
    newNEl += (nEl-1)*2; 
  } else {
    if (magnitude) {
      addNameAppendFieldAuto(name, "Mag", nEl/2 + 1);
      newNEl += nEl/2 +1; 
      void * _buf = malloc(c->field[i].infoSize);
      memcpy(_buf, c->field[i].info, c->field[i].infoSize);
      writer->setFieldInfo(-1,DATATYPE_SPECTRUM_BINS_MAG,_buf,c->field[i].infoSize);  
    }
    if (phase) {
      addNameAppendFieldAuto(name, "Phase", nEl/2 + 1);
      newNEl += nEl/2 +1; 
      void * _buf = malloc(c->field[i].infoSize);
      memcpy(_buf, c->field[i].info, c->field[i].infoSize);
      writer->setFieldInfo(-1,DATATYPE_SPECTRUM_BINS_PHASE,_buf,c->field[i].infoSize);  
    }
  }
  
  

  return newNEl;
}



// a derived class should override this method, in order to implement the actual processing
int cFFTmagphase::processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  long n;
  
  if (inverse) {
    // we assume the first Nsrc/2+1 inputs to be magnitude and the second Nsrc/2-1 inputs to be phase
    if ( (Nsrc & 1) == 1 ) { 
      SMILE_IERR(1,"number of inputs must be even, please check if you have enabled magnitude and phase input!"); 
      COMP_ERR("aborting");
    }
    const FLOAT_DMEM *mag = src;
    const FLOAT_DMEM *phase = src + Nsrc/2+1;
    long myN = Nsrc/2;

    dst[0] = src[0];
    dst[1] = src[Nsrc/2];
    for (n=1; n<myN; n++) {
      dst[n*2] = cos(phase[n])*mag[n];
      dst[n*2+1] = sin(phase[n])*mag[n];
    }
    
  } else {

    if (magnitude) {
      dst[0] = fabs(src[0]);
      for(n=2; n<Nsrc; n += 2) {
        dst[n/2] = sqrt(src[n]*src[n] + src[n+1]*src[n+1]);
      }
      dst[Nsrc/2] = fabs(src[1]);
      dst += Nsrc/2+1;
      //    dst += Nsrc/2;
    }
    if (phase) {
      dst[0] = 0.0;
      for(n=2; n<Nsrc-1; n++) {
        dst[n/2] = atan(src[n+1]/src[n]);
      }
      //    dst[Nsrc/2] = 0.0;
      //dst += Nsrc/2+1;
    }

  }

  return 1;
}

cFFTmagphase::~cFFTmagphase()
{
}

