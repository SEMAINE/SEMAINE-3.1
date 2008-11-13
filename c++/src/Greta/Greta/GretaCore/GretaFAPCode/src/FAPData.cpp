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


#include <stdio.h>	    	// Header File For Standard Input/Output
#include <stdlib.h>
#include "FAPData.h"

FAPdata::FAPdata()
{
	numberofframes = 0;
	fps = 0;
	basename = ((char *) new char[256]);
	version = 0;
	lfaps.resize(69);
}

void FAPdata::allocate_FAP_data(void)
{
	frames.resize(numberofframes);
	lfaps.resize(69);
}

FAPdata::~FAPdata()
{
	deallocate_FAP_data();
}

void FAPdata::deallocate_FAP_data(void)
{

	if (basename!=NULL)
		delete [] basename;

	frames.clear();
	lfaps.clear();
	basename=NULL;

	version = 0;
	numberofframes=0;
	fps=0;
}

bool FAPdata::header_file_init(char *filename)
{
	//reads in and sets numberOfFrames, version and fps rate
	char cbuf[256],cdummy[256];
	
	int numberOfFramesTemp = 0;
	int fpsTemp = 0;
	float versionTemp = 0.0;

	FILE *fapfile = NULL;
	if(!(fapfile = fopen(&filename[0],"r"))){
		printf("WARNING: can't read %s\n", filename);
		return false;
	}
	while(1) {
		if(fscanf(fapfile,"%s",cbuf));
		else {
		  printf("ERROR: bad fapfile: %s\n",cbuf);
		  return false;
		}
		if(cbuf[0] == '#') while(getc(fapfile) != '\n');
		else {
		  sscanf(cbuf,"%f ",&versionTemp);
		if(fscanf(fapfile,"%s %d %d",cdummy,&fpsTemp,&numberOfFramesTemp));
		else {
		  printf("ERROR: bad fapfile: %s\n",cbuf);
		  return false;
		}
		if(fabs(versionTemp - 2.1) > 0.001) printf("WARNING: Expecting FAP file version 2.1 not %f\n",versionTemp);
		while(getc(fapfile) != '\n');
		break;
		}
	}
	//set the data in the FAP
	numberofframes = numberOfFramesTemp;
	fps = fpsTemp;
	version = versionTemp;

	fclose(fapfile);
	return true;
}

//To read the specific FAP file
bool FAPdata::ReadFapFile(char *fapfilename)
{
  FILE *fapfile;
  char cbuf[1000];
  int fapnum,frameind;
  int fap1[8000][4];	//buffer for special reads: not used?
  int fap2[8000][6];	//buffer for special reads: not used?

  if(!(fapfile = fopen(fapfilename,"r"))){
	  printf("WARNING: can't read %s\n",fapfilename);
	  return false;
  }

  //  version basename fps numberofframes
  //  2.1 C:/code/Greta/runtime/output\french 25 95
  
  while(1) {//read header line (again)
    if(!(fscanf(fapfile,"%s",cbuf))){
      printf("ERROR: bad fapfile: %s\n",cbuf);
      return false;
    }

    if(cbuf[0] == '#') 
		while(getc(fapfile) != '\n');
    else {
      sscanf(cbuf,"%f ",&version);
      fscanf(fapfile,"%s %d %d", basename, &fps, &numberofframes);
      if(fabs(version - 2.1) > 0.001)
	  printf("WARNING: Expecting FAP file version 2.1 not %f\n",version);
      while(getc(fapfile) != '\n');
      break;
    }
  }

  //FIXME: this should be done after numberofframes is set, but basename depends on it!
  allocate_FAP_data();

  //for each FAP frame
  for(frameind = 0; frameind < numberofframes; frameind++) {
   
	//(1) read in FAP mask line
    while(1) {
      if(fscanf(fapfile,"%s",cbuf)!= EOF);
      else {
	      printf("ERROR: bad fapfile: %s\n",cbuf);
	      return false;
      }
      if(cbuf[0] == '#') while(getc(fapfile) != '\n');
      else {
	      sscanf(cbuf,"%d",&frames[frameind].FAPs[1].active);
	      for(fapnum=2;fapnum<69;fapnum++)
		      fscanf(fapfile,"%d",&frames[frameind].FAPs[fapnum].active);
	      while(getc(fapfile) != '\n');
	      break;
      }
    }

    //(2) read in FAP value
    while(1) {
      if(fscanf(fapfile,"%s",cbuf)!= EOF);
      else {
	      printf("ERROR: bad fapfile: %s\n",cbuf);
	      return false;
      }
      if(cbuf[0] == '#') while(getc(fapfile) != '\n');
      else {
	      sscanf(cbuf,"%d",&frames[frameind].FAPs[0].value);
	      for(fapnum=1;fapnum<69;fapnum++) 
          {
          //only read in the FAPs if the corresponding mask bit is set to 1
          //if the first or second FAPs are enabled, do special reads
          if(frames[frameind].FAPs[fapnum].active == 1) 
          {
	          if(fapnum == 1) {
	            fscanf(fapfile,"%d %d %d %d",&fap1[frameind][0],&fap1[frameind][1],&fap1[frameind][2],&fap1[frameind][3]);
	          }
	          else if(fapnum == 2) {
	            fscanf(fapfile,"%d %d %d %d %d %d",&fap2[frameind][0],&fap2[frameind][1],&fap2[frameind][2],&fap2[frameind][3],&fap2[frameind][4],&fap2[frameind][5]);
	          }
			  //otherwise, just read in a single value
	          else fscanf(fapfile,"%d",&frames[frameind].FAPs[fapnum].value);
	        }
        }
	    while(getc(fapfile) != '\n');
	    break;
      }
    }

  }

  fclose(fapfile);
  return true;
}

