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

// FAPwriter.cpp: implementation of the FAPwriter class.
//
//////////////////////////////////////////////////////////////////////
//include "stdafx.h"
#include "FAPwriter.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FAPwriter::FAPwriter()
{

}

FAPwriter::~FAPwriter()
{

}

int FAPwriter::WriteToFile(FAPFrameVector *frames, std::string fapfilename, int fps)
{
	
	std::string basename;
	FILE *fapfile;
  	int fapnum,frameind;
	int i;

	if(fapfilename=="")
		return 0;

	if(frames==0)
		return 0;

	if(!(fapfile = fopen(fapfilename.c_str(),"w"))) 
  	{
		printf("ERROR: can't create %s\n",fapfilename.c_str());
		return 0;
  	}
  
	/* Scrivi versione, fapname, framerate e numero totale di frame*/
	
	printf("writing \"%s\"\n",fapfilename.c_str());

	basename=fapfilename;

	i=0;

	while (fapfilename[i]!='.')
	{
		if(fapfilename[i]==' ')
			basename[i]='_';
		i++;
	}
		
	fprintf(fapfile,"%.1f %s %d %d\n",2.1,basename.c_str(),fps,(*frames).size());

	/* Per ogni frame */
  	
	for(frameind=0;frameind<(*frames).size();frameind++) 
	{

		/* Scrivi la fap mask */

		for(fapnum=1;fapnum<69;fapnum++) 
			fprintf(fapfile,"%d ",(*frames)[frameind].FAPs[fapnum].active);
		fprintf(fapfile,"\n");
	
		/* Scrivi il frame corrente */
		/* Devo decidere se fare come Stefano e usare questo elemento */
		/* o se usare direttamente frameind */

		fprintf(fapfile,"%d ",(*frames)[frameind].GetFAP(0));

		/* Scrivi i vari faps */
		for(fapnum=1;fapnum<69;fapnum++) 
			if((*frames)[frameind].FAPs[fapnum].active==true) 
				fprintf(fapfile,"%d ",(*frames)[frameind].GetFAP(fapnum));
		fprintf(fapfile,"\n");
	}      

	fclose(fapfile);
	
	//Liberare le strutture TCB
	//for(i=0; i<68; i++)
		
	return 1;
	//printf("OK!\n");
	
}


std::string FAPwriter::FapFrameToString(FAPFrameVector *frames, int fap)
{
	(*frames)[fap].framenumber=fap;
	return (*frames)[fap].toString();
}

int FAPwriter::WriteToString(FAPFrameVector *frames, std::string **str)
{
	/*
	int framesPerSecond = 25;//2004-04-19 bjoern: debug interpolation problem
	int numFrames = (*frames).size();
	// OPEN OUTPUT FILE
	ostringstream outfile;
	
	string *res = new string(outfile.str());
	*str=res;
	*/
	return 0;
}
