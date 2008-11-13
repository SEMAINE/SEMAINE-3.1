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

// TweenComponent.cpp: implementation of the TweenComponent class.
//
//////////////////////////////////////////////////////////////////////
#include "TweenComponent.h"
#include "GesturePhase.h"
#include "BAPconverter.h"
/**@#-*/
using namespace GestureSpace;
/**@#+*/

extern BAPConverter GestureGlobalBAPconverter;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TweenComponent::TweenComponent()
{
	mT=0.5;
	mFromFrame=NULL;
	mToFrame=NULL;
}

TweenComponent::~TweenComponent()
{

}

void TweenComponent::SetPhases(GesturePhase *from, GesturePhase *to)
{
	mFromFrame=from;
	mToFrame=to;
}

void TweenComponent::GetAngles(BAPFrame &f)
{
	BAPFrame BAPfrom(&GestureGlobalBAPconverter);
	BAPFrame BAPto(&GestureGlobalBAPconverter);

	assert((mFromFrame!=NULL)&&(mToFrame!=NULL));

	mFromFrame->GetAngles(BAPfrom);
	mToFrame->GetAngles(BAPto);

	int tween_val;
	for (int i=0; i<=NUMBAPS; i++) {
		if (BAPfrom.GetMask(i) && BAPto.GetMask(i)) {
			tween_val=int(float(BAPfrom.GetBAP(i))+mT*(float(BAPto.GetBAP(i)-BAPfrom.GetBAP(i))));
			f.SetBAP(i,tween_val);
		}
	}
}

void TweenComponent::SetT(float t)
{
	mT=t;
}