//ADDME: make both functions share code
//To read the specific FAP file
int FAPdata::ReadFapBuffer(char *buffer)
{
	int fapnum,frameind,nof,fps2;
	//  int **isfeatpnt;
	char cdummy[256];

	//  isfeatpnt = FDPs->isfeatpnt;
	
	if(buffer==0){
		printf("WARNING: can't read from fap buffer\n");
		return 0;
	}
  
	sscanf(buffer,"%f %s %d %d", &version, cdummy, &fps2, &nof);

	if(fabs(version - 2.1) > 0.001)
		printf("WARNING: Expecting FAP file version 2.1 not %f\n",version);
		
	deallocate_FAP_data();

	numberofframes=nof;
	fps=fps2;

	allocate_FAP_data();

	while((buffer[0]!='\n')&&(buffer[0]!=0))
		buffer++;
	if(buffer!=0)
		buffer++;

	for(frameind=0;frameind<numberofframes;frameind++)
	{
		//if(frameind%100==0)
		//	printf("reading frame %d\n",frameind);
		//frames[frameind].ReadFromBuffer(buffer);
		
		for(fapnum=1;fapnum<NUMBER_OF_FAPS;fapnum++)
		{
			sscanf(buffer,"%d",&frames[frameind].FAPs[fapnum].active);
			while((buffer[0]!=' ')&&(buffer[0]!=0))
				buffer++;
			if(buffer!=0)
				buffer++;
			else
				break;
		}

		while((buffer[0]!='\n')&&(buffer[0]!=0))
			buffer++;
		buffer++;
		
		for(fapnum=0;fapnum<NUMBER_OF_FAPS;fapnum++) 
		{
			if(fapnum==0)
			{
				sscanf(buffer,"%d",&frames[frameind].FAPs[0].value);
				while((buffer[0]!=' ')&&(buffer[0]!=0))
					buffer++;
				if(buffer!=0)
					buffer++;
				else
					break;
			}
			else
			{
				if(frames[frameind].FAPs[fapnum].active)
				{
					sscanf(buffer,"%d",&frames[frameind].FAPs[fapnum].value);
					while((buffer[0]!=' ')&&(buffer[0]!=0))
						buffer++;
					if(buffer!=0)
						buffer++;
					else
						break;
				}
			}
		}

		while((buffer[0]!='\n')&&(buffer[0]!=0))
			buffer++;
		buffer++;
		
	}
	return true;
}

float FAPdata::CalibrateFaps(int fapnum, const FAPUdata *FAPUs)
{
	switch(fapnum)
	{
		case 4: case 51: //ULH
			return ((1*FAPUs->AFAPU_UL)/GRETA_UL);
			break;
		case 8: case 9: case 55: case 56: //ULH
			return ((1*FAPUs->AFAPU_UL)/GRETA_UL);
			break;
		case 5: case 52:  //LLH
			return ((1*FAPUs->AFAPU_LL)/GRETA_LL);
			break;
		case 57: case 58: case 10: case 11:
			return ((1*FAPUs->AFAPU_LL)/GRETA_LL);
			break;
		case 3: case  18: //JH
			return ((FAPUs->IFAPU_JH)/GRETA_JH);
			break;
		case 53: case 54: case 59: case 60: //LW
			return (FAPUs->IFAPU_MW/GRETA_LW);
			break;
		case 16: //ULP
			return (1.0); //(facedata->IFAPU_ULP/GRETA_ULP);
			break;
		case 17: //LLP
			return (1.0); //(facedata->IFAPU_LLP/(GRETA_LLP*(-1)));
			break;
		case 6: case 7: case 12: case 13: //Corner up and down
			return (FAPUs->MNSFAPU/GRETA_MNS);
			break;
		default: 
			return(1.0); 
			break;
	}
	return(1.0);
}

