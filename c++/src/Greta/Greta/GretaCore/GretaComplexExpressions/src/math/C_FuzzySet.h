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

// C_FuzzySet.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

/**
*
* a trapezoid fuzzy set
* @author Radek
*/

/**
* class :C_FuzzySet
* a trapezoid fuzzy set
* @author Radek
*/

class C_FuzzySet
{
	/**
	* left corner of the base
	*/
private  :

	int a;
	/**
	* left corner of the roof
	*/
	int b;
	/**
	* right corner of the roof
	*/
	int c;
	/**
	* right corner of the base
	*/
	int d;

	int d15;


public:


	/**
	*  
	* 
	*
	* @return 
	*/

	int getA();

	/**
	*  
	* 
	*
	* @return 
	*/

	int getB();

	/**
	*  
	* 
	*
	* @return 
	*/

	int getC();

	/**
	*  
	* 
	*
	* @return 
	*/

	int getD();


	/**
	* destructor 
	*/

	virtual ~C_FuzzySet();        

	/**
	* contructor 
	* creates a fuzzy set
	* @param x center point
	* @param d1 amplitude (min-max of a fap)
	*/


	C_FuzzySet(int x, int d1);        

	/**
	* contructor 
	* creates a fuzzy set
	* 
	* @param x center point
	* @param d1 amplitude (min-max of a fap)
	* @param factor a part of d1 that serves to build a fuzzy set
	*/

	C_FuzzySet(int x, int d1, double factor);       

	/**
	* a value of membership function
	* @param x argument
	* @return a membership value
	*/

	double include(int x);

}//end of class

;
