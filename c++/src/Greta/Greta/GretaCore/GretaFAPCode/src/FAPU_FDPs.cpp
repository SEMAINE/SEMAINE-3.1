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
#include <string.h>
#include "FAPU_FDPs.h"

FDPdata::FDPdata()
{

}

FDPdata::~FDPdata()
{
	delete [] featurePoints;
}

bool FDPdata::ReadFaceFDPs(const char *path, const float **vtxData)
{
	/* Extract from greta_FDPs.txt: 
	#START
	GROUP 2 10
	GROUP 3 4
	GROUP 4 6
	GROUP 5 4
	GROUP 6 4
	GROUP 7 0
	GROUP 8 8
	GROUP 9 15
	GROUP 10 4
	END
	#GROUP 2
	0.000000 -23.450001 17.580000 #2147
	0.000000 -15.100000 19.400000 #1256
	0.000000 -15.040000 19.209999 #2103
	5.090000 -15.060000 16.969999 #11
	-5.090000 -15.060000 16.969999 #2386
	2.980000 -15.030000 18.330000 #1246
	-2.980000 -15.030000 18.330000 #3575
	2.730000 -14.980000 18.230000 #1754
	-2.730000 -14.980000 18.230000 #4058
	0.000000 -20.400000 18.500000 #2143
	#GROUP 3
	*/

	FILE* f;
	char cbuf[1024];
	int group, num, dimension, i, j, id, size;
	float f0, f1, f2;
	int ref;

	if(!(f=fopen(path,"r")))
	{
		printf("FDP ERROR: cant read %s\n", path);
		return false;
	}
	
	// find out the group size (number of FP's in each group)
	fgets(cbuf,1024,f);
	while(strcmp(cbuf,"#START\n"))
		fgets(cbuf,1024,f);
	
	for(i=2;i<=11;i++)
	{
		fgets(cbuf,1024,f);
		sscanf(cbuf, "GROUP %d %d", &id, &size);
		FDPGroupsSize[id]=size;
	}


	// allocate memory for the groups of FDPs
	featurePoints = (float***) new float** [11];


	for(i=2;i<=10;i++)
	{
		featurePoints[i]=(float**) new float* [FDPGroupsSize[i]+1];
		for(j=1;j<=FDPGroupsSize[i];j++)
			featurePoints[i][j]=(float*) new float[3];
	}
	while(strcmp(cbuf,"END\n")!=0)
		fgets(cbuf,1024,f);
	while(cbuf[0]=='#')
		fgets(cbuf,1024,f);

	// read in the FDPs for each group
	for(i=2;i<=10;i++)
	{
		fgets(cbuf,1024,f);
		for(j=1;j<=FDPGroupsSize[i];j++)
		{
			fgets(cbuf,1024,f);
			sscanf(cbuf, "%f %f %f #%i", &f0, &f1, &f2, &ref);
			if(ref==-1)
			{
				featurePoints[i][j][0]=f0;
				featurePoints[i][j][1]=f1;
				featurePoints[i][j][2]=f2;
			}
			else {
				featurePoints[i][j][0]=vtxData[ref][0];
				featurePoints[i][j][1]=vtxData[ref][1];
				featurePoints[i][j][2]=vtxData[ref][2];
			}
		}
	}
	fclose(f);
	return true;
}

FAPUdata::FAPUdata(void)
{

}

FAPUdata::~FAPUdata(void)
{

}

