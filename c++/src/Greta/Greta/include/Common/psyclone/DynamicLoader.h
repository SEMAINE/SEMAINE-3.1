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


// OutputText.h: interface for the OutputText class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _DYNAMICLOADER_H_
#define _DYNAMICLOADER_H_

#include "standard.h"
#include <string.h>
#include <stdio.h>

#include "Object.h"
#include "JString.h"
#include "ObjectDictionary.h"

#include "dll.h"

#ifdef Darwin
#ifndef dlopen
	#define dlopen dllopen
	#define dladdr dlladdr
	#define dlclose dllclose
	#define dlerror dllerror
	#define dlsym dllsym
#endif
#endif

namespace cmlabs {

class LibHandle : public Object
{
public:
	LibHandle();
	virtual ~LibHandle();
	Object* clone() const {return NULL;}

	JString name;
	DLLHandle handle;
};

class DynamicLoader : public Object
{
public:
	DynamicLoader();
	virtual ~DynamicLoader();

	Object* clone() const {return NULL;}
	DLLHandle openHandle(JString libname);
	bool closeHandle(DLLHandle handle);
	bool closeAllHandles();
	bool clearErrors();

	bool loadLibrary(JString libname);
	bool unloadLibrary(JString libname);

	DLLFunction getFunction(JString libname, JString funcname);
	DLLFunction getFunction(JString funcname);
	DLLFunction getFunction(DLLHandle handle, JString funcname);

	bool didErrorHappen();
	JString getLastErrorMessage();

	bool setTestLib(JString lib);
	bool unitTest();

protected:

	ObjectDictionary libHandles;
	JString lastErrorMessage;
	JString testlib;
	JMutex libMutex;
};

} // namespace cmlabs

#endif // _DYNAMICLOADER_H_
