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

#include <math.h>
#include "PlayerJoint.h"
#include <GL/glu.h>

int PlayerJoint::nsons=5;

const char allowed[40][255]={"r_shoulder","r_elbow","r_wrist","r_sternoclavicular",
								"r_acromioclavicular","r_pinky1","r_pinky2","r_pinky3",
								"r_ring1","r_ring2","r_ring3","r_middle1","r_middle2",
								"r_middle3","r_index1","r_index2","r_index3","r_thumb1",
								"r_thumb2","r_thumb3","l_shoulder","l_elbow","l_wrist",
								"l_sternoclavicular","l_acromioclavicular","l_pinky1",
								"l_pinky2","l_pinky3","l_ring1","l_ring2","l_ring3",
								"l_middle1","l_middle2","l_middle3","l_index1","l_index2",
								"l_index3","l_thumb1","l_thumb2","l_thumb3"};


PlayerJoint::PlayerJoint(char* jointname,std::string bonesfilename)
{	
	for(int i=0; i<nsons; i++)
		sons[i]=NULL;
	parent=NULL;
	
	strcpy(name,jointname);
	
	FILE *stream;
	
	char str[255];

	//float value;
	float tx,ty,tz;

	if( (stream  = fopen( bonesfilename.c_str(), "r" )) == NULL ){
		printf( "The file 'local_translations.txt' was not opened\n" );
		return;
	}
	
	//printf( "The file 'local_translations.txt' was opened\n" );

	m=alg3::identity3D();
	wm=alg3::identity3D();

	while(!feof(stream))
	{

		fscanf( stream, "%s", str );

		if(strcmp(str,name)==0)
		{
			fscanf( stream, "%s", str );
			tx=atof(str);
			translation.x=tx;
			fscanf( stream, "%s", str );
			ty=atof(str);
			translation.y=ty;
			fscanf( stream, "%s", str );
			tz=atof(str);
			translation.z=tz;
			Translate(tx,ty,tz);
		}
	}
	fclose(stream);

	enableX=false;
	enableY=false;
	enableZ=false;
}

PlayerJoint::~PlayerJoint()
{

}

void PlayerJoint::ResetRotation()
{
	rotation.x=0;
	rotation.y=0;
	rotation.z=0;
	for(int i=0; i<nsons; i++)
		if(sons[i]!=NULL)sons[i]->ResetRotation();
}

void PlayerJoint::ScanForSons(std::string bonesfilename,std::string hierarchyfilename){

	FILE *stream;
	
	char str1[255];
	char str2[255];
	char str3[255];
	
	//float value;
	
	if( (stream  = fopen( hierarchyfilename.c_str(), "r" )) == NULL ){
		printf( "The file 'hanim_Hierarchy.txt' was not opened\n" );
		return;
	}
	
	//printf( "The file 'hanim_Hierarchy.txt' was opened\n" );

	fscanf( stream, "%s", str2 );
	fscanf( stream, "%s", str3 );

	while (!feof( stream ) )
	{
		strcpy(str1,str2);
		strcpy(str2,str3);
		fscanf( stream, "%s", str3 );

		if(strcmp(str3,name)==0)
			if(strcmp(str2,":")==0)
				AddSon(new PlayerJoint(str1,bonesfilename));
	}
	fclose(stream);
	for(int i=0; i<nsons; i++)
		if(sons[i]!=NULL)sons[i]->ScanForSons(bonesfilename,hierarchyfilename);

}

void PlayerJoint::ScanForConstraints(){
	FILE *stream;
	
	char str[255];
	
	//float value;

	if( (stream  = fopen( "constraints.txt", "r" )) == NULL ){
		printf( "The file 'constraints.txt' was not opened\n" );
		return;
	}
	
	//printf( "The file 'constraints.txt' was opened\n" );


	while (!feof( stream ) )
	{
		fscanf( stream, "%s", str );
		if(strstr(name,str)!=NULL)
		{
			fscanf( stream, "%s", str );
			if(strchr(str,'x')!=NULL)
				enableX=true;
			if(strchr(str,'y')!=NULL)
				enableY=true;
			if(strchr(str,'z')!=NULL)
				enableZ=true;
		}
	}

	fclose(stream);
	for(int i=0; i<nsons; i++)
		if(sons[i]!=NULL)sons[i]->ScanForConstraints();

}

