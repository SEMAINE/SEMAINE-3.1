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

/** it's the agent
*   the agent can be created, moved around, rotated
*   it has a status, can begin or stop talking
*   everything starts from this class
*
* @see Joint, BAPFrame
* @author Maurizio Mancini, manmau@yahoo.com
* @author Bjoern Hartmann, bjoern@graphics.cis.upenn.edu
* @author Massimo Bilvi, bilvi@gsmbox.it
* @author Stefano Pasquariello
* @version 1.0
*/

//***************************NOTE***********************************
//*  this class is the result of some different works              *
//*  written in both C and C++ code                                *
//*  it may be very difficult to understand how it works           *
//*  all the C functions are declared extern                       *
//*  and I tryed to change all the global variables into member    *
//*  variables                                                     *
//*  to do this I've had to modify C functions passing a lot       *
//*  of pointers in their argument list, but it works very well!   *
//******************************************************************

#if !defined(AFX_AGENT_H__26D28CE3_E1DF_4A48_805B_06F42C36A931__INCLUDED_)
#define AFX_AGENT_H__26D28CE3_E1DF_4A48_805B_06F42C36A931__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include <yvals.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <fstream>
#include <gl/glaux.h> //some 3D routines to draw spheres, etc
//#include "PerlinNoise.h"
#include "onlyforface/init.h" //initialization file for the face
#include "onlyforbody/PlayerJoint.h" //body joints
//#include "body files/BAPFrame.h" //BAP frames
#include "onlyforbody/BodyVertexGroup.h" //body vertices groups
#include <time.h>
#include "FAPFrame.h"
#include "FAPData.h"
#include "onlyforface/FaceData.h"
#include "onlyforface/ApplyFAPs.h"
#include "BAPData.h"
//#include "BAPConverter.h"
#include "onlyforbody/BodyData.h"
#include "PlayerPerlinNoise.h"

using namespace std;

/**
*
* class :AgentAspect
*
*/

class AgentAspect
{
public:

	/**
	* agent's name
	*/

	std::string name;

	std::string character;

	/**
	* constructor
	* 
	* @param name
	* @param character
	* @param realtime
	*/

	AgentAspect(std::string name,std::string character,bool realtime=false);

	/**
	* destructor
	* 
	*
	*/

	virtual ~AgentAspect();


	/**
	* this method 
	* 
	*
	*/

	void SetWriteAVI();


	/**
	* this method moves the agent to a given 3D position
	* 
	*
	* @param  x
	* @param  y
	* @param  z
	*/

	void MoveAgent(float x,float y,float z);

	/**
	* this method 
	* 
	*
	* @param  quantity
	*/

	void MoveForward(float quantity);

	/**
	* this method 
	* 
	*
	* @param  x
	* @param  y
	* @param  z
	*/

	void RotateOf(float x,float y,float z);


	/**
	* this method rotates agent by given angles
	* 
	*
	* @param  x
	* @param  y
	* @param  z
	*/

	void RotateAgent(float x,float y,float z);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  NameFile
	* @param  int fapframe_index
	*/

	int AgentAspect::ExportFace(std::string NameFile, int fapframe_index);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  NameFile
	* @param  int fapframe_index
	*/

	int AgentAspect::ExportFaceVRML(std::string NameFile, int fapframe_index);

	//assigns an animation file to the agent
	//until now an animation is stored into 3 files at the same time
	//one BAP + one FAP + one WAV file
	//so the given name is only a base name and the three file names
	//are the joining of the given name with ".bap", ".fap", ".wav"

	/**
	* this method 
	* 
	*
	* @return 
	* @param  name
	*/

	int AssignFile(char* name);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  fapbuffer
	*/

	int AssignFAPBuffer(char* fapbuffer);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  bapbuffer
	*/

	int AssignBAPBuffer(char* bapbuffer);

	//	int ReadFapFile(char *fapfilename);
	//	int ReadFapBuffer(char *fapfilename);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  fapnum
	*/

	float AgentAspect::CalibrateFaps(int fapnum);

	/**
	* this method 
	* 
	*
	*/

	void ReloadFile();

	/**
	* this method 
	* 
	*
	* @return 
	*/

