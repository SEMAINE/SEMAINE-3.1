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

/// @file GestureEngine.h
/// interface for the GestureEngine class.
///
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GESTUREENGINE_H__EE1EBDBE_99B6_4D9D_A066_EB29026872EC__INCLUDED_)
#define AFX_GESTUREENGINE_H__EE1EBDBE_99B6_4D9D_A066_EB29026872EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "GestureEngineGlobals.h"

#include "TurnPlanner.h"
/**@#-*/
namespace GestureSpace {
	/**@#+*/

	/** 
	* class : GestureEngine
	* Encapsulates the entire gesture generating phase.
	* Interface allows for generation of one conversational turn at a time.
	*
	*/

	class GestureEngine  
	{
	private:
		GestureSpace::TurnPlanner* TP;
	public:

		/**
		* contructor 
		*
		*/

		GestureEngine();

		/**
		*  
		* 
		*
		* @param  bapfilename
		* @param  turnsfilename
		*/
		void CalculateTurn(std::string bapfilename,std::string turnsfilename);

		/**
		* destructor 
		*/
		virtual ~GestureEngine();

	};

	/**@#-*/
} //END namespace GestureSpace
/**@#+*/

#endif // !defined(AFX_GESTUREENGINE_H__EE1EBDBE_99B6_4D9D_A066_EB29026872EC__INCLUDED_)
