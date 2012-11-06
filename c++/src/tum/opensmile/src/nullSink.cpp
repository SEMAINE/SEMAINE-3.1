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


/*  openSMILE component: cNullSink
----------------------------------
NULL sink: reads data vectors from data memory and destroys them without writing them anywhere
----------------------------------

11/2009  Written by Florian Eyben
*/


#include <nullSink.hpp>

#define MODULE "cNullSink"


SMILECOMPONENT_STATICS(cNullSink)

SMILECOMPONENT_REGCOMP(cNullSink)
{
  SMILECOMPONENT_REGCOMP_INIT

  scname = COMPONENT_NAME_CNULLSINK;
  sdescription = COMPONENT_DESCRIPTION_CNULLSINK;

  // we inherit cDataSink configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataSink")
  
  SMILECOMPONENT_IFNOTREGAGAIN( {} )

  SMILECOMPONENT_MAKEINFO(cNullSink);
}

SMILECOMPONENT_CREATE(cNullSink)

//-----

cNullSink::cNullSink(const char *_name) :
  cDataSink(_name)
{
}

int cNullSink::myTick(long long t)
{
  SMILE_IDBG(4,"tick # %i, reading value vector to NULL sink",t);
  cVector *vec= reader->getNextFrame();
  if (vec == NULL) return 0;
  else 
    // tick success
    return 1;
}


cNullSink::~cNullSink()
{
}

