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

#include ".\facedata.h"
#include "XMLGenericParser.h"
#include "IniManager.h"
#include <crtdbg.h>
#include <vector>

extern IniManager inimanager;

extern GLfloat init_vdata2[SKIN_VERTEX_NUMBER2][3];
extern GLfloat vdata2[SKIN_VERTEX_NUMBER2][3];


FaceData::FaceData(void)
{
	
	EYES_SCALE=2.75f;
	EYES_LZ=12.0f;
	EYES_LX=7.60f;
	EYES_LY=0.0f;
	EYES_RZ=12.0f;
	EYES_RX=-7.60f;
	EYES_RY=0.0f;
	TEETHS_X=0;
	TEETHS_Y=0;
	TEETHS_Z=0;

	HEADSCALE=0.54f;
	HEADPOS_X=0.0f;
	HEADPOS_Y=27.0f;
	HEADPOS_Z=0.0f;

	HAIR_sx=53.0; //61.0;//54
	HAIR_sy=59.0; //62.0;//67
	HAIR_sz=62.0; //62.0;
	HAIR_rx=90.0;
	HAIR_ry=180.0;
	HAIR_rz=180.0;
	HAIR_tx=0.0;
	HAIR_ty=4.0; //0.0;//4
	HAIR_tz=1.0; //1.0;//0

	HairIndicesNumber=0;

	haircoords=0;

	eyebulbs_texture.image="textures/eyes.bmp";
	eyelashes_texture.image="textures/eyelashes.bmp";
	eyebulbs_texture.alpha=false;
	eyelashes_texture.alpha=false;
	lips_texture.alpha=false;
	head_texture.alpha=false;

	FDPs = new FDPdata();
	FAPUs = new FAPUdata();
}

FaceData::~FaceData(void)
{
	//CHRIS fix
	int i = 0;
	for(i=0;i<FaceVerticesNumber;i++)
	{
		free(vdata[i]);
		free(vdatafordrawing[i]);
		//free(skin_point[i]);
	}
	free(vdata);
	free(vdatafordrawing);
	free(skin_point);
	
	for(i=0;i<TextureVerticesNumber;i++)
	{
		free(tdata[i]);
	}
	free(tdata);

	for(i=1;i<69;i++)
		for(int j=0;j<surface_num_polygons1[i];j++)
			free(regions_indices[i][j]);

	for(i=1;i<69;i++)
		free(regions_indices[i]);

	free(regions_indices);

	if(FaceSkinTextureMapping)
	{
		for(i=1;i<69;i++)
			for(int j=0;j<surface_num_polygons1[i];j++)
				free(textures_indices[i][j]);

		for(i=1;i<69;i++)
			free(textures_indices[i]);

		free(textures_indices);
	}

	if(haircoords!=0)
	{
		for(i=0;i<HairVerticesNumber;i++)
		{
			free(haircoords[i]);
		}
		free(haircoords);

		for(i=0;i<HairNormalsNumber;i++)
		{
			free(hairnormals[i]);
		}
		free(hairnormals);

		for(i=0;i<HairIndicesNumber;i++)
		{
			free(hairindices[i]);
		}
		free(hairindices);
	}

	for(i=1;i<69;i++)
		free(surface_list1[i]);
		
	delete FDPs;
	delete FAPUs;
}

//OLDNAME:init_vertex
void FaceData::CopyVertexData()
{
/*
	int i,j;

	for (i=0;i<this->FaceVerticesNumber;i++)
		for (j=0;j<3;j++)
			vdata[i][j]=init_vdata[i][j];

	for (i=0;i<SKIN_VERTEX_NUMBER2;i++)
		for (j=0;j<3;j++)
			vdata2[i][j]=init_vdata2[i][j];
*/
	// OTTIMIZZAZIONE

	//memcpy(vdata,init_vdata,3*this->FaceVerticesNumber*(sizeof(GLfloat)));
	memcpy(vdata2,init_vdata2,3*SKIN_VERTEX_NUMBER2*(sizeof(GLfloat)));
}

int FaceData::ReadFaceGeometry(std::string modelname)
{
	

	int i;
	i=0;
	std::string characterfilename;
	characterfilename=inimanager.Program_Path+"characters/"+modelname+".xml";
	if(this->GetFileNames(characterfilename)==0)
		return 0;
	
	printf("reading face geometry");
	i=i+ReadFaceVertices(inimanager.Program_Path+"characters/"+this->verticesfilename);
	printf(".");

	i=i+ReadFaceIndices(inimanager.Program_Path+"characters/"+this->indicesfilename);
	printf(".");

	i=i+FDPs->ReadFaceFDPs((inimanager.Program_Path+"characters/"+this->fdpsfilename).c_str(), (const float **) vdata);
	printf(".");

	//this reads hair but will read all the face from a single file
	if(ReadHair(inimanager.Program_Path+"characters/"+this->hairfilename)==0)
	{
		printf("WARNING: hair definition not found.\n");
	}
	else
		printf(".");
	//int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
	//tmpFlag |= _CRTDBG_CHECK_ALWAYS_DF;
	//_CrtSetDbgFlag( tmpFlag );

	if(ReadFaceParts(characterfilename)==0)
	{
		printf("WARNING: face parts definition not found, using defaults.\n");
	}
	else
		printf(".");
	
	if(i==3)
	{
		PreScaleFace();
		FAPUs->CalculateFAPUs(FDPs);
		printf(" done\n");
		return 1;
	}
	else
	{
		printf(" error reading face\n");
	
		return 0;
	}
	
	return 1;
}

int FaceData::ReadFaceVertices(std::string path)
{
	FILE *f;
	char cbuf[1024];
	int tri,co,vn,tm;
	int i;
	int tri_n, obj_n, eb;
	int co0, co1, co2;
	int vn0, vn1, vn2;
	int tm0, tm1, tm2;
	int v_n,n_n,t_n;
	GLfloat v0, v1, v2;
	GLfloat n0, n1, n2;
	GLfloat t0, t1;

	// Ouverture du fichier
	if(!(f = fopen(path.c_str(),"r")))
	{
		printf("\nERROR: can't read \"%s\"\n", path.c_str());
		return 0;
	}

	// Lecture du fichier de données
	sprintf(cbuf,"");
	while(cbuf[0]!='m' || cbuf[1]!='e' || cbuf[2]!='s' || cbuf[3]!='h')
		fgets( cbuf, 1024, f );

	// Read the line: "mesh tri 12278 co 6264 vn 6264 tm 6566"
	//co=vertex coordinate
	//vn=vertex normal
	//tm=texture mapping coordinate
	sscanf(cbuf, "mesh tri %i co %i vn %i tm %i",  &tri, &co, &vn, &tm);
	this->FaceVerticesNumber=co;
	this->VerticesNormalsNumber=vn;
	this->TrianglesNumber=tri;
	this->TextureVerticesNumber=tm;
	
	//initializes the array of triangles and vertices
	if(!(vdata=(GLfloat**)malloc(co*sizeof(GLfloat*))))
	{
		printf("\nERROR: malloc failed on vertices\n");
		fclose(f);
		return 0;
	}

	if(!(vdatafordrawing=(GLfloat**)malloc(co*sizeof(GLfloat*))))
	{
		printf("\nERROR: malloc failed on vertices\n");
		fclose(f);
		return 0;
	}


	if(!(tdata=(GLfloat**)malloc(tm*sizeof(GLfloat*))))
	{
		printf("\nERROR: malloc failed on vertices\n");
		fclose(f);
		return 0;
	}

	if(!(skin_point=(pointtype*)malloc(co*sizeof(pointtype))))
	{
		printf("\nERROR: malloc failed on skin_point\n");
		fclose(f);
		return 0;
	}

	for(i=0;i<co;i++)
	{
		if(!(vdata[i] = (GLfloat*)malloc(3*sizeof(GLfloat))))
		{
			printf("\nERROR: malloc failed on vertices[%i]", i);
			fclose(f);
			return 0;
		}
		if(!(vdatafordrawing[i] = (GLfloat*)malloc(3*sizeof(GLfloat))))
		{
			printf("\nERROR: malloc failed on vertices[%i]", i);
			fclose(f);
			return 0;
		}
	}

	for(i=0;i<tm;i++)
		if(!(tdata[i] = (GLfloat*)malloc(3*sizeof(GLfloat))))
		{
			printf("\nERROR: malloc failed on vertices[%i]", i);
			fclose(f);
			return 0;
		}

	//reads verts & triangles coords and stores them in the corresponding array
	// LECTURE DES TRIANGLES
	for(i=0;i<tri;i++)
	{
		// On vérifie déjà qu'on ne soit pas en fin de fichier
		if(fgets( cbuf, 1024, f ));
		else
		{
			printf("\nERROR: bad geometry file\n");
			fclose(f);
			return 0;
		}
		// On vérifie ensuite qu'on ne lit pas une ligne de commentaires (#...)
		if(cbuf[0] == '#') while(getc(f) != '\n');
		else
		{
			sscanf(cbuf,"  tri %i obj %i eb %i co %i %i %i vn %i %i %i tm %i %i %i",
						   &tri_n, &obj_n, &eb, &co0, &co1, &co2, &vn0, &vn1, &vn2, &tm0, &tm1, &tm2);
		}
	}
	// LECTURE DES SOMMETS
	for(i=0;i<co;i++)
	{
		// On vérifie déjà qu'on ne soit pas en fin de fichier
		if(fgets( cbuf, 1024, f ));
		else
		{
			printf("\nERROR: bad geometry file\n");
			fclose(f);
			return 0;
		}
		// On vérifie ensuite qu'on ne lit pas une ligne de commentaires (#...)
		if(cbuf[0] == '#') while(getc(f) != '\n');
		else
		{
			sscanf(cbuf,"  coord %i (%f,  %f,  %f)",
						   &v_n, &v0, &v1, &v2);
			vdata[i][0]=v0;
			vdata[i][1]=v1;
			vdata[i][2]=v2;

		}
	}

	for(i=0;i<vn;i++)
	{
		// On vérifie déjà qu'on ne soit pas en fin de fichier
		if(fgets( cbuf, 1024, f ));
		else
		{
			printf("\nWARNING: bad geometry file, skipping texture data\n");
			tdata=0;
			fclose(f);
			return 1;
		}
		// On vérifie ensuite qu'on ne lit pas une ligne de commentaires (#...)
		if(cbuf[0] == '#') while(getc(f) != '\n');
		else
		{
			sscanf(cbuf,"  vnormal %i (%f,  %f,  %f)",
						   &n_n, &n0, &n1, &n2);
		}
	}
  
	for(i=0;i<tm;i++)
	{
		// On vérifie déjà qu'on ne soit pas en fin de fichier
		if(fgets( cbuf, 1024, f ));
		else
		{
			printf("\nWARNING: bad geometry file, skipping texture data\n");
			tdata=0;
			fclose(f);
			return 1;
		}
		// On vérifie ensuite qu'on ne lit pas une ligne de commentaires (#...)
		if(cbuf[0] == '#') while(getc(f) != '\n');
		else
		{
			sscanf(cbuf,"  tmcoord %i (%f,  %f)",
						   &t_n, &t0, &t1);
			tdata[i][0]=t0;
			tdata[i][1]=1-t1;
		}
	}
	

	fclose(f);

	return 1;
}

int FaceData::ReadFaceIndices(std::string path)
{
	FILE* f;
	int i,j;
	int sizes[69], v=-1;
	int i_0, i_1, i_2;
	int t_0, t_1, t_2;
	char cbuf[10000];
	std::string t_path;
	

	if(this->textured=="1")
	{
		FaceSkinTextureMapping=true;
	}
	else
		FaceSkinTextureMapping=false;
    
	//t_path=path.substr(0,path.find_last_of("/"))+"/texture_indices.txt";
	// Ouverture du fichier
	if(!(f=fopen(path.c_str(), "r")))
	{
		{
		printf("ERROR: cant read \"%s\"\n", path.c_str());
		//FaceSkinTextureMapping=false;
		//textures_indices=0;
		//	if(!(f=fopen(path.c_str(), "r")))
		//	{
		//		printf("ERROR: cant read \"%s\"\n", path.c_str());
				return 0;
		//	}
		}
	}
	
	
	// Allocation memoire
	regions_indices=(GLint***)malloc(69*sizeof(GLint**));
	if(FaceSkinTextureMapping)
		textures_indices=(GLint***)malloc(69*sizeof(GLint**));
	else
		textures_indices=0;

	sprintf(cbuf,"");
	// Lecture du fichier
	for(i=1;i<69;i++)
	{
		// On lit la zone "v" concernee:					# %d
		while(cbuf[0]!='#') fgets(cbuf,10000,f);
		sscanf(cbuf, "# %d", &v);
		// On lit la taille "sizes[v]" de cette zone:		%d
		fgets(cbuf,10000,f);
		sscanf(cbuf,"SIZE %d", &sizes[v]);
		// Initialisation de la table de tailles
		surface_num_polygons1[v]=sizes[v];
		// Allocation memoire
		if(!(regions_indices[v]=(GLint**)malloc(sizes[v]*sizeof(GLint*))))
		{
			printf("ERROR: malloc failed on indicesm[%i]\n", v);
			return 0;
		}
		if(FaceSkinTextureMapping)
			if(!(textures_indices[v]=(GLint**)malloc(sizes[v]*sizeof(GLint*))))
			{
				printf("ERROR: malloc failed on texturesindicesm[%i]\n", v);
				return 0;
			}

		// Lecture des données
		for(j=0;j<sizes[v];j++)
		{
			if(!(regions_indices[v][j]=(GLint*)malloc(3*sizeof(GLint))))
			{
				printf("ERROR: malloc failed on indicesm[%i]\n", i);
				return 0;
			}
			if(FaceSkinTextureMapping)
			{
				if(!(textures_indices[v][j]=(GLint*)malloc(3*sizeof(GLint))))
				{
					printf("ERROR: malloc failed on texturesindicesm[%i]\n", i);
					return 0;
				}
			}
	
			fgets(cbuf,10000,f);
			if(FaceSkinTextureMapping)
				sscanf(cbuf,"%d %d %d %d %d %d", &i_0, &i_1, &i_2, &t_0, &t_1, &t_2);
			else
				sscanf(cbuf,"%d %d %d", &i_0, &i_1, &i_2);
			if(FaceSkinTextureMapping)
			{
				textures_indices[v][j][0]=t_0;
				textures_indices[v][j][1]=t_1;
				textures_indices[v][j][2]=t_2;
			}
			regions_indices[v][j][0]=i_0;
			regions_indices[v][j][1]=i_1;
			regions_indices[v][j][2]=i_2;
		}
	}
	fclose(f);
	return 1;
}


int FaceData::ReadFaceParts(std::string path)
{
	XMLGenericParser parser;
	XMLGenericTree *partstree;
	XMLGenericTree *node;
	partstree=parser.ParseFile(path);
	if(partstree==0)
		return 0;
	partstree=partstree->FindNodeCalled("CHARACTER");
	node=partstree->FindNodeCalled("HEADPOSITION");

	this->HEADSCALE=atof((node->GetAttribute("scale")).c_str());
	this->HEADPOS_X=atof((node->GetAttribute("x")).c_str());
	this->HEADPOS_Y=atof((node->GetAttribute("y")).c_str());
	this->HEADPOS_Z=atof((node->GetAttribute("z")).c_str());
	node=partstree->FindNodeCalled("LEFT_EYE");
	this->EYES_LX=atof((node->GetAttribute("x")).c_str());
	this->EYES_LY=atof((node->GetAttribute("y")).c_str());
	this->EYES_LZ=atof((node->GetAttribute("z")).c_str());
	node=partstree->FindNodeCalled("RIGHT_EYE");
	this->EYES_RX=atof((node->GetAttribute("x")).c_str());
	this->EYES_RY=atof((node->GetAttribute("y")).c_str());
	this->EYES_RZ=atof((node->GetAttribute("z")).c_str());
	node=partstree->FindNodeCalled("EYE_SCALE");
	this->EYES_SCALE=atof((node->GetAttribute("factor")).c_str());
	node=partstree->FindNodeCalled("TEETHS-TONGUE");
	this->TEETHS_X=atof((node->GetAttribute("x")).c_str());
	this->TEETHS_Y=atof((node->GetAttribute("y")).c_str());
	this->TEETHS_Z=atof((node->GetAttribute("z")).c_str());
	node=partstree->FindNodeCalled("HAIRPOSITION");
	if(node!=0)
	{
		this->HAIR_sx=atof((node->GetAttribute("scalex")).c_str());
		this->HAIR_sy=atof((node->GetAttribute("scaley")).c_str());
		this->HAIR_sz=atof((node->GetAttribute("scalez")).c_str());
		this->HAIR_tx=atof((node->GetAttribute("x")).c_str());
		this->HAIR_ty=atof((node->GetAttribute("y")).c_str());
		this->HAIR_tz=atof((node->GetAttribute("z")).c_str());
	}
	delete partstree;
	return 1;
}

int FaceData::GetFileNames(std::string path)
{
	XMLGenericParser *parser;
	XMLGenericTree *partstree;
	XMLGenericTree *node;
	
	partstree=0;

	
	parser=new XMLGenericParser();
	partstree=parser->ParseFile(path);


	if(partstree==0)
	{
		delete parser;
		return 0;
	}
	partstree=partstree->FindNodeCalled("CHARACTER");
	node=partstree->FindNodeCalled("HEADFILES");

	this->verticesfilename=node->GetAttribute("vertices");
	this->fdpsfilename=node->GetAttribute("fdps");
	this->indicesfilename=node->GetAttribute("indices");
	this->textured=node->GetAttribute("textured");
	this->hairfilename=node->GetAttribute("hair");
	this->lips_texture.image=node->GetAttribute("lips_texture");
	this->head_texture.image=node->GetAttribute("texture_file");
	this->eyebulbs_texture.image=node->GetAttribute("eyes_texture");
	if(this->eyebulbs_texture.image=="")
		this->eyebulbs_texture.image="textures/eyes.bmp";

	delete partstree;
	delete parser;
	return 1;
}


void FaceData::PreScaleFace()
{
	float _SCALE;
	float X_TRANSL,Y_TRANSL,Z_TRANSL;
	int i;

	float ***FDPfp = FDPs->featurePoints;

	float* fdp_3_5 = FDPfp[3][1];	////////////// A changer
	float* fdp_3_6 = FDPfp[3][2];	////////////// A changer
	float* tmp_3_3 = FDPfp[3][3];	////////////// A changer
	float* tmp_3_4 = FDPfp[3][4];	////////////// A changer

	float dist_pupils;
	float dist_3_1__3_6;

	GLfloat RightPupil[3];
	GLfloat LeftPupil[3];

	RightPupil[0] = (FDPfp[3][4][0] + FDPfp[3][2][0])/2.0f;
	RightPupil[1] = (FDPfp[3][4][1] + FDPfp[3][2][1])/2.0f;
	RightPupil[2] = (FDPfp[3][4][2] + FDPfp[3][2][2])/2.0f;

	LeftPupil[0]  = (FDPfp[3][3][0] + FDPfp[3][1][0])/2.0f;
	LeftPupil[1]  = (FDPfp[3][3][1] + FDPfp[3][1][1])/2.0f;
	LeftPupil[2]  = (FDPfp[3][3][2] + FDPfp[3][1][2])/2.0f;

	dist_pupils   = sqrt(	pow(RightPupil[0] - LeftPupil[0], 2) + 
							pow(RightPupil[1] - LeftPupil[1], 2) + 
							pow(RightPupil[2] - LeftPupil[2], 2)
						);
	dist_3_1__3_6 = sqrt(	pow(fdp_3_5[0] - fdp_3_6[0], 2) + 
							pow(fdp_3_5[1] - fdp_3_6[1], 2) + 
							pow(fdp_3_5[2] - fdp_3_6[2], 2)
				 		);
	//division between the distance fdp 3.5 and 3.6 of greta and the loaded model
	_SCALE = 15.0f/dist_pupils;
	//diffence between fdp 3.5 of greta and the loaded model
	X_TRANSL = 0;//7.54 - (fdp_3_5[0]+tmp_3_3[0])/4*_SCALE;
	Y_TRANSL = 0 - (fdp_3_5[1]+tmp_3_3[1])/2*_SCALE;
	Z_TRANSL = 0;// - (fdp_3_5[2]+tmp_3_3[2])/2*_SCALE;

	for(i=0; i<FaceVerticesNumber; i++)
	{
		vdata[i][0] = vdata[i][0]*_SCALE + X_TRANSL;
		vdata[i][1] = vdata[i][1]*_SCALE + Y_TRANSL;
		vdata[i][2] = vdata[i][2]*_SCALE + Z_TRANSL;
	}

	int group,num;
	
	for (group=2; group<=10; group++)
		if(group!=6)
			for (num=1; num<=FDPs->FDPGroupsSize[group]; num++)
			{
				FDPfp[group][num][0] = FDPfp[group][num][0]*_SCALE + X_TRANSL;
				FDPfp[group][num][1] = FDPfp[group][num][1]*_SCALE + Y_TRANSL;
				FDPfp[group][num][2] = FDPfp[group][num][2]*_SCALE + Z_TRANSL;
			}
		
	/*
	printf("eyesx %f\n",FDPfp[3][1][0]);
	printf("eyesx %f\n",LeftPupil[0]);
	EYES_LX=FDPfp[3][3][0]*0.91;
	EYES_LY=(FDPfp[3][3][1] + FDPfp[3][1][1])/2.0f;
	EYES_LZ=FDPfp[3][1][2]-2.0f*EYES_SCALE;
	EYES_RX=FDPfp[3][4][0]*0.91;
	EYES_RY=(FDPfp[3][4][1] + FDPfp[3][2][1])/2.0f;
	EYES_RZ=FDPfp[3][2][2]-2.0f*EYES_SCALE;
	//EYES_Y=LeftPupil[1];
	//EYES_Z=LeftPupil[2];
*/

}

//********************************************************************************************************************
//
//                          Initializes the skin_point[] array
//
//********************************************************************************************************************

//skin_point[] is a variable of the Agent object and so is passed
//during the Agent initialization

//OLDNAME:init_skin_members
void FaceData::InitializeSkinPointArray()
{
	int i, j;

	for (i=0; i<this->FaceVerticesNumber; i++){
		for (j=0; j<3; j++){
			skin_point[i].coordinate[j] = 0.0;//will hold the coordinates of the point
			skin_point[i].increment[j] = 0.0;//will hold the displqcement of the point during animation
			skin_point[i].normal[j] = 0.0;//will hold the normal of the point during animation
		}
		skin_point[i].surface = 0; //the surface which the point belongs to, 0 means no-surface
		skin_point[i].distance = -1.0; //the point has no distance because a valid distance is always positive
		//skin_point[i].simmetric = -1; //will refer to the point simmetric to this one, -1 means no-simmetric
		skin_point[i].color[0] = facial_skin.diffuse[0]; //the color of the skin
		skin_point[i].color[1] = facial_skin.diffuse[1];
		skin_point[i].color[2] = facial_skin.diffuse[2];
	}
}



//********************************************************************************************************************
//
//                          Copies the values in vdata[][] into skin_point[].coordinate[]
//
//********************************************************************************************************************

//OLDNAME:assign_skin_coordinates
//copy_vdata_into_skin_coordinate
void FaceData::CopyVdata2SkinPoint()
{
	int i,j;
	for (i=0; i<this->FaceVerticesNumber; i++){
		for (j=0; j<3; j++){
			skin_point[i].coordinate[j] = vdata[i][j];
		}
	}
}

//********************************************************************************************************************
//
//                     Data contained in vdata_l_wrin[][] is used to calculate skin_point[].distance
//
//********************************************************************************************************************

//OLDNAME:assign_skin_distance
//calculate_skin_distance_for_l_wrin
void FaceData::CalculateSkinDistanceLwrin()
{

	int i, j;
	float temp;

	for (i=0; i<this->FaceVerticesNumber; i++)
	{
		skin_point[i].distance = 100000000; //initialized to a fake number (after the min function will be used)
		if (skin_point[i].surface == 1)
		{//does the calculation only for the points of the surfqce number 1
			skin_point[i].distance = 100000000;
			for (j=0; j<WRIN_VERTEX_NUMBER; j++)
			{
				temp = sqrt((skin_point[i].coordinate[0]-vdata_l_wrin[j][0])*(skin_point[i].coordinate[0]-vdata_l_wrin[j][0])+(skin_point[i].coordinate[1]-vdata_l_wrin[j][1])*(skin_point[i].coordinate[1]-vdata_l_wrin[j][1])+(skin_point[i].coordinate[2]-vdata_l_wrin[j][2])*(skin_point[i].coordinate[2]-vdata_l_wrin[j][2]));//calcolo distanza
				skin_point[i].distance = min(skin_point[i].distance, temp);
			}
		}
		if (skin_point[i].surface == 2)
		{//does the calculation only for the points of the surfqce number 1
			skin_point[i].distance = 100000000;
			for (j=0; j<WRIN_VERTEX_NUMBER; j++)
			{
				temp = sqrt((skin_point[i].coordinate[0]+vdata_l_wrin[j][0])
					*(skin_point[i].coordinate[0]+vdata_l_wrin[j][0])
					+(skin_point[i].coordinate[1]-vdata_l_wrin[j][1])
					*(skin_point[i].coordinate[1]-vdata_l_wrin[j][1])
					+(skin_point[i].coordinate[2]-vdata_l_wrin[j][2])
					*(skin_point[i].coordinate[2]-vdata_l_wrin[j][2]));//calcolo distanza
				skin_point[i].distance = min(skin_point[i].distance, temp);
			}
		}
	}
}

//********************************************************************************************************************
//
//                     Data contained in vdata_l_inf_eyelid[][] is used to calculate skin_point[].distance
//
//********************************************************************************************************************

//OLDNAME:assign_inf_eyelid_distance
//calculate_skin_distance_for_inf_eyelid
void FaceData::CalculateSkinDistanceInfeyelid()
{

	int i, j;
	float temp;

	for (i=0; i<this->FaceVerticesNumber; i++){
		if (skin_point[i].surface == 5 || skin_point[i].surface == 7){
			skin_point[i].distance = 100000000; //initialized to a fake number (after the min function will be used)
			for (j=0; j<INF_EYELID_VERTEX_NUMBER; j++){
				temp = sqrt((skin_point[i].coordinate[0]-vdata_l_inf_eyelid[j][0])*(skin_point[i].coordinate[0]-vdata_l_inf_eyelid[j][0])+(skin_point[i].coordinate[1]-vdata_l_inf_eyelid[j][1])*(skin_point[i].coordinate[1]-vdata_l_inf_eyelid[j][1])+(skin_point[i].coordinate[2]-vdata_l_inf_eyelid[j][2])*(skin_point[i].coordinate[2]-vdata_l_inf_eyelid[j][2]));//calcolo distanza
				skin_point[i].distance = min(skin_point[i].distance, temp);
			}
		}
	}

	for (i=0; i<this->FaceVerticesNumber; i++){
		if (skin_point[i].surface == 6 || skin_point[i].surface == 8 ){
			skin_point[i].distance = 100000000; //initialized to a fake number (after the min function will be used)
			for (j=0; j<INF_EYELID_VERTEX_NUMBER; j++){
temp = sqrt((skin_point[i].coordinate[0]+vdata_l_inf_eyelid[j][0])
			*(skin_point[i].coordinate[0]+vdata_l_inf_eyelid[j][0])
			+(skin_point[i].coordinate[1]-vdata_l_inf_eyelid[j][1])
			*(skin_point[i].coordinate[1]-vdata_l_inf_eyelid[j][1])
			+(skin_point[i].coordinate[2]-vdata_l_inf_eyelid[j][2])
			*(skin_point[i].coordinate[2]-vdata_l_inf_eyelid[j][2]));//calcolo distanza
				skin_point[i].distance = min(skin_point[i].distance, temp);
			}
		}
	}
	/*
	for (i=0; i<this->FaceVerticesNumber; i++){
		if (skin_point[i].surface == 6 || skin_point[i].surface == 8 ){
			skin_point[i].distance = skin_point[skin_point[i].simmetric].distance; 
		}
	}
	*/

}

//********************************************************************************************************************
//
//                     Data contained in vdata_l_sup_eyelid[][] is used to calculate skin_point[].distance
//
//********************************************************************************************************************

//OLDNAME:assign_sup_eyelid_distance
//calculate_skin_distance_for_sup_eyelid
void FaceData::CalculateSkinDistanceSupeyelid()
{

	int i, j;
	float temp;

	for (i=0; i<this->FaceVerticesNumber; i++){
		if (skin_point[i].surface == 9 || skin_point[i].surface == 11)
		{
			skin_point[i].distance = 100000000; //initialized to a fake number (after the min function will be used)
			for (j=0; j<SUP_EYELID_VERTEX_NUMBER; j++)
			{
				temp = sqrt((skin_point[i].coordinate[0]-vdata_l_sup_eyelid[j][0])*(skin_point[i].coordinate[0]-vdata_l_sup_eyelid[j][0])+(skin_point[i].coordinate[1]-vdata_l_sup_eyelid[j][1])*(skin_point[i].coordinate[1]-vdata_l_sup_eyelid[j][1])+(skin_point[i].coordinate[2]-vdata_l_sup_eyelid[j][2])*(skin_point[i].coordinate[2]-vdata_l_sup_eyelid[j][2]));//calcolo distanza
				skin_point[i].distance = min(skin_point[i].distance, temp);
			}
		}
	}
	for (i=0; i<this->FaceVerticesNumber; i++){
		if (skin_point[i].surface == 10 || skin_point[i].surface == 12)
		{
			skin_point[i].distance = 100000000; //initialized to a fake number (after the min function will be used)
			for (j=0; j<SUP_EYELID_VERTEX_NUMBER; j++)
			{
temp = sqrt((skin_point[i].coordinate[0]+vdata_l_sup_eyelid[j][0])
			*(skin_point[i].coordinate[0]+vdata_l_sup_eyelid[j][0])
			+(skin_point[i].coordinate[1]-vdata_l_sup_eyelid[j][1])
			*(skin_point[i].coordinate[1]-vdata_l_sup_eyelid[j][1])
			+(skin_point[i].coordinate[2]-vdata_l_sup_eyelid[j][2])
			*(skin_point[i].coordinate[2]-vdata_l_sup_eyelid[j][2]));//calcolo distanza
				skin_point[i].distance = min(skin_point[i].distance, temp);
			}
		}
	}			

/*
	for (i=0; i<this->FaceVerticesNumber; i++){
		if (skin_point[i].surface == 10 || skin_point[i].surface == 12){
			skin_point[i].distance = skin_point[skin_point[i].simmetric].distance; 
		}
	}
	*/
}

//********************************************************************************************************************
//
//                    skin_point[].distance is used to calculate the color of eyelids, to simulate "makeup"
//
//********************************************************************************************************************

//OLDNAME:assign_eyelid_color
void FaceData::CalculateColorEyelid()
{

	int i, j;


	for(i=5; i < 9; i++)
	{
		j=0;

		while (j < surface_num_polygons1[i])
		{
/*			
			skin_point[skin_surfaces_indices[i][j][0]].color[0] = 0.2+0.6*12*skin_point[skin_surfaces_indices[i][j][0]].distance*skin_point[skin_surfaces_indices[i][j][0]].distance/(1+12*skin_point[skin_surfaces_indices[i][j][0]].distance*skin_point[skin_surfaces_indices[i][j][0]].distance);
			skin_point[skin_surfaces_indices[i][j][0]].color[1] = 0.1+0.5*12*skin_point[skin_surfaces_indices[i][j][0]].distance*skin_point[skin_surfaces_indices[i][j][0]].distance/(1+12*skin_point[skin_surfaces_indices[i][j][0]].distance*skin_point[skin_surfaces_indices[i][j][0]].distance);
			skin_point[skin_surfaces_indices[i][j][0]].color[2] = 0.1+0.4*12*skin_point[skin_surfaces_indices[i][j][0]].distance*skin_point[skin_surfaces_indices[i][j][0]].distance/(1+12*skin_point[skin_surfaces_indices[i][j][0]].distance*skin_point[skin_surfaces_indices[i][j][0]].distance);


			skin_point[skin_surfaces_indices[i][j][1]].color[0] = 0.2+0.6*12*skin_point[skin_surfaces_indices[i][j][1]].distance*skin_point[skin_surfaces_indices[i][j][1]].distance/(1+12*skin_point[skin_surfaces_indices[i][j][1]].distance*skin_point[skin_surfaces_indices[i][j][1]].distance);
			skin_point[skin_surfaces_indices[i][j][1]].color[1] = 0.1+0.5*12*skin_point[skin_surfaces_indices[i][j][1]].distance*skin_point[skin_surfaces_indices[i][j][1]].distance/(1+12*skin_point[skin_surfaces_indices[i][j][1]].distance*skin_point[skin_surfaces_indices[i][j][1]].distance);
			skin_point[skin_surfaces_indices[i][j][1]].color[2] = 0.1+0.4*12*skin_point[skin_surfaces_indices[i][j][1]].distance*skin_point[skin_surfaces_indices[i][j][1]].distance/(1+12*skin_point[skin_surfaces_indices[i][j][1]].distance*skin_point[skin_surfaces_indices[i][j][1]].distance);

			skin_point[skin_surfaces_indices[i][j][2]].color[0] = 0.2+0.6*12*skin_point[skin_surfaces_indices[i][j][2]].distance*skin_point[skin_surfaces_indices[i][j][2]].distance/(1+12*skin_point[skin_surfaces_indices[i][j][2]].distance*skin_point[skin_surfaces_indices[i][j][2]].distance);
			skin_point[skin_surfaces_indices[i][j][2]].color[1] = 0.1+0.5*12*skin_point[skin_surfaces_indices[i][j][2]].distance*skin_point[skin_surfaces_indices[i][j][2]].distance/(1+12*skin_point[skin_surfaces_indices[i][j][2]].distance*skin_point[skin_surfaces_indices[i][j][2]].distance);
			skin_point[skin_surfaces_indices[i][j][2]].color[2] = 0.1+0.4*12*skin_point[skin_surfaces_indices[i][j][2]].distance*skin_point[skin_surfaces_indices[i][j][2]].distance/(1+12*skin_point[skin_surfaces_indices[i][j][2]].distance*skin_point[skin_surfaces_indices[i][j][2]].distance);
*/

			skin_point[regions_indices[i][j][0]].color[0] = facial_skin.diffuse[0]*12*skin_point[regions_indices[i][j][0]].distance*skin_point[regions_indices[i][j][0]].distance/(1+12*skin_point[regions_indices[i][j][0]].distance*skin_point[regions_indices[i][j][0]].distance);
			skin_point[regions_indices[i][j][0]].color[1] = facial_skin.diffuse[1]*12*skin_point[regions_indices[i][j][0]].distance*skin_point[regions_indices[i][j][0]].distance/(1+12*skin_point[regions_indices[i][j][0]].distance*skin_point[regions_indices[i][j][0]].distance);
			skin_point[regions_indices[i][j][0]].color[2] = facial_skin.diffuse[2]*12*skin_point[regions_indices[i][j][0]].distance*skin_point[regions_indices[i][j][0]].distance/(1+12*skin_point[regions_indices[i][j][0]].distance*skin_point[regions_indices[i][j][0]].distance);
			skin_point[regions_indices[i][j][1]].color[0] = facial_skin.diffuse[0]*12*skin_point[regions_indices[i][j][1]].distance*skin_point[regions_indices[i][j][1]].distance/(1+12*skin_point[regions_indices[i][j][1]].distance*skin_point[regions_indices[i][j][1]].distance);
			skin_point[regions_indices[i][j][1]].color[1] = facial_skin.diffuse[1]*12*skin_point[regions_indices[i][j][1]].distance*skin_point[regions_indices[i][j][1]].distance/(1+12*skin_point[regions_indices[i][j][1]].distance*skin_point[regions_indices[i][j][1]].distance);
			skin_point[regions_indices[i][j][1]].color[2] = facial_skin.diffuse[2]*12*skin_point[regions_indices[i][j][1]].distance*skin_point[regions_indices[i][j][1]].distance/(1+12*skin_point[regions_indices[i][j][1]].distance*skin_point[regions_indices[i][j][1]].distance);
			skin_point[regions_indices[i][j][2]].color[0] = facial_skin.diffuse[0]*12*skin_point[regions_indices[i][j][2]].distance*skin_point[regions_indices[i][j][2]].distance/(1+12*skin_point[regions_indices[i][j][2]].distance*skin_point[regions_indices[i][j][2]].distance);
			skin_point[regions_indices[i][j][2]].color[1] = facial_skin.diffuse[1]*12*skin_point[regions_indices[i][j][2]].distance*skin_point[regions_indices[i][j][2]].distance/(1+12*skin_point[regions_indices[i][j][2]].distance*skin_point[regions_indices[i][j][2]].distance);
			skin_point[regions_indices[i][j][2]].color[2] = facial_skin.diffuse[2]*12*skin_point[regions_indices[i][j][2]].distance*skin_point[regions_indices[i][j][2]].distance/(1+12*skin_point[regions_indices[i][j][2]].distance*skin_point[regions_indices[i][j][2]].distance);

			j++;
		}

	}

	//upper eyelid
	for(i=9; i < 13 ;i++) {
		j=0;

		while (j < surface_num_polygons1[i]){
/*			
			skin_point[skin_surfaces_indices[i][j][0]].color[0] = 0.4+0.4*2*skin_point[skin_surfaces_indices[i][j][0]].distance*skin_point[skin_surfaces_indices[i][j][0]].distance/(1+2*skin_point[skin_surfaces_indices[i][j][0]].distance*skin_point[skin_surfaces_indices[i][j][0]].distance);
			skin_point[skin_surfaces_indices[i][j][0]].color[1] = 0.2+0.4*2*skin_point[skin_surfaces_indices[i][j][0]].distance*skin_point[skin_surfaces_indices[i][j][0]].distance/(1+2*skin_point[skin_surfaces_indices[i][j][0]].distance*skin_point[skin_surfaces_indices[i][j][0]].distance);
			skin_point[skin_surfaces_indices[i][j][0]].color[2] = 0.1+0.4*2*skin_point[skin_surfaces_indices[i][j][0]].distance*skin_point[skin_surfaces_indices[i][j][0]].distance/(1+2*skin_point[skin_surfaces_indices[i][j][0]].distance*skin_point[skin_surfaces_indices[i][j][0]].distance);


			skin_point[skin_surfaces_indices[i][j][1]].color[0] = 0.4+0.4*2*skin_point[skin_surfaces_indices[i][j][1]].distance*skin_point[skin_surfaces_indices[i][j][1]].distance/(1+2*skin_point[skin_surfaces_indices[i][j][1]].distance*skin_point[skin_surfaces_indices[i][j][1]].distance);
			skin_point[skin_surfaces_indices[i][j][1]].color[1] = 0.2+0.4*2*skin_point[skin_surfaces_indices[i][j][1]].distance*skin_point[skin_surfaces_indices[i][j][1]].distance/(1+2*skin_point[skin_surfaces_indices[i][j][1]].distance*skin_point[skin_surfaces_indices[i][j][1]].distance);
			skin_point[skin_surfaces_indices[i][j][1]].color[2] = 0.1+0.4*2*skin_point[skin_surfaces_indices[i][j][1]].distance*skin_point[skin_surfaces_indices[i][j][1]].distance/(1+2*skin_point[skin_surfaces_indices[i][j][1]].distance*skin_point[skin_surfaces_indices[i][j][1]].distance);


			skin_point[skin_surfaces_indices[i][j][2]].color[0] = 0.4+0.4*2*skin_point[skin_surfaces_indices[i][j][2]].distance*skin_point[skin_surfaces_indices[i][j][2]].distance/(1+2*skin_point[skin_surfaces_indices[i][j][2]].distance*skin_point[skin_surfaces_indices[i][j][2]].distance);
			skin_point[skin_surfaces_indices[i][j][2]].color[1] = 0.2+0.4*2*skin_point[skin_surfaces_indices[i][j][2]].distance*skin_point[skin_surfaces_indices[i][j][2]].distance/(1+2*skin_point[skin_surfaces_indices[i][j][2]].distance*skin_point[skin_surfaces_indices[i][j][2]].distance);
			skin_point[skin_surfaces_indices[i][j][2]].color[2] = 0.1+0.4*2*skin_point[skin_surfaces_indices[i][j][2]].distance*skin_point[skin_surfaces_indices[i][j][2]].distance/(1+2*skin_point[skin_surfaces_indices[i][j][2]].distance*skin_point[skin_surfaces_indices[i][j][2]].distance);
*/

			skin_point[regions_indices[i][j][0]].color[0] = facial_skin.diffuse[0]*2*skin_point[regions_indices[i][j][0]].distance*skin_point[regions_indices[i][j][0]].distance/(1+2*skin_point[regions_indices[i][j][0]].distance*skin_point[regions_indices[i][j][0]].distance);
			skin_point[regions_indices[i][j][0]].color[1] = facial_skin.diffuse[1]*2*skin_point[regions_indices[i][j][0]].distance*skin_point[regions_indices[i][j][0]].distance/(1+2*skin_point[regions_indices[i][j][0]].distance*skin_point[regions_indices[i][j][0]].distance);
			skin_point[regions_indices[i][j][0]].color[2] = facial_skin.diffuse[2]*2*skin_point[regions_indices[i][j][0]].distance*skin_point[regions_indices[i][j][0]].distance/(1+2*skin_point[regions_indices[i][j][0]].distance*skin_point[regions_indices[i][j][0]].distance);
			skin_point[regions_indices[i][j][1]].color[0] = facial_skin.diffuse[0]*2*skin_point[regions_indices[i][j][1]].distance*skin_point[regions_indices[i][j][1]].distance/(1+2*skin_point[regions_indices[i][j][1]].distance*skin_point[regions_indices[i][j][1]].distance);
			skin_point[regions_indices[i][j][1]].color[1] = facial_skin.diffuse[1]*2*skin_point[regions_indices[i][j][1]].distance*skin_point[regions_indices[i][j][1]].distance/(1+2*skin_point[regions_indices[i][j][1]].distance*skin_point[regions_indices[i][j][1]].distance);
			skin_point[regions_indices[i][j][1]].color[2] = facial_skin.diffuse[2]*2*skin_point[regions_indices[i][j][1]].distance*skin_point[regions_indices[i][j][1]].distance/(1+2*skin_point[regions_indices[i][j][1]].distance*skin_point[regions_indices[i][j][1]].distance);
			skin_point[regions_indices[i][j][2]].color[0] = facial_skin.diffuse[0]*2*skin_point[regions_indices[i][j][2]].distance*skin_point[regions_indices[i][j][2]].distance/(1+2*skin_point[regions_indices[i][j][2]].distance*skin_point[regions_indices[i][j][2]].distance);
			skin_point[regions_indices[i][j][2]].color[1] = facial_skin.diffuse[1]*2*skin_point[regions_indices[i][j][2]].distance*skin_point[regions_indices[i][j][2]].distance/(1+2*skin_point[regions_indices[i][j][2]].distance*skin_point[regions_indices[i][j][2]].distance);
			skin_point[regions_indices[i][j][2]].color[2] = facial_skin.diffuse[2]*2*skin_point[regions_indices[i][j][2]].distance*skin_point[regions_indices[i][j][2]].distance/(1+2*skin_point[regions_indices[i][j][2]].distance*skin_point[regions_indices[i][j][2]].distance);

			j++;
		}
	}
}

void FaceData::CalculateNormal( float *p1, float *p2,  float *p3, float *norm )
{
  float coa, cob, coc ;
  float px1, py1, pz1 ;
  float px2, py2, pz2 ;
  float px3, py3, pz3 ;
  
  float absvec ;
 
#ifdef FACE_POSER
  px3 = p1[0] ;
  py3 = p1[1] ;
  pz3 = p1[2] ;
  
  px2 = p2[0] ;
  py2 = p2[1] ;
  pz2 = p2[2] ;
  
  px1 = p3[0] ;
  py1 = p3[1] ;
  pz1 = p3[2] ;

#else
  px1 = p1[0] ;
  py1 = p1[1] ;
  pz1 = p1[2] ;
  
  px2 = p2[0] ;
  py2 = p2[1] ;
  pz2 = p2[2] ;
  
  px3 = p3[0] ;
  py3 = p3[1] ;
  pz3 = p3[2] ;
#endif

  

  coa = -(py1 * (pz2-pz3) + py2*(pz3-pz1) + py3*(pz1-pz2)) ;
  cob = -(pz1 * (px2-px3) + pz2*(px3-px1) + pz3*(px1-px2)) ;
  coc = -(px1 * (py2-py3) + px2*(py3-py1) + px3*(py1-py2)) ;
  
  absvec = sqrt ((double) ((coa*coa) + (cob*cob) + (coc*coc))) ;
  //absvec = 1.0;	
  
  norm[0] = coa/absvec ;
  norm[1] = cob/absvec ;
  norm[2] = coc/absvec ;
}

//********************************************************************************************************************
//
//             Initializes the "normal" field of the skin_point array
//
//********************************************************************************************************************

//OLDNAME:assign_skin_normals
void FaceData::InitializeSkinNormals()
{
	int i, j;
	float temp_normal[3];
	float *aux_pointer[3]; //the point

	
	//initializes to zero, maybe not necessary
	for(i=0; i < this->FaceVerticesNumber; i++) {
		for (j=0; j < 3; j++) { 
			skin_point[i].normal[j] = 0.0;
		}				
	}

	for(i=1; i < 69; i++) {    //for each surface
		j = 0;

		while(j < surface_num_polygons1[i]) {    //for all its triangles

			aux_pointer[0] = &(skin_point[regions_indices[i][j][0]].coordinate[0]);//si possono accorpare nella chiamata calc_normal
			aux_pointer[1] = &(skin_point[regions_indices[i][j][1]].coordinate[0]);
			aux_pointer[2] = &(skin_point[regions_indices[i][j][2]].coordinate[0]);

			
			CalculateNormal(aux_pointer[0], aux_pointer[1], aux_pointer[2], &temp_normal[0] );	


			skin_point[regions_indices[i][j][0]].normal[0] += temp_normal[0];
			skin_point[regions_indices[i][j][1]].normal[0] += temp_normal[0];
			skin_point[regions_indices[i][j][2]].normal[0] += temp_normal[0];

			skin_point[regions_indices[i][j][0]].normal[1] += temp_normal[1];
			skin_point[regions_indices[i][j][1]].normal[1] += temp_normal[1];
			skin_point[regions_indices[i][j][2]].normal[1] += temp_normal[1];
	
			skin_point[regions_indices[i][j][0]].normal[2] += temp_normal[2];
			skin_point[regions_indices[i][j][1]].normal[2] += temp_normal[2];
			skin_point[regions_indices[i][j][2]].normal[2] += temp_normal[2];

			j++;
		}
	}					
}

//********************************************************************************************************************
//
// function that initializes the "surface" field of each element of the skin_point array
//
//********************************************************************************************************************

//OLDNAME:assign_skin_surfaces
//initialize_skin_surfaces
void FaceData::InitializeSkinSurfaces(int surface_number)
{
	int i, j;

	i=0;

	while (i < surface_num_polygons1[surface_number])//for all the triangles of the surface
	{
		for(j=0; j < 3; j++)
		{
			skin_point[regions_indices[surface_number][i][j]].surface = surface_number; 
		}
		i++;
	}
}

//********************************************************************************************************************
//
// this function builds one list for each area (called here "surface"),
// putting into the list all the indices of the points of that area
// the functions should be called after all the fields of skin_point have been updated
// each list  is terminated by a -1 index
//
//********************************************************************************************************************

//OLDNAME:create_surface_list1
void FaceData::CreateSkinSurfaceList()
{
	int i, num_points[69];
	int *aux_pointer[69];

	//this array will contain the number of points for each surface
	for(i=0; i < 69; i++){
		num_points[i] = 0;
	}

	//for each vertex of the skin
	for(i=0; i < this->FaceVerticesNumber; i++){
		num_points[skin_point[i].surface] += 1;//count the vertex in the corresponding element of the array
	}
	
	for(i=1; i < 69; i++){
		//allocate memory for the 68 lists
		if(!(surface_list1[i] = (int *) new int [(num_points[i]+1)]))//"+1" because the list will also contain a -1 at the end
		//if(!(surface_list1[i] = (int *)malloc((num_points[i]+1)*sizeof(int))))//"+1" because the list will also contain a -1 at the end
		{
		printf("ERROR: malloc failed on surface_list1[]\n");
		exit(2);
		}
	}

	//initializes an array of pointers to the beginning of each of the 68 lists
	for(i=1; i < 69; i++){
		aux_pointer[i] = surface_list1[i];
	}

	//fill in all the lists
	for(i=8; i < this->FaceVerticesNumber; i++)//starting from element number 8 because the first 0-7 elements are the vertices of one cube sorrounding the face model
	{
		if (skin_point[i].surface!=0)
			*(aux_pointer[skin_point[i].surface]++) = i;
		//the same result could be done by the following two instructions
		//*aux_pointer[skin_point[i].surface] = i;
		//aux_pointer[skin_point[i].surface] += 1;
	}

	//put the -1 to mark the end of each list
	for(i=1; i < 69; i++){ 
		*(aux_pointer[i]) = -1;
	}
}

//********************************************************************************************************************
//
//						   calculates the skin normals during animation
//
//********************************************************************************************************************

//OLDNAME:assign_skin_normals_runtime
void FaceData::CalculateSkinNormalsRuntime(GLint* flag_surface1)
{
	int i, j;
	//float vertexdata[SKIN_VERTEX_NUMBER1][3];
	float temp_normal[3];
	float *aux_pointer[3];

/*
	for(i=0; i < SKIN_VERTEX_NUMBER1; i++){
		for(j=0; j < 3; j++){
			vertexdata[i][j] = skin_point[i].coordinate[j]+skin_point[i].increment[j];
		}
	}
*/

	for(i=1; i < 69; i++)
	{
		if(flag_surface1[i])
		{
			//reset normals to zero
			j = 0;
			while(surface_list1[i][j]!=-1){
				skin_point[surface_list1[i][j]].normal[0] = 0;
				skin_point[surface_list1[i][j]].normal[1] = 0;
				skin_point[surface_list1[i][j]].normal[2] = 0;
				j++;
			}
		}
	}

/*
	for(i=1; i < 69; i++)
	{
		if(flag_surface1[i])
		{
			j = 0;
			while(j < surface_num_polygons1[i])
			{
				for(k=0; k < 3; k++)
				{
					skin_point[skin_surfaces_indices[i][j][0]].normal[k] = 0.0;
					skin_point[skin_surfaces_indices[i][j][1]].normal[k] = 0.0;
					skin_point[skin_surfaces_indices[i][j][2]].normal[k] = 0.0;
				}
			j++;
			}
		}
	}
*/

	//vdata[][] contains the update values of the skin coordinates

	for(i=1; i < 69; i++)
	{
		if(flag_surface1[i])
		{
			j = 0;
			while(j < surface_num_polygons1[i])
			{
				aux_pointer[0] = &(vdata[regions_indices[i][j][0]][0]);
				aux_pointer[1] = &(vdata[regions_indices[i][j][1]][0]);
				aux_pointer[2] = &(vdata[regions_indices[i][j][2]][0]);

				CalculateNormal(aux_pointer[0], aux_pointer[1], aux_pointer[2], &temp_normal[0] );	

				skin_point[regions_indices[i][j][0]].normal[0] += temp_normal[0];
				skin_point[regions_indices[i][j][1]].normal[0] += temp_normal[0];
				skin_point[regions_indices[i][j][2]].normal[0] += temp_normal[0];

				skin_point[regions_indices[i][j][0]].normal[1] += temp_normal[1];
				skin_point[regions_indices[i][j][1]].normal[1] += temp_normal[1];
				skin_point[regions_indices[i][j][2]].normal[1] += temp_normal[1];
		
				skin_point[regions_indices[i][j][0]].normal[2] += temp_normal[2];
				skin_point[regions_indices[i][j][1]].normal[2] += temp_normal[2];
				skin_point[regions_indices[i][j][2]].normal[2] += temp_normal[2];

				j++;
			}
		}
	}					
}

//********************************************************************************************************************
//
//					   Funzione che realizza il bump sulla fronte
//	     
//
//********************************************************************************************************************

#define MIN(A,B) ((A) < (B) ? (A) : (B))
#define MAX(A,B) ((A) > (B) ? (A) : (B))

void FaceData::ForeheadBump(FAPdata *FAPs,int frame_count)
{	//A FINIR
	std::vector<float> &lfaps = FAPs->lfaps;

	int k=0, i=0, j=0;
	float distance=0.0, aux=0.0, temp1=0.0, temp2=0.0, temp3=0.0; 
	float node1[3];	// {10.0,  5.0 , 19.4};	//for the left eye
	float node2[3];	// {-10.0,  5.0 , 19.4};	//for the right eye
	
	pointtype* skin_point_k_i;

	node1[0]=10.0;
	node1[1]=5.0;
	node1[2]=19.4;
	node2[0]=-10.0;
	node2[1]=5.0;
	node2[2]=19.4;

	//giochiamo con le normali per fare il bump sulla fronte
	//con questo ciclo facciamo le rughe più esterne
	for (k=31; k<33; k++)
	{
		for(i=0;surface_list1[k][i]!=-1;i++)
		{
			j=1;
			skin_point_k_i = &skin_point[surface_list1[k][i]];

			if (skin_point_k_i->coordinate[j] >= 8.5 && skin_point_k_i->coordinate[j] <= 17.2 && skin_point_k_i->coordinate[j-1] >= 0.0 && skin_point_k_i->coordinate[j-1] <= 13.5 && ( (lfaps[33]) > 0 || (lfaps[34]) > 0) )
			{				
				distance = sqrt(
								3 * (skin_point_k_i->coordinate[0]-node1[0]) * 
								(skin_point_k_i->coordinate[0]-node1[0]) + 
								(skin_point_k_i->coordinate[1]-node1[1]) * 
								(skin_point_k_i->coordinate[1]-node1[1]) + 
								(skin_point_k_i->coordinate[2]-node1[2]) * 
								(skin_point_k_i->coordinate[2]-node1[2])
								);
				aux = 4.0 * (	cos(skin_point_k_i->coordinate[j]/0.7) * 
								sin(skin_point_k_i->coordinate[j]/0.7)
							) * (exp(-(distance)/3.2))
							;
		
				temp1 = 0.0;
				if (lfaps[33]>0 && (i%2==1))
				{
					temp1 = lfaps[33] * lfaps[33];
					skin_point_k_i->normal[j] += MIN(temp1,5)*aux;
				}
				temp2 = 0.0;
				//Occhio destro 
 				if (lfaps[34]>0 && (i%2==0))
				{
					temp2 = lfaps[34] * lfaps[34];
					skin_point_k_i->normal[j] += MIN(temp2,5)*aux;
				}
			}
		}
	}

	//giochiamo con le normali per fare il bump sulla fronte
	//con questo ciclo facciamo le rughe più esterne
	for (k=31; k<33; k++)
	{
		for(i=0;surface_list1[k][i]!=-1;i++)
		{
			j=1; //era j=2 cioè il vecchio asse z
			
			skin_point_k_i = &skin_point[surface_list1[k][i]];

			if (skin_point_k_i->coordinate[j] >= 8.5 && skin_point_k_i->coordinate[j] <= 17.2 && skin_point_k_i->coordinate[j-1] >= -13.5 && skin_point_k_i->coordinate[j-1] <= 13.5 && (lfaps[31] > 0 || lfaps[32] > 0 ) )
			{
				if ((lfaps[31]-0.2)>0 || (lfaps[32]-0.2)>0)
				{
					distance = sqrt(
									(skin_point_k_i->coordinate[0]) * 
									(skin_point_k_i->coordinate[0]) + 
									(skin_point_k_i->coordinate[1] - node1[1]) * 
									(skin_point_k_i->coordinate[1] - node1[1]) / 6 + 
									(skin_point_k_i->coordinate[2] - node1[2]) * 
									(skin_point_k_i->coordinate[2] - node1[2])
									);
					aux = 2.5 * (	cos(skin_point_k_i->coordinate[j]/0.7) * 
									sin(skin_point_k_i->coordinate[j]/0.7)
								) *(exp(-(distance)/3.2))
								;
					temp3=0.0;
					if ((lfaps[31]-0.2)>0 && (i%2==1))
					{
						temp3 += (lfaps[31]-0.2)*(lfaps[31]-0.2);
					}
					if ((lfaps[32]-0.2)>0 && (i%2==0))
					{
						temp3 += (lfaps[32]-0.2)*(lfaps[32]-0.2);
					}
					aux = MIN(temp3,2)*aux;
					skin_point_k_i->normal[j] += aux;	
				}
			}
		}
	}
}


int FaceData::ReadHair(std::string path)
{
	XMLGenericParser parser;
	XMLGenericTree *hairtree;
	XMLGenericTree *node;
	int i;

	hairtree=parser.ParseFile(path);
	if(hairtree==0)
		return 0;
	hairtree=hairtree->FindNodeCalled("HAIR");
	if(hairtree==0)
		return 0;

	//VERTICES
	node=hairtree->FindNodeCalled("VERTICES");
	
	int vnum=node->GetNumberOfChildren();

	HairVerticesNumber=vnum;

	if(!(haircoords=(GLfloat**)malloc(vnum*sizeof(GLfloat*))))
	{
		printf("\nERROR: malloc failed on hair vertices\n");
		return 0;
	}

	for(i=0;i<vnum;i++)
	{
		if(!(haircoords[i] = (GLfloat*)malloc(3*sizeof(GLfloat))))
		{
			printf("\nERROR: malloc failed on hair vertex number %d\n", i);
			return 0;
		}
	}

	std::list<XMLGenericTree*>::iterator iter;

	i=0;

	for(iter=node->child.begin();iter!=node->child.end();iter++)
	{
		haircoords[i][0]=atof(((*iter)->GetAttribute("x")).c_str());
		haircoords[i][1]=atof(((*iter)->GetAttribute("y")).c_str());
		haircoords[i][2]=atof(((*iter)->GetAttribute("z")).c_str());
		i++;
	}

	//NORMALS

	node=hairtree->FindNodeCalled("NORMALS");
	
	if(node!=0)
	{
		int vnum=node->GetNumberOfChildren();

		HairNormalsNumber=vnum;

		if(!(hairnormals=(GLfloat**)malloc(vnum*sizeof(GLfloat*))))
		{
			printf("\nERROR: malloc failed on hair normals\n");
			return 0;
		}

		for(i=0;i<vnum;i++)
		{
			if(!(hairnormals[i] = (GLfloat*)malloc(3*sizeof(GLfloat))))
			{
				printf("\nERROR: malloc failed on hair normal number %d\n", i);
				return 0;
			}
		}

		std::list<XMLGenericTree*>::iterator iter;

		i=0;

		for(iter=node->child.begin();iter!=node->child.end();iter++)
		{
			hairnormals[i][0]=atof(((*iter)->GetAttribute("x")).c_str());
			hairnormals[i][1]=atof(((*iter)->GetAttribute("y")).c_str());
			hairnormals[i][2]=atof(((*iter)->GetAttribute("z")).c_str());
			i++;
		}
	}

	//INDICES

	node=hairtree->FindNodeCalled("TRIANGLES");
	
	int tnum=node->GetNumberOfChildren();

	HairIndicesNumber=tnum;

	if(!(hairindices=(int**)malloc(tnum*sizeof(int*))))
	{
		printf("\nERROR: malloc failed on hair indices\n");
		return 0;
	}

	for(i=0;i<tnum;i++)
	{
		if(!(hairindices[i] = (int*)malloc(9*sizeof(int))))
		{
			printf("\nERROR: malloc failed on hair index number %d\n", i);
			return 0;
		}
	}

	i=0;

	for(iter=node->child.begin();iter!=node->child.end();iter++)
	{
		hairindices[i][0]=atoi(((*iter)->GetAttribute("v1")).c_str());
		hairindices[i][1]=atoi(((*iter)->GetAttribute("v2")).c_str());
		hairindices[i][2]=atoi(((*iter)->GetAttribute("v3")).c_str());
		hairindices[i][3]=atoi(((*iter)->GetAttribute("n1")).c_str());
		hairindices[i][4]=atoi(((*iter)->GetAttribute("n2")).c_str());
		hairindices[i][5]=atoi(((*iter)->GetAttribute("n3")).c_str());
		hairindices[i][6]=atoi(((*iter)->GetAttribute("uv1")).c_str());
		hairindices[i][7]=atoi(((*iter)->GetAttribute("uv2")).c_str());
		hairindices[i][8]=atoi(((*iter)->GetAttribute("uv3")).c_str());
		i++;
	}

	delete hairtree;

	return 1;
}


GLfloat vdata_l_wrin[WRIN_VERTEX_NUMBER][3] = {
	{3.62233f, -7.18528f, 18.66006f}, {3.43011f, -7.12416f, 18.82252f}, {3.19544f, -7.09546f, 19.10124f}, {3.82339f, -7.24242f, 18.52210f}, 
	{3.92367f, -7.43485f, 18.51370f}, {4.02294f, -7.62780f, 18.51193f}, {4.13654f, -7.78920f, 18.49544f}, {4.20727f, -8.01837f, 18.51115f}, 
	{4.38837f, -8.26754f, 18.44777f}, {4.55994f, -8.51813f, 18.37627f}, {4.72907f, -8.76908f, 18.29861f}, {4.90100f, -9.01996f, 18.21383f}, 
	{5.06655f, -9.26284f, 18.12129f}, {5.23064f, -9.50576f, 18.01853f}, {5.39527f, -9.74875f, 17.91686f}, {5.56223f, -9.99186f, 17.81943f}, 
	{5.73767f, -10.26940f, 17.71565f}, {5.91140f, -10.54678f, 17.60853f}, {6.08594f, -10.82439f, 17.50356f}, {6.26334f, -11.10257f, 17.40522f}, 
	{6.63935f, -11.50910f, 17.22349f}, {6.45235f, -11.30615f, 17.31700f}, {6.82362f, -11.71092f, 17.12306f}, {7.00514f, -11.91140f, 17.01550f}, 
	{7.17353f, -12.13562f, 16.89277f}, {7.33857f, -12.35780f, 16.76196f}, {7.50063f, -12.57803f, 16.62430f}, {7.66113f, -12.79714f, 16.48300f}, 
	{7.99001f, -13.14684f, 16.21286f}, {7.82712f, -12.97333f, 16.35150f}, {8.14919f, -13.31733f, 16.06700f}, {8.30280f, -13.48290f, 15.91025f}, 
	{8.37845f, -13.65096f, 15.77915f}, {8.45267f, -13.81775f, 15.64560f}, {8.52541f, -13.98326f, 15.50961f}, {8.59586f, -14.14692f, 15.37016f}, 
	{8.58244f, -14.32129f, 15.28342f}, {8.56967f, -14.49617f, 15.19534f}, {8.55789f, -14.67177f, 15.08743f}, {8.54734f, -14.84825f, 14.96959f}, 
	{8.50571f, -14.96967f, 14.92225f}, {8.46506f, -15.09170f, 14.88209f}, {8.42384f, -15.21333f, 14.85203f}, {8.37924f, -15.33246f, 14.83102f}, 
	{8.31511f, -15.49198f, 14.79238f}, {8.24803f, -15.64881f, 14.75767f}, {8.18201f, -15.80631f, 14.71906f}, {8.12254f, -15.96915f, 14.67094f}, 
	{7.98902f, -16.10995f, 14.70673f}, {7.85938f, -16.25302f, 14.74775f}, {7.73296f, -16.39791f, 14.79293f}, {7.60965f, -16.54411f, 14.84236f}, 
	{7.47027f, -16.66934f, 14.91842f}, {7.33388f, -16.79554f, 14.99835f}, {7.20038f, -16.92269f, 15.08196f}, {7.06868f, -17.05030f, 15.16783f}
};


GLfloat vdata_l_inf_eyelid[INF_EYELID_VERTEX_NUMBER][3] = {
	{6.84163f, -1.24939f, 16.43475f}, {5.77340f, -1.11209f, 16.18826f}, {4.58309f, -0.78385f, 15.75095f}, {7.64822f, -1.32484f, 16.59006f}, 
	{8.41029f, -1.25247f, 16.57222f}, {9.16479f, -1.20244f, 16.43180f}, {9.97193f, -0.92815f, 15.98165f}, {10.69848f, -0.60907f, 15.41874f}, 
	{10.99592f, -0.35039f, 15.11047f}, {11.26978f, -0.18132f, 14.87708f}, {4.16480f, -0.66766f, 15.65335f}
};


GLfloat vdata_l_sup_eyelid[SUP_EYELID_VERTEX_NUMBER][3] = {
	{5.65094f, 0.86619f, 16.27441f}, {4.49140f, 0.06588f, 15.84330f}, {4.14097f, -0.35903f, 15.77358f}, {6.61725f, 1.16806f, 16.54811f}, 
	{7.48495f, 1.18812f, 16.54940f}, {8.39328f, 1.10218f, 16.45436f}, {9.03748f, 0.98139f, 16.22392f}, {9.54836f, 0.89668f, 16.03794f}, 
	{9.99386f, 0.84202f, 15.91313f}, {10.12234f, 0.69586f, 15.68837f}, {10.48336f, 0.51618f, 15.45944f}, {10.92557f, 0.28228f, 15.15869f}, 
	{11.19098f, 0.03811f, 14.90899f}, {11.28343f, -0.08421f, 14.85166f}
};


Material teeth={"teeth",			//name
						{0.2, 0.2, 0.2, 1.0},	//ambient
						{0.8, 0.8, 0.8, 1.0},	//diffuse
						{1.2, 1.2, 1.2, 1.0},	//specular
						{0.0, 0.0, 0.0, 1.0},	//emission
						{80.0},					//shininess
					 };


Material gum={"gum",			//name
						{0.2, 0.2, 0.2, 1.0},	//ambient
						{0.6, 0.3, 0.3, 1.0},	//diffuse
						{0.8, 0.8, 0.8, 1.0},	//specular
						{0.0, 0.0, 0.0, 1.0},	//emission
						{80.0},					//shininess
					 };


Material tongue_mat={"tongue",			//name
						{0.2, 0.2, 0.2, 1.0},	//ambient
						{0.6, 0.3, 0.3, 1.0},	//diffuse
						{0.2, 0.2, 0.2, 1.0},	//specular
						{0.0, 0.0, 0.0, 1.0},	//emission
						{80.0},					//shininess
					 };

 Material hair={"hair",
						{0.4f,0.4f,0.4f},		//ambient
						{0.3f,0.3f,0.3f}, 	//diffuse
						{0.1f,0.1f,0.1f},	//specular
						{0.0f, 0.0f, 0.0f},		//emission
						{5.0f},				//shininess
					 };
#ifdef UNIX
Material facial_skin={"facial_skin",			//name
						{0.2, 0.2, 0.2, 1.0},	//ambient
						{0.8, 0.6, 0.5, 1.0},//{0.6, 0.6, 0.6, 1.0},//{0.8, 0.6, 0.5, 1.0},	//diffuse
						{0.15, 0.2, 0.15, 1.0},//{0.3, 0.3, 0.3, 1.0},//{0.15, 0.2, 0.15, 1.0},	//specular
						{0.0, 0.0, 0.0, 1.0},	//emission
						{20.0},//{15.0},//{20.0},	//30.0				//shininess
					 };
#else
Material facial_skin={"facial_skin",			//name
						{0.2, 0.2, 0.2, 1.0},	//ambient
						{0.8, 0.6, 0.5, 1.0},//{0.6, 0.6, 0.6, 1.0},//{0.8, 0.6, 0.5, 1.0},	//diffuse
						{0.1, 0.15, 0.1, 1.0},//{0.3, 0.3, 0.3, 1.0},//{0.15, 0.2, 0.15, 1.0},	//specular
						{0.0, 0.0, 0.0, 1.0},	//emission
						{20.0},//{15.0},//{20.0},	//30.0				//shininess
					 };
#endif


//Questi struct contengono i vari materiali

/*
Material eyebrow={"eyebrow",			//name
						{0.2, 0.2, 0.2, 1.0},	//ambient
						{0.3, 0.15, 0.075, 1.0},//{0.4, 0.2, 0.1, 1.0},//{0.5, 0.5, 0.5, 1.0},//{0.4, 0.2, 0.1, 1.0},	//diffuse
						{0.05, 0.1, 0.05, 1.0},//{0.3, 0.3, 0.3, 1.0},//{0.05, 0.1, 0.05, 1.0},	//specular
						{0.0, 0.0, 0.0, 1.0},	//emission
						{20.0},					//shininess
					 };
*/

Material eyebrow={"eyebrow",			//name
						{0.2, 0.2, 0.2, 1.0},	//ambient
						{0.1, 0.1, 0.1, 1.0},//{0.4, 0.2, 0.1, 1.0},//{0.5, 0.5, 0.5, 1.0},//{0.4, 0.2, 0.1, 1.0},	//diffuse
						{0.15, 0.1, 0.15, 1.0},//{0.3, 0.3, 0.3, 1.0},//{0.05, 0.1, 0.05, 1.0},	//specular
						{0.0, 0.0, 0.0, 1.0},	//emission
						{20.0},					//shininess
					 };

Material eye_bulb={"eye_bulb",			//name
						{0.2, 0.2, 0.2, 1.0},	//ambient
						{0.9, 0.9, 0.9, 1.0},	//diffuse
						{1.2, 1.2, 1.2, 1.0},	//specular
						{0.0, 0.0, 0.0, 1.0},	//emission
						{100.0},				//shininess
					 };

Material eye_highlight={"eye_highlight",			//name
						{0.2, 0.2, 0.2, 1.0},	//ambient
						{0.0, 0.0, 0.0, 1.0},	//diffuse
						{1.0, 1.0, 1.0, 1.0},	//specular
						{0.0, 0.0, 0.0, 1.0},	//emission
						{100.0},				//shininess
					 };

Material iris={"iris",			//name
						{0.2, 0.2, 0.2, 1.0},//{0.2, 0.2, 0.2, 1.0},	//ambient
						{0.1, 0.35, 0.2, 1.0},//{0.0, 0.25, 0.125, 1.0},//{0.2, 0.1, 0.0, 1.0},//{0.0, 0.2, 0.1, 1.0},	//diffuse
						{0.9, 0.9, 0.9, 1.0},//{0.8, 0.8, 0.8, 1.0},//{1.2, 1.2, 1.2, 1.0},	//specular
						{0.0, 0.0, 0.0, 1.0},	//emission
						{20.0},//{10.0},					//shininess
					 };
/*
Material iris2={"iris2",			//name
						{0.2, 0.2, 0.2, 1.0},//{0.2, 0.2, 0.2, 1.0},	//ambient
						{0.1, 0.4, 0.5, 1.0},//{0.0, 0.25, 0.125, 1.0},//{0.2, 0.1, 0.0, 1.0},//{0.0, 0.2, 0.1, 1.0},	//diffuse
						{0.9, 0.9, 0.9, 1.0},//{0.8, 0.8, 0.8, 1.0},//{1.2, 1.2, 1.2, 1.0},	//specular
						{0.0, 0.0, 0.0, 1.0},	//emission
						{20.0},//{10.0},					//shininess
					 };
*/

Material pupil={"pupil",			//name
						{0.2, 0.2, 0.2, 1.0},	//ambient
						{0.0, 0.0, 0.0, 1.0},	//diffuse
						{0.5, 0.5, 0.5, 1.0},//{1.2, 1.2, 1.2, 1.0},	//specular
						{0.0, 0.0, 0.0, 1.0},	//emission
						{10.0},//{35.0},					//shininess
					 };

Material lips={"lips",			//name
						{0.2, 0.2, 0.2, 1.0},	//ambient
						{0.5, 0.3, 0.25, 1.0},//{0.5, 0.2, 0.2, 1.0},//{0.5, 0.5, 0.5, 1.0},//{0.5, 0.2, 0.2, 1.0},	//diffuse
						{0.5, 0.5, 0.5, 1.0},//{0.3, 0.3, 0.3, 1.0},//{0.8, 0.8, 0.8, 1.0},	//specular
						{0.0, 0.0, 0.0, 1.0},	//emission
						{90.0},					//shininess
					 };

//these are the internal parts of the face (teeths, tongue, eye bulbs)

//number of polygons for each surface
GLint surface_num_polygons2[12]={
	0, //questo non conta 
	46, //1
	46, //2
	70, //3
	70, //4
	240, //5
	240, //6
	1132,  //7
	1016,  //8
	1196, //9
	1142, //10
	356, //11
};

GLfloat vdata2[SKIN_VERTEX_NUMBER2][3];
GLfloat init_vdata2[SKIN_VERTEX_NUMBER2][3] = {
	{-50.00000f, 50.00000f, -50.00000f}, {-50.00000f, -50.00000f, -50.00000f}, {-50.00000f, -50.00000f, 50.00000f}, {-50.00000f, 50.00000f, 50.00000f}, 
	{50.00000f, -50.00000f, 50.00000f}, {50.00000f, 50.00000f, 50.00000f}, {50.00000f, 50.00000f, -50.00000f}, {50.00000f, -50.00000f, -50.00000f}, 
	{3.54237f, -18.14158f, 17.55738f}, {4.07216f, -18.10248f, 17.27273f}, {3.29192f, -17.54848f, 17.98332f}, {3.85092f, -17.48519f, 17.76744f}, 
	{-2.99193f, -17.54848f, 17.98333f}, {-3.77217f, -18.10248f, 17.27274f}, {-3.24237f, -18.14157f, 17.55738f}, {-3.55093f, -17.48519f, 17.76744f}, 
	{3.20946f, -18.27451f, 17.68047f}, {3.10492f, -17.62398f, 18.02870f}, {2.89374f, -18.31940f, 17.80034f}, {2.74397f, -17.68658f, 18.20482f}, 
	{1.89661f, -18.26576f, 18.10909f}, {2.38200f, -18.31710f, 17.96045f}, {1.74231f, -17.72788f, 18.50313f}, {2.22311f, -17.74343f, 18.38997f}, 
	{1.56597f, -18.20746f, 18.24738f}, {1.50748f, -17.71628f, 18.53595f}, {1.26891f, -18.13342f, 18.31561f}, {1.28779f, -17.74402f, 18.56462f}, 
	{0.76916f, -18.20894f, 18.42057f}, {0.81848f, -17.71106f, 18.59563f}, {0.43069f, -18.14553f, 18.40952f}, {0.41125f, -17.68893f, 18.55545f}, 
	{0.15000f, -18.09496f, 18.38967f}, {0.15000f, -17.64924f, 18.53394f}, {-2.80492f, -17.62398f, 18.02870f}, {-2.90947f, -18.27451f, 17.68048f}, 
	{-2.44398f, -17.68658f, 18.20483f}, {-2.59374f, -18.31939f, 17.80034f}, {-1.44232f, -17.72788f, 18.50313f}, {-2.08201f, -18.31710f, 17.96045f}, 
	{-1.59662f, -18.26576f, 18.10909f}, {-1.92312f, -17.74343f, 18.38997f}, {-1.20749f, -17.71628f, 18.53595f}, {-1.26598f, -18.20746f, 18.24739f}, 
	{-0.98780f, -17.74402f, 18.56462f}, {-0.96892f, -18.13342f, 18.31561f}, {-0.51848f, -17.71106f, 18.59562f}, {-0.46916f, -18.20894f, 18.42057f}, 
	{-0.11126f, -17.68893f, 18.55545f}, {-0.13069f, -18.14553f, 18.40952f}, {0.42357f, -16.60900f, 16.55384f}, {0.37089f, -16.31059f, 16.90711f}, 
	{0.63392f, -16.36031f, 16.78937f}, {1.22454f, -16.60694f, 16.43974f}, {1.28939f, -16.36629f, 16.71586f}, {1.52094f, -16.42122f, 16.57641f}, 
	{1.97868f, -16.58514f, 16.06319f}, {2.11403f, -16.35693f, 16.33142f}, {2.31877f, -16.36529f, 16.08204f}, {2.78094f, -16.61401f, 15.44654f}, 
	{2.99412f, -16.30110f, 15.69922f}, {3.15821f, -16.34820f, 15.37847f}, {0.38363f, -16.57742f, 18.24566f}, {0.38363f, -17.44695f, 18.24289f}, 
	{0.15000f, -16.45943f, 18.13208f}, {0.15000f, -17.32896f, 18.12930f}, {0.33188f, -16.57782f, 18.37047f}, {0.15000f, -16.45978f, 18.24176f}, 
	{0.75048f, -16.82393f, 18.26705f}, {0.75048f, -17.69347f, 18.26427f}, {0.75669f, -16.82449f, 18.44023f}, {1.17385f, -16.60761f, 18.21385f}, 
	{1.17385f, -17.47714f, 18.21107f}, {1.23694f, -16.60799f, 18.33066f}, {1.33752f, -16.41579f, 17.95684f}, {1.33752f, -17.28533f, 17.95406f}, 
	{1.39012f, -16.41647f, 18.16675f}, {1.69466f, -16.60028f, 18.10502f}, {1.69466f, -17.46981f, 18.10224f}, {1.68924f, -16.60073f, 18.24516f}, 
	{2.08683f, -16.76619f, 18.01254f}, {2.08683f, -17.63572f, 18.00977f}, {2.15050f, -16.76669f, 18.16955f}, {2.38360f, -16.56311f, 17.84050f}, 
	{2.38360f, -17.43265f, 17.83773f}, {2.47919f, -16.56341f, 17.93484f}, {2.58239f, -16.38104f, 17.50541f}, {2.58239f, -17.25057f, 17.50263f}, 
	{2.64798f, -16.38162f, 17.68893f}, {2.90046f, -16.45262f, 17.58996f}, {2.90046f, -17.32215f, 17.58719f}, {2.90582f, -16.45310f, 17.74192f}, 
	{3.51210f, -16.59643f, 17.40942f}, {3.51210f, -17.46596f, 17.40664f}, {3.62692f, -16.59685f, 17.54191f}, {3.68867f, -16.38919f, 17.01673f}, 
	{3.68867f, -17.25871f, 17.01395f}, {3.82552f, -16.38931f, 17.05469f}, {3.73409f, -16.28485f, 16.65846f}, {3.73409f, -17.15437f, 16.65568f}, 
	{3.86475f, -16.28530f, 16.80246f}, {4.12360f, -16.32315f, 16.62295f}, {4.12360f, -17.19268f, 16.62017f}, {4.16444f, -16.32362f, 16.76964f}, 
	{4.67128f, -16.45077f, 16.36154f}, {4.67128f, -17.32030f, 16.35877f}, {4.80658f, -16.45111f, 16.47022f}, {4.79025f, -16.29178f, 15.86157f}, 
	{4.79025f, -17.16131f, 15.85880f}, {4.94084f, -16.29173f, 15.84637f}, {4.54159f, -16.11806f, 15.34328f}, {4.54159f, -16.98760f, 15.34051f}, 
	{4.76138f, -16.11842f, 15.45307f}, {5.00681f, -16.20505f, 15.23809f}, {5.00681f, -17.07458f, 15.23531f}, {5.08201f, -16.20546f, 15.36621f}, 
	{5.44263f, -16.53272f, 14.74993f}, {5.44263f, -17.40226f, 14.74715f}, {5.61096f, -16.53291f, 14.80721f}, {5.41955f, -16.35474f, 14.30175f}, 
	{5.41955f, -17.22428f, 14.29897f}, {5.56340f, -16.35462f, 14.26324f}, {5.19228f, -16.27120f, 13.83381f}, {5.19228f, -17.14073f, 13.83103f}, 
	{5.40518f, -16.27147f, 13.91975f}, {5.61426f, -16.35001f, 13.68429f}, {5.61426f, -17.21954f, 13.68152f}, {5.67927f, -16.35043f, 13.81672f}, 
	{6.10339f, -16.51748f, 13.37818f}, {6.10339f, -17.38701f, 13.37541f}, {6.22304f, -16.51780f, 13.48003f}, {6.35518f, -16.58334f, 12.78052f}, 
	{6.35518f, -17.45287f, 12.77775f}, {6.50549f, -16.58346f, 12.81655f}, {6.40058f, -16.54804f, 12.14936f}, {6.40058f, -17.41758f, 12.14659f}, 
	{6.55208f, -16.54800f, 12.13716f}, {6.30695f, -16.32246f, 11.74448f}, {6.30695f, -17.19199f, 11.74170f}, {6.44025f, -16.32229f, 11.69223f}, 
	{6.01675f, -16.19107f, 11.22928f}, {6.01675f, -17.06060f, 11.22650f}, {6.22921f, -16.19129f, 11.29983f}, {6.41997f, -16.25047f, 11.03417f}, 
	{6.41997f, -17.12000f, 11.03139f}, {6.51647f, -16.25086f, 11.15704f}, {6.69722f, -16.21441f, 10.70849f}, {6.69722f, -17.08394f, 10.70571f}, 
	{6.84497f, -16.21460f, 10.76959f}, {6.78282f, -16.24878f, 9.66669f}, {6.78282f, -17.11831f, 9.66392f}, {6.93687f, -16.24872f, 9.64696f}, 
	{6.64388f, -16.27314f, 9.26979f}, {6.64388f, -17.14267f, 9.26701f}, {6.76961f, -16.27282f, 9.16890f}, {6.02454f, -16.28072f, 8.88532f}, 
	{6.02454f, -17.15025f, 8.88254f}, {6.08554f, -16.28027f, 8.74436f}, {5.54178f, -16.36358f, 8.75665f}, {5.54178f, -17.23311f, 8.75387f}, 
	{5.55708f, -16.36309f, 8.60368f}, {5.03447f, -16.37637f, 8.78624f}, {5.03447f, -17.24590f, 8.78347f}, {4.96288f, -16.37590f, 8.63853f}, 
	{4.66846f, -16.52002f, 9.20094f}, {4.66846f, -17.38955f, 9.19816f}, {4.52683f, -16.51981f, 9.13771f}, {4.54240f, -16.75300f, 10.26795f}, 
	{4.54240f, -17.62253f, 10.26517f}, {4.38744f, -16.75299f, 10.26483f}, {4.55758f, -16.71406f, 10.52742f}, {4.55758f, -17.58359f, 10.52464f}, 
	{4.41086f, -16.71414f, 10.55099f}, {4.69044f, -16.54728f, 11.01763f}, {4.69044f, -17.41681f, 11.01485f}, {4.51462f, -16.54713f, 10.97218f}, 
	{4.42659f, -16.64519f, 11.29437f}, {4.42659f, -17.51472f, 11.29159f}, {4.29507f, -16.64493f, 11.21590f}, {4.30416f, -16.67392f, 11.63252f}, 
	{4.30416f, -17.54345f, 11.62974f}, {4.15812f, -16.67378f, 11.59003f}, {4.19864f, -16.70626f, 12.11929f}, {4.19864f, -17.57579f, 12.11652f}, 
	{4.04808f, -16.70617f, 12.09266f}, {4.09840f, -16.59247f, 12.83309f}, {4.09840f, -17.46200f, 12.83031f}, {3.94822f, -16.59247f, 12.83243f}, 
	{4.12608f, -16.51933f, 13.07994f}, {4.12608f, -17.38886f, 13.07716f}, {3.98350f, -16.51942f, 13.10578f}, {4.24518f, -16.37471f, 13.54867f}, 
	{4.24518f, -17.24424f, 13.54589f}, {4.07025f, -16.37447f, 13.47331f}, {4.05939f, -16.39693f, 13.66012f}, {4.05939f, -17.26647f, 13.65734f}, 
	{3.94456f, -16.39659f, 13.55330f}, {3.78581f, -16.40792f, 14.16403f}, {3.78581f, -17.27745f, 14.16125f}, {3.64461f, -16.40773f, 14.10543f}, 
	{3.62565f, -16.41902f, 14.70367f}, {3.62565f, -17.28855f, 14.70089f}, {3.46594f, -16.41903f, 14.70590f}, {3.75165f, -16.32374f, 15.06235f}, 
	{3.75165f, -17.19327f, 15.05957f}, {3.56891f, -16.32356f, 15.00556f}, {3.43042f, -16.37625f, 15.30671f}, {3.43042f, -17.24579f, 15.30393f}, 
	{3.32574f, -16.37589f, 15.19362f}, {3.28340f, -16.34848f, 15.46503f}, {3.28340f, -17.21801f, 15.46225f}, {3.14280f, -16.30123f, 15.74019f}, 
	{3.14280f, -17.17076f, 15.73742f}, {3.12015f, -16.27754f, 16.12644f}, {3.12015f, -17.14707f, 16.12367f}, {2.97670f, -16.27702f, 15.96447f}, 
	{2.63755f, -16.33925f, 16.09340f}, {2.63755f, -17.20879f, 16.09062f}, {2.60686f, -16.33876f, 15.94026f}, {2.41558f, -16.36568f, 16.20405f}, 
	{2.41558f, -17.23521f, 16.20127f}, {2.25450f, -16.35715f, 16.39936f}, {2.25450f, -17.22668f, 16.39659f}, {2.15772f, -16.29986f, 16.88180f}, 
	{2.15772f, -17.16939f, 16.87903f}, {2.04095f, -16.29919f, 16.67411f}, {1.66839f, -16.40964f, 16.69181f}, {1.66839f, -17.27917f, 16.68903f}, 
	{1.67508f, -16.40914f, 16.53410f}, {1.58168f, -16.42167f, 16.71864f}, {1.58168f, -17.29121f, 16.71586f}, {1.39127f, -16.36665f, 16.82942f}, 
	{1.39127f, -17.23619f, 16.82664f}, {1.14153f, -16.32810f, 17.16072f}, {1.14153f, -17.19763f, 17.15794f}, {1.11155f, -16.32742f, 16.94897f}, 
	{0.86273f, -16.34637f, 16.95482f}, {0.86273f, -17.21590f, 16.95205f}, {0.91620f, -16.34589f, 16.80598f}, {0.66194f, -16.36081f, 16.94404f}, 
	{0.66194f, -17.23034f, 16.94126f}, {0.45598f, -16.31100f, 17.03563f}, {0.45598f, -17.18053f, 17.03285f}, {0.15000f, -16.31283f, 17.22315f}, 
	{0.15000f, -17.18236f, 17.22038f}, {0.15000f, -16.31248f, 17.11155f}, {6.80099f, -16.39038f, 11.18962f}, {7.02849f, -16.32058f, 10.76807f}, 
	{7.14663f, -16.38835f, 9.63448f}, {7.13077f, -17.42856f, 11.27769f}, {7.32305f, -17.40587f, 10.83853f}, {7.50063f, -17.46662f, 9.58445f}, 
	{6.61101f, -16.37084f, 11.41688f}, {6.97734f, -17.43253f, 11.58894f}, {6.60694f, -16.56077f, 11.72296f}, {6.87073f, -17.42926f, 11.87769f}, 
	{6.65229f, -16.76245f, 12.13664f}, {6.84773f, -17.45402f, 12.23261f}, {6.62087f, -16.78944f, 12.84949f}, {6.72779f, -17.48849f, 12.94794f}, 
	{6.31035f, -16.67688f, 13.54087f}, {6.54928f, -17.50145f, 13.49907f}, {5.95110f, -16.60293f, 13.89333f}, {6.27637f, -17.50282f, 13.91645f}, 
	{5.76010f, -16.53643f, 14.07339f}, {6.13419f, -17.44699f, 14.14116f}, {5.81368f, -16.66734f, 14.40680f}, {6.04872f, -17.46894f, 14.41084f}, 
	{5.77524f, -16.75740f, 14.93126f}, {5.89528f, -17.48420f, 15.07487f}, {5.43073f, -16.61892f, 15.56846f}, {5.56960f, -17.32085f, 15.62428f}, 
	{5.21646f, -16.48566f, 15.72759f}, {5.38350f, -17.31268f, 15.85830f}, {5.10634f, -16.54465f, 15.98036f}, {5.26222f, -17.31305f, 16.10171f}, 
	{4.95218f, -16.64990f, 16.47534f}, {5.05115f, -17.40389f, 16.61391f}, {4.47652f, -16.60106f, 16.94593f}, {4.72466f, -17.45324f, 16.96280f}, 
	{4.18776f, -16.58426f, 17.12879f}, {4.51835f, -17.49722f, 17.15470f}, {4.04500f, -16.65079f, 17.33375f}, {4.29635f, -17.51252f, 17.41816f}, 
	{3.75026f, -16.77907f, 17.66793f}, {3.21928f, -16.81591f, 17.91075f}, {2.90316f, -16.70914f, 17.96416f}, {2.66883f, -16.76680f, 18.07408f}, 
	{1.77806f, -16.88992f, 18.40067f}, {2.22472f, -16.93445f, 18.28806f}, {1.53571f, -16.83332f, 18.35925f}, {1.32267f, -16.99157f, 18.45149f}, 
	{0.81269f, -17.02906f, 18.53465f}, {0.37309f, -17.05232f, 18.51335f}, {0.15000f, -17.01648f, 18.50326f}, {4.79029f, -16.69029f, 8.44763f}, 
	{4.33918f, -16.74412f, 9.01835f}, {4.22027f, -16.95352f, 10.28300f}, {4.48608f, -17.48560f, 8.01629f}, {3.88728f, -17.41995f, 8.98924f}, 
	{3.77527f, -17.46899f, 10.25540f}, {4.14880f, -17.98211f, 7.65355f}, {3.10442f, -17.89152f, 9.04533f}, {3.10323f, -17.87963f, 10.09082f}, 
	{5.48854f, -16.63360f, 8.29630f}, {5.55062f, -17.52226f, 7.95331f}, {5.45821f, -18.31991f, 7.76702f}, {6.19095f, -16.49228f, 8.41885f}, 
	{6.37287f, -17.46583f, 8.14249f}, {6.94015f, -16.36930f, 8.94058f}, {7.29451f, -17.31352f, 8.94653f}, {0.82271f, -16.59437f, 16.52344f}, 
	{0.59758f, -17.50663f, 15.99260f}, {0.35137f, -17.52198f, 15.99220f}, {0.15000f, -17.53015f, 15.96665f}, {0.15000f, -16.59072f, 16.57453f}, 
	{1.56825f, -16.60824f, 16.35048f}, {1.27039f, -17.49709f, 15.88743f}, {1.00926f, -17.46275f, 15.95593f}, {0.81735f, -17.52735f, 15.96341f}, 
	{1.01348f, -16.57322f, 16.49153f}, {2.45298f, -16.52933f, 15.80598f}, {2.00246f, -17.53938f, 15.49248f}, {1.73897f, -17.52087f, 15.64987f}, 
	{1.46432f, -17.51436f, 15.79678f}, {1.79123f, -16.63624f, 16.21331f}, {3.06128f, -16.62115f, 15.07190f}, {2.59916f, -17.50123f, 14.85000f}, 
	{2.38567f, -17.55714f, 15.09466f}, {2.22891f, -17.57133f, 15.29389f}, {2.68482f, -16.54997f, 15.68552f}, {3.81578f, -17.43547f, 10.57807f}, 
	{3.14501f, -17.88905f, 10.44784f}, {4.22359f, -16.91411f, 10.56863f}, {3.83829f, -17.46568f, 10.93771f}, {3.10002f, -17.96578f, 10.79305f}, 
	{4.26705f, -16.92089f, 10.93118f}, {3.79019f, -17.44482f, 11.20354f}, {3.11507f, -18.01817f, 11.06244f}, {4.16017f, -16.83016f, 11.21846f}, 
	{3.72365f, -17.47191f, 11.47561f}, {3.13530f, -18.06940f, 11.42104f}, {4.01832f, -16.88097f, 11.49864f}, {3.53544f, -17.52769f, 11.91342f}, 
	{3.08055f, -18.09845f, 11.86699f}, {3.87467f, -16.86582f, 11.97311f}, {3.35922f, -17.52832f, 12.66535f}, {2.79434f, -18.20942f, 12.40832f}, 
	{3.75478f, -16.83636f, 12.77088f}, {3.33973f, -17.52654f, 12.98172f}, {2.71234f, -18.22093f, 12.65377f}, {3.76865f, -16.79962f, 13.07859f}, 
	{3.30895f, -17.56738f, 13.27881f}, {2.69597f, -18.19642f, 13.03480f}, {3.78466f, -16.75718f, 13.41767f}, {3.22929f, -17.54265f, 13.63233f}, 
	{2.58315f, -18.19961f, 13.29536f}, {3.61137f, -16.79070f, 13.65561f}, {3.00195f, -17.54308f, 13.93750f}, {2.37983f, -18.23689f, 13.56390f}, 
	{3.38213f, -16.74952f, 14.11414f}, {2.89591f, -17.54367f, 14.29101f}, {2.29779f, -18.27434f, 13.88079f}, {3.27418f, -16.70850f, 14.62102f}, 
	{2.75087f, -17.52730f, 14.60879f}, {2.10424f, -18.29055f, 14.14516f}, {3.25335f, -16.65748f, 14.85624f}, {1.94357f, -18.30028f, 14.31517f}, 
	{1.78236f, -18.29915f, 14.52089f}, {1.68203f, -18.27726f, 14.67638f}, {1.46049f, -18.28354f, 14.88365f}, {1.24536f, -18.27449f, 15.02443f}, 
	{1.10933f, -18.27316f, 15.09856f}, {0.96282f, -18.24938f, 15.16086f}, {0.79299f, -18.25705f, 15.20604f}, {0.66796f, -18.25124f, 15.21815f}, 
	{0.50545f, -18.24238f, 15.26312f}, {0.36254f, -18.23552f, 15.26284f}, {0.15000f, -18.21692f, 15.29126f}, {-0.33392f, -16.36031f, 16.78937f}, 
	{-0.07090f, -16.31059f, 16.90711f}, {-0.12357f, -16.60900f, 16.55384f}, {-1.22095f, -16.42122f, 16.57642f}, {-0.98940f, -16.36629f, 16.71586f}, 
	{-0.92455f, -16.60694f, 16.43974f}, {-2.01878f, -16.36529f, 16.08204f}, {-1.81403f, -16.35693f, 16.33143f}, {-1.67869f, -16.58513f, 16.06320f}, 
	{-2.85821f, -16.34820f, 15.37847f}, {-2.69412f, -16.30110f, 15.69923f}, {-2.48095f, -16.61401f, 15.44655f}, {-0.08364f, -17.44695f, 18.24289f}, 
	{-0.08364f, -16.57742f, 18.24566f}, {-0.03189f, -16.57782f, 18.37047f}, {-0.45048f, -17.69347f, 18.26427f}, {-0.45048f, -16.82393f, 18.26705f}, 
	{-0.45670f, -16.82449f, 18.44023f}, {-0.87385f, -17.47714f, 18.21107f}, {-0.87385f, -16.60761f, 18.21385f}, {-0.93695f, -16.60798f, 18.33066f}, 
	{-1.03753f, -17.28533f, 17.95407f}, {-1.03753f, -16.41579f, 17.95685f}, {-1.09013f, -16.41647f, 18.16676f}, {-1.39467f, -17.46981f, 18.10225f}, 
	{-1.39467f, -16.60028f, 18.10502f}, {-1.38925f, -16.60073f, 18.24516f}, {-1.78684f, -17.63572f, 18.00977f}, {-1.78684f, -16.76619f, 18.01254f}, 
	{-1.85051f, -16.76669f, 18.16956f}, {-2.08361f, -17.43264f, 17.83773f}, {-2.08361f, -16.56311f, 17.84051f}, {-2.17919f, -16.56341f, 17.93484f}, 
	{-2.28239f, -17.25056f, 17.50263f}, {-2.28239f, -16.38103f, 17.50541f}, {-2.34799f, -16.38162f, 17.68893f}, {-2.60046f, -17.32215f, 17.58719f}, 
	{-2.60047f, -16.45261f, 17.58997f}, {-2.60583f, -16.45310f, 17.74192f}, {-3.21210f, -17.46596f, 17.40664f}, {-3.21210f, -16.59642f, 17.40942f}, 
	{-3.32693f, -16.59685f, 17.54191f}, {-3.38868f, -17.25871f, 17.01396f}, {-3.38868f, -16.38918f, 17.01673f}, {-3.52553f, -16.38930f, 17.05470f}, 
	{-3.43410f, -17.15437f, 16.65568f}, {-3.43410f, -16.28484f, 16.65846f}, {-3.56476f, -16.28530f, 16.80246f}, {-3.82360f, -17.19268f, 16.62017f}, 
	{-3.82360f, -16.32315f, 16.62295f}, {-3.86445f, -16.32361f, 16.76965f}, {-4.37128f, -17.32029f, 16.35877f}, {-4.37128f, -16.45077f, 16.36155f}, 
	{-4.50659f, -16.45111f, 16.47023f}, {-4.49025f, -17.16131f, 15.85880f}, {-4.49025f, -16.29178f, 15.86158f}, {-4.64084f, -16.29173f, 15.84638f}, 
	{-4.24160f, -16.98759f, 15.34051f}, {-4.24160f, -16.11806f, 15.34329f}, {-4.46138f, -16.11841f, 15.45308f}, {-4.70681f, -17.07458f, 15.23532f}, 
	{-4.70681f, -16.20504f, 15.23810f}, {-4.78202f, -16.20545f, 15.36622f}, {-5.14264f, -17.40225f, 14.74715f}, {-5.14264f, -16.53272f, 14.74993f}, 
	{-5.31097f, -16.53290f, 14.80721f}, {-5.11955f, -17.22427f, 14.29898f}, {-5.11955f, -16.35474f, 14.30175f}, {-5.26341f, -16.35462f, 14.26324f}, 
	{-4.89228f, -17.14072f, 13.83104f}, {-4.89228f, -16.27119f, 13.83382f}, {-5.10518f, -16.27147f, 13.91975f}, {-5.31427f, -17.21954f, 13.68152f}, 
	{-5.31427f, -16.35001f, 13.68430f}, {-5.37927f, -16.35043f, 13.81673f}, {-5.80340f, -17.38701f, 13.37541f}, {-5.80340f, -16.51747f, 13.37819f}, 
	{-5.92304f, -16.51780f, 13.48004f}, {-6.05518f, -17.45287f, 12.77775f}, {-6.05518f, -16.58334f, 12.78053f}, {-6.20549f, -16.58345f, 12.81655f}, 
	{-6.10058f, -17.41757f, 12.14659f}, {-6.10058f, -16.54804f, 12.14937f}, {-6.25209f, -16.54800f, 12.13717f}, {-6.00695f, -17.19199f, 11.74171f}, 
	{-6.00695f, -16.32245f, 11.74449f}, {-6.14025f, -16.32229f, 11.69224f}, {-5.71675f, -17.06060f, 11.22651f}, {-5.71675f, -16.19106f, 11.22928f}, 
	{-5.92921f, -16.19129f, 11.29983f}, {-6.11997f, -17.11999f, 11.03140f}, {-6.11997f, -16.25046f, 11.03417f}, {-6.21647f, -16.25085f, 11.15705f}, 
	{-6.39722f, -17.08393f, 10.70572f}, {-6.39722f, -16.21440f, 10.70850f}, {-6.54498f, -16.21460f, 10.76960f}, {-6.48282f, -17.11831f, 9.66393f}, 
	{-6.48282f, -16.24878f, 9.66670f}, {-6.63687f, -16.24871f, 9.64697f}, {-6.34389f, -17.14267f, 9.26702f}, {-6.34389f, -16.27314f, 9.26980f}, 
	{-6.46961f, -16.27282f, 9.16891f}, {-5.72454f, -17.15025f, 8.88255f}, {-5.72454f, -16.28072f, 8.88533f}, {-5.78554f, -16.28027f, 8.74437f}, 
	{-5.24178f, -17.23311f, 8.75388f}, {-5.24178f, -16.36358f, 8.75666f}, {-5.25708f, -16.36309f, 8.60368f}, {-4.73447f, -17.24590f, 8.78347f}, 
	{-4.73447f, -16.37637f, 8.78625f}, {-4.66288f, -16.37590f, 8.63853f}, {-4.36846f, -17.38954f, 9.19817f}, {-4.36846f, -16.52001f, 9.20094f}, 
	{-4.22683f, -16.51981f, 9.13772f}, {-4.24240f, -17.62252f, 10.26518f}, {-4.24240f, -16.75299f, 10.26795f}, {-4.08744f, -16.75298f, 10.26484f}, 
	{-4.25758f, -17.58359f, 10.52465f}, {-4.25758f, -16.71406f, 10.52742f}, {-4.11086f, -16.71414f, 10.55100f}, {-4.39044f, -17.41681f, 11.01486f}, 
	{-4.39044f, -16.54728f, 11.01763f}, {-4.21462f, -16.54713f, 10.97219f}, {-4.12660f, -17.51471f, 11.29160f}, {-4.12660f, -16.64519f, 11.29437f}, 
	{-3.99507f, -16.64493f, 11.21590f}, {-4.00417f, -17.54345f, 11.62975f}, {-4.00417f, -16.67391f, 11.63253f}, {-3.85812f, -16.67378f, 11.59003f}, 
	{-3.89864f, -17.57579f, 12.11652f}, {-3.89864f, -16.70625f, 12.11930f}, {-3.74808f, -16.70617f, 12.09267f}, {-3.79841f, -17.46200f, 12.83032f}, 
	{-3.79841f, -16.59247f, 12.83309f}, {-3.64823f, -16.59247f, 12.83243f}, {-3.82608f, -17.38886f, 13.07716f}, {-3.82608f, -16.51933f, 13.07994f}, 
	{-3.68350f, -16.51941f, 13.10579f}, {-3.94519f, -17.24424f, 13.54590f}, {-3.94519f, -16.37471f, 13.54867f}, {-3.77026f, -16.37447f, 13.47331f}, 
	{-3.75940f, -17.26646f, 13.65734f}, {-3.75940f, -16.39693f, 13.66012f}, {-3.64457f, -16.39659f, 13.55331f}, {-3.48582f, -17.27745f, 14.16126f}, 
	{-3.48582f, -16.40792f, 14.16403f}, {-3.34462f, -16.40773f, 14.10543f}, {-3.32566f, -17.28855f, 14.70090f}, {-3.32566f, -16.41902f, 14.70367f}, 
	{-3.16595f, -16.41902f, 14.70590f}, {-3.45166f, -17.19327f, 15.05958f}, {-3.45166f, -16.32374f, 15.06235f}, {-3.26892f, -16.32356f, 15.00556f}, 
	{-3.13043f, -17.24578f, 15.30394f}, {-3.13043f, -16.37625f, 15.30671f}, {-3.02575f, -16.37589f, 15.19362f}, {-2.98341f, -17.21801f, 15.46226f}, 
	{-2.98341f, -16.34848f, 15.46503f}, {-2.84281f, -17.17076f, 15.73742f}, {-2.84281f, -16.30123f, 15.74020f}, {-2.82016f, -17.14707f, 16.12367f}, 
	{-2.82016f, -16.27754f, 16.12645f}, {-2.67670f, -16.27702f, 15.96447f}, {-2.33755f, -17.20878f, 16.09063f}, {-2.33755f, -16.33925f, 16.09341f}, 
	{-2.30687f, -16.33876f, 15.94026f}, {-2.11559f, -17.23521f, 16.20128f}, {-2.11559f, -16.36568f, 16.20405f}, {-1.95451f, -17.22668f, 16.39659f}, 
	{-1.95451f, -16.35714f, 16.39937f}, {-1.85773f, -17.16939f, 16.87903f}, {-1.85773f, -16.29986f, 16.88180f}, {-1.74096f, -16.29919f, 16.67411f}, 
	{-1.36839f, -17.27917f, 16.68904f}, {-1.36839f, -16.40964f, 16.69181f}, {-1.37509f, -16.40914f, 16.53410f}, {-1.28169f, -17.29120f, 16.71586f}, 
	{-1.28169f, -16.42167f, 16.71864f}, {-1.09127f, -17.23619f, 16.82664f}, {-1.09127f, -16.36665f, 16.82942f}, {-0.84154f, -17.19763f, 17.15794f}, 
	{-0.84154f, -16.32810f, 17.16072f}, {-0.81156f, -16.32742f, 16.94897f}, {-0.56274f, -17.21590f, 16.95205f}, {-0.56274f, -16.34637f, 16.95482f}, 
	{-0.61621f, -16.34589f, 16.80598f}, {-0.36195f, -17.23033f, 16.94126f}, {-0.36195f, -16.36081f, 16.94404f}, {-0.15599f, -17.18053f, 17.03285f}, 
	{-0.15599f, -16.31100f, 17.03563f}, {-6.72849f, -16.32058f, 10.76808f}, {-6.50099f, -16.39037f, 11.18963f}, {-6.84664f, -16.38834f, 9.63448f}, 
	{-7.02305f, -17.40587f, 10.83853f}, {-6.83077f, -17.42855f, 11.27770f}, {-7.20063f, -17.46661f, 9.58446f}, {-6.31102f, -16.37083f, 11.41689f}, 
	{-6.67734f, -17.43253f, 11.58894f}, {-6.30694f, -16.56077f, 11.72296f}, {-6.57073f, -17.42926f, 11.87770f}, {-6.35229f, -16.76245f, 12.13665f}, 
	{-6.54774f, -17.45402f, 12.23262f}, {-6.32087f, -16.78943f, 12.84949f}, {-6.42780f, -17.48849f, 12.94795f}, {-6.01036f, -16.67688f, 13.54088f}, 
	{-6.24929f, -17.50144f, 13.49908f}, {-5.65110f, -16.60292f, 13.89334f}, {-5.97638f, -17.50281f, 13.91646f}, {-5.46010f, -16.53643f, 14.07339f}, 
	{-5.83420f, -17.44699f, 14.14117f}, {-5.51369f, -16.66734f, 14.40681f}, {-5.74873f, -17.46893f, 14.41085f}, {-5.47524f, -16.75740f, 14.93127f}, 
	{-5.59528f, -17.48419f, 15.07488f}, {-5.13074f, -16.61892f, 15.56847f}, {-5.26961f, -17.32085f, 15.62429f}, {-4.91646f, -16.48566f, 15.72759f}, 
	{-5.08351f, -17.31268f, 15.85830f}, {-4.80635f, -16.54465f, 15.98037f}, {-4.96222f, -17.31304f, 16.10171f}, {-4.65219f, -16.64989f, 16.47535f}, 
	{-4.75116f, -17.40388f, 16.61391f}, {-4.17652f, -16.60106f, 16.94594f}, {-4.42467f, -17.45324f, 16.96280f}, {-3.88777f, -16.58426f, 17.12879f}, 
	{-4.21836f, -17.49722f, 17.15470f}, {-3.74501f, -16.65079f, 17.33376f}, {-3.99636f, -17.51252f, 17.41817f}, {-3.45026f, -16.77907f, 17.66793f}, 
	{-2.91929f, -16.81590f, 17.91075f}, {-2.60317f, -16.70914f, 17.96416f}, {-2.36884f, -16.76680f, 18.07409f}, {-1.92473f, -16.93445f, 18.28806f}, 
	{-1.47807f, -16.88992f, 18.40067f}, {-1.23572f, -16.83332f, 18.35925f}, {-1.02268f, -16.99157f, 18.45149f}, {-0.51270f, -17.02906f, 18.53465f}, 
	{-0.07309f, -17.05232f, 18.51335f}, {-4.03918f, -16.74412f, 9.01835f}, {-4.49029f, -16.69029f, 8.44763f}, {-3.92027f, -16.95352f, 10.28300f}, 
	{-3.58728f, -17.41994f, 8.98925f}, {-4.18608f, -17.48560f, 8.01630f}, {-3.47527f, -17.46898f, 10.25540f}, {-2.80442f, -17.89152f, 9.04534f}, 
	{-3.84880f, -17.98210f, 7.65355f}, {-2.80323f, -17.87962f, 10.09082f}, {-5.18854f, -16.63360f, 8.29631f}, {-5.25062f, -17.52226f, 7.95332f}, 
	{-5.15821f, -18.31991f, 7.76703f}, {-5.89095f, -16.49228f, 8.41886f}, {-6.07287f, -17.46583f, 8.14249f}, {-6.64015f, -16.36930f, 8.94059f}, 
	{-6.99452f, -17.31352f, 8.94654f}, {-0.52271f, -16.59437f, 16.52344f}, {-0.29759f, -17.50663f, 15.99260f}, {-0.05137f, -17.52198f, 15.99220f}, 
	{-1.26826f, -16.60824f, 16.35049f}, {-0.97040f, -17.49709f, 15.88743f}, {-0.70926f, -17.46275f, 15.95593f}, {-0.51736f, -17.52735f, 15.96341f}, 
	{-0.71349f, -16.57322f, 16.49153f}, {-2.15299f, -16.52932f, 15.80599f}, {-1.70247f, -17.53938f, 15.49248f}, {-1.43897f, -17.52087f, 15.64987f}, 
	{-1.16433f, -17.51436f, 15.79678f}, {-1.49124f, -16.63624f, 16.21331f}, {-2.76129f, -16.62115f, 15.07190f}, {-2.29917f, -17.50123f, 14.85001f}, 
	{-2.08567f, -17.55714f, 15.09467f}, {-1.92892f, -17.57133f, 15.29390f}, {-2.38483f, -16.54997f, 15.68553f}, {-2.84502f, -17.88904f, 10.44785f}, 
	{-3.51578f, -17.43547f, 10.57807f}, {-3.92359f, -16.91410f, 10.56863f}, {-2.80003f, -17.96577f, 10.79306f}, {-3.53829f, -17.46568f, 10.93772f}, 
	{-3.96705f, -16.92089f, 10.93119f}, {-2.81507f, -18.01817f, 11.06244f}, {-3.49019f, -17.44482f, 11.20355f}, {-3.86018f, -16.83016f, 11.21846f}, 
	{-2.83530f, -18.06940f, 11.42104f}, {-3.42365f, -17.47191f, 11.47562f}, {-3.71832f, -16.88097f, 11.49864f}, {-2.78055f, -18.09845f, 11.86699f}, 
	{-3.23544f, -17.52769f, 11.91342f}, {-3.57467f, -16.86582f, 11.97312f}, {-2.49434f, -18.20942f, 12.40832f}, {-3.05922f, -17.52832f, 12.66535f}, 
	{-3.45479f, -16.83636f, 12.77089f}, {-2.41235f, -18.22093f, 12.65377f}, {-3.03973f, -17.52654f, 12.98172f}, {-3.46865f, -16.79962f, 13.07860f}, 
	{-2.39598f, -18.19642f, 13.03480f}, {-3.00895f, -17.56738f, 13.27881f}, {-3.48466f, -16.75718f, 13.41767f}, {-2.28315f, -18.19961f, 13.29536f}, 
	{-2.92929f, -17.54265f, 13.63234f}, {-3.31138f, -16.79070f, 13.65561f}, {-2.07983f, -18.23689f, 13.56390f}, {-2.70196f, -17.54308f, 13.93750f}, 
	{-3.08214f, -16.74952f, 14.11414f}, {-1.99779f, -18.27433f, 13.88079f}, {-2.59591f, -17.54367f, 14.29101f}, {-2.97419f, -16.70850f, 14.62102f}, 
	{-1.80425f, -18.29055f, 14.14516f}, {-2.45087f, -17.52730f, 14.60879f}, {-2.95335f, -16.65748f, 14.85624f}, {-1.64357f, -18.30028f, 14.31517f}, 
	{-1.48236f, -18.29915f, 14.52089f}, {-1.38204f, -18.27726f, 14.67638f}, {-1.16049f, -18.28354f, 14.88365f}, {-0.94537f, -18.27449f, 15.02443f}, 
	{-0.80934f, -18.27316f, 15.09856f}, {-0.66282f, -18.24938f, 15.16086f}, {-0.49299f, -18.25705f, 15.20604f}, {-0.36797f, -18.25124f, 15.21815f}, 
	{-0.20545f, -18.24238f, 15.26312f}, {-0.06255f, -18.23552f, 15.26284f}, {-4.15219f, -18.24679f, 8.52454f}, {-3.38373f, -17.88536f, 10.25603f}, 
	{-3.99798f, -17.32182f, 10.02677f}, {-4.90094f, -17.69812f, 8.26015f}, {-4.79812f, -18.56006f, 7.16806f}, {-5.61465f, -18.05094f, 6.88910f}, 
	{3.99798f, -17.32182f, 10.02677f}, {3.38373f, -17.88536f, 10.25603f}, {4.15219f, -18.24679f, 8.52454f}, {4.90094f, -17.69812f, 8.26015f}, 
	{4.79812f, -18.56006f, 7.16806f}, {5.61465f, -18.05094f, 6.88910f}, {-2.38968f, -17.23402f, 13.95009f}, {-1.87577f, -17.65988f, 14.01223f}, 
	{-1.70777f, -17.37707f, 15.15309f}, {-1.97221f, -17.03255f, 15.40980f}, {-2.79147f, -16.70857f, 13.92613f}, {-2.25463f, -16.52979f, 15.54165f}, 
	{-2.95444f, -16.12689f, 13.84766f}, {-2.37391f, -15.97736f, 15.51029f}, {-2.14191f, -17.98998f, 12.34077f}, {-2.80253f, -17.53455f, 12.16494f}, 
	{-3.32058f, -16.98207f, 12.00341f}, {-3.53686f, -16.36480f, 11.88411f}, {-2.57669f, -18.34995f, 10.49305f}, {-4.23989f, -16.69162f, 9.88889f}, 
	{-5.07743f, -17.09378f, 8.08457f}, {-5.93912f, -17.47902f, 6.70643f}, {-5.59284f, -18.34263f, 5.85518f}, {-5.89887f, -17.80721f, 5.65110f}, 
	{-4.81749f, -16.48851f, 8.02933f}, {-4.02704f, -16.05681f, 9.82733f}, {-4.10255f, -15.92850f, 8.12392f}, {-3.41129f, -15.48314f, 9.86457f}, 
	{-3.15676f, -15.55604f, 8.27827f}, {-2.59509f, -15.10734f, 9.96450f}, {-5.61558f, -16.88809f, 6.68950f}, {-4.79842f, -16.35889f, 6.79222f}, 
	{-3.75893f, -16.00313f, 6.95522f}, {-5.59218f, -17.25644f, 5.66220f}, {-4.85736f, -16.75780f, 5.85198f}, {-4.07588f, -16.40378f, 6.13219f}, 
	{-2.25647f, -15.44011f, 15.34572f}, {-2.80093f, -15.54172f, 13.72303f}, {-1.97287f, -14.99641f, 15.04570f}, {-2.39501f, -15.03744f, 13.57751f}, 
	{-1.70324f, -14.75098f, 14.68427f}, {-1.87291f, -14.72021f, 13.50651f}, {-3.35210f, -15.73774f, 11.79771f}, {-2.83742f, -15.18346f, 11.76151f}, 
	{-2.15863f, -14.82195f, 11.80359f}, {1.27868f, -17.44512f, 15.61951f}, {1.70777f, -17.37707f, 15.15309f}, {1.97221f, -17.03255f, 15.40980f}, 
	{1.42975f, -16.98882f, 16.33739f}, {2.25463f, -16.52979f, 15.54165f}, {1.61352f, -16.45142f, 16.66626f}, {2.37391f, -15.97736f, 15.51029f}, 
	{1.69444f, -15.89968f, 16.69381f}, {0.67443f, -17.51274f, 15.99902f}, {0.75131f, -16.99547f, 16.90163f}, {0.84485f, -16.42643f, 17.34457f}, 
	{0.88703f, -15.86677f, 17.41125f}, {0.00000f, -17.54030f, 16.13829f}, {0.00000f, -17.00320f, 17.09389f}, {0.00000f, -16.42240f, 17.57475f}, 
	{0.00000f, -15.85997f, 17.65550f}, {-0.75131f, -16.99547f, 16.90163f}, {-0.67443f, -17.51274f, 15.99902f}, {-0.84485f, -16.42643f, 17.34457f}, 
	{-0.88703f, -15.86677f, 17.41125f}, {-1.42975f, -16.98882f, 16.33739f}, {-1.27868f, -17.44512f, 15.61951f}, {-1.61352f, -16.45142f, 16.66626f}, 
	{-1.69444f, -15.89968f, 16.69381f}, {0.00000f, -15.39130f, 17.38917f}, {-0.84511f, -15.38305f, 17.15891f}, {0.00000f, -15.05759f, 16.76505f}, 
	{-0.75471f, -15.01496f, 16.56334f}, {0.00000f, -14.87550f, 15.71254f}, {-0.68103f, -14.78999f, 15.54837f}, {-1.61337f, -15.39232f, 16.47833f}, 
	{-1.42773f, -14.98067f, 15.98804f}, {-1.27029f, -14.72178f, 15.15068f}, {1.61337f, -15.39232f, 16.47833f}, {2.25647f, -15.44011f, 15.34572f}, 
	{1.42773f, -14.98067f, 15.98804f}, {1.97287f, -14.99641f, 15.04570f}, {1.27029f, -14.72178f, 15.15068f}, {1.70324f, -14.75098f, 14.68427f}, 
	{0.84511f, -15.38305f, 17.15891f}, {0.75471f, -15.01496f, 16.56334f}, {0.68103f, -14.78999f, 15.54837f}, {1.29200f, -14.62290f, 13.73939f}, 
	{1.87291f, -14.72022f, 13.50651f}, {1.44873f, -14.70374f, 11.95660f}, {2.15863f, -14.82195f, 11.80359f}, {1.74034f, -14.98589f, 10.08093f}, 
	{2.59509f, -15.10734f, 9.96450f}, {0.67812f, -14.70791f, 14.00047f}, {0.74758f, -14.79603f, 12.12504f}, {0.89005f, -15.07564f, 10.18035f}, 
	{0.00000f, -14.82846f, 14.12811f}, {0.00000f, -14.92851f, 12.21186f}, {0.00000f, -15.19979f, 10.22735f}, {-0.67812f, -14.70791f, 14.00047f}, 
	{-0.74758f, -14.79603f, 12.12504f}, {-0.89005f, -15.07564f, 10.18035f}, {-1.29200f, -14.62290f, 13.73939f}, {-1.44873f, -14.70374f, 11.95660f}, 
	{-1.74034f, -14.98589f, 10.08093f}, {0.00000f, -15.64998f, 8.42366f}, {-1.10094f, -15.52520f, 8.40818f}, {0.00000f, -16.28705f, 6.88474f}, 
	{-1.36986f, -16.03899f, 6.88905f}, {0.00000f, -16.86212f, 5.67617f}, {-1.70714f, -16.62627f, 5.71490f}, {-2.14823f, -15.44177f, 8.36799f}, 
	{-2.63823f, -15.93775f, 6.93219f}, {-3.15006f, -16.47928f, 5.88104f}, {2.14823f, -15.44177f, 8.36799f}, {3.15676f, -15.55604f, 8.27827f}, 
	{2.63823f, -15.93775f, 6.93219f}, {3.75893f, -16.00313f, 6.95522f}, {3.15006f, -16.47928f, 5.88104f}, {4.07588f, -16.40378f, 6.13218f}, 
	{1.10094f, -15.52520f, 8.40818f}, {1.36986f, -16.03899f, 6.88905f}, {1.70714f, -16.62627f, 5.71490f}, {3.85045f, -16.98692f, 5.23556f}, 
	{4.85736f, -16.75780f, 5.85198f}, {4.46596f, -17.51022f, 4.89983f}, {5.59218f, -17.25644f, 5.66220f}, {4.72528f, -18.04198f, 4.84507f}, 
	{5.89887f, -17.80721f, 5.65110f}, {2.10864f, -17.17936f, 4.91031f}, {2.45275f, -17.71117f, 4.46010f}, {2.59234f, -18.16822f, 4.36647f}, 
	{0.00000f, -17.39967f, 4.83528f}, {0.00000f, -17.88812f, 4.35840f}, {0.00000f, -18.22843f, 4.25073f}, {-2.10864f, -17.17936f, 4.91031f}, 
	{-2.45275f, -17.71117f, 4.46010f}, {-2.59234f, -18.16822f, 4.36647f}, {-3.85045f, -16.98692f, 5.23556f}, {-4.46596f, -17.51022f, 4.89983f}, 
	{-4.72528f, -18.04198f, 4.84507f}, {-2.44741f, -18.69632f, 4.64328f}, {0.00000f, -18.73507f, 4.52903f}, {-4.46705f, -18.56721f, 5.09012f}, 
	{5.59284f, -18.34263f, 5.85518f}, {4.46705f, -18.56721f, 5.09012f}, {2.44741f, -18.69632f, 4.64328f}, {-0.72246f, -18.50592f, 12.72255f}, 
	{-0.86420f, -18.89883f, 10.77886f}, {0.00000f, -18.97011f, 10.81281f}, {0.00000f, -18.57196f, 12.78983f}, {-0.65647f, -18.06430f, 14.55641f}, 
	{0.00000f, -18.11760f, 14.65921f}, {-1.43856f, -18.31106f, 12.55367f}, {-1.72495f, -18.68974f, 10.67296f}, {-1.28693f, -17.91231f, 14.30273f}, 
	{1.72495f, -18.68974f, 10.67296f}, {2.57669f, -18.34995f, 10.49305f}, {2.14191f, -17.98998f, 12.34077f}, {1.43856f, -18.31106f, 12.55367f}, 
	{1.87577f, -17.65988f, 14.01223f}, {1.28693f, -17.91231f, 14.30273f}, {0.86420f, -18.89883f, 10.77886f}, {0.72246f, -18.50592f, 12.72255f}, 
	{0.65647f, -18.06430f, 14.55641f}, {2.38968f, -17.23402f, 13.95009f}, {2.80253f, -17.53455f, 12.16494f}, {2.79147f, -16.70857f, 13.92613f}, 
	{3.32058f, -16.98207f, 12.00341f}, {2.95444f, -16.12689f, 13.84766f}, {3.53686f, -16.36480f, 11.88411f}, {4.23989f, -16.69162f, 9.88889f}, 
	{2.80093f, -15.54172f, 13.72303f}, {3.35210f, -15.73774f, 11.79771f}, {4.02704f, -16.05681f, 9.82733f}, {2.39501f, -15.03744f, 13.57751f}, 
	{2.83742f, -15.18345f, 11.76151f}, {3.41129f, -15.48314f, 9.86457f}, {4.81749f, -16.48851f, 8.02933f}, {5.07743f, -17.09378f, 8.08457f}, 
	{5.61558f, -16.88809f, 6.68950f}, {5.93912f, -17.47902f, 6.70643f}, {4.10255f, -15.92850f, 8.12392f}, {4.79842f, -16.35889f, 6.79222f}, 
	{-8.54481f, 0.37040f, 16.20614f}, {-8.58470f, 0.01055f, 16.20651f}, {-7.76182f, 0.10900f, 16.37233f}, {-7.73406f, 0.19830f, 16.36826f}, 
	{-7.72735f, 0.26210f, 16.37364f}, {-7.20000f, 0.02937f, 15.73995f}, {-8.41299f, 0.70770f, 16.20579f}, {-7.67787f, 0.34128f, 16.36953f}, 
	{-7.65637f, 0.39985f, 16.37483f}, {-8.19822f, 0.99933f, 16.20550f}, {-7.58793f, 0.46421f, 16.37066f}, {-7.55437f, 0.51369f, 16.37583f}, 
	{-7.91521f, 1.22538f, 16.20529f}, {-7.47052f, 0.55904f, 16.37155f}, {-7.42862f, 0.59697f, 16.37657f}, {-7.58283f, 1.37074f, 16.20515f}, 
	{-7.33385f, 0.61975f, 16.37215f}, {-7.28748f, 0.64601f, 16.37667f}, {-7.20000f, 1.42458f, 16.20474f}, {-7.17500f, 0.63670f, 16.37294f}, 
	{-7.06252f, 0.64601f, 16.37667f}, {-6.81717f, 1.37074f, 16.20515f}, {-6.92138f, 0.59697f, 16.37657f}, {-6.48479f, 1.22538f, 16.20528f}, 
	{-7.01615f, 0.61975f, 16.37215f}, {-6.79563f, 0.51369f, 16.37583f}, {-6.20178f, 0.99933f, 16.20550f}, {-6.87948f, 0.55904f, 16.37155f}, 
	{-6.69363f, 0.39985f, 16.37484f}, {-5.98701f, 0.70770f, 16.20579f}, {-6.76207f, 0.46421f, 16.37066f}, {-6.62264f, 0.26210f, 16.37364f}, 
	{-5.85519f, 0.37040f, 16.20614f}, {-6.67212f, 0.34128f, 16.36953f}, {-6.58818f, 0.10900f, 16.37233f}, {-5.81530f, 0.01055f, 16.20651f}, 
	{-6.61594f, 0.19830f, 16.36826f}, {-6.59355f, -0.04940f, 16.37098f}, {-5.86999f, -0.34722f, 16.20689f}, {-6.59767f, 0.04481f, 16.36691f}, 
	{-6.63941f, -0.20212f, 16.36970f}, {-6.01542f, -0.67850f, 16.20725f}, {-6.61890f, -0.10872f, 16.36558f}, {-6.72336f, -0.33809f, 16.36856f}, 
	{-6.24149f, -0.96076f, 16.20754f}, {-6.67848f, -0.25167f, 16.36436f}, {-6.84014f, -0.44718f, 16.36765f}, {-6.53261f, -1.17345f, 16.20791f}, 
	{-6.77252f, -0.37398f, 16.36334f}, {-6.98012f, -0.52020f, 16.36703f}, {-6.86019f, -1.27986f, 16.21619f}, {-6.89466f, -0.46712f, 16.36254f}, 
	{-7.17500f, -0.55270f, 16.37069f}, {-7.20000f, -1.32557f, 16.22152f}, {-7.03703f, -0.52215f, 16.36322f}, {-7.53981f, -1.27986f, 16.21619f}, 
	{-7.31296f, -0.52215f, 16.36322f}, {-7.36988f, -0.52020f, 16.36703f}, {-7.86739f, -1.17345f, 16.20791f}, {-7.45534f, -0.46712f, 16.36254f}, 
	{-7.50986f, -0.44718f, 16.36765f}, {-8.15851f, -0.96076f, 16.20754f}, {-7.57748f, -0.37398f, 16.36334f}, {-7.62664f, -0.33809f, 16.36856f}, 
	{-8.38458f, -0.67850f, 16.20725f}, {-7.67152f, -0.25167f, 16.36436f}, {-7.71059f, -0.20212f, 16.36970f}, {-8.53001f, -0.34722f, 16.20689f}, 
	{-7.73110f, -0.10872f, 16.36558f}, {-7.75645f, -0.04940f, 16.37098f}, {-7.75233f, 0.04481f, 16.36691f}, {7.80227f, 0.08325f, 16.37233f}, 
	{8.58470f, 0.01055f, 16.20651f}, {8.54481f, 0.37040f, 16.20614f}, {7.77377f, 0.17140f, 16.36826f}, {7.76689f, 0.23438f, 16.37364f}, 
	{7.17500f, 0.00437f, 15.73995f}, {8.41299f, 0.70770f, 16.20579f}, {7.71611f, 0.31255f, 16.36953f}, {7.69404f, 0.37036f, 16.37483f}, 
	{8.19822f, 0.99933f, 16.20550f}, {7.62380f, 0.43390f, 16.37066f}, {7.58935f, 0.48275f, 16.37583f}, {7.91521f, 1.22538f, 16.20529f}, 
	{7.50329f, 0.52752f, 16.37155f}, {7.46029f, 0.56496f, 16.37657f}, {7.58283f, 1.37074f, 16.20515f}, {7.36303f, 0.58745f, 16.37215f}, 
	{7.31544f, 0.61337f, 16.37667f}, {7.20000f, 1.42458f, 16.20474f}, {7.20000f, 0.60418f, 16.37294f}, {6.81717f, 1.37074f, 16.20515f}, 
	{7.08456f, 0.61337f, 16.37667f}, {6.48479f, 1.22538f, 16.20528f}, {6.93971f, 0.56496f, 16.37657f}, {7.03697f, 0.58745f, 16.37215f}, 
	{6.20178f, 0.99933f, 16.20550f}, {6.81065f, 0.48275f, 16.37583f}, {6.89671f, 0.52752f, 16.37155f}, {5.98701f, 0.70770f, 16.20579f}, 
	{6.70596f, 0.37036f, 16.37483f}, {6.77621f, 0.43390f, 16.37066f}, {5.85519f, 0.37040f, 16.20614f}, {6.63311f, 0.23438f, 16.37364f}, 
	{6.68389f, 0.31255f, 16.36953f}, {5.81530f, 0.01055f, 16.20651f}, {6.59773f, 0.08325f, 16.37233f}, {6.62623f, 0.17140f, 16.36826f}, 
	{5.86999f, -0.34722f, 16.20689f}, {6.60325f, -0.07312f, 16.37098f}, {6.60748f, 0.01988f, 16.36691f}, {6.01542f, -0.67850f, 16.20725f}, 
	{6.65031f, -0.22388f, 16.36970f}, {6.62927f, -0.13169f, 16.36558f}, {6.24149f, -0.96076f, 16.20754f}, {6.73648f, -0.35811f, 16.36856f}, 
	{6.69042f, -0.27280f, 16.36436f}, {6.53261f, -1.17345f, 16.20791f}, {6.85633f, -0.46581f, 16.36765f}, {6.78693f, -0.39355f, 16.36334f}, 
	{6.86019f, -1.27986f, 16.21619f}, {6.99999f, -0.53789f, 16.36703f}, {6.91228f, -0.48549f, 16.36254f}, {7.20000f, -1.32557f, 16.22152f}, 
	{7.20000f, -0.56965f, 16.37069f}, {7.05840f, -0.53981f, 16.36322f}, {7.53981f, -1.27986f, 16.21619f}, {7.34160f, -0.53981f, 16.36322f}, 
	{7.40001f, -0.53789f, 16.36703f}, {7.86739f, -1.17345f, 16.20791f}, {7.48772f, -0.48549f, 16.36254f}, {7.54367f, -0.46581f, 16.36765f}, 
	{8.15851f, -0.96076f, 16.20754f}, {7.61307f, -0.39354f, 16.36334f}, {7.66352f, -0.35811f, 16.36856f}, {8.38458f, -0.67850f, 16.20725f}, 
	{7.70958f, -0.27280f, 16.36436f}, {7.74969f, -0.22388f, 16.36970f}, {8.53001f, -0.34722f, 16.20689f}, {7.77073f, -0.13169f, 16.36558f}, 
	{7.79675f, -0.07312f, 16.37098f}, {7.79252f, 0.01988f, 16.36691f}, {-9.80800f, 0.73900f, 15.71654f}, {-9.90000f, 0.04019f, 15.71654f}, 
	{-9.53827f, 1.39019f, 15.71654f}, {-9.10919f, 1.94938f, 15.71654f}, {-8.55000f, 2.37846f, 15.71654f}, {-7.89881f, 2.64819f, 15.71654f}, 
	{-7.20000f, 2.74019f, 15.71654f}, {-6.50119f, 2.64819f, 15.71654f}, {-5.85000f, 2.37846f, 15.71654f}, {-5.29081f, 1.94938f, 15.71654f}, 
	{-4.86173f, 1.39019f, 15.71654f}, {-4.59200f, 0.73900f, 15.71654f}, {-4.50000f, 0.04019f, 15.71654f}, {-4.59200f, -0.65862f, 15.71654f}, 
	{-4.86173f, -1.30981f, 15.71654f}, {-5.29081f, -1.86900f, 15.71654f}, {-5.85000f, -2.29807f, 15.71654f}, {-6.50119f, -2.56781f, 15.71654f}, 
	{-7.20000f, -2.65981f, 15.71654f}, {-7.89881f, -2.56781f, 15.71654f}, {-8.55000f, -2.29807f, 15.71654f}, {-9.10919f, -1.86900f, 15.71654f}, 
	{-9.53827f, -1.30981f, 15.71654f}, {-9.80800f, -0.65862f, 15.71654f}, {-10.88827f, 1.02846f, 14.85838f}, {-11.01838f, 0.04019f, 14.85837f}, 
	{-10.50681f, 1.94938f, 14.85838f}, {-9.90000f, 2.74019f, 14.85837f}, {-9.10919f, 3.34700f, 14.85837f}, {-8.18827f, 3.72846f, 14.85838f}, 
	{-7.20000f, 3.85857f, 14.85838f}, {-6.21173f, 3.72846f, 14.85838f}, {-5.29081f, 3.34700f, 14.85837f}, {-4.50000f, 2.74019f, 14.85837f}, 
	{-3.89319f, 1.94938f, 14.85838f}, {-3.51173f, 1.02846f, 14.85838f}, {-3.38162f, 0.04019f, 14.85837f}, {-3.51173f, -0.94808f, 14.85838f}, 
	{-3.89319f, -1.86900f, 14.85837f}, {-4.50000f, -2.65981f, 14.85838f}, {-5.29081f, -3.26662f, 14.85837f}, {-6.21173f, -3.64808f, 14.85838f}, 
	{-7.20000f, -3.77818f, 14.85838f}, {-8.18827f, -3.64808f, 14.85838f}, {-9.10919f, -3.26662f, 14.85837f}, {-9.90000f, -2.65981f, 14.85838f}, 
	{-10.50681f, -1.86900f, 14.85837f}, {-10.88827f, -0.94808f, 14.85838f}, {-11.71719f, 1.25057f, 13.74000f}, {-11.87654f, 0.04019f, 13.74000f}, 
	{-11.25000f, 2.37846f, 13.74000f}, {-10.50681f, 3.34700f, 13.74000f}, {-9.53827f, 4.09019f, 13.74000f}, {-8.41038f, 4.55738f, 13.74000f}, 
	{-7.20000f, 4.71673f, 13.74000f}, {-5.98962f, 4.55738f, 13.74000f}, {-4.86173f, 4.09019f, 13.74000f}, {-3.89319f, 3.34700f, 13.74000f}, 
	{-3.15000f, 2.37846f, 13.74000f}, {-2.68281f, 1.25057f, 13.74000f}, {-2.52346f, 0.04019f, 13.74000f}, {-2.68281f, -1.17018f, 13.74000f}, 
	{-3.15000f, -2.29807f, 13.74000f}, {-3.89319f, -3.26662f, 13.74000f}, {-4.86173f, -4.00981f, 13.74000f}, {-5.98962f, -4.47700f, 13.74000f}, 
	{-7.20000f, -4.63634f, 13.74000f}, {-8.41038f, -4.47700f, 13.74000f}, {-9.53827f, -4.00981f, 13.74000f}, {-10.50681f, -3.26662f, 13.74000f}, 
	{-11.25000f, -2.29807f, 13.74000f}, {-11.71719f, -1.17018f, 13.74000f}, {-12.23827f, 1.39019f, 12.43762f}, {-12.41600f, 0.04019f, 12.43762f}, 
	{-11.71719f, 2.64819f, 12.43762f}, {-10.88827f, 3.72846f, 12.43762f}, {-9.80800f, 4.55738f, 12.43762f}, {-8.55000f, 5.07846f, 12.43762f}, 
	{-7.20000f, 5.25619f, 12.43762f}, {-5.85000f, 5.07846f, 12.43762f}, {-4.59200f, 4.55738f, 12.43762f}, {-3.51173f, 3.72846f, 12.43762f}, 
	{-2.68281f, 2.64819f, 12.43762f}, {-2.16173f, 1.39019f, 12.43762f}, {-1.98400f, 0.04019f, 12.43762f}, {-2.16173f, -1.30981f, 12.43762f}, 
	{-2.68281f, -2.56781f, 12.43762f}, {-3.51173f, -3.64808f, 12.43762f}, {-4.59200f, -4.47700f, 12.43762f}, {-5.85000f, -4.99808f, 12.43762f}, 
	{-7.20000f, -5.17581f, 12.43762f}, {-8.55000f, -4.99808f, 12.43762f}, {-9.80800f, -4.47700f, 12.43762f}, {-10.88827f, -3.64808f, 12.43762f}, 
	{-11.71719f, -2.56781f, 12.43762f}, {-12.23827f, -1.30981f, 12.43762f}, {-12.41600f, 1.43781f, 11.04000f}, {-12.60000f, 0.04019f, 11.04000f}, 
	{-11.87654f, 2.74019f, 11.04000f}, {-11.01838f, 3.85857f, 11.04000f}, {-9.90000f, 4.71673f, 11.04000f}, {-8.59762f, 5.25619f, 11.04000f}, 
	{-7.20000f, 5.44019f, 11.04000f}, {-5.80238f, 5.25619f, 11.04000f}, {-4.50000f, 4.71673f, 11.04000f}, {-3.38162f, 3.85857f, 11.04000f}, 
	{-2.52346f, 2.74019f, 11.04000f}, {-1.98400f, 1.43781f, 11.04000f}, {-1.80000f, 0.04019f, 11.04000f}, {-1.98400f, -1.35743f, 11.04000f}, 
	{-2.52346f, -2.65981f, 11.04000f}, {-3.38162f, -3.77818f, 11.04000f}, {-4.50000f, -4.63634f, 11.04000f}, {-5.80238f, -5.17581f, 11.04000f}, 
	{-7.20000f, -5.35981f, 11.04000f}, {-8.59762f, -5.17581f, 11.04000f}, {-9.90000f, -4.63634f, 11.04000f}, {-11.01838f, -3.77818f, 11.04000f}, 
	{-11.87654f, -2.65981f, 11.04000f}, {-12.41600f, -1.35743f, 11.04000f}, {9.90000f, 0.04019f, 15.71654f}, {9.80800f, 0.73900f, 15.71654f}, 
	{9.53827f, 1.39019f, 15.71654f}, {9.10919f, 1.94938f, 15.71654f}, {8.55000f, 2.37846f, 15.71654f}, {7.89881f, 2.64819f, 15.71654f}, 
	{7.20000f, 2.74019f, 15.71654f}, {6.50119f, 2.64819f, 15.71654f}, {5.85000f, 2.37846f, 15.71654f}, {5.29081f, 1.94938f, 15.71654f}, 
	{4.86173f, 1.39019f, 15.71654f}, {4.59200f, 0.73900f, 15.71654f}, {4.50000f, 0.04019f, 15.71654f}, {4.59200f, -0.65862f, 15.71654f}, 
	{4.86173f, -1.30981f, 15.71654f}, {5.29081f, -1.86900f, 15.71654f}, {5.85000f, -2.29807f, 15.71654f}, {6.50119f, -2.56781f, 15.71654f}, 
	{7.20000f, -2.65981f, 15.71654f}, {7.89881f, -2.56781f, 15.71654f}, {8.55000f, -2.29807f, 15.71654f}, {9.10919f, -1.86900f, 15.71654f}, 
	{9.53827f, -1.30981f, 15.71654f}, {9.80800f, -0.65862f, 15.71654f}, {11.01838f, 0.04019f, 14.85837f}, {10.88827f, 1.02846f, 14.85838f}, 
	{10.50681f, 1.94938f, 14.85838f}, {9.90000f, 2.74019f, 14.85837f}, {9.10919f, 3.34700f, 14.85837f}, {8.18827f, 3.72846f, 14.85838f}, 
	{7.20000f, 3.85857f, 14.85838f}, {6.21173f, 3.72846f, 14.85838f}, {5.29081f, 3.34700f, 14.85837f}, {4.50000f, 2.74019f, 14.85837f}, 
	{3.89319f, 1.94938f, 14.85838f}, {3.51173f, 1.02846f, 14.85838f}, {3.38162f, 0.04019f, 14.85837f}, {3.51173f, -0.94808f, 14.85838f}, 
	{3.89319f, -1.86900f, 14.85837f}, {4.50000f, -2.65981f, 14.85838f}, {5.29081f, -3.26662f, 14.85837f}, {6.21173f, -3.64808f, 14.85838f}, 
	{7.20000f, -3.77818f, 14.85838f}, {8.18827f, -3.64808f, 14.85838f}, {9.10919f, -3.26662f, 14.85837f}, {9.90000f, -2.65981f, 14.85838f}, 
	{10.50681f, -1.86900f, 14.85837f}, {10.88827f, -0.94808f, 14.85838f}, {11.87654f, 0.04019f, 13.74000f}, {11.71719f, 1.25057f, 13.74000f}, 
	{11.25000f, 2.37846f, 13.74000f}, {10.50681f, 3.34700f, 13.74000f}, {9.53827f, 4.09019f, 13.74000f}, {8.41038f, 4.55738f, 13.74000f}, 
	{7.20000f, 4.71673f, 13.74000f}, {5.98962f, 4.55738f, 13.74000f}, {4.86173f, 4.09019f, 13.74000f}, {3.89319f, 3.34700f, 13.74000f}, 
	{3.15000f, 2.37846f, 13.74000f}, {2.68281f, 1.25057f, 13.74000f}, {2.52346f, 0.04019f, 13.74000f}, {2.68281f, -1.17018f, 13.74000f}, 
	{3.15000f, -2.29807f, 13.74000f}, {3.89319f, -3.26662f, 13.74000f}, {4.86173f, -4.00981f, 13.74000f}, {5.98962f, -4.47700f, 13.74000f}, 
	{7.20000f, -4.63634f, 13.74000f}, {8.41038f, -4.47700f, 13.74000f}, {9.53827f, -4.00981f, 13.74000f}, {10.50681f, -3.26662f, 13.74000f}, 
	{11.25000f, -2.29807f, 13.74000f}, {11.71719f, -1.17018f, 13.74000f}, {12.41600f, 0.04019f, 12.43762f}, {12.23827f, 1.39019f, 12.43762f}, 
	{11.71719f, 2.64819f, 12.43762f}, {10.88827f, 3.72846f, 12.43762f}, {9.80800f, 4.55738f, 12.43762f}, {8.55000f, 5.07846f, 12.43762f}, 
	{7.20000f, 5.25619f, 12.43762f}, {5.85000f, 5.07846f, 12.43762f}, {4.59200f, 4.55738f, 12.43762f}, {3.51173f, 3.72846f, 12.43762f}, 
	{2.68281f, 2.64819f, 12.43762f}, {2.16173f, 1.39019f, 12.43762f}, {1.98400f, 0.04019f, 12.43762f}, {2.16173f, -1.30981f, 12.43762f}, 
	{2.68281f, -2.56781f, 12.43762f}, {3.51173f, -3.64808f, 12.43762f}, {4.59200f, -4.47700f, 12.43762f}, {5.85000f, -4.99808f, 12.43762f}, 
	{7.20000f, -5.17581f, 12.43762f}, {8.55000f, -4.99808f, 12.43762f}, {9.80800f, -4.47700f, 12.43762f}, {10.88827f, -3.64808f, 12.43762f}, 
	{11.71719f, -2.56781f, 12.43762f}, {12.23827f, -1.30981f, 12.43762f}, {12.60000f, 0.04019f, 11.04000f}, {12.41600f, 1.43781f, 11.04000f}, 
	{11.87654f, 2.74019f, 11.04000f}, {11.01838f, 3.85857f, 11.04000f}, {9.90000f, 4.71673f, 11.04000f}, {8.59762f, 5.25619f, 11.04000f}, 
	{7.20000f, 5.44019f, 11.04000f}, {5.80238f, 5.25619f, 11.04000f}, {4.50000f, 4.71673f, 11.04000f}, {3.38162f, 3.85857f, 11.04000f}, 
	{2.52346f, 2.74019f, 11.04000f}, {1.98400f, 1.43781f, 11.04000f}, {1.80000f, 0.04019f, 11.04000f}, {1.98400f, -1.35743f, 11.04000f}, 
	{2.52346f, -2.65981f, 11.04000f}, {3.38162f, -3.77818f, 11.04000f}, {4.50000f, -4.63634f, 11.04000f}, {5.80238f, -5.17581f, 11.04000f}, 
	{7.20000f, -5.35981f, 11.04000f}, {8.59762f, -5.17581f, 11.04000f}, {9.90000f, -4.63634f, 11.04000f}, {11.01838f, -3.77818f, 11.04000f}, 
	{11.87654f, -2.65981f, 11.04000f}, {12.41600f, -1.35743f, 11.04000f}, {-0.00000f, -17.63397f, 18.01682f}, {-0.18189f, -16.89275f, 18.30752f}, 
	{-0.25089f, -17.75383f, 18.12329f}, {-0.60670f, -17.14060f, 18.37296f}, {-0.59841f, -18.00034f, 18.12425f}, {-1.08695f, -16.92222f, 18.26719f}, 
	{-1.00282f, -17.78351f, 18.09362f}, {-1.17000f, -17.58660f, 17.80897f}, {-1.53925f, -16.91347f, 18.18182f}, {-1.54647f, -17.77412f, 17.97717f}, 
	{-2.00051f, -17.07809f, 18.10334f}, {-1.91562f, -17.93827f, 17.87618f}, {-2.32919f, -16.87074f, 17.87221f}, {-2.20174f, -17.73266f, 17.72860f}, 
	{-2.74868f, -17.61741f, 17.46082f}, {-3.47693f, -16.89732f, 17.47875f}, {-3.32383f, -17.75818f, 17.28428f}, {0.25088f, -17.75383f, 18.12329f}, 
	{0.18188f, -16.89275f, 18.30752f}, {0.59840f, -18.00034f, 18.12425f}, {0.60669f, -17.14060f, 18.37296f}, {1.00281f, -17.78351f, 18.09361f}, 
	{1.08695f, -16.92222f, 18.26719f}, {1.54646f, -17.77412f, 17.97716f}, {1.53924f, -16.91347f, 18.18182f}, {1.16999f, -17.58660f, 17.80897f}, 
	{1.91561f, -17.93827f, 17.87618f}, {2.00050f, -17.07809f, 18.10334f}, {2.20174f, -17.73266f, 17.72860f}, {2.32919f, -16.87075f, 17.87221f}, 
	{3.32382f, -17.75818f, 17.28428f}, {3.47692f, -16.89732f, 17.47875f}, {2.74867f, -17.61741f, 17.46082f}, {-2.93021f, -15.96212f, 15.76579f}, 
	{-3.08854f, -15.77805f, 15.81391f}, {-2.92527f, -15.86660f, 16.07586f}, {-6.01876f, -15.58637f, 8.76831f}, {-6.22845f, -15.44866f, 9.04236f}, 
	{-6.01544f, -15.32532f, 8.98908f}, {-0.00000f, -16.77248f, 18.18089f}, {-0.20681f, -16.52526f, 18.28843f}, {-0.00000f, -16.44730f, 18.18091f}, 
	{-0.64828f, -16.64143f, 18.36977f}, {-1.04868f, -16.64084f, 18.26986f}, {-1.24013f, -16.72787f, 18.10665f}, {-1.26353f, -16.56788f, 18.08681f}, 
	{-1.48898f, -16.63105f, 18.15051f}, {-1.97902f, -16.58957f, 18.05955f}, {-2.27069f, -16.57750f, 17.87932f}, {-2.49799f, -16.68469f, 17.62951f}, 
	{-2.41053f, -17.54415f, 17.36701f}, {-2.49720f, -16.47977f, 17.58515f}, {-2.75583f, -16.75708f, 17.68124f}, {-2.78045f, -16.48022f, 17.64839f}, 
	{-3.33308f, -16.52739f, 17.50797f}, {-3.67553f, -16.68130f, 16.99523f}, {-3.49307f, -17.54476f, 16.92677f}, {-3.54981f, -16.35629f, 16.99254f}, 
	{-3.71476f, -16.57292f, 16.74486f}, {-3.54054f, -17.43346f, 16.53504f}, {-3.61014f, -16.34762f, 16.72301f}, {-4.01445f, -16.61065f, 16.71137f}, 
	{-3.95999f, -17.47112f, 16.49797f}, {-3.90576f, -16.37760f, 16.70575f}, {-4.65659f, -16.73290f, 16.40978f}, {-4.47618f, -17.59442f, 16.24705f}, 
	{-4.57599f, -16.37442f, 16.41466f}, {-4.79085f, -16.56265f, 15.78880f}, {-4.59006f, -17.42758f, 15.79122f}, {-4.67789f, -16.22028f, 15.70261f}, 
	{-4.61138f, -16.38250f, 15.39859f}, {-4.31834f, -17.24399f, 15.23438f}, {-4.58330f, -16.18027f, 15.40410f}, {-4.93202f, -16.46802f, 15.31022f}, 
	{-4.83175f, -17.32900f, 15.12158f}, {-4.87977f, -16.22781f, 15.33700f}, {-5.46097f, -16.78566f, 14.74559f}, {-5.23653f, -17.64859f, 14.65138f}, 
	{-5.42450f, -16.41422f, 14.75442f}, {-5.41341f, -16.59791f, 14.20481f}, {-5.22160f, -17.46348f, 14.23830f}, {-5.31383f, -16.32783f, 14.08459f}, 
	{-5.25518f, -16.50878f, 13.86282f}, {-4.97131f, -17.37092f, 13.73042f}, {-5.20639f, -16.15113f, 13.83179f}, {-5.52927f, -16.58593f, 13.75843f}, 
	{-5.44260f, -17.44679f, 13.56405f}, {-5.53116f, -16.32294f, 13.74737f}, {-6.07304f, -16.74740f, 13.41888f}, {-5.91352f, -17.60910f, 13.26526f}, 
	{-6.11409f, -16.37473f, 13.30807f}, {-6.35549f, -16.80146f, 12.75435f}, {-6.15508f, -17.66498f, 12.68847f}, {-6.33425f, -16.39914f, 12.69598f}, 
	{-6.40209f, -16.75416f, 12.07569f}, {-6.20008f, -17.61900f, 12.07410f}, {-6.35077f, -16.29942f, 11.98525f}, {-6.29025f, -16.52071f, 11.63476f}, 
	{-6.11251f, -17.38666f, 11.68656f}, {-6.23080f, -16.17274f, 11.60796f}, {-6.07921f, -16.38289f, 11.24471f}, {-5.79593f, -17.24546f, 11.13281f}, 
	{-6.02333f, -16.01091f, 11.22203f}, {-6.36647f, -16.43995f, 11.10090f}, {-6.23780f, -17.30108f, 10.91925f}, {-6.28294f, -16.10692f, 11.06275f}, 
	{-6.69498f, -16.39694f, 10.71414f}, {-6.49796f, -17.25976f, 10.61484f}, {-6.61477f, -16.08470f, 10.63196f}, {-6.78687f, -16.41146f, 9.59109f}, 
	{-6.58147f, -17.27651f, 9.59954f}, {-6.65127f, -16.08354f, 9.65100f}, {-6.61961f, -16.42721f, 9.11268f}, {-6.45198f, -17.29450f, 9.22932f}, 
	{-6.49804f, -16.01636f, 9.15024f}, {-5.93554f, -16.42726f, 8.68807f}, {-5.85421f, -17.29564f, 8.85813f}, {-5.95714f, -16.19143f, 8.69504f}, 
	{-5.40708f, -16.50761f, 8.54597f}, {-5.38669f, -17.37633f, 8.73204f}, {-5.33857f, -16.15534f, 8.51209f}, {-4.81288f, -16.52102f, 8.58059f}, 
	{-4.90833f, -17.38960f, 8.75965f}, {-4.76921f, -16.30601f, 8.64995f}, {-4.37683f, -16.67363f, 9.07718f}, {-4.56567f, -17.53988f, 9.14361f}, 
	{-4.35713f, -16.45562f, 9.08197f}, {-4.23744f, -16.92643f, 10.20006f}, {-4.44405f, -17.79103f, 10.18636f}, {-4.17561f, -16.53908f, 10.11336f}, 
	{-4.26086f, -16.89259f, 10.48686f}, {-4.45649f, -17.75645f, 10.43758f}, {-4.17884f, -16.54363f, 10.48233f}, {-4.36462f, -16.73296f, 10.91090f}, 
	{-4.59905f, -17.59871f, 10.95363f}, {-4.36904f, -16.49005f, 10.91991f}, {-4.14507f, -16.83500f, 11.15287f}, {-4.32044f, -17.70167f, 11.23962f}, 
	{-4.12227f, -16.60585f, 11.15324f}, {-4.00812f, -16.87037f, 11.52644f}, {-4.20285f, -17.73605f, 11.56523f}, {-3.94686f, -16.57609f, 11.51806f}, 
	{-3.89808f, -16.91153f, 12.02843f}, {-4.09883f, -17.77677f, 12.04608f}, {-3.88336f, -16.56627f, 11.95848f}, {-3.79823f, -16.81075f, 12.77007f}, 
	{-3.99846f, -17.67528f, 12.75310f}, {-3.75045f, -16.52942f, 12.79199f}, {-3.83350f, -16.74248f, 13.04466f}, {-4.02361f, -17.60628f, 12.99235f}, 
	{-3.78063f, -16.50096f, 12.97884f}, {-3.92026f, -16.60397f, 13.41465f}, {-4.15350f, -17.47056f, 13.49726f}, {-3.91425f, -16.37817f, 13.38260f}, 
	{-3.79457f, -16.62748f, 13.49425f}, {-3.94767f, -17.49493f, 13.61879f}, {-3.74132f, -16.33274f, 13.52496f}, {-3.49462f, -16.64826f, 14.04610f}, 
	{-3.68288f, -17.51438f, 14.10637f}, {-3.34989f, -16.30836f, 14.06133f}, {-3.31595f, -16.67003f, 14.64628f}, {-3.52889f, -17.53448f, 14.62546f}, 
	{-3.27124f, -16.29050f, 14.67305f}, {-3.41892f, -16.57981f, 14.94756f}, {-3.66257f, -17.44588f, 15.00541f}, {-3.41493f, -16.32122f, 14.94285f}, 
	{-3.17575f, -16.63542f, 15.13468f}, {-3.31532f, -17.50304f, 15.26758f}, {-3.19459f, -16.31390f, 15.09032f}, {-3.00821f, -16.61095f, 15.31998f}, 
	{-3.17514f, -17.47784f, 15.41752f}, {-2.96014f, -16.31181f, 15.29529f}, {-2.84412f, -16.56946f, 15.64151f}, {-3.04237f, -17.43509f, 15.67827f}, 
	{-2.83914f, -16.24132f, 15.77852f}, {-2.82670f, -16.55001f, 15.90714f}, {-3.01797f, -17.41898f, 16.10521f}, {-2.87121f, -16.25501f, 15.97263f}, 
	{-2.45687f, -16.61132f, 15.88185f}, {-2.49778f, -17.48004f, 16.06815f}, {-2.47908f, -16.23381f, 16.12002f}, {-2.16878f, -16.64032f, 16.02314f}, 
	{-2.29786f, -17.50818f, 16.16794f}, {-2.29440f, -16.28086f, 16.22129f}, {-1.96403f, -16.63631f, 16.27264f}, {-2.15133f, -17.50269f, 16.34536f}, 
	{-2.04367f, -16.33271f, 16.42100f}, {-1.89096f, -16.58457f, 16.61628f}, {-2.04665f, -17.45479f, 16.87529f}, {-1.93828f, -16.36148f, 16.72964f}, 
	{-1.52509f, -16.69205f, 16.47437f}, {-1.51616f, -17.56090f, 16.66676f}, {-1.72358f, -16.33915f, 16.76174f}, {-1.37095f, -16.70487f, 16.51647f}, 
	{-1.45193f, -17.57329f, 16.68821f}, {-1.39155f, -16.42730f, 16.78125f}, {-1.13939f, -16.65238f, 16.65685f}, {-1.27524f, -17.52002f, 16.79037f}, 
	{-1.20378f, -16.37202f, 16.94149f}, {-0.96156f, -16.61759f, 16.89060f}, {-1.00153f, -17.48792f, 17.15502f}, {-0.97900f, -16.43574f, 17.01930f}, 
	{-0.76621f, -16.63355f, 16.74732f}, {-0.69492f, -17.50216f, 16.92788f}, {-0.86426f, -16.46003f, 16.90396f}, {-0.48393f, -16.64769f, 16.73045f}, 
	{-0.52129f, -17.51645f, 16.91879f}, {-0.51601f, -16.40963f, 16.89666f}, {-0.22090f, -16.60003f, 16.84904f}, {-0.33435f, -17.46807f, 17.00252f}, 
	{-0.27387f, -16.36433f, 17.00211f}, {-0.00000f, -16.60548f, 17.05342f}, {-0.00000f, -17.47306f, 17.18434f}, {-0.00000f, -16.34721f, 17.21239f}, 
	{-0.00000f, -16.06287f, 17.34981f}, {-0.28100f, -16.02986f, 17.40386f}, {-4.22114f, -16.01495f, 10.12686f}, {-4.33643f, -15.89144f, 9.27672f}, 
	{-4.57889f, -15.61569f, 10.13172f}, {-4.70636f, -15.57754f, 9.45623f}, {-4.71745f, -15.84483f, 8.69924f}, {-4.81345f, -15.65396f, 8.82753f}, 
	{-4.23454f, -16.02613f, 10.59018f}, {-4.54044f, -15.72231f, 10.68044f}, {-4.38091f, -16.08115f, 10.90548f}, {-4.58069f, -15.84429f, 10.94259f}, 
	{-4.12671f, -16.04087f, 11.12972f}, {-4.46244f, -15.74634f, 11.13704f}, {-3.93721f, -16.05787f, 11.59497f}, {-4.28673f, -15.59357f, 11.65166f}, 
	{-3.96342f, -16.08933f, 11.93101f}, {-4.18090f, -15.76165f, 12.00300f}, {-3.76205f, -15.95925f, 12.69738f}, {-4.05611f, -15.58886f, 12.76469f}, 
	{-3.79121f, -16.01675f, 13.05742f}, {-4.10712f, -15.80033f, 13.16541f}, {-3.96522f, -16.00747f, 13.36718f}, {-4.04153f, -15.87251f, 13.37134f}, 
	{-3.72815f, -15.87966f, 13.55126f}, {-3.88104f, -15.58522f, 13.67708f}, {-3.42492f, -15.70591f, 14.10238f}, {-3.68563f, -15.38686f, 14.23321f}, 
	{-3.32531f, -15.78422f, 14.65833f}, {-3.54965f, -15.60181f, 14.76904f}, {-3.40833f, -15.97226f, 14.92716f}, {-3.52144f, -15.74485f, 14.96265f}, 
	{-3.25788f, -15.91059f, 15.05911f}, {-3.43915f, -15.59048f, 15.18447f}, {-3.04115f, -15.89060f, 15.30526f}, {-3.30196f, -15.55740f, 15.41841f}, 
	{-2.75509f, -15.91793f, 16.30255f}, {-2.93308f, -15.57305f, 16.51809f}, {-3.13426f, -15.48159f, 16.28392f}, {-3.35651f, -15.70018f, 16.09178f}, 
	{-5.74664f, -15.17496f, 9.51177f}, {-6.28676f, -15.37070f, 9.63748f}, {-5.25236f, -15.54258f, 8.81842f}, {-5.31241f, -15.78587f, 8.52599f}, 
	{-5.20228f, -15.57840f, 9.54797f}, {-6.12873f, -15.35143f, 10.47583f}, {-5.69252f, -15.20898f, 10.26723f}, {-5.12121f, -15.62519f, 10.24450f}, 
	{-5.98630f, -15.45203f, 10.97194f}, {-5.58687f, -15.25545f, 10.78219f}, {-5.08847f, -15.70379f, 10.76953f}, {-5.81427f, -15.55941f, 11.18255f}, 
	{-5.52333f, -15.37532f, 11.03533f}, {-4.95489f, -15.62757f, 11.04273f}, {-5.87788f, -15.50957f, 11.53586f}, {-5.46714f, -15.30330f, 11.32058f}, 
	{-4.93970f, -15.67254f, 11.26826f}, {-5.96350f, -15.45577f, 12.00377f}, {-5.44501f, -15.22706f, 11.91661f}, {-4.86886f, -15.70898f, 11.84720f}, 
	{-5.80002f, -15.52239f, 12.66636f}, {-5.31487f, -15.26326f, 12.48130f}, {-4.74491f, -15.72873f, 12.31223f}, {-5.66405f, -15.48009f, 13.16366f}, 
	{-5.13162f, -15.19295f, 12.95983f}, {-4.57981f, -15.68718f, 12.89718f}, {-5.40916f, -15.63371f, 13.65853f}, {-4.97264f, -15.32583f, 13.43429f}, 
	{-4.49252f, -15.69866f, 13.22121f}, {-5.16536f, -15.72230f, 13.80101f}, {-4.97007f, -15.52473f, 13.64093f}, {-4.40235f, -15.57841f, 13.54497f}, 
	{-5.23516f, -15.71393f, 14.02968f}, {-4.93636f, -15.40146f, 13.92917f}, {-4.38949f, -15.64464f, 13.86929f}, {-5.18943f, -15.57441f, 14.62486f}, 
	{-4.84043f, -15.28047f, 14.58723f}, {-4.13580f, -15.45495f, 14.46422f}, {-4.75078f, -15.60345f, 15.20184f}, {-4.50094f, -15.37555f, 15.03666f}, 
	{-4.03816f, -15.59406f, 14.87183f}, {-4.50312f, -15.68448f, 15.38540f}, {-4.28271f, -15.44998f, 15.25497f}, {-3.83573f, -15.54294f, 15.19436f}, 
	{-4.49383f, -15.62954f, 15.66940f}, {-4.23121f, -15.37898f, 15.46353f}, {-3.75736f, -15.63966f, 15.47198f}, {-4.34341f, -15.56197f, 16.26898f}, 
	{-4.01031f, -15.23474f, 16.04436f}, {-3.50619f, -15.52003f, 15.69637f}, {-3.78891f, -15.56810f, 16.59193f}, {-3.57198f, -15.29442f, 16.32681f}, 
	{-3.46819f, -15.65091f, 16.67103f}, {-3.32069f, -15.39813f, 16.45002f}, {-3.38016f, -15.63072f, 16.90314f}, {-3.18557f, -15.25771f, 16.63471f}, 
	{-3.10944f, -15.54915f, 17.31991f}, {-2.77354f, -15.21254f, 16.98788f}, {-2.65161f, -15.56832f, 16.76363f}, {-2.66584f, -15.61693f, 17.58131f}, 
	{-2.52808f, -15.21009f, 17.21763f}, {-2.31350f, -15.53379f, 16.99180f}, {-2.45872f, -15.63766f, 17.59361f}, {-2.28087f, -15.32894f, 17.39884f}, 
	{-2.07257f, -15.55681f, 17.19529f}, {-2.22159f, -15.56677f, 17.83329f}, {-2.05019f, -15.26224f, 17.51194f}, {-1.91171f, -15.58698f, 17.29058f}, 
	{-1.82046f, -15.55449f, 18.00671f}, {-1.64209f, -15.23464f, 17.63744f}, {-1.55822f, -15.60630f, 17.41406f}, {-1.42858f, -15.57474f, 18.07820f}, 
	{-1.39798f, -15.26302f, 17.74549f}, {-1.34180f, -15.57650f, 17.49630f}, {-1.15531f, -15.57967f, 18.09600f}, {-1.09003f, -15.29629f, 17.81794f}, 
	{-1.05706f, -15.58860f, 17.53312f}, {-0.91420f, -15.54442f, 18.17885f}, {-0.86774f, -15.22234f, 17.90417f}, {-0.84162f, -15.60649f, 17.56141f}, 
	{-0.55732f, -15.49977f, 18.26960f}, {-0.50739f, -15.22840f, 17.95647f}, {-0.52369f, -15.62257f, 17.59078f}, {-0.21834f, -15.55533f, 18.28294f}, 
	{-0.14972f, -15.23919f, 18.00756f}, {-0.25326f, -15.61909f, 17.62785f}, {-0.00000f, -15.54509f, 18.18963f}, {-0.00000f, -15.28727f, 17.91082f}, 
	{-0.00000f, -15.59107f, 17.63604f}, {-2.51820f, -15.92005f, 16.53624f}, {-2.19544f, -15.94612f, 16.77547f}, {-1.96199f, -16.02412f, 16.97507f}, 
	{-1.73089f, -16.00174f, 17.10364f}, {-1.49455f, -15.97343f, 17.17772f}, {-1.25707f, -15.97972f, 17.27908f}, {-1.00215f, -16.01999f, 17.29433f}, 
	{-0.82417f, -16.02423f, 17.33266f}, {-0.52706f, -16.02871f, 17.35020f}, {2.92526f, -15.86660f, 16.07586f}, {3.08853f, -15.77805f, 15.81390f}, 
	{2.93020f, -15.96212f, 15.76579f}, {6.01544f, -15.32532f, 8.98908f}, {6.22845f, -15.44867f, 9.04235f}, {6.01876f, -15.58637f, 8.76830f}, 
	{0.20681f, -16.52526f, 18.28843f}, {0.64827f, -16.64143f, 18.36977f}, {1.04867f, -16.64084f, 18.26986f}, {1.24012f, -16.72787f, 18.10665f}, 
	{1.26353f, -16.56788f, 18.08680f}, {1.48897f, -16.63105f, 18.15051f}, {1.97901f, -16.58957f, 18.05955f}, {2.27068f, -16.57750f, 17.87932f}, 
	{2.41052f, -17.54415f, 17.36700f}, {2.49798f, -16.68469f, 17.62951f}, {2.49719f, -16.47977f, 17.58515f}, {2.75582f, -16.75709f, 17.68124f}, 
	{2.78045f, -16.48022f, 17.64839f}, {3.33307f, -16.52739f, 17.50797f}, {3.49306f, -17.54477f, 16.92677f}, {3.67552f, -16.68130f, 16.99523f}, 
	{3.54980f, -16.35630f, 16.99253f}, {3.54054f, -17.43346f, 16.53504f}, {3.71475f, -16.57292f, 16.74486f}, {3.61013f, -16.34762f, 16.72301f}, 
	{3.95998f, -17.47112f, 16.49796f}, {4.01444f, -16.61065f, 16.71137f}, {3.90575f, -16.37760f, 16.70574f}, {4.47617f, -17.59442f, 16.24705f}, 
	{4.65658f, -16.73290f, 16.40977f}, {4.57598f, -16.37442f, 16.41466f}, {4.59005f, -17.42758f, 15.79121f}, {4.79084f, -16.56266f, 15.78880f}, 
	{4.67788f, -16.22028f, 15.70260f}, {4.31833f, -17.24399f, 15.23438f}, {4.61138f, -16.38250f, 15.39859f}, {4.58329f, -16.18027f, 15.40410f}, 
	{4.83174f, -17.32900f, 15.12157f}, {4.93201f, -16.46802f, 15.31022f}, {4.87976f, -16.22781f, 15.33699f}, {5.23652f, -17.64860f, 14.65137f}, 
	{5.46096f, -16.78566f, 14.74558f}, {5.42450f, -16.41422f, 14.75442f}, {5.22160f, -17.46348f, 14.23829f}, {5.41340f, -16.59791f, 14.20481f}, 
	{5.31383f, -16.32783f, 14.08458f}, {4.97131f, -17.37092f, 13.73041f}, {5.25518f, -16.50878f, 13.86282f}, {5.20639f, -16.15113f, 13.83179f}, 
	{5.44260f, -17.44679f, 13.56405f}, {5.52927f, -16.58593f, 13.75843f}, {5.53115f, -16.32295f, 13.74737f}, {5.91351f, -17.60910f, 13.26525f}, 
	{6.07304f, -16.74740f, 13.41887f}, {6.11409f, -16.37473f, 13.30807f}, {6.15508f, -17.66498f, 12.68847f}, {6.35549f, -16.80146f, 12.75435f}, 
	{6.33425f, -16.39914f, 12.69597f}, {6.20008f, -17.61900f, 12.07409f}, {6.40209f, -16.75416f, 12.07568f}, {6.35077f, -16.29942f, 11.98524f}, 
	{6.11251f, -17.38666f, 11.68655f}, {6.29025f, -16.52072f, 11.63476f}, {6.23080f, -16.17274f, 11.60795f}, {5.79593f, -17.24546f, 11.13280f}, 
	{6.07921f, -16.38289f, 11.24470f}, {6.02333f, -16.01091f, 11.22203f}, {6.23780f, -17.30108f, 10.91924f}, {6.36647f, -16.43995f, 11.10090f}, 
	{6.28293f, -16.10692f, 11.06275f}, {6.49796f, -17.25977f, 10.61484f}, {6.69497f, -16.39694f, 10.71413f}, {6.61477f, -16.08470f, 10.63196f}, 
	{6.58147f, -17.27651f, 9.59954f}, {6.78687f, -16.41146f, 9.59109f}, {6.65127f, -16.08354f, 9.65099f}, {6.45198f, -17.29450f, 9.22931f}, 
	{6.61961f, -16.42722f, 9.11268f}, {6.49804f, -16.01636f, 9.15024f}, {5.85421f, -17.29565f, 8.85813f}, {5.93554f, -16.42726f, 8.68807f}, 
	{5.95714f, -16.19143f, 8.69504f}, {5.38669f, -17.37633f, 8.73203f}, {5.40708f, -16.50761f, 8.54596f}, {5.33857f, -16.15534f, 8.51208f}, 
	{4.90833f, -17.38960f, 8.75965f}, {4.81288f, -16.52102f, 8.58059f}, {4.76921f, -16.30601f, 8.64994f}, {4.56567f, -17.53988f, 9.14361f}, 
	{4.37683f, -16.67363f, 9.07718f}, {4.35713f, -16.45562f, 9.08197f}, {4.44405f, -17.79103f, 10.18636f}, {4.23744f, -16.92643f, 10.20006f}, 
	{4.17561f, -16.53908f, 10.11335f}, {4.45649f, -17.75645f, 10.43758f}, {4.26086f, -16.89259f, 10.48686f}, {4.17884f, -16.54363f, 10.48233f}, 
	{4.59904f, -17.59872f, 10.95362f}, {4.36462f, -16.73296f, 10.91090f}, {4.36904f, -16.49005f, 10.91991f}, {4.32044f, -17.70167f, 11.23962f}, 
	{4.14507f, -16.83500f, 11.15286f}, {4.12227f, -16.60585f, 11.15323f}, {4.20284f, -17.73605f, 11.56523f}, {4.00812f, -16.87037f, 11.52644f}, 
	{3.94685f, -16.57609f, 11.51806f}, {4.09882f, -17.77677f, 12.04608f}, {3.89808f, -16.91153f, 12.02843f}, {3.88336f, -16.56627f, 11.95848f}, 
	{3.99846f, -17.67528f, 12.75310f}, {3.79822f, -16.81075f, 12.77007f}, {3.75045f, -16.52942f, 12.79198f}, {4.02361f, -17.60628f, 12.99234f}, 
	{3.83350f, -16.74248f, 13.04466f}, {3.78063f, -16.50096f, 12.97884f}, {4.15349f, -17.47056f, 13.49726f}, {3.92025f, -16.60397f, 13.41465f}, 
	{3.91425f, -16.37817f, 13.38260f}, {3.94767f, -17.49493f, 13.61878f}, {3.79456f, -16.62749f, 13.49425f}, {3.74132f, -16.33274f, 13.52496f}, 
	{3.68288f, -17.51438f, 14.10636f}, {3.49461f, -16.64826f, 14.04609f}, {3.34989f, -16.30836f, 14.06133f}, {3.52888f, -17.53448f, 14.62545f}, 
	{3.31594f, -16.67003f, 14.64628f}, {3.27123f, -16.29050f, 14.67305f}, {3.66256f, -17.44588f, 15.00541f}, {3.41891f, -16.57981f, 14.94756f}, 
	{3.41492f, -16.32122f, 14.94285f}, {3.31531f, -17.50304f, 15.26758f}, {3.17574f, -16.63542f, 15.13468f}, {3.19458f, -16.31390f, 15.09031f}, 
	{3.17513f, -17.47785f, 15.41752f}, {3.00821f, -16.61095f, 15.31998f}, {2.96013f, -16.31181f, 15.29529f}, {3.04237f, -17.43509f, 15.67827f}, 
	{2.84412f, -16.56946f, 15.64151f}, {2.83914f, -16.24132f, 15.77852f}, {3.01797f, -17.41898f, 16.10520f}, {2.82670f, -16.55001f, 15.90714f}, 
	{2.87120f, -16.25501f, 15.97263f}, {2.49777f, -17.48004f, 16.06814f}, {2.45686f, -16.61132f, 15.88185f}, {2.47907f, -16.23381f, 16.12002f}, 
	{2.29785f, -17.50819f, 16.16794f}, {2.16877f, -16.64032f, 16.02314f}, {2.29440f, -16.28086f, 16.22129f}, {2.15133f, -17.50269f, 16.34536f}, 
	{1.96403f, -16.63631f, 16.27264f}, {2.04366f, -16.33271f, 16.42100f}, {2.04664f, -17.45479f, 16.87529f}, {1.89095f, -16.58457f, 16.61628f}, 
	{1.93828f, -16.36148f, 16.72964f}, {1.51615f, -17.56090f, 16.66676f}, {1.52508f, -16.69205f, 16.47437f}, {1.72358f, -16.33915f, 16.76174f}, 
	{1.45193f, -17.57329f, 16.68821f}, {1.37094f, -16.70487f, 16.51647f}, {1.39154f, -16.42730f, 16.78125f}, {1.27523f, -17.52002f, 16.79037f}, 
	{1.13939f, -16.65238f, 16.65685f}, {1.20378f, -16.37202f, 16.94149f}, {1.00152f, -17.48792f, 17.15502f}, {0.96155f, -16.61759f, 16.89060f}, 
	{0.97899f, -16.43574f, 17.01930f}, {0.69491f, -17.50216f, 16.92788f}, {0.76621f, -16.63356f, 16.74732f}, {0.86426f, -16.46003f, 16.90396f}, 
	{0.52128f, -17.51645f, 16.91879f}, {0.48392f, -16.64769f, 16.73045f}, {0.51600f, -16.40963f, 16.89666f}, {0.33434f, -17.46807f, 17.00252f}, 
	{0.22089f, -16.60003f, 16.84904f}, {0.27387f, -16.36433f, 17.00211f}, {0.28099f, -16.02986f, 17.40386f}, {4.22114f, -16.01496f, 10.12686f}, 
	{4.33643f, -15.89144f, 9.27672f}, {4.57889f, -15.61569f, 10.13171f}, {4.70636f, -15.57754f, 9.45623f}, {4.71745f, -15.84483f, 8.69923f}, 
	{4.81345f, -15.65396f, 8.82752f}, {4.23453f, -16.02613f, 10.59018f}, {4.54044f, -15.72232f, 10.68044f}, {4.38091f, -16.08115f, 10.90547f}, 
	{4.58068f, -15.84429f, 10.94258f}, {4.12671f, -16.04087f, 11.12971f}, {4.46244f, -15.74634f, 11.13704f}, {3.93720f, -16.05787f, 11.59497f}, 
	{4.28673f, -15.59358f, 11.65165f}, {3.96342f, -16.08933f, 11.93100f}, {4.18090f, -15.76165f, 12.00300f}, {3.76204f, -15.95925f, 12.69738f}, 
	{4.05611f, -15.58886f, 12.76469f}, {3.79121f, -16.01676f, 13.05742f}, {4.10712f, -15.80033f, 13.16541f}, {3.96521f, -16.00747f, 13.36717f}, 
	{4.04153f, -15.87251f, 13.37134f}, {3.72815f, -15.87966f, 13.55126f}, {3.88104f, -15.58522f, 13.67708f}, {3.42491f, -15.70591f, 14.10238f}, 
	{3.68563f, -15.38686f, 14.23321f}, {3.32530f, -15.78422f, 14.65833f}, {3.54964f, -15.60181f, 14.76904f}, {3.40832f, -15.97226f, 14.92716f}, 
	{3.52143f, -15.74485f, 14.96265f}, {3.25787f, -15.91059f, 15.05911f}, {3.43915f, -15.59048f, 15.18447f}, {3.04114f, -15.89060f, 15.30526f}, 
	{3.30195f, -15.55740f, 15.41841f}, {2.75508f, -15.91793f, 16.30255f}, {2.93307f, -15.57305f, 16.51808f}, {3.13425f, -15.48159f, 16.28392f}, 
	{3.35651f, -15.70018f, 16.09178f}, {5.74664f, -15.17496f, 9.51176f}, {6.28676f, -15.37070f, 9.63747f}, {5.31240f, -15.78588f, 8.52599f}, 
	{5.25236f, -15.54258f, 8.81841f}, {5.20228f, -15.57840f, 9.54796f}, {5.69252f, -15.20899f, 10.26722f}, {6.12872f, -15.35143f, 10.47583f}, 
	{5.12121f, -15.62520f, 10.24449f}, {5.58686f, -15.25545f, 10.78219f}, {5.98630f, -15.45204f, 10.97194f}, {5.08847f, -15.70379f, 10.76953f}, 
	{5.52333f, -15.37532f, 11.03533f}, {5.81426f, -15.55942f, 11.18255f}, {4.95489f, -15.62757f, 11.04272f}, {5.46714f, -15.30330f, 11.32057f}, 
	{5.87788f, -15.50957f, 11.53586f}, {4.93969f, -15.67255f, 11.26826f}, {5.44500f, -15.22706f, 11.91661f}, {5.96350f, -15.45577f, 12.00377f}, 
	{4.86886f, -15.70898f, 11.84720f}, {5.31486f, -15.26326f, 12.48129f}, {5.80002f, -15.52239f, 12.66635f}, {4.74491f, -15.72873f, 12.31223f}, 
	{5.13162f, -15.19295f, 12.95983f}, {5.66405f, -15.48009f, 13.16366f}, {4.57981f, -15.68718f, 12.89718f}, {4.97263f, -15.32583f, 13.43428f}, 
	{5.40916f, -15.63371f, 13.65852f}, {4.49252f, -15.69866f, 13.22120f}, {4.97007f, -15.52473f, 13.64093f}, {5.16536f, -15.72231f, 13.80100f}, 
	{4.40235f, -15.57841f, 13.54497f}, {4.93635f, -15.40146f, 13.92916f}, {5.23515f, -15.71393f, 14.02967f}, {4.38949f, -15.64465f, 13.86929f}, 
	{4.84043f, -15.28047f, 14.58722f}, {5.18942f, -15.57441f, 14.62486f}, {4.13579f, -15.45495f, 14.46421f}, {4.50094f, -15.37555f, 15.03666f}, 
	{4.75078f, -15.60345f, 15.20183f}, {4.03815f, -15.59406f, 14.87182f}, {4.28270f, -15.44998f, 15.25497f}, {4.50311f, -15.68448f, 15.38539f}, 
	{3.83573f, -15.54294f, 15.19435f}, {4.23121f, -15.37898f, 15.46353f}, {4.49382f, -15.62954f, 15.66940f}, {3.75735f, -15.63966f, 15.47198f}, 
	{4.01031f, -15.23475f, 16.04436f}, {4.34340f, -15.56197f, 16.26897f}, {3.50619f, -15.52003f, 15.69637f}, {3.57197f, -15.29442f, 16.32680f}, 
	{3.78890f, -15.56811f, 16.59193f}, {3.32068f, -15.39814f, 16.45002f}, {3.46819f, -15.65091f, 16.67103f}, {3.18556f, -15.25772f, 16.63471f}, 
	{3.38016f, -15.63073f, 16.90314f}, {2.77354f, -15.21255f, 16.98788f}, {3.10943f, -15.54915f, 17.31990f}, {2.65161f, -15.56832f, 16.76363f}, 
	{2.52807f, -15.21009f, 17.21763f}, {2.66583f, -15.61693f, 17.58131f}, {2.31349f, -15.53379f, 16.99180f}, {2.28086f, -15.32894f, 17.39883f}, 
	{2.45871f, -15.63766f, 17.59361f}, {2.07256f, -15.55682f, 17.19529f}, {2.05018f, -15.26224f, 17.51193f}, {2.22158f, -15.56677f, 17.83329f}, 
	{1.91171f, -15.58698f, 17.29058f}, {1.64208f, -15.23464f, 17.63744f}, {1.82045f, -15.55449f, 18.00671f}, {1.55822f, -15.60630f, 17.41406f}, 
	{1.39797f, -15.26302f, 17.74549f}, {1.42857f, -15.57474f, 18.07819f}, {1.34179f, -15.57650f, 17.49630f}, {1.09002f, -15.29629f, 17.81794f}, 
	{1.15530f, -15.57967f, 18.09600f}, {1.05705f, -15.58860f, 17.53312f}, {0.86773f, -15.22234f, 17.90417f}, {0.91419f, -15.54442f, 18.17885f}, 
	{0.84161f, -15.60649f, 17.56141f}, {0.50738f, -15.22840f, 17.95647f}, {0.55731f, -15.49977f, 18.26960f}, {0.52368f, -15.62257f, 17.59078f}, 
	{0.14971f, -15.23919f, 18.00756f}, {0.21833f, -15.55533f, 18.28294f}, {0.25325f, -15.61909f, 17.62784f}, {2.51819f, -15.92006f, 16.53624f}, 
	{2.19543f, -15.94612f, 16.77547f}, {1.96198f, -16.02412f, 16.97507f}, {1.73088f, -16.00174f, 17.10364f}, {1.49454f, -15.97343f, 17.17772f}, 
	{1.25706f, -15.97972f, 17.27908f}, {1.00215f, -16.01999f, 17.29433f}, {0.82416f, -16.02423f, 17.33266f}, {0.52705f, -16.02871f, 17.35019f}, 
	{0.72709f, -14.32095f, 16.56489f}, {0.32841f, -14.31169f, 16.75926f}, {0.38094f, -14.29444f, 16.36507f}, {1.82198f, -14.41187f, 16.62791f}, 
	{1.61128f, -14.36396f, 16.97133f}, {1.38825f, -14.33244f, 16.37873f}, {2.50696f, -14.44761f, 16.49971f}, {2.27876f, -14.43572f, 16.50359f}, 
	{2.12179f, -14.43648f, 16.02854f}, {2.49094f, -14.46718f, 16.19481f}, {3.56213f, -14.48246f, 13.43287f}, {3.60937f, -14.48697f, 13.76761f}, 
	{3.28772f, -14.36064f, 13.62865f}, {1.53135f, -13.69690f, 14.88033f}, {2.31259f, -14.44121f, 15.80257f}, {2.52648f, -14.41275f, 15.53080f}, 
	{1.71875f, -13.73126f, 14.64669f}, {2.67715f, -14.37932f, 15.29957f}, {0.36795f, -13.75498f, 15.32174f}, {0.39069f, -13.21574f, 14.22202f}, 
	{0.72152f, -13.68502f, 15.35318f}, {1.03115f, -13.73321f, 15.27867f}, {0.61663f, -13.21907f, 14.17500f}, {1.22292f, -13.73250f, 15.14480f}, 
	{0.86575f, -13.24500f, 14.03518f}, {-0.00000f, -14.22040f, 17.18481f}, {0.42596f, -14.30790f, 16.87882f}, {-0.00000f, -14.21753f, 17.27532f}, 
	{-0.00000f, -14.35106f, 16.53291f}, {0.42596f, -13.96620f, 16.86798f}, {-0.00000f, -13.87582f, 17.26448f}, {0.75469f, -14.31607f, 16.71865f}, 
	{0.75469f, -13.97436f, 16.70781f}, {1.11639f, -14.32411f, 16.75473f}, {1.17684f, -14.32871f, 16.60985f}, {1.06221f, -14.27336f, 16.26356f}, 
	{1.11639f, -13.98240f, 16.74389f}, {1.64840f, -14.35687f, 17.19474f}, {1.64840f, -14.01517f, 17.18390f}, {1.92850f, -14.40827f, 16.74137f}, 
	{1.92850f, -14.06656f, 16.73053f}, {2.08912f, -14.41910f, 16.65692f}, {2.05056f, -14.42385f, 16.50746f}, {1.86331f, -14.41674f, 16.20200f}, 
	{2.08913f, -14.07740f, 16.64608f}, {2.28162f, -14.43083f, 16.65784f}, {2.28162f, -14.08913f, 16.64700f}, {2.66486f, -14.44295f, 16.64653f}, 
	{2.66486f, -14.10124f, 16.63569f}, {2.64222f, -14.46688f, 16.20431f}, {2.64222f, -14.12517f, 16.19347f}, {2.68423f, -14.44722f, 15.96186f}, 
	{2.54541f, -14.44946f, 15.89092f}, {2.68423f, -14.10551f, 15.95102f}, {2.82854f, -14.40701f, 15.81554f}, {2.74657f, -14.41124f, 15.68204f}, 
	{2.82854f, -14.06530f, 15.80470f}, {3.35828f, -14.46696f, 15.65465f}, {3.16074f, -14.47010f, 15.55544f}, {3.35828f, -14.12525f, 15.64381f}, 
	{3.12794f, -14.37069f, 15.02597f}, {2.97315f, -14.37007f, 15.04518f}, {2.76140f, -14.35954f, 14.96293f}, {3.12794f, -14.02898f, 15.01512f}, 
	{3.15213f, -14.41092f, 14.77851f}, {3.00694f, -14.41278f, 14.72005f}, {2.82699f, -14.36529f, 14.59956f}, {3.15213f, -14.06922f, 14.76767f}, 
	{3.44484f, -14.41302f, 14.40260f}, {3.35508f, -14.41714f, 14.27262f}, {3.13820f, -14.37505f, 14.14969f}, {3.44484f, -14.07131f, 14.39176f}, 
	{4.01284f, -14.52965f, 14.23686f}, {3.79526f, -14.53256f, 14.14502f}, {3.26578f, -14.37258f, 13.96656f}, {4.01284f, -14.18794f, 14.22602f}, 
	{3.75443f, -14.48838f, 13.72292f}, {3.75443f, -14.14668f, 13.71208f}, {3.71720f, -14.48166f, 13.45810f}, {3.71720f, -14.13995f, 13.44726f}, 
	{3.83603f, -14.45927f, 13.22224f}, {3.71376f, -14.46223f, 13.12906f}, {3.55073f, -14.40663f, 13.06677f}, {3.83603f, -14.11757f, 13.21140f}, 
	{4.24234f, -14.51748f, 12.86190f}, {4.04555f, -14.51840f, 12.83299f}, {3.59937f, -14.37066f, 12.75656f}, {4.24234f, -14.17577f, 12.85106f}, 
	{3.90234f, -14.40721f, 12.32973f}, {3.76070f, -14.40557f, 12.38144f}, {3.54783f, -14.30383f, 12.32359f}, {3.90234f, -14.06550f, 12.31890f}, 
	{3.82850f, -14.32867f, 11.77876f}, {3.67774f, -14.32868f, 11.77837f}, {3.44843f, -14.20586f, 11.76442f}, {3.82850f, -13.98697f, 11.76792f}, 
	{3.90420f, -14.29576f, 11.25363f}, {3.75859f, -14.29717f, 11.20891f}, {3.54643f, -14.22849f, 11.07434f}, {3.90420f, -13.95405f, 11.24279f}, 
	{4.18760f, -14.32667f, 10.67324f}, {4.07362f, -14.33012f, 10.56474f}, {3.89828f, -14.23170f, 10.54354f}, {4.18760f, -13.98497f, 10.66240f}, 
	{4.85433f, -14.47293f, 10.32159f}, {4.63541f, -14.47472f, 10.26511f}, {4.21981f, -14.30466f, 10.12483f}, {4.85433f, -14.13122f, 10.31075f}, 
	{4.45757f, -14.36281f, 9.71216f}, {4.31477f, -14.36122f, 9.76214f}, {4.16550f, -14.22626f, 9.73578f}, {4.45757f, -14.02110f, 9.70132f}, 
	{4.41799f, -14.29323f, 9.32536f}, {4.26688f, -14.29394f, 9.30293f}, {4.10616f, -14.10980f, 9.25252f}, {4.41799f, -13.95152f, 9.31452f}, 
	{4.66309f, -14.19808f, 8.74831f}, {4.53827f, -14.20104f, 8.65502f}, {4.37726f, -14.01520f, 8.57037f}, {4.66309f, -13.85638f, 8.73747f}, 
	{5.23008f, -14.27848f, 8.28500f}, {5.17371f, -14.28323f, 8.13535f}, {5.05083f, -14.06900f, 7.95745f}, {5.23008f, -13.93678f, 8.27416f}, 
	{5.87165f, -14.14906f, 8.25810f}, {5.91321f, -14.15375f, 8.11028f}, {5.94581f, -13.90185f, 7.97614f}, {5.87165f, -13.80735f, 8.24726f}, 
	{6.56856f, -13.90228f, 8.70266f}, {6.69562f, -13.90503f, 8.61619f}, {6.75116f, -13.70451f, 8.50625f}, {6.56856f, -13.56058f, 8.69182f}, 
	{6.67838f, -13.61204f, 9.32372f}, {6.83181f, -13.61233f, 9.31444f}, {6.94688f, -13.33924f, 9.34402f}, {6.67838f, -13.27033f, 9.31288f}, 
	{6.63488f, -13.65275f, 10.11368f}, {6.78117f, -13.65193f, 10.13974f}, {6.92333f, -13.30529f, 10.14440f}, {6.63488f, -13.31105f, 10.10284f}, 
	{6.53750f, -13.84562f, 10.48545f}, {6.65424f, -13.84352f, 10.55172f}, {6.74499f, -13.63192f, 10.73399f}, {6.53750f, -13.50392f, 10.47461f}, 
	{6.14803f, -14.10782f, 10.86327f}, {6.34558f, -14.10671f, 10.89798f}, {6.60500f, -13.71290f, 10.99707f}, {6.14803f, -13.76611f, 10.85243f}, 
	{6.33852f, -13.90784f, 11.27719f}, {6.48160f, -13.90873f, 11.24915f}, {6.58308f, -13.62083f, 11.29738f}, {6.33852f, -13.56614f, 11.26635f}, 
	{6.35077f, -13.79372f, 11.77365f}, {6.50223f, -13.79323f, 11.78913f}, {6.58445f, -13.45578f, 11.78257f}, {6.35077f, -13.45202f, 11.76281f}, 
	{6.19511f, -13.79560f, 12.41722f}, {6.33773f, -13.79415f, 12.46285f}, {6.44299f, -13.51916f, 12.44806f}, {6.19511f, -13.45389f, 12.40638f}, 
	{6.04288f, -13.99222f, 12.84947f}, {6.15098f, -13.98970f, 12.92890f}, {6.25147f, -13.64231f, 12.91453f}, {6.04288f, -13.65051f, 12.83862f}, 
	{5.55204f, -14.33145f, 13.14115f}, {5.76891f, -14.32854f, 13.23271f}, {6.05945f, -13.77185f, 13.21888f}, {5.55204f, -13.98974f, 13.13031f}, 
	{5.71460f, -13.98652f, 13.53464f}, {5.83831f, -13.98674f, 13.52773f}, {6.02099f, -13.66592f, 13.52266f}, {5.71460f, -13.64482f, 13.52380f}, 
	{5.64518f, -13.71096f, 14.00971f}, {5.79951f, -13.70934f, 14.06078f}, {5.93254f, -13.47344f, 14.03772f}, {5.64518f, -13.36926f, 13.99887f}, 
	{5.40828f, -13.82202f, 14.43772f}, {5.50828f, -13.81874f, 14.54099f}, {5.58040f, -13.50936f, 14.59071f}, {5.40828f, -13.48032f, 14.42688f}, 
	{4.91511f, -14.09707f, 14.63695f}, {5.15486f, -14.09422f, 14.72694f}, {5.46483f, -13.59181f, 14.78762f}, {4.91511f, -13.75537f, 14.62611f}, 
	{5.15344f, -13.83942f, 15.02557f}, {5.28371f, -13.83966f, 15.01822f}, {5.43289f, -13.50826f, 15.03215f}, {5.15344f, -13.49772f, 15.01473f}, 
	{5.00216f, -13.55412f, 15.44687f}, {5.15120f, -13.55205f, 15.51202f}, {5.23019f, -13.30472f, 15.54178f}, {5.00216f, -13.21241f, 15.43603f}, 
	{4.75919f, -13.69742f, 15.88922f}, {4.85745f, -13.69419f, 15.99113f}, {4.93736f, -13.41666f, 16.00085f}, {4.75919f, -13.35572f, 15.87838f}, 
	{4.27868f, -13.97811f, 16.08354f}, {4.47419f, -13.97447f, 16.19845f}, {4.76244f, -13.54688f, 16.26746f}, {4.27868f, -13.63641f, 16.07270f}, 
	{4.39457f, -13.62747f, 16.54840f}, {4.51100f, -13.62663f, 16.57481f}, {4.67535f, -13.27418f, 16.59589f}, {4.39457f, -13.28577f, 16.53756f}, 
	{4.08179f, -13.17368f, 16.96210f}, {4.20711f, -13.16954f, 17.09285f}, {4.29199f, -12.94203f, 17.07018f}, {4.08179f, -12.83198f, 16.95126f}, 
	{3.56095f, -13.50033f, 17.29600f}, {3.60214f, -13.49584f, 17.43728f}, {3.64225f, -13.20471f, 17.52091f}, {3.56095f, -13.15862f, 17.28517f}, 
	{3.20674f, -13.55502f, 17.32402f}, {3.27785f, -13.55052f, 17.46586f}, {3.35099f, -13.37244f, 17.64397f}, {3.20674f, -13.21332f, 17.31318f}, 
	{2.98089f, -13.60288f, 17.56748f}, {3.08280f, -13.59904f, 17.68866f}, {3.17077f, -13.34475f, 17.76174f}, {2.98089f, -13.26118f, 17.55664f}, 
	{2.65946f, -13.40576f, 17.75524f}, {2.71922f, -13.40118f, 17.89931f}, {2.75417f, -13.17713f, 18.02040f}, {2.65946f, -13.06405f, 17.74441f}, 
	{2.23923f, -13.46488f, 17.87834f}, {2.24959f, -13.46019f, 18.02616f}, {2.28295f, -13.19922f, 18.17483f}, {2.23923f, -13.12317f, 17.86750f}, 
	{1.88788f, -13.62045f, 17.80650f}, {1.94962f, -13.61468f, 17.98829f}, {1.98157f, -13.28521f, 18.20757f}, {1.88788f, -13.27875f, 17.79566f}, 
	{1.60108f, -13.38147f, 18.15053f}, {1.67541f, -13.37771f, 18.26922f}, {1.74848f, -13.16791f, 18.35342f}, {1.60108f, -13.03977f, 18.13969f}, 
	{1.00974f, -13.13998f, 18.32567f}, {1.03124f, -13.13504f, 18.48151f}, {1.00084f, -12.89576f, 18.60852f}, {1.00974f, -12.79828f, 18.31483f}, 
	{0.33507f, -13.26397f, 18.30963f}, {0.29998f, -13.25948f, 18.45119f}, {0.29303f, -12.96375f, 18.60188f}, {0.33507f, -12.92226f, 18.29879f}, 
	{-0.00000f, -13.47231f, 18.19103f}, {-0.00000f, -13.46867f, 18.30555f}, {-0.00000f, -13.12674f, 18.56674f}, {-0.00000f, -13.13060f, 18.18019f}, 
	{-0.00000f, -13.80197f, 15.23230f}, {1.90771f, -13.77517f, 14.42526f}, {2.16099f, -13.76580f, 14.12460f}, {2.28335f, -13.69494f, 13.87021f}, 
	{2.49690f, -13.74192f, 13.64816f}, {2.56474f, -13.63023f, 13.34029f}, {2.62912f, -13.50894f, 12.92119f}, {2.66922f, -13.51356f, 12.66665f}, 
	{2.71426f, -13.44698f, 12.35655f}, {2.75927f, -13.39556f, 11.65097f}, {2.91995f, -13.33857f, 11.07183f}, {3.19623f, -13.43587f, 10.49493f}, 
	{3.33625f, -13.38280f, 9.98888f}, {3.39215f, -13.39917f, 9.62633f}, {3.47323f, -13.35619f, 9.09301f}, {3.81639f, -13.23476f, 8.31337f}, 
	{4.73569f, -13.19139f, 7.59767f}, {-0.00000f, -13.27058f, 14.24427f}, {1.00581f, -13.20721f, 13.86222f}, {1.24228f, -13.11388f, 13.66959f}, 
	{1.44130f, -13.09627f, 13.49922f}, {1.59061f, -13.05949f, 13.24921f}, {1.81756f, -13.01527f, 13.05133f}, {1.93667f, -12.92999f, 12.81237f}, 
	{2.01720f, -12.90616f, 12.59496f}, {2.03665f, -12.80042f, 12.28705f}, {2.23758f, -12.70372f, 11.69096f}, {2.43043f, -12.61512f, 11.01644f}, 
	{2.56368f, -12.50157f, 10.45058f}, {2.62945f, -12.48242f, 10.05343f}, {2.71208f, -12.45326f, 9.60267f}, {2.82467f, -12.46189f, 9.11129f}, 
	{3.16360f, -12.46739f, 8.09996f}, {4.44895f, -12.09021f, 7.31385f}, {5.67945f, -12.01191f, 7.47319f}, {5.83315f, -12.92589f, 7.68506f}, 
	{7.06682f, -11.85044f, 7.98458f}, {6.93284f, -12.78364f, 8.08360f}, {7.15305f, -12.61161f, 9.26674f}, {-0.00000f, -12.47650f, 18.65700f}, 
	{0.35528f, -12.43222f, 18.60138f}, {1.02169f, -12.38612f, 18.58930f}, {1.79065f, -12.35379f, 18.37433f}, {2.07693f, -12.34036f, 18.24533f}, 
	{2.33579f, -12.38779f, 18.15079f}, {2.77510f, -12.36219f, 18.02648f}, {3.21267f, -12.38431f, 17.80277f}, {3.37145f, -12.36492f, 17.65861f}, 
	{3.61719f, -12.37188f, 17.45855f}, {4.32590f, -12.37283f, 17.02733f}, {4.68741f, -12.39460f, 16.57882f}, {4.84809f, -12.42329f, 16.29032f}, 
	{4.94577f, -12.45277f, 16.03511f}, {5.23917f, -12.44961f, 15.56093f}, {5.47534f, -12.51104f, 15.06664f}, {5.56444f, -12.52044f, 14.80626f}, 
	{5.65661f, -12.55924f, 14.48273f}, {5.87286f, -12.59787f, 14.07511f}, {6.08224f, -12.61541f, 13.43845f}, {6.13174f, -12.64735f, 13.17091f}, 
	{6.27429f, -12.63962f, 12.86046f}, {6.41470f, -12.67468f, 12.39076f}, {6.56777f, -12.67012f, 11.71930f}, {6.58192f, -12.67698f, 11.36892f}, 
	{6.69792f, -12.67636f, 10.92137f}, {6.79600f, -12.66417f, 10.65189f}, {6.96651f, -12.63114f, 10.01919f}, {-0.38095f, -14.29444f, 16.36507f}, 
	{-0.32842f, -14.31169f, 16.75926f}, {-0.72709f, -14.32095f, 16.56489f}, {-1.38826f, -14.33244f, 16.37873f}, {-1.61128f, -14.36396f, 16.97133f}, 
	{-1.82199f, -14.41187f, 16.62791f}, {-2.12179f, -14.43648f, 16.02854f}, {-2.27876f, -14.43572f, 16.50359f}, {-2.50696f, -14.44761f, 16.49971f}, 
	{-2.49095f, -14.46718f, 16.19481f}, {-3.28773f, -14.36064f, 13.62865f}, {-3.60937f, -14.48697f, 13.76761f}, {-3.56213f, -14.48246f, 13.43287f}, 
	{-2.31260f, -14.44121f, 15.80257f}, {-1.53135f, -13.69690f, 14.88033f}, {-2.67715f, -14.37932f, 15.29957f}, {-1.71876f, -13.73126f, 14.64669f}, 
	{-2.52648f, -14.41275f, 15.53080f}, {-0.72153f, -13.68502f, 15.35318f}, {-0.39069f, -13.21574f, 14.22202f}, {-0.36796f, -13.75498f, 15.32174f}, 
	{-1.22293f, -13.73250f, 15.14480f}, {-0.61663f, -13.21907f, 14.17500f}, {-1.03115f, -13.73321f, 15.27867f}, {-0.86575f, -13.24500f, 14.03517f}, 
	{-0.42597f, -14.30790f, 16.87882f}, {-0.42597f, -13.96620f, 16.86798f}, {-0.75469f, -14.31607f, 16.71865f}, {-0.75469f, -13.97437f, 16.70781f}, 
	{-1.11639f, -14.32411f, 16.75473f}, {-1.17685f, -14.32871f, 16.60985f}, {-1.06222f, -14.27336f, 16.26356f}, {-1.11639f, -13.98240f, 16.74389f}, 
	{-1.64840f, -14.35687f, 17.19474f}, {-1.64840f, -14.01517f, 17.18390f}, {-1.92851f, -14.40827f, 16.74137f}, {-1.92851f, -14.06656f, 16.73053f}, 
	{-2.08913f, -14.41910f, 16.65692f}, {-2.05057f, -14.42385f, 16.50746f}, {-1.86332f, -14.41674f, 16.20200f}, {-2.08913f, -14.07740f, 16.64608f}, 
	{-2.28163f, -14.43083f, 16.65784f}, {-2.28163f, -14.08913f, 16.64700f}, {-2.66486f, -14.44295f, 16.64653f}, {-2.66486f, -14.10124f, 16.63569f}, 
	{-2.64223f, -14.46688f, 16.20431f}, {-2.64223f, -14.12517f, 16.19347f}, {-2.68423f, -14.44722f, 15.96186f}, {-2.54542f, -14.44947f, 15.89092f}, 
	{-2.68423f, -14.10551f, 15.95102f}, {-2.82855f, -14.40701f, 15.81554f}, {-2.74658f, -14.41124f, 15.68204f}, {-2.82855f, -14.06530f, 15.80470f}, 
	{-3.35828f, -14.46696f, 15.65465f}, {-3.16074f, -14.47011f, 15.55544f}, {-3.35828f, -14.12525f, 15.64381f}, {-3.12795f, -14.37069f, 15.02597f}, 
	{-2.97316f, -14.37007f, 15.04518f}, {-2.76141f, -14.35954f, 14.96293f}, {-3.12795f, -14.02898f, 15.01512f}, {-3.15213f, -14.41092f, 14.77851f}, 
	{-3.00694f, -14.41278f, 14.72005f}, {-2.82699f, -14.36529f, 14.59956f}, {-3.15213f, -14.06922f, 14.76767f}, {-3.44484f, -14.41302f, 14.40260f}, 
	{-3.35509f, -14.41714f, 14.27262f}, {-3.13821f, -14.37505f, 14.14969f}, {-3.44484f, -14.07131f, 14.39176f}, {-4.01285f, -14.52965f, 14.23686f}, 
	{-3.79527f, -14.53256f, 14.14502f}, {-3.26578f, -14.37258f, 13.96656f}, {-4.01285f, -14.18794f, 14.22602f}, {-3.75444f, -14.48838f, 13.72292f}, 
	{-3.75444f, -14.14668f, 13.71208f}, {-3.71721f, -14.48166f, 13.45810f}, {-3.71721f, -14.13995f, 13.44726f}, {-3.83603f, -14.45927f, 13.22224f}, 
	{-3.71376f, -14.46223f, 13.12906f}, {-3.55073f, -14.40663f, 13.06677f}, {-3.83603f, -14.11757f, 13.21140f}, {-4.24235f, -14.51748f, 12.86190f}, 
	{-4.04555f, -14.51840f, 12.83299f}, {-3.59938f, -14.37066f, 12.75656f}, {-4.24235f, -14.17577f, 12.85106f}, {-3.90235f, -14.40721f, 12.32973f}, 
	{-3.76071f, -14.40557f, 12.38144f}, {-3.54784f, -14.30383f, 12.32359f}, {-3.90235f, -14.06550f, 12.31890f}, {-3.82850f, -14.32867f, 11.77876f}, 
	{-3.67774f, -14.32868f, 11.77837f}, {-3.44844f, -14.20586f, 11.76442f}, {-3.82850f, -13.98697f, 11.76792f}, {-3.90421f, -14.29576f, 11.25363f}, 
	{-3.75859f, -14.29717f, 11.20891f}, {-3.54643f, -14.22849f, 11.07434f}, {-3.90421f, -13.95405f, 11.24279f}, {-4.18760f, -14.32667f, 10.67324f}, 
	{-4.07363f, -14.33012f, 10.56474f}, {-3.89829f, -14.23170f, 10.54354f}, {-4.18760f, -13.98497f, 10.66240f}, {-4.85434f, -14.47293f, 10.32159f}, 
	{-4.63542f, -14.47472f, 10.26511f}, {-4.21982f, -14.30466f, 10.12483f}, {-4.85434f, -14.13122f, 10.31075f}, {-4.45758f, -14.36281f, 9.71216f}, 
	{-4.31477f, -14.36122f, 9.76214f}, {-4.16551f, -14.22627f, 9.73578f}, {-4.45758f, -14.02111f, 9.70132f}, {-4.41800f, -14.29323f, 9.32536f}, 
	{-4.26688f, -14.29394f, 9.30293f}, {-4.10616f, -14.10980f, 9.25252f}, {-4.41800f, -13.95152f, 9.31452f}, {-4.66310f, -14.19809f, 8.74831f}, 
	{-4.53827f, -14.20104f, 8.65502f}, {-4.37726f, -14.01520f, 8.57037f}, {-4.66310f, -13.85638f, 8.73747f}, {-5.23009f, -14.27848f, 8.28500f}, 
	{-5.17372f, -14.28323f, 8.13535f}, {-5.05083f, -14.06900f, 7.95745f}, {-5.23009f, -13.93678f, 8.27416f}, {-5.87166f, -14.14906f, 8.25810f}, 
	{-5.91322f, -14.15375f, 8.11028f}, {-5.94582f, -13.90185f, 7.97614f}, {-5.87166f, -13.80735f, 8.24726f}, {-6.56856f, -13.90229f, 8.70266f}, 
	{-6.69563f, -13.90503f, 8.61619f}, {-6.75116f, -13.70451f, 8.50625f}, {-6.56856f, -13.56058f, 8.69182f}, {-6.67838f, -13.61204f, 9.32372f}, 
	{-6.83182f, -13.61233f, 9.31444f}, {-6.94689f, -13.33924f, 9.34402f}, {-6.67838f, -13.27033f, 9.31288f}, {-6.63488f, -13.65276f, 10.11368f}, 
	{-6.78117f, -13.65193f, 10.13974f}, {-6.92333f, -13.30529f, 10.14440f}, {-6.63488f, -13.31105f, 10.10284f}, {-6.53750f, -13.84562f, 10.48545f}, 
	{-6.65424f, -13.84352f, 10.55172f}, {-6.74499f, -13.63192f, 10.73399f}, {-6.53750f, -13.50392f, 10.47461f}, {-6.14803f, -14.10782f, 10.86327f}, 
	{-6.34559f, -14.10671f, 10.89798f}, {-6.60501f, -13.71290f, 10.99707f}, {-6.14803f, -13.76611f, 10.85243f}, {-6.33852f, -13.90784f, 11.27719f}, 
	{-6.48161f, -13.90873f, 11.24915f}, {-6.58309f, -13.62083f, 11.29738f}, {-6.33852f, -13.56614f, 11.26635f}, {-6.35078f, -13.79372f, 11.77365f}, 
	{-6.50223f, -13.79323f, 11.78913f}, {-6.58446f, -13.45578f, 11.78257f}, {-6.35078f, -13.45202f, 11.76281f}, {-6.19512f, -13.79560f, 12.41722f}, 
	{-6.33774f, -13.79415f, 12.46285f}, {-6.44299f, -13.51916f, 12.44806f}, {-6.19512f, -13.45389f, 12.40638f}, {-6.04288f, -13.99222f, 12.84947f}, 
	{-6.15099f, -13.98970f, 12.92890f}, {-6.25147f, -13.64231f, 12.91453f}, {-6.04288f, -13.65052f, 12.83862f}, {-5.55205f, -14.33145f, 13.14115f}, 
	{-5.76892f, -14.32854f, 13.23271f}, {-6.05945f, -13.77185f, 13.21888f}, {-5.55205f, -13.98975f, 13.13031f}, {-5.71460f, -13.98652f, 13.53464f}, 
	{-5.83831f, -13.98674f, 13.52773f}, {-6.02100f, -13.66592f, 13.52266f}, {-5.71460f, -13.64482f, 13.52380f}, {-5.64518f, -13.71096f, 14.00971f}, 
	{-5.79951f, -13.70934f, 14.06078f}, {-5.93255f, -13.47344f, 14.03772f}, {-5.64518f, -13.36926f, 13.99887f}, {-5.40828f, -13.82202f, 14.43772f}, 
	{-5.50829f, -13.81875f, 14.54099f}, {-5.58041f, -13.50936f, 14.59071f}, {-5.40828f, -13.48032f, 14.42688f}, {-4.91512f, -14.09707f, 14.63695f}, 
	{-5.15486f, -14.09422f, 14.72694f}, {-5.46483f, -13.59181f, 14.78762f}, {-4.91512f, -13.75537f, 14.62611f}, {-5.15344f, -13.83942f, 15.02557f}, 
	{-5.28371f, -13.83966f, 15.01822f}, {-5.43290f, -13.50826f, 15.03215f}, {-5.15344f, -13.49772f, 15.01473f}, {-5.00216f, -13.55412f, 15.44687f}, 
	{-5.15121f, -13.55205f, 15.51202f}, {-5.23019f, -13.30472f, 15.54178f}, {-5.00216f, -13.21241f, 15.43603f}, {-4.75920f, -13.69742f, 15.88922f}, 
	{-4.85746f, -13.69419f, 15.99113f}, {-4.93737f, -13.41666f, 16.00085f}, {-4.75920f, -13.35572f, 15.87838f}, {-4.27869f, -13.97812f, 16.08354f}, 
	{-4.47420f, -13.97447f, 16.19845f}, {-4.76245f, -13.54688f, 16.26746f}, {-4.27869f, -13.63641f, 16.07270f}, {-4.39457f, -13.62748f, 16.54840f}, 
	{-4.51100f, -13.62663f, 16.57481f}, {-4.67535f, -13.27418f, 16.59589f}, {-4.39457f, -13.28577f, 16.53756f}, {-4.08179f, -13.17368f, 16.96210f}, 
	{-4.20712f, -13.16954f, 17.09285f}, {-4.29200f, -12.94203f, 17.07018f}, {-4.08179f, -12.83198f, 16.95126f}, {-3.56095f, -13.50033f, 17.29600f}, 
	{-3.60215f, -13.49584f, 17.43728f}, {-3.64226f, -13.20471f, 17.52091f}, {-3.56095f, -13.15862f, 17.28516f}, {-3.20675f, -13.55502f, 17.32402f}, 
	{-3.27786f, -13.55052f, 17.46586f}, {-3.35099f, -13.37244f, 17.64397f}, {-3.20675f, -13.21332f, 17.31318f}, {-2.98090f, -13.60288f, 17.56748f}, 
	{-3.08280f, -13.59904f, 17.68866f}, {-3.17078f, -13.34475f, 17.76174f}, {-2.98090f, -13.26118f, 17.55664f}, {-2.65946f, -13.40576f, 17.75524f}, 
	{-2.71922f, -13.40118f, 17.89931f}, {-2.75417f, -13.17713f, 18.02040f}, {-2.65946f, -13.06405f, 17.74441f}, {-2.23924f, -13.46488f, 17.87834f}, 
	{-2.24959f, -13.46019f, 18.02616f}, {-2.28296f, -13.19922f, 18.17483f}, {-2.23924f, -13.12318f, 17.86750f}, {-1.88789f, -13.62045f, 17.80650f}, 
	{-1.94962f, -13.61468f, 17.98829f}, {-1.98157f, -13.28521f, 18.20757f}, {-1.88789f, -13.27875f, 17.79566f}, {-1.60108f, -13.38147f, 18.15053f}, 
	{-1.67541f, -13.37771f, 18.26922f}, {-1.74849f, -13.16791f, 18.35342f}, {-1.60108f, -13.03977f, 18.13969f}, {-1.00975f, -13.13998f, 18.32567f}, 
	{-1.03125f, -13.13504f, 18.48151f}, {-1.00084f, -12.89576f, 18.60852f}, {-1.00975f, -12.79828f, 18.31483f}, {-0.33507f, -13.26397f, 18.30963f}, 
	{-0.29999f, -13.25948f, 18.45119f}, {-0.29304f, -12.96375f, 18.60188f}, {-0.33507f, -12.92226f, 18.29879f}, {-1.90771f, -13.77517f, 14.42526f}, 
	{-2.16100f, -13.76580f, 14.12460f}, {-2.28336f, -13.69494f, 13.87021f}, {-2.49691f, -13.74192f, 13.64816f}, {-2.56474f, -13.63023f, 13.34029f}, 
	{-2.62912f, -13.50894f, 12.92119f}, {-2.66923f, -13.51356f, 12.66665f}, {-2.71427f, -13.44698f, 12.35654f}, {-2.75928f, -13.39556f, 11.65097f}, 
	{-2.91996f, -13.33857f, 11.07183f}, {-3.19624f, -13.43587f, 10.49493f}, {-3.33626f, -13.38280f, 9.98888f}, {-3.39216f, -13.39917f, 9.62632f}, 
	{-3.47323f, -13.35619f, 9.09301f}, {-3.81639f, -13.23476f, 8.31337f}, {-4.73570f, -13.19139f, 7.59767f}, {-1.00582f, -13.20721f, 13.86222f}, 
	{-1.24228f, -13.11389f, 13.66959f}, {-1.44131f, -13.09627f, 13.49922f}, {-1.59062f, -13.05949f, 13.24921f}, {-1.81756f, -13.01527f, 13.05133f}, 
	{-1.93667f, -12.92999f, 12.81237f}, {-2.01721f, -12.90616f, 12.59496f}, {-2.03666f, -12.80042f, 12.28705f}, {-2.23759f, -12.70372f, 11.69096f}, 
	{-2.43043f, -12.61512f, 11.01644f}, {-2.56368f, -12.50157f, 10.45058f}, {-2.62945f, -12.48242f, 10.05343f}, {-2.71209f, -12.45326f, 9.60267f}, 
	{-2.82467f, -12.46189f, 9.11129f}, {-3.16360f, -12.46739f, 8.09996f}, {-4.44896f, -12.09021f, 7.31385f}, {-5.83315f, -12.92589f, 7.68506f}, 
	{-5.67946f, -12.01191f, 7.47319f}, {-6.93284f, -12.78364f, 8.08360f}, {-7.06683f, -11.85044f, 7.98459f}, {-7.15306f, -12.61161f, 9.26674f}, 
	{-0.35529f, -12.43222f, 18.60138f}, {-1.02169f, -12.38612f, 18.58930f}, {-1.79065f, -12.35380f, 18.37433f}, {-2.07694f, -12.34037f, 18.24533f}, 
	{-2.33579f, -12.38779f, 18.15079f}, {-2.77511f, -12.36219f, 18.02648f}, {-3.21268f, -12.38431f, 17.80277f}, {-3.37146f, -12.36492f, 17.65861f}, 
	{-3.61719f, -12.37188f, 17.45855f}, {-4.32590f, -12.37283f, 17.02733f}, {-4.68742f, -12.39460f, 16.57882f}, {-4.84810f, -12.42329f, 16.29032f}, 
	{-4.94578f, -12.45277f, 16.03511f}, {-5.23918f, -12.44961f, 15.56093f}, {-5.47535f, -12.51104f, 15.06664f}, {-5.56445f, -12.52044f, 14.80626f}, 
	{-5.65661f, -12.55924f, 14.48273f}, {-5.87287f, -12.59787f, 14.07511f}, {-6.08224f, -12.61541f, 13.43845f}, {-6.13174f, -12.64735f, 13.17091f}, 
	{-6.27430f, -12.63962f, 12.86046f}, {-6.41470f, -12.67468f, 12.39076f}, {-6.56777f, -12.67013f, 11.71930f}, {-6.58192f, -12.67698f, 11.36891f}, 
	{-6.69792f, -12.67636f, 10.92137f}, {-6.79601f, -12.66417f, 10.65189f}, {-6.96652f, -12.63114f, 10.01919f}, {4.13468f, -15.02420f, 15.54640f}, 
	{3.82740f, -14.96591f, 15.92857f}, {3.74621f, -15.19755f, 15.59255f}, {-0.00000f, -12.92663f, 18.15734f}, {0.29998f, -13.25947f, 18.45119f}, 
	{0.34008f, -12.71841f, 18.27208f}, {0.35244f, -13.65313f, 18.66025f}, {-0.00000f, -13.93099f, 18.44769f}, {1.03124f, -13.13504f, 18.48151f}, 
	{1.00667f, -12.59450f, 18.28608f}, {1.02335f, -13.56288f, 18.61986f}, {1.67540f, -13.37771f, 18.26922f}, {1.59045f, -12.83582f, 18.11625f}, 
	{1.75262f, -13.72003f, 18.32630f}, {1.94961f, -13.61468f, 17.98829f}, {1.87906f, -13.07508f, 17.76321f}, {1.91919f, -13.83809f, 18.05013f}, 
	{2.24958f, -13.46019f, 18.02616f}, {2.23775f, -12.91936f, 17.83990f}, {2.26863f, -13.79035f, 18.12881f}, {2.71921f, -13.40118f, 17.89931f}, 
	{2.65092f, -12.86021f, 17.71733f}, {2.74079f, -13.78926f, 18.01024f}, {3.08279f, -13.59904f, 17.68866f}, {2.96633f, -13.05724f, 17.53284f}, 
	{3.14898f, -13.84731f, 17.65216f}, {3.27785f, -13.55052f, 17.46586f}, {3.19658f, -13.00947f, 17.28643f}, {3.24292f, -14.03157f, 17.47106f}, 
	{3.60213f, -13.49584f, 17.43728f}, {3.55505f, -12.95477f, 17.25850f}, {3.54309f, -13.94153f, 17.52708f}, {4.20711f, -13.16954f, 17.09285f}, 
	{4.06388f, -12.62808f, 16.92610f}, {4.37125f, -14.07027f, 17.22761f}, {4.51099f, -13.62663f, 16.57481f}, {4.37792f, -13.08140f, 16.52730f}, 
	{4.55450f, -14.00316f, 16.51990f}, {4.47419f, -13.97447f, 16.19845f}, {4.25074f, -13.43244f, 16.04980f}, {4.52974f, -14.21761f, 16.26624f}, 
	{4.85745f, -13.69419f, 15.99113f}, {4.74515f, -13.15169f, 15.85734f}, {4.91675f, -14.16605f, 16.09461f}, {5.15120f, -13.55205f, 15.51202f}, 
	{4.98086f, -13.00822f, 15.42024f}, {5.28017f, -14.17299f, 15.59886f}, {5.28370f, -13.83966f, 15.01822f}, {5.13482f, -13.29319f, 15.00929f}, 
	{5.31738f, -14.32092f, 15.02809f}, {5.15485f, -14.09422f, 14.72694f}, {4.88086f, -13.55129f, 14.60677f}, {5.19026f, -14.26432f, 14.72937f}, 
	{5.50827f, -13.81874f, 14.54099f}, {5.39399f, -13.27630f, 14.40564f}, {5.51660f, -14.33898f, 14.61036f}, {5.79950f, -13.70934f, 14.06078f}, 
	{5.62313f, -13.16500f, 13.98509f}, {5.86655f, -14.26633f, 14.17021f}, {5.83830f, -13.98674f, 13.52774f}, {5.69692f, -13.44030f, 13.51830f}, 
	{5.89421f, -14.34407f, 13.57284f}, {5.76891f, -14.32854f, 13.23271f}, {5.52106f, -13.78567f, 13.11075f}, {5.80822f, -14.55184f, 13.22334f}, 
	{6.15098f, -13.98970f, 12.92891f}, {6.02743f, -13.44639f, 12.82079f}, {6.15394f, -14.53682f, 13.06111f}, {6.33773f, -13.79415f, 12.46285f}, 
	{6.17473f, -13.24961f, 12.39337f}, {6.41868f, -14.40445f, 12.59924f}, {6.50222f, -13.79323f, 11.78913f}, {6.32913f, -13.24760f, 11.75412f}, 
	{6.56577f, -14.34494f, 11.87968f}, {6.48160f, -13.90873f, 11.24915f}, {6.31807f, -13.36152f, 11.26387f}, {6.54450f, -14.42316f, 11.25985f}, 
	{6.34558f, -14.10671f, 10.89798f}, {6.11980f, -13.56178f, 10.84099f}, {6.33672f, -14.54401f, 10.84570f}, {6.65423f, -13.84352f, 10.55172f}, 
	{6.52081f, -13.29973f, 10.45865f}, {6.61667f, -14.37944f, 10.65804f}, {6.78116f, -13.65193f, 10.13974f}, {6.61397f, -13.10668f, 10.09263f}, 
	{6.89642f, -14.30092f, 10.22043f}, {6.83181f, -13.61233f, 9.31444f}, {6.65645f, -13.06580f, 9.30772f}, {6.92454f, -14.16453f, 9.36609f}, 
	{6.69562f, -13.90503f, 8.61620f}, {6.55040f, -13.35570f, 8.69769f}, {6.72931f, -14.20230f, 8.52197f}, {5.91321f, -14.15375f, 8.11028f}, 
	{5.86571f, -13.60219f, 8.26190f}, {5.89580f, -14.57948f, 8.12821f}, {5.17371f, -14.28323f, 8.13535f}, {5.23813f, -13.73161f, 8.28905f}, 
	{5.28923f, -14.65601f, 8.10244f}, {4.53826f, -14.20104f, 8.65502f}, {4.68092f, -13.65147f, 8.74432f}, {4.73198f, -14.73463f, 8.49628f}, 
	{4.26687f, -14.29394f, 9.30294f}, {4.43957f, -13.74693f, 9.31124f}, {4.38176f, -14.74529f, 9.29252f}, {4.31476f, -14.36122f, 9.76214f}, 
	{4.47797f, -13.81684f, 9.68770f}, {4.38246f, -14.72775f, 9.81364f}, {4.63541f, -14.47472f, 10.26511f}, {4.88560f, -13.92648f, 10.31233f}, 
	{4.66665f, -14.84467f, 10.32510f}, {4.07362f, -14.33011f, 10.56474f}, {4.20387f, -13.77999f, 10.67141f}, {4.25086f, -14.86383f, 10.46033f}, 
	{3.75858f, -14.29717f, 11.20891f}, {3.92500f, -13.74936f, 11.24269f}, {3.87856f, -14.86947f, 11.19261f}, {3.67773f, -14.32868f, 11.77837f}, 
	{3.85003f, -13.78248f, 11.76149f}, {3.73253f, -14.82859f, 11.77957f}, {3.76070f, -14.40557f, 12.38144f}, {3.92257f, -13.86125f, 12.30502f}, 
	{3.93214f, -14.86487f, 12.45432f}, {4.04554f, -14.51840f, 12.83299f}, {4.27045f, -13.97116f, 12.84870f}, {4.15659f, -14.81132f, 12.82293f}, 
	{3.71375f, -14.46223f, 13.12906f}, {3.85349f, -13.91266f, 13.21822f}, {3.78271f, -14.79514f, 13.06612f}, {3.56212f, -14.48246f, 13.43287f}, 
	{3.73935f, -13.93535f, 13.44439f}, {3.64670f, -14.88714f, 13.38296f}, {3.60936f, -14.48696f, 13.76761f}, {3.77515f, -13.94239f, 13.69921f}, 
	{3.71606f, -14.93742f, 13.81154f}, {3.79526f, -14.53256f, 14.14502f}, {4.04392f, -13.98304f, 14.23265f}, {3.87868f, -14.81158f, 14.15763f}, 
	{3.35508f, -14.41714f, 14.27263f}, {3.45765f, -13.86623f, 14.40384f}, {3.45263f, -14.75379f, 14.28149f}, {3.00693f, -14.41278f, 14.72005f}, 
	{3.17286f, -13.86446f, 14.76954f}, {3.08578f, -14.81553f, 14.74750f}, {2.97314f, -14.37007f, 15.04518f}, {3.15005f, -13.82458f, 15.00589f}, 
	{3.05875f, -14.76455f, 15.22075f}, {3.16073f, -14.47010f, 15.55544f}, {3.38649f, -13.92032f, 15.65149f}, {3.22833f, -14.63527f, 15.62157f}, 
	{2.74657f, -14.41124f, 15.68204f}, {2.84025f, -13.86021f, 15.81728f}, {2.96707f, -14.62317f, 15.71596f}, {2.54541f, -14.44946f, 15.89092f}, 
	{2.70405f, -13.90070f, 15.95466f}, {2.72039f, -14.69058f, 16.01638f}, {2.49094f, -14.46718f, 16.19481f}, {2.66383f, -13.92064f, 16.18834f}, 
	{2.65469f, -14.66596f, 16.29020f}, {2.50695f, -14.44761f, 16.49971f}, {2.68741f, -13.89609f, 16.65018f}, {2.64309f, -14.54827f, 16.68781f}, 
	{2.27875f, -14.43572f, 16.50359f}, {2.28203f, -13.88394f, 16.66255f}, {2.29217f, -14.59278f, 16.69123f}, {2.05056f, -14.42385f, 16.50746f}, 
	{2.09463f, -13.87223f, 16.66094f}, {2.07998f, -14.58625f, 16.79132f}, {1.82198f, -14.41187f, 16.62791f}, {1.94371f, -13.86156f, 16.74025f}, 
	{1.88019f, -14.55580f, 16.88878f}, {1.61127f, -14.36396f, 16.97133f}, {1.65370f, -13.80967f, 17.20932f}, {1.65946f, -14.50166f, 17.21528f}, 
	{1.17684f, -14.32871f, 16.60985f}, {1.10775f, -13.77726f, 16.75810f}, {1.29980f, -14.51921f, 16.89344f}, {0.72708f, -14.32095f, 16.56489f}, 
	{0.75862f, -13.76918f, 16.72328f}, {0.76706f, -14.55755f, 16.95950f}, {0.32841f, -14.31169f, 16.75926f}, {0.43989f, -13.76117f, 16.87858f}, 
	{0.39376f, -14.52030f, 17.00532f}, {-0.00000f, -14.22040f, 17.18480f}, {-0.00000f, -13.67092f, 17.27092f}, {-0.00000f, -14.33965f, 17.38059f}, 
	{5.98326f, -15.01354f, 8.33794f}, {6.72902f, -14.97209f, 8.65864f}, {6.85800f, -14.94044f, 9.46558f}, {6.42028f, -14.97918f, 10.80636f}, 
	{6.55167f, -15.09982f, 11.21962f}, {6.64886f, -15.16653f, 11.81881f}, {-0.00000f, -14.69092f, 17.69299f}, {0.36107f, -14.70566f, 17.64573f}, 
	{-0.00000f, -14.99443f, 18.07742f}, {0.36652f, -15.01926f, 18.10471f}, {0.88755f, -14.74410f, 17.62822f}, {0.99804f, -14.97703f, 17.97239f}, 
	{1.36232f, -14.76484f, 17.57707f}, {1.43940f, -14.99318f, 17.93017f}, {1.70603f, -14.73613f, 17.47490f}, {1.80122f, -15.05644f, 17.90900f}, 
	{2.04347f, -14.70946f, 17.36447f}, {2.07137f, -15.03670f, 17.78871f}, {2.25152f, -14.70590f, 17.18810f}, {2.44078f, -14.98754f, 17.58921f}, 
	{2.54543f, -14.71390f, 16.97501f}, {2.86171f, -14.98607f, 17.38363f}, {2.78826f, -14.67960f, 16.81033f}, {3.03953f, -14.94408f, 17.18726f}, 
	{3.10097f, -14.80367f, 16.64006f}, {3.27771f, -15.00124f, 16.92597f}, {3.22640f, -14.80209f, 16.30771f}, {3.53359f, -15.10479f, 16.57952f}, 
	{3.39036f, -14.81837f, 16.04323f}, {3.74884f, -14.97364f, 16.29472f}, {3.46924f, -14.85273f, 15.77990f}, {3.27299f, -15.12663f, 15.40744f}, 
	{3.28331f, -15.23162f, 14.85730f}, {3.78033f, -15.54918f, 15.04960f}, {3.73185f, -15.18259f, 14.39035f}, {4.12627f, -15.30794f, 14.56492f}, 
	{3.99090f, -15.05720f, 14.21663f}, {4.21302f, -15.17712f, 14.31426f}, {3.92489f, -15.28080f, 13.90840f}, {4.33895f, -15.44115f, 14.03182f}, 
	{3.87957f, -15.31211f, 13.52468f}, {4.40119f, -15.59353f, 13.56913f}, {4.08407f, -15.26757f, 13.07470f}, {4.46472f, -15.49231f, 13.20471f}, 
	{4.32438f, -15.12619f, 12.85655f}, {4.54654f, -15.27228f, 12.86077f}, {4.19391f, -15.30486f, 12.42281f}, {4.60755f, -15.53442f, 12.51499f}, 
	{4.21189f, -15.42523f, 11.88213f}, {4.70004f, -15.63234f, 11.97889f}, {4.29053f, -15.33344f, 11.29269f}, {4.76255f, -15.53553f, 11.33168f}, 
	{4.52429f, -15.40426f, 10.52705f}, {4.97070f, -15.64912f, 10.66659f}, {4.83483f, -15.27436f, 10.31640f}, {5.11608f, -15.36786f, 10.34821f}, 
	{4.71562f, -15.43497f, 9.88906f}, {5.13922f, -15.57019f, 10.04822f}, {4.75057f, -15.44049f, 9.35340f}, {5.22911f, -15.65321f, 9.67937f}, 
	{5.04105f, -15.29550f, 8.63163f}, {5.51844f, -15.32436f, 8.91588f}, {5.51245f, -15.10598f, 8.24637f}, {5.69827f, -15.23257f, 8.64071f}, 
	{5.99764f, -15.06624f, 8.76799f}, {4.43697f, -15.21407f, 16.18909f}, {4.56054f, -15.35598f, 15.94936f}, {4.27580f, -15.43056f, 16.57434f}, 
	{4.22978f, -15.27774f, 15.27074f}, {4.41292f, -15.11574f, 14.88615f}, {4.62016f, -15.14550f, 14.45608f}, {4.80579f, -15.07469f, 14.10695f}, 
	{4.87612f, -15.25943f, 13.81935f}, {5.02867f, -15.06900f, 13.50042f}, {5.16681f, -15.13955f, 13.14326f}, {5.32583f, -15.17739f, 12.65623f}, 
	{5.33369f, -15.25323f, 12.40877f}, {5.45619f, -15.26572f, 11.46243f}, {5.62709f, -15.15318f, 11.12074f}, {5.73576f, -15.15403f, 10.65074f}, 
	{5.79414f, -15.24406f, 10.36297f}, {5.86141f, -15.18372f, 9.99958f}, {5.97342f, -15.06996f, 9.24406f}, {6.50716f, -15.23703f, 8.83760f}, 
	{6.59441f, -15.32163f, 9.35196f}, {6.50861f, -15.51546f, 10.23624f}, {6.34248f, -15.39063f, 10.54564f}, {6.20074f, -15.22513f, 10.78008f}, 
	{6.17263f, -15.54238f, 11.20392f}, {6.12266f, -15.58364f, 11.74689f}, {5.92164f, -15.60150f, 12.56426f}, {5.72267f, -15.47505f, 12.97543f}, 
	{5.54737f, -15.25137f, 13.23369f}, {5.55368f, -15.37304f, 13.46441f}, {5.32307f, -15.58266f, 14.12108f}, {5.11359f, -15.39393f, 14.43936f}, 
	{4.92854f, -15.20452f, 14.66082f}, {4.95343f, -15.33371f, 14.90721f}, {4.73808f, -15.56567f, 15.53419f}, {1.86079f, -15.11266f, 18.19902f}, 
	{2.15336f, -15.31848f, 18.21219f}, {1.57231f, -15.32993f, 18.41984f}, {-0.00000f, -15.13895f, 18.56237f}, {0.36685f, -15.35750f, 18.52003f}, 
	{3.25030f, -15.18154f, 17.54325f}, {3.47635f, -15.31730f, 17.41687f}, {3.06881f, -15.27512f, 17.71044f}, {0.99939f, -15.36296f, 18.47755f}, 
	{2.58557f, -15.33124f, 18.02773f}, {4.01439f, -15.54715f, 17.12273f}, {-0.00000f, -14.78946f, 18.68510f}, {0.41897f, -14.75582f, 18.78615f}, 
	{0.97892f, -14.84860f, 18.73516f}, {1.79025f, -14.80464f, 18.51945f}, {1.94707f, -14.75423f, 18.34623f}, {2.27893f, -14.76085f, 18.28685f}, 
	{2.73718f, -14.79284f, 18.18449f}, {3.24488f, -14.74410f, 17.80289f}, {3.34792f, -14.72953f, 17.64614f}, {3.61424f, -14.78049f, 17.63069f}, 
	{4.28446f, -14.84284f, 17.33721f}, {4.51504f, -14.88797f, 16.71508f}, {4.61086f, -14.86856f, 16.25772f}, {4.90744f, -14.88679f, 16.00758f}, 
	{5.20099f, -14.96437f, 15.65736f}, {5.23682f, -14.96849f, 15.09716f}, {5.25666f, -14.98780f, 14.74200f}, {5.50971f, -14.98387f, 14.51853f}, 
	{5.67602f, -14.96324f, 14.36784f}, {5.86568f, -15.02581f, 13.62217f}, {5.86882f, -15.03100f, 13.29315f}, {6.15391f, -15.03788f, 13.08459f}, 
	{6.36248f, -15.10330f, 12.72152f}, {6.64570f, -15.03455f, 10.59976f}, {6.81723f, -15.06858f, 10.37344f}, {-3.74622f, -15.19755f, 15.59255f}, 
	{-3.82740f, -14.96591f, 15.92857f}, {-4.13469f, -15.02420f, 15.54640f}, {-0.34009f, -12.71841f, 18.27208f}, {-0.29998f, -13.25948f, 18.45119f}, 
	{-0.35245f, -13.65313f, 18.66025f}, {-1.00667f, -12.59450f, 18.28608f}, {-1.03125f, -13.13504f, 18.48151f}, {-1.02335f, -13.56288f, 18.61986f}, 
	{-1.59046f, -12.83582f, 18.11625f}, {-1.67541f, -13.37771f, 18.26922f}, {-1.75263f, -13.72003f, 18.32630f}, {-1.87907f, -13.07508f, 17.76321f}, 
	{-1.94962f, -13.61468f, 17.98829f}, {-1.91920f, -13.83809f, 18.05013f}, {-2.23776f, -12.91936f, 17.83990f}, {-2.24959f, -13.46019f, 18.02616f}, 
	{-2.26863f, -13.79035f, 18.12881f}, {-2.65092f, -12.86021f, 17.71733f}, {-2.71922f, -13.40118f, 17.89931f}, {-2.74080f, -13.78926f, 18.01024f}, 
	{-2.96633f, -13.05724f, 17.53284f}, {-3.08280f, -13.59904f, 17.68866f}, {-3.14899f, -13.84731f, 17.65216f}, {-3.19659f, -13.00947f, 17.28643f}, 
	{-3.27785f, -13.55052f, 17.46586f}, {-3.24293f, -14.03157f, 17.47106f}, {-3.55506f, -12.95477f, 17.25850f}, {-3.60214f, -13.49584f, 17.43728f}, 
	{-3.54310f, -13.94153f, 17.52708f}, {-4.06388f, -12.62808f, 16.92610f}, {-4.20711f, -13.16954f, 17.09285f}, {-4.37126f, -14.07027f, 17.22761f}, 
	{-4.37793f, -13.08140f, 16.52730f}, {-4.51100f, -13.62663f, 16.57481f}, {-4.55451f, -14.00316f, 16.51990f}, {-4.25075f, -13.43244f, 16.04980f}, 
	{-4.47419f, -13.97447f, 16.19845f}, {-4.52974f, -14.21761f, 16.26624f}, {-4.74516f, -13.15169f, 15.85734f}, {-4.85745f, -13.69419f, 15.99113f}, 
	{-4.91676f, -14.16605f, 16.09461f}, {-4.98087f, -13.00822f, 15.42024f}, {-5.15121f, -13.55205f, 15.51202f}, {-5.28018f, -14.17300f, 15.59886f}, 
	{-5.13483f, -13.29320f, 15.00929f}, {-5.28371f, -13.83966f, 15.01822f}, {-5.31739f, -14.32092f, 15.02809f}, {-4.88087f, -13.55129f, 14.60677f}, 
	{-5.15486f, -14.09422f, 14.72694f}, {-5.19027f, -14.26432f, 14.72937f}, {-5.39400f, -13.27630f, 14.40564f}, {-5.50828f, -13.81875f, 14.54099f}, 
	{-5.51660f, -14.33898f, 14.61036f}, {-5.62313f, -13.16500f, 13.98509f}, {-5.79951f, -13.70934f, 14.06078f}, {-5.86655f, -14.26633f, 14.17021f}, 
	{-5.69693f, -13.44030f, 13.51830f}, {-5.83831f, -13.98674f, 13.52774f}, {-5.89422f, -14.34407f, 13.57284f}, {-5.52106f, -13.78567f, 13.11075f}, 
	{-5.76892f, -14.32854f, 13.23271f}, {-5.80823f, -14.55184f, 13.22334f}, {-6.02744f, -13.44639f, 12.82079f}, {-6.15099f, -13.98970f, 12.92890f}, 
	{-6.15395f, -14.53682f, 13.06111f}, {-6.17474f, -13.24961f, 12.39337f}, {-6.33773f, -13.79415f, 12.46285f}, {-6.41869f, -14.40445f, 12.59924f}, 
	{-6.32914f, -13.24760f, 11.75412f}, {-6.50223f, -13.79323f, 11.78913f}, {-6.56578f, -14.34494f, 11.87968f}, {-6.31808f, -13.36152f, 11.26387f}, 
	{-6.48160f, -13.90873f, 11.24915f}, {-6.54451f, -14.42316f, 11.25985f}, {-6.11980f, -13.56178f, 10.84099f}, {-6.34559f, -14.10671f, 10.89798f}, 
	{-6.33673f, -14.54401f, 10.84570f}, {-6.52082f, -13.29973f, 10.45865f}, {-6.65424f, -13.84352f, 10.55172f}, {-6.61667f, -14.37945f, 10.65804f}, 
	{-6.61398f, -13.10668f, 10.09263f}, {-6.78117f, -13.65193f, 10.13974f}, {-6.89643f, -14.30092f, 10.22042f}, {-6.65646f, -13.06580f, 9.30772f}, 
	{-6.83182f, -13.61233f, 9.31444f}, {-6.92454f, -14.16453f, 9.36609f}, {-6.55040f, -13.35570f, 8.69769f}, {-6.69562f, -13.90503f, 8.61620f}, 
	{-6.72932f, -14.20230f, 8.52197f}, {-5.86572f, -13.60219f, 8.26189f}, {-5.91322f, -14.15375f, 8.11028f}, {-5.89580f, -14.57948f, 8.12821f}, 
	{-5.23814f, -13.73161f, 8.28905f}, {-5.17371f, -14.28323f, 8.13535f}, {-5.28924f, -14.65601f, 8.10244f}, {-4.68093f, -13.65147f, 8.74432f}, 
	{-4.53827f, -14.20104f, 8.65502f}, {-4.73199f, -14.73463f, 8.49628f}, {-4.43958f, -13.74693f, 9.31124f}, {-4.26688f, -14.29394f, 9.30294f}, 
	{-4.38177f, -14.74529f, 9.29252f}, {-4.47797f, -13.81684f, 9.68770f}, {-4.31477f, -14.36122f, 9.76214f}, {-4.38247f, -14.72775f, 9.81364f}, 
	{-4.88561f, -13.92648f, 10.31233f}, {-4.63542f, -14.47472f, 10.26511f}, {-4.66665f, -14.84467f, 10.32510f}, {-4.20388f, -13.77999f, 10.67141f}, 
	{-4.07362f, -14.33011f, 10.56474f}, {-4.25087f, -14.86383f, 10.46033f}, {-3.92501f, -13.74936f, 11.24269f}, {-3.75859f, -14.29717f, 11.20891f}, 
	{-3.87857f, -14.86947f, 11.19261f}, {-3.85003f, -13.78248f, 11.76149f}, {-3.67774f, -14.32868f, 11.77837f}, {-3.73254f, -14.82859f, 11.77957f}, 
	{-3.92258f, -13.86125f, 12.30502f}, {-3.76070f, -14.40557f, 12.38144f}, {-3.93215f, -14.86487f, 12.45432f}, {-4.27046f, -13.97116f, 12.84870f}, 
	{-4.04555f, -14.51840f, 12.83299f}, {-4.15660f, -14.81132f, 12.82293f}, {-3.85350f, -13.91266f, 13.21822f}, {-3.71376f, -14.46223f, 13.12906f}, 
	{-3.78272f, -14.79514f, 13.06612f}, {-3.73935f, -13.93535f, 13.44439f}, {-3.56213f, -14.48246f, 13.43287f}, {-3.64671f, -14.88714f, 13.38296f}, 
	{-3.77516f, -13.94239f, 13.69921f}, {-3.60937f, -14.48696f, 13.76761f}, {-3.71606f, -14.93742f, 13.81154f}, {-4.04393f, -13.98304f, 14.23265f}, 
	{-3.79526f, -14.53256f, 14.14502f}, {-3.87868f, -14.81158f, 14.15763f}, {-3.45766f, -13.86623f, 14.40384f}, {-3.35509f, -14.41714f, 14.27263f}, 
	{-3.45264f, -14.75379f, 14.28149f}, {-3.17287f, -13.86446f, 14.76954f}, {-3.00694f, -14.41278f, 14.72005f}, {-3.08578f, -14.81553f, 14.74750f}, 
	{-3.15006f, -13.82458f, 15.00589f}, {-2.97315f, -14.37007f, 15.04518f}, {-3.05875f, -14.76455f, 15.22075f}, {-3.38650f, -13.92032f, 15.65149f}, 
	{-3.16074f, -14.47011f, 15.55544f}, {-3.22834f, -14.63527f, 15.62157f}, {-2.84025f, -13.86021f, 15.81728f}, {-2.74658f, -14.41124f, 15.68204f}, 
	{-2.96708f, -14.62317f, 15.71596f}, {-2.70406f, -13.90070f, 15.95466f}, {-2.54541f, -14.44947f, 15.89092f}, {-2.72039f, -14.69058f, 16.01638f}, 
	{-2.66384f, -13.92064f, 16.18834f}, {-2.49094f, -14.46718f, 16.19481f}, {-2.65470f, -14.66596f, 16.29020f}, {-2.68741f, -13.89609f, 16.65018f}, 
	{-2.50696f, -14.44761f, 16.49971f}, {-2.64310f, -14.54827f, 16.68781f}, {-2.28203f, -13.88394f, 16.66255f}, {-2.27876f, -14.43572f, 16.50359f}, 
	{-2.29217f, -14.59278f, 16.69123f}, {-2.09464f, -13.87223f, 16.66094f}, {-2.05056f, -14.42385f, 16.50746f}, {-2.07999f, -14.58625f, 16.79132f}, 
	{-1.94372f, -13.86156f, 16.74025f}, {-1.82198f, -14.41187f, 16.62791f}, {-1.88020f, -14.55580f, 16.88878f}, {-1.65370f, -13.80967f, 17.20932f}, 
	{-1.61128f, -14.36396f, 16.97134f}, {-1.65946f, -14.50166f, 17.21528f}, {-1.10775f, -13.77726f, 16.75810f}, {-1.17685f, -14.32871f, 16.60985f}, 
	{-1.29981f, -14.51921f, 16.89344f}, {-0.75863f, -13.76918f, 16.72328f}, {-0.72709f, -14.32095f, 16.56489f}, {-0.76706f, -14.55755f, 16.95950f}, 
	{-0.43990f, -13.76117f, 16.87858f}, {-0.32841f, -14.31169f, 16.75926f}, {-0.39377f, -14.52030f, 17.00532f}, {-5.98326f, -15.01354f, 8.33794f}, 
	{-6.72903f, -14.97209f, 8.65864f}, {-6.85801f, -14.94044f, 9.46558f}, {-6.42028f, -14.97918f, 10.80636f}, {-6.55168f, -15.09982f, 11.21962f}, 
	{-6.64887f, -15.16653f, 11.81881f}, {-0.36108f, -14.70566f, 17.64573f}, {-0.36653f, -15.01926f, 18.10471f}, {-0.88756f, -14.74410f, 17.62822f}, 
	{-0.99805f, -14.97703f, 17.97240f}, {-1.36232f, -14.76484f, 17.57707f}, {-1.43941f, -14.99318f, 17.93017f}, {-1.70604f, -14.73613f, 17.47490f}, 
	{-1.80122f, -15.05644f, 17.90900f}, {-2.04348f, -14.70946f, 17.36447f}, {-2.07137f, -15.03670f, 17.78871f}, {-2.25153f, -14.70590f, 17.18810f}, 
	{-2.44079f, -14.98754f, 17.58921f}, {-2.54544f, -14.71391f, 16.97501f}, {-2.86172f, -14.98607f, 17.38363f}, {-2.78827f, -14.67960f, 16.81033f}, 
	{-3.03953f, -14.94408f, 17.18726f}, {-3.10098f, -14.80367f, 16.64006f}, {-3.27772f, -15.00124f, 16.92597f}, {-3.22640f, -14.80210f, 16.30771f}, 
	{-3.53359f, -15.10479f, 16.57952f}, {-3.39037f, -14.81837f, 16.04323f}, {-3.74884f, -14.97364f, 16.29472f}, {-3.46925f, -14.85273f, 15.77990f}, 
	{-3.27299f, -15.12663f, 15.40744f}, {-3.28332f, -15.23162f, 14.85730f}, {-3.78033f, -15.54918f, 15.04960f}, {-3.73185f, -15.18259f, 14.39035f}, 
	{-4.12628f, -15.30794f, 14.56492f}, {-3.99091f, -15.05720f, 14.21663f}, {-4.21303f, -15.17713f, 14.31426f}, {-3.92490f, -15.28080f, 13.90840f}, 
	{-4.33895f, -15.44115f, 14.03182f}, {-3.87957f, -15.31211f, 13.52468f}, {-4.40119f, -15.59353f, 13.56913f}, {-4.08408f, -15.26757f, 13.07470f}, 
	{-4.46473f, -15.49231f, 13.20471f}, {-4.32439f, -15.12619f, 12.85655f}, {-4.54655f, -15.27228f, 12.86077f}, {-4.19391f, -15.30486f, 12.42281f}, 
	{-4.60756f, -15.53442f, 12.51499f}, {-4.21190f, -15.42523f, 11.88213f}, {-4.70005f, -15.63234f, 11.97889f}, {-4.29053f, -15.33344f, 11.29269f}, 
	{-4.76256f, -15.53553f, 11.33168f}, {-4.52430f, -15.40426f, 10.52705f}, {-4.97070f, -15.64912f, 10.66659f}, {-4.83484f, -15.27436f, 10.31640f}, 
	{-5.11609f, -15.36786f, 10.34821f}, {-4.71563f, -15.43497f, 9.88906f}, {-5.13923f, -15.57019f, 10.04822f}, {-4.75058f, -15.44049f, 9.35340f}, 
	{-5.22912f, -15.65321f, 9.67936f}, {-5.04106f, -15.29550f, 8.63163f}, {-5.51845f, -15.32436f, 8.91587f}, {-5.51245f, -15.10598f, 8.24637f}, 
	{-5.69828f, -15.23258f, 8.64071f}, {-5.99765f, -15.06624f, 8.76799f}, {-4.56055f, -15.35598f, 15.94936f}, {-4.43698f, -15.21407f, 16.18909f}, 
	{-4.27581f, -15.43056f, 16.57434f}, {-4.22979f, -15.27774f, 15.27074f}, {-4.41292f, -15.11574f, 14.88615f}, {-4.62016f, -15.14550f, 14.45608f}, 
	{-4.80580f, -15.07469f, 14.10695f}, {-4.87613f, -15.25943f, 13.81935f}, {-5.02868f, -15.06900f, 13.50042f}, {-5.16681f, -15.13955f, 13.14326f}, 
	{-5.32584f, -15.17740f, 12.65623f}, {-5.33370f, -15.25323f, 12.40877f}, {-5.45620f, -15.26572f, 11.46243f}, {-5.62710f, -15.15318f, 11.12074f}, 
	{-5.73577f, -15.15403f, 10.65074f}, {-5.79414f, -15.24406f, 10.36297f}, {-5.86142f, -15.18372f, 9.99958f}, {-5.97343f, -15.06996f, 9.24406f}, 
	{-6.50717f, -15.23703f, 8.83760f}, {-6.59442f, -15.32163f, 9.35196f}, {-6.50861f, -15.51546f, 10.23624f}, {-6.34249f, -15.39063f, 10.54564f}, 
	{-6.20075f, -15.22513f, 10.78008f}, {-6.17264f, -15.54238f, 11.20392f}, {-6.12267f, -15.58364f, 11.74689f}, {-5.92165f, -15.60150f, 12.56426f}, 
	{-5.72267f, -15.47505f, 12.97543f}, {-5.54738f, -15.25137f, 13.23369f}, {-5.55368f, -15.37304f, 13.46441f}, {-5.32308f, -15.58266f, 14.12108f}, 
	{-5.11360f, -15.39393f, 14.43936f}, {-4.92855f, -15.20452f, 14.66082f}, {-4.95344f, -15.33371f, 14.90721f}, {-4.73809f, -15.56568f, 15.53419f}, 
	{-2.15337f, -15.31848f, 18.21219f}, {-1.86080f, -15.11266f, 18.19902f}, {-1.57232f, -15.32993f, 18.41984f}, {-0.36686f, -15.35750f, 18.52003f}, 
	{-3.47636f, -15.31730f, 17.41687f}, {-3.25030f, -15.18154f, 17.54325f}, {-3.06881f, -15.27512f, 17.71044f}, {-0.99940f, -15.36296f, 18.47755f}, 
	{-2.58558f, -15.33124f, 18.02773f}, {-4.01440f, -15.54715f, 17.12273f}, {-0.41898f, -14.75582f, 18.78615f}, {-0.97893f, -14.84860f, 18.73516f}, 
	{-1.79026f, -14.80464f, 18.51945f}, {-1.94708f, -14.75423f, 18.34623f}, {-2.27894f, -14.76085f, 18.28685f}, {-2.73719f, -14.79284f, 18.18449f}, 
	{-3.24489f, -14.74410f, 17.80289f}, {-3.34793f, -14.72953f, 17.64614f}, {-3.61425f, -14.78049f, 17.63069f}, {-4.28446f, -14.84284f, 17.33721f}, 
	{-4.51505f, -14.88797f, 16.71508f}, {-4.61087f, -14.86856f, 16.25772f}, {-4.90745f, -14.88680f, 16.00758f}, {-5.20100f, -14.96437f, 15.65736f}, 
	{-5.23683f, -14.96849f, 15.09716f}, {-5.25666f, -14.98781f, 14.74200f}, {-5.50972f, -14.98387f, 14.51853f}, {-5.67603f, -14.96324f, 14.36784f}, 
	{-5.86569f, -15.02581f, 13.62217f}, {-5.86883f, -15.03100f, 13.29315f}, {-6.15392f, -15.03788f, 13.08459f}, {-6.36249f, -15.10330f, 12.72152f}, 
	{-6.64571f, -15.03455f, 10.59976f}, {-6.81723f, -15.06859f, 10.37344f}
};



GLint inf_gum_indices[1132][3] = {
	{10, 9, 8}, {9, 10, 11}, {14, 13, 12}, {15, 12, 13}, 
	{17, 8, 16}, {8, 17, 10}, {19, 16, 18}, {16, 19, 17}, 
	{22, 21, 20}, {21, 22, 23}, {23, 18, 21}, {18, 23, 19}, 
	{25, 20, 24}, {20, 25, 22}, {27, 24, 26}, {24, 27, 25}, 
	{29, 26, 28}, {26, 29, 27}, {31, 28, 30}, {28, 31, 29}, 
	{33, 30, 32}, {30, 33, 31}, {35, 14, 34}, {12, 34, 14}, 
	{37, 35, 36}, {34, 36, 35}, {40, 39, 38}, {41, 38, 39}, 
	{39, 37, 41}, {36, 41, 37}, {43, 40, 42}, {38, 42, 40}, 
	{45, 43, 44}, {42, 44, 43}, {47, 45, 46}, {44, 46, 45}, 
	{49, 47, 48}, {46, 48, 47}, {32, 49, 33}, {48, 33, 49}, 
	{52, 51, 50}, {55, 54, 53}, {58, 57, 56}, {61, 60, 59}, 
	{64, 63, 62}, {63, 64, 65}, {67, 62, 66}, {62, 67, 64}, 
	{62, 69, 68}, {69, 62, 63}, {66, 68, 70}, {68, 66, 62}, 
	{68, 72, 71}, {72, 68, 69}, {70, 71, 73}, {71, 70, 68}, 
	{71, 75, 74}, {75, 71, 72}, {73, 74, 76}, {74, 73, 71}, 
	{74, 78, 77}, {78, 74, 75}, {76, 77, 79}, {77, 76, 74}, 
	{77, 81, 80}, {81, 77, 78}, {79, 80, 82}, {80, 79, 77}, 
	{80, 84, 83}, {84, 80, 81}, {82, 83, 85}, {83, 82, 80}, 
	{83, 87, 86}, {87, 83, 84}, {85, 86, 88}, {86, 85, 83}, 
	{86, 90, 89}, {90, 86, 87}, {88, 89, 91}, {89, 88, 86}, 
	{89, 93, 92}, {93, 89, 90}, {91, 92, 94}, {92, 91, 89}, 
	{92, 96, 95}, {96, 92, 93}, {94, 95, 97}, {95, 94, 92}, 
	{95, 99, 98}, {99, 95, 96}, {97, 98, 100}, {98, 97, 95}, 
	{98, 102, 101}, {102, 98, 99}, {100, 101, 103}, {101, 100, 98}, 
	{101, 105, 104}, {105, 101, 102}, {103, 104, 106}, {104, 103, 101}, 
	{104, 108, 107}, {108, 104, 105}, {106, 107, 109}, {107, 106, 104}, 
	{107, 111, 110}, {111, 107, 108}, {109, 110, 112}, {110, 109, 107}, 
	{110, 114, 113}, {114, 110, 111}, {112, 113, 115}, {113, 112, 110}, 
	{113, 117, 116}, {117, 113, 114}, {115, 116, 118}, {116, 115, 113}, 
	{116, 120, 119}, {120, 116, 117}, {118, 119, 121}, {119, 118, 116}, 
	{119, 123, 122}, {123, 119, 120}, {121, 122, 124}, {122, 121, 119}, 
	{122, 126, 125}, {126, 122, 123}, {124, 125, 127}, {125, 124, 122}, 
	{125, 129, 128}, {129, 125, 126}, {127, 128, 130}, {128, 127, 125}, 
	{128, 132, 131}, {132, 128, 129}, {130, 131, 133}, {131, 130, 128}, 
	{131, 135, 134}, {135, 131, 132}, {133, 134, 136}, {134, 133, 131}, 
	{134, 138, 137}, {138, 134, 135}, {136, 137, 139}, {137, 136, 134}, 
	{137, 141, 140}, {141, 137, 138}, {139, 140, 142}, {140, 139, 137}, 
	{140, 144, 143}, {144, 140, 141}, {142, 143, 145}, {143, 142, 140}, 
	{143, 147, 146}, {147, 143, 144}, {145, 146, 148}, {146, 145, 143}, 
	{146, 150, 149}, {150, 146, 147}, {148, 149, 151}, {149, 148, 146}, 
	{149, 153, 152}, {153, 149, 150}, {151, 152, 154}, {152, 151, 149}, 
	{152, 156, 155}, {156, 152, 153}, {154, 155, 157}, {155, 154, 152}, 
	{155, 159, 158}, {159, 155, 156}, {157, 158, 160}, {158, 157, 155}, 
	{158, 162, 161}, {162, 158, 159}, {160, 161, 163}, {161, 160, 158}, 
	{161, 165, 164}, {165, 161, 162}, {163, 164, 166}, {164, 163, 161}, 
	{164, 168, 167}, {168, 164, 165}, {166, 167, 169}, {167, 166, 164}, 
	{167, 171, 170}, {171, 167, 168}, {169, 170, 172}, {170, 169, 167}, 
	{170, 174, 173}, {174, 170, 171}, {172, 173, 175}, {173, 172, 170}, 
	{173, 177, 176}, {177, 173, 174}, {175, 176, 178}, {176, 175, 173}, 
	{176, 180, 179}, {180, 176, 177}, {178, 179, 181}, {179, 178, 176}, 
	{179, 183, 182}, {183, 179, 180}, {181, 182, 184}, {182, 181, 179}, 
	{182, 186, 185}, {186, 182, 183}, {184, 185, 187}, {185, 184, 182}, 
	{185, 189, 188}, {189, 185, 186}, {187, 188, 190}, {188, 187, 185}, 
	{188, 192, 191}, {192, 188, 189}, {190, 191, 193}, {191, 190, 188}, 
	{191, 195, 194}, {195, 191, 192}, {193, 194, 196}, {194, 193, 191}, 
	{194, 198, 197}, {198, 194, 195}, {196, 197, 199}, {197, 196, 194}, 
	{197, 201, 200}, {201, 197, 198}, {199, 200, 202}, {200, 199, 197}, 
	{200, 204, 203}, {204, 200, 201}, {202, 203, 205}, {203, 202, 200}, 
	{203, 207, 206}, {207, 203, 204}, {205, 206, 208}, {206, 205, 203}, 
	{206, 210, 209}, {210, 206, 207}, {208, 209, 61}, {209, 208, 206}, 
	{209, 212, 211}, {212, 209, 210}, {61, 211, 60}, {211, 61, 209}, 
	{211, 214, 213}, {214, 211, 212}, {60, 213, 215}, {213, 60, 211}, 
	{213, 217, 216}, {217, 213, 214}, {215, 216, 218}, {216, 215, 213}, 
	{216, 220, 219}, {220, 216, 217}, {218, 219, 58}, {219, 218, 216}, 
	{219, 222, 221}, {222, 219, 220}, {58, 221, 57}, {221, 58, 219}, 
	{221, 224, 223}, {224, 221, 222}, {57, 223, 225}, {223, 57, 221}, 
	{223, 227, 226}, {227, 223, 224}, {225, 226, 228}, {226, 225, 223}, 
	{226, 230, 229}, {230, 226, 227}, {228, 229, 55}, {229, 228, 226}, 
	{229, 232, 231}, {232, 229, 230}, {55, 231, 54}, {231, 55, 229}, 
	{231, 234, 233}, {234, 231, 232}, {54, 233, 235}, {233, 54, 231}, 
	{233, 237, 236}, {237, 233, 234}, {235, 236, 238}, {236, 235, 233}, 
	{236, 240, 239}, {240, 236, 237}, {238, 239, 52}, {239, 238, 236}, 
	{239, 242, 241}, {242, 239, 240}, {52, 241, 51}, {241, 52, 239}, 
	{241, 244, 243}, {244, 241, 242}, {51, 243, 245}, {243, 51, 241}, 
	{145, 247, 246}, {247, 145, 148}, {148, 248, 247}, {248, 148, 151}, 
	{246, 250, 249}, {250, 246, 247}, {247, 251, 250}, {251, 247, 248}, 
	{142, 246, 252}, {246, 142, 145}, {252, 249, 253}, {249, 252, 246}, 
	{139, 252, 254}, {252, 139, 142}, {254, 253, 255}, {253, 254, 252}, 
	{136, 254, 256}, {254, 136, 139}, {256, 255, 257}, {255, 256, 254}, 
	{133, 256, 258}, {256, 133, 136}, {258, 257, 259}, {257, 258, 256}, 
	{130, 258, 260}, {258, 130, 133}, {260, 259, 261}, {259, 260, 258}, 
	{127, 260, 262}, {260, 127, 130}, {262, 261, 263}, {261, 262, 260}, 
	{124, 262, 264}, {262, 124, 127}, {264, 263, 265}, {263, 264, 262}, 
	{121, 264, 266}, {264, 121, 124}, {266, 265, 267}, {265, 266, 264}, 
	{118, 266, 268}, {266, 118, 121}, {268, 267, 269}, {267, 268, 266}, 
	{115, 268, 270}, {268, 115, 118}, {270, 269, 271}, {269, 270, 268}, 
	{112, 270, 272}, {270, 112, 115}, {272, 271, 273}, {271, 272, 270}, 
	{109, 272, 274}, {272, 109, 112}, {274, 273, 275}, {273, 274, 272}, 
	{106, 274, 276}, {274, 106, 109}, {276, 275, 277}, {275, 276, 274}, 
	{103, 276, 278}, {276, 103, 106}, {278, 277, 279}, {277, 278, 276}, 
	{100, 278, 280}, {278, 100, 103}, {280, 279, 281}, {279, 280, 278}, 
	{97, 280, 282}, {280, 97, 100}, {282, 281, 283}, {281, 282, 280}, 
	{94, 282, 284}, {282, 94, 97}, {284, 283, 11}, {283, 284, 282}, 
	{91, 284, 285}, {284, 91, 94}, {285, 11, 10}, {11, 285, 284}, 
	{88, 285, 286}, {285, 88, 91}, {286, 10, 17}, {10, 286, 285}, 
	{85, 286, 287}, {286, 85, 88}, {287, 17, 19}, {17, 287, 286}, 
	{79, 289, 288}, {289, 79, 82}, {82, 287, 289}, {287, 82, 85}, 
	{288, 23, 22}, {23, 288, 289}, {289, 19, 23}, {19, 289, 287}, 
	{76, 288, 290}, {288, 76, 79}, {290, 22, 25}, {22, 290, 288}, 
	{73, 290, 291}, {290, 73, 76}, {291, 25, 27}, {25, 291, 290}, 
	{70, 291, 292}, {291, 70, 73}, {292, 27, 29}, {27, 292, 291}, 
	{66, 292, 293}, {292, 66, 70}, {293, 29, 31}, {29, 293, 292}, 
	{67, 293, 294}, {293, 67, 66}, {294, 31, 33}, {31, 294, 293}, 
	{163, 296, 295}, {296, 163, 166}, {166, 297, 296}, {297, 166, 169}, 
	{295, 299, 298}, {299, 295, 296}, {296, 300, 299}, {300, 296, 297}, 
	{298, 302, 301}, {302, 298, 299}, {299, 303, 302}, {303, 299, 300}, 
	{160, 295, 304}, {295, 160, 163}, {304, 298, 305}, {298, 304, 295}, 
	{305, 301, 306}, {301, 305, 298}, {157, 304, 307}, {304, 157, 160}, 
	{307, 305, 308}, {305, 307, 304}, {154, 307, 309}, {307, 154, 157}, 
	{309, 308, 310}, {308, 309, 307}, {151, 309, 248}, {309, 151, 154}, 
	{248, 310, 251}, {310, 248, 309}, {238, 50, 311}, {50, 238, 52}, 
	{50, 312, 311}, {312, 50, 313}, {50, 314, 313}, {314, 50, 315}, 
	{50, 245, 315}, {245, 50, 51}, {228, 53, 316}, {53, 228, 55}, 
	{53, 317, 316}, {317, 53, 318}, {53, 319, 318}, {319, 53, 320}, 
	{53, 235, 320}, {235, 53, 54}, {218, 56, 321}, {56, 218, 58}, 
	{56, 322, 321}, {322, 56, 323}, {56, 324, 323}, {324, 56, 325}, 
	{56, 225, 325}, {225, 56, 57}, {208, 59, 326}, {59, 208, 61}, 
	{59, 327, 326}, {327, 59, 328}, {59, 329, 328}, {329, 59, 330}, 
	{59, 215, 330}, {215, 59, 60}, {332, 300, 331}, {300, 332, 303}, 
	{331, 297, 333}, {297, 331, 300}, {297, 172, 333}, {172, 297, 169}, 
	{335, 331, 334}, {331, 335, 332}, {334, 333, 336}, {333, 334, 331}, 
	{336, 172, 175}, {172, 336, 333}, {338, 334, 337}, {334, 338, 335}, 
	{337, 336, 339}, {336, 337, 334}, {336, 178, 339}, {178, 336, 175}, 
	{341, 337, 340}, {337, 341, 338}, {340, 339, 342}, {339, 340, 337}, 
	{339, 181, 342}, {181, 339, 178}, {344, 340, 343}, {340, 344, 341}, 
	{343, 342, 345}, {342, 343, 340}, {342, 184, 345}, {184, 342, 181}, 
	{347, 343, 346}, {343, 347, 344}, {346, 345, 348}, {345, 346, 343}, 
	{345, 187, 348}, {187, 345, 184}, {350, 346, 349}, {346, 350, 347}, 
	{349, 348, 351}, {348, 349, 346}, {348, 190, 351}, {190, 348, 187}, 
	{353, 349, 352}, {349, 353, 350}, {352, 351, 354}, {351, 352, 349}, 
	{351, 193, 354}, {193, 351, 190}, {356, 352, 355}, {352, 356, 353}, 
	{355, 354, 357}, {354, 355, 352}, {354, 196, 357}, {196, 354, 193}, 
	{359, 355, 358}, {355, 359, 356}, {358, 357, 360}, {357, 358, 355}, 
	{357, 199, 360}, {199, 357, 196}, {362, 358, 361}, {358, 362, 359}, 
	{361, 360, 363}, {360, 361, 358}, {360, 202, 363}, {202, 360, 199}, 
	{365, 361, 364}, {361, 365, 362}, {364, 363, 366}, {363, 364, 361}, 
	{363, 205, 366}, {205, 363, 202}, {367, 364, 327}, {364, 367, 365}, 
	{327, 366, 326}, {366, 327, 364}, {366, 208, 326}, {208, 366, 205}, 
	{368, 327, 328}, {327, 368, 367}, {369, 328, 329}, {328, 369, 368}, 
	{370, 329, 322}, {329, 370, 369}, {322, 330, 321}, {330, 322, 329}, 
	{330, 218, 321}, {218, 330, 215}, {371, 322, 323}, {322, 371, 370}, 
	{372, 323, 324}, {323, 372, 371}, {373, 324, 317}, {324, 373, 372}, 
	{317, 325, 316}, {325, 317, 324}, {325, 228, 316}, {228, 325, 225}, 
	{374, 317, 318}, {317, 374, 373}, {375, 318, 319}, {318, 375, 374}, 
	{376, 319, 312}, {319, 376, 375}, {312, 320, 311}, {320, 312, 319}, 
	{320, 238, 311}, {238, 320, 235}, {377, 312, 313}, {312, 377, 376}, 
	{378, 313, 314}, {313, 378, 377}, {381, 380, 379}, {384, 383, 382}, 
	{387, 386, 385}, {390, 389, 388}, {392, 391, 64}, {65, 64, 391}, 
	{393, 392, 67}, {64, 67, 392}, {395, 394, 392}, {391, 392, 394}, 
	{396, 395, 393}, {392, 393, 395}, {398, 397, 395}, {394, 395, 397}, 
	{399, 398, 396}, {395, 396, 398}, {401, 400, 398}, {397, 398, 400}, 
	{402, 401, 399}, {398, 399, 401}, {404, 403, 401}, {400, 401, 403}, 
	{405, 404, 402}, {401, 402, 404}, {407, 406, 404}, {403, 404, 406}, 
	{408, 407, 405}, {404, 405, 407}, {410, 409, 407}, {406, 407, 409}, 
	{411, 410, 408}, {407, 408, 410}, {413, 412, 410}, {409, 410, 412}, 
	{414, 413, 411}, {410, 411, 413}, {416, 415, 413}, {412, 413, 415}, 
	{417, 416, 414}, {413, 414, 416}, {419, 418, 416}, {415, 416, 418}, 
	{420, 419, 417}, {416, 417, 419}, {422, 421, 419}, {418, 419, 421}, 
	{423, 422, 420}, {419, 420, 422}, {425, 424, 422}, {421, 422, 424}, 
	{426, 425, 423}, {422, 423, 425}, {428, 427, 425}, {424, 425, 427}, 
	{429, 428, 426}, {425, 426, 428}, {431, 430, 428}, {427, 428, 430}, 
	{432, 431, 429}, {428, 429, 431}, {434, 433, 431}, {430, 431, 433}, 
	{435, 434, 432}, {431, 432, 434}, {437, 436, 434}, {433, 434, 436}, 
	{438, 437, 435}, {434, 435, 437}, {440, 439, 437}, {436, 437, 439}, 
	{441, 440, 438}, {437, 438, 440}, {443, 442, 440}, {439, 440, 442}, 
	{444, 443, 441}, {440, 441, 443}, {446, 445, 443}, {442, 443, 445}, 
	{447, 446, 444}, {443, 444, 446}, {449, 448, 446}, {445, 446, 448}, 
	{450, 449, 447}, {446, 447, 449}, {452, 451, 449}, {448, 449, 451}, 
	{453, 452, 450}, {449, 450, 452}, {455, 454, 452}, {451, 452, 454}, 
	{456, 455, 453}, {452, 453, 455}, {458, 457, 455}, {454, 455, 457}, 
	{459, 458, 456}, {455, 456, 458}, {461, 460, 458}, {457, 458, 460}, 
	{462, 461, 459}, {458, 459, 461}, {464, 463, 461}, {460, 461, 463}, 
	{465, 464, 462}, {461, 462, 464}, {467, 466, 464}, {463, 464, 466}, 
	{468, 467, 465}, {464, 465, 467}, {470, 469, 467}, {466, 467, 469}, 
	{471, 470, 468}, {467, 468, 470}, {473, 472, 470}, {469, 470, 472}, 
	{474, 473, 471}, {470, 471, 473}, {476, 475, 473}, {472, 473, 475}, 
	{477, 476, 474}, {473, 474, 476}, {479, 478, 476}, {475, 476, 478}, 
	{480, 479, 477}, {476, 477, 479}, {482, 481, 479}, {478, 479, 481}, 
	{483, 482, 480}, {479, 480, 482}, {485, 484, 482}, {481, 482, 484}, 
	{486, 485, 483}, {482, 483, 485}, {488, 487, 485}, {484, 485, 487}, 
	{489, 488, 486}, {485, 486, 488}, {491, 490, 488}, {487, 488, 490}, 
	{492, 491, 489}, {488, 489, 491}, {494, 493, 491}, {490, 491, 493}, 
	{495, 494, 492}, {491, 492, 494}, {497, 496, 494}, {493, 494, 496}, 
	{498, 497, 495}, {494, 495, 497}, {500, 499, 497}, {496, 497, 499}, 
	{501, 500, 498}, {497, 498, 500}, {503, 502, 500}, {499, 500, 502}, 
	{504, 503, 501}, {500, 501, 503}, {506, 505, 503}, {502, 503, 505}, 
	{507, 506, 504}, {503, 504, 506}, {509, 508, 506}, {505, 506, 508}, 
	{510, 509, 507}, {506, 507, 509}, {512, 511, 509}, {508, 509, 511}, 
	{513, 512, 510}, {509, 510, 512}, {515, 514, 512}, {511, 512, 514}, 
	{516, 515, 513}, {512, 513, 515}, {518, 517, 515}, {514, 515, 517}, 
	{519, 518, 516}, {515, 516, 518}, {521, 520, 518}, {517, 518, 520}, 
	{522, 521, 519}, {518, 519, 521}, {524, 523, 521}, {520, 521, 523}, 
	{525, 524, 522}, {521, 522, 524}, {527, 526, 524}, {523, 524, 526}, 
	{528, 527, 525}, {524, 525, 527}, {530, 529, 527}, {526, 527, 529}, 
	{531, 530, 528}, {527, 528, 530}, {533, 532, 530}, {529, 530, 532}, 
	{534, 533, 531}, {530, 531, 533}, {536, 535, 533}, {532, 533, 535}, 
	{388, 536, 534}, {533, 534, 536}, {538, 537, 536}, {535, 536, 537}, 
	{389, 538, 388}, {536, 388, 538}, {540, 539, 538}, {537, 538, 539}, 
	{541, 540, 389}, {538, 389, 540}, {543, 542, 540}, {539, 540, 542}, 
	{544, 543, 541}, {540, 541, 543}, {546, 545, 543}, {542, 543, 545}, 
	{385, 546, 544}, {543, 544, 546}, {548, 547, 546}, {545, 546, 547}, 
	{386, 548, 385}, {546, 385, 548}, {550, 549, 548}, {547, 548, 549}, 
	{551, 550, 386}, {548, 386, 550}, {553, 552, 550}, {549, 550, 552}, 
	{554, 553, 551}, {550, 551, 553}, {556, 555, 553}, {552, 553, 555}, 
	{382, 556, 554}, {553, 554, 556}, {558, 557, 556}, {555, 556, 557}, 
	{383, 558, 382}, {556, 382, 558}, {560, 559, 558}, {557, 558, 559}, 
	{561, 560, 383}, {558, 383, 560}, {563, 562, 560}, {559, 560, 562}, 
	{564, 563, 561}, {560, 561, 563}, {566, 565, 563}, {562, 563, 565}, 
	{379, 566, 564}, {563, 564, 566}, {568, 567, 566}, {565, 566, 567}, 
	{380, 568, 379}, {566, 379, 568}, {243, 244, 568}, {567, 568, 244}, 
	{245, 243, 380}, {568, 380, 243}, {570, 569, 471}, {474, 471, 569}, 
	{569, 571, 474}, {477, 474, 571}, {573, 572, 570}, {569, 570, 572}, 
	{572, 574, 569}, {571, 569, 574}, {575, 570, 468}, {471, 468, 570}, 
	{576, 573, 575}, {570, 575, 573}, {577, 575, 465}, {468, 465, 575}, 
	{578, 576, 577}, {575, 577, 576}, {579, 577, 462}, {465, 462, 577}, 
	{580, 578, 579}, {577, 579, 578}, {581, 579, 459}, {462, 459, 579}, 
	{582, 580, 581}, {579, 581, 580}, {583, 581, 456}, {459, 456, 581}, 
	{584, 582, 583}, {581, 583, 582}, {585, 583, 453}, {456, 453, 583}, 
	{586, 584, 585}, {583, 585, 584}, {587, 585, 450}, {453, 450, 585}, 
	{588, 586, 587}, {585, 587, 586}, {589, 587, 447}, {450, 447, 587}, 
	{590, 588, 589}, {587, 589, 588}, {591, 589, 444}, {447, 444, 589}, 
	{592, 590, 591}, {589, 591, 590}, {593, 591, 441}, {444, 441, 591}, 
	{594, 592, 593}, {591, 593, 592}, {595, 593, 438}, {441, 438, 593}, 
	{596, 594, 595}, {593, 595, 594}, {597, 595, 435}, {438, 435, 595}, 
	{598, 596, 597}, {595, 597, 596}, {599, 597, 432}, {435, 432, 597}, 
	{600, 598, 599}, {597, 599, 598}, {601, 599, 429}, {432, 429, 599}, 
	{602, 600, 601}, {599, 601, 600}, {603, 601, 426}, {429, 426, 601}, 
	{604, 602, 603}, {601, 603, 602}, {605, 603, 423}, {426, 423, 603}, 
	{606, 604, 605}, {603, 605, 604}, {607, 605, 420}, {423, 420, 605}, 
	{15, 606, 607}, {605, 607, 606}, {608, 607, 417}, {420, 417, 607}, 
	{12, 15, 608}, {607, 608, 15}, {609, 608, 414}, {417, 414, 608}, 
	{34, 12, 609}, {608, 609, 12}, {610, 609, 411}, {414, 411, 609}, 
	{36, 34, 610}, {609, 610, 34}, {612, 611, 405}, {408, 405, 611}, 
	{611, 610, 408}, {411, 408, 610}, {38, 41, 612}, {611, 612, 41}, 
	{41, 36, 611}, {610, 611, 36}, {613, 612, 402}, {405, 402, 612}, 
	{42, 38, 613}, {612, 613, 38}, {614, 613, 399}, {402, 399, 613}, 
	{44, 42, 614}, {613, 614, 42}, {615, 614, 396}, {399, 396, 614}, 
	{46, 44, 615}, {614, 615, 44}, {616, 615, 393}, {396, 393, 615}, 
	{48, 46, 616}, {615, 616, 46}, {294, 616, 67}, {393, 67, 616}, 
	{33, 48, 294}, {616, 294, 48}, {618, 617, 489}, {492, 489, 617}, 
	{617, 619, 492}, {495, 492, 619}, {621, 620, 618}, {617, 618, 620}, 
	{620, 622, 617}, {619, 617, 622}, {624, 623, 621}, {620, 621, 623}, 
	{623, 625, 620}, {622, 620, 625}, {626, 618, 486}, {489, 486, 618}, 
	{627, 621, 626}, {618, 626, 621}, {628, 624, 627}, {621, 627, 624}, 
	{629, 626, 483}, {486, 483, 626}, {630, 627, 629}, {626, 629, 627}, 
	{631, 629, 480}, {483, 480, 629}, {632, 630, 631}, {629, 631, 630}, 
	{571, 631, 477}, {480, 477, 631}, {574, 632, 571}, {631, 571, 632}, 
	{633, 381, 564}, {379, 564, 381}, {633, 634, 381}, {635, 381, 634}, 
	{635, 314, 381}, {315, 381, 314}, {315, 245, 381}, {380, 381, 245}, 
	{636, 384, 554}, {382, 554, 384}, {636, 637, 384}, {638, 384, 637}, 
	{638, 639, 384}, {640, 384, 639}, {640, 561, 384}, {383, 384, 561}, 
	{641, 387, 544}, {385, 544, 387}, {641, 642, 387}, {643, 387, 642}, 
	{643, 644, 387}, {645, 387, 644}, {645, 551, 387}, {386, 387, 551}, 
	{646, 390, 534}, {388, 534, 390}, {646, 647, 390}, {648, 390, 647}, 
	{648, 649, 390}, {650, 390, 649}, {650, 541, 390}, {389, 390, 541}, 
	{652, 622, 651}, {625, 651, 622}, {653, 619, 652}, {622, 652, 619}, 
	{653, 498, 619}, {495, 619, 498}, {655, 652, 654}, {651, 654, 652}, 
	{656, 653, 655}, {652, 655, 653}, {501, 498, 656}, {653, 656, 498}, 
	{658, 655, 657}, {654, 657, 655}, {659, 656, 658}, {655, 658, 656}, 
	{659, 504, 656}, {501, 656, 504}, {661, 658, 660}, {657, 660, 658}, 
	{662, 659, 661}, {658, 661, 659}, {662, 507, 659}, {504, 659, 507}, 
	{664, 661, 663}, {660, 663, 661}, {665, 662, 664}, {661, 664, 662}, 
	{665, 510, 662}, {507, 662, 510}, {667, 664, 666}, {663, 666, 664}, 
	{668, 665, 667}, {664, 667, 665}, {668, 513, 665}, {510, 665, 513}, 
	{670, 667, 669}, {666, 669, 667}, {671, 668, 670}, {667, 670, 668}, 
	{671, 516, 668}, {513, 668, 516}, {673, 670, 672}, {669, 672, 670}, 
	{674, 671, 673}, {670, 673, 671}, {674, 519, 671}, {516, 671, 519}, 
	{676, 673, 675}, {672, 675, 673}, {677, 674, 676}, {673, 676, 674}, 
	{677, 522, 674}, {519, 674, 522}, {679, 676, 678}, {675, 678, 676}, 
	{680, 677, 679}, {676, 679, 677}, {680, 525, 677}, {522, 677, 525}, 
	{682, 679, 681}, {678, 681, 679}, {683, 680, 682}, {679, 682, 680}, 
	{683, 528, 680}, {525, 680, 528}, {685, 682, 684}, {681, 684, 682}, 
	{686, 683, 685}, {682, 685, 683}, {686, 531, 683}, {528, 683, 531}, 
	{647, 685, 687}, {684, 687, 685}, {646, 686, 647}, {685, 647, 686}, 
	{646, 534, 686}, {531, 686, 534}, {648, 647, 688}, {687, 688, 647}, 
	{649, 648, 689}, {688, 689, 648}, {642, 649, 690}, {689, 690, 649}, 
	{641, 650, 642}, {649, 642, 650}, {641, 544, 650}, {541, 650, 544}, 
	{643, 642, 691}, {690, 691, 642}, {644, 643, 692}, {691, 692, 643}, 
	{637, 644, 693}, {692, 693, 644}, {636, 645, 637}, {644, 637, 645}, 
	{636, 554, 645}, {551, 645, 554}, {638, 637, 694}, {693, 694, 637}, 
	{639, 638, 695}, {694, 695, 638}, {634, 639, 696}, {695, 696, 639}, 
	{633, 640, 634}, {639, 634, 640}, {633, 564, 640}, {561, 640, 564}, 
	{635, 634, 697}, {696, 697, 634}, {314, 635, 378}, {697, 378, 635}
};


GLint tongue_indices[356][3] = {
	{700, 699, 698}, {698, 701, 700}, {701, 698, 702}, {702, 703, 701}, 
	{706, 705, 704}, {704, 707, 706}, {708, 706, 707}, {707, 709, 708}, 
	{712, 711, 710}, {712, 710, 713}, {713, 710, 714}, {713, 714, 715}, 
	{715, 714, 716}, {715, 716, 717}, {710, 711, 718}, {718, 719, 710}, 
	{714, 710, 719}, {719, 720, 714}, {716, 714, 720}, {720, 721, 716}, 
	{719, 718, 722}, {722, 699, 719}, {720, 719, 699}, {699, 700, 720}, 
	{721, 720, 700}, {700, 723, 721}, {723, 700, 701}, {701, 724, 723}, 
	{724, 701, 703}, {703, 725, 724}, {725, 703, 726}, {726, 727, 725}, 
	{723, 724, 728}, {723, 728, 729}, {729, 728, 730}, {729, 730, 731}, 
	{731, 730, 732}, {731, 732, 733}, {724, 725, 734}, {724, 734, 728}, 
	{728, 734, 735}, {728, 735, 730}, {730, 735, 736}, {730, 736, 732}, 
	{725, 727, 737}, {725, 737, 734}, {734, 737, 738}, {734, 738, 735}, 
	{735, 738, 739}, {735, 739, 736}, {740, 717, 716}, {716, 741, 740}, 
	{742, 740, 741}, {741, 743, 742}, {744, 742, 743}, {743, 745, 744}, 
	{741, 716, 721}, {721, 746, 741}, {743, 741, 746}, {746, 747, 743}, 
	{743, 747, 748}, {743, 748, 745}, {721, 723, 729}, {721, 729, 746}, 
	{746, 729, 731}, {746, 731, 747}, {747, 731, 733}, {747, 733, 748}, 
	{751, 750, 749}, {749, 752, 751}, {753, 751, 752}, {752, 754, 753}, 
	{755, 753, 754}, {754, 756, 755}, {752, 749, 757}, {757, 758, 752}, 
	{752, 758, 759}, {752, 759, 754}, {754, 759, 760}, {754, 760, 756}, 
	{758, 757, 761}, {761, 762, 758}, {758, 762, 763}, {758, 763, 759}, 
	{759, 763, 764}, {759, 764, 760}, {761, 766, 765}, {761, 765, 762}, 
	{763, 762, 765}, {765, 767, 763}, {764, 763, 767}, {767, 768, 764}, 
	{766, 770, 769}, {766, 769, 765}, {767, 765, 769}, {769, 771, 767}, 
	{768, 767, 771}, {771, 772, 768}, {770, 712, 713}, {770, 713, 769}, 
	{769, 713, 715}, {769, 715, 771}, {771, 715, 717}, {771, 717, 772}, 
	{774, 773, 764}, {764, 768, 774}, {775, 773, 774}, {774, 776, 775}, 
	{776, 778, 777}, {775, 776, 777}, {768, 772, 779}, {768, 779, 774}, 
	{776, 774, 779}, {779, 780, 776}, {778, 776, 780}, {780, 781, 778}, 
	{779, 772, 717}, {717, 740, 779}, {780, 779, 740}, {740, 742, 780}, 
	{781, 780, 742}, {742, 744, 781}, {755, 756, 782}, {755, 782, 783}, 
	{783, 782, 784}, {783, 784, 785}, {785, 784, 786}, {785, 786, 787}, 
	{782, 756, 760}, {760, 788, 782}, {782, 788, 789}, {782, 789, 784}, 
	{784, 789, 790}, {784, 790, 786}, {764, 773, 788}, {760, 764, 788}, 
	{788, 773, 775}, {788, 775, 789}, {777, 790, 789}, {789, 775, 777}, 
	{787, 786, 791}, {787, 791, 792}, {792, 791, 793}, {792, 793, 794}, 
	{794, 793, 795}, {794, 795, 796}, {786, 790, 797}, {786, 797, 791}, 
	{791, 797, 798}, {791, 798, 793}, {793, 798, 799}, {793, 799, 795}, 
	{790, 777, 800}, {790, 800, 797}, {797, 800, 801}, {797, 801, 798}, 
	{802, 799, 798}, {798, 801, 802}, {800, 777, 778}, {778, 803, 800}, 
	{801, 800, 803}, {803, 804, 801}, {804, 805, 802}, {801, 804, 802}, 
	{803, 778, 781}, {781, 806, 803}, {804, 803, 806}, {806, 807, 804}, 
	{805, 804, 807}, {807, 808, 805}, {806, 781, 744}, {744, 745, 806}, 
	{807, 806, 745}, {745, 748, 807}, {808, 807, 748}, {748, 733, 808}, 
	{809, 802, 805}, {805, 810, 809}, {811, 809, 810}, {810, 812, 811}, 
	{812, 814, 813}, {811, 812, 813}, {810, 805, 808}, {808, 815, 810}, 
	{812, 810, 815}, {815, 816, 812}, {814, 812, 816}, {816, 817, 814}, 
	{815, 808, 733}, {733, 732, 815}, {816, 815, 732}, {732, 736, 816}, 
	{817, 816, 736}, {736, 739, 817}, {796, 795, 818}, {796, 818, 819}, 
	{819, 818, 820}, {819, 820, 821}, {821, 820, 822}, {821, 822, 823}, 
	{795, 799, 824}, {795, 824, 818}, {818, 824, 825}, {818, 825, 820}, 
	{820, 825, 826}, {820, 826, 822}, {799, 802, 809}, {799, 809, 824}, 
	{824, 809, 811}, {824, 811, 825}, {813, 826, 825}, {825, 811, 813}, 
	{823, 822, 827}, {823, 827, 828}, {828, 827, 829}, {828, 829, 830}, 
	{830, 829, 831}, {830, 831, 832}, {822, 826, 833}, {822, 833, 827}, 
	{827, 833, 834}, {827, 834, 829}, {829, 834, 835}, {829, 835, 831}, 
	{826, 813, 836}, {826, 836, 833}, {833, 836, 837}, {833, 837, 834}, 
	{834, 837, 838}, {834, 838, 835}, {836, 813, 814}, {814, 839, 836}, 
	{837, 836, 839}, {839, 840, 837}, {838, 837, 840}, {840, 841, 838}, 
	{839, 814, 817}, {817, 842, 839}, {840, 839, 842}, {842, 843, 840}, 
	{841, 840, 843}, {843, 844, 841}, {842, 817, 739}, {739, 738, 842}, 
	{843, 842, 738}, {738, 737, 843}, {844, 843, 737}, {737, 727, 844}, 
	{838, 841, 845}, {838, 845, 846}, {841, 844, 847}, {841, 847, 845}, 
	{844, 727, 726}, {844, 726, 847}, {848, 832, 831}, {831, 849, 848}, 
	{849, 831, 835}, {835, 850, 849}, {850, 835, 838}, {838, 846, 850}, 
	{853, 852, 851}, {853, 851, 854}, {854, 851, 855}, {854, 855, 856}, 
	{856, 855, 766}, {856, 766, 761}, {852, 858, 857}, {852, 857, 851}, 
	{851, 857, 859}, {851, 859, 855}, {855, 859, 770}, {855, 770, 766}, 
	{858, 722, 718}, {858, 718, 857}, {857, 718, 711}, {857, 711, 859}, 
	{859, 711, 712}, {859, 712, 770}, {862, 861, 860}, {860, 863, 862}, 
	{864, 862, 863}, {863, 865, 864}, {750, 864, 865}, {865, 749, 750}, 
	{863, 860, 866}, {866, 867, 863}, {865, 863, 867}, {867, 868, 865}, 
	{749, 865, 868}, {868, 757, 749}, {867, 866, 853}, {853, 854, 867}, 
	{868, 867, 854}, {854, 856, 868}, {757, 868, 856}, {856, 761, 757}, 
	{750, 751, 869}, {750, 869, 864}, {862, 864, 869}, {869, 870, 862}, 
	{861, 862, 870}, {870, 705, 861}, {751, 753, 871}, {751, 871, 869}, 
	{870, 869, 871}, {871, 872, 870}, {705, 870, 872}, {872, 704, 705}, 
	{753, 755, 873}, {753, 873, 871}, {872, 871, 873}, {873, 874, 872}, 
	{704, 872, 874}, {874, 875, 704}, {873, 755, 783}, {783, 876, 873}, 
	{874, 873, 876}, {876, 877, 874}, {874, 877, 878}, {874, 878, 875}, 
	{876, 783, 785}, {785, 879, 876}, {877, 876, 879}, {879, 880, 877}, 
	{877, 880, 881}, {877, 881, 878}, {879, 785, 787}, {787, 792, 879}, 
	{879, 792, 794}, {879, 794, 880}, {880, 794, 796}, {880, 796, 881}, 
	{875, 878, 882}, {875, 882, 883}, {883, 882, 884}, {883, 884, 885}, 
	{885, 884, 830}, {885, 830, 832}, {878, 881, 886}, {878, 886, 882}, 
	{882, 886, 887}, {882, 887, 884}, {884, 887, 828}, {884, 828, 830}, 
	{881, 796, 819}, {881, 819, 886}, {886, 819, 821}, {886, 821, 887}, 
	{887, 821, 823}, {887, 823, 828}, {707, 704, 875}, {875, 883, 707}, 
	{709, 707, 883}, {883, 885, 709}, {848, 709, 885}, {885, 832, 848}
};


GLint r_iris_indices[70][3] = {
	{890, 889, 888}, {888, 892, 891}, {888, 891, 890}, {892, 888, 894}, 
	{894, 896, 895}, {894, 895, 892}, {896, 894, 897}, {897, 899, 898}, 
	{897, 898, 896}, {899, 897, 900}, {900, 902, 901}, {900, 901, 899}, 
	{902, 900, 903}, {903, 905, 904}, {903, 904, 902}, {905, 903, 906}, 
	{906, 907, 905}, {906, 909, 908}, {906, 908, 907}, {909, 911, 910}, 
	{909, 910, 912}, {909, 912, 908}, {911, 914, 913}, {911, 913, 915}, 
	{911, 915, 910}, {914, 917, 916}, {914, 916, 918}, {914, 918, 913}, 
	{917, 920, 919}, {917, 919, 921}, {917, 921, 916}, {920, 923, 922}, 
	{920, 922, 924}, {920, 924, 919}, {923, 926, 925}, {923, 925, 927}, 
	{923, 927, 922}, {926, 929, 928}, {926, 928, 930}, {926, 930, 925}, 
	{929, 932, 931}, {929, 931, 933}, {929, 933, 928}, {932, 935, 934}, 
	{932, 934, 936}, {932, 936, 931}, {935, 938, 937}, {935, 937, 939}, 
	{935, 939, 934}, {938, 941, 940}, {938, 940, 942}, {938, 942, 937}, 
	{940, 941, 943}, {943, 945, 944}, {943, 944, 940}, {945, 943, 946}, 
	{946, 948, 947}, {946, 947, 945}, {948, 946, 949}, {949, 951, 950}, 
	{949, 950, 948}, {951, 949, 952}, {952, 954, 953}, {952, 953, 951}, 
	{954, 952, 955}, {955, 957, 956}, {955, 956, 954}, {957, 955, 889}, 
	{958, 957, 889}, {889, 890, 958}
};


GLint r_pupil_indices[46][3] = {
	{893, 890, 891}, {893, 891, 892}, {893, 892, 895}, {893, 895, 896}, 
	{893, 896, 898}, {893, 898, 899}, {893, 899, 901}, {893, 901, 902}, 
	{893, 902, 904}, {893, 904, 905}, {893, 905, 907}, {893, 907, 908}, 
	{893, 908, 912}, {893, 912, 910}, {893, 910, 915}, {893, 915, 913}, 
	{893, 913, 918}, {893, 918, 916}, {893, 916, 921}, {893, 921, 919}, 
	{893, 919, 924}, {893, 924, 922}, {927, 925, 893}, {893, 922, 927}, 
	{930, 928, 893}, {893, 925, 930}, {893, 928, 933}, {893, 933, 931}, 
	{893, 931, 936}, {893, 936, 934}, {893, 934, 939}, {893, 939, 937}, 
	{893, 937, 942}, {893, 942, 940}, {893, 940, 944}, {893, 944, 945}, 
	{893, 945, 947}, {893, 947, 948}, {893, 948, 950}, {893, 950, 951}, 
	{893, 951, 953}, {893, 953, 954}, {893, 954, 956}, {893, 956, 957}, 
	{893, 957, 958}, {893, 958, 890}
};


GLint l_iris_indices[70][3] = {
	{961, 960, 959}, {961, 959, 962}, {961, 962, 963}, {965, 961, 963}, 
	{965, 963, 966}, {965, 966, 967}, {968, 965, 967}, {968, 967, 969}, 
	{968, 969, 970}, {971, 968, 970}, {971, 970, 972}, {971, 972, 973}, 
	{974, 971, 973}, {974, 973, 975}, {974, 975, 976}, {977, 974, 976}, 
	{976, 978, 977}, {980, 979, 977}, {977, 978, 980}, {982, 981, 979}, 
	{979, 980, 983}, {979, 983, 982}, {985, 984, 981}, {981, 982, 986}, 
	{981, 986, 985}, {988, 987, 984}, {984, 985, 989}, {984, 989, 988}, 
	{991, 990, 987}, {987, 988, 992}, {987, 992, 991}, {994, 993, 990}, 
	{990, 991, 995}, {990, 995, 994}, {997, 996, 993}, {998, 997, 993}, 
	{993, 994, 998}, {1000, 999, 996}, {996, 997, 1001}, {996, 1001, 1000}, 
	{1003, 1002, 999}, {999, 1000, 1004}, {999, 1004, 1003}, {1006, 1005, 1002}, 
	{1002, 1003, 1007}, {1002, 1007, 1006}, {1009, 1008, 1005}, {1005, 1006, 1010}, 
	{1005, 1010, 1009}, {1012, 1011, 1008}, {1008, 1009, 1013}, {1008, 1013, 1012}, 
	{1014, 1011, 1012}, {1014, 1012, 1015}, {1014, 1015, 1016}, {1017, 1014, 1016}, 
	{1017, 1016, 1018}, {1017, 1018, 1019}, {1020, 1017, 1019}, {1020, 1019, 1021}, 
	{1020, 1021, 1022}, {1023, 1020, 1022}, {1023, 1022, 1024}, {1023, 1024, 1025}, 
	{1026, 1023, 1025}, {1026, 1025, 1027}, {1026, 1027, 1028}, {960, 1026, 1028}, 
	{960, 1028, 1029}, {960, 1029, 959}
};


GLint l_pupil_indices[46][3] = {
	{964, 963, 962}, {964, 962, 959}, {964, 967, 966}, {964, 966, 963}, 
	{964, 970, 969}, {964, 969, 967}, {964, 973, 972}, {964, 972, 970}, 
	{964, 976, 975}, {964, 975, 973}, {964, 978, 976}, {964, 980, 978}, 
	{964, 982, 983}, {964, 983, 980}, {964, 985, 986}, {964, 986, 982}, 
	{964, 988, 989}, {964, 989, 985}, {964, 991, 992}, {964, 992, 988}, 
	{964, 994, 995}, {964, 995, 991}, {964, 997, 998}, {964, 998, 994}, 
	{964, 1000, 1001}, {964, 1001, 997}, {964, 1003, 1004}, {964, 1004, 1000}, 
	{964, 1006, 1007}, {964, 1007, 1003}, {964, 1009, 1010}, {964, 1010, 1006}, 
	{964, 1012, 1013}, {964, 1013, 1009}, {964, 1016, 1015}, {964, 1015, 1012}, 
	{964, 1019, 1018}, {964, 1018, 1016}, {964, 1022, 1021}, {964, 1021, 1019}, 
	{964, 1025, 1024}, {964, 1024, 1022}, {1027, 1025, 964}, {964, 1028, 1027}, 
	{964, 959, 1029}, {964, 1029, 1028}
};


GLint r_bulb_indices[240][3] = {
	{1031, 1030, 888}, {1031, 888, 889}, {1030, 1032, 894}, {1030, 894, 888}, 
	{1032, 1033, 897}, {1032, 897, 894}, {1033, 1034, 900}, {1033, 900, 897}, 
	{1034, 1035, 903}, {1034, 903, 900}, {1035, 1036, 906}, {1035, 906, 903}, 
	{906, 1036, 1037}, {1037, 909, 906}, {909, 1037, 1038}, {1038, 911, 909}, 
	{911, 1038, 1039}, {1039, 914, 911}, {914, 1039, 1040}, {1040, 917, 914}, 
	{917, 1040, 1041}, {1041, 920, 917}, {920, 1041, 1042}, {1042, 923, 920}, 
	{923, 1042, 1043}, {1043, 926, 923}, {926, 1043, 1044}, {1044, 929, 926}, 
	{929, 1044, 1045}, {1045, 932, 929}, {932, 1045, 1046}, {1046, 935, 932}, 
	{935, 1046, 1047}, {1047, 938, 935}, {938, 1047, 1048}, {1048, 941, 938}, 
	{1048, 1049, 943}, {1048, 943, 941}, {1049, 1050, 946}, {1049, 946, 943}, 
	{1050, 1051, 949}, {1050, 949, 946}, {1051, 1052, 952}, {1051, 952, 949}, 
	{1052, 1053, 955}, {1052, 955, 952}, {1053, 1031, 889}, {1053, 889, 955}, 
	{1055, 1054, 1030}, {1055, 1030, 1031}, {1054, 1056, 1032}, {1054, 1032, 1030}, 
	{1056, 1057, 1033}, {1056, 1033, 1032}, {1057, 1058, 1034}, {1057, 1034, 1033}, 
	{1058, 1059, 1035}, {1058, 1035, 1034}, {1059, 1060, 1036}, {1059, 1036, 1035}, 
	{1036, 1060, 1061}, {1061, 1037, 1036}, {1037, 1061, 1062}, {1062, 1038, 1037}, 
	{1038, 1062, 1063}, {1063, 1039, 1038}, {1039, 1063, 1064}, {1064, 1040, 1039}, 
	{1040, 1064, 1065}, {1065, 1041, 1040}, {1065, 1066, 1042}, {1065, 1042, 1041}, 
	{1042, 1066, 1067}, {1067, 1043, 1042}, {1067, 1068, 1044}, {1067, 1044, 1043}, 
	{1068, 1069, 1045}, {1068, 1045, 1044}, {1045, 1069, 1070}, {1070, 1046, 1045}, 
	{1070, 1071, 1047}, {1070, 1047, 1046}, {1047, 1071, 1072}, {1072, 1048, 1047}, 
	{1072, 1073, 1049}, {1072, 1049, 1048}, {1049, 1073, 1074}, {1074, 1050, 1049}, 
	{1074, 1075, 1051}, {1074, 1051, 1050}, {1075, 1076, 1052}, {1075, 1052, 1051}, 
	{1052, 1076, 1077}, {1077, 1053, 1052}, {1053, 1077, 1055}, {1055, 1031, 1053}, 
	{1079, 1078, 1054}, {1079, 1054, 1055}, {1078, 1080, 1056}, {1078, 1056, 1054}, 
	{1080, 1081, 1057}, {1080, 1057, 1056}, {1057, 1081, 1082}, {1082, 1058, 1057}, 
	{1058, 1082, 1083}, {1083, 1059, 1058}, {1059, 1083, 1084}, {1084, 1060, 1059}, 
	{1084, 1085, 1061}, {1084, 1061, 1060}, {1085, 1086, 1062}, {1085, 1062, 1061}, 
	{1086, 1087, 1063}, {1086, 1063, 1062}, {1063, 1087, 1088}, {1088, 1064, 1063}, 
	{1064, 1088, 1089}, {1089, 1065, 1064}, {1065, 1089, 1090}, {1090, 1066, 1065}, 
	{1090, 1091, 1067}, {1090, 1067, 1066}, {1091, 1092, 1068}, {1091, 1068, 1067}, 
	{1092, 1093, 1069}, {1092, 1069, 1068}, {1069, 1093, 1094}, {1094, 1070, 1069}, 
	{1070, 1094, 1095}, {1095, 1071, 1070}, {1071, 1095, 1096}, {1096, 1072, 1071}, 
	{1096, 1097, 1073}, {1096, 1073, 1072}, {1097, 1098, 1074}, {1097, 1074, 1073}, 
	{1098, 1099, 1075}, {1098, 1075, 1074}, {1075, 1099, 1100}, {1100, 1076, 1075}, 
	{1076, 1100, 1101}, {1101, 1077, 1076}, {1077, 1101, 1079}, {1079, 1055, 1077}, 
	{1103, 1102, 1078}, {1103, 1078, 1079}, {1102, 1104, 1080}, {1102, 1080, 1078}, 
	{1104, 1105, 1081}, {1104, 1081, 1080}, {1081, 1105, 1106}, {1106, 1082, 1081}, 
	{1082, 1106, 1107}, {1107, 1083, 1082}, {1083, 1107, 1108}, {1108, 1084, 1083}, 
	{1108, 1109, 1085}, {1108, 1085, 1084}, {1109, 1110, 1086}, {1109, 1086, 1085}, 
	{1110, 1111, 1087}, {1110, 1087, 1086}, {1087, 1111, 1112}, {1112, 1088, 1087}, 
	{1088, 1112, 1113}, {1113, 1089, 1088}, {1089, 1113, 1114}, {1114, 1090, 1089}, 
	{1114, 1115, 1091}, {1114, 1091, 1090}, {1115, 1116, 1092}, {1115, 1092, 1091}, 
	{1116, 1117, 1093}, {1116, 1093, 1092}, {1093, 1117, 1118}, {1118, 1094, 1093}, 
	{1094, 1118, 1119}, {1119, 1095, 1094}, {1095, 1119, 1120}, {1120, 1096, 1095}, 
	{1120, 1121, 1097}, {1120, 1097, 1096}, {1121, 1122, 1098}, {1121, 1098, 1097}, 
	{1122, 1123, 1099}, {1122, 1099, 1098}, {1099, 1123, 1124}, {1124, 1100, 1099}, 
	{1100, 1124, 1125}, {1125, 1101, 1100}, {1101, 1125, 1103}, {1103, 1079, 1101}, 
	{1127, 1126, 1102}, {1127, 1102, 1103}, {1126, 1128, 1104}, {1126, 1104, 1102}, 
	{1128, 1129, 1105}, {1128, 1105, 1104}, {1105, 1129, 1130}, {1130, 1106, 1105}, 
	{1106, 1130, 1131}, {1131, 1107, 1106}, {1107, 1131, 1132}, {1132, 1108, 1107}, 
	{1132, 1133, 1109}, {1132, 1109, 1108}, {1133, 1134, 1110}, {1133, 1110, 1109}, 
	{1134, 1135, 1111}, {1134, 1111, 1110}, {1111, 1135, 1136}, {1136, 1112, 1111}, 
	{1112, 1136, 1137}, {1137, 1113, 1112}, {1113, 1137, 1138}, {1138, 1114, 1113}, 
	{1138, 1139, 1115}, {1138, 1115, 1114}, {1139, 1140, 1116}, {1139, 1116, 1115}, 
	{1140, 1141, 1117}, {1140, 1117, 1116}, {1117, 1141, 1142}, {1142, 1118, 1117}, 
	{1118, 1142, 1143}, {1143, 1119, 1118}, {1119, 1143, 1144}, {1144, 1120, 1119}, 
	{1144, 1145, 1121}, {1144, 1121, 1120}, {1145, 1146, 1122}, {1145, 1122, 1121}, 
	{1146, 1147, 1123}, {1146, 1123, 1122}, {1123, 1147, 1148}, {1148, 1124, 1123}, 
	{1124, 1148, 1149}, {1149, 1125, 1124}, {1125, 1149, 1127}, {1127, 1103, 1125}
};


GLint l_bulb_indices[240][3] = {
	{1150, 960, 961}, {1150, 961, 1151}, {1151, 961, 965}, {1151, 965, 1152}, 
	{1152, 965, 968}, {1152, 968, 1153}, {1153, 968, 971}, {1153, 971, 1154}, 
	{1154, 971, 974}, {1154, 974, 1155}, {1155, 974, 977}, {1155, 977, 1156}, 
	{1157, 1156, 977}, {977, 979, 1157}, {1158, 1157, 979}, {979, 981, 1158}, 
	{1159, 1158, 981}, {981, 984, 1159}, {1160, 1159, 984}, {984, 987, 1160}, 
	{1161, 1160, 987}, {987, 990, 1161}, {1162, 1161, 990}, {990, 993, 1162}, 
	{1163, 1162, 993}, {993, 996, 1163}, {1164, 1163, 996}, {996, 999, 1164}, 
	{1165, 1164, 999}, {999, 1002, 1165}, {1166, 1165, 1002}, {1002, 1005, 1166}, 
	{1167, 1166, 1005}, {1005, 1008, 1167}, {1168, 1167, 1008}, {1008, 1011, 1168}, 
	{1168, 1011, 1014}, {1168, 1014, 1169}, {1169, 1014, 1017}, {1169, 1017, 1170}, 
	{1170, 1017, 1020}, {1170, 1020, 1171}, {1171, 1020, 1023}, {1171, 1023, 1172}, 
	{1172, 1023, 1026}, {1172, 1026, 1173}, {1173, 1026, 960}, {1173, 960, 1150}, 
	{1174, 1150, 1151}, {1174, 1151, 1175}, {1175, 1151, 1152}, {1175, 1152, 1176}, 
	{1176, 1152, 1153}, {1176, 1153, 1177}, {1177, 1153, 1154}, {1177, 1154, 1178}, 
	{1178, 1154, 1155}, {1178, 1155, 1179}, {1179, 1155, 1156}, {1179, 1156, 1180}, 
	{1181, 1180, 1156}, {1156, 1157, 1181}, {1182, 1181, 1157}, {1157, 1158, 1182}, 
	{1183, 1182, 1158}, {1158, 1159, 1183}, {1184, 1183, 1159}, {1159, 1160, 1184}, 
	{1185, 1184, 1160}, {1160, 1161, 1185}, {1185, 1161, 1162}, {1185, 1162, 1186}, 
	{1187, 1186, 1162}, {1162, 1163, 1187}, {1187, 1163, 1164}, {1187, 1164, 1188}, 
	{1188, 1164, 1165}, {1188, 1165, 1189}, {1190, 1189, 1165}, {1165, 1166, 1190}, 
	{1190, 1166, 1167}, {1190, 1167, 1191}, {1192, 1191, 1167}, {1167, 1168, 1192}, 
	{1192, 1168, 1169}, {1192, 1169, 1193}, {1194, 1193, 1169}, {1169, 1170, 1194}, 
	{1194, 1170, 1171}, {1194, 1171, 1195}, {1195, 1171, 1172}, {1195, 1172, 1196}, 
	{1197, 1196, 1172}, {1172, 1173, 1197}, {1174, 1197, 1173}, {1173, 1150, 1174}, 
	{1198, 1174, 1175}, {1198, 1175, 1199}, {1199, 1175, 1176}, {1199, 1176, 1200}, 
	{1200, 1176, 1177}, {1200, 1177, 1201}, {1202, 1201, 1177}, {1177, 1178, 1202}, 
	{1203, 1202, 1178}, {1178, 1179, 1203}, {1204, 1203, 1179}, {1179, 1180, 1204}, 
	{1204, 1180, 1181}, {1204, 1181, 1205}, {1205, 1181, 1182}, {1205, 1182, 1206}, 
	{1206, 1182, 1183}, {1206, 1183, 1207}, {1208, 1207, 1183}, {1183, 1184, 1208}, 
	{1209, 1208, 1184}, {1184, 1185, 1209}, {1210, 1209, 1185}, {1185, 1186, 1210}, 
	{1210, 1186, 1187}, {1210, 1187, 1211}, {1211, 1187, 1188}, {1211, 1188, 1212}, 
	{1212, 1188, 1189}, {1212, 1189, 1213}, {1214, 1213, 1189}, {1189, 1190, 1214}, 
	{1215, 1214, 1190}, {1190, 1191, 1215}, {1216, 1215, 1191}, {1191, 1192, 1216}, 
	{1216, 1192, 1193}, {1216, 1193, 1217}, {1217, 1193, 1194}, {1217, 1194, 1218}, 
	{1218, 1194, 1195}, {1218, 1195, 1219}, {1220, 1219, 1195}, {1195, 1196, 1220}, 
	{1221, 1220, 1196}, {1196, 1197, 1221}, {1198, 1221, 1197}, {1197, 1174, 1198}, 
	{1222, 1198, 1199}, {1222, 1199, 1223}, {1223, 1199, 1200}, {1223, 1200, 1224}, 
	{1224, 1200, 1201}, {1224, 1201, 1225}, {1226, 1225, 1201}, {1201, 1202, 1226}, 
	{1227, 1226, 1202}, {1202, 1203, 1227}, {1228, 1227, 1203}, {1203, 1204, 1228}, 
	{1228, 1204, 1205}, {1228, 1205, 1229}, {1229, 1205, 1206}, {1229, 1206, 1230}, 
	{1230, 1206, 1207}, {1230, 1207, 1231}, {1232, 1231, 1207}, {1207, 1208, 1232}, 
	{1233, 1232, 1208}, {1208, 1209, 1233}, {1234, 1233, 1209}, {1209, 1210, 1234}, 
	{1234, 1210, 1211}, {1234, 1211, 1235}, {1235, 1211, 1212}, {1235, 1212, 1236}, 
	{1236, 1212, 1213}, {1236, 1213, 1237}, {1238, 1237, 1213}, {1213, 1214, 1238}, 
	{1239, 1238, 1214}, {1214, 1215, 1239}, {1240, 1239, 1215}, {1215, 1216, 1240}, 
	{1240, 1216, 1217}, {1240, 1217, 1241}, {1241, 1217, 1218}, {1241, 1218, 1242}, 
	{1242, 1218, 1219}, {1242, 1219, 1243}, {1244, 1243, 1219}, {1219, 1220, 1244}, 
	{1245, 1244, 1220}, {1220, 1221, 1245}, {1222, 1245, 1221}, {1221, 1198, 1222}, 
	{1246, 1222, 1223}, {1246, 1223, 1247}, {1247, 1223, 1224}, {1247, 1224, 1248}, 
	{1248, 1224, 1225}, {1248, 1225, 1249}, {1250, 1249, 1225}, {1225, 1226, 1250}, 
	{1251, 1250, 1226}, {1226, 1227, 1251}, {1252, 1251, 1227}, {1227, 1228, 1252}, 
	{1252, 1228, 1229}, {1252, 1229, 1253}, {1253, 1229, 1230}, {1253, 1230, 1254}, 
	{1254, 1230, 1231}, {1254, 1231, 1255}, {1256, 1255, 1231}, {1231, 1232, 1256}, 
	{1257, 1256, 1232}, {1232, 1233, 1257}, {1258, 1257, 1233}, {1233, 1234, 1258}, 
	{1258, 1234, 1235}, {1258, 1235, 1259}, {1259, 1235, 1236}, {1259, 1236, 1260}, 
	{1260, 1236, 1237}, {1260, 1237, 1261}, {1262, 1261, 1237}, {1237, 1238, 1262}, 
	{1263, 1262, 1238}, {1238, 1239, 1263}, {1264, 1263, 1239}, {1239, 1240, 1264}, 
	{1264, 1240, 1241}, {1264, 1241, 1265}, {1265, 1241, 1242}, {1265, 1242, 1266}, 
	{1266, 1242, 1243}, {1266, 1243, 1267}, {1268, 1267, 1243}, {1243, 1244, 1268}, 
	{1269, 1268, 1244}, {1244, 1245, 1269}, {1246, 1269, 1245}, {1245, 1222, 1246}
};


GLint inf_teeth_indices[1196][3] = {
	{1272, 1271, 1270}, {1274, 1273, 1272}, {1271, 1272, 1273}, {1276, 1275, 1274}, 
	{1273, 1274, 1275}, {1279, 1278, 1277}, {1281, 1280, 1279}, {1278, 1279, 1280}, 
	{1283, 1282, 1281}, {1280, 1281, 1282}, {1286, 1285, 1284}, {1270, 1288, 1287}, 
	{1287, 1290, 1289}, {1290, 1287, 1288}, {1289, 1292, 1291}, {1292, 1289, 1290}, 
	{1295, 1294, 1293}, {1293, 1297, 1296}, {1297, 1293, 1294}, {1296, 1299, 1298}, 
	{1299, 1296, 1297}, {1302, 1301, 1300}, {1305, 1304, 1303}, {1308, 1307, 1306}, 
	{1309, 1270, 1271}, {1271, 1310, 1309}, {1311, 1309, 1310}, {1273, 1312, 1271}, 
	{1310, 1271, 1312}, {1275, 1313, 1273}, {1312, 1273, 1313}, {1277, 1314, 1276}, 
	{1275, 1276, 1314}, {1314, 1315, 1275}, {1313, 1275, 1315}, {1314, 1277, 1278}, 
	{1278, 1316, 1314}, {1315, 1314, 1316}, {1280, 1317, 1278}, {1316, 1278, 1317}, 
	{1282, 1318, 1280}, {1317, 1280, 1318}, {1320, 1319, 1283}, {1282, 1283, 1319}, 
	{1319, 1321, 1282}, {1318, 1282, 1321}, {1284, 1322, 1320}, {1319, 1320, 1322}, 
	{1322, 1323, 1319}, {1321, 1319, 1323}, {1322, 1284, 1285}, {1285, 1324, 1322}, 
	{1323, 1322, 1324}, {1326, 1325, 1286}, {1285, 1286, 1325}, {1325, 1327, 1285}, 
	{1324, 1285, 1327}, {1329, 1328, 1326}, {1325, 1326, 1328}, {1328, 1330, 1325}, 
	{1327, 1325, 1330}, {1332, 1331, 1329}, {1328, 1329, 1331}, {1331, 1333, 1328}, 
	{1330, 1328, 1333}, {1335, 1334, 1332}, {1331, 1332, 1334}, {1334, 1336, 1331}, 
	{1333, 1331, 1336}, {1338, 1337, 1335}, {1334, 1335, 1337}, {1337, 1339, 1334}, 
	{1336, 1334, 1339}, {1341, 1340, 1338}, {1337, 1338, 1340}, {1340, 1342, 1337}, 
	{1339, 1337, 1342}, {1344, 1343, 1341}, {1340, 1341, 1343}, {1343, 1345, 1340}, 
	{1342, 1340, 1345}, {1347, 1346, 1344}, {1343, 1344, 1346}, {1346, 1348, 1343}, 
	{1345, 1343, 1348}, {1350, 1349, 1347}, {1346, 1347, 1349}, {1349, 1351, 1346}, 
	{1348, 1346, 1351}, {1353, 1352, 1350}, {1349, 1350, 1352}, {1352, 1354, 1349}, 
	{1351, 1349, 1354}, {1356, 1355, 1353}, {1352, 1353, 1355}, {1355, 1357, 1352}, 
	{1354, 1352, 1357}, {1359, 1358, 1356}, {1355, 1356, 1358}, {1358, 1360, 1355}, 
	{1357, 1355, 1360}, {1362, 1361, 1359}, {1358, 1359, 1361}, {1361, 1363, 1358}, 
	{1360, 1358, 1363}, {1365, 1364, 1362}, {1361, 1362, 1364}, {1364, 1366, 1361}, 
	{1363, 1361, 1366}, {1368, 1367, 1365}, {1364, 1365, 1367}, {1367, 1369, 1364}, 
	{1366, 1364, 1369}, {1371, 1370, 1368}, {1367, 1368, 1370}, {1370, 1372, 1367}, 
	{1369, 1367, 1372}, {1374, 1373, 1371}, {1370, 1371, 1373}, {1373, 1375, 1370}, 
	{1372, 1370, 1375}, {1377, 1376, 1374}, {1373, 1374, 1376}, {1376, 1378, 1373}, 
	{1375, 1373, 1378}, {1380, 1379, 1377}, {1376, 1377, 1379}, {1379, 1381, 1376}, 
	{1378, 1376, 1381}, {1383, 1382, 1380}, {1379, 1380, 1382}, {1382, 1384, 1379}, 
	{1381, 1379, 1384}, {1386, 1385, 1383}, {1382, 1383, 1385}, {1385, 1387, 1382}, 
	{1384, 1382, 1387}, {1389, 1388, 1386}, {1385, 1386, 1388}, {1388, 1390, 1385}, 
	{1387, 1385, 1390}, {1392, 1391, 1389}, {1388, 1389, 1391}, {1391, 1393, 1388}, 
	{1390, 1388, 1393}, {1395, 1394, 1392}, {1391, 1392, 1394}, {1394, 1396, 1391}, 
	{1393, 1391, 1396}, {1398, 1397, 1395}, {1394, 1395, 1397}, {1397, 1399, 1394}, 
	{1396, 1394, 1399}, {1401, 1400, 1398}, {1397, 1398, 1400}, {1400, 1402, 1397}, 
	{1399, 1397, 1402}, {1404, 1403, 1401}, {1400, 1401, 1403}, {1403, 1405, 1400}, 
	{1402, 1400, 1405}, {1407, 1406, 1404}, {1403, 1404, 1406}, {1406, 1408, 1403}, 
	{1405, 1403, 1408}, {1410, 1409, 1407}, {1406, 1407, 1409}, {1409, 1411, 1406}, 
	{1408, 1406, 1411}, {1413, 1412, 1410}, {1409, 1410, 1412}, {1412, 1414, 1409}, 
	{1411, 1409, 1414}, {1416, 1415, 1413}, {1412, 1413, 1415}, {1415, 1417, 1412}, 
	{1414, 1412, 1417}, {1419, 1418, 1416}, {1415, 1416, 1418}, {1418, 1420, 1415}, 
	{1417, 1415, 1420}, {1422, 1421, 1419}, {1418, 1419, 1421}, {1421, 1423, 1418}, 
	{1420, 1418, 1423}, {1425, 1424, 1422}, {1421, 1422, 1424}, {1424, 1426, 1421}, 
	{1423, 1421, 1426}, {1428, 1427, 1425}, {1424, 1425, 1427}, {1427, 1429, 1424}, 
	{1426, 1424, 1429}, {1431, 1430, 1428}, {1427, 1428, 1430}, {1430, 1432, 1427}, 
	{1429, 1427, 1432}, {1434, 1433, 1431}, {1430, 1431, 1433}, {1433, 1435, 1430}, 
	{1432, 1430, 1435}, {1437, 1436, 1434}, {1433, 1434, 1436}, {1436, 1438, 1433}, 
	{1435, 1433, 1438}, {1440, 1439, 1437}, {1436, 1437, 1439}, {1439, 1441, 1436}, 
	{1438, 1436, 1441}, {1443, 1442, 1440}, {1439, 1440, 1442}, {1442, 1444, 1439}, 
	{1441, 1439, 1444}, {1446, 1445, 1443}, {1442, 1443, 1445}, {1445, 1447, 1442}, 
	{1444, 1442, 1447}, {1449, 1448, 1446}, {1445, 1446, 1448}, {1448, 1450, 1445}, 
	{1447, 1445, 1450}, {1452, 1451, 1449}, {1448, 1449, 1451}, {1451, 1453, 1448}, 
	{1450, 1448, 1453}, {1455, 1454, 1452}, {1451, 1452, 1454}, {1454, 1456, 1451}, 
	{1453, 1451, 1456}, {1458, 1457, 1455}, {1454, 1455, 1457}, {1457, 1459, 1454}, 
	{1456, 1454, 1459}, {1461, 1460, 1458}, {1457, 1458, 1460}, {1460, 1462, 1457}, 
	{1459, 1457, 1462}, {1464, 1463, 1461}, {1460, 1461, 1463}, {1463, 1465, 1460}, 
	{1462, 1460, 1465}, {1467, 1466, 1464}, {1463, 1464, 1466}, {1466, 1468, 1463}, 
	{1465, 1463, 1468}, {1470, 1469, 1467}, {1466, 1467, 1469}, {1469, 1471, 1466}, 
	{1468, 1466, 1471}, {1473, 1472, 1470}, {1469, 1470, 1472}, {1472, 1474, 1469}, 
	{1471, 1469, 1474}, {1476, 1475, 1473}, {1472, 1473, 1475}, {1475, 1477, 1472}, 
	{1474, 1472, 1477}, {1479, 1478, 1476}, {1475, 1476, 1478}, {1478, 1480, 1475}, 
	{1477, 1475, 1480}, {1482, 1481, 1479}, {1478, 1479, 1481}, {1481, 1483, 1478}, 
	{1480, 1478, 1483}, {1485, 1480, 1484}, {1483, 1484, 1480}, {1399, 1486, 1396}, 
	{1487, 1396, 1486}, {1486, 1488, 1487}, {1489, 1487, 1488}, {1490, 1393, 1487}, 
	{1396, 1487, 1393}, {1491, 1490, 1489}, {1487, 1489, 1490}, {1402, 1492, 1399}, 
	{1486, 1399, 1492}, {1492, 1493, 1486}, {1488, 1486, 1493}, {1405, 1494, 1402}, 
	{1492, 1402, 1494}, {1494, 1495, 1492}, {1493, 1492, 1495}, {1408, 1496, 1405}, 
	{1494, 1405, 1496}, {1496, 1497, 1494}, {1495, 1494, 1497}, {1411, 1498, 1408}, 
	{1496, 1408, 1498}, {1498, 1499, 1496}, {1497, 1496, 1499}, {1414, 1500, 1411}, 
	{1498, 1411, 1500}, {1500, 1501, 1498}, {1499, 1498, 1501}, {1417, 1502, 1414}, 
	{1500, 1414, 1502}, {1502, 1503, 1500}, {1501, 1500, 1503}, {1420, 1504, 1417}, 
	{1502, 1417, 1504}, {1504, 1505, 1502}, {1503, 1502, 1505}, {1423, 1506, 1420}, 
	{1504, 1420, 1506}, {1506, 1507, 1504}, {1505, 1504, 1507}, {1426, 1508, 1423}, 
	{1506, 1423, 1508}, {1508, 1509, 1506}, {1507, 1506, 1509}, {1429, 1510, 1426}, 
	{1508, 1426, 1510}, {1510, 1511, 1508}, {1509, 1508, 1511}, {1432, 1512, 1429}, 
	{1510, 1429, 1512}, {1512, 1513, 1510}, {1511, 1510, 1513}, {1435, 1514, 1432}, 
	{1512, 1432, 1514}, {1514, 1515, 1512}, {1513, 1512, 1515}, {1438, 1516, 1435}, 
	{1514, 1435, 1516}, {1516, 1517, 1514}, {1515, 1514, 1517}, {1441, 1518, 1438}, 
	{1516, 1438, 1518}, {1518, 1519, 1516}, {1517, 1516, 1519}, {1444, 1303, 1441}, 
	{1518, 1441, 1303}, {1303, 1304, 1518}, {1519, 1518, 1304}, {1447, 1305, 1444}, 
	{1303, 1444, 1305}, {1447, 1450, 1305}, {1520, 1305, 1450}, {1520, 1521, 1305}, 
	{1522, 1305, 1521}, {1523, 1304, 1522}, {1305, 1522, 1304}, {1308, 1524, 1307}, 
	{1525, 1307, 1524}, {1525, 1381, 1307}, {1384, 1307, 1381}, {1384, 1387, 1307}, 
	{1306, 1307, 1387}, {1527, 1526, 1306}, {1308, 1306, 1526}, {1526, 1528, 1308}, 
	{1524, 1308, 1528}, {1490, 1491, 1527}, {1526, 1527, 1491}, {1491, 1489, 1526}, 
	{1528, 1526, 1489}, {1527, 1390, 1490}, {1393, 1490, 1390}, {1387, 1390, 1306}, 
	{1527, 1306, 1390}, {1530, 1529, 1524}, {1525, 1524, 1529}, {1531, 1530, 1528}, 
	{1524, 1528, 1530}, {1533, 1532, 1530}, {1529, 1530, 1532}, {1534, 1533, 1531}, 
	{1530, 1531, 1533}, {1536, 1535, 1533}, {1532, 1533, 1535}, {1537, 1536, 1534}, 
	{1533, 1534, 1536}, {1539, 1538, 1536}, {1535, 1536, 1538}, {1540, 1539, 1537}, 
	{1536, 1537, 1539}, {1542, 1541, 1539}, {1538, 1539, 1541}, {1543, 1542, 1540}, 
	{1539, 1540, 1542}, {1545, 1544, 1542}, {1541, 1542, 1544}, {1546, 1545, 1543}, 
	{1542, 1543, 1545}, {1548, 1547, 1545}, {1544, 1545, 1547}, {1549, 1548, 1546}, 
	{1545, 1546, 1548}, {1551, 1550, 1548}, {1547, 1548, 1550}, {1552, 1551, 1549}, 
	{1548, 1549, 1551}, {1554, 1553, 1551}, {1550, 1551, 1553}, {1555, 1554, 1552}, 
	{1551, 1552, 1554}, {1557, 1556, 1554}, {1553, 1554, 1556}, {1558, 1557, 1555}, 
	{1554, 1555, 1557}, {1560, 1559, 1557}, {1556, 1557, 1559}, {1561, 1560, 1558}, 
	{1557, 1558, 1560}, {1563, 1562, 1560}, {1559, 1560, 1562}, {1564, 1563, 1561}, 
	{1560, 1561, 1563}, {1566, 1565, 1563}, {1562, 1563, 1565}, {1567, 1566, 1564}, 
	{1563, 1564, 1566}, {1569, 1568, 1566}, {1565, 1566, 1568}, {1570, 1569, 1567}, 
	{1566, 1567, 1569}, {1572, 1571, 1569}, {1568, 1569, 1571}, {1573, 1572, 1570}, 
	{1569, 1570, 1572}, {1575, 1574, 1572}, {1571, 1572, 1574}, {1523, 1575, 1573}, 
	{1572, 1573, 1575}, {1577, 1576, 1575}, {1574, 1575, 1576}, {1522, 1577, 1523}, 
	{1575, 1523, 1577}, {1579, 1578, 1577}, {1576, 1577, 1578}, {1521, 1579, 1522}, 
	{1577, 1522, 1579}, {1581, 1580, 1579}, {1578, 1579, 1580}, {1582, 1581, 1521}, 
	{1579, 1521, 1581}, {1584, 1583, 1581}, {1580, 1581, 1583}, {1585, 1584, 1582}, 
	{1581, 1582, 1584}, {1587, 1586, 1584}, {1583, 1584, 1586}, {1588, 1587, 1585}, 
	{1584, 1585, 1587}, {1590, 1589, 1587}, {1586, 1587, 1589}, {1591, 1590, 1588}, 
	{1587, 1588, 1590}, {1593, 1592, 1590}, {1589, 1590, 1592}, {1594, 1593, 1591}, 
	{1590, 1591, 1593}, {1596, 1595, 1593}, {1592, 1593, 1595}, {1597, 1596, 1594}, 
	{1593, 1594, 1596}, {1599, 1598, 1596}, {1595, 1596, 1598}, {1600, 1599, 1597}, 
	{1596, 1597, 1599}, {1602, 1601, 1599}, {1598, 1599, 1601}, {1603, 1602, 1600}, 
	{1599, 1600, 1602}, {1605, 1604, 1602}, {1601, 1602, 1604}, {1606, 1605, 1603}, 
	{1602, 1603, 1605}, {1608, 1607, 1605}, {1604, 1605, 1607}, {1609, 1608, 1606}, 
	{1605, 1606, 1608}, {1611, 1610, 1608}, {1607, 1608, 1610}, {1612, 1611, 1609}, 
	{1608, 1609, 1611}, {1488, 1531, 1489}, {1528, 1489, 1531}, {1612, 1609, 1484}, 
	{1485, 1484, 1609}, {1450, 1453, 1520}, {1613, 1520, 1453}, {1613, 1582, 1520}, 
	{1521, 1520, 1582}, {1453, 1456, 1613}, {1614, 1613, 1456}, {1614, 1585, 1613}, 
	{1582, 1613, 1585}, {1456, 1459, 1614}, {1615, 1614, 1459}, {1615, 1588, 1614}, 
	{1585, 1614, 1588}, {1459, 1462, 1615}, {1616, 1615, 1462}, {1616, 1591, 1615}, 
	{1588, 1615, 1591}, {1462, 1465, 1616}, {1617, 1616, 1465}, {1617, 1594, 1616}, 
	{1591, 1616, 1594}, {1465, 1468, 1617}, {1618, 1617, 1468}, {1618, 1597, 1617}, 
	{1594, 1617, 1597}, {1468, 1471, 1618}, {1619, 1618, 1471}, {1619, 1600, 1618}, 
	{1597, 1618, 1600}, {1471, 1474, 1619}, {1620, 1619, 1474}, {1620, 1603, 1619}, 
	{1600, 1619, 1603}, {1474, 1477, 1620}, {1621, 1620, 1477}, {1621, 1606, 1620}, 
	{1603, 1620, 1606}, {1477, 1480, 1621}, {1485, 1621, 1480}, {1485, 1609, 1621}, 
	{1606, 1621, 1609}, {1493, 1534, 1488}, {1531, 1488, 1534}, {1495, 1537, 1493}, 
	{1534, 1493, 1537}, {1497, 1540, 1495}, {1537, 1495, 1540}, {1499, 1543, 1497}, 
	{1540, 1497, 1543}, {1501, 1546, 1499}, {1543, 1499, 1546}, {1503, 1549, 1501}, 
	{1546, 1501, 1549}, {1505, 1552, 1503}, {1549, 1503, 1552}, {1507, 1555, 1505}, 
	{1552, 1505, 1555}, {1509, 1558, 1507}, {1555, 1507, 1558}, {1511, 1561, 1509}, 
	{1558, 1509, 1561}, {1513, 1564, 1511}, {1561, 1511, 1564}, {1515, 1567, 1513}, 
	{1564, 1513, 1567}, {1517, 1570, 1515}, {1567, 1515, 1570}, {1519, 1573, 1517}, 
	{1570, 1517, 1573}, {1304, 1523, 1519}, {1573, 1519, 1523}, {1529, 1378, 1525}, 
	{1381, 1525, 1378}, {1532, 1375, 1529}, {1378, 1529, 1375}, {1535, 1372, 1532}, 
	{1375, 1532, 1372}, {1538, 1369, 1535}, {1372, 1535, 1369}, {1541, 1366, 1538}, 
	{1369, 1538, 1366}, {1544, 1363, 1541}, {1366, 1541, 1363}, {1547, 1360, 1544}, 
	{1363, 1544, 1360}, {1550, 1357, 1547}, {1360, 1547, 1357}, {1553, 1354, 1550}, 
	{1357, 1550, 1354}, {1556, 1351, 1553}, {1354, 1553, 1351}, {1559, 1348, 1556}, 
	{1351, 1556, 1348}, {1562, 1345, 1559}, {1348, 1559, 1345}, {1565, 1342, 1562}, 
	{1345, 1562, 1342}, {1568, 1339, 1565}, {1342, 1565, 1339}, {1571, 1336, 1568}, 
	{1339, 1568, 1336}, {1574, 1333, 1571}, {1336, 1571, 1333}, {1576, 1330, 1574}, 
	{1333, 1574, 1330}, {1578, 1327, 1576}, {1330, 1576, 1327}, {1580, 1324, 1578}, 
	{1327, 1578, 1324}, {1583, 1323, 1580}, {1324, 1580, 1323}, {1586, 1321, 1583}, 
	{1323, 1583, 1321}, {1589, 1318, 1586}, {1321, 1586, 1318}, {1592, 1317, 1589}, 
	{1318, 1589, 1317}, {1595, 1316, 1592}, {1317, 1592, 1316}, {1598, 1315, 1595}, 
	{1316, 1595, 1315}, {1601, 1313, 1598}, {1315, 1598, 1313}, {1604, 1312, 1601}, 
	{1313, 1601, 1312}, {1607, 1310, 1604}, {1312, 1604, 1310}, {1610, 1311, 1607}, 
	{1310, 1607, 1311}, {1624, 1623, 1622}, {1627, 1626, 1625}, {1288, 1270, 1309}, 
	{1309, 1628, 1288}, {1628, 1309, 1311}, {1288, 1629, 1290}, {1629, 1288, 1628}, 
	{1290, 1630, 1292}, {1630, 1290, 1629}, {1291, 1631, 1295}, {1631, 1291, 1292}, 
	{1292, 1632, 1631}, {1632, 1292, 1630}, {1294, 1295, 1631}, {1631, 1633, 1294}, 
	{1633, 1631, 1632}, {1294, 1634, 1297}, {1634, 1294, 1633}, {1297, 1635, 1299}, 
	{1635, 1297, 1634}, {1298, 1637, 1636}, {1637, 1298, 1299}, {1299, 1638, 1637}, 
	{1638, 1299, 1635}, {1636, 1639, 1302}, {1639, 1636, 1637}, {1637, 1640, 1639}, 
	{1640, 1637, 1638}, {1301, 1302, 1639}, {1639, 1641, 1301}, {1641, 1639, 1640}, 
	{1300, 1643, 1642}, {1643, 1300, 1301}, {1301, 1644, 1643}, {1644, 1301, 1641}, 
	{1642, 1646, 1645}, {1646, 1642, 1643}, {1643, 1647, 1646}, {1647, 1643, 1644}, 
	{1645, 1649, 1648}, {1649, 1645, 1646}, {1646, 1650, 1649}, {1650, 1646, 1647}, 
	{1648, 1652, 1651}, {1652, 1648, 1649}, {1649, 1653, 1652}, {1653, 1649, 1650}, 
	{1651, 1655, 1654}, {1655, 1651, 1652}, {1652, 1656, 1655}, {1656, 1652, 1653}, 
	{1654, 1658, 1657}, {1658, 1654, 1655}, {1655, 1659, 1658}, {1659, 1655, 1656}, 
	{1657, 1661, 1660}, {1661, 1657, 1658}, {1658, 1662, 1661}, {1662, 1658, 1659}, 
	{1660, 1664, 1663}, {1664, 1660, 1661}, {1661, 1665, 1664}, {1665, 1661, 1662}, 
	{1663, 1667, 1666}, {1667, 1663, 1664}, {1664, 1668, 1667}, {1668, 1664, 1665}, 
	{1666, 1670, 1669}, {1670, 1666, 1667}, {1667, 1671, 1670}, {1671, 1667, 1668}, 
	{1669, 1673, 1672}, {1673, 1669, 1670}, {1670, 1674, 1673}, {1674, 1670, 1671}, 
	{1672, 1676, 1675}, {1676, 1672, 1673}, {1673, 1677, 1676}, {1677, 1673, 1674}, 
	{1675, 1679, 1678}, {1679, 1675, 1676}, {1676, 1680, 1679}, {1680, 1676, 1677}, 
	{1678, 1682, 1681}, {1682, 1678, 1679}, {1679, 1683, 1682}, {1683, 1679, 1680}, 
	{1681, 1685, 1684}, {1685, 1681, 1682}, {1682, 1686, 1685}, {1686, 1682, 1683}, 
	{1684, 1688, 1687}, {1688, 1684, 1685}, {1685, 1689, 1688}, {1689, 1685, 1686}, 
	{1687, 1691, 1690}, {1691, 1687, 1688}, {1688, 1692, 1691}, {1692, 1688, 1689}, 
	{1690, 1694, 1693}, {1694, 1690, 1691}, {1691, 1695, 1694}, {1695, 1691, 1692}, 
	{1693, 1697, 1696}, {1697, 1693, 1694}, {1694, 1698, 1697}, {1698, 1694, 1695}, 
	{1696, 1700, 1699}, {1700, 1696, 1697}, {1697, 1701, 1700}, {1701, 1697, 1698}, 
	{1699, 1703, 1702}, {1703, 1699, 1700}, {1700, 1704, 1703}, {1704, 1700, 1701}, 
	{1702, 1706, 1705}, {1706, 1702, 1703}, {1703, 1707, 1706}, {1707, 1703, 1704}, 
	{1705, 1709, 1708}, {1709, 1705, 1706}, {1706, 1710, 1709}, {1710, 1706, 1707}, 
	{1708, 1712, 1711}, {1712, 1708, 1709}, {1709, 1713, 1712}, {1713, 1709, 1710}, 
	{1711, 1715, 1714}, {1715, 1711, 1712}, {1712, 1716, 1715}, {1716, 1712, 1713}, 
	{1714, 1718, 1717}, {1718, 1714, 1715}, {1715, 1719, 1718}, {1719, 1715, 1716}, 
	{1717, 1721, 1720}, {1721, 1717, 1718}, {1718, 1722, 1721}, {1722, 1718, 1719}, 
	{1720, 1724, 1723}, {1724, 1720, 1721}, {1721, 1725, 1724}, {1725, 1721, 1722}, 
	{1723, 1727, 1726}, {1727, 1723, 1724}, {1724, 1728, 1727}, {1728, 1724, 1725}, 
	{1726, 1730, 1729}, {1730, 1726, 1727}, {1727, 1731, 1730}, {1731, 1727, 1728}, 
	{1729, 1733, 1732}, {1733, 1729, 1730}, {1730, 1734, 1733}, {1734, 1730, 1731}, 
	{1732, 1736, 1735}, {1736, 1732, 1733}, {1733, 1737, 1736}, {1737, 1733, 1734}, 
	{1735, 1739, 1738}, {1739, 1735, 1736}, {1736, 1740, 1739}, {1740, 1736, 1737}, 
	{1738, 1742, 1741}, {1742, 1738, 1739}, {1739, 1743, 1742}, {1743, 1739, 1740}, 
	{1741, 1745, 1744}, {1745, 1741, 1742}, {1742, 1746, 1745}, {1746, 1742, 1743}, 
	{1744, 1748, 1747}, {1748, 1744, 1745}, {1745, 1749, 1748}, {1749, 1745, 1746}, 
	{1747, 1751, 1750}, {1751, 1747, 1748}, {1748, 1752, 1751}, {1752, 1748, 1749}, 
	{1750, 1754, 1753}, {1754, 1750, 1751}, {1751, 1755, 1754}, {1755, 1751, 1752}, 
	{1753, 1757, 1756}, {1757, 1753, 1754}, {1754, 1758, 1757}, {1758, 1754, 1755}, 
	{1756, 1760, 1759}, {1760, 1756, 1757}, {1757, 1761, 1760}, {1761, 1757, 1758}, 
	{1759, 1763, 1762}, {1763, 1759, 1760}, {1760, 1764, 1763}, {1764, 1760, 1761}, 
	{1762, 1766, 1765}, {1766, 1762, 1763}, {1763, 1767, 1766}, {1767, 1763, 1764}, 
	{1765, 1769, 1768}, {1769, 1765, 1766}, {1766, 1770, 1769}, {1770, 1766, 1767}, 
	{1768, 1772, 1771}, {1772, 1768, 1769}, {1769, 1773, 1772}, {1773, 1769, 1770}, 
	{1771, 1775, 1774}, {1775, 1771, 1772}, {1772, 1776, 1775}, {1776, 1772, 1773}, 
	{1774, 1778, 1777}, {1778, 1774, 1775}, {1775, 1779, 1778}, {1779, 1775, 1776}, 
	{1777, 1781, 1780}, {1781, 1777, 1778}, {1778, 1782, 1781}, {1782, 1778, 1779}, 
	{1780, 1784, 1783}, {1784, 1780, 1781}, {1781, 1785, 1784}, {1785, 1781, 1782}, 
	{1783, 1787, 1786}, {1787, 1783, 1784}, {1784, 1788, 1787}, {1788, 1784, 1785}, 
	{1786, 1790, 1789}, {1790, 1786, 1787}, {1787, 1791, 1790}, {1791, 1787, 1788}, 
	{1789, 1793, 1792}, {1793, 1789, 1790}, {1790, 1794, 1793}, {1794, 1790, 1791}, 
	{1792, 1796, 1795}, {1796, 1792, 1793}, {1793, 1797, 1796}, {1797, 1793, 1794}, 
	{1795, 1481, 1482}, {1481, 1795, 1796}, {1796, 1483, 1481}, {1483, 1796, 1797}, 
	{1484, 1797, 1798}, {1797, 1484, 1483}, {1713, 1799, 1716}, {1799, 1713, 1800}, 
	{1800, 1801, 1799}, {1801, 1800, 1802}, {1800, 1710, 1803}, {1710, 1800, 1713}, 
	{1802, 1803, 1804}, {1803, 1802, 1800}, {1716, 1805, 1719}, {1805, 1716, 1799}, 
	{1799, 1806, 1805}, {1806, 1799, 1801}, {1719, 1807, 1722}, {1807, 1719, 1805}, 
	{1805, 1808, 1807}, {1808, 1805, 1806}, {1722, 1809, 1725}, {1809, 1722, 1807}, 
	{1807, 1810, 1809}, {1810, 1807, 1808}, {1725, 1811, 1728}, {1811, 1725, 1809}, 
	{1809, 1812, 1811}, {1812, 1809, 1810}, {1728, 1813, 1731}, {1813, 1728, 1811}, 
	{1811, 1814, 1813}, {1814, 1811, 1812}, {1731, 1815, 1734}, {1815, 1731, 1813}, 
	{1813, 1816, 1815}, {1816, 1813, 1814}, {1734, 1817, 1737}, {1817, 1734, 1815}, 
	{1815, 1818, 1817}, {1818, 1815, 1816}, {1737, 1819, 1740}, {1819, 1737, 1817}, 
	{1817, 1820, 1819}, {1820, 1817, 1818}, {1740, 1821, 1743}, {1821, 1740, 1819}, 
	{1819, 1822, 1821}, {1822, 1819, 1820}, {1743, 1823, 1746}, {1823, 1743, 1821}, 
	{1821, 1824, 1823}, {1824, 1821, 1822}, {1746, 1825, 1749}, {1825, 1746, 1823}, 
	{1823, 1826, 1825}, {1826, 1823, 1824}, {1749, 1827, 1752}, {1827, 1749, 1825}, 
	{1825, 1828, 1827}, {1828, 1825, 1826}, {1752, 1829, 1755}, {1829, 1752, 1827}, 
	{1827, 1830, 1829}, {1830, 1827, 1828}, {1755, 1831, 1758}, {1831, 1755, 1829}, 
	{1829, 1832, 1831}, {1832, 1829, 1830}, {1758, 1624, 1761}, {1624, 1758, 1831}, 
	{1831, 1623, 1624}, {1623, 1831, 1832}, {1761, 1622, 1764}, {1622, 1761, 1624}, 
	{1622, 1767, 1764}, {1767, 1622, 1833}, {1622, 1834, 1833}, {1834, 1622, 1835}, 
	{1835, 1623, 1836}, {1623, 1835, 1622}, {1626, 1837, 1625}, {1837, 1626, 1838}, 
	{1626, 1698, 1838}, {1698, 1626, 1701}, {1626, 1704, 1701}, {1704, 1626, 1627}, 
	{1627, 1840, 1839}, {1840, 1627, 1625}, {1625, 1841, 1840}, {1841, 1625, 1837}, 
	{1839, 1804, 1803}, {1804, 1839, 1840}, {1840, 1802, 1804}, {1802, 1840, 1841}, 
	{1803, 1707, 1839}, {1707, 1803, 1710}, {1627, 1707, 1704}, {1707, 1627, 1839}, 
	{1837, 1843, 1842}, {1843, 1837, 1838}, {1841, 1842, 1844}, {1842, 1841, 1837}, 
	{1842, 1846, 1845}, {1846, 1842, 1843}, {1844, 1845, 1847}, {1845, 1844, 1842}, 
	{1845, 1849, 1848}, {1849, 1845, 1846}, {1847, 1848, 1850}, {1848, 1847, 1845}, 
	{1848, 1852, 1851}, {1852, 1848, 1849}, {1850, 1851, 1853}, {1851, 1850, 1848}, 
	{1851, 1855, 1854}, {1855, 1851, 1852}, {1853, 1854, 1856}, {1854, 1853, 1851}, 
	{1854, 1858, 1857}, {1858, 1854, 1855}, {1856, 1857, 1859}, {1857, 1856, 1854}, 
	{1857, 1861, 1860}, {1861, 1857, 1858}, {1859, 1860, 1862}, {1860, 1859, 1857}, 
	{1860, 1864, 1863}, {1864, 1860, 1861}, {1862, 1863, 1865}, {1863, 1862, 1860}, 
	{1863, 1867, 1866}, {1867, 1863, 1864}, {1865, 1866, 1868}, {1866, 1865, 1863}, 
	{1866, 1870, 1869}, {1870, 1866, 1867}, {1868, 1869, 1871}, {1869, 1868, 1866}, 
	{1869, 1873, 1872}, {1873, 1869, 1870}, {1871, 1872, 1874}, {1872, 1871, 1869}, 
	{1872, 1876, 1875}, {1876, 1872, 1873}, {1874, 1875, 1877}, {1875, 1874, 1872}, 
	{1875, 1879, 1878}, {1879, 1875, 1876}, {1877, 1878, 1880}, {1878, 1877, 1875}, 
	{1878, 1882, 1881}, {1882, 1878, 1879}, {1880, 1881, 1883}, {1881, 1880, 1878}, 
	{1881, 1885, 1884}, {1885, 1881, 1882}, {1883, 1884, 1886}, {1884, 1883, 1881}, 
	{1884, 1888, 1887}, {1888, 1884, 1885}, {1886, 1887, 1836}, {1887, 1886, 1884}, 
	{1887, 1890, 1889}, {1890, 1887, 1888}, {1836, 1889, 1835}, {1889, 1836, 1887}, 
	{1889, 1892, 1891}, {1892, 1889, 1890}, {1835, 1891, 1834}, {1891, 1835, 1889}, 
	{1891, 1894, 1893}, {1894, 1891, 1892}, {1834, 1893, 1895}, {1893, 1834, 1891}, 
	{1893, 1897, 1896}, {1897, 1893, 1894}, {1895, 1896, 1898}, {1896, 1895, 1893}, 
	{1896, 1900, 1899}, {1900, 1896, 1897}, {1898, 1899, 1901}, {1899, 1898, 1896}, 
	{1899, 1903, 1902}, {1903, 1899, 1900}, {1901, 1902, 1904}, {1902, 1901, 1899}, 
	{1902, 1906, 1905}, {1906, 1902, 1903}, {1904, 1905, 1907}, {1905, 1904, 1902}, 
	{1905, 1909, 1908}, {1909, 1905, 1906}, {1907, 1908, 1910}, {1908, 1907, 1905}, 
	{1908, 1912, 1911}, {1912, 1908, 1909}, {1910, 1911, 1913}, {1911, 1910, 1908}, 
	{1911, 1915, 1914}, {1915, 1911, 1912}, {1913, 1914, 1916}, {1914, 1913, 1911}, 
	{1914, 1918, 1917}, {1918, 1914, 1915}, {1916, 1917, 1919}, {1917, 1916, 1914}, 
	{1917, 1921, 1920}, {1921, 1917, 1918}, {1919, 1920, 1922}, {1920, 1919, 1917}, 
	{1920, 1610, 1611}, {1610, 1920, 1921}, {1922, 1611, 1612}, {1611, 1922, 1920}, 
	{1802, 1844, 1801}, {1844, 1802, 1841}, {1484, 1922, 1612}, {1922, 1484, 1798}, 
	{1833, 1770, 1767}, {1770, 1833, 1923}, {1833, 1895, 1923}, {1895, 1833, 1834}, 
	{1923, 1773, 1770}, {1773, 1923, 1924}, {1923, 1898, 1924}, {1898, 1923, 1895}, 
	{1924, 1776, 1773}, {1776, 1924, 1925}, {1924, 1901, 1925}, {1901, 1924, 1898}, 
	{1925, 1779, 1776}, {1779, 1925, 1926}, {1925, 1904, 1926}, {1904, 1925, 1901}, 
	{1926, 1782, 1779}, {1782, 1926, 1927}, {1926, 1907, 1927}, {1907, 1926, 1904}, 
	{1927, 1785, 1782}, {1785, 1927, 1928}, {1927, 1910, 1928}, {1910, 1927, 1907}, 
	{1928, 1788, 1785}, {1788, 1928, 1929}, {1928, 1913, 1929}, {1913, 1928, 1910}, 
	{1929, 1791, 1788}, {1791, 1929, 1930}, {1929, 1916, 1930}, {1916, 1929, 1913}, 
	{1930, 1794, 1791}, {1794, 1930, 1931}, {1930, 1919, 1931}, {1919, 1930, 1916}, 
	{1931, 1797, 1794}, {1797, 1931, 1798}, {1931, 1922, 1798}, {1922, 1931, 1919}, 
	{1801, 1847, 1806}, {1847, 1801, 1844}, {1806, 1850, 1808}, {1850, 1806, 1847}, 
	{1808, 1853, 1810}, {1853, 1808, 1850}, {1810, 1856, 1812}, {1856, 1810, 1853}, 
	{1812, 1859, 1814}, {1859, 1812, 1856}, {1814, 1862, 1816}, {1862, 1814, 1859}, 
	{1816, 1865, 1818}, {1865, 1816, 1862}, {1818, 1868, 1820}, {1868, 1818, 1865}, 
	{1820, 1871, 1822}, {1871, 1820, 1868}, {1822, 1874, 1824}, {1874, 1822, 1871}, 
	{1824, 1877, 1826}, {1877, 1824, 1874}, {1826, 1880, 1828}, {1880, 1826, 1877}, 
	{1828, 1883, 1830}, {1883, 1828, 1880}, {1830, 1886, 1832}, {1886, 1830, 1883}, 
	{1832, 1836, 1623}, {1836, 1832, 1886}, {1838, 1695, 1843}, {1695, 1838, 1698}, 
	{1843, 1692, 1846}, {1692, 1843, 1695}, {1846, 1689, 1849}, {1689, 1846, 1692}, 
	{1849, 1686, 1852}, {1686, 1849, 1689}, {1852, 1683, 1855}, {1683, 1852, 1686}, 
	{1855, 1680, 1858}, {1680, 1855, 1683}, {1858, 1677, 1861}, {1677, 1858, 1680}, 
	{1861, 1674, 1864}, {1674, 1861, 1677}, {1864, 1671, 1867}, {1671, 1864, 1674}, 
	{1867, 1668, 1870}, {1668, 1867, 1671}, {1870, 1665, 1873}, {1665, 1870, 1668}, 
	{1873, 1662, 1876}, {1662, 1873, 1665}, {1876, 1659, 1879}, {1659, 1876, 1662}, 
	{1879, 1656, 1882}, {1656, 1879, 1659}, {1882, 1653, 1885}, {1653, 1882, 1656}, 
	{1885, 1650, 1888}, {1650, 1885, 1653}, {1888, 1647, 1890}, {1647, 1888, 1650}, 
	{1890, 1644, 1892}, {1644, 1890, 1647}, {1892, 1641, 1894}, {1641, 1892, 1644}, 
	{1894, 1640, 1897}, {1640, 1894, 1641}, {1897, 1638, 1900}, {1638, 1897, 1640}, 
	{1900, 1635, 1903}, {1635, 1900, 1638}, {1903, 1634, 1906}, {1634, 1903, 1635}, 
	{1906, 1633, 1909}, {1633, 1906, 1634}, {1909, 1632, 1912}, {1632, 1909, 1633}, 
	{1912, 1630, 1915}, {1630, 1912, 1632}, {1915, 1629, 1918}, {1629, 1915, 1630}, 
	{1918, 1628, 1921}, {1628, 1918, 1629}, {1921, 1311, 1610}, {1311, 1921, 1628}
};


GLint sup_gum_indices[1016][3] = {
	{1934, 1933, 1932}, {1937, 1936, 1935}, {1940, 1939, 1938}, {1940, 1938, 1941}, 
	{1944, 1943, 1942}, {1946, 1945, 1940}, {1949, 1948, 1947}, {1952, 1951, 1950}, 
	{1955, 1954, 1953}, {1948, 1956, 1945}, {1959, 1958, 1957}, {1933, 1957, 1958}, 
	{1934, 1960, 1933}, {1957, 1933, 1960}, {1962, 1961, 1959}, {1958, 1959, 1961}, 
	{1958, 1963, 1933}, {1932, 1933, 1963}, {1961, 1964, 1958}, {1963, 1958, 1964}, 
	{1963, 1965, 1932}, {1966, 1932, 1965}, {1967, 1934, 1966}, {1932, 1966, 1934}, 
	{1964, 1968, 1963}, {1965, 1963, 1968}, {1965, 1969, 1966}, {1936, 1966, 1969}, 
	{1937, 1967, 1936}, {1966, 1936, 1967}, {1968, 1970, 1965}, {1969, 1965, 1970}, 
	{1969, 1971, 1936}, {1935, 1936, 1971}, {1970, 1972, 1969}, {1971, 1969, 1972}, 
	{1971, 1973, 1935}, {1974, 1935, 1973}, {1975, 1937, 1974}, {1935, 1974, 1937}, 
	{1972, 1976, 1971}, {1973, 1971, 1976}, {1973, 1977, 1974}, {1939, 1974, 1977}, 
	{1940, 1975, 1939}, {1974, 1939, 1975}, {1976, 1978, 1973}, {1977, 1973, 1978}, 
	{1977, 1979, 1939}, {1938, 1939, 1979}, {1978, 1980, 1977}, {1979, 1977, 1980}, 
	{1979, 1981, 1938}, {1941, 1938, 1981}, {1980, 1982, 1979}, {1981, 1979, 1982}, 
	{1981, 1983, 1941}, {1984, 1941, 1983}, {1946, 1940, 1984}, {1941, 1984, 1940}, 
	{1982, 1985, 1981}, {1983, 1981, 1985}, {1983, 1986, 1984}, {1987, 1984, 1986}, 
	{1947, 1946, 1987}, {1984, 1987, 1946}, {1985, 1988, 1983}, {1986, 1983, 1988}, 
	{1986, 1989, 1987}, {1990, 1987, 1989}, {1949, 1947, 1990}, {1987, 1990, 1947}, 
	{1988, 1991, 1986}, {1989, 1986, 1991}, {1989, 1992, 1990}, {1993, 1990, 1992}, 
	{1994, 1949, 1993}, {1990, 1993, 1949}, {1991, 1995, 1989}, {1992, 1989, 1995}, 
	{1992, 1996, 1993}, {1997, 1993, 1996}, {1998, 1994, 1997}, {1993, 1997, 1994}, 
	{1995, 1999, 1992}, {1996, 1992, 1999}, {1996, 2000, 1997}, {2001, 1997, 2000}, 
	{2002, 1998, 2001}, {1997, 2001, 1998}, {1999, 2003, 1996}, {2000, 1996, 2003}, 
	{2000, 2004, 2001}, {2005, 2001, 2004}, {2006, 2002, 2005}, {2001, 2005, 2002}, 
	{2003, 2007, 2000}, {2004, 2000, 2007}, {2004, 2008, 2005}, {1943, 2005, 2008}, 
	{1944, 2006, 1943}, {2005, 1943, 2006}, {2007, 2009, 2004}, {2008, 2004, 2009}, 
	{2008, 2010, 1943}, {1942, 1943, 2010}, {2009, 2011, 2008}, {2010, 2008, 2011}, 
	{2010, 2012, 1942}, {2013, 1942, 2012}, {2014, 1944, 2013}, {1942, 2013, 1944}, 
	{2011, 2015, 2010}, {2012, 2010, 2015}, {2012, 2016, 2013}, {2017, 2013, 2016}, 
	{2018, 2014, 2017}, {2013, 2017, 2014}, {2015, 2019, 2012}, {2016, 2012, 2019}, 
	{2016, 2020, 2017}, {2021, 2017, 2020}, {2022, 2018, 2021}, {2017, 2021, 2018}, 
	{2019, 2023, 2016}, {2020, 2016, 2023}, {2020, 2024, 2021}, {2025, 2021, 2024}, 
	{2026, 2022, 2025}, {2021, 2025, 2022}, {2023, 2027, 2020}, {2024, 2020, 2027}, 
	{2024, 2028, 2025}, {2029, 2025, 2028}, {2030, 2026, 2029}, {2025, 2029, 2026}, 
	{2027, 2031, 2024}, {2028, 2024, 2031}, {2028, 2032, 2029}, {2033, 2029, 2032}, 
	{2034, 2030, 2033}, {2029, 2033, 2030}, {2031, 2035, 2028}, {2032, 2028, 2035}, 
	{2032, 2036, 2033}, {2037, 2033, 2036}, {2038, 2034, 2037}, {2033, 2037, 2034}, 
	{2035, 2039, 2032}, {2036, 2032, 2039}, {2036, 2040, 2037}, {2041, 2037, 2040}, 
	{2042, 2038, 2041}, {2037, 2041, 2038}, {2039, 2043, 2036}, {2040, 2036, 2043}, 
	{2040, 2044, 2041}, {2045, 2041, 2044}, {2046, 2042, 2045}, {2041, 2045, 2042}, 
	{2043, 2047, 2040}, {2044, 2040, 2047}, {2044, 2048, 2045}, {2049, 2045, 2048}, 
	{2050, 2046, 2049}, {2045, 2049, 2046}, {2047, 2051, 2044}, {2048, 2044, 2051}, 
	{2048, 2052, 2049}, {2053, 2049, 2052}, {2054, 2050, 2053}, {2049, 2053, 2050}, 
	{2051, 2055, 2048}, {2052, 2048, 2055}, {2052, 2056, 2053}, {2057, 2053, 2056}, 
	{2058, 2054, 2057}, {2053, 2057, 2054}, {2055, 2059, 2052}, {2056, 2052, 2059}, 
	{2056, 2060, 2057}, {2061, 2057, 2060}, {2062, 2058, 2061}, {2057, 2061, 2058}, 
	{2059, 2063, 2056}, {2060, 2056, 2063}, {2060, 2064, 2061}, {2065, 2061, 2064}, 
	{2066, 2062, 2065}, {2061, 2065, 2062}, {2063, 2067, 2060}, {2064, 2060, 2067}, 
	{2064, 2068, 2065}, {2069, 2065, 2068}, {2070, 2066, 2069}, {2065, 2069, 2066}, 
	{2067, 2071, 2064}, {2068, 2064, 2071}, {2068, 2072, 2069}, {2073, 2069, 2072}, 
	{2074, 2070, 2073}, {2069, 2073, 2070}, {2071, 2075, 2068}, {2072, 2068, 2075}, 
	{2072, 2076, 2073}, {2077, 2073, 2076}, {2078, 2074, 2077}, {2073, 2077, 2074}, 
	{2075, 2079, 2072}, {2076, 2072, 2079}, {2076, 2080, 2077}, {2081, 2077, 2080}, 
	{2082, 2078, 2081}, {2077, 2081, 2078}, {2079, 2083, 2076}, {2080, 2076, 2083}, 
	{2080, 2084, 2081}, {2085, 2081, 2084}, {2086, 2082, 2085}, {2081, 2085, 2082}, 
	{2083, 2087, 2080}, {2084, 2080, 2087}, {2084, 2088, 2085}, {2089, 2085, 2088}, 
	{2090, 2086, 2089}, {2085, 2089, 2086}, {2087, 2091, 2084}, {2088, 2084, 2091}, 
	{2088, 2092, 2089}, {2093, 2089, 2092}, {2094, 2090, 2093}, {2089, 2093, 2090}, 
	{2091, 2095, 2088}, {2092, 2088, 2095}, {2092, 2096, 2093}, {2097, 2093, 2096}, 
	{2098, 2094, 2097}, {2093, 2097, 2094}, {2095, 2099, 2092}, {2096, 2092, 2099}, 
	{2096, 2100, 2097}, {2101, 2097, 2100}, {2102, 2098, 2101}, {2097, 2101, 2098}, 
	{2099, 2103, 2096}, {2100, 2096, 2103}, {2100, 2104, 2101}, {2105, 2101, 2104}, 
	{2106, 2102, 2105}, {2101, 2105, 2102}, {2103, 2107, 2100}, {2104, 2100, 2107}, 
	{2104, 2108, 2105}, {2109, 2105, 2108}, {2110, 2106, 2109}, {2105, 2109, 2106}, 
	{2107, 2111, 2104}, {2108, 2104, 2111}, {2108, 2112, 2109}, {2113, 2109, 2112}, 
	{2114, 2110, 2113}, {2109, 2113, 2110}, {2111, 2115, 2108}, {2112, 2108, 2115}, 
	{2112, 2116, 2113}, {2117, 2113, 2116}, {2118, 2114, 2117}, {2113, 2117, 2114}, 
	{2115, 2119, 2112}, {2116, 2112, 2119}, {2116, 2120, 2117}, {2121, 2117, 2120}, 
	{2122, 2118, 2121}, {2117, 2121, 2118}, {2119, 2123, 2116}, {2120, 2116, 2123}, 
	{2120, 2124, 2121}, {2125, 2121, 2124}, {2126, 2122, 2125}, {2121, 2125, 2122}, 
	{2123, 2127, 2120}, {2124, 2120, 2127}, {2124, 2128, 2125}, {2129, 2125, 2128}, 
	{2130, 2126, 2129}, {2125, 2129, 2126}, {2127, 2131, 2124}, {2128, 2124, 2131}, 
	{2128, 2132, 2129}, {2133, 2129, 2132}, {2134, 2130, 2133}, {2129, 2133, 2130}, 
	{2131, 2135, 2128}, {2132, 2128, 2135}, {2132, 2136, 2133}, {2137, 2133, 2136}, 
	{2138, 2134, 2137}, {2133, 2137, 2134}, {2135, 2139, 2132}, {2136, 2132, 2139}, 
	{2136, 2140, 2137}, {2141, 2137, 2140}, {2142, 2138, 2141}, {2137, 2141, 2138}, 
	{2139, 2143, 2136}, {2140, 2136, 2143}, {2140, 2144, 2141}, {2145, 2141, 2144}, 
	{2146, 2142, 2145}, {2141, 2145, 2142}, {2143, 2147, 2140}, {2144, 2140, 2147}, 
	{2144, 2148, 2145}, {2149, 2145, 2148}, {2150, 2146, 2149}, {2145, 2149, 2146}, 
	{2147, 2151, 2144}, {2148, 2144, 2151}, {2148, 2152, 2149}, {2153, 2149, 2152}, 
	{2154, 2150, 2153}, {2149, 2153, 2150}, {2151, 2155, 2148}, {2152, 2148, 2155}, 
	{2152, 2156, 2153}, {2157, 2153, 2156}, {2158, 2154, 2157}, {2153, 2157, 2154}, 
	{2155, 2159, 2152}, {2156, 2152, 2159}, {2156, 2160, 2157}, {2161, 2157, 2160}, 
	{2162, 2158, 2161}, {2157, 2161, 2158}, {2159, 2163, 2156}, {2160, 2156, 2163}, 
	{2160, 2164, 2161}, {2165, 2161, 2164}, {2166, 2162, 2165}, {2161, 2165, 2162}, 
	{2163, 2167, 2160}, {2164, 2160, 2167}, {2164, 2168, 2165}, {2169, 2165, 2168}, 
	{2170, 2166, 2169}, {2165, 2169, 2166}, {2167, 2171, 2164}, {2168, 2164, 2171}, 
	{2168, 2172, 2169}, {2173, 2169, 2172}, {2174, 2170, 2173}, {2169, 2173, 2170}, 
	{2171, 2175, 2168}, {2172, 2168, 2175}, {2172, 2176, 2173}, {2177, 2173, 2176}, 
	{2178, 2174, 2177}, {2173, 2177, 2174}, {2175, 2179, 2172}, {2176, 2172, 2179}, 
	{1934, 1950, 1960}, {2180, 1960, 1950}, {1967, 1952, 1934}, {1950, 1934, 1952}, 
	{1937, 1953, 1967}, {1952, 1967, 1953}, {1975, 1955, 1937}, {1953, 1937, 1955}, 
	{1940, 1945, 1975}, {1955, 1975, 1945}, {1947, 1948, 1946}, {1945, 1946, 1948}, 
	{1994, 2181, 1949}, {1948, 1949, 2181}, {1998, 2182, 1994}, {2181, 1994, 2182}, 
	{2002, 2183, 1998}, {2182, 1998, 2183}, {2006, 2184, 2002}, {2183, 2002, 2184}, 
	{1944, 2185, 2006}, {2184, 2006, 2185}, {2014, 2186, 1944}, {2185, 1944, 2186}, 
	{2018, 2187, 2014}, {2186, 2014, 2187}, {2022, 2188, 2018}, {2187, 2018, 2188}, 
	{2026, 2189, 2022}, {2188, 2022, 2189}, {2030, 2190, 2026}, {2189, 2026, 2190}, 
	{2034, 2191, 2030}, {2190, 2030, 2191}, {2038, 2192, 2034}, {2191, 2034, 2192}, 
	{2042, 2193, 2038}, {2192, 2038, 2193}, {2046, 2194, 2042}, {2193, 2042, 2194}, 
	{2050, 2195, 2046}, {2194, 2046, 2195}, {2054, 2196, 2050}, {2195, 2050, 2196}, 
	{1950, 1951, 2180}, {2197, 2180, 1951}, {1953, 1954, 1952}, {1951, 1952, 1954}, 
	{1945, 1956, 1955}, {1954, 1955, 1956}, {2181, 2198, 1948}, {1956, 1948, 2198}, 
	{2182, 2199, 2181}, {2198, 2181, 2199}, {2183, 2200, 2182}, {2199, 2182, 2200}, 
	{2184, 2201, 2183}, {2200, 2183, 2201}, {2185, 2202, 2184}, {2201, 2184, 2202}, 
	{2186, 2203, 2185}, {2202, 2185, 2203}, {2187, 2204, 2186}, {2203, 2186, 2204}, 
	{2188, 2205, 2187}, {2204, 2187, 2205}, {2189, 2206, 2188}, {2205, 2188, 2206}, 
	{2190, 2207, 2189}, {2206, 2189, 2207}, {2191, 2208, 2190}, {2207, 2190, 2208}, 
	{2192, 2209, 2191}, {2208, 2191, 2209}, {2193, 2210, 2192}, {2209, 2192, 2210}, 
	{2194, 2211, 2193}, {2210, 2193, 2211}, {2195, 2212, 2194}, {2211, 2194, 2212}, 
	{2196, 2213, 2195}, {2212, 2195, 2213}, {2215, 2214, 2196}, {2213, 2196, 2214}, 
	{2217, 2216, 2215}, {2214, 2215, 2216}, {2058, 2215, 2054}, {2196, 2054, 2215}, 
	{2062, 2217, 2058}, {2215, 2058, 2217}, {2066, 2218, 2062}, {2217, 2062, 2218}, 
	{2220, 2174, 2219}, {2178, 2219, 2174}, {2221, 2170, 2220}, {2174, 2220, 2170}, 
	{2222, 2166, 2221}, {2170, 2221, 2166}, {2223, 2162, 2222}, {2166, 2222, 2162}, 
	{2224, 2158, 2223}, {2162, 2223, 2158}, {2225, 2154, 2224}, {2158, 2224, 2154}, 
	{2226, 2150, 2225}, {2154, 2225, 2150}, {2227, 2146, 2226}, {2150, 2226, 2146}, 
	{2228, 2142, 2227}, {2146, 2227, 2142}, {2229, 2138, 2228}, {2142, 2228, 2138}, 
	{2230, 2134, 2229}, {2138, 2229, 2134}, {2231, 2130, 2230}, {2134, 2230, 2130}, 
	{2232, 2126, 2231}, {2130, 2231, 2126}, {2233, 2122, 2232}, {2126, 2232, 2122}, 
	{2234, 2118, 2233}, {2122, 2233, 2118}, {2235, 2114, 2234}, {2118, 2234, 2114}, 
	{2236, 2110, 2235}, {2114, 2235, 2110}, {2237, 2106, 2236}, {2110, 2236, 2106}, 
	{2238, 2102, 2237}, {2106, 2237, 2102}, {2239, 2098, 2238}, {2102, 2238, 2098}, 
	{2240, 2094, 2239}, {2098, 2239, 2094}, {2241, 2090, 2240}, {2094, 2240, 2090}, 
	{2242, 2086, 2241}, {2090, 2241, 2086}, {2243, 2082, 2242}, {2086, 2242, 2082}, 
	{2244, 2078, 2243}, {2082, 2243, 2078}, {2245, 2074, 2244}, {2078, 2244, 2074}, 
	{2246, 2070, 2245}, {2074, 2245, 2070}, {2218, 2066, 2246}, {2070, 2246, 2066}, 
	{2249, 2248, 2247}, {2252, 2251, 2250}, {2255, 2254, 2253}, {2256, 2255, 2253}, 
	{2259, 2258, 2257}, {2253, 2261, 2260}, {2264, 2263, 2262}, {2267, 2266, 2265}, 
	{2270, 2269, 2268}, {2261, 2271, 2263}, {1957, 2272, 1959}, {2272, 1957, 2248}, 
	{2248, 1960, 2247}, {1960, 2248, 1957}, {1959, 2273, 1962}, {2273, 1959, 2272}, 
	{2248, 2274, 2272}, {2274, 2248, 2249}, {2272, 2275, 2273}, {2275, 2272, 2274}, 
	{2249, 2276, 2274}, {2276, 2249, 2277}, {2277, 2247, 2278}, {2247, 2277, 2249}, 
	{2274, 2279, 2275}, {2279, 2274, 2276}, {2277, 2280, 2276}, {2280, 2277, 2251}, 
	{2251, 2278, 2250}, {2278, 2251, 2277}, {2276, 2281, 2279}, {2281, 2276, 2280}, 
	{2251, 2282, 2280}, {2282, 2251, 2252}, {2280, 2283, 2281}, {2283, 2280, 2282}, 
	{2252, 2284, 2282}, {2284, 2252, 2285}, {2285, 2250, 2286}, {2250, 2285, 2252}, 
	{2282, 2287, 2283}, {2287, 2282, 2284}, {2285, 2288, 2284}, {2288, 2285, 2254}, 
	{2254, 2286, 2253}, {2286, 2254, 2285}, {2284, 2289, 2287}, {2289, 2284, 2288}, 
	{2254, 2290, 2288}, {2290, 2254, 2255}, {2288, 2291, 2289}, {2291, 2288, 2290}, 
	{2255, 2292, 2290}, {2292, 2255, 2256}, {2290, 2293, 2291}, {2293, 2290, 2292}, 
	{2256, 2294, 2292}, {2294, 2256, 2295}, {2295, 2253, 2260}, {2253, 2295, 2256}, 
	{2292, 2296, 2293}, {2296, 2292, 2294}, {2295, 2297, 2294}, {2297, 2295, 2298}, 
	{2298, 2260, 2264}, {2260, 2298, 2295}, {2294, 2299, 2296}, {2299, 2294, 2297}, 
	{2298, 2300, 2297}, {2300, 2298, 2301}, {2301, 2264, 2262}, {2264, 2301, 2298}, 
	{2297, 2302, 2299}, {2302, 2297, 2300}, {2301, 2303, 2300}, {2303, 2301, 2304}, 
	{2304, 2262, 2305}, {2262, 2304, 2301}, {2300, 2306, 2302}, {2306, 2300, 2303}, 
	{2304, 2307, 2303}, {2307, 2304, 2308}, {2308, 2305, 2309}, {2305, 2308, 2304}, 
	{2303, 2310, 2306}, {2310, 2303, 2307}, {2308, 2311, 2307}, {2311, 2308, 2312}, 
	{2312, 2309, 2313}, {2309, 2312, 2308}, {2307, 2314, 2310}, {2314, 2307, 2311}, 
	{2312, 2315, 2311}, {2315, 2312, 2316}, {2316, 2313, 2317}, {2313, 2316, 2312}, 
	{2311, 2318, 2314}, {2318, 2311, 2315}, {2316, 2319, 2315}, {2319, 2316, 2258}, 
	{2258, 2317, 2257}, {2317, 2258, 2316}, {2315, 2320, 2318}, {2320, 2315, 2319}, 
	{2258, 2321, 2319}, {2321, 2258, 2259}, {2319, 2322, 2320}, {2322, 2319, 2321}, 
	{2259, 2323, 2321}, {2323, 2259, 2324}, {2324, 2257, 2325}, {2257, 2324, 2259}, 
	{2321, 2326, 2322}, {2326, 2321, 2323}, {2324, 2327, 2323}, {2327, 2324, 2328}, 
	{2328, 2325, 2329}, {2325, 2328, 2324}, {2323, 2330, 2326}, {2330, 2323, 2327}, 
	{2328, 2331, 2327}, {2331, 2328, 2332}, {2332, 2329, 2333}, {2329, 2332, 2328}, 
	{2327, 2334, 2330}, {2334, 2327, 2331}, {2332, 2335, 2331}, {2335, 2332, 2336}, 
	{2336, 2333, 2337}, {2333, 2336, 2332}, {2331, 2338, 2334}, {2338, 2331, 2335}, 
	{2336, 2339, 2335}, {2339, 2336, 2340}, {2340, 2337, 2341}, {2337, 2340, 2336}, 
	{2335, 2342, 2338}, {2342, 2335, 2339}, {2340, 2343, 2339}, {2343, 2340, 2344}, 
	{2344, 2341, 2345}, {2341, 2344, 2340}, {2339, 2346, 2342}, {2346, 2339, 2343}, 
	{2344, 2347, 2343}, {2347, 2344, 2348}, {2348, 2345, 2349}, {2345, 2348, 2344}, 
	{2343, 2350, 2346}, {2350, 2343, 2347}, {2348, 2351, 2347}, {2351, 2348, 2352}, 
	{2352, 2349, 2353}, {2349, 2352, 2348}, {2347, 2354, 2350}, {2354, 2347, 2351}, 
	{2352, 2355, 2351}, {2355, 2352, 2356}, {2356, 2353, 2357}, {2353, 2356, 2352}, 
	{2351, 2358, 2354}, {2358, 2351, 2355}, {2356, 2359, 2355}, {2359, 2356, 2360}, 
	{2360, 2357, 2361}, {2357, 2360, 2356}, {2355, 2362, 2358}, {2362, 2355, 2359}, 
	{2360, 2363, 2359}, {2363, 2360, 2364}, {2364, 2361, 2365}, {2361, 2364, 2360}, 
	{2359, 2366, 2362}, {2366, 2359, 2363}, {2364, 2367, 2363}, {2367, 2364, 2368}, 
	{2368, 2365, 2369}, {2365, 2368, 2364}, {2363, 2370, 2366}, {2370, 2363, 2367}, 
	{2368, 2371, 2367}, {2371, 2368, 2372}, {2372, 2369, 2373}, {2369, 2372, 2368}, 
	{2367, 2374, 2370}, {2374, 2367, 2371}, {2372, 2375, 2371}, {2375, 2372, 2376}, 
	{2376, 2373, 2377}, {2373, 2376, 2372}, {2371, 2378, 2374}, {2378, 2371, 2375}, 
	{2376, 2379, 2375}, {2379, 2376, 2380}, {2380, 2377, 2381}, {2377, 2380, 2376}, 
	{2375, 2382, 2378}, {2382, 2375, 2379}, {2380, 2383, 2379}, {2383, 2380, 2384}, 
	{2384, 2381, 2385}, {2381, 2384, 2380}, {2379, 2386, 2382}, {2386, 2379, 2383}, 
	{2384, 2387, 2383}, {2387, 2384, 2388}, {2388, 2385, 2389}, {2385, 2388, 2384}, 
	{2383, 2390, 2386}, {2390, 2383, 2387}, {2388, 2391, 2387}, {2391, 2388, 2392}, 
	{2392, 2389, 2393}, {2389, 2392, 2388}, {2387, 2394, 2390}, {2394, 2387, 2391}, 
	{2392, 2395, 2391}, {2395, 2392, 2396}, {2396, 2393, 2397}, {2393, 2396, 2392}, 
	{2391, 2398, 2394}, {2398, 2391, 2395}, {2396, 2399, 2395}, {2399, 2396, 2400}, 
	{2400, 2397, 2401}, {2397, 2400, 2396}, {2395, 2402, 2398}, {2402, 2395, 2399}, 
	{2400, 2403, 2399}, {2403, 2400, 2404}, {2404, 2401, 2405}, {2401, 2404, 2400}, 
	{2399, 2406, 2402}, {2406, 2399, 2403}, {2404, 2407, 2403}, {2407, 2404, 2408}, 
	{2408, 2405, 2409}, {2405, 2408, 2404}, {2403, 2410, 2406}, {2410, 2403, 2407}, 
	{2408, 2411, 2407}, {2411, 2408, 2412}, {2412, 2409, 2413}, {2409, 2412, 2408}, 
	{2407, 2414, 2410}, {2414, 2407, 2411}, {2412, 2415, 2411}, {2415, 2412, 2416}, 
	{2416, 2413, 2417}, {2413, 2416, 2412}, {2411, 2418, 2414}, {2418, 2411, 2415}, 
	{2416, 2419, 2415}, {2419, 2416, 2420}, {2420, 2417, 2421}, {2417, 2420, 2416}, 
	{2415, 2422, 2418}, {2422, 2415, 2419}, {2420, 2423, 2419}, {2423, 2420, 2424}, 
	{2424, 2421, 2425}, {2421, 2424, 2420}, {2419, 2426, 2422}, {2426, 2419, 2423}, 
	{2424, 2427, 2423}, {2427, 2424, 2428}, {2428, 2425, 2429}, {2425, 2428, 2424}, 
	{2423, 2430, 2426}, {2430, 2423, 2427}, {2428, 2431, 2427}, {2431, 2428, 2432}, 
	{2432, 2429, 2433}, {2429, 2432, 2428}, {2427, 2434, 2430}, {2434, 2427, 2431}, 
	{2432, 2435, 2431}, {2435, 2432, 2436}, {2436, 2433, 2437}, {2433, 2436, 2432}, 
	{2431, 2438, 2434}, {2438, 2431, 2435}, {2436, 2439, 2435}, {2439, 2436, 2440}, 
	{2440, 2437, 2441}, {2437, 2440, 2436}, {2435, 2442, 2438}, {2442, 2435, 2439}, 
	{2440, 2443, 2439}, {2443, 2440, 2444}, {2444, 2441, 2445}, {2441, 2444, 2440}, 
	{2439, 2446, 2442}, {2446, 2439, 2443}, {2444, 2447, 2443}, {2447, 2444, 2448}, 
	{2448, 2445, 2449}, {2445, 2448, 2444}, {2443, 2450, 2446}, {2450, 2443, 2447}, 
	{2448, 2451, 2447}, {2451, 2448, 2452}, {2452, 2449, 2453}, {2449, 2452, 2448}, 
	{2447, 2454, 2450}, {2454, 2447, 2451}, {2452, 2455, 2451}, {2455, 2452, 2456}, 
	{2456, 2453, 2457}, {2453, 2456, 2452}, {2451, 2458, 2454}, {2458, 2451, 2455}, 
	{2456, 2459, 2455}, {2459, 2456, 2460}, {2460, 2457, 2461}, {2457, 2460, 2456}, 
	{2455, 2462, 2458}, {2462, 2455, 2459}, {2460, 2463, 2459}, {2463, 2460, 2464}, 
	{2464, 2461, 2465}, {2461, 2464, 2460}, {2459, 2466, 2462}, {2466, 2459, 2463}, 
	{2464, 2467, 2463}, {2467, 2464, 2468}, {2468, 2465, 2469}, {2465, 2468, 2464}, 
	{2463, 2470, 2466}, {2470, 2463, 2467}, {2468, 2471, 2467}, {2471, 2468, 2472}, 
	{2472, 2469, 2473}, {2469, 2472, 2468}, {2467, 2474, 2470}, {2474, 2467, 2471}, 
	{2472, 2475, 2471}, {2475, 2472, 2476}, {2476, 2473, 2477}, {2473, 2476, 2472}, 
	{2471, 2478, 2474}, {2478, 2471, 2475}, {2476, 2479, 2475}, {2479, 2476, 2480}, 
	{2480, 2477, 2481}, {2477, 2480, 2476}, {2475, 2482, 2478}, {2482, 2475, 2479}, 
	{2480, 2483, 2479}, {2483, 2480, 2484}, {2484, 2481, 2485}, {2481, 2484, 2480}, 
	{2479, 2486, 2482}, {2486, 2479, 2483}, {2484, 2176, 2483}, {2176, 2484, 2177}, 
	{2177, 2485, 2178}, {2485, 2177, 2484}, {2483, 2179, 2486}, {2179, 2483, 2176}, 
	{1960, 2267, 2247}, {2267, 1960, 2180}, {2247, 2265, 2278}, {2265, 2247, 2267}, 
	{2278, 2270, 2250}, {2270, 2278, 2265}, {2250, 2268, 2286}, {2268, 2250, 2270}, 
	{2286, 2261, 2253}, {2261, 2286, 2268}, {2260, 2263, 2264}, {2263, 2260, 2261}, 
	{2262, 2487, 2305}, {2487, 2262, 2263}, {2305, 2488, 2309}, {2488, 2305, 2487}, 
	{2309, 2489, 2313}, {2489, 2309, 2488}, {2313, 2490, 2317}, {2490, 2313, 2489}, 
	{2317, 2491, 2257}, {2491, 2317, 2490}, {2257, 2492, 2325}, {2492, 2257, 2491}, 
	{2325, 2493, 2329}, {2493, 2325, 2492}, {2329, 2494, 2333}, {2494, 2329, 2493}, 
	{2333, 2495, 2337}, {2495, 2333, 2494}, {2337, 2496, 2341}, {2496, 2337, 2495}, 
	{2341, 2497, 2345}, {2497, 2341, 2496}, {2345, 2498, 2349}, {2498, 2345, 2497}, 
	{2349, 2499, 2353}, {2499, 2349, 2498}, {2353, 2500, 2357}, {2500, 2353, 2499}, 
	{2357, 2501, 2361}, {2501, 2357, 2500}, {2361, 2502, 2365}, {2502, 2361, 2501}, 
	{2180, 2266, 2267}, {2266, 2180, 2197}, {2265, 2269, 2270}, {2269, 2265, 2266}, 
	{2268, 2271, 2261}, {2271, 2268, 2269}, {2263, 2503, 2487}, {2503, 2263, 2271}, 
	{2487, 2504, 2488}, {2504, 2487, 2503}, {2488, 2505, 2489}, {2505, 2488, 2504}, 
	{2489, 2506, 2490}, {2506, 2489, 2505}, {2490, 2507, 2491}, {2507, 2490, 2506}, 
	{2491, 2508, 2492}, {2508, 2491, 2507}, {2492, 2509, 2493}, {2509, 2492, 2508}, 
	{2493, 2510, 2494}, {2510, 2493, 2509}, {2494, 2511, 2495}, {2511, 2494, 2510}, 
	{2495, 2512, 2496}, {2512, 2495, 2511}, {2496, 2513, 2497}, {2513, 2496, 2512}, 
	{2497, 2514, 2498}, {2514, 2497, 2513}, {2498, 2515, 2499}, {2515, 2498, 2514}, 
	{2499, 2516, 2500}, {2516, 2499, 2515}, {2500, 2517, 2501}, {2517, 2500, 2516}, 
	{2501, 2518, 2502}, {2518, 2501, 2517}, {2502, 2520, 2519}, {2520, 2502, 2518}, 
	{2519, 2522, 2521}, {2522, 2519, 2520}, {2365, 2519, 2369}, {2519, 2365, 2502}, 
	{2369, 2521, 2373}, {2521, 2369, 2519}, {2373, 2523, 2377}, {2523, 2373, 2521}, 
	{2219, 2485, 2524}, {2485, 2219, 2178}, {2524, 2481, 2525}, {2481, 2524, 2485}, 
	{2525, 2477, 2526}, {2477, 2525, 2481}, {2526, 2473, 2527}, {2473, 2526, 2477}, 
	{2527, 2469, 2528}, {2469, 2527, 2473}, {2528, 2465, 2529}, {2465, 2528, 2469}, 
	{2529, 2461, 2530}, {2461, 2529, 2465}, {2530, 2457, 2531}, {2457, 2530, 2461}, 
	{2531, 2453, 2532}, {2453, 2531, 2457}, {2532, 2449, 2533}, {2449, 2532, 2453}, 
	{2533, 2445, 2534}, {2445, 2533, 2449}, {2534, 2441, 2535}, {2441, 2534, 2445}, 
	{2535, 2437, 2536}, {2437, 2535, 2441}, {2536, 2433, 2537}, {2433, 2536, 2437}, 
	{2537, 2429, 2538}, {2429, 2537, 2433}, {2538, 2425, 2539}, {2425, 2538, 2429}, 
	{2539, 2421, 2540}, {2421, 2539, 2425}, {2540, 2417, 2541}, {2417, 2540, 2421}, 
	{2541, 2413, 2542}, {2413, 2541, 2417}, {2542, 2409, 2543}, {2409, 2542, 2413}, 
	{2543, 2405, 2544}, {2405, 2543, 2409}, {2544, 2401, 2545}, {2401, 2544, 2405}, 
	{2545, 2397, 2546}, {2397, 2545, 2401}, {2546, 2393, 2547}, {2393, 2546, 2397}, 
	{2547, 2389, 2548}, {2389, 2547, 2393}, {2548, 2385, 2549}, {2385, 2548, 2389}, 
	{2549, 2381, 2550}, {2381, 2549, 2385}, {2550, 2377, 2523}, {2377, 2550, 2381}
};


GLint sup_teeth_indices[1142][3] = {
	{2553, 2552, 2551}, {2556, 2555, 2554}, {2177, 2554, 2555}, {2555, 2557, 2177}, 
	{2558, 2177, 2557}, {2560, 2559, 2556}, {2555, 2556, 2559}, {2559, 2561, 2555}, 
	{2557, 2555, 2561}, {2563, 2562, 2560}, {2559, 2560, 2562}, {2562, 2564, 2559}, 
	{2561, 2559, 2564}, {2566, 2565, 2563}, {2562, 2563, 2565}, {2565, 2567, 2562}, 
	{2564, 2562, 2567}, {2569, 2568, 2566}, {2565, 2566, 2568}, {2568, 2570, 2565}, 
	{2567, 2565, 2570}, {2572, 2571, 2569}, {2568, 2569, 2571}, {2571, 2573, 2568}, 
	{2570, 2568, 2573}, {2575, 2574, 2572}, {2571, 2572, 2574}, {2574, 2576, 2571}, 
	{2573, 2571, 2576}, {2578, 2577, 2575}, {2574, 2575, 2577}, {2577, 2579, 2574}, 
	{2576, 2574, 2579}, {2581, 2580, 2578}, {2577, 2578, 2580}, {2580, 2582, 2577}, 
	{2579, 2577, 2582}, {2584, 2583, 2581}, {2580, 2581, 2583}, {2583, 2585, 2580}, 
	{2582, 2580, 2585}, {2587, 2586, 2584}, {2583, 2584, 2586}, {2586, 2588, 2583}, 
	{2585, 2583, 2588}, {2590, 2589, 2587}, {2586, 2587, 2589}, {2589, 2591, 2586}, 
	{2588, 2586, 2591}, {2593, 2592, 2590}, {2589, 2590, 2592}, {2592, 2594, 2589}, 
	{2591, 2589, 2594}, {2596, 2595, 2593}, {2592, 2593, 2595}, {2595, 2597, 2592}, 
	{2594, 2592, 2597}, {2599, 2598, 2596}, {2595, 2596, 2598}, {2598, 2600, 2595}, 
	{2597, 2595, 2600}, {2602, 2601, 2599}, {2598, 2599, 2601}, {2601, 2603, 2598}, 
	{2600, 2598, 2603}, {2605, 2604, 2602}, {2601, 2602, 2604}, {2604, 2606, 2601}, 
	{2603, 2601, 2606}, {2608, 2607, 2605}, {2604, 2605, 2607}, {2607, 2609, 2604}, 
	{2606, 2604, 2609}, {2611, 2610, 2608}, {2607, 2608, 2610}, {2610, 2612, 2607}, 
	{2609, 2607, 2612}, {2614, 2613, 2611}, {2610, 2611, 2613}, {2613, 2615, 2610}, 
	{2612, 2610, 2615}, {2617, 2616, 2614}, {2613, 2614, 2616}, {2616, 2618, 2613}, 
	{2615, 2613, 2618}, {2620, 2619, 2617}, {2616, 2617, 2619}, {2619, 2621, 2616}, 
	{2618, 2616, 2621}, {2623, 2622, 2620}, {2619, 2620, 2622}, {2622, 2624, 2619}, 
	{2621, 2619, 2624}, {2626, 2625, 2623}, {2622, 2623, 2625}, {2625, 2627, 2622}, 
	{2624, 2622, 2627}, {2629, 2628, 2626}, {2625, 2626, 2628}, {2628, 2630, 2625}, 
	{2627, 2625, 2630}, {2632, 2631, 2629}, {2628, 2629, 2631}, {2631, 2633, 2628}, 
	{2630, 2628, 2633}, {2635, 2634, 2632}, {2631, 2632, 2634}, {2634, 2636, 2631}, 
	{2633, 2631, 2636}, {2638, 2637, 2635}, {2634, 2635, 2637}, {2637, 2639, 2634}, 
	{2636, 2634, 2639}, {2641, 2640, 2638}, {2637, 2638, 2640}, {2640, 2642, 2637}, 
	{2639, 2637, 2642}, {2644, 2643, 2641}, {2640, 2641, 2643}, {2643, 2645, 2640}, 
	{2642, 2640, 2645}, {2647, 2646, 2644}, {2643, 2644, 2646}, {2646, 2648, 2643}, 
	{2645, 2643, 2648}, {2650, 2649, 2647}, {2646, 2647, 2649}, {2649, 2651, 2646}, 
	{2648, 2646, 2651}, {2653, 2652, 2650}, {2649, 2650, 2652}, {2652, 2654, 2649}, 
	{2651, 2649, 2654}, {2656, 2655, 2653}, {2652, 2653, 2655}, {2655, 2657, 2652}, 
	{2654, 2652, 2657}, {2659, 2658, 2656}, {2655, 2656, 2658}, {2658, 2660, 2655}, 
	{2657, 2655, 2660}, {2662, 2661, 2659}, {2658, 2659, 2661}, {2661, 2663, 2658}, 
	{2660, 2658, 2663}, {2665, 2664, 2662}, {2661, 2662, 2664}, {2664, 2666, 2661}, 
	{2663, 2661, 2666}, {2668, 2667, 2665}, {2664, 2665, 2667}, {2667, 2669, 2664}, 
	{2666, 2664, 2669}, {2671, 2670, 2668}, {2667, 2668, 2670}, {2670, 2672, 2667}, 
	{2669, 2667, 2672}, {2674, 2673, 2671}, {2670, 2671, 2673}, {2673, 2675, 2670}, 
	{2672, 2670, 2675}, {2677, 2676, 2674}, {2673, 2674, 2676}, {2676, 2678, 2673}, 
	{2675, 2673, 2678}, {2680, 2679, 2677}, {2676, 2677, 2679}, {2679, 2681, 2676}, 
	{2678, 2676, 2681}, {2683, 2682, 2680}, {2679, 2680, 2682}, {2682, 2684, 2679}, 
	{2681, 2679, 2684}, {2686, 2685, 2683}, {2682, 2683, 2685}, {2685, 2687, 2682}, 
	{2684, 2682, 2687}, {2689, 2688, 2686}, {2685, 2686, 2688}, {2688, 2690, 2685}, 
	{2687, 2685, 2690}, {2692, 2691, 2689}, {2688, 2689, 2691}, {2691, 2693, 2688}, 
	{2690, 2688, 2693}, {2695, 2694, 2692}, {2691, 2692, 2694}, {2694, 2696, 2691}, 
	{2693, 2691, 2696}, {2698, 2697, 2695}, {2694, 2695, 2697}, {2697, 2699, 2694}, 
	{2696, 2694, 2699}, {2701, 2700, 2698}, {2697, 2698, 2700}, {2700, 2702, 2697}, 
	{2699, 2697, 2702}, {2704, 2703, 2701}, {2700, 2701, 2703}, {2703, 2705, 2700}, 
	{2702, 2700, 2705}, {2707, 2706, 2704}, {2703, 2704, 2706}, {2706, 2708, 2703}, 
	{2705, 2703, 2708}, {2710, 2709, 2707}, {2706, 2707, 2709}, {2709, 2711, 2706}, 
	{2708, 2706, 2711}, {2713, 2712, 2710}, {2709, 2710, 2712}, {2712, 2714, 2709}, 
	{2711, 2709, 2714}, {2716, 2715, 2713}, {2712, 2713, 2715}, {2715, 2717, 2712}, 
	{2714, 2712, 2717}, {2719, 2718, 2716}, {2715, 2716, 2718}, {2718, 2720, 2715}, 
	{2717, 2715, 2720}, {2722, 2721, 2719}, {2718, 2719, 2721}, {2721, 2723, 2718}, 
	{2720, 2718, 2723}, {2725, 2724, 2722}, {2721, 2722, 2724}, {2724, 2726, 2721}, 
	{2723, 2721, 2726}, {2728, 2727, 2725}, {2724, 2725, 2727}, {2727, 2729, 2724}, 
	{2726, 2724, 2729}, {2731, 2730, 2728}, {2727, 2728, 2730}, {2730, 2732, 2727}, 
	{2729, 2727, 2732}, {2734, 2733, 2731}, {2730, 2731, 2733}, {2733, 2735, 2730}, 
	{2732, 2730, 2735}, {2645, 2736, 2642}, {2737, 2642, 2736}, {2738, 2639, 2737}, 
	{2642, 2737, 2639}, {2630, 2739, 2627}, {2740, 2627, 2739}, {2741, 2624, 2740}, 
	{2627, 2740, 2624}, {2743, 2732, 2742}, {2735, 2742, 2732}, {2745, 2743, 2744}, 
	{2742, 2744, 2743}, {2746, 2729, 2743}, {2732, 2743, 2729}, {2747, 2746, 2745}, 
	{2743, 2745, 2746}, {2748, 2726, 2746}, {2729, 2746, 2726}, {2749, 2748, 2747}, 
	{2746, 2747, 2748}, {2750, 2723, 2748}, {2726, 2748, 2723}, {2751, 2750, 2749}, 
	{2748, 2749, 2750}, {2752, 2720, 2750}, {2723, 2750, 2720}, {2753, 2752, 2751}, 
	{2750, 2751, 2752}, {2754, 2717, 2752}, {2720, 2752, 2717}, {2755, 2754, 2753}, 
	{2752, 2753, 2754}, {2756, 2714, 2754}, {2717, 2754, 2714}, {2757, 2756, 2755}, 
	{2754, 2755, 2756}, {2758, 2711, 2756}, {2714, 2756, 2711}, {2759, 2758, 2757}, 
	{2756, 2757, 2758}, {2760, 2708, 2758}, {2711, 2758, 2708}, {2761, 2760, 2759}, 
	{2758, 2759, 2760}, {2762, 2705, 2760}, {2708, 2760, 2705}, {2763, 2762, 2761}, 
	{2760, 2761, 2762}, {2764, 2702, 2762}, {2705, 2762, 2702}, {2765, 2764, 2763}, 
	{2762, 2763, 2764}, {2766, 2699, 2764}, {2702, 2764, 2699}, {2552, 2766, 2765}, 
	{2764, 2765, 2766}, {2767, 2696, 2766}, {2699, 2766, 2696}, {2553, 2767, 2552}, 
	{2766, 2552, 2767}, {2768, 2693, 2767}, {2696, 2767, 2693}, {2769, 2768, 2553}, 
	{2767, 2553, 2768}, {2770, 2690, 2768}, {2693, 2768, 2690}, {2771, 2770, 2769}, 
	{2768, 2769, 2770}, {2772, 2687, 2770}, {2690, 2770, 2687}, {2773, 2772, 2771}, 
	{2770, 2771, 2772}, {2774, 2684, 2772}, {2687, 2772, 2684}, {2775, 2774, 2773}, 
	{2772, 2773, 2774}, {2776, 2681, 2774}, {2684, 2774, 2681}, {2777, 2776, 2775}, 
	{2774, 2775, 2776}, {2778, 2678, 2776}, {2681, 2776, 2678}, {2779, 2778, 2777}, 
	{2776, 2777, 2778}, {2780, 2675, 2778}, {2678, 2778, 2675}, {2781, 2780, 2779}, 
	{2778, 2779, 2780}, {2782, 2672, 2780}, {2675, 2780, 2672}, {2783, 2782, 2781}, 
	{2780, 2781, 2782}, {2784, 2669, 2782}, {2672, 2782, 2669}, {2785, 2784, 2783}, 
	{2782, 2783, 2784}, {2786, 2666, 2784}, {2669, 2784, 2666}, {2787, 2786, 2785}, 
	{2784, 2785, 2786}, {2788, 2663, 2786}, {2666, 2786, 2663}, {2789, 2788, 2787}, 
	{2786, 2787, 2788}, {2790, 2660, 2788}, {2663, 2788, 2660}, {2791, 2790, 2789}, 
	{2788, 2789, 2790}, {2792, 2657, 2790}, {2660, 2790, 2657}, {2793, 2792, 2791}, 
	{2790, 2791, 2792}, {2794, 2654, 2792}, {2657, 2792, 2654}, {2795, 2794, 2793}, 
	{2792, 2793, 2794}, {2796, 2651, 2794}, {2654, 2794, 2651}, {2797, 2796, 2795}, 
	{2794, 2795, 2796}, {2798, 2648, 2796}, {2651, 2796, 2648}, {2799, 2798, 2797}, 
	{2796, 2797, 2798}, {2645, 2648, 2736}, {2798, 2736, 2648}, {2800, 2736, 2799}, 
	{2798, 2799, 2736}, {2802, 2551, 2801}, {2552, 2801, 2551}, {2803, 2801, 2765}, 
	{2552, 2765, 2801}, {2804, 2769, 2551}, {2553, 2551, 2769}, {2805, 2771, 2804}, 
	{2769, 2804, 2771}, {2806, 2773, 2805}, {2771, 2805, 2773}, {2807, 2775, 2806}, 
	{2773, 2806, 2775}, {2808, 2777, 2807}, {2775, 2807, 2777}, {2809, 2779, 2808}, 
	{2777, 2808, 2779}, {2810, 2781, 2809}, {2779, 2809, 2781}, {2811, 2783, 2810}, 
	{2781, 2810, 2783}, {2812, 2785, 2811}, {2783, 2811, 2785}, {2813, 2787, 2812}, 
	{2785, 2812, 2787}, {2814, 2789, 2813}, {2787, 2813, 2789}, {2815, 2791, 2814}, 
	{2789, 2814, 2791}, {2816, 2793, 2815}, {2791, 2815, 2793}, {2817, 2795, 2816}, 
	{2793, 2816, 2795}, {2818, 2797, 2817}, {2795, 2817, 2797}, {2800, 2799, 2818}, 
	{2797, 2818, 2799}, {2736, 2800, 2737}, {2819, 2737, 2800}, {2820, 2738, 2819}, 
	{2737, 2819, 2738}, {2820, 2819, 2818}, {2800, 2818, 2819}, {2821, 2820, 2817}, 
	{2818, 2817, 2820}, {2822, 2821, 2816}, {2817, 2816, 2821}, {2822, 2816, 2823}, 
	{2815, 2823, 2816}, {2824, 2823, 2814}, {2815, 2814, 2823}, {2825, 2824, 2813}, 
	{2814, 2813, 2824}, {2826, 2825, 2812}, {2813, 2812, 2825}, {2827, 2826, 2811}, 
	{2812, 2811, 2826}, {2828, 2827, 2810}, {2811, 2810, 2827}, {2829, 2828, 2809}, 
	{2810, 2809, 2828}, {2830, 2829, 2808}, {2809, 2808, 2829}, {2831, 2830, 2807}, 
	{2808, 2807, 2830}, {2832, 2831, 2806}, {2807, 2806, 2831}, {2833, 2832, 2805}, 
	{2806, 2805, 2832}, {2834, 2833, 2804}, {2805, 2804, 2833}, {2802, 2834, 2551}, 
	{2804, 2551, 2834}, {2836, 2753, 2835}, {2751, 2835, 2753}, {2837, 2835, 2749}, 
	{2751, 2749, 2835}, {2839, 2745, 2838}, {2744, 2838, 2745}, {2841, 2761, 2840}, 
	{2759, 2840, 2761}, {2842, 2840, 2757}, {2759, 2757, 2840}, {2837, 2749, 2843}, 
	{2747, 2843, 2749}, {2839, 2843, 2745}, {2747, 2745, 2843}, {2844, 2755, 2836}, 
	{2753, 2836, 2755}, {2842, 2757, 2844}, {2755, 2844, 2757}, {2845, 2763, 2841}, 
	{2761, 2841, 2763}, {2803, 2765, 2845}, {2763, 2845, 2765}, {2847, 2839, 2846}, 
	{2838, 2846, 2839}, {2848, 2843, 2847}, {2839, 2847, 2843}, {2849, 2837, 2848}, 
	{2843, 2848, 2837}, {2850, 2835, 2849}, {2837, 2849, 2835}, {2851, 2836, 2850}, 
	{2835, 2850, 2836}, {2852, 2844, 2851}, {2836, 2851, 2844}, {2853, 2842, 2852}, 
	{2844, 2852, 2842}, {2854, 2840, 2853}, {2842, 2853, 2840}, {2855, 2841, 2854}, 
	{2840, 2854, 2841}, {2856, 2845, 2855}, {2841, 2855, 2845}, {2857, 2803, 2856}, 
	{2845, 2856, 2803}, {2858, 2801, 2857}, {2803, 2857, 2801}, {2859, 2802, 2858}, 
	{2801, 2858, 2802}, {2860, 2834, 2859}, {2802, 2859, 2834}, {2861, 2833, 2860}, 
	{2834, 2860, 2833}, {2862, 2832, 2861}, {2833, 2861, 2832}, {2863, 2831, 2862}, 
	{2832, 2862, 2831}, {2864, 2830, 2863}, {2831, 2863, 2830}, {2865, 2829, 2864}, 
	{2830, 2864, 2829}, {2866, 2828, 2865}, {2829, 2865, 2828}, {2867, 2827, 2866}, 
	{2828, 2866, 2827}, {2868, 2826, 2867}, {2827, 2867, 2826}, {2741, 2825, 2868}, 
	{2826, 2868, 2825}, {2740, 2824, 2741}, {2825, 2741, 2824}, {2739, 2823, 2740}, 
	{2824, 2740, 2823}, {2869, 2822, 2739}, {2823, 2739, 2822}, {2870, 2821, 2869}, 
	{2822, 2869, 2821}, {2738, 2820, 2870}, {2821, 2870, 2820}, {2557, 2847, 2558}, 
	{2846, 2558, 2847}, {2561, 2848, 2557}, {2847, 2557, 2848}, {2564, 2849, 2561}, 
	{2848, 2561, 2849}, {2567, 2850, 2564}, {2849, 2564, 2850}, {2570, 2851, 2567}, 
	{2850, 2567, 2851}, {2573, 2852, 2570}, {2851, 2570, 2852}, {2576, 2853, 2573}, 
	{2852, 2573, 2853}, {2579, 2854, 2576}, {2853, 2576, 2854}, {2582, 2855, 2579}, 
	{2854, 2579, 2855}, {2585, 2856, 2582}, {2855, 2582, 2856}, {2588, 2857, 2585}, 
	{2856, 2585, 2857}, {2591, 2858, 2588}, {2857, 2588, 2858}, {2594, 2859, 2591}, 
	{2858, 2591, 2859}, {2597, 2860, 2594}, {2859, 2594, 2860}, {2600, 2861, 2597}, 
	{2860, 2597, 2861}, {2603, 2862, 2600}, {2861, 2600, 2862}, {2606, 2863, 2603}, 
	{2862, 2603, 2863}, {2609, 2864, 2606}, {2863, 2606, 2864}, {2612, 2865, 2609}, 
	{2864, 2609, 2865}, {2615, 2866, 2612}, {2865, 2612, 2866}, {2618, 2867, 2615}, 
	{2866, 2615, 2867}, {2621, 2868, 2618}, {2867, 2618, 2868}, {2624, 2741, 2621}, 
	{2868, 2621, 2741}, {2633, 2869, 2630}, {2739, 2630, 2869}, {2636, 2870, 2633}, 
	{2869, 2633, 2870}, {2639, 2738, 2636}, {2870, 2636, 2738}, {2873, 2872, 2871}, 
	{2554, 2875, 2874}, {2875, 2554, 2177}, {2177, 2876, 2875}, {2876, 2177, 2558}, 
	{2874, 2878, 2877}, {2878, 2874, 2875}, {2875, 2879, 2878}, {2879, 2875, 2876}, 
	{2877, 2881, 2880}, {2881, 2877, 2878}, {2878, 2882, 2881}, {2882, 2878, 2879}, 
	{2880, 2884, 2883}, {2884, 2880, 2881}, {2881, 2885, 2884}, {2885, 2881, 2882}, 
	{2883, 2887, 2886}, {2887, 2883, 2884}, {2884, 2888, 2887}, {2888, 2884, 2885}, 
	{2886, 2890, 2889}, {2890, 2886, 2887}, {2887, 2891, 2890}, {2891, 2887, 2888}, 
	{2889, 2893, 2892}, {2893, 2889, 2890}, {2890, 2894, 2893}, {2894, 2890, 2891}, 
	{2892, 2896, 2895}, {2896, 2892, 2893}, {2893, 2897, 2896}, {2897, 2893, 2894}, 
	{2895, 2899, 2898}, {2899, 2895, 2896}, {2896, 2900, 2899}, {2900, 2896, 2897}, 
	{2898, 2902, 2901}, {2902, 2898, 2899}, {2899, 2903, 2902}, {2903, 2899, 2900}, 
	{2901, 2905, 2904}, {2905, 2901, 2902}, {2902, 2906, 2905}, {2906, 2902, 2903}, 
	{2904, 2908, 2907}, {2908, 2904, 2905}, {2905, 2909, 2908}, {2909, 2905, 2906}, 
	{2907, 2911, 2910}, {2911, 2907, 2908}, {2908, 2912, 2911}, {2912, 2908, 2909}, 
	{2910, 2914, 2913}, {2914, 2910, 2911}, {2911, 2915, 2914}, {2915, 2911, 2912}, 
	{2913, 2917, 2916}, {2917, 2913, 2914}, {2914, 2918, 2917}, {2918, 2914, 2915}, 
	{2916, 2920, 2919}, {2920, 2916, 2917}, {2917, 2921, 2920}, {2921, 2917, 2918}, 
	{2919, 2923, 2922}, {2923, 2919, 2920}, {2920, 2924, 2923}, {2924, 2920, 2921}, 
	{2922, 2926, 2925}, {2926, 2922, 2923}, {2923, 2927, 2926}, {2927, 2923, 2924}, 
	{2925, 2929, 2928}, {2929, 2925, 2926}, {2926, 2930, 2929}, {2930, 2926, 2927}, 
	{2928, 2932, 2931}, {2932, 2928, 2929}, {2929, 2933, 2932}, {2933, 2929, 2930}, 
	{2931, 2935, 2934}, {2935, 2931, 2932}, {2932, 2936, 2935}, {2936, 2932, 2933}, 
	{2934, 2938, 2937}, {2938, 2934, 2935}, {2935, 2939, 2938}, {2939, 2935, 2936}, 
	{2937, 2941, 2940}, {2941, 2937, 2938}, {2938, 2942, 2941}, {2942, 2938, 2939}, 
	{2940, 2944, 2943}, {2944, 2940, 2941}, {2941, 2945, 2944}, {2945, 2941, 2942}, 
	{2943, 2947, 2946}, {2947, 2943, 2944}, {2944, 2948, 2947}, {2948, 2944, 2945}, 
	{2946, 2950, 2949}, {2950, 2946, 2947}, {2947, 2951, 2950}, {2951, 2947, 2948}, 
	{2949, 2953, 2952}, {2953, 2949, 2950}, {2950, 2954, 2953}, {2954, 2950, 2951}, 
	{2952, 2956, 2955}, {2956, 2952, 2953}, {2953, 2957, 2956}, {2957, 2953, 2954}, 
	{2955, 2959, 2958}, {2959, 2955, 2956}, {2956, 2960, 2959}, {2960, 2956, 2957}, 
	{2958, 2962, 2961}, {2962, 2958, 2959}, {2959, 2963, 2962}, {2963, 2959, 2960}, 
	{2961, 2965, 2964}, {2965, 2961, 2962}, {2962, 2966, 2965}, {2966, 2962, 2963}, 
	{2964, 2968, 2967}, {2968, 2964, 2965}, {2965, 2969, 2968}, {2969, 2965, 2966}, 
	{2967, 2971, 2970}, {2971, 2967, 2968}, {2968, 2972, 2971}, {2972, 2968, 2969}, 
	{2970, 2974, 2973}, {2974, 2970, 2971}, {2971, 2975, 2974}, {2975, 2971, 2972}, 
	{2973, 2977, 2976}, {2977, 2973, 2974}, {2974, 2978, 2977}, {2978, 2974, 2975}, 
	{2976, 2980, 2979}, {2980, 2976, 2977}, {2977, 2981, 2980}, {2981, 2977, 2978}, 
	{2979, 2983, 2982}, {2983, 2979, 2980}, {2980, 2984, 2983}, {2984, 2980, 2981}, 
	{2982, 2986, 2985}, {2986, 2982, 2983}, {2983, 2987, 2986}, {2987, 2983, 2984}, 
	{2985, 2989, 2988}, {2989, 2985, 2986}, {2986, 2990, 2989}, {2990, 2986, 2987}, 
	{2988, 2992, 2991}, {2992, 2988, 2989}, {2989, 2993, 2992}, {2993, 2989, 2990}, 
	{2991, 2995, 2994}, {2995, 2991, 2992}, {2992, 2996, 2995}, {2996, 2992, 2993}, 
	{2994, 2998, 2997}, {2998, 2994, 2995}, {2995, 2999, 2998}, {2999, 2995, 2996}, 
	{2997, 3001, 3000}, {3001, 2997, 2998}, {2998, 3002, 3001}, {3002, 2998, 2999}, 
	{3000, 3004, 3003}, {3004, 3000, 3001}, {3001, 3005, 3004}, {3005, 3001, 3002}, 
	{3003, 3007, 3006}, {3007, 3003, 3004}, {3004, 3008, 3007}, {3008, 3004, 3005}, 
	{3006, 3010, 3009}, {3010, 3006, 3007}, {3007, 3011, 3010}, {3011, 3007, 3008}, 
	{3009, 3013, 3012}, {3013, 3009, 3010}, {3010, 3014, 3013}, {3014, 3010, 3011}, 
	{3012, 3016, 3015}, {3016, 3012, 3013}, {3013, 3017, 3016}, {3017, 3013, 3014}, 
	{3015, 3019, 3018}, {3019, 3015, 3016}, {3016, 3020, 3019}, {3020, 3016, 3017}, 
	{3018, 3022, 3021}, {3022, 3018, 3019}, {3019, 3023, 3022}, {3023, 3019, 3020}, 
	{3021, 3025, 3024}, {3025, 3021, 3022}, {3022, 3026, 3025}, {3026, 3022, 3023}, 
	{3024, 3028, 3027}, {3028, 3024, 3025}, {3025, 3029, 3028}, {3029, 3025, 3026}, 
	{3027, 3031, 3030}, {3031, 3027, 3028}, {3028, 3032, 3031}, {3032, 3028, 3029}, 
	{3030, 3034, 3033}, {3034, 3030, 3031}, {3031, 3035, 3034}, {3035, 3031, 3032}, 
	{3033, 3037, 3036}, {3037, 3033, 3034}, {3034, 3038, 3037}, {3038, 3034, 3035}, 
	{3036, 3040, 3039}, {3040, 3036, 3037}, {3037, 3041, 3040}, {3041, 3037, 3038}, 
	{3039, 3043, 3042}, {3043, 3039, 3040}, {3040, 3044, 3043}, {3044, 3040, 3041}, 
	{3042, 3046, 3045}, {3046, 3042, 3043}, {3043, 3047, 3046}, {3047, 3043, 3044}, 
	{3045, 3049, 3048}, {3049, 3045, 3046}, {3046, 3050, 3049}, {3050, 3046, 3047}, 
	{3048, 2733, 2734}, {2733, 3048, 3049}, {3049, 2735, 2733}, {2735, 3049, 3050}, 
	{2960, 3051, 2963}, {3051, 2960, 3052}, {3052, 2957, 3053}, {2957, 3052, 2960}, 
	{2945, 3054, 2948}, {3054, 2945, 3055}, {3055, 2942, 3056}, {2942, 3055, 2945}, 
	{2742, 3050, 3057}, {3050, 2742, 2735}, {2744, 3057, 3058}, {3057, 2744, 2742}, 
	{3057, 3047, 3059}, {3047, 3057, 3050}, {3058, 3059, 3060}, {3059, 3058, 3057}, 
	{3059, 3044, 3061}, {3044, 3059, 3047}, {3060, 3061, 3062}, {3061, 3060, 3059}, 
	{3061, 3041, 3063}, {3041, 3061, 3044}, {3062, 3063, 3064}, {3063, 3062, 3061}, 
	{3063, 3038, 3065}, {3038, 3063, 3041}, {3064, 3065, 3066}, {3065, 3064, 3063}, 
	{3065, 3035, 3067}, {3035, 3065, 3038}, {3066, 3067, 3068}, {3067, 3066, 3065}, 
	{3067, 3032, 3069}, {3032, 3067, 3035}, {3068, 3069, 3070}, {3069, 3068, 3067}, 
	{3069, 3029, 3071}, {3029, 3069, 3032}, {3070, 3071, 3072}, {3071, 3070, 3069}, 
	{3071, 3026, 3073}, {3026, 3071, 3029}, {3072, 3073, 3074}, {3073, 3072, 3071}, 
	{3073, 3023, 3075}, {3023, 3073, 3026}, {3074, 3075, 3076}, {3075, 3074, 3073}, 
	{3075, 3020, 3077}, {3020, 3075, 3023}, {3076, 3077, 3078}, {3077, 3076, 3075}, 
	{3077, 3017, 3079}, {3017, 3077, 3020}, {3078, 3079, 2872}, {3079, 3078, 3077}, 
	{3079, 3014, 3080}, {3014, 3079, 3017}, {2872, 3080, 2871}, {3080, 2872, 3079}, 
	{3080, 3011, 3081}, {3011, 3080, 3014}, {2871, 3081, 3082}, {3081, 2871, 3080}, 
	{3081, 3008, 3083}, {3008, 3081, 3011}, {3082, 3083, 3084}, {3083, 3082, 3081}, 
	{3083, 3005, 3085}, {3005, 3083, 3008}, {3084, 3085, 3086}, {3085, 3084, 3083}, 
	{3085, 3002, 3087}, {3002, 3085, 3005}, {3086, 3087, 3088}, {3087, 3086, 3085}, 
	{3087, 2999, 3089}, {2999, 3087, 3002}, {3088, 3089, 3090}, {3089, 3088, 3087}, 
	{3089, 2996, 3091}, {2996, 3089, 2999}, {3090, 3091, 3092}, {3091, 3090, 3089}, 
	{3091, 2993, 3093}, {2993, 3091, 2996}, {3092, 3093, 3094}, {3093, 3092, 3091}, 
	{3093, 2990, 3095}, {2990, 3093, 2993}, {3094, 3095, 3096}, {3095, 3094, 3093}, 
	{3095, 2987, 3097}, {2987, 3095, 2990}, {3096, 3097, 3098}, {3097, 3096, 3095}, 
	{3097, 2984, 3099}, {2984, 3097, 2987}, {3098, 3099, 3100}, {3099, 3098, 3097}, 
	{3099, 2981, 3101}, {2981, 3099, 2984}, {3100, 3101, 3102}, {3101, 3100, 3099}, 
	{3101, 2978, 3103}, {2978, 3101, 2981}, {3102, 3103, 3104}, {3103, 3102, 3101}, 
	{3103, 2975, 3105}, {2975, 3103, 2978}, {3104, 3105, 3106}, {3105, 3104, 3103}, 
	{3105, 2972, 3107}, {2972, 3105, 2975}, {3106, 3107, 3108}, {3107, 3106, 3105}, 
	{3107, 2969, 3109}, {2969, 3107, 2972}, {3108, 3109, 3110}, {3109, 3108, 3107}, 
	{3109, 2966, 3111}, {2966, 3109, 2969}, {3110, 3111, 3112}, {3111, 3110, 3109}, 
	{3051, 2966, 2963}, {2966, 3051, 3111}, {3112, 3051, 3113}, {3051, 3112, 3111}, 
	{3115, 2873, 3114}, {2873, 3115, 2872}, {3078, 3115, 3116}, {3115, 3078, 2872}, 
	{2873, 3082, 3117}, {3082, 2873, 2871}, {3117, 3084, 3118}, {3084, 3117, 3082}, 
	{3118, 3086, 3119}, {3086, 3118, 3084}, {3119, 3088, 3120}, {3088, 3119, 3086}, 
	{3120, 3090, 3121}, {3090, 3120, 3088}, {3121, 3092, 3122}, {3092, 3121, 3090}, 
	{3122, 3094, 3123}, {3094, 3122, 3092}, {3123, 3096, 3124}, {3096, 3123, 3094}, 
	{3124, 3098, 3125}, {3098, 3124, 3096}, {3125, 3100, 3126}, {3100, 3125, 3098}, 
	{3126, 3102, 3127}, {3102, 3126, 3100}, {3127, 3104, 3128}, {3104, 3127, 3102}, 
	{3128, 3106, 3129}, {3106, 3128, 3104}, {3129, 3108, 3130}, {3108, 3129, 3106}, 
	{3130, 3110, 3131}, {3110, 3130, 3108}, {3131, 3112, 3113}, {3112, 3131, 3110}, 
	{3052, 3113, 3051}, {3113, 3052, 3132}, {3132, 3053, 3133}, {3053, 3132, 3052}, 
	{3131, 3132, 3133}, {3132, 3131, 3113}, {3130, 3133, 3134}, {3133, 3130, 3131}, 
	{3129, 3134, 3135}, {3134, 3129, 3130}, {3136, 3129, 3135}, {3129, 3136, 3128}, 
	{3127, 3136, 3137}, {3136, 3127, 3128}, {3126, 3137, 3138}, {3137, 3126, 3127}, 
	{3125, 3138, 3139}, {3138, 3125, 3126}, {3124, 3139, 3140}, {3139, 3124, 3125}, 
	{3123, 3140, 3141}, {3140, 3123, 3124}, {3122, 3141, 3142}, {3141, 3122, 3123}, 
	{3121, 3142, 3143}, {3142, 3121, 3122}, {3120, 3143, 3144}, {3143, 3120, 3121}, 
	{3119, 3144, 3145}, {3144, 3119, 3120}, {3118, 3145, 3146}, {3145, 3118, 3119}, 
	{3117, 3146, 3147}, {3146, 3117, 3118}, {2873, 3147, 3114}, {3147, 2873, 3117}, 
	{3149, 3066, 3148}, {3066, 3149, 3064}, {3062, 3149, 3150}, {3149, 3062, 3064}, 
	{2838, 3058, 3151}, {3058, 2838, 2744}, {3153, 3074, 3152}, {3074, 3153, 3072}, 
	{3070, 3153, 3154}, {3153, 3070, 3072}, {3155, 3062, 3150}, {3062, 3155, 3060}, 
	{3058, 3155, 3151}, {3155, 3058, 3060}, {3148, 3068, 3156}, {3068, 3148, 3066}, 
	{3156, 3070, 3154}, {3070, 3156, 3068}, {3152, 3076, 3157}, {3076, 3152, 3074}, 
	{3157, 3078, 3116}, {3078, 3157, 3076}, {2846, 3151, 3158}, {3151, 2846, 2838}, 
	{3158, 3155, 3159}, {3155, 3158, 3151}, {3159, 3150, 3160}, {3150, 3159, 3155}, 
	{3160, 3149, 3161}, {3149, 3160, 3150}, {3161, 3148, 3162}, {3148, 3161, 3149}, 
	{3162, 3156, 3163}, {3156, 3162, 3148}, {3163, 3154, 3164}, {3154, 3163, 3156}, 
	{3164, 3153, 3165}, {3153, 3164, 3154}, {3165, 3152, 3166}, {3152, 3165, 3153}, 
	{3166, 3157, 3167}, {3157, 3166, 3152}, {3167, 3116, 3168}, {3116, 3167, 3157}, 
	{3168, 3115, 3169}, {3115, 3168, 3116}, {3169, 3114, 3170}, {3114, 3169, 3115}, 
	{3170, 3147, 3171}, {3147, 3170, 3114}, {3171, 3146, 3172}, {3146, 3171, 3147}, 
	{3172, 3145, 3173}, {3145, 3172, 3146}, {3173, 3144, 3174}, {3144, 3173, 3145}, 
	{3174, 3143, 3175}, {3143, 3174, 3144}, {3175, 3142, 3176}, {3142, 3175, 3143}, 
	{3176, 3141, 3177}, {3141, 3176, 3142}, {3177, 3140, 3178}, {3140, 3177, 3141}, 
	{3178, 3139, 3179}, {3139, 3178, 3140}, {3179, 3138, 3056}, {3138, 3179, 3139}, 
	{3056, 3137, 3055}, {3137, 3056, 3138}, {3055, 3136, 3054}, {3136, 3055, 3137}, 
	{3054, 3135, 3180}, {3135, 3054, 3136}, {3180, 3134, 3181}, {3134, 3180, 3135}, 
	{3181, 3133, 3053}, {3133, 3181, 3134}, {2558, 3158, 2876}, {3158, 2558, 2846}, 
	{2876, 3159, 2879}, {3159, 2876, 3158}, {2879, 3160, 2882}, {3160, 2879, 3159}, 
	{2882, 3161, 2885}, {3161, 2882, 3160}, {2885, 3162, 2888}, {3162, 2885, 3161}, 
	{2888, 3163, 2891}, {3163, 2888, 3162}, {2891, 3164, 2894}, {3164, 2891, 3163}, 
	{2894, 3165, 2897}, {3165, 2894, 3164}, {2897, 3166, 2900}, {3166, 2897, 3165}, 
	{2900, 3167, 2903}, {3167, 2900, 3166}, {2903, 3168, 2906}, {3168, 2903, 3167}, 
	{2906, 3169, 2909}, {3169, 2906, 3168}, {2909, 3170, 2912}, {3170, 2909, 3169}, 
	{2912, 3171, 2915}, {3171, 2912, 3170}, {2915, 3172, 2918}, {3172, 2915, 3171}, 
	{2918, 3173, 2921}, {3173, 2918, 3172}, {2921, 3174, 2924}, {3174, 2921, 3173}, 
	{2924, 3175, 2927}, {3175, 2924, 3174}, {2927, 3176, 2930}, {3176, 2927, 3175}, 
	{2930, 3177, 2933}, {3177, 2930, 3176}, {2933, 3178, 2936}, {3178, 2933, 3177}, 
	{2936, 3179, 2939}, {3179, 2936, 3178}, {2939, 3056, 2942}, {3056, 2939, 3179}, 
	{2948, 3180, 2951}, {3180, 2948, 3054}, {2951, 3181, 2954}, {3181, 2951, 3180}, 
	{2954, 3053, 2957}, {3053, 2954, 3181}
};

//definiamo un array di puntatori agli indici delle superfici della pelle del volto
//GLint **skin_surfaces_indices[69]; //il termine surface_indices[0] non punterà a nulla, però gli altri punteranno ad altri vettori di interi bidimensionali
GLint **misc_surfaces_indices[12]; //per misc_point 


//questo è un vettore di cui ogni elemento è un vettore che punta ad una lista di interi
// che contiene gli indici dei punti che appartengono alle relative superfici,

GLint *surface_list2[12]; //per misc_point
