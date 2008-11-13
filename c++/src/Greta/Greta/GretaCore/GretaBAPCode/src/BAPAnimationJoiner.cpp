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

#include "BAPAnimationJoiner.h"

BAPAnimationJoiner::BAPAnimationJoiner(void)
{
}

BAPAnimationJoiner::~BAPAnimationJoiner(void)
{
}

BAPFrameVector *BAPAnimationJoiner::Join(BAPFrameVector* a1,std::vector<BAPFrame>* a2)
{
	BAPFrameVector::iterator iter;
	std::vector<BAPFrame>::iterator iter2;
	iter2=a2->begin();
	int count;
	count=0;
	for(iter=a1->begin();iter!=a1->end();iter++)
	{
		if(iter2!=a2->end())
		{
			BAPVector::iterator bap1,bap2;
			bap1=(*iter)->mBAPVector.begin();
			for(bap2=(*iter2).mBAPVector.begin();bap2!=(*iter2).mBAPVector.end();bap2++)
			{
				if((*bap2).GetMask()==true)
					//if((*iter)->mBAPVector[(*bap).GetBAPType()].GetMask()==false)
					{
						if(bap1!=(*iter)->mBAPVector.end())
							(*iter)->mBAPVector[(*bap2).GetBAPType()].SetBAPValue((*bap2).GetBAPValue()+(*bap1).GetBAPValue());
						else
							(*iter)->mBAPVector[(*bap2).GetBAPType()].SetBAPValue((*bap2).GetBAPValue());
						(*iter)->mBAPVector[(*bap2).GetBAPType()].SetMask(true);
					}
				bap1++;
			}
			iter2++;
			count++;
		}
		else
			break;
	}
	return a1;
}