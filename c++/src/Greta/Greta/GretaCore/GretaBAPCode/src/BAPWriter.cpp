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

// BAPWriter.cpp: implementation of the BAPWriter class.
//
//////////////////////////////////////////////////////////////////////
//include "stdafx.h"
#include "BAPWriter.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BAPWriter::BAPWriter()
{

}

BAPWriter::~BAPWriter()
{

}

int BAPWriter::WriteToFile(BAPFrameVector *frames, string filename)
{
	int framesPerSecond = 25;//2004-04-19 bjoern: debug interpolation problem
	int numFrames = (*frames).size();
	// OPEN OUTPUT FILE
	ofstream outfile(filename.c_str());
	if (!outfile) { 
		cout << "! BAPWriter::WriteToFile: error opening output file! Returning.\n"; 
		return 1;
	}
	
	//output header info
	outfile << "0.0 " << "ActionName " << framesPerSecond <<" " << numFrames <<endl;
	
	// ITERATE THROUGH OUTPUT FRAMES: GET ANGLES AND WRITE THEM TO FILE
	BAPFrameVector::iterator iter;
	for(iter=(*frames).begin();iter!=(*frames).end();iter++) {
		outfile << (*iter)->WriteBAP();
	}
	
	outfile.close();	
	
	return 0;
}


int BAPWriter::WriteToString(BAPFrameVector *frames, string **str)
{
	int framesPerSecond = 25;//2004-04-19 bjoern: debug interpolation problem
	int numFrames = (*frames).size();
	// OPEN OUTPUT FILE
	ostringstream outfile;
	
	//output header info
	outfile << "0.0 " << "ActionName " << framesPerSecond <<" " << numFrames <<endl;
	
	// ITERATE THROUGH OUTPUT FRAMES: GET ANGLES AND WRITE THEM TO FILE
	BAPFrameVector::iterator iter;
	for(iter=(*frames).begin();iter!=(*frames).end();iter++) {
		outfile << (*iter)->WriteBAP();
	}
	
	string *res = new string(outfile.str());
	*str=res;
	
	return 0;
}
