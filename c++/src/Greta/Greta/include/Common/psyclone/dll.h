/***************************** License **********************************

 Copyright (C) 2006 by Communicative Machines
 http://www.cmlabs.com   All rights reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

************************************************************************/


#include "Messenger.h"

#ifndef _DLL_H_
#define _DLL_H_

	#ifdef WIN32
		#include <windows.h>
		#define DLLHandle HINSTANCE
	#else // WIN32
		#ifdef Darwin
			#include "dlfcn_darwin.h"
		#else
			#include <dlfcn.h>
		#endif
		#define DLLHandle void*
	#endif // WIN32

// ****** Here goes the dll function declarations ******

namespace cmlabs {

//	typedef int (* DLLFunction)(ObjectCollection* inputMessages, ObjectDictionary* parameters, ObjectDictionary* mediaConnections, ObjectCollection* outputMessages);
	typedef int (* DLLFunction)(Messenger* messenger);
	typedef int (* DLLUnitTestFunction)(ObjectCollection* col);

} // namespace cmlabs

// *****************************************************

#endif // _DLL_H_
