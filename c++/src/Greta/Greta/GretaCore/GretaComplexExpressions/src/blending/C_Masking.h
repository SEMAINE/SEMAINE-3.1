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

// C_Masking.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Emotion.h"
#include "Expression.h"

#include "FaceExpressionDictionary.h"
#include "FaceExpression.h"
#include "FAPFrame.h"

#include ".\..\utils\CemGlobals.h"

#include <string.h>

/**
* class :C_Masking
*
*/

class C_Masking
{

	Emotion *emotion1;
	Emotion *emotion2;

	int **fake_expression;
	int **felt_expression;

	int **final_expression;

	std::string type;

	FaceExpressionDictionary *facedictionary;
	//int [][] faces;

	int **facesareas;

	// final results - this array contains {-1,0,1,2}
	int division [RESULT_LENGTH];

	//temporal results this array can contain the real values
	double result[RESULT_LENGTH];

public :


	/**
	* defines emotions for which the expression is generated
	*
	* @return 
	* @param e emotion
	*/

	bool addEmotion(Emotion *e);

	/**
	* returns type of expression, not used  
	*
	* @return 
	*/

	std::string getType();

	/**
	* sets type of expression, not used 
	* 
	* @param  type
	*/

	void setType(std::string type);                                	             	

	/**
	* contructor 
	*
	*/

	C_Masking() ;

	/**
	* destructor 
	*/

	virtual ~C_Masking() ;

	/**
	* It returns a result of facial masking in a form of an array where:
	* 
	* -1 FELT 
	* 0  NEUTRAL
	* 1  FAKE
	*2 RANDOM
	*
	* @return array of face division for masking expression
	* 
	*/
	int *getDivision();

	/**
	* main method of the algorithm, it finds deception clues
	* @param maxareas number of facial areas
	* @return  true if OK
	*/
	bool evaluate(int maxareas);	

	/*
	*genenerates a complex esxpression in MPEG4 standard
	*
	* @return the array of faps for the final expressions
	* @param  resultline
	*/

	int **getExpression(float resultline);

private :


	/**
	* it joins fake and felt parts in one facial expression of masking
	* It returns a value in interval (-1, 2)

	* @return returns which part has to be displayed
	* @param felt the value of similarity for felt expression
	* @param fake the value of simiularity for felt expression
	*/
	double join(double felt, double fake) ;

	/**
	*  
	* 
	*
	* @return 
	* @param  newone
	* @param  double oldone
	*/

	double count(double newone, double oldone);

	/**
	* implements Ekman's deception cues
	* @return returns the array of values in the interval  -1, 1 , where: -1 - not seen, 0 - I dont know,  1 - seen
	* @param similarities
	*/
	double *findfake(double *similarities);    

	/**
	* implement Ekman's hyphothesis 
	* @return returns the array of values in the interval  -1, 1 , where: -1 - not seen, 0 - I dont know, 1 - seen
	* @param similarities
	*/

	double *findfelt(double *similarities);    

};
