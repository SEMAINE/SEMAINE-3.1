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

//////////////////////////////////////////////////////////////////////
#include <string>
#include <map>
#include <vector>

#include "XMLGenericParser.h"
#include "HeadGesture.h"


namespace HeadSpace {

	/**
	* class :HeadGestuary
	*
	*/

	class HeadGestuary 

	{

	public:


		/**
		* contructor 
		*
		*/

		HeadGestuary();

		/**
		* destructor 
		*/

		virtual ~HeadGestuary();


		/**
		* 
		*
		* @return 
		* @param  gestuaryfilename
		*/

		int LoadGestuary(std::string gestuaryfilename);


		/**
		* 
		*
		* @return 
		*/

		std::map<std::string,HeadGesture> getGestuary();

		std::map<std::string,HeadGesture> gestuary;
		std::vector<HeadGesture> gestures;

	};
}
