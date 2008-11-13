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

#pragma once
#ifndef COMPILED_FACEDATA
#define COMPILED_FACEDATA

#include "../PlayerGlobals.h"
#include "init.h"
#include "FAPData.h"

#include <string>
#include <FL/gl.h>

/**
*
* class :GretaTexture
*
*/

class GretaTexture
{
public:
	std::string image;
	bool alpha;

};


/**
*
* class :FaceData
*
*/

class FaceData
{
public:

	/**
	*
	* contructor 
	*
	*/

	FaceData(void);

	/**
	*
	* destructor 
	*/

	~FaceData(void);


	/**
	* this method 
	* 
	*
	* @return 
	* @param  modelname
	*/

	int GetFileNames(std::string modelname);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  modelname
	*/

	int ReadFaceGeometry(std::string modelname);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  path
	*/

	int ReadFaceVertices(std::string path);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  path
	*/

	int ReadFaceIndices(std::string path);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  path
	*/

	int ReadFaceParts(std::string path);

	/**
	* this method 
	* 
	*
	*/

	void PreScaleFace();


	/**
	* this method 
	* 
	*
	*/

	void CopyVertexData();


	/**
	* this method 
	* 
	*
	*/

	void CopyVdata2SkinPoint();

	/**
	* this method 
	* 
	*
	*/

	void InitializeSkinPointArray();

	/**
	* this method 
	* 
	*
	*/

	void CalculateSkinDistanceLwrin();

	/**
	* this method 
	* 
	*
	*/

	void CalculateSkinDistanceInfeyelid();

	/**
	* this method 
	* 
	*
	*/

	void CalculateSkinDistanceSupeyelid();

	/**
	* this method 
	* 
	*
	*/

	void CalculateColorEyelid();


	/**
	* this method 
	* 
	*
	* @param p1
	* @param p2
	* @param p3
	* @param norm 
	*/

	void CalculateNormal( float *p1, float *p2,  float *p3, float *norm );

	/**
	* this method 
	* 
	*
	*/

	void InitializeSkinNormals();

	/**
	* this method 
	* 
	*
	* @param  surface_number
	*/

	void InitializeSkinSurfaces(int surface_number);

	/**
	* this method 
	* 
	*
	*/

	void CreateSkinSurfaceList();

	/**
	* this method 
	* 
	*
	* @param  flag_surface1
	*/

	void CalculateSkinNormalsRuntime(GLint* flag_surface1);

	/**
	* this method 
	* 
	*
	* @param FAPs
	* @param  frame_count
	*/

	void ForeheadBump(FAPdata *FAPs,int frame_count);


	/**
	* this method 
	* 
	*
	* @return 
	* @param  path
	*/

	int ReadHair(std::string path);

	std::string verticesfilename,fdpsfilename,indicesfilename,textured,hairfilename;

	FDPdata *FDPs;
	FAPUdata *FAPUs;

	GLfloat **haircoords;
	GLfloat **hairnormals;
	GLint ** hairindices;

	GLfloat HAIR_sx; //61.0;//54
	GLfloat HAIR_sy; //62.0;//67
	GLfloat HAIR_sz; //62.0;
	GLfloat HAIR_rx;
	GLfloat HAIR_ry;
	GLfloat HAIR_rz;
	GLfloat HAIR_tx;
	GLfloat HAIR_ty; //0.0;//4
	GLfloat HAIR_tz; //1.0;//0

	int FaceVerticesNumber;
	int TextureVerticesNumber;
	int VerticesNormalsNumber;
	int TrianglesNumber;
	int HairVerticesNumber;
	int HairNormalsNumber;
	int HairIndicesNumber;

	bool FaceSkinTextureMapping;

	GLfloat **vdata;
	GLfloat **tdata;
	pointtype *skin_point;//FACE:face skin vertices
	GLfloat **vdatafordrawing;

	GLint *surface_list1[69];
	GLint surface_num_polygons1[69];
	GLint *** regions_indices;
	GLint *** textures_indices;

	float EYES_SCALE;
	float EYES_LZ;
	float EYES_LX;
	float EYES_LY;
	float EYES_RZ;
	float EYES_RX;
	float EYES_RY;
	float TEETHS_X;
	float TEETHS_Y;
	float TEETHS_Z;

	float HEADSCALE;

	float HEADPOS_X;
	float HEADPOS_Y;
	float HEADPOS_Z;


	GretaTexture eyebulbs_texture;
	GretaTexture eyelashes_texture;
	GretaTexture lips_texture;
	GretaTexture head_texture;
};


#endif
