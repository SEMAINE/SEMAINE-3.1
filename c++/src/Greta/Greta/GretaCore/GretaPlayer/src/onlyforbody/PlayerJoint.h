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

/** holds the structure and functions of a single skeleton joint
*   it's the most important class of the BAP player
*
* @see BodyVertexGroup
* @author Maurizio Mancini, manmau@yahoo.com
* @author Bjoern Hartmann, bjoern@graphics.cis.upenn.edu 
* @version 1.0
*/
#ifndef _MAURIZIO_JOINT_H
#define _MAURIZIO_JOINT_H

#include <GL/glut.h>
#include <vector>

#include "../point3d.h"

//body vertices are split into some subsets (called groups) and each subset
//is associated with a single joint. so each subset is an array of indices
//pointing some vertices of the body vertex array. this array is store into
//an object of BodyVertexGroup class.
#include "BodyVertexGroup.h"

//this is a very useful math library to do 3D operations between vectors
//and matrices. it'll be used very often in the code.
#include "BAPConverter.h"
#include "algebra3.h"
//this class holds all the joint's informations. its position and state of rotation
//and a lot of functions.
/**
*
* class :PlayerJoint
*
*/

class PlayerJoint  
{

//private members
private:

	//holds the maximum sons number for each joint
	static int PlayerJoint::nsons;

	//from Bjoern Hartmann's Joint code 
	alg3::mat4 m;//rotaion & translation matrix for this joint
	alg3::mat4 wm;//world rotation & translation matrix (multiplication of all
	              //the parent's world matrices)
	alg3::vec3 tx0;//translation vector from the parent joint to this joint
	bool changed; //has this joint changed since last time?
//public members
public:

/**
 *  
 * 
 *
* @return 
* @param  jointname
*/

	bool LoadGLWorldMatrix(const char* jointname);

/**
*
* contructor 
*
* @param  jointname
* @param p
*/


/**
 *  
 * 
 *
* @param  jointname
* @param p
*/

	void GetPointer(const char* jointname, PlayerJoint **p);

/**
 *  
 * 
 *
* @return 
*/

	bool GetChanged();

/**
 *  
 * 
 *
* @param  c
*/

	void SetChanged(bool c);

/**
 *  
 * 
 *
* @param  c
*/

	void SetChangedRecursive(bool c);

/**
 *  
 * 
 *
* @param  c=false
*/

	void PropagateChanged(bool c=false);

/**
 *  
 * contructor, needs joint's name
 *
* @return 
* @param  jointname
* @param  bonesfilename
*/

	PlayerJoint(char* jointname,std::string bonesfilename);
	
/**
*
* destructor 
*/

	virtual ~PlayerJoint();

/**
 *  
 * 
 *
*/

	void ResetRotation();

/**
*
* array to hold the pointers to the joint's sons
*
*/

	PlayerJoint* sons[5];

/**
*
* pointer to parent joint
*
*/

	PlayerJoint* parent;
	//name of the joint
	char name[30];
	//joint's translation
	point3d translation;
	//joint's rotation
	point3d rotation;

	//not all the joints are allowed to rotate on both three axes
	//not very useful in the BAP player but this class is also
	//used in the pose editor
	bool enableX,enableY,enableZ;

/**
 *  
 * given a joint's pointer adds it to the current joint's sons vector
 *
* @return 
* @param  son
*/

	bool PlayerJoint::AddSon(PlayerJoint* son);
	
/**
 *  
 * scan the skeleton's file searching for current joint's sons
 *
* @param  bonesfilename
* @param  hierarchyfilename
*/

	void PlayerJoint::ScanForSons(std::string bonesfilename,std::string hierarchyfilename);

/**
 *  
 * scans the constraint's file and sets enableX,enableY,enableZ
 *
*/

	void PlayerJoint::ScanForConstraints();

/**
 *  
 * gets joint's x-y-z rotations
 *
* @return 
*/

	alg3::vec3 PlayerJoint::GetTranslation();
	
	//FROM BJOERN HARTMANN'S JOINT CODE

/**
 *  
 * stores the joint's translation into tx0
 *
*/

	void PlayerJoint::SaveInitialTranslation();

/**
 *  
 * calculates wm by multiplication of all parent's world matrices
 *
*/

	void PlayerJoint::SetWorldMatrix();
	
/**
 *  
 * rotates joint around the given axis by the given angle
 *NOTE:rotation is incremental
 *
* @param  Axis
* @param  const double angleDeg
*/

	void PlayerJoint::Rotate(alg3::vec3 Axis, const double angleDeg);

/**
 *  
 * NOTE:rotation is incremental
 *
* @param  x
* @param  double y
* @param  double z
* @param  double ang
*/

