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

// Psydule.h: interface for the Psydule class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
//#include "audio.h"
#include "Communicator.h"
#include "Component.h"
#include "CppAIRPlug.h"


//#include <iostream>
#include <string>
#include <list>
#include <fstream>


using namespace cmlabs;

/**
*
* class :Psydule
*
*/

class Psydule : public CppAIRPlug
{
public:

	/**
	*
	* contructor 
	*
	* @param  name
	* @param  JString host
	* @param  int port
	*/

	Psydule(JString name, JString host, int port);

	/**
	*
	* destructor 
	*/

	virtual ~Psydule();


	/**
	* this method 
	* 
	*
	* @return 
	* @param  whiteboard
	* @param  JString datatype
	*/

	int Register(JString whiteboard, JString datatype);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  rawregistrationstring
	*/

	int RawRegister(JString rawregistrationstring);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  whiteboard
	* @param  std::list<JString> datatypes
	*/

	int Register(JString whiteboard, std::list<JString> datatypes);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  filename
	* @param  JString whiteboard
	* @param  JString datatype
	*/

	int PostFile(std::string filename, JString whiteboard, JString datatype);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  tosend
	* @param  JString whiteboard
	* @param  JString datatype
	*/

	int PostString(std::string tosend, JString whiteboard, JString datatype);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  timeout
	*/

	JString Psydule::ReceiveString(int timeout);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  timeout
	*/

	Message* Psydule::ReceiveMessage(int timeout);
	//ObjectCollection* Psydule::ReceiveAllMessages(int timeout);


	/**
	* this method 
	* 
	*
	* @return 
	* @param  filename
	* @param msg
	*/

	int Psydule::WriteBinaryFile(std::string filename, Message *msg);


	/**
	* this method 
	* 
	*
	* @return 
	* @param  fapfilename
	* @param  JString whiteboard
	* @param  JString datatype
	*/

	int Psydule::SendFAPs(std::string fapfilename, JString whiteboard, JString datatype);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  bapfilename
	* @param  JString whiteboard
	* @param  JString datatype
	*/

	int Psydule::SendBAPs(std::string bapfilename, JString whiteboard, JString datatype);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  binfilename
	* @param  JString whiteboard
	* @param  JString datatype
	*/

	int Psydule::SendBinaryFile(std::string binfilename, JString whiteboard, JString datatype);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  tosend
	* @param  JString whiteboard
	* @param  JString datatype
	* @param  size
	*/

	int Psydule::PostSpeechFromChar(char* tosend, JString whiteboard, JString datatype,int * size);

	//	int Psydule::ReceiveWAV(Message *msg);

	JString name,host;
	int port;
};
