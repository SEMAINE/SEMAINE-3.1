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

// FaceEngine.h: interface for the FaceEngine class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "FaceEngineGlobals.h"
#include "FaceAnimationGenerator.h"
#include "APMLCommunicativeAct.h"
#include <stdio.h>
#include <libxml/xmlmemory.h>

/**
*
* class :FaceEngine
*
*/

class FaceEngine  
{
public:

	/**
	*
	* contructor 
	*
	* @param first_comm_act
	* @param last_comm_act
	* @param  bml
	*/

	FaceEngine(CommunicativeAct *first_comm_act,CommunicativeAct *last_comm_act,bool bml);


	/**
	*
	*
	* @return 
	* @param  fapfilename
	* @param  animationlength
	* @param PhoVector
	* @param phonemes
	* @param  int bml
	* @param  speech_delay=0.0f
	*/

	int FaceEngine::CalculateTurn(std::string fapfilename,float animationlength,PhonemeVector *PhoVector,phoneme *phonemes, int bml,float speech_delay=0.0f);


	/**
	*
	*
	* @param  target
	*/

	void FaceEngine::InvertSlashes(char* target);


	/**
	*
	* destructor 
	*/

	virtual ~FaceEngine();


	/**
	*
	*
	* @return 
	* @param  fapfilename
	*/

	int WriteFapFile(std::string fapfilename);


	/**
	*
	*
	* @return 
	*/

	std::vector<FAPFrame> *GetAnimation()
	{
		return FAPFileGenerator->GetAnimation();
	};


	/**
	*
	*
	* @return 
	*/

	std::vector<FAPFrame> *GetAnimationWithoutNoise()
	{
		return FAPFileGenerator->GetAnimationWithoutNoise();
	};

	FaceAnimationGenerator *FAPFileGenerator;

	CommunicativeAct *first_comm_act;

	CommunicativeAct *last_comm_act;


	/**
	*
	*
	* @param frame
	* @param  bool yes
	*/

	void setFirstFrame(FAPFrame *frame, bool yes);

private:


	/**
	*
	*
	* @param  xmlfile
	*/

	void BuildFileNames(char* xmlfile);


	/**
	*
	*
	* @param  &myString
	*/

	void stringToLower(std::string &myString);


	/**
	*
	*
	* @param s
	*/

	void stringToLower(char *s);


	/**
	*
	*
	* @return 
	* @param  filename
	*/

	int **LoadExpressionFromFAPFile(std::string filename);

	//	FaceLibraryParser *FaceLibParser;

	xmlNodePtr apmltreeroot;

	int tmp_file_index;



};
