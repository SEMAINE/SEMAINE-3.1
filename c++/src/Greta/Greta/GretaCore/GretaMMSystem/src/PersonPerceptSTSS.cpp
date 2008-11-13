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

//-----------------------------------------------------------------------------
// Christopher Peters
//-----------------------------------------------------------------------------
#include "PersonPerceptSTSS.h"
#include "math.h"

PersonPerceptSTSS::PersonPerceptSTSS(float body, float head, float eyes, float tS)
{
    //construct a new percept
    eyeAngleDeg = eyes;
    headAngleDeg = head;
    bodyAngleDeg = body;
    distance = 0.0;
    eyesVisible = true;
    headVisible = true;
    bodyVisible = true;
    timeStamp = tS;
    MAMactivated = false;
    ILcache = 0.0;
}

void PersonPerceptSTSS::update(void)
{
	MAMactivated = (eyeAngleDeg > -1.0) && (eyeAngleDeg < 1.0);

    //update a percept
	attentionLevelEyes = 1.0 - fabs(eyeAngleDeg)/90.0;
	attentionLevelHead = 1.0 - fabs(headAngleDeg)/90.0;
	attentionLevelBody = 1.0 - fabs(bodyAngleDeg)/90.0; 

    //simplified metric
	if(MAMactivated)
		attentionLevelMetric = ((attentionLevelEyes  * 0.6) + 
								(attentionLevelHead  * 0.3) + 
								(attentionLevelBody  * 0.1));
	else
		attentionLevelMetric = ((attentionLevelEyes  * 0.5) + 
								(attentionLevelHead  * 0.3) + 
								(attentionLevelBody  * 0.2));

	//printf("attention %f %f %f\n",attentionLevelEyes,attentionLevelHead,attentionLevelBody);

    
}