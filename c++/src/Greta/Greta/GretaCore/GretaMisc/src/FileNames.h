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

/**
*
* class :FileNames
*
*/

class FileNames
{
public:

	/**
	*
	* contructor 
	*
	*/

	FileNames(void);

	/**
	*
	* destructor 
	*/

	~FileNames(void);

	/**
	* this method 
	* 
	*
	* @param  filename
	*/

	void BuildFileNames(std::string filename);

	std::string Base_File;
	std::string APML_File;
	std::string FMLAPML_to_BML_File;
	std::string Fap_File;
	std::string Bap_File;
	std::string Fap_File_Listener;
	std::string Wav_File;
	std::string Phonemes_File;
	std::string Script_File;
	std::string Text_File;
	std::string Turns_File;
};