	char* GetWavFileName();

	/**
	* this method 
	* 
	*
	* @return 
	*/

	std::string GetBaseFileName();

	/**
	* this method 
	* 
	*
	* @return 
	*/

	int GetTotalNumberOfFrames();

	/**
	* this method 
	* 
	*
	* @return 
	*/

	int GetFapFps();


	/**
	* this method tells the agent to begin or stop talking
	* 
	*
	*/

	void StartTalking();

	/**
	* this method 
	* 
	*
	*/

	void StopTalking();

	/**
	* this method 
	* 
	*
	* @param  audio
	*/

	void EnableAudio(bool audio);

	/**
	* this method 
	* 
	*
	*/

	void OneFrameUp();

	/**
	* this method 
	* 
	*
	*/

	void OneFrameDown();


	/**
	* this method draws the agent (OpenGL)
	* 
	*
	*/

	void draw();

	/**
	* this method 
	* 
	*
	* @param  fapframe_index
	* @param  int BAPFrame_index
	*/

	void drawGreta(int fapframe_index, int BAPFrame_index);

	/**
	* this method 
	* 
	*
	* @param  fapframe_index
	*/

	void display_teeths_eyes_tongue(int fapframe_index);


	/**
	* this method gets agent's status
	* 
	*
	* @return 
	*/

	char* GetStatus();

	/**
	* this method 
	* 
	*
	* @return 
	*/

	int GetCurrentFrame();

	/**
	* this method 
	* 
	*
	* @param  c
	*/

	void SetCurrentFrame(int c);


	/**
	* this method 
	* 
	*
	* @param f
	*/

	void LoadFAPFrame(FAPFrame *f);


	/**
	* this method switches the visualization of some agent's body parts
	* 
	*
	*/

	void SwitchSkin();

	/**
	* this method 
	* 
	*
	*/

	void SwitchIFAPU();

	/**
	* this method 
	* 
	*
	*/

	void SwitchEyes();

	/**
	* this method 
	* 
	*
	*/

	void SwitchSkeleton();

	/**
	* this method 
	* 
	*
	*/

	void SwitchHair();

	/**
	* this method 
	* 
	*
	*/

	void SwitchWire1();

	/**
	* this method 
	* 
	*
	*/

	void SwitchWire2();


	/**
	* this method gets the visualization state of some agent's body parts
	* 
	*
	* @return 
	*/

	bool GetSkinState();

	/**
	* this method 
	* 
	*
	* @return 
	*/

	bool GetEyesState();

	/**
	* this method 
	* 
	*
	* @return 
	*/

	bool GetBodyState();

	/**
	* this method 
	* 
	*
	* @return 
	*/

	bool GetHairState();

	/**
	* this method gets the agent's position and rotation
	* 
	*
	* @return 
	*/

	point3d GetPosition();

	/**
	* this method 
	* 
	*
	* @return 
	*/

	point3d GetRotation();


	/**
	* this method 
	* 
	*
	* @param  r
	* @param  g
	* @param  b
	*/

	void SetSkinColor(float r,float g,float b);


	/**
	* this method 
	* 
	*
	* @param  r
	* @param  g
	* @param  b
	*/

	void SetHairColor(float r,float g,float b);

	float skincolor_red,skincolor_green,skincolor_blue;
	float haircolor_red,haircolor_green,haircolor_blue;

	bool WRITEAVI;
	bool realtime;

private:


	float started_animation;
	bool audio,audioenabled;
	clock_t agent_created;
	float local_time;

	std::string openedfile;

	/**
	* FACE:OpenGL list to draw hair
	* 
	*/
	GLint hair_list;

	/**
	* this method 
	* 
	*
	* @return 
	*/
	GLint HairGen3DObjectList();
	/*
	GLint inf_lashes_list;//FACE:OpenGL list to draw inf lashes
	GLint sup_lashes_list;//FACE:OpenGL list to draw sup lashes
	GLint sim_inf_lashes_list;
	GLint sim_sup_lashes_list;
	*/
	bool flag_wire[2];//FACE:wireframe rendering
	bool flag_shade[3];//visualization of face skin,eyes(and teeth,thongue),body skin
	bool flag_hair;//visualization of hair
	bool flag_faceskintexturemapping;
	bool flag_skeleton;

