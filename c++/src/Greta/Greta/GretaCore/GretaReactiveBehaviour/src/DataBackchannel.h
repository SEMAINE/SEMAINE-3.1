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


// DataBackchannel.h: interface for the DataBackchannel class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include <iostream>
#include <utility>
#include <vector>
#include <map>
#include <string>

/**
 *  Contains informations on the DataBackchannel.
 *  It contains the necessary data to write a backchannel
 *  in BML or FML
 *  @author Elisabetta Bevacqua elisabetta.bevacqua@libero.it
 *  @date 2008
 */
class DataBackchannel
{
public:
	/** 
    * Class default contructor.
    * 
    */
	DataBackchannel();
	/** 
    * Class contructor.
    * @ param type the type of backchannel: MIMICRY REACTIVE COGNITIVE
    */
	DataBackchannel(std::string type);
	/** 
    * Class destructor.
    * 
    */
	virtual ~DataBackchannel();
	/** 
    * Print DataBackchannel data for debugging purpose
    * 
    */
	void DataBackchannel::Print();
	/** 
    * Copy the data
    * @param db the DataBackchannel object from which copy the data
    */
	void DataBackchannel::CopyDataBackchannel(DataBackchannel *db);
	/** 
    * Clean all data in a DataBackchannel object
    * 
    */
	void DataBackchannel::CleanDataBackchannel();
	


	/** 
    * Time of the backchannel request
    * 
    */
	int time;
	/** 
    * Type of the backchannel MIMICRY, REACTIVE or COGNITIVE
    * 
    */
	std::string type;
	/** 
    * Backchannel priority
    * 
    */
	int priority;
	/** 
    * Channel on which signal must be performed - for BML only
    * 
    */
	std::string modality;
	/** 
    * Name of the signal to perform - for BML only
    * 
    */
	std::string reference;
	std::map<std::string, std::string> referencesMap;
	/** 
    * Duration of the backchannel
    * 
    */
	std::string end;
	/** 
    * Name of the event that elicited the backchannel
    * 
    */
	std::string zone;
	/** 
    * Agent's emotional state (not used yet)
    * 
    */
	std::string emotionalstate;
	/** 
    * List of the communicative intentions the agent wants to transmit
	* through a reactive backchannel - for FML only
    * 
    */
	std::vector<std::string> communicativefunction;

};