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

// BodyData.h: interface for the BodyData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BODYDATA_H__B9498960_68D4_11D9_9C92_E01642AFC047__INCLUDED_)
#define AFX_BODYDATA_H__B9498960_68D4_11D9_9C92_E01642AFC047__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "BodyVertexGroup.h"
#include "BAPConverter.h"
#include <string>
#include "FL/gl.h"

/**
*
* class :BodyData
*
*/

class BodyData  
{
public:

/**
*
* contructor 
*
* @param conv
*/

	BodyData(BAPConverter *conv);

/**
*
* destructor 
*/

	virtual ~BodyData();

/**
 *  
 * 
 *
* @return 
*/

	bool ReadBodyGeometry();

	int num_v;//BODY:total number of body vertices
	int num_e;//BODY:total number of body edges, not used
	int num_f;//BODY:total number of body faces, not used
	GLfloat* normals;//BODY:skin normals,rest position
	GLfloat* vertices;//BODY:skin vertices,rest position
	GLfloat* normals2;//BODY:skin normals,dynamic position
	GLfloat* vertices2;//BODY:skin vertices,dynamic position
	GLfloat* bodyUV;//BODY:UV coordinates
	GLint* indices;//BODY:indices to allow body visualization with glDrawElements
	BodyVertexGroup* bodyvertexgroups[NUMJOINTS];//body vertex groups; one group for each joint
	BodyVertexGroup* pJointBVGs[NUMJOINTS];//BODY: direct access to joint body vertex groups
	BAPConverter *conv;

/**
 *  
 * 
 *
* @return 
* @param  path
*/

	int GetFileNames(std::string path);
	std::string meshfilename,hierarchyfilename,bonesfilename;

	typedef struct
	{
		std::string image;
		bool alpha;
		
	} Texture;

	Texture body_texture;
};

#endif // !defined(AFX_BODYDATA_H__B9498960_68D4_11D9_9C92_E01642AFC047__INCLUDED_)
