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

#include "XMLGenericParser.h"

#include <string>

/**
*
* class :Agent
*
*/

class Agent
{
public:

	/**
	* contructor 
	*
	*/

	Agent(void);

	/**
	* destructor 
	*/

	~Agent(void);

	/**
	* 
	*
	* @return 
	* @param  xmlfilename
	*/

	int Load(std::string xmlfilename);
	//general informations
	std::string name;
	enum GenderType {male,female};
	GenderType gender;
	int age;
	enum HandType {left,right};
	HandType hand;
	std::string origin;
	//general expressivity tendency
	float general_OAC;
	float general_SPC;
	float general_TMP;
	float general_FLD;
	float general_PWR;
	float general_REP;
	//for the lips
	enum LipArticulationTensionType {low,medium,strong};
	LipArticulationTensionType liptension;
	LipArticulationTensionType liparticulation;
	//modality preference
	float hierarchy_face;
	float hierarchy_gesture;
	float hierarchy_posture;
	float hierarchy_gaze;
	float hierarchy_head;
	//influence of expressivity in each modality
	float factor_face;
	float factor_gesture;
	float factor_posture;
	float factor_gaze;
	float factor_head;

};
