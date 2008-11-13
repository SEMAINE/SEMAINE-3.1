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

// BAP.h: interface for the BAP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BAP_H__D22F1601_3E6B_48C8_B8F8_CE62E7C5B481__INCLUDED_)
#define AFX_BAP_H__D22F1601_3E6B_48C8_B8F8_CE62E7C5B481__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <iostream>
#include <vector>
#include "baptypes.h"


/**@#-*/
using namespace std ;
using namespace std::rel_ops ;
/**@#+*/

	/**
	 * BAP=Body Animation Parameter class. Holds one BAP w/ accessors. 
	 *
	 * @author Bjoern Hartmann, bjoern@graphics.cis.upenn.edu 
	 * @version 1.0
	 */
	class BAP  
	{
	public:

		/** INLINE accessor for mMask */
		bool		GetMask()				{ return mMask; }
		
		/** Conversion routine that returns BAP value in degrees or radians */
		float		GetAngleValue(AngleType t);
		
		/** @return BAP value in radians */
		float		GetAngleValue()			{ return (float(mBAPValue)/100000.f); }
		
		/** accessor for BAP type */
		BAPType		GetBAPType()			{ return mBAPType; }
		
		/** INLINE accessor for BAP value = int(radians*100000.0) */
		int			GetBAPValue()			{ return mBAPValue; }
		
		/** convert from angles or radians and set BAP value */
		void		SetAngleValue(AngleType a, float f);
		
		/** Set BAP value */
		void		SetBAPValue(int i)		{ mBAPValue=i; }
		
		/** Set mask */
		void		SetMask(bool b)			{ mMask=b; }
		
		/** Default constructor */
		BAP() : mMask(false), mBAPType(null_bap), mBAPValue(0) {}
		
		/** Construct and set mBAPType */
		BAP(BAPType btype);
		
		/** Dectructor */
		virtual ~BAP();


		/** Is this BAP active? true=yes, false=no */
		bool		mMask;
		
		/** The type/number of this BAP. */
		BAPType		mBAPType;
		
		/** The value of this BAP as int(radians*100000.0) */
		int			mBAPValue;

	};
	
	/** A BAPVector is a vector/array of BAP objects */
	typedef vector<BAP> BAPVector;


#endif // !defined(AFX_BAP_H__D22F1601_3E6B_48C8_B8F8_CE62E7C5B481__INCLUDED_)
