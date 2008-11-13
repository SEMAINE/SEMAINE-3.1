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
#include "aiPersonMemory.h"
#include <vector>

AIPersonMemory::AIPersonMemory()
{

}

/**
 * Destructor
 */
AIPersonMemory::~AIPersonMemory()
{
  //delete the early person percepts list
}

void AIPersonMemory::addRecord(float bodyDegrees, float headDegrees, float eyeDegrees, float timeStamp)
{
  PersonPerceptSTSS* pp = new PersonPerceptSTSS(bodyDegrees, headDegrees, eyeDegrees, timeStamp);
  pp->update();
  
  attentionProfile.push_back(pp);
  if (attentionProfile.size() > 1) 
  {
    //store the interest level between the last record and this one
    pp->ILcache = getInterestLevel(attentionProfile.size()-1); 
  }
}

//returns value between 0.0 and 1.0
//get the attention level at any timestamp, interpolating if necessary
float AIPersonMemory::getAttentionLevel(float timeStamp)
{
  int nextRecIndex = getNearestNextRecordIndex(timeStamp);

  //toppa!
  if(nextRecIndex==0) return 0;

  PersonPerceptSTSS* nextRec = attentionProfile[nextRecIndex];
  
  if (nextRec->timeStamp == timeStamp) 
    return nextRec->attentionLevelMetric;

  PersonPerceptSTSS* previousRec = attentionProfile[nextRecIndex-1];

 
  //do a linear interpolation if timeStamp not on a perceptual update event
  float previousAL = previousRec->attentionLevelMetric;
  float nextAL = nextRec->attentionLevelMetric;
  float tTotal = nextRec->timeStamp - previousRec->timeStamp;
  float tDelta = (timeStamp - previousRec->timeStamp)/tTotal;
  return (1-tDelta)*previousAL + tDelta*nextAL;
}

//gets the next record thats closest to the specific time
int AIPersonMemory::getNearestNextRecordIndex(float timeStamp)
{
  std::vector<PersonPerceptSTSS*>::iterator PPitr = attentionProfile.begin();
  std::vector<PersonPerceptSTSS*>::iterator PEitr = attentionProfile.end();

  for (int i = 0; PPitr != PEitr; PPitr++, i++)
  {
    if (timeStamp == (*PPitr)->timeStamp) return i;
	if (timeStamp < (*PPitr)->timeStamp) return i;
  }
  return attentionProfile.size();
}

//gets the previous record thats closest to the specific time
int AIPersonMemory::getNearestPreviousRecordIndex(float timeStamp)
{
  std::vector<PersonPerceptSTSS*>::iterator PPitr = attentionProfile.begin();
  std::vector<PersonPerceptSTSS*>::iterator PEitr = attentionProfile.end();

  for (int i = 0; PPitr != PEitr; PPitr++, i++)
  {
    if (timeStamp == (*PPitr)->timeStamp) return i;
	if (timeStamp < (*PPitr)->timeStamp) return i-1;
  }
  return attentionProfile.size();
}

//get time between any two times on the attention profile
//presume both times correspond to records
//returns value between 0.0 and 1.0
float AIPersonMemory::getInterestLevel(float time1, float time2)
{
  int numAttentionRecords = attentionProfile.size();

  if (numAttentionRecords == 0) return 0.0;
  if (numAttentionRecords == 1) return attentionProfile[0]->attentionLevelMetric;

  if ((time2 < time1) ||
	 (time1 < attentionProfile[0]->timeStamp) || 
     (time2 > attentionProfile[numAttentionRecords-1]->timeStamp))
  {
    //printf("Warning - time range out of bounds\n");
	int i=numAttentionRecords-1;
	while((i>=0)&&(attentionProfile[i]->timeStamp>time2))
		i=i-1;
	time2=attentionProfile[i]->timeStamp;
    //return 0.0;
  }

  //find the nearest next record to time1
  int startRec = getNearestNextRecordIndex(time1);
  //find the nearest previous record to time2
  int endRec = getNearestPreviousRecordIndex(time2);
  //if they are the same, just return integral of interpolated AL values

//  printf("StartRec: %d endRec: %d", startRec, endRec);

  float accumulator = 0.0, tDelta = 0.0, t1AL = 0.0, t2AL = 0.0;

  //get integral from time1 up to startRec
  if (time1 != attentionProfile[startRec]->timeStamp)
  {
    t1AL = getAttentionLevel(time1);
    t2AL = attentionProfile[startRec]->attentionLevelMetric;
    tDelta = attentionProfile[startRec]->timeStamp - time1;
    accumulator += (t1AL+t2AL)*tDelta*0.5;
  }

//  printf("1: Accumulator %lf", accumulator);

  //simple trapeziodal integration between each record
  for (int i = startRec+1; i < endRec; i++){
    /*
    t1AL = attentionProfile[i]->attentionLevelMetric;
    t2AL = attentionProfile[i+1]->attentionLevelMetric;
    tDelta = attentionProfile[i+1]->timeStamp - attentionProfile[i]->timeStamp;
    accumulator += (t1AL+t2AL)*tDelta*0.5;
    */
    if (i != 0) accumulator += attentionProfile[i]->ILcache; 
  }
  accumulator += attentionProfile[endRec]->ILcache; 

//	printf("2: Accumulator %lf", accumulator);

  //get integral from endRec up to time2
  if (time2 != attentionProfile[endRec]->timeStamp)
  {
    t1AL = attentionProfile[endRec]->attentionLevelMetric;
    t2AL = getAttentionLevel(time2);
    tDelta = time2 - attentionProfile[endRec]->timeStamp;
    accumulator += (t1AL+t2AL)*tDelta*0.5;
  }

//  printf("3: Accumulator %lf", accumulator);

  float maxArea = time2 - time1;
  return accumulator/maxArea;
}

float AIPersonMemory::getInterestLevel(int recIndex)
{
  //get the interest level between this percept record and the previous sample
  float t1AL = attentionProfile[recIndex]->attentionLevelMetric;
  float t2AL = attentionProfile[recIndex-1]->attentionLevelMetric;
  float tDelta = attentionProfile[recIndex]->timeStamp - attentionProfile[recIndex-1]->timeStamp;
  return ((t1AL+t2AL)*tDelta*0.5);
}
