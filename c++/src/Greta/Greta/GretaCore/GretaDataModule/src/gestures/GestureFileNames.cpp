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

// GestureFileNames.cpp: implementation of the GestureFileNames class.
//
//////////////////////////////////////////////////////////////////////
#include <cassert>

#include "GestureFileNames.h"
//#include "winbase.h"
#include "IniManager.h"

extern IniManager inimanager;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GestureFileNames::GestureFileNames()
{	
	std::string gesturesfolder;
	assert(inimanager.initialized);
	gesturesfolder=inimanager.Program_Path+"gestures\\*.txt";
	filehandle=FindFirstFile(gesturesfolder.c_str(), &filedata);
	int i=GetLastError();
}

char *GestureFileNames::GetFileName(){
	return filedata.cFileName;
}

bool GestureFileNames::Next(){
	BOOL result=1; //changed from bool b/c of WIN32 call below 2004-04-21 bjoern
	bool nomorefiles=1;
	while(result!=0){
		result=FindNextFile(filehandle, &filedata);
		if(result==0)nomorefiles=0;
		if((result!=0)&&(filedata.dwFileAttributes!=FILE_ATTRIBUTE_DIRECTORY))
			result=0;
	}
	if(nomorefiles==0){
		Close();
		return 0;
	}else return 1;
}



void GestureFileNames::Close(){
	FindClose(filehandle);
}

GestureFileNames::~GestureFileNames()
{

}
