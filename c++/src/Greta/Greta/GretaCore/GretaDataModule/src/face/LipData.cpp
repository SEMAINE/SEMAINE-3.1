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

// Phonemes.cpp: implementation of the Phonemes class.
//
//////////////////////////////////////////////////////////////////////

#include "LipData.h"
#include <math.h>

extern IniManager inimanager;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern FILE *data_log;


LipData::LipData()
{

}

LipData::~LipData()
{

}


int LipData::ReadPhonemes()
{
	LoadConsonantTarget();
	LoadVowelTarget();
	LoadTongueTarget();

	//LoadRealDataEmotion();

	return(0);
}



// **************************	GET_VOWEL_TARGET  *******************************
//  Load data about vowels stored into the files .txt in the folder called coart
// ******************************************************************************
int LipData::LoadVowelTarget() 
{
	FILE *fid;
	int i, j, k;

	char vow[5];
	char cons[5];
	float duration;
	char FileName[256];
	char path[50];
	std::string emotion;

//	XMLGenericTree* PhoData;

	/*phonemeParser=new XMLGenericParser();
	consonantTree=phonemeParser->ParseFile("coart/phoneme.xml");
	delete phonemeParser;
	PhoData=trovaTarget(consonantTree,"b","LW");
	if (PhoData!=0)
	{
		std::list<XMLGenericTree*>::iterator iter;
		for(iter=PhoData->child.begin();iter!=PhoData->child.end();iter++)
			printf("time= %s	target=%s\n", ((*iter)->GetAttribute("time")).c_str(), ((*iter)->GetAttribute("target")).c_str());
	}
*/
	for(k=0;k<NUM_EMOT;k++) {
		

		strcpy(path, "coart/");

		switch(k) {
		case 0: strcat(path, "Neutral/");
				emotion="neutral";	break;
		case 1: strcat(path, "Joy/");
				emotion="joy";		break;
		case 2: strcat(path, "Surprise/");
				emotion="surprise";	break;
		case 3: strcat(path, "Fear/");	
				emotion="fear";		break;
		case 4: strcat(path, "Anger/");
				emotion="anger";	break;
		case 5: strcat(path, "Disgust/");
				emotion="disgust";	break;
		case 6: strcat(path, "Sadness/");	
				emotion="sadness";	break;
		default: break;

		}

		for(i=0; i<NUM_VOW; i++) {
			
			strcpy(FileName, path);
			
			
			switch(i) {
			case 0: strcat(FileName, "a_stressed.txt");
					strcpy(vow, "a1");	break;
			case 1: strcat(FileName, "a_not_stressed.txt");
					strcpy(vow, "a");	break;
			case 2: strcat(FileName, "e_stressed.txt"); 
					strcpy(vow, "e1");	break;
			case 3: strcat(FileName, "e_not_stressed.txt"); 
					strcpy(vow, "e");	break;
			case 4: strcat(FileName, "E1_stressed.txt"); 
					strcpy(vow, "E1");	break;
			case 5: strcat(FileName, "i_stressed.txt"); 
					strcpy(vow, "i1");	break;
			case 6: strcat(FileName, "i_not_stressed.txt"); 
					strcpy(vow, "i");	break;
			case 7: strcat(FileName, "o_stressed.txt"); 

					strcpy(vow, "o1");	break;
			case 8: strcat(FileName, "o_not_stressed.txt"); 
					strcpy(vow, "o");	break;
			case 9: strcat(FileName, "O1_stressed.txt"); 
					strcpy(vow, "O1");	break;
			case 10:strcat(FileName, "u_stressed.txt"); 
					strcpy(vow, "u1");	break;
			case 11:strcat(FileName, "u_not_stressed.txt"); 
					strcpy(vow, "u");	break;
			case 12:strcat(FileName, "y.txt"); 
					strcpy(vow, "y");	break;
			default:break;
			}

			if ((fid = fopen(FileName, "r"))!=NULL)
			{
				while (fscanf(fid, "%s", cons) != EOF) 
				{	
					fscanf(fid, "%f", &duration);
					Vowel newVowel;	
					newVowel.vowel=(std::string)vow;
					newVowel.emotion=emotion;
					newVowel.close_consonant=(std::string)cons;
					newVowel.duration=duration;

					for (j=0; j<LIP_PAR; j++) {
						fscanf(fid, "%f", &(newVowel.time[j][1]));
						fscanf(fid, "%f", &(newVowel.target[j][1]));
						fscanf(fid, "%f", &(newVowel.target[j][2]));
						newVowel.time[j][0] = (newVowel.time[j][1]/2);
						newVowel.time[j][2] = ((newVowel.time[j][1]+newVowel.duration)/2);
		
						newVowel.target[j][0]=newVowel.target[j][2]; 
					}
					Vowels.push_back(newVowel);
				}
				fclose(fid);
			}
			else
				return(0);
		}
	}
	return(1);
}


