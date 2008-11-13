#include "Mpeg4elements.h"
#include "stdlib.h"
#include "stdio.h"

SelectedFDP::SelectedFDP(int v0, int v1, int v2)
{
	this->vertices=(int*)malloc(3*sizeof(int));
	this->vertices[0]=v0;
	this->vertices[1]=v1;
	this->vertices[2]=v2;
	this->pointer=0;
}
SelectedFDP::~SelectedFDP()
{
}
int SelectedFDP::next()
{
	return (this->vertices[++this->pointer%3]);
}

// Size of group
int sizeOfGroup(int group)
{
	switch(group)
	{
		case GROUP_2:
			return GROUP_2_SIZE;
		case GROUP_3:
			return GROUP_3_SIZE;
		case GROUP_4:
			return GROUP_4_SIZE;
		case GROUP_5:
			return GROUP_5_SIZE;
		case GROUP_6:
			return GROUP_6_SIZE;
		case GROUP_7:
			return GROUP_7_SIZE;
		case GROUP_8:
			return GROUP_8_SIZE;
		case GROUP_9:
			return GROUP_9_SIZE;
		case GROUP_10:
			return GROUP_10_SIZE;
		case GROUP_11:
			return GROUP_11_SIZE;
		default:
			return 0;
	}
}

int groupOfRegion(int region)
{
	// Cette fonction relie la région définie
	//	au groupe auquel il appartient
	// Exemple:
	//	la joue droite (L_CHEECK) appartient au groupe "Cheeks" (GROUP_5)
	switch (region)
	{
		case L_CHEECK:
			return GROUP_5;
		case R_CHEECK:
		 	return GROUP_5;
		case L_NOSE:
			return GROUP_9;
		case R_NOSE:
			return GROUP_9;
		case L_INF_EYELID_A:
			return GROUP_3;
		case R_INF_EYELID_A:
			return GROUP_3;
		case L_INF_EYELID_B:
			return GROUP_3;
		case R_INF_EYELID_B:
			return GROUP_3;
		case L_SUP_EYELID_A:
			return GROUP_3;
		case R_SUP_EYELID_A:
			return GROUP_3;
		case L_SUP_EYELID_B:
			return GROUP_3;
		case R_SUP_EYELID_B:
			return GROUP_3;
		case L_EYEBROW_A:
			return GROUP_4;
		case R_EYEBROW_A:
			return GROUP_4;
		case L_EYEBROW_B:
			return GROUP_4;
		case R_EYEBROW_B:
			return GROUP_4;
		case L_EYEBROW_C:
			return GROUP_4;
		case R_EYEBROW_C:
			return GROUP_4;
		case L_INF_FOREHEAD_A:
			return GROUP_11;
		case R_INF_FOREHEAD_A:
			return GROUP_11;
		case L_INF_FOREHEAD_B:
			return GROUP_11;
		case R_INF_FOREHEAD_B:
			return GROUP_11;
		case L_INF_FOREHEAD_C:
			return GROUP_11;
		case R_INF_FOREHEAD_C:
			return GROUP_11;
		case L_INF_FOREHEAD_D:
			return GROUP_11;
		case R_INF_FOREHEAD_D:
			return GROUP_11;
		case L_INF_FOREHEAD_E:
			return GROUP_11;
		case R_INF_FOREHEAD_E:
			return GROUP_11;
		case L_INF_FOREHEAD_F:
			return GROUP_11;
		case R_INF_FOREHEAD_F:
			return GROUP_11;
		case L_SUP_FOREHEAD:
			return GROUP_11;
		case R_SUP_FOREHEAD:
			return GROUP_11;
		case L_SUP_LIP_A:
			return GROUP_2;
		case R_SUP_LIP_A:
			return GROUP_2;
		case L_SUP_LIP_B:
			return GROUP_2;
		case R_SUP_LIP_B:
			return GROUP_2;
		case L_SUP_LIP_C:
			return GROUP_2;
		case R_SUP_LIP_C:
			return GROUP_2;
		case L_INF_LIP_A:
			return GROUP_2;
		case R_INF_LIP_A:
			return GROUP_2;
		case L_INF_LIP_B:
			return GROUP_2;
		case R_INF_LIP_B:
			return GROUP_2;
		case L_INF_LIP_C:
			return GROUP_2;
		case R_INF_LIP_C:
			return GROUP_2;
		case L_SUP_MOUTH_A:
			return GROUP_8;
		case R_SUP_MOUTH_A:
			return GROUP_8;
		case L_SUP_MOUTH_B:
			return GROUP_8;
		case R_SUP_MOUTH_B:
			return GROUP_8;
		case L_SUP_MOUTH_C:
			return GROUP_8;
		case R_SUP_MOUTH_C:
			return GROUP_8;
		case L_MOUTH:
			return GROUP_8;
		case R_MOUTH:
			return GROUP_8;
		case L_INF_MOUTH_A:
			return GROUP_8;
		case R_INF_MOUTH_A:
			return GROUP_8;
		case L_INF_MOUTH_B:
			return GROUP_8;
		case R_INF_MOUTH_B:
			return GROUP_8;
		case L_INF_MOUTH_C:
			return GROUP_8;
		case R_INF_MOUTH_C:
			return GROUP_8;
		case L_CHIN:
			return GROUP_2;
		case R_CHIN:
			return GROUP_2;
		case L_EAR:
			return GROUP_10;
		case R_EAR:
			return GROUP_10;
		case L_SKIN_A:
			return GROUP_11;
		case R_SKIN_A:
			return GROUP_11;
		case L_SKIN_B:
			return GROUP_11;
		case R_SKIN_B:
			return GROUP_11;
		case L_SKIN_C:
			return GROUP_11;
		case R_SKIN_C:
			return GROUP_11;
		default:
			return -1;
	}
}

