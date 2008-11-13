//Copyright 1999-2008 Catherine Pelachaud - c.pelachaud@iut.univ-paris8.fr
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

// C_SchemaSection.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Emotion.h"


/*
* SchemaSection.h
*
* Created on 18 styczeñ 2006, 20:42
*
*
* uwaga kazda sekcja skalda sie z n obszarow
* obszary sa identyfikowalne przez numery nadane obszarom od 0 do MAX_AREAS
*
* sekcji jest m, gdzie m>=n
*
* natomiast identyfikatore sekcji sa od 1 do m
* i musza odpowiadac obiektom w modelu fuzzy
*
*/



/**
* class :C_SchemaSection
*
*/

class C_SchemaSection 
{

private:

	/**
	* the list of areas which belongs to given section
	*/	
	int *sections;


	/**
	* the length of te list
	*/    
	int length;

	/**
	* name of the section, e.g. S1, S2, ...
	*/
	std::string name;

	/**
	* the numeric id of the section
	*/
	int id;

public :


	/**
	*  
	* 
	*
	* @return 
	* @param  text
	*/

	C_SchemaSection(std::string text);

	/**
	*  
	* 
	*
	* @return 
	* @param  text
	* @param  int id1
	*/

	C_SchemaSection(std::string text, int id1);

	/**
	* contructor 
	*
	*/

	C_SchemaSection();        

	/**
	* destructor 
	*/

	virtual ~C_SchemaSection();

	/**
	*  
	* 
	*
	* @return id
	*/

	int getId();

	/**
	*  add an element to section
	* 
	*
	* @param  section
	*/

	void addSection(int section);            

	/**
	*  
	* 
	*
	* @return an array of elements
	*/

	int *getSections() ;                    

	/**
	*  
	*
	* @return 
	* @param  max1
	*/

	int *getDivision(int max1) ;    

	/**
	*  
	*
	* @return the name
	*/

	std::string getLabel();    

	/**
	*  
	*
	* @return the number of elements
	*/

	int getLength();

}//end of class
;