// *******************************	GET_CONSONANT_TARGET  **************************
//	Load data about consonants stored into the file .txt in the folder called coart

// *********************************************************************************
int LipData::LoadConsonantTarget()
{

	FILE *fid;
	int j, k;
	char FileName[256];
	char path[50];
	char cons[5];
	char vow[5];
	float duration;
	std::string emotion;

	for(k=0;k<NUM_EMOT;k++) {
		
		strcpy(path, "coart/");

		switch(k) {
		case 0: strcat(path, "Neutral/");
				emotion="neutral";	break;
		case 1: strcat(path, "Joy/");
				emotion="joy";		break;
		case 2: strcat(path, "Surprise/");
				emotion="surprise";	break;
		case 3: strcat(path, "Fear/");	
				emotion="fear";		break;
		case 4: strcat(path, "Anger/");
				emotion="anger";	break;
		case 5: strcat(path, "Disgust/");
				emotion="disgust";	break;
		case 6: strcat(path, "Sadness/");	
				emotion="sadness";	break;
		default: break;
		}

		strcpy(FileName, path);
		strcat(FileName, "consonants.txt");

		if((fid = fopen(FileName, "r"))!=NULL)
		{
			while (fscanf(fid, "%s", cons) != EOF) 
			{
				fscanf(fid, "%s", vow);
				fscanf(fid, "%f", &duration);
			
				Consonant newConsonant;
				newConsonant.consonant=(std::string)cons;
				newConsonant.emotion=emotion;
				newConsonant.close_vowel=(std::string)vow;
				newConsonant.duration=duration;
			
				for (j=0; j<LIP_PAR; j++) {
					fscanf(fid, "%f", &(newConsonant.time[j]));
					fscanf(fid, "%f", &(newConsonant.target[j]));
				}

				Consonants.push_back(newConsonant);	
			}
			fclose(fid);
		}
		else
			return(0);
	}
	return(1);
}

// *******************************	GET_TONGUE_TARGET  *************************
//	Load data about tongue stored into the file .txt in the folder called coart
// *****************************************************************************
int LipData::LoadTongueTarget()
{
	FILE *fid;
	int i, k=0;
	char pho[5];

	if ((fid = fopen("coart/vow_tongue.txt", "r"))!=NULL)
	{
		while (fscanf(fid, "%s", pho) != EOF) 
		{
			Tongue newTongueVow;
			newTongueVow.phoneme=(std::string)pho;

			for (i=0;i<TONGUE_PAR;i++) 
			{
				fscanf(fid, "%f", &newTongueVow.target[i][0]);
				fscanf(fid, "%f", &newTongueVow.target[i][1]);
			}
			Tongues.push_back(newTongueVow);	
		}
		fclose(fid);
	}
	else
		return(0);


	if((fid = fopen("coart/cons_tongue.txt", "r"))!=NULL)
	{
		while (fscanf(fid, "%s", pho) != EOF) {
			Tongue newTongueCons;
			newTongueCons.phoneme=(std::string)pho;

			for (i=0;i<TONGUE_PAR;i++) {

				fscanf(fid, "%f", &newTongueCons.target[i][0]);
				newTongueCons.target[i][1]=0;
			}
			Tongues.push_back(newTongueCons);	
		}
		fclose(fid);
	}
	else
		return(0);

	return(1);
}


// ************************************	GET_EMOTION  ****************************
//	Load data about emotion stored into the file .txt in the folder called coart
// ******************************************************************************
int LipData::LoadRealDataEmotion() 
{
	FILE *fid;
	int i, k;
	char emotion[256];

	if((fid = fopen("coart/emotion.txt", "r"))!=NULL)
	{
		while (fscanf(fid, "%s", emotion) != EOF) 
		{
			MixEmotion newEmotion;
			newEmotion.emotion=(std::string)emotion;

			fscanf(fid, "%f", &newEmotion.tension);
			fscanf(fid, "%f", &newEmotion.artic);
			fscanf(fid, "%d", &newEmotion.AsSinistra);
			fscanf(fid, "%d", &newEmotion.AsDestra);
			for (i=0;i<NUM_EMOT;i++) 
				for (k=0;k<LIP_PAR;k++)
					fscanf(fid, "%f", &newEmotion.EmotPar[i][k]);
			MixEmotions.push_back(newEmotion);
		}
		fclose(fid);
	}
	else
		return(0);

	return(1);
}

