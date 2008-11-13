// AnvilManager2.h: interface for the AnvilManager2 class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "XMLGenericParser.h"
#include <string>

class AnvilManager2
{
public:
	AnvilManager2();
	virtual ~AnvilManager2();

	int ParseAnvil(std::string anvilfilename);

	int SaveBML(std::string anvilfilename);

private:
	XMLGenericTree *anviltree;
	std::string nameFaceTrack;
	std::string nameHeadTrack;
	std::string nameGazeTrack;

	std::string nameGestureTrack;
	std::string namePhaseTrack;

	XMLGenericTree *GetPhase(int index);

	XMLGenericTree *FindFirstStroke(int indexstart,int indexend);
};
