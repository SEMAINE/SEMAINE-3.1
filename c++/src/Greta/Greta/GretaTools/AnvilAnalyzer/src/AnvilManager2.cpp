// AnvilManager2.cpp: implementation of the AnvilManager2 class.
//
//////////////////////////////////////////////////////////////////////

#include "AnvilManager2.h"

#include <iostream>
#include <fstream>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AnvilManager2::AnvilManager2()
{
	nameFaceTrack="face";
	nameFaceTrack="head";
	nameFaceTrack="gaze";

	nameGestureTrack="gesture.g-signal.data";
	namePhaseTrack="gesture.g-signal.phase";
	anviltree=0;
}

AnvilManager2::~AnvilManager2()
{
	if(anviltree!=0)
		delete anviltree;
}

int AnvilManager2::ParseAnvil(std::string anvilfilename)
{
	XMLGenericParser parser;
	parser.SetValidating(false);
	anviltree=parser.ParseFile(anvilfilename);
	if(anviltree==0)
		return 0;
	return 1;
}

int AnvilManager2::SaveBML(std::string anvilfilename)
{

	std::string channel;
	std::string id, starttime, endtime;
	std::string classname,instancename;
	std::ofstream outputfile(anvilfilename.c_str());

	if(!outputfile.is_open())
	{
		return 0;
	}

	outputfile << "<?xml version=\"1.0\" ?>\n";
	outputfile << "<!DOCTYPE score SYSTEM \"\" []>\n";
	outputfile << "<bml>\n";

	XMLGenericTree *signals;

	if(anviltree==0)
		return 0;

	XMLGenericTree *body;

	body=anviltree->FindNodeCalled("body");

	if(body==0)
		return 0;

	std::list<XMLGenericTree*>::iterator track_iter;
	
	for(track_iter=body->child.begin();track_iter!=body->child.end();track_iter++)
	{
		if((*track_iter)->GetAttribute("name")=="face.eyebrows"||(*track_iter)->GetAttribute("name")=="face.eyelids")
		{
			channel="face";
			classname="eyes";
		}
		if((*track_iter)->GetAttribute("name")=="face.mouth")
		{
			channel="face";
			classname="mouth";
		}
		else
		if((*track_iter)->GetAttribute("name")=="head.movement"||(*track_iter)->GetAttribute("name")=="head.direction")
		{
			channel="head";
			classname="head";
		}
		else
		if((*track_iter)->GetAttribute("name")=="gaze")
		{
			channel="gaze";
			classname="gaze";
		}

		signals=(*track_iter);

		std::list<XMLGenericTree*>::iterator signal_iter;

		for(signal_iter=signals->child.begin();signal_iter!=signals->child.end();signal_iter++)
		{
			if((*signal_iter)->name=="el")
			{
				id=channel + (*signal_iter)->GetAttribute("index");
				starttime=(*signal_iter)->GetAttribute("start");
				endtime=(*signal_iter)->GetAttribute("end");
				std::list<XMLGenericTree*>::iterator attribute_iter;
					for(attribute_iter=(*signal_iter)->child.begin();attribute_iter!=(*signal_iter)->child.end();attribute_iter++)
				{
					if((*attribute_iter)->GetAttribute("name")=="signal")
						instancename=(*attribute_iter)->FindNodeCalled("text")->value;
				}

				outputfile << "<" << channel << " id='" << id;
				outputfile << "' type='" << classname << "=" << instancename;
				outputfile << "' start='" << starttime;
				outputfile << "' end='" << endtime <<"'/>\n";
			}
		}
	}
	
	outputfile << "</bml>";
	outputfile.close();
	
	if(track_iter==body->child.end())
		return 0;
	
	return 1;
}

XMLGenericTree *AnvilManager2::GetPhase(int index)
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

XMLGenericTree *AnvilManager2::FindFirstStroke(int indexstart,int indexend)
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