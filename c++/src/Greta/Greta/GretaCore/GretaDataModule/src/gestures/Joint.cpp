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

// Joint.cpp: implementation of the Joint class.
//
//////////////////////////////////////////////////////////////////////
#include "Joint.h"
//#include "jackieGroups.h"
/**@#-*/
using namespace GestureSpace;
/**@#+*/

//#include "head_displacement.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Joint::Joint()
{
	parent=0;
	children.clear();
	m=alg3::identity3D();
	wm=alg3::identity3D();
	rotate_order="xyz";
	name="";
	mRotX=0.0;
	mRotY=0.0;
	mRotZ=0.0;
	output=false;
}

Joint::~Joint()
{
	children.clear();
}

void Joint::Print()
{
	std::cout << "MATRIX "<<name<<std::endl;
	std::cout << "       "<<mRotX<<", "<<mRotY<<", "<<mRotZ<<endl;
/*
	for (int i=0; i<4; i++) {
		alg3::vec4 v=m[i];
		for (int j=0; j<4; j++) {
			std::cout << v[j] <<" ";
		}
		std::cout << std::endl;
	}
	alg3::vec3 testv=wm * alg3::vec3(0.0,0.0,0.0) ;
	std::cout<< "Point (0,0,0) now at: ("<<testv[0]<<", "<<testv[1]<<", "<<testv[2]<<")"<<std::endl;
	std::cout << "Children:"<<std::endl;
*/	
	JointVector::iterator iter;
	for (iter=children.begin();iter!=children.end();iter++) {
		(*iter)->Print();
	}
}

void Joint::SetWorldMatrix()
{
	if (parent!=0) {
		wm=(parent->wm)*m;
		wmi=m*parent->wmi;
	}
	else {
		wm=m;
		wmi=m;
	}

	JointVector::iterator iter;
	for (iter=children.begin();iter!=children.end();iter++) {
		(*iter)->SetWorldMatrix();
	}

}

void Joint::Translate(vec3 &v)
{
	m=translation3D(v)*m;
	SetWorldMatrix();
}

void Joint::Translate(double x, double y, double z)
{
	Translate(vec3(x,y,z));
}

void Joint::TranslateAbs(double x, double y, double z) {
	m[0][3]=x;
	m[1][3]=y;
	m[2][3]=z;
}
void Joint::Rotate(vec3 Axis, const double angleDeg)
{
	double tx=m[0][3];
	double ty=m[1][3];
	double tz=m[2][3];
	Translate(-tx,-ty,-tz);
	m=rotation3D(Axis,angleDeg)*m;
	Translate(tx,ty,tz);
	SetWorldMatrix();
}

void Joint::Rotate(double x, double y, double z, double ang)
{
	Rotate(vec3(x,y,z),ang);
}

void Joint::PrintPoint(double x, double y, double z)
{
	vec4 v(x,y,z,1);
	vec4 vp=wm*v;
	if (output) std::cout<<"("<<x<<", "<<y<<", "<<z<<") -> ("<<vp[0]<<", "<<vp[1]<<", "<<vp[2]<<")\n";
}

void Joint::RotateXYZ(double x, double y, double z)
{
	mRotX += x;
	mRotY += y; 
	mRotZ += z;
	for(int i=0; i<3; i++) {
		std::string this_rot= rotate_order.substr(i,1);
		if ((this_rot.compare("x")==0) && (x!=0.0)) {
			Rotate(1.0,0.0,0.0,x);
		} else if ((this_rot.compare("y")==0) && (y!=0.0)) {
			Rotate(0.0,1.0,0.0,y);
		} else if ((this_rot.compare("z")==0) && (z!=0.0)) {
			Rotate(0.0,0.0,1.0,z);
		}
	}
}


void Joint::SetParent(Joint *p)
{
	parent=p;
	p->children.push_back(this);
}

vec3 Joint::GetTranslation()
{
	vec3 v(0,0,0);
	return (wm*v);
	//return vec3(wm[0][3],wm[1][3],wm[2][3]);
}

void Joint::RotateAbsXYZ(double x, double y, double z)
{
	mRotX=0.0;
	mRotY=0.0;
	mRotZ=0.0;
	//set all rotation entries in matrix to 0
	for(int i=0; i<=2; i++) {
		for (int j=0; j<=2; j++) {
			if (i==j) 
				m[i][j]=1.0; 
			else 
				m[i][j]=0.0;
		}
	}
	//then rotate
	RotateXYZ(x,y,z);
}

void Joint::Draw()
{	
	glPushMatrix();
	glTranslatef(m[0][3],m[1][3],m[2][3]);
	
	// :TRICKY: TRANSPOSED MATRIX TYPES!ROTATE ORDER REVERSED!
	for(int i=2; i>=0; i--) {
		std::string this_rot= rotate_order.substr(i,1);
		if ((this_rot.compare("x")==0) && (mRotX!=0.0)) {
			glRotatef(mRotX,1.0,0.0,0.0);
			
		} else if ((this_rot.compare("y")==0) && (mRotY!=0.0)) {
			glRotatef(mRotY,0.0,1.0,0.0);
			
		} else if ((this_rot.compare("z")==0) && (mRotZ!=0.0)) {
			glRotatef(mRotZ,0.0,0.0,1.0);
			
		}
		
	}
	
	//:TRICKY: two draw routines
	DrawLocal();
	//DrawLocal3();
	
	JointVector::iterator iter;
	for (iter=children.begin();iter!=children.end();iter++) {
		(*iter)->Draw();
	}
	glPopMatrix();

}

