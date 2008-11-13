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

// GestureEngine.cpp: implementation of the GestureEngine class.
//
//////////////////////////////////////////////////////////////////////
/**
* GestureEngine class: calculates agent's body animation
*
* @see GestureEngine, APML_AgentEngine
* @see greta.c
* @author Maurizio Mancini, manmau@yahoo.com
* @version 1.0
*/


/////
#include "GestureEngine.h"
//#include "Parser.h"
//#include "IKArm.h"
/**@#-*/
using namespace GestureSpace;
using namespace std;
/**@#+*/
	extern FILE* gesture_log;
////

	

//this variable is set up by face engine in xml2fap.c
//extern "C" char BAP_FILE[256];


	//extern FILE* gesture_log;


//constructor, does nothing
GestureEngine::GestureEngine()
{
}

//the main and only method of the class
//creates a new instance of a TurnPlanner object
//TurnPlanner doesn't need parameters because it
//uses another variable set up by xml2fap.c
//to obtain its input
void GestureEngine::CalculateTurn(std::string bapfilename,std::string turnsfilename)
{
	//new istance of TurnPlanner
	TP=new GestureSpace::TurnPlanner;
	//executes the body animation calculations
	TP->CalculateTurn(turnsfilename);

	TP->GetMPlannerPtr()->GenerateAnimation();
	//writes final animation frames to the bap file (interpolation done here too?)
	TP->GetMPlannerPtr()->WriteAnimation(bapfilename);
	//TurnPlanner object is deleted
	delete TP;
}

//destructor
GestureEngine::~GestureEngine()
{

}