void FAPdata::print(void)
{
	printf("\nFAP details");
	printf("\nName: %s Frames: %d, fps: %d, version: %f", basename, numberofframes, fps, version);
}

void FAPdata::ScaleFAPs(int frameind, FDPdata *FDPs, const FAPUdata *FAPUs)
{
	int fapnum;

	for(fapnum=0;fapnum<69;fapnum++) 
		Fap2LFap(fapnum, frameind, FDPs, FAPUs);
}

void FAPdata::LoadFAPFrame(FAPFrame *frame, FDPdata *FDPs, const FAPUdata *FAPUs)
{
	frames[0] = *frame;
	ScaleFAPs(0, FDPs, FAPUs);
}

void FAPdata::Fap2LFap(int fapnum, int frameind, FDPdata *FDPs, const FAPUdata *FAPUs)
{
//	int **isfeatpnt;
//	isfeatpnt = FDPs->isfeatpnt;

	float ***FDPfp = FDPs->featurePoints;

	//First check the pupils, because they have a non zero LFAP when not active
	if (fapnum>=29 && fapnum<31)
	{
		if(frames[frameind].FAPs[fapnum].active) 
			lfaps[fapnum] = (((float)frames[frameind].FAPs[fapnum].value)/FAPUs->IRISDFAPU)+(fabs(FDPfp[3][2][1] - FDPfp[3][4][1])/3.0);
		else 
			lfaps[fapnum] = fabs(FDPfp[3][2][1] - FDPfp[3][4][1])/3.0;//set pupil diameter to 1/3rd iris diameter;
	}
	//Otherwise, skip if the FAP is not active
	else if(!frames[frameind].FAPs[fapnum].active)
	{
		lfaps[fapnum] = 0;
		return;
	}
	
	//Set the LFAP and divide it by the right FAPU value

	if ((fapnum>=3 && fapnum<19) || fapnum==44 || fapnum==46 || (fapnum>=51 && fapnum<53) || (fapnum>=55 && fapnum<61))
		lfaps[fapnum] = ((float)frames[frameind].FAPs[fapnum].value)/FAPUs->MNSFAPU;
	else if (fapnum>=19 && fapnum<23)
		lfaps[fapnum] = ((float)frames[frameind].FAPs[fapnum].value)/FAPUs->IRISDFAPU;
	else if (fapnum>=23 && fapnum<27)
		lfaps[fapnum] = ((float)frames[frameind].FAPs[fapnum].value)/100000.0;/*convert to radians*/
	else if ((fapnum>=27 && fapnum<29) || (fapnum>=31 && fapnum<43))
		lfaps[fapnum] = ((float)frames[frameind].FAPs[fapnum].value)/FAPUs->ENSFAPU;
	else if (fapnum==43 || fapnum==45 || (fapnum>=53 && fapnum<55) || (fapnum>=61 && fapnum<69))
		lfaps[fapnum] = ((float)frames[frameind].FAPs[fapnum].value)/FAPUs->MWFAPU;
	else if (fapnum>=47 && fapnum<51)
		lfaps[fapnum] = ((float)frames[frameind].FAPs[fapnum].value)/100000.0;/*convert to radians*/
	else //We don't handle this FAP so return
	{
		lfaps[fapnum] = 0;
		return;
	}

	//Finally, multiply the LFAP value by the correstponding callibration value
	lfaps[fapnum] *= CalibrateFaps(fapnum, FAPUs);

//left_right_interpolation(FAPs,frameind, 6, 12);/*inner lips*/
//left_right_interpolation(FAPs,frameind, 53, 61);/*outer lips,nose*/
//left_right_interpolation(FAPs,frameind, 19, 41);/*eyelids,eyebrows,eyeballs,pupils,cheeks*/
//left_right_interpolation(FAPs,frameind, 65, 67);/*ears*/
 
}

