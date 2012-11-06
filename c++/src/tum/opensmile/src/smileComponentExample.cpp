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

example for a direct descendant of cSmileComponent

no dataMemory interface functionality is used (thus, the tick method may be ignored)

the component may only process messages, if that is desired

*/


#include <smileComponentExample.hpp>

#define MODULE "cSmileComponentExample"

SMILECOMPONENT_STATICS(cSmileComponentExample)

SMILECOMPONENT_REGCOMP(cSmileComponentExample)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CSMILECOMPONENTEXAMPLE;
  sdescription = COMPONENT_DESCRIPTION_CSMILECOMPONENTEXAMPLE;

  // configure your component's configType:
  SMILECOMPONENT_CREATE_CONFIGTYPE
  //ct->setField("reader", "dataMemory interface configuration (i.e. what slot to read from)", sconfman->getTypeObj("cDataReader"), NO_ARRAY, DONT_FREE);
  SMILECOMPONENT_IFNOTREGAGAIN( {} )

  SMILECOMPONENT_MAKEINFO(cSmileComponentExample);
}

SMILECOMPONENT_CREATE(cSmileComponentExample)



//-----

cSmileComponentExample::cSmileComponentExample(const char *_name) :
  cSmileComponent(_name)
{
  // initialisation code...

}

void cSmileComponentExample::fetchConfig() 
{

}

void cSmileComponentExample::mySetEnvironment()
{

}

int cSmileComponentExample::myRegisterInstance()
{

  return 1;
}

int cSmileComponentExample::myConfigureInstance()
{

  return 1;
}

int cSmileComponentExample::myFinaliseInstance()
{

  return 1;
}

int cSmileComponentExample::processComponentMessage( cComponentMessage *_msg ) 
{ 
  if (isMessageType(_msg,"myXYZmessage")) {  // see doc/messages.txt for documentation of currently available messages
   
    // return 1;  // if message was processed
  }
  return 0; // if message was not processed
}  

int cSmileComponentExample::myTick(long long t) 
{

  // return 1;  // tick did succeed!

  return 0; // tick did not succeed, i.e. nothing was processed or there was nothing to process
}

cSmileComponentExample::~cSmileComponentExample()
{
  // cleanup code...

}

