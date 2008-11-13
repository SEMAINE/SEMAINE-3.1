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

#include ".\worldobject.h"
#include "IniManager.h"
#include "XMLGenericParser.h"

extern IniManager inimanager;

WorldObject::WorldObject(std::string name,std::string objectshape)
{
	this->name=name;
	this->shape=objectshape;
	x=0;
	y=0;
	z=0;
	rotx=0;
	roty=0;
	rotz=0;
	num_v=-1;
	num_n=-1;
	num_uv=-1;
	num_t=-1;
	texture=0;
	ReadMesh(inimanager.Program_Path+"objects/"+objectshape+".xml");

	material_ambient[0]=0.4f;
	material_ambient[1]=0.4f;
	material_ambient[2]=0.4f;
	material_ambient[3]=1.0f;
	material_specular[0]=0.35f;
	material_specular[1]=0.35f;
	material_specular[2]=0.35f;
	material_specular[3]=1.0f;
	material_shininess[0]=20.0f;

	linearmapping=false;
}

WorldObject::~WorldObject(void)
{
}

void WorldObject::Move(float x,float y,float z)
{
	this->x=x;
	this->y=y;
	this->z=z;
}

void WorldObject::Rotate(float x,float y,float z)
{
	this->rotx=x;
	this->roty=y;
	this->rotz=z;
}

void WorldObject::draw(void)
{
	if(num_t==-1)
		return;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, &material_ambient[0]); 
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , &material_specular[0]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &material_shininess[0]); 

	glPushMatrix();

	glTranslatef(x,y,z);
	glRotatef(rotx,1,0,0);
	glRotatef(roty,0,1,0);
	glRotatef(rotz,0,0,1);

	int i;
	if(texture!=0)
	{
		glColor3f(1,1,1);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture);
	}

	glBegin(GL_TRIANGLES);
	for(i=0;i<num_t;i++)
	{
		glNormal3f(normals[triangles[i].n1].x,normals[triangles[i].n1].y,normals[triangles[i].n1].z);
		glTexCoord2f(uvs[triangles[i].uv1].x,uvs[triangles[i].uv1].y);
		glVertex3f(vertices[triangles[i].v1].x,vertices[triangles[i].v1].y,vertices[triangles[i].v1].z);

		glNormal3f(normals[triangles[i].n2].x,normals[triangles[i].n2].y,normals[triangles[i].n2].z);
		glTexCoord2f(uvs[triangles[i].uv2].x,uvs[triangles[i].uv2].y);
		glVertex3f(vertices[triangles[i].v2].x,vertices[triangles[i].v2].y,vertices[triangles[i].v2].z);

		glNormal3f(normals[triangles[i].n3].x,normals[triangles[i].n3].y,normals[triangles[i].n3].z);
		glTexCoord2f(uvs[triangles[i].uv3].x,uvs[triangles[i].uv3].y);
		glVertex3f(vertices[triangles[i].v3].x,vertices[triangles[i].v3].y,vertices[triangles[i].v3].z);
	}
	glEnd();

	if(texture!=0)
	{
		glDisable(GL_TEXTURE_2D);
	}

	glPopMatrix();

}

AUX_RGBImageRec *WorldObject::LoadBMP(char *Filename)
{
	FILE *File = NULL;

	if( !Filename )
		return NULL;

	File = fopen(Filename,"r");

	if( File )
	{
		fclose(File);
		return auxDIBImageLoad(Filename);
	}

	return NULL;
}

#define MIN_ALPHA_VALUE 128

//********************************************
// AddAlphaLayer
//********************************************
unsigned char* WorldObject::AddAlpha(unsigned char *src, int size) // 0 - 255
{
	// Alloc memory
	unsigned char *pData;
	int i;

	pData= (unsigned char*)malloc(size*4);
	if(pData == NULL)
	{
		return 0;
	}

	for(i=0;i<size;i++)
	{
		pData[4*i+0] = src[3*i+0];
		pData[4*i+1] = src[3*i+1];
		pData[4*i+2] = src[3*i+2];
		if (src[3*i+0]>MIN_ALPHA_VALUE&&
			src[3*i+1]>MIN_ALPHA_VALUE&&
			src[3*i+2]>MIN_ALPHA_VALUE)
				pData[4*i+3] = 0x00;
		else
				pData[4*i+3] = 0xFF;

	}
	
	return pData;
}

