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

very basic and low-level cSmileComponent template ...

*/


#include <componentTemplate.hpp>

#define MODULE "myComponentTemplate"

/*Library:
sComponentInfo * registerMe(cConfigManager *_confman) {
  cMyComponent::registerComponent(_confman);
}
*/

SMILECOMPONENT_STATICS(cMyComponent)
                 // vvv!!!!! change cMyComponent to ..... !
SMILECOMPONENT_REGCOMP(cMyComponent)
//sComponentInfo * cMyComponent::registerComponent(cConfigManager *_confman)
{
  SMILECOMPONENT_REGCOMP_INIT
  cname = COMPONENT_NAME_CMYCOMPONENT;
  description = COMPONENT_DESCRIPTION_CMYCOMPONENT;

  // configure your component's configType:
  SMILECOMPONENT_CREATE_CONFIGTYPE
 // OR: inherit a type, e.g. cDataSink:   SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataSink")

  SMILECOMPONENT_IFNOTREGAGAIN_BEGIN
  
  ct->setField("f1", "this is an example int", 0);
  if (ct->setField("subconf", "this is config of sub-component",
                  sconfman->getTypeObj("nameOfSubCompType"), NO_ARRAY, DONT_FREE) == -1) {
     rA=1; // if subtype not yet found, request , re-register in the next iteration
  }

  SMILECOMPONENT_IFNOTREGAGAIN_END

    // vvv!!!!! change cMyComponent to ..... !
  SMILECOMPONENT_MAKEINFO(cMyComponent);
}


SMILECOMPONENT_CREATE(cMyComponent)

// NOTE: use this for abstract classes, e.g. dataSource, etc., which do NOT implement fetchConfig()
//SMILECOMPONENT_CREATE_ABSTRACT(cMyComponent)


// ----

// constructor:
cMyComponent::cMyComponent(const char *_name) : cSmileComponent(_name);
{

}

/*
virtual int cMyComponent::configureInstance()
{
  if (isConfigured()) return 1;
  
  // ....
}

virtual int cMyComponent::finaliseInstance()
{
  if (isFinalised()) return 1;
  
  // ....
}

virtual int tick(long long t)
{
  if (!isFinalised()) return 0;
  
  // ....
}
*/