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

// APML_AgentEngine.h: interface for the APML_AgentEngine class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#pragma warning(disable:4786)

#include "XMLGenericParser.h"
#include "IniManager.h"
#include "FaceEngine.h"
#include "GestureEngine.h"
#include "APMLCommunicativeAct.h"
#include "Phoneme.h"
#include "string.h"
#include "Agent.h"

/**
* class :APML_AgentEngine
*
*/

class APML_AgentEngine  
{

public:

	/**
	* contructor 
	*
	*/
	APML_AgentEngine();

	/**
	* this method 
	* 
	*
	* @return 
	* @param  xmlfile
	*/
	int CalculateTurn(std::string xmlfile);

	/**
	* 
	* @return 
	* @param  buffer
	*/
	int CalculateTurnFromBuffer(std::string buffer);

	/**
	* destructor 
	*/
	virtual ~APML_AgentEngine();

	/**
	* 
	*
	* @return 
	*/
	int CallMary();

	/**
	* this method 
	* 
	*
	* @return 
	*/
	int CallFestival();

	/**
	* this method 
	* 
	*
	* @return 
	*/
	int CallEuler();

	/**
	* this method 
	* 
	*
	* @return 
	*/
	int CallRealSpeech();

	xmlNodePtr apmltreeroot;
	CommunicativeAct *first_comm_act;
	CommunicativeAct *last_comm_act;


	/**
	* this method 
	* 
	*
	* @param root
	*/
	void CreateNewTxtFileWithAccents(XMLGenericTree *root);

	/**
	* this method 
	* 
	*
	* @return 
	* @param xmlfile
	*/
	char* RewriteXMLFile(char *xmlfile);


	/**
	* this method 
	* 
	*
	* @return 
	*/
	int Copy_DTD_in_C_TMP();

	Agent agent;

	phoneme *all_phonemes_list;
	PhonemeVector *PhoVector;


	/**
	* this method 
	* 
	*
	* @return 
	*/
	int CallSpeechSynthesizer();

	float calculated_animation_time;
	int number_of_comm_act;
	int numText;
};
