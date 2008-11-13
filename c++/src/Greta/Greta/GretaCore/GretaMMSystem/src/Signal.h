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
#include <vector>
#include "EngineParameterSet.h"
#include "EngineParameter.h"
#include "XMLGenericParser.h"
#include "IniManager.h"

/**@#-*/
namespace MMSystemSpace {
/**@#+*/

/**
 * A Signal represents a specific behavior (speech, facial expression,
 * hand gesture, head movement and so on) performed by the
 * agent.
 * A signal contains timing information for the beginning and
 * end of the signal, for the strokes positions, the reference
 * of the signal in a repository, and other things.
 * It also contains a set of parameters that can contain
 * any kind of useful information, for example the expressivity
 * values.
 *  
 *  @author Maurizio Mancini manmau@yahoo.com
 *  @date 2007
 */
class TimeMarker
{
public:
	TimeMarker(void);
	~TimeMarker(void);
	std::string id;
	float time;
	bool concretized;
};

class prosodystructure {
public:
	std::string start;
	std::string end;
	std::string type;
};

class alternativeshape {
public:
	std::string name;
	float probability;
	alternativeshape()
	{
		probability=0;
	};
};

class Signal
{
public:
	/** 
    * Class contructor.
    * 
    */
	Signal(void);
	/** 
    * Copy contructor.
    * 
    */
	Signal(const Signal &rhs);
	/** 
    * Class destructor.
    * 
    */
	~Signal(void);
	bool operator<(Signal& a);
	bool operator>(Signal& a);
	/** 
    * The modality on which the signal should be emitted.
    * 
    */
	std::string modality;
	/** 
    * The type of the signal.
    * 
	* Depending on the modality on which the signal is produced
	* this parameter will contain different things. For example
	* for the gesture modality the type indicates if the gesture
	* is a deictic, iconic, etc. For the face is inicates which
	* part of the face is used to perform a facial expression.
    */
	std::string type;
	
	/** 
    * Specific for the gaze modality.
    * 
    */
	std::string direction;
	/** 
    * Specific for the gesture modality.
    * 
    */
	std::string shape;
	/** 
    * Specific for the torso modality.
    * 
    */
	std::string posture;
	/** 
    * Specific for the speech modality.
    * 
    */
	std::string language;
	/** 
    * Specific for the speech modality.
    * 
    */
	std::string voice;
   /** 
    * Specific for the face modality ?
    * 
    */
	float intensity;
	/** 
    * Specific for the speech modality.
    * 
    */
	float speed_num;
	/** 
    * Specific for the speech modality.
    * 
    */
	std::string speed_sym;
	/** 
    * Specific for the speech modality.
    * 
    */
	std::string text;
	/** 
    * Specific for the speech modality. It contains the set
	* of time markers of the speech signal, used by the other
	* modalities to synchronize to the uttered text.
    * 
    */
	std::map<std::string,TimeMarker> timemarkers;
	/** 
    * This is an external reference to a repository
	* containing the description of the signal.
    * 
    */
	std::string reference;
	std::vector<alternativeshape> alternativeshapes;
	/** 
    * A unique id for the signal.
    * 
    */
	std::string id;
	/** 
    * A search map containing the parameters of the signal.
	* We use it almost always to contain the expressivity parameters
	* of the signal.
    * 
    */
	EngineParameters parameters;
	/** 
    * Start time of the signal. It is an absolute time.
    * 
    */
	float start;
	/** 
    * Duration of the signal, relative to the start time.
    * 
    */
	float duration;
	bool noend;
	/** 
    * Start time of the signal. It is an absolute time.
    * 
    */
	std::string start_sym;
	/** 
    * Duration of the signal, relative to the start time.
    * 
    */
	std::string duration_sym;
	/**
	* A vector containing the strokes associated to the signal.
	*/
	std::vector<float> strokes;
	std::vector<float> stressingpoints;
	std::string BMLcode;
	std::list<prosodystructure> boundaries;
	std::list<prosodystructure> pitchaccents;
	bool concretized;
	/** 
    * It returns the value of the parameter specified by the argument.
    * 
    */
	float GetParamValuef(std::string name);
	/** 
    * It sets the parameter specified by the argument name to
	* the specified value.
    * 
    */
	void SetParameterf(std::string name, float value);
	/** 
    * It sets the parameter specified by the argument name to
	* value specified in the string. The string should contain
	* numeric values anyway.
    * 
    */
	void SetParameter(std::string name, std::string value);
	/** 
    * Given some BML code stored in a tree structure
	* it stores the BML data into the signal.
    * 
    */
	int StoreBML(XMLGenericTree *t,IniManager inimanager);

	/**
    * Method for computing the start time and duration of
	* the signal. It uses a vector containing
	* all the other signals and the content
	* of the speech tag.
	* There is a counter to avoid deadlock in recursion in case
	* of looping references between tags.
    */

	int Temporize(std::vector<Signal> &signals,void *speech,int *loopnum);
	int Temporize(std::vector<Signal*> &signals,void *speech,int *loopnum);
	/**
    * Submethod that performs some operations on the
	* start or duration of a signal.
	* This method could be included in the Temporize method
	* but it has bees splitted here to avoid code
	* redundancy.
    */

	void Clear();
	/**
	*Method for cleaning the signal
	*/
private:
	int TemporizeAttribute(float *attribute,std::string *attribute_sym,std::vector<Signal> &signals,void *speech,int *loopnum);
	int TemporizeAttribute(float *attribute,std::string *attribute_sym,std::vector<Signal*> &signals,void *speech,int *loopnum);
public:
	Signal *GetSignal(std::vector<Signal> &signals,std::string id);
	Signal *GetSignal(std::vector<Signal*> &signals,std::string id);

	//commented because there was already the flag "noend" to
	//specify that the signal has no end (the same meaning!)
	//I replaced idle with noend

	//bool idle;

	std::string GetBML(int *uid);

	float GetTimeMarker(std::string name);

	void StoreTimeMarkers(XMLGenericTree *t);

	void StoreTimeMarkersTimings(std::list<TimeMarker> *tml);

	std::string ToMaryXML();

	int TemporizeTimeMarkers(float time=0.0f);

	/** 
    * generates speech using wav file
    * 
    */
	int GenerateSpeech(std::string wavefilename);

	/** 
    * generates speech without using wav file
    * char - content of the speech
	* size lunghezza of the speech
    */
	char *GenerateSpeechToChar( int *size);
	
	void ShuffleShape();

	/** 
    * It returns the value of the intensity.
    * 
    */
	float GetIntensity();
   /** 
    * It allows todefine the value of the intensity.
    * 
    */
	void SetIntensity(float intensity1);

	Signal *clone();

	bool intersect(float begin, float end);
	bool intersect(float begin);

};

/** 
* List of Signal objects.
* 
*/
typedef std::list<Signal> SignalList;

typedef std::vector<Signal> SignalVector;

/**@#-*/
}
/**@#+*/