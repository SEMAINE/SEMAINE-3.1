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

// EnvObject.cpp: implementation of the EnvObject class.
//
//////////////////////////////////////////////////////////////////////
#include "EnvObject.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


EnvObject::EnvObject(float x,float y, float z)
{
	xshoulder=-0.2;
	yshoulder=1.5;
	zshoulder=-0.02;
	xpos=x;
	ypos=y;
	zpos=z;
	r=0.0;
	g=0.5;
	b=0.0;
	a=0.5;
}

void EnvObject::SetShoulderPos(float sx, float sy, float sz){
	xshoulder=sx;
	yshoulder=sy;
	zshoulder=sz;	
}

float EnvObject::GetX(){
	return xpos;
}
float EnvObject::GetY(){
	return ypos;
}
float EnvObject::GetZ(){
	return zpos;
}

void EnvObject::Draw()
{
	glColor4f(r,g,b,a);
	glTranslatef(xpos,ypos,zpos);
	glBegin(GL_TRIANGLES);
	glVertex3f(-0.2,0.0,-0.2);
	glVertex3f(0.2,0.0,-0.2);
	glVertex3f(0.0,1.0,0.0);

	glVertex3f(-0.2,0.0,-0.2);
	glVertex3f(-0.2,0.0,0.2);
	glVertex3f(0.0,1.0,0.0);

	glVertex3f(0.2,0.0,-0.2);
	glVertex3f(-0.2,0.0,0.2);
	glVertex3f(0.0,1.0,0.0);
	glEnd();
	glTranslatef(-xpos,-ypos,-zpos);
}

void EnvObject::DrawDirection()
{
	glColor3f(1.0,0.0,0.0);
	glBegin(GL_LINE_STRIP);
	glVertex3f(xpos,ypos,zpos);
	glVertex3f(xshoulder,yshoulder,zshoulder);
//	glVertex3f(0.0,DELTA_Y,0);
	glEnd();
}

EnvObject::~EnvObject()
{

}
