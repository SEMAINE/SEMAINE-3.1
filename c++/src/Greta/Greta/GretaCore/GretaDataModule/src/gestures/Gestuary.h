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

/// @file Gestuary.h
/// interface for the Gestuary class.
///
//////////////////////////////////////////////////////////////////////


#pragma once


//#include <iostream>
//#include <string>
//#include <map>
#include "GestureFileNames.h"
#include "Gesture.h"
//#include "GestureEngineGlobals.h"



/**@#-*/
using namespace std;
namespace GestureSpace {
	/**@#+*/

	/** GestureMapType is a map (={key,content} pair) to find gesture objects by name.
	*
	* The key value is the gesture description (eg. ADJECTIVAL=SMALL),
	* the content is a pointer to the respective gesture in the database 
	* @see Gesture 
	*/
	typedef map<string, Gesture*, less<string> > GestureMapType;

	/** The Gestuary is the database of known, defined gestures.
	*
	* The database is populated with gestures created by
	* reading gesture definition files in LoadLibrary().
	* Gestures are retrieved using FindBest()
	*
	* @author Bjoern Hartmann, bjoern@graphics.cis.upenn.edu 
	* @version 1.0
	*/
	class Gestuary  
	{

	public:
		/** output operator to print gesture to cout. OBSOLETE? */
		friend ostream& operator<<( ostream&, const Gestuary&);

		/** Load all known gesture definitions from file
		*
		* If you write a new gesture definition file, make sure to add it in here! */
		void			LoadLibrary( );

		/** Search for a gesture in the database by description. 
		*
		* If a matching gesture is found, returns a pointer to it. If no match is found, returns NULL */
		Gesture*		FindBest( string description);

		Gestuary( );
		virtual ~Gestuary( );

		/** The map used to seach for gestures by their descriptions */
		GestureMapType	mMap;
	protected:	
		/** Insert key for gesture into map; called by LoadLibrary() */
		void			Insert( Gesture* g);

		/** Insert key for gesture into map; called by LoadLibrary() */
		void			Insert( string key, Gesture* g);

		/** debug output flag */
		bool output;
	private:

	};

	/**@#-*/
} // END namespace GestureSpace
/**@#+*/