bool PlayerJoint::AddSon(PlayerJoint* son){
	int i;
	i=0;
	bool fatto;
	fatto=false;
	while((i<nsons)&&(fatto==false)){
		if(sons[i]==NULL){
			sons[i]=son;
			son->parent=this;
			fatto=true;
		}
		i++;
	}
	return fatto;
}


void PlayerJoint::Draw(char* jointname){
	gluSphere(gluNewQuadric(),1.0f,8,8);
	glRotatef(rotation.z,0.0f,0.0f,1.0f);
	glRotatef(rotation.y,0.0f,1.0f,0.0f);
	glRotatef(rotation.x,1.0f,0.0f,0.0f);
	if(strcmp(name,jointname)==0)glColor3f(0.0f,0.0f,0.8f);
	for(int i=0; i<nsons; i++)
		if(sons[i]!=NULL){
			glBegin(GL_TRIANGLES);
				glVertex3f( -0.5f, 0.0f, -0.5f);
				glVertex3f(0.5f,0.0f, 0.5f);
				glVertex3f( sons[i]->translation.x,sons[i]->translation.y,sons[i]->translation.z);
			glEnd();
			glTranslatef(sons[i]->translation.x, sons[i]->translation.y, sons[i]->translation.z);
			sons[i]->Draw(jointname);
			glTranslatef(-sons[i]->translation.x, -sons[i]->translation.y, -sons[i]->translation.z);
		}
	glRotatef(-rotation.x,1.0f,0.0f,0.0f);
	glRotatef(-rotation.y,0.0f,1.0f,0.0f);
	glRotatef(-rotation.z,0.0f,0.0f,1.0f);
	glColor3f(0.8f,0.8f,0.8f);
}

//bjoern
void PlayerJoint::SetWorldMatrix()
{
	if (parent!=0) {
		wm=(parent->wm)*m;
	}
	else {
		wm=m;
	}
	for(int i=0; i<nsons; i++)
		if(sons[i]!=NULL)sons[i]->SetWorldMatrix();

}

//bjoern
alg3::vec3 PlayerJoint::GetTranslation()
{
	alg3::vec3 v(0,0,0);
	return (wm*v);
	//return vec3(wm[0][3],wm[1][3],wm[2][3]);
}

//bjoern
void PlayerJoint::SaveInitialTranslation()
{
	tx0=GetTranslation();
	for(int i=0; i<nsons; i++)
		if(sons[i]!=NULL)
			sons[i]->SaveInitialTranslation();
}

//bjoern
void PlayerJoint::Translate(alg3::vec3 &v)
{
	m=alg3::translation3D(v)*m;

}

//bjoern
void PlayerJoint::Translate(double x, double y, double z)
{
	Translate(alg3::vec3(x,y,z));
}

