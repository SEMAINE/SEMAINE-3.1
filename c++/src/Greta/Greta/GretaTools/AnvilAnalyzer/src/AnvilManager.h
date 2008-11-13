// AnvilManager.h: interface for the AnvilManager class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "XMLGenericParser.h"
#include <string>

class AnvilManager  
{
public:
	AnvilManager();
	virtual ~AnvilManager();

	int ParseAnvil(std::string anvilfilename);

	int SaveBML(std::string anvilfilename);

private:
	XMLGenericTree *anviltree;
	std::string nameGestureTrack;
	std::string namePhaseTrack;

	XMLGenericTree *GetPhase(int index);

	XMLGenericTree *FindFirstStroke(int indexstart,int indexend);
};
