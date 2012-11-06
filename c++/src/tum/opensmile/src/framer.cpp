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

data framer
 
*/


#include <framer.hpp>
//#include <math.h>

#define MODULE "cFramer"


SMILECOMPONENT_STATICS(cFramer)

SMILECOMPONENT_REGCOMP(cFramer)
{
  SMILECOMPONENT_REGCOMP_INIT


  scname = COMPONENT_NAME_CFRAMER;
  sdescription = COMPONENT_DESCRIPTION_CFRAMER;

  // we inherit cWinToVecProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cWinToVecProcessor")
  
  SMILECOMPONENT_IFNOTREGAGAIN( {} 

  )
  
  SMILECOMPONENT_MAKEINFO(cFramer);
}

SMILECOMPONENT_CREATE(cFramer)

//-----

cFramer::cFramer(const char *_name) :
  cWinToVecProcessor(_name)
{

}


// this must return the multiplier, i.e. the vector size returned for each input element (e.g. number of functionals, etc.)
int cFramer::getMultiplier()
{
  return frameSizeFrames;
}

// idxi is index of input element
// row is the input row
// y is the output vector (part) for the input row
int cFramer::doProcess(int idxi, cMatrix *row, FLOAT_DMEM*y)
{
  // copy row to matrix... simple memcpy here
  memcpy(y,row->dataF,row->nT*sizeof(FLOAT_DMEM));
  // return the number of components in y!!
  return row->nT;
}
int cFramer::doProcess(int idxi, cMatrix *row, INT_DMEM*y)
{
  // copy row to matrix... simple memcpy here
  memcpy(y,row->dataI,row->nT*sizeof(INT_DMEM));
  // return the number of components in y!!
  return row->nT;
}


cFramer::~cFramer()
{
}