//bjoern
void PlayerJoint::RotateAbsXYZ(double x, double y, double z)
{
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

//bjoern
void PlayerJoint::RotateXYZ(double x, double y, double z)
{

	// rotation order XYZ; 
	// modified: code pulled into single function on 3/23/04
	// also calculation of world matrices separated out - 
	// has to be called explicitly in agent's LoadBapFrame
	double tx=m[0][3];
	double ty=m[1][3];
	double tz=m[2][3];
	Translate(-tx,-ty,-tz);
	m=alg3::rotation3D(alg3::vec3(1.0,0.0,0.0),x)*m;
	m=alg3::rotation3D(alg3::vec3(0.0,1.0,0.0),y)*m;
	m=alg3::rotation3D(alg3::vec3(0.0,0.0,1.0),z)*m;
	Translate(tx,ty,tz);

	// for reading 169BAP files from Bjoern's HMS preprocessor,
	// rotation order and axis orientation are different.
	// use order XZY


}

//bjoern
void PlayerJoint::Rotate(double x, double y, double z, double ang)
{
	Rotate(alg3::vec3(x,y,z),ang);
}

//bjoern
void PlayerJoint::Rotate(alg3::vec3 Axis, const double angleDeg)
{
	double tx=m[0][3];
	double ty=m[1][3];
	double tz=m[2][3];
	Translate(-tx,-ty,-tz);
	m=alg3::rotation3D(Axis,angleDeg)*m;
	Translate(tx,ty,tz);
}

//bjoern
void PlayerJoint::Draw3(GLfloat* v_abs, GLfloat* v_rel, GLfloat* n_abs, GLfloat* n_rel, BodyVertexGroup* groups[])
{
		
	int whichVertex;
	int size=0;
	int i,j;

	for(j=0; j<NUMJOINTS;j++)
		if(groups[j]!=NULL)
		{
			if(strcmp(this->name,groups[j]->name)==0)
			{
				size=groups[j]->size;
				break;
			}
		}
	
	// ONLY CALCULATE IF POINTER NOT NULL AND SET NON_EMPTY
	if (size!=0 && groups[j]->changed)  {
		
		alg3::vec3 vNew,vOld;
		alg3::vec3 nOld,nNew;

		alg3::mat4 theM(wm);
		
		theM[0][3]=0.0;
		theM[1][3]=0.0;
		theM[2][3]=0.0;
		
		GLfloat* pVertex, *pNormal;

		for (int i=0; i<size; i++) {
			whichVertex = groups[j]->vertexarray[i];
			
			//setup pointers to input data
			pVertex=v_abs+whichVertex*3;
			pNormal=n_abs+whichVertex*3;
			
			// LOAD INITIAL VERTEX COORDS + NORMALS FROM vertices[] AND normals[]
			vOld=alg3::vec3(*(pVertex),*(pVertex+1),*(pVertex+2));
			nOld=alg3::vec3(*(pNormal),*(pNormal+1),*(pNormal+2));
			
			// TRANSFORM THE POINTS GIVEN THE CURRENT ANGLES IN THE SKELETON
			vNew=wm*(vOld - tx0); //new coord = world_matrix*local coordinate of point
			nNew=theM*(nOld); // new normal = world_matrix (without translation!) * old normal

			//WRITE NEW VLAUES INTO ARRAYS vertices2[] AND normals2[]
			//point to output arrays and write transformed data
			pVertex=v_rel+whichVertex*3;
			pNormal=n_rel+whichVertex*3;
			*(pVertex)=vNew[0];
			*(pVertex+1)=vNew[1];
			*(pVertex+2)=vNew[2];
			*(pNormal)=nNew[0];
			*(pNormal+1)=nNew[1];
			*(pNormal+2)=nNew[2];

		}
	
	}

	// RECURSE ON CHILDREN
	for(i=0; i<nsons; i++)
		if(sons[i]!=NULL)
			sons[i]->Draw3(v_abs,v_rel,n_abs,n_rel,groups);
}

void PlayerJoint::ExecuteRotations()
{
	
	RotateAbsXYZ(rotation.x,rotation.y,rotation.z);
	
	for(int i=0; i<nsons; i++)
		if(sons[i]!=NULL)
			sons[i]->ExecuteRotations();
}

void PlayerJoint::ModelDraw(char* jointname,point3d totaltranslation,GLfloat* vertex_new,GLfloat* normal_new,BodyVertexGroup* groups[])
{
	//bool found=false;
	//for(int j=0; j<40; j++)
		//if(strcmp(name,allowed[j])==0)found=true;
	//if(found==false)return;
//converts joint's name into standard c++ string 	
	//std::string namestring(name);
//gets the name of the set of the joint's vertexes
//	int*	jointVertices	= GetPolySet(name);
//and its size
//	int		size			= GetPolySetSize(name);
//local variables

	int whichVertex;
	int size=0;
	int i,j;

	//printf("\ndoing model draw for %s\n",this->name);

	for(j=0; j<NUMJOINTS;j++)
		if(groups[j]!=NULL)
		{
			if(strcmp(this->name,groups[j]->name)==0)
			{
				size=groups[j]->size;
				break;
			}
		}

	//if(j==NUMJOINTS)
		//printf("****** group not found!\n");

		point3d center;
		PlayerJoint* prevjoint;
		alg3::vec3 vertex_rot;
		alg3::vec3 normal_rot;
		alg3::mat3 rotmatx(alg3::vec3(0,0,0),alg3::vec3(0,0,0),alg3::vec3(0,0,0));
		alg3::mat3 rotmaty(alg3::vec3(0,0,0),alg3::vec3(0,0,0),alg3::vec3(0,0,0));
		alg3::mat3 rotmatz(alg3::vec3(0,0,0),alg3::vec3(0,0,0),alg3::vec3(0,0,0));
		alg3::mat3 tempmat(alg3::vec3(1,0,0),alg3::vec3(0,1,0),alg3::vec3(0,0,1));


	//adds local translation to total translation to know where in the world this joint is
		totaltranslation.x=totaltranslation.x+this->translation.x;
		totaltranslation.y=totaltranslation.y+this->translation.y;
		totaltranslation.z=totaltranslation.z+this->translation.z;

	//to rotate this joint correctly it has to rotate around local center by local angle
	//and repeat it through all the joint's parents

		prevjoint=this;

		center.x=totaltranslation.x;
		center.y=totaltranslation.y;
		center.z=totaltranslation.z;
	//it moves through all the parents from here to the root
		while(prevjoint!=NULL)
		{
	//calculates three rotation matrix, one for each axis

			rotmatx[0][0]=1;
			rotmatx[0][1]=0;
			rotmatx[0][2]=0;
			rotmatx[1][0]=0;
			rotmatx[1][1]=cos(prevjoint->rotation.x*0.017444);
			rotmatx[1][2]=-sin(prevjoint->rotation.x*0.017444);
			rotmatx[2][0]=0;
			rotmatx[2][1]=sin(prevjoint->rotation.x*0.017444);
			rotmatx[2][2]=cos(prevjoint->rotation.x*0.017444);

			rotmaty[0][0]=cos(prevjoint->rotation.y*0.017444);
			rotmaty[0][1]=0;
			rotmaty[0][2]=sin(prevjoint->rotation.y*0.017444);
			rotmaty[1][0]=0;
			rotmaty[1][1]=1;
			rotmaty[1][2]=0;
			rotmaty[2][0]=-sin(prevjoint->rotation.y*0.017444);
			rotmaty[2][1]=0;
			rotmaty[2][2]=cos(prevjoint->rotation.y*0.017444);

			rotmatz[0][0]=cos(prevjoint->rotation.z*0.017444);
			rotmatz[0][1]=-sin(prevjoint->rotation.z*0.017444);
			rotmatz[0][2]=0;
			rotmatz[1][0]=sin(prevjoint->rotation.z*0.017444);
			rotmatz[1][1]=cos(prevjoint->rotation.z*0.017444);
			rotmatz[1][2]=0;
			rotmatz[2][0]=0;
			rotmatz[2][1]=0;
			rotmatz[2][2]=1;

	//tempmat will store the final rotation matrix

			tempmat[0][0]=1;
			tempmat[0][1]=0;
			tempmat[0][2]=0;
			tempmat[1][0]=0;
			tempmat[1][1]=1;
			tempmat[1][2]=0;
			tempmat[2][0]=0;
			tempmat[2][1]=0;
			tempmat[2][2]=1;
			tempmat=tempmat*rotmatz;
			tempmat=tempmat*rotmaty;
			tempmat=tempmat*rotmatx;

	//now for all the joint's vertexes it does the rotation around the current centre
	//and by the current angle
			for (i=0; i<size; i++)
			{
		//finds the actual vertex
					whichVertex = groups[j]->vertexarray[i];
		//its coords
					vertex_rot=alg3::vec3(*(vertex_new+whichVertex*3),*(vertex_new+whichVertex*3+1),*(vertex_new+whichVertex*3+2));
		//and its normal vector
					normal_rot=alg3::vec3(*(normal_new+whichVertex*3),*(normal_new+whichVertex*3+1),*(normal_new+whichVertex*3+2));
		//moves it to the rotation's origin
					vertex_rot[0]=vertex_rot[0]-center.x;
					vertex_rot[1]=vertex_rot[1]-center.y;
					vertex_rot[2]=vertex_rot[2]-center.z;
		//does the rotation of vertex's coords
					vertex_rot=tempmat*vertex_rot;
		//and normal
					normal_rot=tempmat*normal_rot;
		//moves the vertex back to its original coord system
					vertex_rot[0]=vertex_rot[0]+center.x;
					vertex_rot[1]=vertex_rot[1]+center.y;
					vertex_rot[2]=vertex_rot[2]+center.z;
		//and updates it in the set of model's vertexes
					*(vertex_new+whichVertex*3)=vertex_rot[0];
					*(vertex_new+whichVertex*3+1)=vertex_rot[1];
					*(vertex_new+whichVertex*3+2)=vertex_rot[2];
		//then also updates normal vector
					*(normal_new+whichVertex*3)=normal_rot[0];
					*(normal_new+whichVertex*3+1)=normal_rot[1];
					*(normal_new+whichVertex*3+2)=normal_rot[2];
			}
	//if there's a parent vertex
			if(prevjoint!=NULL){
	//updates the centre of rotation to the parent's origin
				center.x=center.x-prevjoint->translation.x;
				center.y=center.y-prevjoint->translation.y;
				center.z=center.z-prevjoint->translation.z;
			}
	//and goes to the next rotation
			prevjoint=prevjoint->parent;
		}
	
//this joint is finished so we go down to the joint's sons
	for(i=0; i<nsons; i++)
		if(sons[i]!=NULL)
			sons[i]->ModelDraw(jointname,totaltranslation,vertex_new,normal_new,groups);

}

void PlayerJoint::SetRotation(const char* jointname, float x, float y, float z){
	if(strcmp(jointname,name)==0){
		if(enableX)
		{
			rotation.x=x;
		}
		else rotation.x=0;
		if(enableY)
		{
			rotation.y=y;
		}
		else rotation.y=0;
		if(enableZ)
		{
			rotation.z=z;
		}
		else rotation.z=0;
		//bjoern
	}else
		for(int i=0; i<nsons; i++)
			if(sons[i]!=NULL)sons[i]->SetRotation(jointname,x,y,z);
}

void PlayerJoint::GetRotation(char* jointname, float* x, float* y, float* z){
	if(strcmp(jointname,name)==0){
		*x=rotation.x;
		*y=rotation.y; 
		*z=rotation.z;
	}else
		for(int i=0; i<nsons; i++)
			if(sons[i]!=NULL)sons[i]->GetRotation(jointname,x,y,z);
}

bool PlayerJoint::GetEnableX(char* jointname){
	if(strcmp(jointname,name)==0){
		return enableX;
	}else
		for(int i=0; i<nsons; i++)
			if(sons[i]!=NULL)
				if (sons[i]->GetEnableX(jointname)==true) return true;
	return false;
}
bool PlayerJoint::GetEnableY(char* jointname){
	if(strcmp(jointname,name)==0){
		return enableY;
	}else
		for(int i=0; i<nsons; i++)
			if(sons[i]!=NULL)
				if (sons[i]->GetEnableY(jointname)==true) return true;
	return false;
}
bool PlayerJoint::GetEnableZ(char* jointname){
	if(strcmp(jointname,name)==0){
		return enableZ;
	}else
		for(int i=0; i<nsons; i++)
			if(sons[i]!=NULL)
				if (sons[i]->GetEnableZ(jointname)==true) return true;
	return false;
}

void PlayerJoint::Write(FILE *stream, char* jointname){
	
	char buffer[255];
	
	if(strcmp(jointname,name)==0){
		if(strstr(name,"distal_tip")==NULL){
			sprintf(buffer,"%s %f %f %f\n",name,rotation.x,rotation.y,rotation.z);
			fprintf(stream,"%s",buffer);
		}
		for(int i=0; i<nsons; i++)
			if(sons[i]!=NULL)sons[i]->Write(stream,sons[i]->name);
	}
	else
		for(int i=0; i<nsons; i++)
			if(sons[i]!=NULL)sons[i]->Write(stream,jointname);
}

void PlayerJoint::right2left(){
	char aux[255];
	PlayerJoint* root;
	if(strstr(name,"r_")!=NULL){
		strcpy(aux,name);
		strncpy(aux,"l_",2);
		root=this;
		while(root->parent!=NULL)
			root=root->parent;
		root->SetRotation(aux,rotation.x,-rotation.y,-rotation.z);
	}
	for(int i=0; i<nsons; i++)
	if(sons[i]!=NULL)sons[i]->right2left();
}

void PlayerJoint::left2right(){
	char aux[255];
	PlayerJoint* root;
	if(strstr(name,"l_")!=NULL){
		strcpy(aux,name);
		strncpy(aux,"r_",2);
		root=this;
		while(root->parent!=NULL)
			root=root->parent;
		root->SetRotation(aux,rotation.x,-rotation.y,-rotation.z);
	}
	for(int i=0; i<nsons; i++)
	if(sons[i]!=NULL)sons[i]->left2right();
}

void PlayerJoint::SetRotationX(const char* jointname, float x){
	if(strcmp(jointname,name)==0){
		if(enableX)
		{
			rotation.x=x;
			
		}
		else rotation.x=0;
	}else
		for(int i=0; i<nsons; i++)
			if(sons[i]!=NULL)sons[i]->SetRotationX(jointname,x);
}

void PlayerJoint::SetRotationY(const char* jointname, float y){
	if(strcmp(jointname,name)==0){
		if(enableY)
		{
			rotation.y=y;
			
		}
		else rotation.y=0;
	}else
		for(int i=0; i<nsons; i++)
			if(sons[i]!=NULL)sons[i]->SetRotationY(jointname,y);
}

void PlayerJoint::SetRotationZ(const char* jointname, float z){
	if(strcmp(jointname,name)==0){
		if(enableZ)
		{
			rotation.z=z;
			
		}
		else rotation.z=0;
	}else
		for(int i=0; i<nsons; i++)
			if(sons[i]!=NULL)sons[i]->SetRotationZ(jointname,z);
}

float PlayerJoint::GetRotationX(char* jointname)
{
	if(strcmp(jointname,name)==0)
		return rotation.x;
	else
	{
		float res;
		res=0;
		for(int i=0; i<nsons; i++)
			if(sons[i]!=NULL)res=res+sons[i]->GetRotationX(jointname);
		return res;
	}
}

float PlayerJoint::GetRotationY(char* jointname)
{
	if(strcmp(jointname,name)==0)
		return rotation.y;
	else
	{
		float res;
		res=0;
		for(int i=0; i<nsons; i++)
			if(sons[i]!=NULL)res=res+sons[i]->GetRotationY(jointname);
		return res;
	}
}

float PlayerJoint::GetRotationZ(char* jointname)
{
	if(strcmp(jointname,name)==0)
		return rotation.z;
	else
	{
		float res;
		res=0;
		for(int i=0; i<nsons; i++)
			if(sons[i]!=NULL)res=res+sons[i]->GetRotationZ(jointname);
		return res;
	}
}

bool PlayerJoint::LoadGLWorldMatrix(const char *jointname)
{
	int i;
	bool ret=false;
	
	if(strcmp(jointname,name)==0) {
	
		float glWm[16];
		for(int col=0; col<4; col++) {
			for(int row=0;row<4; row++) {
				glWm[col*4+row]=wm[row][col];
				
			}
		}
		glMultMatrixf(&glWm[0]);
		return true;
	} else  {
		for(i=0; i<nsons; i++)
			if(sons[i]!=NULL) {
				ret=sons[i]->LoadGLWorldMatrix(jointname);
				if (ret) return true;
			}
	}
	return false;
}

void PlayerJoint::GetPointer(const char* jointname, PlayerJoint **p){
	if(strcmp(jointname,name)==0)
		*p=this;
	else
	{
		for(int i=0; i<nsons; i++)
			if(sons[i]!=NULL) sons[i]->GetPointer(jointname,p);
	}
}


void PlayerJoint::SetChanged(bool c) {
	changed=c;
}
bool PlayerJoint::GetChanged() {
	return changed;
}
void PlayerJoint::SetChangedRecursive(bool c) {
	changed=c;

	{
		for(int i=0; i<nsons; i++)
			if(sons[i]!=NULL) sons[i]->SetChangedRecursive(c);
	}
}

void PlayerJoint::PropagateChanged(bool c) {
	if(c)
		changed=c;

	{
		for(int i=0; i<nsons; i++)
			if(sons[i]!=NULL) sons[i]->PropagateChanged(changed);
	}
}