void init_fdps()
{
	// Allocation memoire de fdp_names
	int i, j;
	for(i=0;i<GROUP_NUMBER;i++)
		if( i==GROUP_2 || i==GROUP_3 || i==GROUP_4 || i==GROUP_5 || i==GROUP_6 || i==GROUP_7 || i==GROUP_8 || i==GROUP_9 || i==GROUP_10 || i==GROUP_11)
			fdp_names[i]=(char**)malloc((sizeOfGroup(i)+1)*sizeof(char*));
	for(i=0;i<GROUP_NUMBER;i++)
		for(j=1;j<=sizeOfGroup(i);j++)
			fdp_names[i][j]=(char*)malloc(MAX_LENGTH_OF_NAME*sizeof(char));
	// Initialisation de fdp_names
	for(i=0;i<GROUP_NUMBER;i++)
		for(j=1;j<=sizeOfGroup(i);j++)
			sprintf(fdp_names[i][j], "%i.%i", i, j);
}

// indice of a FDP
//	Convertit un couple (region, indice) en une valeur v: 0 <= v < FDP_NUMBER
int fdp_value(int group, int indice)
{
	// Condition d'arret
	if(group<0 || indice<0)
		return -1;
	// Declarations
	int v=0;
	// Initialisation
	for(int i=0;i<group;i++)
		v+=sizeOfGroup(i);
	return (v+indice);
}

// getFDPs
//	Retourne la liste des noms d'un certain groupe
char** getFDPs(int group)
{
	// Condition d'arret
	if(group<0)
		return NULL;
	// Declarations
	char** list;
	int size=sizeOfGroup(group);
	list=(char**)malloc((size+1)*sizeof(char*));
	// Initialisation
	for(int i=1;i<=size;i++)
		list[i]=fdp_names[group][i];
	return list;
}
// getName
char* getName(int fdp)
{
	// Condition d'arret
	if(fdp<0)
		return NULL;
	// Declarations
	int cpt=0;
	// Initialisation
	for(int i=0;i<GROUP_NUMBER;i++)
		if(cpt+sizeOfGroup(i)<=fdp)
			cpt+=sizeOfGroup(i);
		else
			return fdp_names[i][fdp-cpt+1];
	return NULL;
}
// nameOfFAP
char* nameOfFAP(int fap_value)
{
	// Condition d'arret
	if(fap_value<0)
		return NULL;
	// 
	return region_names[fap_value];
}