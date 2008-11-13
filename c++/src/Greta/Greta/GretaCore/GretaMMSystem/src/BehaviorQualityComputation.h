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
#pragma warning(disable:4786)

#include "BehaviorQualifier.h"
#include "EngineParameterSetOfSets.h"
#include "CommunicativeIntention.h"

/**@#-*/
namespace MMSystemSpace {
/**@#+*/

/**
 *  This is the object that is charged of performing the transformation of the Baseline
 *  into the Dynamicline.
 *  The object is initialized by giving the path of an xml file containing a set of qualifiers
 *  that act on the Baselines depending of the different communicative acts.
 *  Once the object is initialized, it is ready to perform the transformation of
 *  the Baseline into the Dynamicline by calling the corresponding method.
 *  
 *  @author Maurizio Mancini manmau@yahoo.com
 *  @date 2007
 */
class BehaviorQualityComputation
{
public:
	/** 
     * Class contructor.
     * 
     */
	BehaviorQualityComputation();
	BehaviorQualityComputation(std::string filepath);
	/** 
     * Class destructor.
     * 
     */
	~BehaviorQualityComputation(void);
	/** 
     * This function computes the Dynamicline.
	 * @param ca It is the @ref CommunicativeIntention that is used to alter the Baseline and
	 * obtain the Dynamicline.
	 * @param bl It is the input Baseline. The Dynamicline will be obtained by applying
	 * the qualifiers corresponding to the actual CommunicativeIntention @ref ca to the Baseline.
     * 
     */
	Dynamicline ComputeDynamicline(MMSystemSpace::CommunicativeIntention *ca,Baseline bl);
	/** 
     * This function loads the xml file containing the qualifiers that has to be applied to the
	 * Baseline to obtain the Dynamicline.
     * @param std::string filepath The filepath of the file containing the qualifiers 
	 * @param char *buffer The buffer containing the qualifiers to load
     */
	int LoadQualifiersList(std::string filepath,char *buffer);
private:
	/** 
     * This structure is the search map of the qualifiers loaded from the xml file.
     * 
     */
	BehaviorQualifierMap qualifiers;
	/** 
     * A function for eliminating spaces and newlines from a string.
     * @param std::string &s the adress of the string to alter
     */
	void RemoveSpacesAndNewlines(std::string &s);
	/** 
     * Logfile of the object. Data is saved in html format.
     * 
     */
	FILE *logfile;
	/** 
     * This function adds a qualifier to the search map of the modifiers.
	 * @param qual The qualifier to add to the search map.
     * 
     */
	int AddQualifier(BehaviorQualifier qual);
	/** 
     * It performs a mathematical operation on the given operands and puts
	 * the result in the destination parameter.
	 * @param destination The result of the operation will be stored here.
	 * It is a pointer to a float.
     * @param operat It is the name of the operation to be performed.
	 * Allowed operators are listed in the file @link global.xsd.
	 * @param op1,op1,op3 The values of the operands involved in the operation.
     */
	void ComputeOperation(float *destination,std::string operat,float op1,float op2,float op3);
	/** 
     * Function that limits the value of the first parameter by the values of the
	 * other two.
	 * @param val The value to be limited.
	 * @param min,max the lower and upper limits to apply to the first parameter.
	 */
	float Limit(float val,float min,float max);
	/** 
     * Just for debugging, it prints out the map of the qualifiers.
     * 
     */
	void PrintQualifiers();
	/** 
     * It returns the behavior qualifier corresponding to the given
	 * communicative intention.
     * 
     */
	BehaviorQualifier *FromCAtoQualifier(MMSystemSpace::CommunicativeIntention ca);
};

/**@#-*/
}
/**@#+*/