	PlayerJoint* skeletonroot;//BODY:pointer to the body's root joint	
	PlayerJoint* pJoints[NUMJOINTS]; //BODY: direct access pointers to different joints

	point3d agentpos;//agent'sposition
	point3d agentrot;//agent's rotation
	point3d zero;//point (0,0,0)
	int AnimationActualFrame;


	float t_eyelid_rotation_l,b_eyelid_rotation_l;
	float t_eyelid_rotation_r,b_eyelid_rotation_r;

	int flag_rotation;

	float rot_head_pitch,rot_head_yaw,rot_head_roll; //new

	/////////////////////////////////

	GLuint head_texture;//array of agent's textures
	GLuint body_texture;//array of agent's textures
	GLuint eyes_texture;//array of agent's textures
	GLuint eyelashes_texture;//array of agent's textures
	GLuint lips_texture;//array of agent's textures

	int use_eyes_textures;//FACE:flag that activate eyes' texturing

	/**
	* this method 
	* 
	*
	* @param  fapframe_index
	*/

	void RenderEyes(int fapframe_index);

	/**
	* this method 
	* 
	*
	*/

	void RenderEyesTextures();

	/**
	* this method 
	* 
	*
	* @param  float c_x
	* @param  float c_y
	* @param  float c_z
	* @param  double r
	* @param  int n 
	*/

	void RenderSphere( float c_x, float c_y, float c_z, double r, int n );
public:


	ApplyFAPs *applyfaps;
	FaceData *facedata;
	FAPdata *FAPs;
	BAPData *BAPs;
	pointtype misc_point[SKIN_VERTEX_NUMBER2];
	GLint flag_surface1[69];
	GLint flag_surface2[12];
	std::string fap_filename, bap_filename, wav_filename;
	int enable_sound;

	int animationfps;
	int maxframes;

	/**
	* this method loads a single BAP frame into the body joints
	* 
	*
	* @param  index
	* @param  inputBAPFrame
	*/
	void LoadBAPFrame(int index, BAPFrame *inputBAPFrame);

private:

	/**
	* agent's status
	* 
	*/
	std::string status;

	/**
	* this method draws only the body
	* 
	*/
	void DrawBody();

	BodyData *bodydata;	

	int global_fps;

	/**
	* this method 
	* 
	*
	* @return 
	*/

	bool check_fps();


	/**
	* this method 
	* 
	*
	* @return 
	*/

	bool initFAPs(void);

	/**
	* this method 
	* 
	*
	* @return 
	*/

	bool init_face(void);

	/**
	* this method 
	* 
	*
	* @param  fapframe_index
	*/

	void DrawFace(int fapframe_index);


	/**
	* this method 
	* 
	*
	*/

	void DisplayFaceSkin();

	/**
	* this method 
	* 
	*
	*/

	void DisplayFaceSkinWithTexture();

	/**
	* this method initializes face structures and reads FAP file
	* 
	* @param buffer
	*/
	bool init_face(char *buffer);

	/**
	* this method loads texture
	* 
	* @return AUX_RGBImageRec
	* @param Filename
	*/

	AUX_RGBImageRec *LoadBMP(char *Filename);

	/**
	* this method activate texture's alpha channel
	* 
	* @return
	* @param src
	* @param size
	*/

	unsigned char* AddAlpha(unsigned char *src, int size);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  filename
	* @param  alpha
	* @param texture_dest
	*/
	int AgentAspect::LoadGLTextures(std::string filename,bool alpha,GLuint *texture_dest);

	/**
	* this method initializes textures, not used
	* 
	*/

	void InitGLTextures();
	BAPConverter conv;
	float PerlinStep1;
	float PerlinFactor1;
	float PerlinStep2;
	float PerlinFactor2;
	PlayerPerlinNoise noise1,noise2;
	int noisecounter1;
	int noisecounter2;

	/**
	* this method 
	* 
	*
	*/
	void RestartAnimation();
	float restartingtime;
	float stoppingtime;
};

#endif // !defined(AFX_AGENT_H__26D28CE3_E1DF_4A48_805B_06F42C36A931__INCLUDED_)
