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
#pragma warning(disable:4786)
#include "XMLGenericParser.h"
#include "Signal.h"
#include "CommunicativeIntention.h"
#include <string>
#include <map>

#include "IniManager.h"
#include "RandomGen.h"
#include "FileNames.h"
#include "BehaviorQualityComputation.h"
#include "MultimodalSignalSelection.h"
//#include "GMTextElement.h"
#include <sstream>

/**
*
* class :FMLAPML_AgentEngine
*
*/

class FMLAPML_AgentEngine
{
public:

	/**	
	* contructor 
	*
	*/

	FMLAPML_AgentEngine(void);

	/**
	* destructor 
	*/

	~FMLAPML_AgentEngine(void);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  FMLAPMLfilename
	* @param  baselinefilename
	* @param buffer
	* @param  int realtime
	*/

	std::string FMLAPML_AgentEngine::Execute(std::string FMLAPMLfilename,std::string baselinefilename,char *buffer, int realtime);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  baselinefilename
	* @param buffer
	*/

	int loadBaseline(std::string baselinefilename,char *buffer);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  bqcfilename
	* @param buffer
	*/

	int loadBQC(std::string bqcfilename,char *buffer);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  mmsfilename
	* @param buffer
	*/

	int loadMMS(std::string mmsfilename,char *buffer);

	/**
	* this method 
	* 
	*
	* @return 
	*/

	int reset();

	/**
	* this method 
	* 
	*
	* @return 
	* @param buffer
	*/

	std::string Execute(char *buffer);
private:

	/**
	* this method 
	* 
	*
	* @return 
	* @param  FMLAPMLfilename
	* @param buffer
	*/

	int LoadFMLAPML(std::string FMLAPMLfilename,char *buffer);

	/**
	* this method 
	* 
	*
	* @param t
	*/

	void StoreCommunicativeIntentions(XMLGenericTree *t);

	/**
	* this method 
	* 
	*
	* @param t
	* @param  tagsname
	*/

	void StoreCommunicativeIntentions(XMLGenericTree *t,std::string tagsname);

	/**
	* this method 
	* 
	*
	* @return 
	*/

	int TemporizeCommunicativeIntentions(void);

	/**
	* this method 
	* 
	*
	* @return 
	*/

	void DefineCommunicativeContexts(void);
	MMSystemSpace::Signal speech;	
	MMSystemSpace::Baseline bl;
	MMSystemSpace::BehaviorQualityComputation bqc;
	MMSystemSpace::MultimodalSignalSelection mss;
	MMSystemSpace::Dynamicline dl;
	std::vector<MMSystemSpace::CommunicativeIntention> commacts;
	XMLGenericTree *apml;
	FILE *logfile;
};
