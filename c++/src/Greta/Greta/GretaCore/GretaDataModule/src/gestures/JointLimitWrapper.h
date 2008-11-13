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

/// @file JointLimitWrapper.h
/// interface for the JointLimitWrapper class.
///
/////////////////////////////////////////////////////////////////////

#if !defined(JOINTLIMITWRAPPER_H)
#define JOINTLIMITWRAPPER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "GestureEngineGlobals.h"

#include "jointLimits.h"
#include "Wm4Matrix3.h"

namespace GestureSpace {

/** Interface layer between the GestureEngine and the Wilhelms/vanGelder joint limit code. 
 * @author Bjoern Hartmann, b.hartmann@iut.univ-paris8.fr
 * @see WristAngleSkeleton
*/ 
class JointLimitWrapper
{
public:
	JointLimitWrapper(void);
	virtual ~JointLimitWrapper(void);
	
	/** Test a set of rotation angles for the left wrist against joint limits. 
	 * If limits are exceeded, orientation is projected back to reach cone boundaries.
	 * Assumes Euler angle rotation in order: ?
	 * @param x rotation around x in (degrees? radians?); adjusted if input is not in reach cone.
	 * @param y rotation around y in (degrees? radians?); adjusted if input is not in reach cone.
	 * @param z rotation around z in (degrees? radians?); adjusted if input is not in reach cone.
	 */
	int CheckLWristXYZ(float &x, float &y, float &z);
	
	/** Test a set of rotation angles for the right wrist against joint limits.
	 * @see CheckLWristXYZ
	 */
	int CheckRWristXYZ(float &x, float &y, float &z);
	
	/** Test a set of rotation angles for the left shoulder against joint limits. */
	int CheckLShoulderXYZ(float &x, float &y, float &z);
	
	/** Test a set of rotation angles for the right shoulder against joint limits. */
	int CheckRShoulderXYZ(float &x, float &y, float &z);

protected:
	void InitRWrist();
	void InitLWrist();
	void InitRShoulder();
	void InitLShoulder();
	
	Jtl_LimitPtr rWristLimits;
	Jtl_LimitPtr lWristLimits;
	Jtl_LimitPtr rShoulderLimits;
	Jtl_LimitPtr lShoulderLimits;
	
	/** Converts rotation representation from Greta's system to the jtl system, calls jtl solver, re-converts results. */
	int CheckLimits(float &x, float &y, float &z, Jtl_LimitPtr pLimits);
	
	bool ToEulerAnglesXYZ2 (Wm4::Matrix3f &m, float& rfXAngle, float& rfYAngle,
    float& rfZAngle);

};
}//end namespace

#endif //JOINTLIMITWRAPPER_H