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

#include "FAPAnimationJoiner.h"

FAPAnimationJoiner::FAPAnimationJoiner(void)
{
}

FAPAnimationJoiner::~FAPAnimationJoiner(void)
{
}

FAPFrameVector *FAPAnimationJoiner::Join(FAPFrameVector* a1,FAPFrameVector* a2)
{
	FAPFrameVector::iterator iter;
	FAPFrameVector::iterator iter2;
	iter2=a2->begin();
	int count;
	count=0;
	for(iter=a1->begin();iter!=a1->end();iter++)
	{
		if(iter2!=a2->end())
		{
			int fap1,fap2;
			fap1=0;
			for(fap2=0;fap2<NUMBER_OF_FAPS;fap2++)
			{
				if((*iter2).FAPs[fap2].active==true)
					//if((*iter)->mBAPVector[(*bap).GetBAPType()].GetMask()==false)
					{
						if(fap1<NUMBER_OF_FAPS)
							(*iter).FAPs[fap2].value=((*iter2).FAPs[fap2].value+(*iter).FAPs[fap1].value);
						else
							(*iter).FAPs[fap2].value=(*iter).FAPs[fap2].value;
						(*iter).FAPs[fap2].active=true;
					}
				fap1++;
			}
			iter2++;
			count++;
		}
		else
			break;
	}
	return a1;
}