int WorldObject::LoadGLTextures(std::string filename,bool alpha,GLuint *texture_dest)
{
//	int i;

	AUX_RGBImageRec *TextureImage;

	TextureImage=0;

	// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
	if (TextureImage = LoadBMP((char*)filename.c_str()))
	{
		if (alpha)
		{
			TextureImage->data=AddAlpha(TextureImage->data,TextureImage->sizeX*TextureImage->sizeY);

			glGenTextures(1, texture_dest);
			// Typical Texture Generation Using Data From The Bitmap
			glBindTexture(GL_TEXTURE_2D, *texture_dest);
			glTexImage2D(GL_TEXTURE_2D, 0, 4, TextureImage->sizeX, TextureImage->sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, TextureImage->data);
		}
		else
		{
			//qui mette la texture sulle labbra
			glGenTextures(1, texture_dest);
			// Typical Texture Generation Using Data From The Bitmap
			glBindTexture(GL_TEXTURE_2D, *texture_dest);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage->sizeX, TextureImage->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage->data);
		}		
		if(linearmapping==true)
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		}
		free(TextureImage->data);
		free(TextureImage);

		return 1;
	}
	else
		return 0;
}

void WorldObject::InitGLTextures()
{

    glEnable(GL_DEPTH_TEST); // enable the z-buffer
    glEnable(GL_TEXTURE_2D); // Enable Texture Mapping

}

int WorldObject::ReadMesh(std::string filename)
{
	XMLGenericParser p;
	XMLGenericTree *t,*node;
	std::list<XMLGenericTree *>::iterator iter;
	p.SetValidating(true);
	t=p.ParseFile(filename);
	if(t==0)
		return 0;

	num_v=(int)t->GetAttributef("num_v");
	num_n=(int)t->GetAttributef("num_n");
	num_uv=(int)t->GetAttributef("num_uv");
	num_t=(int)t->GetAttributef("num_t");

	int i;

	threeDpoint *point;
	for(i=0;i<num_v;i++)
	{
		point=new threeDpoint;
		vertices.push_back(*point);
	}

	for(i=0;i<num_n;i++)
	{
		point=new threeDpoint;
		normals.push_back(*point);
	}

	twoDpoint *point2d;
	for(i=0;i<num_uv;i++)
	{
		point2d=new twoDpoint;
		uvs.push_back(*point2d);
	}

	triangle *tri;
	for(i=0;i<num_t;i++)
	{
		tri=new triangle;
		triangles.push_back(*tri);
	}


	node=t->FindNodeCalled("VERTICES");
	for(iter=node->child.begin();iter!=node->child.end();iter++)
	{
		vertices[(int)((*iter)->GetAttributef("n"))].x=(*iter)->GetAttributef("x");
		vertices[(int)((*iter)->GetAttributef("n"))].y=(*iter)->GetAttributef("y");
		vertices[(int)((*iter)->GetAttributef("n"))].z=(*iter)->GetAttributef("z");
	}
	node=t->FindNodeCalled("NORMALS");
	for(iter=node->child.begin();iter!=node->child.end();iter++)
	{
		normals[(int)((*iter)->GetAttributef("n"))].x=(*iter)->GetAttributef("x");
		normals[(int)((*iter)->GetAttributef("n"))].y=(*iter)->GetAttributef("y");
		normals[(int)((*iter)->GetAttributef("n"))].z=(*iter)->GetAttributef("z");
	}
	node=t->FindNodeCalled("UVS");
	for(iter=node->child.begin();iter!=node->child.end();iter++)
	{
		uvs[(int)((*iter)->GetAttributef("n"))].x=(*iter)->GetAttributef("x");
		uvs[(int)((*iter)->GetAttributef("n"))].y=(*iter)->GetAttributef("y");
	}
	node=t->FindNodeCalled("TRIANGLES");
	for(iter=node->child.begin();iter!=node->child.end();iter++)
	{
		triangles[(int)((*iter)->GetAttributef("n"))].v1=(int)(*iter)->GetAttributef("v1");
		triangles[(int)((*iter)->GetAttributef("n"))].v2=(int)(*iter)->GetAttributef("v2");
		triangles[(int)((*iter)->GetAttributef("n"))].v3=(int)(*iter)->GetAttributef("v3");
		triangles[(int)((*iter)->GetAttributef("n"))].n1=(int)(*iter)->GetAttributef("n1");
		triangles[(int)((*iter)->GetAttributef("n"))].n2=(int)(*iter)->GetAttributef("n2");
		triangles[(int)((*iter)->GetAttributef("n"))].n3=(int)(*iter)->GetAttributef("n3");
		triangles[(int)((*iter)->GetAttributef("n"))].uv1=(int)(*iter)->GetAttributef("uv1");
		triangles[(int)((*iter)->GetAttributef("n"))].uv2=(int)(*iter)->GetAttributef("uv2");
		triangles[(int)((*iter)->GetAttributef("n"))].uv3=(int)(*iter)->GetAttributef("uv3");
	}
	node=t->FindNodeCalled("TEXTUREFILE");
	if(node->GetAttribute("path")!="")
		LoadGLTextures(inimanager.Program_Path+"textures/"+node->GetAttribute("path"),0,&texture);
	delete t;
	return 1;
}
