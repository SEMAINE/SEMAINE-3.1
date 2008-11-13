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

// WristOrientation.cpp: implementation of the WristOrientation class.
//
//////////////////////////////////////////////////////////////////////
#include "WristOrientation.h"

/**@#-*/
using namespace std ;
using namespace GestureSpace;
/**@#+*/

// static class variables need to be instantiated at file scope
WristAngleSkeleton * WristOrientation::mWASkeleton=new WristAngleSkeleton();

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

WristOrientation::WristOrientation()
{
	palm=PalmNone;
	fingerbase=FBNone;
	fileFolder=fileFolder+"wrists\\";
	
	slerp=1.0f;
	fromPalm=PalmNone;
	fromFingerbase=FBNone;
	interpolating=false;
}

WristOrientation::~WristOrientation()
{
}

void WristOrientation::Print()
{
	cout << "WristOrientation::Print()\n"; 
	cout << "+--- PalmOrientation : "<<palm<<endl; 
}

/* implements GestureComponent::GetAngles */
void WristOrientation::GetAngles(BAPFrame &f)
{
	if ((fromPalm==PalmNone) && (fromFingerbase==FBNone)) {
		// don't write any angles
		return;
	}	
	if (output) cout <<"s WristOrientation::GetAngles(BAPframe &f)\n";

	////////***********************MODIFICA MAURIZIO 11/9/2003***************
	////////********************************ATTENZIONE!!!!!!!!!!!!!!!!!!!!!!!
	mWASkeleton->GetWristAngles(f, fromPalm, fromFingerbase, palm, fingerbase, side, slerp);
	////////***************TOGLIERE QUESTI COMMENTI SE CI SONO PROBLEMI COI POLSI
	////////***************E COMPILARE ANCHE JOINT,WRISTANGLESKELETON E SKELETON
	return;
}

/** seems obsolete now. */
void WristOrientation::Concretize()
{
	status = concrete;
}
