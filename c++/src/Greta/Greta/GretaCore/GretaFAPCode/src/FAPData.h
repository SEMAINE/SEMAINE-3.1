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


#ifndef FAPS_H
#define FAPS_H

#include <math.h>
#include <vector>
#include "FAPU_FDPs.h"
#include "FAPFrame.h"

//Animation of the model is described only by the FAPs
//Define the displacements of the feature points in relation to their positions in the neutral face
//FAPs usually encode magnitude of displacement of a feature point along one of the three Cartesian axes
//(except in cases where it encodes rotation of whole head, eyeball, etc)
//FAPs: 68 parameters categorised into 10 groups related to parts of the face
//Example: FAP 3 = open jaw, FDP: Grp 2, Subgrp 1 (2.1), pos motion: down, units: MNS, Unidir
//replaces FAPtype
//FAPs 48, 49, 50 are the head rotation
/**
* class :FAPdata
*
*/

class FAPdata
{
public:

	/**
	* contructor 
	*
	*/

	FAPdata();

	/**
	* destructor 
	*/

	~FAPdata();

	bool header_file_init(char *filename);	//fill in the numFrames, fps and version from a header file
	void allocate_FAP_data(void);	//allocate main structures
	void deallocate_FAP_data(void);	//deallocate main structures


	/**
	*  
	* 
	*
	* @return 
	* @param fapfilename
	*/

	bool ReadFapFile(char *fapfilename);

	/**
	*  
	* 
	*
	* @return 
	* @param buffer
	*/

	int ReadFapBuffer(char *buffer);

	/**
	*  
	* 
	*
	* @return 
	* @param  fapnum
	* @param FAPUs
	*/

	float CalibrateFaps(int fapnum, const FAPUdata *FAPUs);

	/**
	*  
	* 
	*
	* @param  frameind
	* @param FDPs
	* @param FAPUs
	*/

	void ScaleFAPs(int frameind, FDPdata *FDPs, const FAPUdata *FAPUs);

	/**
	*  
	* 
	*
	* @param  fapnum
	* @param  int frameind
	* @param FDPs
	* @param FAPUs
	*/

	void Fap2LFap(int fapnum, int frameind, FDPdata *FDPs, const FAPUdata *FAPUs);

	/**
	*  
	* 
	*
	* @param frame
	* @param FDPs
	* @param FAPUs
	*/

	void LoadFAPFrame(FAPFrame *frame, FDPdata *FDPs, const FAPUdata *FAPUs);



	/**
	*  
	* 
	*
	* @return 
	*/

	void print(void);

	FAPFrameVector frames;
	std::vector<float>lfaps;
	int numberofframes;
	int fps;
	char *basename;	/*[256]*/
	float version;
};


#endif
