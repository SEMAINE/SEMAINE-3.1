//Copyright 1999-2008 Catherine Pelachaud - c.pelachaud@iut.univ-paris8.fr
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

// GazePseudoBelief.h: interface for the GazePseudoBelief class.
//
//////////////////////////////////////////////////////////////////////

#pragma once


/**
*
* class :GazePseudoBelief
*
*/

class GazePseudoBelief
{
	int previousgazespeaker;
	int previousgazelistener;
	int movespeaker;
	int movelistener;
	int speakerduration;
	int listenerduration;

	float belief_probability[64][10];

public:


/**
*
* contructor 
*
*/

	GazePseudoBelief();
	

/**
*
* destructor 
*/

	virtual ~GazePseudoBelief();
	

/**
 *
 *
* @param  value
*/

	void setPreviousGazeSpeaker(int value);

/**
 *
 *
* @param  value
*/

	void setPreviousGazeListener(int value);

/**
 *
 *
* @param  value
*/

	void setMoveSpeaker(int value);

/**
 *
 *
* @param  value
*/

	void setMoveListener(int value);

/**
 *
 *
* @param  value
*/

	void setSpeakerDuration(int value);

/**
 *
 *
* @param  value
*/

	void setListenerDuration(int value);
	

/**
 *
 *
* @return 
* @param  value
*/

	float getValue(int value);


/**
 *
 *
*/

	void clear();
};
