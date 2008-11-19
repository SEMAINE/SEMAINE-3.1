/*******************************************************************************
 * feaTUM, fast, efficient audio feature extractor by TUM
 * Copyright (C) 2008  Florian Eyben, Martin Woellmer
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
 *******************************************************************************/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
#ifndef __AMQ_OUTPUT_HPP
#define __AMQ_OUTPUT_HPP

#include <stdio.h>
#include <malloc.h>

#include <semaine/config.h>
#include <cms/CMSException.h>
#include <semaine/components/Component.h>
#include <semaine/cms/sender/FeatureSender.h>

extern "C" {
#include <tum/smile/src/featum_common.h>
#include <tum/smile/src/featum_util.h>
}

#include <tum/smile/src/feature-memory.hpp>
#include <tum/smile/src/GENoutput.hpp>

class cAmqOutput : public cGenOutput {
  
  public:
    // _data is declared in base class cGenOutput !!
    
    cAmqOutput( cFeatureMemory &mem, FeatureSender * _featureSender, MetaMessenger * _meta );
    
    static int writeFrame( pGenOutput obj, pOutputVector vec );
    
    int sendCurrentFrame();
    int sendLastFrame();
        
    ~cAmqOutput ();

  private:
    MetaMessenger * meta;
    FeatureSender * featureSender;

};


#endif // #ifndef __AMQ_OUPTUT_HPP
