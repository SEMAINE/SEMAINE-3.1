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

#ifndef _AIPERSONMEMORY_H_
#define _AIPERSONMEMORY_H_

#include "PersonPerceptSTSS.h"
/**
 *  An artificial intelligence personnal memories class. 
 *  Used to implement the memory.
 *  @author Christopher Peters
 *  @date 2005
 */
class AIPersonMemory {

private:
public:
	/**
	* the attention profile (just an array of person percepts)
	*/
  std::vector<PersonPerceptSTSS*> attentionProfile;

public:

  /**
  * Constructor to use
  */ 
  AIPersonMemory();
  /**
  * Destructor
  */
  ~AIPersonMemory();
  /**
  * Add a record into the memory
  * @param float bodyDegrees
  * @param float headDegrees
  * @param float eyeDegrees
  * @param float timeStamp
  */
  void addRecord(float bodyDegrees, float headDegrees, float eyeDegrees, float timeStamp);
  /**
  * The attention level getter
  * @param float timeStamp the concerned timestamp
  * @return the attention level at the concerned timestamp (value between 0.0 and 1.0)
  */
  float getAttentionLevel(float timeStamp);
  /**
  * Gets the index of the next record nearest to timeStamp
  * @param float timeStamp the concerned timestamp
  * @return the next nearest record
  */
  int getNearestNextRecordIndex(float timeStamp);
  /**
  * Gets the index of the previous record nearest to timeStamp
  * @param float timeStamp the concerned timestamp
  * @return the previous nearest record
  */
  int getNearestPreviousRecordIndex(float timeStamp);
  /**
  * Gets the interest level between the two time stamps
  * @param float time1 the begining time stamp
  * @param float time2 the ending time stamp
  * @return the interest level between the two time stamps (value between 0.0 and 1.0)
  */
  float getInterestLevel(float time1, float time2);
  /**
  * Gets the interest level between this record and the previous one
  * @param int recIndex the concerned index
  * @return the interest level between this record and the previous one
  */
  float getInterestLevel(int recIndex);

  //float classifyAttentionProfile(float time1, float time2);

};

#endif
