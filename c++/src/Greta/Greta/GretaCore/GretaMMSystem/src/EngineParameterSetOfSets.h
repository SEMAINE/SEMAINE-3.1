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
#include <map>
#include "EngineParameter.h"
#include "EngineParameterSet.h"

/**@#-*/
namespace MMSystemSpace {
/**@#+*/

/**
 *  A set of behavior EngineParameterSetOfSets.
 *  Behavior EngineParameterSetOfSets are the channels involved in the communication between humans.
 *  In the present system we use five EngineParameterSetOfSets (excluding speech): face, head, gaze, gestures, body.
 *  This class is an extension of the stl#map class.
 *  @author Maurizio Mancini manmau@yahoo.com
 *  @date 2007
 */
class EngineParameterSetOfSets : public std::map<std::string,EngineParameterSet>
{
public:
	/** 
    * Class contructor.
    * 
    */
	EngineParameterSetOfSets(void);
	/** 
    * Class destructor.
    * 
    */
	~EngineParameterSetOfSets(void);

	/** 
    * It loads the definition of the EngineParameterSetOfSets from a file.
	* The format of this file is defined in @link profiles.xsd
    * 
    */
	int Load(std::string filepath,char *buffer);
	//int LoadFromFile(std::string filepath);
	int LoadFromBuffer(std::string buffer);
	/** 
    * It returns the pointer to the EngineParameterSet specified in the parameter.
    * 
    */
	EngineParameterSet *GetEngineParameterSet(std::string name);

	/** 
    * It returns the value of the attribute specified in the parameter
	* @param name Is the parameter name in the form: EngineParameterSetname.parameter.attribute
    * 
    */
	float GetAttrValue(std::string name);

	/** 
    * It set the value of the attribute specified in the parameter.
	* @param name Is the parameter name in the form: EngineParameterSetname.parameter.attribute
	* @param value Is the value the attribute name is set with 
    * 
    */
	int SetAttrValue(std::string name, std::string value);

	/** 
    * It prints all the EngineParameterSetOfSets in the search map o a string in XML format.
    * 
    */
	std::string PrintXML();

	/** 
    * It prints all the EngineParameterSetOfSets in the search map.
    * 
    */
	void Print();
	/** 
    * \deprecated It saves the parameters of the different EngineParameterSetOfSets to separate files.
	* Of course this method works only if these EngineParameterSetOfSets (gesture, torso,
	* head, face and gaze) are present. It is not a general method, so it should
	* be used only by expert users.
    * 
    */
	void SaveCSV(float time,FILE *preferencefile,FILE *gesturefile,FILE *torsofile,FILE *headfile,FILE *facefile,FILE *gazefile);
};

/** 
* Name definition, the name Baseline is just a synonym of EngineParameterSetOfSets.
* 
*/
typedef EngineParameterSetOfSets Baseline;

/** 
* Name definition, the name Dynamicline is just a synonym of EngineParameterSetOfSets.
* 
*/
typedef EngineParameterSetOfSets Dynamicline;

/**@#-*/
}
/**@#+*/