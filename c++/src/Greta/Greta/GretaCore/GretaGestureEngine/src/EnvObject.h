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

/// @file EnvObject.h
/// interface for the EnvObject class (environment objects; not used?)
///
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENVOBJECT_H__30C72782_5150_11D6_9F3E_B790A24CE46D__INCLUDED_)
#define AFX_ENVOBJECT_H__30C72782_5150_11D6_9F3E_B790A24CE46D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include <gl/glut.h>
#include <time.h>
#include "GestureEngineGlobals.h"


/**
*
* class :EnvObject
*
*/

class EnvObject  
{
private:
	float xshoulder;
	float yshoulder;
	float zshoulder;
	float xpos;
	float ypos;
	float zpos;
	float r,g,b,a;
public:

	/**
	* contructor 
	*
	* @param  x
	* @param  y
	* @param  z
	*/

	EnvObject(float x,float y,float z);

	/**
	*  
	* 
	*
	* @param  sx
	* @param  float sy
	* @param  float sz
	*/

	void EnvObject::SetShoulderPos(float sx, float sy, float sz);

	/**
	*  
	* 
	*
	* @return 
	*/

	float EnvObject::GetX();

	/**
	*  
	* 
	*
	* @return 
	*/

	float EnvObject::GetY();

	/**
	*  
	* 
	*
	* @return 
	*/

	float EnvObject::GetZ();

	/**
	*  
	* 
	*
	*/

	void EnvObject::DrawDirection();

	/**
	*  
	* 
	*
	*/

	void EnvObject::Draw();

	/**
	* destructor 
	*/

	virtual ~EnvObject();

};

#endif // !defined(AFX_ENVOBJECT_H__30C72782_5150_11D6_9F3E_B790A24CE46D__INCLUDED_)
