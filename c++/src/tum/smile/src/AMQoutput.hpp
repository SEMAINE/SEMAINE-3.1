/*******************************************************************************
 * openSMILE
 *  - open Speech and Music Interpretation by Large-space Extraction -
 * Copyright (C) 2008  Florian Eyben, Martin Woellmer, Bjoern Schuller
 * 
 * Institute for Human-Machine Communication
 * Technische Universitaet Muenchen (TUM)
 * D-80333 Munich, Germany
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
 *******************************************************************************/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ActiveMQ feature output support for SEMAINE architecture

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
#ifndef __AMQ_OUTPUT_HPP
#define __AMQ_OUTPUT_HPP

#include <stdio.h>
#include <stdlib.h>

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
    
    int sendCurrentFrame(int speakingStatus);
    int sendLastFrame();
	int sendLastFrame(int speakingStatus);
        
    ~cAmqOutput ();

  private:
    MetaMessenger * meta;
    FeatureSender * featureSender;
//    int seqNr;
};


#endif // #ifndef __AMQ_OUPTUT_HPP
