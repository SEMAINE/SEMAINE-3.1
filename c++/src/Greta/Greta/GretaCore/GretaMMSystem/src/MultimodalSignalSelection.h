//Copyright 1999-2008 Catherine Pelachaud - c.pelachaud@iut.univ-paris8.fr
//
//This file is part of Greta.
//
//Greta is free software; you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation; either version 2 of the License, or
//(at your option) any later version.
//
//Greta is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with Greta; if not, write to the Free Software
//Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#pragma once

#include <string>
#include <list>
#include <map>
#include "Signal.h"
#include "CommunicativeIntention.h"
#include "BehaviorSet.h"
#include "MultimodalSignal.h"


/**@#-*/
namespace MMSystemSpace {
/**@#+*/

/**
 *  It selects the multimodal signals that should be used for conveying the given
 *  communicative intention. This object loads a lexicon of MMSystemSpace#BehaviorSet
 *  objects. Then you can call the method MultimodalSignalSelection#SelectMultimodalSignals
 *  (there is an "s" at the end of the name)
 *  to obtain the multimodal signals that can be used to convey the given intention.
 *  However you can go a step further, and call the method MultimodalSignalSelection#SelectMultimodalSignal
 *  (no "s" at the end this time) to select only one multimodal signal between the ones
 *  you obtained with the previous method. To perform this selection the system uses
 *  some data from the agent's Dynamicline.
 *  @author Maurizio Mancini manmau@yahoo.com
 *  @date 2007
 */
class MultimodalSignalSelection
{
public:
	/** 
    * Class contructor. The filepath
	* parameter is the path of the lexicon file
	* containing the behavior sets that should be used to perform the object
	* operations.
    * 
    */
	MultimodalSignalSelection();
	MultimodalSignalSelection(std::string filepath);
	/** 
    * Class destructor.
    * 
    */
	~MultimodalSignalSelection(void);
	/** 
    * It selects the multimodal signal that could be used to convey the given
	* communicative intention.
	* \param possiblesignals the set of multimodal signals among which
	* the selection will be done.
    * \param ca the communicative intention we want to convey.
	* \param dl the agent's Dynamicline
    */
	MultimodalSignal *SelectMultimodalSignal(std::vector<MultimodalSignal*> *possiblesignals,MMSystemSpace::CommunicativeIntention ca,Dynamicline *dl);
	
	std::vector<MultimodalSignal*> *SelectMultimodalSignals(MMSystemSpace::CommunicativeIntention ca,
								Dynamicline *dl,
								std::list<std::string> &BusyModalities);

	int LoadSelectionLexicon(std::string filepath,char *buffer);

	BehaviorSet *FromCAtoBehaviorSet(MMSystemSpace::CommunicativeIntention ca);
	
	FILE *logfile;

	BehaviorSetMap BehaviorSets;

private:
	int max_num_signals;
};

/**@#-*/
}
/**@#+*/
