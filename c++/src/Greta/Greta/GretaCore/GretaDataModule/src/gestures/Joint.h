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

/// @file Joint.h
/// interface for the Joint class.
///
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JOINT_H__CFEED377_75D8_49B9_9D48_987A416F9AD6__INCLUDED_)
#define AFX_JOINT_H__CFEED377_75D8_49B9_9D48_987A416F9AD6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "GestureEngineGlobals.h"

//#include <string>
//#include <iostream>
//#include <vector>


//#include "baptypes.h"
#include "BAPframe.h"
#include "algebra3.h"
#include "glut.h"

/**@#-*/
using namespace alg3;
namespace GestureSpace {
/**@#+*/

	class Joint; // forward declaration
	
	/** A JointVector is a vector/array of pointers to Joint objects */
	typedef std::vector<Joint*> JointVector;
	
	/** A Joint is a node in the skeletal hierarchy with transformation and display functions
	*
	* @author Bjoern Hartmann, bjoern@graphics.cis.upenn.edu 
	* @version 1.0
	*/
	class Joint  
	{
	public:
		void SaveInitialTranslation();
		void LoadBAPs(BAPFrame &f);
		void RotateAbsXYZ(BAPFrame& f);
		void RotateAbsXYZ(double x, double y, double z);
		vec3 GetTranslation();
		void SetParent(Joint* p);
		
		/** string that holds the H-Anim name of this joint */
		std::string name;
		
		/** string that holds the rotate order for Euler-angle rotations */
		std::string rotate_order;
		
		/** Relative Rotation: */
		void RotateXYZ(double x, double y, double z);
		
		void PrintPoint(double x, double y, double z);
		void Rotate(double x, double y, double z, double ang);
		void Rotate(vec3 Axis, const double angleDeg);
		void Translate(double x, double y, double z);
		void TranslateAbs(double x, double y, double z);
		void Translate(vec3& v);
		
		/** set world matrix top down, depth-first (recurses on children) */
		void SetWorldMatrix();  
		
		void Print();
		
		/** Draw function that draws joint-and-bone skeleton only */
		void Draw();
		
		Joint();
		virtual ~Joint();
		
		/* which joint in the H-Anim joint hierarchy is this? */
		joint_type type;
		
		/** local transformation matrix */
		mat4 m;	  
		/** global transformation matrix */
		mat4 wm;  
		/** NOT IMPLEMENTED: inverse of world matrix */
		mat4 wmi; 
		
		/** vector that saves initial global translation of this joint from the world origin */
		vec3 tx0;
		
		/** Pointer to this Joint's parent or NULL if parent does not exist */
		Joint*	parent;
		
		/** Vector of pointers to this Joint's children */
		JointVector children;
		
		/** Draw routine that recalculates the vertices and normals for model "jackie"
		*
		* See jackie.h and jakieGroups.h
		*/
	private:
		
		/** OBSOLETE */
		void DrawLocal2();
		
		/** this joint's local Euler rotations */
		double mRotX, mRotY, mRotZ;
		
		bool output;
		
		/** used by draw() to display bones and joints using OpenGL */
		void DrawLocal();
	};
/**@#-*/
} // END namespace GestureSpace
/**@#+*/
#endif // !defined(AFX_JOINT_H__CFEED377_75D8_49B9_9D48_987A416F9AD6__INCLUDED_)
