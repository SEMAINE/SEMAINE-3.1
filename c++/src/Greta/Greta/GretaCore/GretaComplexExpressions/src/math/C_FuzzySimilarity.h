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

// C_FuzzySimilairty.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "FaceExpressionDictionary.h"
#include "Expression.h"
#include "../utils/CemGlobals.h"

#include <string>
#include <vector>

/**
* class :C_FuzzySimilarity
*
*/

class C_FuzzySimilarity 
{

	//std::vector<Expression> *expressions;
	FaceExpressionDictionary *facedictionary;

	int **limits;

	//int [] masking;

public:


	/**
	* contructor 
	*
	*/

	C_FuzzySimilarity();

	/**
	* destructor 
	*/

	virtual ~C_FuzzySimilarity();	        

	/**
	* finds the value of similarity between two facial expressions defined by two labels
	* @param basic_label label of basic expression
	* @param complex_label label of complex expression
	* @return similarity value
	*/

	double findSimilarity(std::string complex_label, std::string basic_label);

private: 


	/**
	*establishes the value similarity betwwen two expressions defined by arrays of faps
	*
	* @param first expression
	* @param second expression
	* @return a value of similarity in the interval [-1,1]
	*
	*/

	double count_similarity(int * first, int *second);            

	/**
	*
	* @param value a float value
	* @return the input value with the 0,01 precision 
	*/

	double roundtotwo(double wynik);

}//end of class
;
