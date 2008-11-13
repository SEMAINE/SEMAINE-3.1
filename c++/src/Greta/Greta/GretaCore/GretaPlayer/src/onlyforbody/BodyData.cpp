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

// BodyData.cpp: implementation of the BodyData class.
//
//////////////////////////////////////////////////////////////////////

#include "BodyData.h"
#include "XMLGenericParser.h"
#include "IniManager.h"

extern IniManager inimanager;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BodyData::BodyData(BAPConverter *conv)
{
	for(int i=0; i<NUMJOINTS; i++)
	{
		bodyvertexgroups[i]=NULL;
		pJointBVGs[i]=NULL;
	}
	this->conv=conv;
}

BodyData::~BodyData()
{
	delete[] vertices;
	delete[] normals;
	delete[] vertices2;
	delete[] normals2;
	delete[] indices;	

}

bool BodyData::ReadBodyGeometry()
{
	char cbuf[1024];
	float n1,n2,n3;
	int num,i,count;

	FILE *geofile;

	printf("reading body geometry");

	if(!(geofile = fopen((inimanager.Program_Path+"characters/"+this->meshfilename).c_str(),"r")))
	{
		printf("ERROR: can't read %s\n",this->meshfilename.c_str());
		return false;
	}

	//reads the textures file name

	if(fgets( cbuf, 256, geofile ));
	else
	{
		printf("ERROR: bad geometry file: %s\n",cbuf);
		return false;
	}
	if(cbuf[0] == '#') while(getc(geofile) != '\n');
	else
	{
		char textureimage[256];
		sscanf(cbuf,"%s",&textureimage);
	}

	printf(".");
	
	//reads the number of verts,edges,faces
	
	sprintf(cbuf,"");
	while(strcmp(cbuf,"VALUES\n")!=0)
		fgets( cbuf, 256, geofile );
	
	if(fgets( cbuf, 256, geofile ));
	else
	{
		printf("ERROR: bad geometry file: %s\n",cbuf);
		return false;
	}
	if(cbuf[0] == '#') while(getc(geofile) != '\n');
	else
	{
		sscanf(cbuf,"%d %d %d",&num_v,&num_e,&num_f);
	}

	printf(".");

	//initializes the arrays of verts and normals

	vertices=new GLfloat[num_v*3];
	normals=new GLfloat[num_v*3];
	vertices2=new GLfloat[num_v*3];
	normals2=new GLfloat[num_v*3];
	bodyUV=new GLfloat[num_v*2];
	indices=new GLint[num_v];
	
	for(int z=0;z<num_v*3;z++) {
		vertices[z]=vertices2[z]=normals[z]=normals2[z]=0.0f;
	}

	//reads verts coords and stores them in the vertices array
	
	sprintf(cbuf,"");
	while(strcmp(cbuf,"VERTICES\n")!=0)
		fgets( cbuf, 256, geofile );

	count=0;

	for(i=0; i<(num_f*3); i++)
	{
		if(fgets( cbuf, 256, geofile ));
		else
		{
			printf("ERROR: bad geometry file: %s\n",cbuf);
			return false;
		}
		if(cbuf[0] == '#') while(getc(geofile) != '\n');
		else
		{
			sscanf(cbuf,"%f %f %f",&n1,&n2,&n3);
			vertices[count]=n1;
			vertices[count+1]=n2;
			vertices[count+2]=n3;
			count=count+3;
		}
	}

	printf(".");
	
	//reads verts groups and stores each group in a BodyVertexGroup object
	//later this objects will be used to perform body mesh deformation
	//before ing body

	sprintf(cbuf,"");
	while(strcmp(cbuf,"GROUPS\n")!=0)
		fgets( cbuf, 256, geofile );

	int group=0;

	sprintf(cbuf,"");
	while(strcmp(cbuf,"NORMALS\n")!=0)
	{
		if(bodyvertexgroups[group]==NULL)
			bodyvertexgroups[group]=new BodyVertexGroup();
		if(strcspn(cbuf,"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRTSTUWXYZ")!=strlen(cbuf))
			if(strstr(cbuf,"Size")!=NULL)
			{
				cbuf[strlen(cbuf)-1]=NULL;
				char* check;
				check=strstr(cbuf,"=");
				check=check+2*sizeof(char);
				if(atoi(check)!=bodyvertexgroups[group]->size)
				{
					printf("BODY GEOMETRY FILE: checksum error on skin groups!\n");
					exit(2);
				}
				bodyvertexgroups[group]->StoreArray();
				group++;
			}
			else
			{
				strncpy(bodyvertexgroups[group]->name,cbuf,strlen(cbuf)-1);
				bodyvertexgroups[group]->name[strlen(cbuf)-5]=NULL;
			}
		else
		{
			char aux[100];
			sprintf(aux,"");
			int auxp=0;
			for(unsigned int j=0; j<strlen(cbuf); j++)
			{
				if((cbuf[j]!=' ')&&(cbuf[j]!=10))
				{
					aux[auxp]=cbuf[j];
					auxp++;
					aux[auxp]=NULL;
				}
				else
				{
					if(strcmp(aux,"")!=0)
					{
						bodyvertexgroups[group]->AddVertex(atoi(aux));
						sprintf(aux,"");
						auxp=0;
					}
				}
			}
		}
		if(fgets( cbuf, 256, geofile ));
			else
			{
				printf("ERROR: bad geometry file: %s\n",cbuf);
				return false;
			}
	}

	printf(".");

	//reads norms coords and stores them in the normals array

	count=0;

	for(i=0; i<(num_f*3); i++)
	{
		if(fgets( cbuf, 256, geofile ));
		else
		{
			printf("ERROR: bad geometry file: %s\n",cbuf);
			return false;
		}
		if(cbuf[0] == '#') while(getc(geofile) != '\n');
		else
		{
			sscanf(cbuf,"%f %f %f",&n1,&n2,&n3);
			normals[count]=n1;
			normals[count+1]=n2;
			normals[count+2]=n3;
			count=count+3;
		}
	}

	printf(".");

	sprintf(cbuf,"");
	while(strcmp(cbuf,"INDICES\n")!=0)
		fgets( cbuf, 256, geofile );


	//reads indexes and stores them in the indices array

	count=0;

	for(i=0; i<(num_f*3); i++)
	{
		if(fscanf( geofile,"%s", cbuf));
		else
		{
			printf("ERROR: bad geometry file: %s\n",cbuf);
			return false;
		}
		if(cbuf[0] == '#') while(getc(geofile) != '\n');
		else
		{
			sscanf(cbuf,"%d",&num);
			indices[count]=num;
			count=count+1;
		}
	}

	printf(". ");

	
	sprintf(cbuf,"");
	while(strcmp(cbuf,"UV\n")!=0)
		fgets( cbuf, 256, geofile );

	//reads UV texturing coords and stores them in the UV array

	count=0;

	for(i=0; i<(num_f*3); i++)
	{
		if(fgets( cbuf, 256, geofile ));
		else
		{
			printf("ERROR: bad geometry file: %s\n",cbuf);
			return false;
		}
		if(cbuf[0] == '#') while(getc(geofile) != '\n');
		else
		{
			sscanf(cbuf,"%f %f",&n1,&n2);
			bodyUV[count]=n1;
			bodyUV[count+1]=n2;
			count=count+2;
		}
	}

  

	fclose(geofile);

	printf("done\n");

	for(i=0; i<NUMJOINTS; i++)
	{
		if(bodyvertexgroups[i]!=NULL)
		{
			pJointBVGs[conv->JointStringToJointType(string(bodyvertexgroups[i]->name))]=bodyvertexgroups[i];
		}
	}

	return true;
}

int BodyData::GetFileNames(std::string path)
{
	XMLGenericParser parser;
	XMLGenericTree *partstree;
	XMLGenericTree *node;
	partstree=parser.ParseFile(path);
	if(partstree==0)
		return 0;
	partstree=partstree->FindNodeCalled("CHARACTER");
	node=partstree->FindNodeCalled("BODYFILES");
	this->meshfilename=node->GetAttribute("mesh");
	this->hierarchyfilename=node->GetAttribute("hierarchy");
	this->bonesfilename=node->GetAttribute("bones");
	this->body_texture.image=node->GetAttribute("texture_file");
	this->body_texture.alpha=false;
	return 1;
}
