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

// BAPconverter.h: interface for the BAPconverter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BAPCONVERTER_H__E46C5AD3_F11A_4C5F_862F_2758B5291A7F__INCLUDED_)
#define AFX_BAPCONVERTER_H__E46C5AD3_F11A_4C5F_862F_2758B5291A7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

#include "BAPTypes.h"
#include "BAP.h"
#include <stdio.h>
#include <sstream>

	/** 
	* class :BAPConverter
	* Utility conversions between BAPTypes, ints, strings, SidelessBAPTypes 
	* 
	* Using lookup-tables, BAPTypes are converted from and to
	* different formats.
	* <pre>
	* BAPType        : a enumeration type; example: l_shoulder_abduct
	* int            : the integer corresponding to the BAPType = the MPEG4 BAP number
	* string         : Maya joint rotation name, such as "r_shoulder.rotateX"
	* SidelessBAPType: example: shoulder_abduct; together with side=l, will yield l_shoulder_abduct
	* </pre>
	*
	* @author Bjoern Hartmann, bjoern@graphics.cis.upenn.edu 
	* @version 1.0
	*/
	
	class BAPConverter  
	{
	public:

/**
 *  
 * 
 *
* @return 
* @param  i
*/

		string IntToMJoint(int i);

/**
 *  
 * 
 *
* @return 
* @param  i
*/

		joint_type BAPToJointType(int i);

/**
 *  
 * 
 *
* @return 
* @param  s
*/

		joint_type JointStringToJointType(string s);

/**
 *  
 * 
 *
* @return 
* @param  i
*/

		string JointTypeToJointString(int i);
		

/**
 *  
 * 
 *
* @return 
* @param  t
* @param  int i
*/

		double IntToAngle(AngleType t, int i);

/**
 *  
 * 
 *
* @return 
* @param  t
* @param  double f
*/

		int AngleToInt(AngleType t, double f);


/**
 *  
 * 
 *
* @return 
* @param  t
* @param  int i
*/

		double IntToLength(LengthType t, int i);

/**
 *  
 * 
 *
* @return 
* @param  t
* @param  double f
*/

		int LengthToInt(LengthType t, double f);
		


/**
 *  
 * 
 *
* @return 
* @param  m
*/

		BAPType BVHJointToBAP(string m);

/**
 *  
 * 
 *
* @return 
* @param  m
*/

		BAPType DiGuyJointToBAP(string m);
		/** Given Sideless BAPType and a side, return the corresponding BAPType */

/**
 *  
 * 
 *
* @return 
* @param  b
* @param  SideType s
*/

		BAPType GetSideBAP(SidelessBAPType b, SideType s);
		

/**
 *  
 * 
 *
* @return BAPType corresponding to BAP number
* @param  i
*/

		BAPType IntToBAP(int i);
		
		/** return BAPType corresponding to Maya joint rotation string */

/**
 *  
 * 
 *
* @return 
* @param  m
*/

		BAPType MJointToBAP(string m);
		

/**
* contructor : initialize lookup-table
*
*/

		BAPConverter();
		

/**
* destructor 
*/

		virtual ~BAPConverter();

	private:
		/** array for int-to-BAPType conversion */
		BAPType a[NUMBAPS+1];
		/** array for BAPType-to-Maya joint conversion */
		string s[NUMBAPS+1];
		/** array for BAPType-to-joint_type conversion */
		joint_type b2j[NUMBAPS+1];
		/** array for Jointname-to-joint_type conversion */
		string j2j[NUMJOINTS+1];

	};



#endif // !defined(AFX_BAPCONVERTER_H__E46C5AD3_F11A_4C5F_862F_2758B5291A7F__INCLUDED_)
