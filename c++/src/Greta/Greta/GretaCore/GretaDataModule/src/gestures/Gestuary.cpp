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

// Gestuary.cpp: implementation of the Gestuary class.
//
//////////////////////////////////////////////////////////////////////
#define OUTPUT

#include "Gestuary.h"

#include "IniManager.h"
extern IniManager inimanager;

/**@#-*/
using namespace GestureSpace;
/**@#+*/
extern FILE* data_log;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Gestuary::Gestuary()
{
	mMap.clear();
	output=true;
}

Gestuary::~Gestuary()
{
	GestureMapType::const_iterator theIterator;
	theIterator = mMap.begin();
	for(;theIterator!=mMap.end(); theIterator++)
	{
		delete (*theIterator).second;	
	}
	mMap.clear();
}

Gesture* Gestuary::FindBest(string description)
{
	GestureMapType::iterator theIterator;
	theIterator = mMap.find(description);
	if(theIterator != mMap.end() ) {
#ifdef OUTPUT
		fprintf(data_log,"* Gestuary: Gesture found. Argument: %s\n",description.c_str());
#endif
		return (theIterator->second);
	} else {    // some character other than 0 - 9
#ifdef OUTPUT
		fprintf(data_log,"! Gestuary: Gesture NOT found! Argument: %s\n",description.c_str());
#endif
       	return 0;
	}
}

void Gestuary::Insert(string key, Gesture *g)
{
	mMap.insert(GestureMapType::value_type(key,g));
}

void Gestuary::Insert(Gesture *g)
{
	mMap.insert(GestureMapType::value_type(g->GetDescription(),g));
#ifdef OUTPUT
	fprintf(data_log,"Loaded gesture: %s\n",g->GetDescription().c_str());
#endif

}

void Gestuary::LoadLibrary()
{
	GestureFileNames filenames;
	Gesture *g;
	bool flag=false;
	while(flag==false)
	{

#ifdef OUTPUT
		fprintf(data_log,"Reading gesture: %s\n",filenames.GetFileName());
#endif
		
		g = new Gesture();
		g->Load(inimanager.Program_Path+"gestures/"+filenames.GetFileName());
		Insert(g);
		if(filenames.Next()==0)flag=true;
	}

}

ostream& operator<<(ostream& os, const Gestuary& g)
{
	
	os << "GESTUARY:\n";
	GestureMapType::const_iterator theIterator;
	theIterator = g.mMap.begin();
	for(;theIterator!=g.mMap.end(); theIterator++) {
		os << theIterator->first<<"\t"<<theIterator->second<<endl;
		
	}
		return os;
}