void FAPUdata::CalculateFAPUs(const FDPdata *fdps)
{
	// FAPUs

	// Mouth Width
	MWFAPU = fdps->featurePoints[8][3][0] - fdps->featurePoints[8][4][0];

	/// Mouth-Nose Separation
	MNSFAPU	= fdps->featurePoints[9][15][1] - fdps->featurePoints[2][2][1];

	/// Eyes-Nose Separation
		// On change legerement le calcul du FAPU ENS par rapport a la norme.
		// Au lieu de prendre la distance entre le bas du nez (9.15) et la droite formé par le centre des pupilles (3.5, 3.6)
		//	on prend la distance entre le bas du nez et la droite formée par les distances moyennée
		//	(3.2, 3.4) et (3.1, 3.3)
	ENSFAPU	=	(fdps->featurePoints[3][1][1] + fdps->featurePoints[3][3][1] + fdps->featurePoints[3][2][1] 
				+ fdps->featurePoints[3][4][1] ) / 4 - fdps->featurePoints[9][15][1];
	
	/// Eyes Separation
		// On change legerement le calcul du FAPU ES par rapport a la norme.
		// Au lieu de prendre la distance entre les 2 yeux (3.5, 3.6)
		//	on prend la distance moyennée entre:
		//	(3.1, 3.2) et (3.3, 3.4)
	ESFAPU = ((fdps->featurePoints[3][1][0] - fdps->featurePoints[3][2][0]) + 
				(fdps->featurePoints[3][3][0] - fdps->featurePoints[3][4][0])) / 2;
	
	// Iris Distance
	IRISDFAPU = fdps->featurePoints[3][1][1] - fdps->featurePoints[3][4][1];
	
	// IFAPUs
	IFAPU_MW = fdps->featurePoints[8][3][0] - fdps->featurePoints[8][4][0];
	IFAPU_MH = fdps->featurePoints[8][1][1] - fdps->featurePoints[8][2][1];
	IFAPU_JH = fdps->featurePoints[8][2][1] - fdps->featurePoints[2][1][1];
	IFAPU_FI = (fdps->featurePoints[4][1][2] + fdps->featurePoints[4][2][2]) / 2 - fdps->featurePoints[6][1][2];
	IFAPU_RCB =	fdps->featurePoints[8][4][2] - fdps->featurePoints[5][2][2];
	IFAPU_LCB = fdps->featurePoints[8][3][2] - fdps->featurePoints[5][1][2];
	IFAPU_ULP = fdps->featurePoints[8][1][2] - fdps->featurePoints[9][15][2];
	IFAPU_LLP = fdps->featurePoints[8][2][2] - fdps->featurePoints[2][10][2];

	AFAPU_UL = fdps->featurePoints[8][1][1] - fdps->featurePoints[2][2][1];
	AFAPU_LL = fdps->featurePoints[2][3][1] - fdps->featurePoints[8][2][1];

	// conversion from FAPUs
	MWFAPU		=	1024.0 / MWFAPU;
	MNSFAPU		=	1024.0 / MNSFAPU;
	ENSFAPU		=	1024.0 / ENSFAPU;
	ESFAPU		=	1024.0 / ESFAPU;
	IRISDFAPU	=	1024.0 / IRISDFAPU;

	// conversion to IFAPUs
	IFAPU_MW		=	1024.0 / IFAPU_MW;
	IFAPU_MH		=	1024.0 / IFAPU_MH;
	IFAPU_JH		=	1024.0 / IFAPU_JH;
	IFAPU_ULP		=	1024.0 / IFAPU_ULP;
	IFAPU_LLP		=	1024.0 / IFAPU_LLP;
	IFAPU_FI		=	1024.0 / IFAPU_FI;
	IFAPU_RCB		=	1024.0 / IFAPU_RCB;
	IFAPU_LCB		=	1024.0 / IFAPU_LCB;

	AFAPU_UL		=   1024.0 / AFAPU_UL;
	AFAPU_LL		=   1024.0 / AFAPU_LL;
}

void FAPUdata::print(void)
{
	printf("FAPUs and IFAPUs:\n");
	printf("    FAPU         MW   %f\n",MWFAPU);
	printf("    FAPU         MNS  %f\n",MNSFAPU);
	printf("    FAPU         ENS  %f\n",ENSFAPU);
	printf("    FAPU         ES   %f\n",ESFAPU);
	printf("    FAPU         IRIS %f\n",IRISDFAPU);
	printf("    IFAPU        LW   %f\n",IFAPU_MW);
	printf("    IFAPU        LH   %f\n",IFAPU_MH);
	printf("    IFAPU        JH   %f\n",IFAPU_JH);
	printf("    IFAPU        ULP  %f\n",IFAPU_ULP);
	printf("    IFAPU        LLP  %f\n",IFAPU_LLP);
	printf("    IFAPU        FI   %f\n",IFAPU_FI);
	printf("    IFAPU        JDR  %f\n",IFAPU_RCB);
	printf("    IFAPU        JDL  %f\n",IFAPU_LCB);

	printf("    AFAPU        UL  %f\n",AFAPU_UL);
	printf("    AFAPU        LL  %f\n",AFAPU_LL);
}