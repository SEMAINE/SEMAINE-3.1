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
// IniManager.h: interface for the IniManager class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "IniParameter.h"
#include <list>
#include <string>

/*! \brief reads and writes ini files
*
* this is the main object for managing ini files.
* after calling the constructor you have to call the ReadIniFile method
* to load the ini file in memory and then you can call other methods
* to retrieve the parameters that you want. at any time you can save all the stored
* parameters to an ini file (the same you have read or a new one)
* @see GetValueInt() GetValueFloat() GetValueString() WriteIniFile()
* @see IniParameter
*/
class IniManager  
{
public:
	/*! \brief class contructor
	*
	*  calculates the proper value of the Program_Path member variable
	*  @see Program_Path
	*/
	IniManager();
	/*! \brief class destructor
	*
	*  given the name of a IniParameter and an integer value it stores the value
	*  in the IniParameter
	*/
	virtual ~IniManager();
	/*! \brief tells if IniManager is initialized or not
	*
	*  this flag can be used to know if the IniManager object has already
	*  read the ini file. for example can be used before trying to get
	*  parameters values from the object to ensure that the result will
	*  of the modulation will be coherent
	*/
	bool initialized;
	/*! \brief for internal use only. changer a string to uppercase
	*
	*  this method changes the content of a string always to uppercase
	*  @todo not yet working!
	*/
	void ToUppercase(std::string &s);
	/*! \brief returns the value of a IniParameter as an integer
	*
	*  given the name of a IniParameter contained in the ini file this method
	*  returns the value of the parameters if found, otherwise returns the value -999999999
	*  @todo
	*  - find a better way to manage error situations, avoiding to return -999999999
	*  - allow the method to work in a case-independent way
	*/
	int GetValueInt(std::string name);
	/*! \brief reads the content of an ini file
	*
	*  given the name of an ini file it reads its content and stores it in a list
	*  of parameters that can be accessed with other member functions.
	*  @param IniFileName the name (path) of the ini file to be read
	*  @return 1 if ini file is successfully transferred in memory, 0 if not
	*  @see GetValueInt() GetValueFloat() GetValueString() WriteIniFile()
	*/
	int IniManager::ReadIniFile(std::string IniFileName);
	/*! \brief write the stored parameters to an ini file
	*
	*  given the name of an ini file it writes the parameters that are
	*  stored in memory to it
	*  @param IniFileName the name (path) of the ini file to be read
	*  @return 1 if ini file is successfully written, 0 if not
	*  @see ReadIniFile()
	*/
	int IniManager::WriteIniFile(std::string IniFileName);
	/*! \brief returns the value of a IniParameter as a string
	*
	*  given the name of a IniParameter contained in the ini file this method
	*  returns the value of the parameters if found, otherwise returns the empty string ""
	*  @todo
	*  allow the method to work in a case-independent way
	*/
	std::string IniManager::GetValueString(std::string name);
	/*! \brief returns the value of a IniParameter as a float
	*
	*  given the name of a IniParameter contained in the ini file this method
	*  returns the value of the parameters if found, otherwise returns the value -999999999.0f
	*  @todo
	*  - find a better way to manage error situations, avoiding to return -999999999.0f
	*  - allow the method to work in a case-independent way
	*/
	float IniManager::GetValueFloat(std::string name);
	/*! \brief contains the path of the executable
	*
	*  it is initialized with the physical path of the executable file for the
	*  main thread of execution
	*/
	std::string Program_Path;	
	/*! \brief list of parameters
	*
	*  it stores the parameters that has been read from the ini file
	*/
	std::list<IniParameter*> parameters;
	/*! \brief not more used
	*/
	std::string ExtractValueString(std::string s);
	/*! \brief not more used
	*/
	float ExtractValueFloat(std::string s);
	/*! \brief inverts slashes characters
	*  given a string it scans it and changes all the slashes from '\' to '/'
	*/
	void InvertSlashes(std::string & s);
	/*! \brief sets the value of a IniParameter
	*
	*  given the name of a IniParameter and an integer value it stores the value
	*  in the IniParameter
	*/
	void SetValueInt(std::string name, int value);
	/*! \brief sets the value of a IniParameter
	*
	*  given the name of a IniParameter and an float value it stores the value
	*  in the IniParameter
	*/
	void SetValueFloat(std::string name, float value);
	/*! \brief sets the value of a IniParameter
	*
	*  given the name of a IniParameter and an string value it stores the value
	*  in the IniParameter
	*/
	void SetValueString(std::string name, std::string value);
};