	void PlayerJoint::Rotate(double x, double y, double z, double ang);

/**
 *  
 * rotates joint around x-y-z axes by the given angles
 * NOTE:rotation is absolute
 *
* @param  x
* @param  double y
* @param  double z
*/

	void PlayerJoint::RotateAbsXYZ(double x, double y, double z);
	
/**
 *  
 * NOTE:rotation is incremental
 *
* @param  x
* @param  double y
* @param  double z
*/

	void PlayerJoint::RotateXYZ(double x, double y, double z);

/**
 *  
 * translates the joint by given x-y-z offsets
 *
* @param  x
* @param  double y
* @param  double z
*/

	void PlayerJoint::Translate(double x, double y, double z);


/**
 *  
 * NOTE:vector version 
 *
* @param  &v
*/

	void PlayerJoint::Translate(alg3::vec3 &v);
	
	//IMPORTANT:
	//this function does the final 3D joint drawing
	//the process is based on OpenGL glDrawElements function
	//Draw3 calculates the body vertex array that will be used by glDrawElements
	//to draw the body
	//so this functions does only some vertices rotations based on rotation matrices

/**
 *  
 * this function does the final 3D joint drawing
 *
* @param  v_abs
* @param  v_rel
* @param  n_abs
* @param  n_rel
* @param  groups[]
*/

	void PlayerJoint::Draw3(GLfloat* v_abs, GLfloat* v_rel, GLfloat* n_abs, GLfloat* n_rel,BodyVertexGroup* groups[]);
	
	//END OF BJOERN HARTMANN'S CODE

	
/**
 *  
 * this function translates the joint's rotations into matrices
* it MUST be called before Draw3 and after all the joint's rotations
 *
*/

	void PlayerJoint::ExecuteRotations();

/**
 *  
 * SIMPLE DRAWING:given a joint's name draws it using opengl
 * the result is a bone!
 * 
 *
* @param  jointname
*/

	void PlayerJoint::Draw(char* jointname);

/**
 *  
 * OLD 3D DRAWING:very slow but working.
 * not to be used anymore!
 * 
 *
* @param  jointname
*/

	void PlayerJoint::ModelDraw(char* jointname,
							point3d totaltranslation,
							GLfloat* vertex_new,
							GLfloat* normal_new,
							BodyVertexGroup* groups[]);


/**
 *  
 * given a joint's name sets its rotation 
 *
* @param  jointname
* @param  float x
* @param  float y
* @param  float z
*/

	void PlayerJoint::SetRotation(const char* jointname, float x, float y, float z);

/**
 *  
 * 
 *
* @param  jointname
* @param  float x
*/

	void PlayerJoint::SetRotationX(const char* jointname, float x);

/**
 *  
 * 
 *
* @param  jointname
* @param  float y
*/

	void PlayerJoint::SetRotationY(const char* jointname, float y);

/**
 *  
 * 
 *
* @param  jointname
* @param  float z
*/

	void PlayerJoint::SetRotationZ(const char* jointname, float z);
	

/**
 *  
 * gets joint's constraints, useful only for the pose editor
 *
* @return 
* @param  jointname
*/

	bool PlayerJoint::GetEnableX(char* jointname);

/**
 *  
 * 
 *
* @return 
* @param  jointname
*/

	bool PlayerJoint::GetEnableY(char* jointname);

/**
 *  
 * 
 *
* @return 
* @param  jointname
*/

	bool PlayerJoint::GetEnableZ(char* jointname);


/**
 *  
 * given a joint's name gets how much the joint is rotated
 *
* @param  jointname
* @param  x
* @param  y
* @param  z
*/

	void PlayerJoint::GetRotation(char* jointname, float* x, float* y, float* z);

/**
 *  
 * 
 *
* @return 
* @param  jointname
*/

	float PlayerJoint::GetRotationX(char* jointname);

/**
 *  
 * 
 *
* @return 
* @param  jointname
*/

	float PlayerJoint::GetRotationY(char* jointname);

/**
 *  
 * 
 *
* @return 
* @param  jointname
*/

	float PlayerJoint::GetRotationZ(char* jointname);

/**
 *  
 * exports joint's rotations. to be used in the pose editor
 *
* @param stream
* @param  jointname
*/

	void PlayerJoint::Write(FILE *stream, char* jointname);


/**
 *  
 * dumps all the joints' rotations of the right part of the body
 * into the left
*/

	void PlayerJoint::right2left();
	
/**
 * dumps all the joints' rotations of the right part of the body
 * into the right
*
*/

	void PlayerJoint::left2right();
};

#endif
