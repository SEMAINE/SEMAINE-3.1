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

concatenates vectors from multiple levels and copy to another level

*/


#include <vectorConcat.hpp>

#define MODULE "cVectorConcat"

SMILECOMPONENT_STATICS(cVectorConcat)

SMILECOMPONENT_REGCOMP(cVectorConcat)
{
  SMILECOMPONENT_REGCOMP_INIT

  scname = COMPONENT_NAME_CVECTORCONCAT;
  sdescription = COMPONENT_DESCRIPTION_CVECTORCONCAT;

  // we inherit cVectorProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cVectorProcessor")
  
    SMILECOMPONENT_IFNOTREGAGAIN( {}
    //ct->setField("expandFields", "expand fields to single elements, i.e. each field in the output will correspond to exactly one element in the input [not yet implemented]", 0);
  )

  SMILECOMPONENT_MAKEINFO(cVectorConcat);
}

SMILECOMPONENT_CREATE(cVectorConcat)

//-----

cVectorConcat::cVectorConcat(const char *_name) :
  cVectorProcessor(_name)
{
}


int cVectorConcat::processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  if (dst!=src)
    memcpy( dst, src,  MIN(Ndst,Nsrc)*sizeof(INT_DMEM) );
  return 1;
}

int cVectorConcat::processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  if (dst!=src)
    memcpy( dst, src,  MIN(Ndst,Nsrc)*sizeof(FLOAT_DMEM) );
  return 1;
}

cVectorConcat::~cVectorConcat()
{
}

