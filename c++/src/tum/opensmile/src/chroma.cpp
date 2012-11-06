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


/*  openSMILE component: cChroma

Component to compute chroma features from a semi-tone spectrum. 

This component is based on original code from Moritz Dausinger (wave2chroma). 
The openSMILE chroma component was implemented by Christoph Kozielski.

*/


#include <chroma.hpp>

#define MODULE "cChroma"

/*Library:
sComponentInfo * registerMe(cConfigManager *_confman) {
  cDataProcessor::registerComponent(_confman);
}
*/

SMILECOMPONENT_STATICS(cChroma)

SMILECOMPONENT_REGCOMP(cChroma)
{
  if (_confman == NULL) return NULL;
  int rA = 0;

  sconfman = _confman;
  scname = COMPONENT_NAME_CCHROMA;
  sdescription = COMPONENT_DESCRIPTION_CCHROMA;

  // we inherit cVectorProcessor configType and extend it:
  ConfigType *ct = new ConfigType( *(sconfman->getTypeObj("cVectorProcessor")) , scname );
  if (ct == NULL) {
    SMILE_WRN(4,"cVectorProcessor config Type not found!");
    rA=1;
  }
  if (rA==0) {
    ct->setField("nameAppend",NULL,"chroma");
    ct->setField("copyInputName",NULL,0);
    ct->setField("octaveSize","The size of an octave, i.e. the number of output bins, or the interval to which the input bins are mapped via warping.",12);
    ct->setField("silThresh","Chroma 'floor', i.e. silence threshold. If at least on element of the (unnormalised) chroma vector falls below the threshold, the vector will not be normalised, and a 0 vector will be output instead.",0.001); // chroma toolbox uses 0.001 ?
    ConfigInstance *Tdflt = new ConfigInstance( scname, ct, 1 );
    sconfman->registerType(Tdflt);
  } 

  SMILECOMPONENT_MAKEINFO(cChroma);
}

SMILECOMPONENT_CREATE(cChroma)

//-----

cChroma::cChroma(const char *_name) :
  cVectorProcessor(_name), silThresh(0.0)
{

}

void cChroma::fetchConfig()
{
  cVectorProcessor::fetchConfig();
  
  silThresh = (FLOAT_DMEM)getDouble("silThresh");

  octaveSize = getInt("octaveSize");
  SMILE_IDBG(2,"octaveSize = %i",octaveSize);
}


int cChroma::setupNamesForField(int i, const char*name, long nEl)
{
  return cVectorProcessor::setupNamesForField(i,name,octaveSize);
}



// a derived class should override this method, in order to implement the actual processing
int cChroma::processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
// input: power of tonespec...!

  int i,j;
  double sum = 0.0;
  int silFlag = 0;
  
  nOctaves = Nsrc / octaveSize;
  if (Nsrc % octaveSize == 0) {
    for (i = 0;i < octaveSize; i++) {
      dst[i] = 0.0;
      for (j = 0;j < nOctaves; j++) {
        dst[i] += src[j * octaveSize + i];
      }
      //sum += (dst[i]*dst[i]); // <- old pre 1.0.0 code
      if (dst[i] < silThresh) silFlag = 1;
      sum += (double)dst[i];
    }
    if ((sum != 0.0)&&(!silFlag)) {
      //double power = sqrt(sum);  // <- old pre 1.0.0 code
      for (i = 0;i < octaveSize; i++)
        dst[i] = dst[i] / (FLOAT_DMEM)sum;
        //dst[i] = dst[i] / (FLOAT_DMEM)power;  // <- old pre 1.0.0 code
    } else {
      for (i = 0;i < octaveSize; i++) dst[i] = 0.0;
    }
  } else {
    SMILE_IERR(1,"cannot compute Chroma features from an input frame with a size (%i) no an integer multiple of 'octaveSize' (%i)! Check your tonespec configuration!",Nsrc,octaveSize);
  }

  return 1;
}

cChroma::~cChroma()
{
}
