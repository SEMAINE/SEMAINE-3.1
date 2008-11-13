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

// MultimodalEmotionConstraint.h: interface for the MultimodalEmotionConstraint class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Constraint.h"
#include "ConstraintSignal.h"

#include <string>
#include <vector>
#include <map>



/**
* class :MultimodalEmotionConstraint
*
*/

class MultimodalEmotionConstraint 
{

private:

	std::vector<Constraint> *constraints;
	std::vector<ConstraintSignal> *signals;
	std::string name;

public:


	/**
	* contructor 
	*
	*/

	MultimodalEmotionConstraint ();

	/**
	* destructor 
	*/

	virtual ~MultimodalEmotionConstraint ();


	/**
	*  
	* 
	*
	* @return 
	*/

	std::string getName();

	/**
	*  
	* 
	*
	* @param  name1
	*/

	void setName(std::string name1);

	/**
	*  
	* 
	*
	* @param  constraint1
	*/

	void addConstraint(Constraint constraint1);	

	/**
	*  
	* 
	*
	* @return 
	*/

	std::vector<Constraint>* getConstraints();


	/**
	*  
	* 
	*
	* @return 
	* @param  name
	*/

	ConstraintSignal* getConstraintSignal(std::string name) ;

	/**
	*  
	* 
	*
	* @param  constraintsignal
	*/

	void addConstraintSignal(ConstraintSignal constraintsignal) ;

	/**
	*  
	* 
	*
	* @param constraintsignal
	*/

	void addConstraint(Constraint *constraintsignal) ;


	/**
	*  
	* 
	*
	* @return 
	* @param  name
	*/

	int getIdOfSignal(std::string name);


	/**
	*  
	* 
	*
	* @return 
	*/

	std::vector<ConstraintSignal>* getConstraintSignals() ;

	/**
	*  
	* 
	*
	* @return 
	* @param  name
	*/

	std::vector<Constraint>*  getConstraintsOfOneSignal(std::string name) ;

};

/** 
* List of MultimdalEmotionConstraint objects.
* 
*/

typedef std::map<std::string,MultimodalEmotionConstraint> MultimodalEmotionConstraints;
