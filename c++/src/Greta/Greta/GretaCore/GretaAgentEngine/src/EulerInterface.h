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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <direct.h>
#include "APMLCommunicativeAct.h"
#include "SpeechInterface.h"

using namespace std;

/**
*
* class :EulerInterface
*
*/

class EulerInterface : public SpeechInterface
{
public:

	/**
	* contructor 
	*
	* @param  first_comm_act
	*/

	EulerInterface(CommunicativeAct* first_comm_act);

	/**
	* destructor 
	*/

	~EulerInterface(void);

	/**
	* 
	*
	* @return 
	*/

	int Prepare();

	/**
	* 
	*
	* @return 
	*/

	int Run();

	/**
	* 
	*
	* @return 
	* @param  nameFile
	* @param  string finalFile
	* @param  float speed
	*/

	int ConvertFilePho(string nameFile, string finalFile, float speed);

	/**
	* 
	*
	* @return 
	* @param  nameFile
	* @param  string finalFile
	* @param  float speed
	*/

	int EulerInterface::ChangeTimeFilePho(string nameFile, string finalFile, float speed);

	//prova

	/**
	* 
	*
	* @return 
	* @param  nameFile
	*/

	int EulerInterface::WriteTXT(string nameFile);

private:

	FILE *script_for_festival;

	/**
	* 
	*
	* @param  target
	*/

	void InvertSlashes(char* target);

	/**
	* 
	*
	* @return 
	* @param  s
	*/

	std::string utfToAscii(std::string s);
	int TotFileX;

};
