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


// Mimicry.h: interface for the Mimicry class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include <iostream>
#include <utility>
#include <vector>
#include "InputData.h"
#include "CentralClock.h"
#include "DataBackchannel.h"

/**
 *  Contains informations on the Mimicry.
 *  When the Trigger send a "ping" to this object, the Mimicry
 *  selects the signal the agent wants to mimic
 *  through a mimicry backchannel
 *  @author Elisabetta Bevacqua elisabetta.bevacqua@libero.it
 *  @date 2008
 */
class Mimicry 
{
public:
	/** 
    * Class  contructor.
    * 
    */
	Mimicry();
	/** 
    * Class destructor.
    * 
    */
	~Mimicry();
	/** 
    * From the data in input about the user's behaviour, this function
	* decides the mimicry behaviour
    * @param data the input data about the event produced by the user (user's behaviour)
    */
	DataBackchannel *GenerateMimicryBehaviour(InputData data);
	/** 
    * It looks at the user's movement to derive the signal to mimic
	* it creates a DataBackchannel object that contains all the information
	* used later by the selector to write a BML
    * @param data the input data about the event produced by the user (user's behaviour)
	* @param time the time of the request of a backchannel signal
    */
	DataBackchannel *Mimicry::CalulateBML(InputData data, int time);

private:
	/** 
    * time when a nod performed by the user has produced a backchannel
	* needed with Watson since it sends a lot of messages to say there's
	* a nod. To avoid the generation of many backchannels for just one nod
    * the following messages containing a nod signals are not considered
	* for three seconds
    */
	int nodtime;
	/** 
    * time when a shake performed by the user has produced a backchannel
	* needed with Watson since it sends a lot of messages to say there's
	* a shake. To avoid the generation of many backchannels for just one shake
    * the following messages containing a shake signals are not considered
	* for three seconds
    */
	int shaketime;
};