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

// C_Superposition.h: 
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "Emotion.h"
#include "Expression.h"
#include "./../structures/C_SchemaSection.h"
#include "./../utils/CemGlobals.h"

#include "FaceExpressionDictionary.h"
#include "FaceExpression.h"
#include "FAPFrame.h"

#include <string>

/**
* class :C_Superposition
*
*/

class C_Superposition  
{

	Emotion *emotion1;
	Emotion *emotion2;

	int ** expression1;
	int ** expression2;

	int **final_expression;

	std::string type;

	FaceExpressionDictionary *facedictionary;

	//facelibrary
	//int [][] faces = null;

	//eight facial areas
	int **facesareas;

	//a list of avaiable emotions
	//Vector emotions = null; 

	int *division;

	//?
	double fuzzyresults[RESULT_LENGTH];;

	//superpostion schemas
	std::vector<C_SchemaSection> *superpositionschemas1; 

public :


	/**
	* defines emotions for which the expression is generated
	*
	* @return 
	* @param e
	*/

	bool addEmotion(Emotion *e);    

	/**
	* returns type of expression, not used
	* @return type of expression
	*/
	std::string getType();     

	/**
	* sets a type of expression, not used
	* @param type of expression
	*/
	void setType(std::string type);                               

	/**
	* contructor 
	*
	*/

	C_Superposition();	

	/**
	* destructor 
	*/

	virtual ~C_Superposition();        

	/**
	* It returns a result of expression of superposition in a form of an array where:
	*     
	* @return array of face division for the superposition expression
	*/        
	int *getDivision();

	/*
	* genenerates a complex esxpression in MPEG4 standard
	*
	* @return returns the array of faps for the final expressions
	*/
	int **getExpression();

	/**
	* main method of  the alorithm
	* @param maxareas the number of facial areas
	*/

	bool evaluate(int maxareas);

private :

	/**
	* implements Ekman's superposition schemas
	* @return returns the array of values in the interval  -1, 1 , where: -1 - not seen, 1 - seen
	* @param similarities
	* @param  double deltaintensity
	*/
	int find(double **similarities, double deltaintensity);

};