//////////////////////////////////////////////////////////////////////
// DRAW SIMPLE JOINT-AND-BONES SKELETON
//////////////////////////////////////////////////////////////////////
void Joint::DrawLocal()
{
	
	glutSolidCube(0.015);
	
	glEnable(GL_NORMALIZE);
	JointVector::iterator iter;
	vec3 v1,v2,v3,n;
	float neg=-0.01; 
	float pos=0.01;
		
	for (iter=children.begin();iter!=children.end();iter++) {
		
		float sign=1.0;
		if ((*iter)->m[1][3] < 0) sign=(-1.0);
		
		glBegin(GL_TRIANGLES);
		
		
		v1=vec3(neg,0,pos);
		v2=vec3(pos,0,pos);
		v3=vec3((*iter)->m[0][3],(*iter)->m[1][3],(*iter)->m[2][3]);
		n=(sign)*((v1-v3)^(v2-v3));
		
		glNormal3f(n[0],n[1],n[2]);
		glVertex3f(v1[0],v1[1],v1[2]);
		glVertex3f(v2[0],v2[1],v2[2]);
		glVertex3f(v3[0],v3[1],v3[2]);
		//glEnd();

		//glBegin(GL_TRIANGLES);
		v1=vec3(pos,0,pos);
		v2=vec3(pos,0,neg);
		n=(sign)*((v1-v3)^(v2-v3));
		
		glNormal3f(n[0],n[1],n[2]);
		glVertex3f(v1[0],v1[1],v1[2]);
		glVertex3f(v2[0],v2[1],v2[2]);
		glVertex3f(v3[0],v3[1],v3[2]);
		//glEnd();
		
	//	glBegin(GL_TRIANGLES);
		v1=vec3(pos,0,neg);		
		v2=vec3(neg,0,neg);
		n=(sign)*((v1-v3)^(v2-v3));
		
		glNormal3f(n[0],n[1],n[2]);
		glVertex3f(v1[0],v1[1],v1[2]);
		glVertex3f(v2[0],v2[1],v2[2]);
		glVertex3f(v3[0],v3[1],v3[2]);
		//glEnd();
		
		//glBegin(GL_TRIANGLES);
		v1=vec3(neg,0,neg);
		v2=vec3(neg,0,pos);		
		n=(sign)*((v1-v3)^(v2-v3));
		
		glNormal3f(n[0],n[1],n[2]);
		glVertex3f(v1[0],v1[1],v1[2]);
		glVertex3f(v2[0],v2[1],v2[2]);
		glVertex3f(v3[0],v3[1],v3[2]);
		glEnd();			
	}
	
}

void Joint::RotateAbsXYZ(BAPFrame &f)
{
	
	string x_str = name+".rotateX";
	string y_str = name+".rotateY";
	string z_str = name+".rotateZ";
	float x_val, y_val, z_val;
	
	if (f.GetMask(x_str))
		x_val = f.GetBAPAngle(x_str,degrees);
	else
		x_val=0.0;

	if (f.GetMask(y_str))
		y_val = f.GetBAPAngle(y_str,degrees);
	else
		y_val=0.0;	
	
	if (f.GetMask(z_str))
		z_val = f.GetBAPAngle(z_str,degrees);
	else
		z_val=0.0;
	
	RotateAbsXYZ(x_val,y_val,z_val);


}

void Joint::LoadBAPs(BAPFrame &f)
{
	RotateAbsXYZ(f);
	JointVector::iterator iter;
	for (iter=children.begin();iter!=children.end();iter++) {
		(*iter)->LoadBAPs(f);
	}
}

//////////////////////////////////////////////////////////////////////
// DRAW THE FIGURE
//////////////////////////////////////////////////////////////////////

void Joint::DrawLocal2()
{
/*
	int* pFaces=GetFaceSet(name);	
	int size=GetFaceSetSize(name);
	
	// exit if NULL Pointer retrurned
	if (pFaces==NULL)  {		
		return;
	}
	
	int whichFace;
	int whichVertex[3];
	
	glBegin(GL_TRIANGLES);
	for (int i=0; i<size; i++) {
		whichFace      = *(pFaces+i); // correct!
		whichVertex[0] = indices[whichFace*3 +0];
		whichVertex[1] = indices[whichFace*3 +1];
		whichVertex[2] = indices[whichFace*3 +2];
		glArrayElement(whichVertex[0]);
		glArrayElement(whichVertex[1]);
		glArrayElement(whichVertex[2]);
	}
	glEnd();
*/
}

extern "C" 
{
	float* find_nearest_vertex(float*,float**);
}

//////////////////////////////////////////////////////////////////////
// DRAW THE FIGURE3: 
// - recalculate global coordinates for vertices
//////////////////////////////////////////////////////////////////////




void Joint::SaveInitialTranslation()
{
	tx0=GetTranslation();
	JointVector::iterator iter;
	for (iter=children.begin();iter!=children.end();iter++) {
		(*iter)->SaveInitialTranslation();
	}
	
}
