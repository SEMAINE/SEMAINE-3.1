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

// 
//
//////////////////////////////////////////////////////////////////////


#include <stdio.h>	    	// Header File For Standard Input/Output
#include <stdlib.h>
#include "BAPData.h"

BAPData::BAPData()
{
	//frames = new BAPframeVector ;

	numberofframes = 0;
	fps = 0;
	version = 0;
}

// Load a BAP file from disk using the BAPParser class
bool BAPData::ReadBAPFile(char* path)
{
	int status;
	BAPParser p;
	
	frames.clear();// delete any previous frames.

	status= p.ParseFile(string(path),&frames, NULL);

	numberofframes=(int)frames.size();
	fps=p.fps;

	if(status==1) return false;
	return true;
	//todo: get fps?
}

// Load a BAP file from buffer using the BAPParser class
bool BAPData::ReadBAPBuffer(char* buffer)
{
	int status;
	BAPParser p;
	
	frames.clear();// delete any previous frames.

	status= p.ParseBuffer(buffer,&frames);

	numberofframes=(int)frames.size();
	fps=p.fps;

	if(status==1) return false;
	return true;
	//todo: get fps?
}