std::vector<Vowel>::iterator LipData::FindVowel(std::string vowel, std::string close_consonant, std::string emotion)
{
	std::vector<Vowel>::iterator itervowels;
	for(itervowels=Vowels.begin();itervowels!=Vowels.end();itervowels++)
	{
		if(((*itervowels).vowel==vowel) && ((*itervowels).close_consonant==close_consonant) && ((*itervowels).emotion==emotion))
			return itervowels;
	}
}

std::vector<Consonant>::iterator LipData::FindConsonant(std::string consonant, std::string close_vowel, std::string emotion)
{
	std::vector<Consonant>::iterator iterconsonants;

	for(iterconsonants=Consonants.begin();iterconsonants!=Consonants.end();iterconsonants++)
	{
		if(((*iterconsonants).consonant==consonant) && ((*iterconsonants).close_vowel==close_vowel) && ((*iterconsonants).emotion==emotion))
			return iterconsonants;
	}
}

std::vector<Tongue>::iterator LipData::FindTongue(std::string phoneme)
{
	std::vector<Tongue>::iterator itertongues;

	for(itertongues=Tongues.begin();itertongues!=Tongues.end();itertongues++)
	{
		if((*itertongues).phoneme==phoneme)
			return itertongues;
	}
}

std::vector<MixEmotion>::iterator LipData::FindEmotion(std::string emotion)
{
	std::vector<MixEmotion>::iterator itermixemotion;

	for(itermixemotion=MixEmotions.begin();itermixemotion!=MixEmotions.end();itermixemotion++)
	{
		if((*itermixemotion).emotion==emotion)
			return itermixemotion;
	}
}

float LipData::direction(float x1, float y1, float x2, float y2, float x)
{
	float m, c;

	m = (y2-y1)/(x2-x1);
	c = y1 - m*x1;

	return(m*x+c);
}

void LipData::PrintVowels()
{
	std::vector<Vowel>::iterator itervowels;
	for(itervowels=Vowels.begin();itervowels!=Vowels.end();itervowels++)
	{
		int j;
		printf("%s   %s\n%f\n", (*itervowels).vowel.c_str(), (*itervowels).close_consonant.c_str(), (*itervowels).duration);
		for (j=0; j<LIP_PAR; j++) 
			printf("%.2f   %.2f  %.2f\n", (*itervowels).time[j][1], (*itervowels).target[j][1], (*itervowels).target[j][2]);
	}
}

void LipData::PrintConsonants()
{
	std::vector<Consonant>::iterator iterconsonants;

	for(iterconsonants=Consonants.begin();iterconsonants!=Consonants.end();iterconsonants++)
	{
		int j;
		printf("%s   %s\n%f\n", (*iterconsonants).consonant.c_str(), (*iterconsonants).close_vowel.c_str(), (*iterconsonants).duration);
		for (j=0; j<LIP_PAR; j++) 
			printf("%.2f   %.2f\n", (*iterconsonants).time[j], (*iterconsonants).target[j]);
	}
}

void LipData::PrintTongue()
{
	std::vector<Tongue>::iterator itertongue;

	for(itertongue=Tongues.begin();itertongue!=Tongues.end();itertongue++)
	{
		int j;
		printf("%s \n", (*itertongue).phoneme.c_str());
		for (j=0; j<TONGUE_PAR; j++) 
			printf("%.2f   %.2f\n", (*itertongue).target[j][0], (*itertongue).target[j][1]);
	}
}

void LipData::PrintEmotion()
{
	std::vector<MixEmotion>::iterator itermixemotion;

	for(itermixemotion=MixEmotions.begin();itermixemotion!=MixEmotions.end();itermixemotion++)
	{
		int j, i;
		printf("%s \n", (*itermixemotion).emotion.c_str());
		printf("%.2f \n", (*itermixemotion).tension);
		printf("%.2f \n", (*itermixemotion).artic);
		printf("%d\n", (*itermixemotion).AsDestra);
		printf("%d\n", (*itermixemotion).AsSinistra);
		for (j=0; j<NUM_EMOT; j++)
		{
			for (i=0; i<LIP_PAR; i++) 
				printf("%.2f   %.2f\n", (*itermixemotion).EmotPar[j][i]);
			printf("\n");
		}
	}
}