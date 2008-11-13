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

//BML Engine for Greta
//http://twiki.isi.edu/Public/BMLSpecification

#pragma once
#pragma warning(disable:4786)
#include "Signal.h"
#include "GesturePlanner.h"
#include "MotorPlanner.h"
#include "TorsoEngine.h"
#include "HeadEngine.h"
#include "FaceEngine.h"
#include "BAPFrame.h"
#include "FAPFrame.h"
#include <string>
#include <vector>



/**
*
* class :BML_AgentEngine
*
*/

class BML_AgentEngine
{
public:

	/**
	*
	* contructor 
	*
	*/

	BML_AgentEngine(void);

	/**
	*
	* destructor 
	*/

	~BML_AgentEngine(void);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  BMLfilename
	* @param buffer
	* @param  writetofile
	* @param  startbf=0
	* @param  startff=0
	* @param  isrealtime=false
	*/

	int Execute(std::string BMLfilename,char *buffer,bool writetofile,BAPFrame* startbf=0,FAPFrame* startff=0,bool isrealtime=false);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  bapfilename
	* @param  startbf=0
	*/

	int GenerateGestures(std::string bapfilename,BAPFrame* startbf=0);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  fapfilename
	* @param  startff=0
	* @param  isrealtime=false
	*/

	int GenerateFace(std::string fapfilename,FAPFrame* startff=0,bool isrealtime=false);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  fapfilename
	*/

	int GenerateHead(std::string fapfilename);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  bapfilename
	*/

	int GenerateTorso(std::string bapfilename);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  s
	*/

	std::string ToUpper(std::string s);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  BMLfilename
	* @param buffer
	* @param  isrealtime=false
	*/

	int LoadBML(std::string BMLfilename,char *buffer,bool isrealtime=false);


	/**
	* this method 
	* 
	*
	* @return 
	*/

	FaceEngine* GetFaceEngine();

	/**
	* this method 
	* 
	*
	* @return 
	*/

	GestureSpace::MotorPlanner* GetMPlanner();

	std::vector<MMSystemSpace::Signal> signals;
	std::string phofile;


	/**
	* this method 
	* 
	*
	* @return 
	*/

	bool hasNoEndFaceSignals();


	/**
	* this method 
	* 
	*
	* @return 
	*/
	bool hasSpeech();

	char* content_of_speech;
	int speech_size;

private:
	GestureSpace::MotorPlanner *mMPlanner;
	GestureSpace::GesturePlanner *mGPlanner;
	TorsoSpace::TorsoEngine *te;
	HeadSpace::HeadEngine *he;
	FaceEngine *faceengine;
	float gesturetorsolength;
	float faceheadlength;

	//we need it to proccess infinite signals in realtime version
	//renamed from "idle"
	bool noend_facesignals;

public:

	//it is used to memorize a first frame in real-time version
	MMSystemSpace::Signal *speech;

	//for a speech tag only
	float speech_delay;

};
