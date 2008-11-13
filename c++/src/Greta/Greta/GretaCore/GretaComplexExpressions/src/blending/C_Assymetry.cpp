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

//  C-Assymetry.cpp implementation of the  C_Assymetry class.
//
//////////////////////////////////////////////////////////////////////

#include "C_Assymetry.h"
#include "./../math/C_FuzzySimilarity.h"

#include "../utils/CemGlobals.h"


C_Assymetry::C_Assymetry() 
{

	for (int i=0; i<CEM_MAX_FAP; i++)
	{
		directions [i]=0;
	}

	//6,7
	directions[6]=1;directions[7]=2;
	//8,9
	directions[8]=1;directions[9]=2;
	//10,11
	directions[10]=1;directions[11]=2;
	//12,13
	directions[12]=1;directions[13]=2;
	//19,20
	directions[19]=1;directions[20]=2;
	//21,22
	directions[21]=1;directions[22]=2;
	//31,32
	directions[31]=1;directions[32]=2;
	//33,34
	directions[33]=1;directions[34]=2;
	//35,36
	directions[35]=1;directions[36]=2;
	//37,38
	directions[37]=1;directions[38]=2;
	//39,40
	directions[39]=1;directions[40]=2;
	//41,42
	directions[41]=1;directions[42]=2;
	//53,54
	directions[53]=1;directions[54]=2;
	//55,56
	directions[55]=1;directions[56]=2;
	//57,58
	directions[57]=1;directions[58]=2;
	//59,60
	directions[59]=1;directions[60]=2;
	//61,62
	directions[61]=1;directions[62]=2;

	//onthelist = new double [32][CEM_MAX_FAP];

	for (unsigned int i=0; i<12; i++)
	{			
		for (unsigned int j=0; j<CEM_MAX_FAP; j++)
		{
			onthelist[i][j]=0;
		}
	}

	//UWAGA TO JEST ZLE KIERUNKI SA ZAMIENIENIONE!!!!!!!!!!!!!!!!!

	//anger 2

	onthelist[2][31]=(double)0.8;//left 
	//onthelist[2][32]=1;//right
	onthelist[2][33]=(double)0.8;//left 
	//onthelist[2][34]=1;//right        
	onthelist[2][35]=(double)0.8;//left 
	//onthelist[2][36]=1;//right
	onthelist[2][37]=(double)0.8;//left 
	//onthelist[2][38]=1;//right

	//smile 6
	onthelist[6][53]=(double)0.9;//left 
	onthelist[6][54]=(double)1.1;//right
	onthelist[6][59]=(double)0.9;//left 
	onthelist[6][60]=(double)1.1;//right

	//fear 8
	//onthelist[8][10]=1;//left
	onthelist[8][11]=(double)0.8;//right        
	//onthelist[8][53]=1;//left
	onthelist[8][54]=(double)0.8;//right        
	//onthelist[8][55]=1;//left
	onthelist[8][56]=(double)0.8;//right        
	//onthelist[8][57]=1;//left
	onthelist[8][58]=(double)0.8;//right        
	//onthelist[8][59]=1;//left
	onthelist[8][60]=(double)0.8;//right        

	//disgust 4
	//onthelist[4][61]=1;//left
	onthelist[4][62]=(double)0.7;//right        

	//surpise 12
	onthelist[12][0]=(double)0.8;//right        

	//sadness 10
	onthelist[10][0]=(double)0.8;//right        

}

/**
* 
* destructor
* 
*/

C_Assymetry::~C_Assymetry()
{
}


void C_Assymetry::update(int **expression, Emotion *e)
{
	std::string emotion_name;

	emotion_name=e->getName();

	C_FuzzySimilarity *fs1 = new C_FuzzySimilarity();

	double anger1=fs1->findSimilarity(emotion_name, "anger");
	double disgust1=fs1->findSimilarity(emotion_name, "disgust");
	double sadness1=fs1->findSimilarity(emotion_name, "sadness");
	double surprise1=fs1->findSimilarity(emotion_name, "surprise");
	double fear1=fs1->findSimilarity(emotion_name, "fear");
	double joy1=fs1->findSimilarity(emotion_name, "joy");
	double neutral1=0.0;

	double max1=MAX(anger1,disgust1);
	max1=MAX(max1,sadness1);
	max1=MAX(max1,fear1);
	max1=MAX(max1,joy1);	
	max1=MAX(max1,surprise1);


	if (anger1==max1) 
	{
		surprise1=disgust1=sadness1=fear1=joy1=neutral1=0.0;
		emotion_name="anger";
	}
	if (disgust1==max1) 
	{
		surprise1=anger1=sadness1=fear1=joy1=neutral1=0.0;
		emotion_name="disgust";
	}
	if (sadness1==max1) 
	{
		surprise1=disgust1=anger1=fear1=joy1=neutral1=0.0;
		emotion_name="sadness";
	}
	if (fear1==max1) 
	{
		surprise1=disgust1=sadness1=anger1=joy1=neutral1=0.0;
		emotion_name="fear";
	}
	if (joy1==max1) 
	{
		surprise1=disgust1=sadness1=anger1=fear1=neutral1=0.0;
		emotion_name="joy";
	}

	if (surprise1==max1) 
	{
		neutral1=disgust1=sadness1=anger1=joy1=fear1=0.0;
		emotion_name="surprise";
	}


	if (ASSYMETRY == 1)
	{            

		int id = 0;

		if (emotion_name.compare("joy")==0) id = 6;
		if (emotion_name.compare("anger")==0) id = 2;
		if (emotion_name.compare("fear")==0) id = 8;
		if (emotion_name.compare("disgust")==0) id = 4;

		for (int i=1;i<CEM_MAX_FAP;i++)
		{												
			//left                
			if ( (onthelist[id][i]!=(double)(0.0)) && (id!=0) ) 
				expression[1][i] = (int)((double)expression[1][i]*onthelist[id][i]);
			//right
			if ((onthelist[id][i]!=(double)(0.0)) && (id!=0) ) 
				expression[1][i] = (int)((double)expression[1][i]*onthelist[id][i]);
		}

	}//no assymetry                
}    
