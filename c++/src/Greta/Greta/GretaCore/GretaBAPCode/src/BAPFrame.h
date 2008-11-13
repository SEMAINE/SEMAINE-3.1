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

// BAPFrame.h: interface for the BAPFrame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BAPFrame_H__98FDBC8E_D9F5_4298_9606_4623BAC13E61__INCLUDED_)
#define AFX_BAPFrame_H__98FDBC8E_D9F5_4298_9606_4623BAC13E61__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

#include "baptypes.h"
#include "BAP.h"
#include "BAPConverter.h"

#include <stdio.h>

#include <sstream>

/**@#-*/
using namespace std ;
using namespace std::rel_ops ;
/**@#+*/

/** A BAPFrame is a complete collection of BAPs describing a skeletal position.
*
* In a vector of BAPs, all skeletal joint angles are recorded that
* completely define the skeleton's positioning at one moment in time.
*
* @author Bjoern Hartmann, bjoern@graphics.cis.upenn.edu 
* @version 1.0
*/
class BAPFrame  
{
public:
	float GetTCBParam(TCBParamType which);
	void SetTCBParam(float T, float C, float B);
	void SetTension(float T);
	void SetContinuity(float C);
	void SetBias(float B);
	bool operator<(BAPFrame& a);

	/** CopyAnglesNoMaks: Copy all BAP angles from a given source frame without 
	* setting the BAP mask. Called from MotorPlanner::Init.
	* @see MotorPlanner::Init .*/
	void CopyAnglesNoMask(BAPFrame* pSource);
	void CopyAngles(BAPFrame* pSource);
	void CopyAngles(BAPFrame *pSource,float scalefactor);

	/** SetGroupMask: Set mask bit for an entire ArmGroup. */
	void SetGroupMask(ArmGroupType group, SideType side, bool b);

	/** Set Mask for _one_ BAP in the BAPvector */
	void SetMask(SidelessBAPType which, SideType side, bool b);

	/** Set Mask for _one_ BAP in the BAPvector */
	void SetMask(BAPType which, bool b);

	/** IsDefined: returns true if at least one of the BAPs in 
	*  the ArmGroup is set. */
	bool	IsDefined(ArmGroupType argGroup, SideType argSide);

	/** Get Mask of _one_ BAP in the BAPvector */
	bool	GetMask(SidelessBAPType which, SideType side);
	/** Get Mask of _one_ BAP in the BAPvector */
	bool	GetMask(int which);
	/** Get Mask of _one_ BAP in the BAPvector */
	bool	GetMask(BAPType which);
	/** Get Mask of _one_ BAP in the BAPvector */
	bool	GetMask(string mjointname);

	/** Get value of _one_ BAP in the BAPvector */
	int		GetBAP( int which);
	/** Get value of _one_ BAP in the BAPvector */
	int		GetBAP( BAPType which);
	/** Get value of _one_ BAP in the BAPvector */
	int		GetBAP( string mjointname);
	/** Get value of _one_ BAP in the BAPvector */
	int		GetBAP( SidelessBAPType which, SideType side);

	/** Get value of _one_ BAP in the BAPvector */
	double	GetBAPAngle( int which, AngleType t);
	/** Get value of _one_ BAP in the BAPvector */
	double	GetBAPAngle( BAPType which, AngleType t);
	/** Get value of _one_ BAP in the BAPvector */
	double	GetBAPAngle( string mjointname, AngleType t);
	/** Get value of _one_ BAP in the BAPvector */
	double	GetBAPAngle( SidelessBAPType which, SideType side, AngleType t);

	/** Get value of _one_ BAP in the BAPvector */
	double	GetBAPLength( int which, LengthType t);
	/** Get value of _one_ BAP in the BAPvector */
	double	GetBAPLength( BAPType which, LengthType t);
	/** Get value of _one_ BAP in the BAPvector */
	double	GetBAPLength( string mjointname, LengthType t);
	/** Get value of _one_ BAP in the BAPvector */
	double	GetBAPLength( SidelessBAPType which, SideType side, LengthType t);

	/** Set value of _one_ BAP in the BAPvector */
	void	SetBAP( string mjointname, int value);
	/** Set value of _one_ BAP in the BAPvector */
	void	SetBAP( SidelessBAPType which, SideType side, int value);
	/** Set value of _one_ BAP in the BAPvector */
	void	SetBAP( BAPType which, int value);
	/** Set value of _one_ BAP in the BAPvector */
	void	SetBAP( int which, int value);

	/** Set value of one BAP int the BAPvector */
	void	SetBAP( string mjointname, AngleType t, double a);
	/** Set value of one BAP int the BAPvector */
	void	SetBAP( SidelessBAPType which, SideType side, AngleType t, double a);
	/** Set value of one BAP int the BAPvector */
	void	SetBAP( BAPType which, AngleType t, double a);
	/** Set value of one BAP int the BAPvector */
	void	SetBAP( int which, AngleType t, double a);

	/** accessor for mFrameNumber */
	int		GetFrameNumber( )		{ return mFrameNumber;}
	/** accessor for mFrameNumber */
	void	SetFrameNumber( int n)	{ /*assert(n>=0);*/ mFrameNumber=n;}


	/** returns frame description in MPEG4 BAP file format 
	*
	* The string can then be directly written to a BAP file */
	string	WriteBAP( );

	void BAPFrame::ReadFromBuffer(char *buffer);

	/** Read a pose definition file and store all values in the BAPvector */
	int	ReadPose( string file_name, bool skip_zeros = true, bool switch_sides = false);

	/** Read a pose definition file and store all values in the BAPvector */
	int	ReadPose( string file_name, bool skip_zeros, bool switch_sides, float scaling);

	/** Print info about self to cout */
	void Print( );

	/** Create the elements of the BAPvector and initialize to 0 */
	void Init( );

	void AddBAPFrame(BAPFrame *bframe);


	/** Tension-Continuity-Bias parameters in range [-1,1] 
	*  
	*  Used in interpolation step, but so far only initialized to default values.*/
	float	TCBParam[3];

	/* Decault constructor */
	BAPFrame(BAPConverter *converter);

	/** Construct and set frame number */
	BAPFrame(BAPConverter *converter,int num);

	/** Construct with pose file and frame number */
	BAPFrame(BAPConverter *converter,string filename, int n);

	/** Destructor */
	virtual ~BAPFrame();

	/** The vector that stores all BAP values */
	BAPVector		mBAPVector;

	std::string id;

private:
	/** the absolute frame number of this frame */
	int				mFrameNumber;
	/** BAPConverter object to perform various type conversions */
	BAPConverter	*mConverter;
protected:
	/** debug output flag */
	bool output;
};

/** A BAPFrameVector is a vector/array of pointers to BAPFrames */
typedef vector<BAPFrame *> BAPFrameVector;


#endif // !defined(AFX_BAPFrame_H__98FDBC8E_D9F5_4298_9606_4623BAC13E61__INCLUDED_)
