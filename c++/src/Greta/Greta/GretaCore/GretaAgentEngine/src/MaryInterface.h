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

// MaryInterface.h: interface for the MaryInterface class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#pragma warning(disable:4786)

#include <string>
#include "XMLGenericTree.h"
#include "APMLCommunicativeAct.h"
#include "SpeechInterface.h"
#include "Signal.h"

/**
*
* class :MaryInterface
*
*/

class MaryInterface : public SpeechInterface
{
public:

/**
* contructor 
*
* @param  first_comm_act
*/

	MaryInterface(CommunicativeAct* first_comm_act);

/**
* contructor 
*
*/

	MaryInterface();

/**
* destructor 
*/

	virtual ~MaryInterface();

/**
 * this method 
 * 
 *
* @return 
* @param  input
* @param  outputfilename
*/

	int SSMLToPhonemes(std::string input,std::string outputfilename);

/**
 * this method 
 * 
 *
* @return 
* @param  input
* @param  outputfilename
*/

	int SSMLToWave(std::string input,std::string outputfilename);

/**
 * this method 
 * 
 *
* @return 
* @param  input
* @param  outputfilename
*/

	int SSMLToMp3(std::string input,std::string outputfilename);

/**
 * this method 
 * 
 *
* @return 
* @param  input
* @param  outputfilename
*/

	int MaryXMLToPhonemes(std::string input,std::string outputfilename);

/**
 * this method 
 * 
 *
* @return 
* @param  input
* @param  outputfilename
*/

	int MaryXMLToWave(std::string input,std::string outputfilename);

/**
 * this method 
 * 
 *
* @return 
* @param  input
* @param  outputfilename
*/

	int MaryXMLToMp3(std::string input,std::string outputfilename);

/**
 * this method 
 * 
 *
* @return 
* @param  input
* @param  outputfilename
*/

	int APMLToPhonemes(std::string input,std::string outputfilename);

/**
 * this method 
 * 
 *
* @return 
* @param  input
* @param  outputfilename
*/

	int APMLToWave(std::string input,std::string outputfilename);

/**
 * this method 
 * 
 *
* @return 
* @param  input
* @param  outputfilename
*/

	int APMLToMp3(std::string input,std::string outputfilename);

/**
 * this method 
 * 
 *
* @return 
* @param  input
* @param  outputfilename
*/

	int TextToPhonemes(std::string input,std::string outputfilename);

/**
 * this method 
 * 
 *
* @return 
* @param  input
* @param  outputfilename
*/

	int TextToWave(std::string input,std::string outputfilename);

/**
 * this method 
 * 
 *
* @return 
* @param  input
* @param  outputfilename
*/

	int TextToMp3(std::string input,std::string outputfilename);

/**
 * this method 
 * 
 *
*/

	void SetEnglish();

/**
 * this method 
 * 
 *
*/

	void SetGerman();

/**
 * this method 
 * 
 *
*/

	void SetFrench();

/**
 * this method 
 * 
 *
* @param  n
*/

	void SetMale(int n);

/**
 * this method 
 *
* @param  n
*/

	void SetFemale(int n);

/**
 * this method 
 * 
 *
* @return 
* @param  inputfilename
* @param  maryfilename
* @param  wavefilename
*/

	int Run(std::string inputfilename,std::string maryfilename,std::string wavefilename);

/**
 * this method 
 * 
 *
* @return 
* @param t
*/

	int ExtractPhonemesFromMaryOutput(XMLGenericTree *t);

/**
 * this method 
 * 
 *
* @param  f
* @param t
* @param tml
* @param stressingpoints
* @param time
*/

	void ExtractAllPhonemes(FILE* f,XMLGenericTree *t,std::list<MMSystemSpace::TimeMarker> *tml,std::vector<float> *stressingpoints,float *time);

/**
 * this method 
 * 
 *
* @param  filename
* @param t
* @param tml
* @param stressingpoints
* @param  time
*/

	void ExtractAllPhonemes(std::string filename,XMLGenericTree *t,std::list<MMSystemSpace::TimeMarker> *tml,std::vector<float> *stressingpoints,float* time);

/**
 * this method 
 * 
 *
* @return 
* @param  input
* @param size
*/

	char* MaryInterface::MaryXMLToWaveInChar(std::string input, int *size);

/**
 * this method 
 * 
 *
* @return 
* @param  input
* @param output
*/

	int MaryInterface::MaryXMLToWaveInChar(std::string input,std::string *output);

private:

	std::string host;
	int port;
	std::string in;
	std::string out;
	std::string audiotype;
	std::string voice;
	bool initialized;
	std::string id;

/**
 * this method 
 * 
 *
* @return 
* @param  input
* @param  outputfilename
*/

	int SendAndReceive(std::string input,std::string outputfilename);	

/**
 * this method 
 * 
 *
* @return 
* @param  input
* @param  size
*/

	char* SendAndReceiveNoFile(std::string input, int * size);


/**
 * this method 
 * 
 *
* @return 
*/

	int Connect();

/**
 * this method 
 * 
 *
*/

	void Deconnect();
	std::string sex;

/**
 * this method 
 * 
 *
* @param t
*/

	void APMLToSSML(XMLGenericTree *t);
	FILE *SSMLfile;

/**
 * this method 
 * 
 *
* @return 
* @param  p
*/

	std::string SampaToFestival(std::string p);
	bool english,german,french;

};
