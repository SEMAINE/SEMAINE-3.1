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

#include ".\attentioninterestmodel.h"
#include <stdlib.h>
#include "RandomGen.h"

extern RandomGen *randomgen;

AttentionInterestModel::AttentionInterestModel(void)
{
}

AttentionInterestModel::~AttentionInterestModel(void)
{
}

void AttentionInterestModel::addRecord(float bodyDegrees, float headDegrees, float eyeDegrees, float currentTime)
{
}

MemoryRecord* AttentionInterestModel::queryRecord(float timeStamp)
{
	return 0;
}

MemoryRecord* AttentionInterestModel::queryRecord(int listNumber)
{
	return 0;
}

float AttentionInterestModel::getAttentionLevel(float timeStamp)
{
	return 0;
}

float AttentionInterestModel::getInterestLevelBack(float timeInterval, float currentTime)
{
	return 0;
}

float AttentionInterestModel::getInterestLevelInterval(float timeInterval1, float timeInterval2)
{
	return randomgen->GetRand01();
}