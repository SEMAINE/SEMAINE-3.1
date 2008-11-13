//Copyright 1999-2008 Catherine Pelachaud - c.pelachaud@iut.univ-paris8.fr
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

// GazePseudoBelief.h: implementation for the GazePseudoBelief class.
//
//////////////////////////////////////////////////////////////////////

#include "GazePseudoBelief.h"

#include "IniManager.h"
extern IniManager inimanager;

//load the values of gaze  belief network

#include <stdio.h>
//#include <stdlib.h>


GazePseudoBelief::GazePseudoBelief()
{

	previousgazespeaker=-1;
	previousgazelistener=-1;
	
	movespeaker=-1;
	movelistener=-1;
	
	speakerduration=-1;
	listenerduration=-1;

	//load data
	FILE *fp;

	for(unsigned int i=0; i<64; i++)
	  for(unsigned int j=0; j<10; j++)
			belief_probability[i][j]=-1.0f;

	std::string filename=inimanager.Program_Path+"\\bn\\new_gaze";
	if((fp=fopen(filename.c_str(), "rb"))==NULL) 
	{
    printf("Cannot open file.\n");
	//throw Exception
	}

  if(fread(belief_probability, sizeof(float), 640, fp) != 640) 
  {
    if(feof(fp))
       printf("Premature end of file.");
    else
       printf("File read error.");
  }

  fclose(fp);

}
	
GazePseudoBelief::~GazePseudoBelief()
{

}

	
void GazePseudoBelief::setPreviousGazeSpeaker(int value)
{
	previousgazespeaker=value;
}

void GazePseudoBelief::setPreviousGazeListener(int value)
{
	previousgazelistener=value;
}

void GazePseudoBelief::setMoveSpeaker(int value)
{
	movespeaker=value;
	
}

void GazePseudoBelief::setMoveListener(int value)
{
	movelistener=value;
}

void GazePseudoBelief::setSpeakerDuration(int value)
{
	speakerduration=value;
}

void GazePseudoBelief::setListenerDuration(int value)
{
	listenerduration=value;
}
	

float GazePseudoBelief::getValue(int value)
{
	
	// if ((value>3)|| (value<0)) throw Exception
	
	/*
	
		if (
		(previousgazespeaker=-1)|| (previousgazelistener=-1)
		(movespeaker=-1)|| (movelistener=-1)
		(speakerduration=-1)|| (listenerduration=-1)	
	
	) throw Exception
	
	*/
	
	int licznik=0;
	
	if (previousgazespeaker==1)	licznik+=32;
	if (previousgazelistener==1) licznik+=16;
	if (movespeaker==1) licznik+=8;
	if (movelistener==1) licznik+=4;
	if (speakerduration==1) licznik+=2;
	if (listenerduration==1) licznik+=1;
	
	int index2=6+value;

	return belief_probability[licznik][index2];

}

void GazePseudoBelief::clear()
	{
	previousgazespeaker=-1;
	previousgazelistener=-1;
	
	movespeaker=-1;
	movelistener=-1;
	
	speakerduration=-1;
	listenerduration=-1;
}


