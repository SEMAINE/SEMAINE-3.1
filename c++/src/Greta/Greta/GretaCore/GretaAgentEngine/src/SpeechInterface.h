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

// 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "APMLCommunicativeAct.h"
#include "APMLCommunicativeActTimingCalc.h"

/**
*
* class :SpeechInterface
*
*/

class SpeechInterface
{
public:

	/**
	* contructor 
	*
	* @param  first_comm_act
	*/

	SpeechInterface(CommunicativeAct* first_comm_act);

	/**
	* destructor 
	*/

	~SpeechInterface(void);


	/**
	* this method 
	* 
	*
	* @return 
	* @param  first_comm_act
	* @param  &calculated_animation_time
	* @param  int &number_of_comm_act
	*/

	int CalculateTimings(CommunicativeAct* first_comm_act,float &calculated_animation_time, int &number_of_comm_act);

	CommunicativeActTimingCalc *timingscalculator;

	CommunicativeAct* first_comm_act;

};
