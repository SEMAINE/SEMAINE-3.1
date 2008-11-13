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
#include <list>
#include <vector>

/**@#-*/
namespace MMSystemSpace {
/**@#+*/

/**
 *  Contains informations on a single communicative intention.
 *  The communicative intention can have a unique ID and/or the name of the class of communicative intentions
 *  it belongs to and/or the name of the instance. The representation class-instance is typical
 *  of languages like APML. For other languages these fields can be left blank and the ID can
 *  be used instead. The other fields contain information on the communicative intention level
 *  of importance, the time in which it starts and its duration.
 *  @author Maurizio Mancini manmau@yahoo.com
 *  @date 2007
 */
class CommunicativeIntention
{
public:
	/** 
     * Class contructor.
     * 
     */
	CommunicativeIntention(void);
	/** 
     * Class destructor.
     * 
     */
	~CommunicativeIntention(void);
	/** 
     * The < operator. Used to apply the stl sorting algorithm
	 * to any list of communicative intentions.
     * 
     */
	bool operator < (CommunicativeIntention&);
	/**
    * The id of the communicative intention.
	* Depending on the application, this could be a unique ID.
    */
	std::string id;
	/**
    * The name of the class of the communicative act.
	* This is usually used when working with APML.
	* @see InstanceName
    */
	std::string name;
	/**
    * Every communicative intention has a type attribute.
	*
    */
	std::string type;
	/**
    * Some communicative intentions may have a target attribute.
	*
    */
	std::string target;
	/**
    * Some communicative intentions may have a reference type attribute.
	*
    */
	std::string ref_type;
	/**
    * Some communicative intentions may have a reference id attribute.
	*
    */
	std::string ref_id;
	/**
    * Some communicative intentions may have a property type attribute.
	*
    */
	std::string prop_type;
	/**
    * Some communicative intentions may have a property value attribute.
	*
    */
	std::string prop_value;
	
	
	/**
    * The communicative act's context.
	* It is a list of pointers of the communicative acts
	* that overlap in time with this communicative act.
	* They will be considered during the computation
	* of the signals associated to the communicative act.
    */
	
	std::list<CommunicativeIntention*> CommunicativeContext;
	/**
    * Method for computing the start time and duration of
	* the communicative act. It uses a search map containing
	* all the other communicative acts and the content
	* of the speech tag of the bml part of fml-apml.
	* There is a counter to avoid deadlock in recursion in case
	* of looping references between tags.
    */
	int Temporize(std::vector<MMSystemSpace::CommunicativeIntention> &commacts,void *speech,int *loopnum);
	/**
    * Submethod that performs some operations on the
	* start or duration of a communicative act.
	* This method could be included in the Temporize method
	* but it has bees splitted here to avoid code
	* redundancy.
    */
	int TemporizeAttribute(float *attribute,std::string *attribute_sym,std::vector<MMSystemSpace::CommunicativeIntention> &commacts,void *speech,int *loopnum);
	/**
    * This method searches for communicative acts that overlap in
	* time with the current act. It fills up the list of the
	* overlapping communicative acts associated to this act.
	*/
	void DefineContext(std::vector<MMSystemSpace::CommunicativeIntention> &commacts);
	/**
    * It looks for the given communicative intention in the given list of
	* communicative intentions. It uses the act id to compare the intentions.
	*
    */
	CommunicativeIntention *GetAct(std::vector<CommunicativeIntention> &commacts,std::string id);
	/**
    * (not used) Starting from the communicative intentions in the context
	* of this communicative intention (the context is usually the set of intentions
	* co-occurring with this) it returns the first pitch accent communicative intention.
	*
    */
	CommunicativeIntention *GetPitchAccentFromContext();
	/**
    * It returns the context of the communicative intention. The context
	* is composed by those intentions which overlap in time with the communicative intention.
	*
    */
	std::list<std::string> GetContextUsedModalities();
	/**
    * It looks for the given communicative intention in the given list of
	* communicative intentions. It uses the act id to compare the intentions.
	*
    */
	CommunicativeIntention *GetActFromContext(std::string name);
	/**
    * It prints the communicative intention to a text file.
	*
    */
	void Print(FILE *f);
	/**
    * The importance of the communicative act.
	* It has to be a floating point number between 0 and 1.
    */
	float importance;
	/**
    * Absolute starting time of the communicative act.
	* It is represented in seconds.
    */
	float start;
	/**
    * Duration time of the communicative act. 
	* It is represented in seconds.
    */
	float duration;
	/**
    * The starting time of the intention can be a string referring to
	* another intention.
	*
    */
	std::string start_sym;
	/**
    * The duration time of the intention can be a string referring
	* to another intention.
	*
    */
	std::string dur_sym;
	/**
    * This flag is true if and only if the communicative intention has
	* the correct timing.
	*
    */
	bool concretized;
	/**
    * This flag is true if and only if the communicative intention
	* has already been translated into multimodal signals.
	*
    */
	bool instantiated;
	/**
    * This flag is true if the communicative intention has not
	* an ending time.
	*
    */
	bool noend;
	/**
    * This flag is true if and only if the communicative intention
	* corresponds to communicating (in any way) an emotional state.
	*
    */
	bool isemotion;
	/**
    * This flag is true if and only if the communicative intention
	* corresponds to communicating (in any way) an emotional state.
	*
    */
	bool isemphasis;
	/**
    * This flag is true if and only if the communicative intention
	* corresponds to communicating (in any way) an emotional state.
	*
    */
	bool ispitchaccent;
	/**
    * This is the list of modalities names that are used for communicating
	* the communicative intention.
	*
    */
	std::list<std::string> UsedModalities;

	/**
    * Some communicative intentions may have an intensity.
	*
    */

	//added by radek

private:

	float intensity;

public:

	void SetIntensity(float intensity1);
    float GetIntensity();

};

/**@#-*/
}
/**@#+*/