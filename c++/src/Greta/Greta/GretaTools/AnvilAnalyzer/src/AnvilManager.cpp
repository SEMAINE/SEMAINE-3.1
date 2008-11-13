// AnvilManager.cpp: implementation of the AnvilManager class.
//
//////////////////////////////////////////////////////////////////////

#include "AnvilManager.h"

#include <iostream>
#include <fstream>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AnvilManager::AnvilManager()
{
	nameGestureTrack="gesture.g-signal.data";
	namePhaseTrack="gesture.g-signal.phase";
	anviltree=0;
}

AnvilManager::~AnvilManager()
{
	if(anviltree!=0)
		delete anviltree;
}

int AnvilManager::ParseAnvil(std::string anvilfilename)
{
	XMLGenericParser parser;
	parser.SetValidating(false);
	anviltree=parser.ParseFile(anvilfilename);
	if(anviltree==0)
		return 0;
	return 1;
}

int AnvilManager::SaveBML(std::string anvilfilename)
{

	std::ofstream outputfile(anvilfilename.c_str());

	if(!outputfile.is_open())
	{
		return 0;
	}

	outputfile << "<?xml version=\"1.0\" ?>\n";
	outputfile << "<!DOCTYPE score SYSTEM \"\" []>\n";
	outputfile << "<bml>\n";

	XMLGenericTree *gestures;

	if(anviltree==0)
		return 0;

	XMLGenericTree *body;

	body=anviltree->FindNodeCalled("body");

	if(body==0)
		return 0;

	std::list<XMLGenericTree*>::iterator track_iter;
	
	for(track_iter=body->child.begin();track_iter!=body->child.end();track_iter++)
		if((*track_iter)->GetAttribute("name")==nameGestureTrack)
		{
			break;
		}
	if(track_iter==body->child.end())
		return 0;

	gestures=(*track_iter);

	std::list<XMLGenericTree*>::iterator gesture_iter;

	for(gesture_iter=gestures->child.begin();gesture_iter!=gestures->child.end();gesture_iter++)
		if((*gesture_iter)->name=="el")
		{
			std::string classname,instancename;
			std::list<XMLGenericTree*>::iterator attribute_iter;
			XMLGenericTree* startphase,*endphase,*strokephase;
			std::string starttime,endtime;
			for(attribute_iter=(*gesture_iter)->child.begin();attribute_iter!=(*gesture_iter)->child.end();attribute_iter++)
			{
				if((*attribute_iter)->GetAttribute("name")=="class")
				{
					classname=(*attribute_iter)->FindNodeCalled("text")->value;
				}
				if((*attribute_iter)->GetAttribute("name")=="instance")
					instancename=(*attribute_iter)->FindNodeCalled("text")->value;
			}
			startphase=GetPhase((*gesture_iter)->GetAttributef("start"));
			endphase=GetPhase((*gesture_iter)->GetAttributef("end"));

			if(startphase==0)
				return 0;
			if(endphase==0)
				return 0;
			
			starttime=startphase->GetAttribute("start");

			endtime=endphase->GetAttribute("end");

			std::string firststroke;

			strokephase=FindFirstStroke((*gesture_iter)->GetAttributef("start"),(*gesture_iter)->GetAttributef("end"));

			if(strokephase!=0)
				firststroke=strokephase->GetAttribute("end");

			outputfile << "<gesture id='gesture_" << (*gesture_iter)->GetAttribute("index");
			outputfile << "' type='" << classname << "=" << instancename;
			outputfile << "' start='" << starttime;
			outputfile << "' end='" << endtime <<"'>\n";
			outputfile << "<gesture id='gesture_" << (*gesture_iter)->GetAttribute("index");
			outputfile << "' stroke='" << firststroke <<"'>\n";
		}

	outputfile << "</bml>";

	outputfile.close();

	return 1;
}

XMLGenericTree *AnvilManager::GetPhase(int index)
{
	XMLGenericTree *phases;

	XMLGenericTree *result;

	result=0;

	if(anviltree==0)
		return 0;

	XMLGenericTree *body;

	body=anviltree->FindNodeCalled("body");

	if(body==0)
		return 0;

	std::list<XMLGenericTree*>::iterator track_iter;

	for(track_iter=body->child.begin();track_iter!=body->child.end();track_iter++)
		if((*track_iter)->GetAttribute("name")==namePhaseTrack)
		{
			break;
		}

	if(track_iter==body->child.end())
		return 0;

	phases=(*track_iter);

	std::list<XMLGenericTree*>::iterator phase_iter;

	for(phase_iter=phases->child.begin();phase_iter!=phases->child.end();phase_iter++)
		if((*phase_iter)->name=="el")
		{
			if((*phase_iter)->GetAttributef("index")==index)
			{
				result=(*phase_iter);
				break;
			}
		}

	return result;
}

XMLGenericTree *AnvilManager::FindFirstStroke(int indexstart,int indexend)
{
	XMLGenericTree *phase;

	XMLGenericTree *result;

	result=0;

	for(int i=indexstart;i<=indexend;i++)
	{
		phase=GetPhase(i);

		std::list<XMLGenericTree*>::iterator attribute_iter;
		for(attribute_iter=phase->child.begin();attribute_iter!=phase->child.end();attribute_iter++)
		{
			if((*attribute_iter)->GetAttribute("name")=="name")
				if((*attribute_iter)->FindNodeCalled("text")->value=="stroke")
				{
					result=phase;
					break;
				}
		}

	}

	return result;
}