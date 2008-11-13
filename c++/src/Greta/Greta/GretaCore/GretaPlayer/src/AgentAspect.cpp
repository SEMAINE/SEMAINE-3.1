//Copyright 1999-2008 Catherine Pelachaud - c.pelachaud@iut.univ-paris8.fr
//
//This file is part of Greta.
//
//Greta is free software; you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation; either versaudioenabledion 2 of the License, or
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

// AgentAspect.cpp: implementation of the AgentAspect class.
//
//////////////////////////////////////////////////////////////////////

#pragma warning( disable : 4305 )
#include "AgentAspect.h"
//#include "algebra3.h"
#include <gl/glaux.h>
#include <gl/glext.h>
#include <math.h>
#include <crtdbg.h>
#include "IniManager.h"

//#define MAX(_x, _y) ((_x > _y) ? _x : _y))

extern IniManager inimanager;


//#include "audio.h"

//#include "../OOGretaEngine/FaceEngine/defines.h"


//agent.h was here

#include "onlyforface/queue.h"
//#include "GestureKinematics.h"

//2 includes below are for new skeleton initialization
#include <string>
#include <BAPConverter.h>

#define G_NRESOLUTION			50


//FAPU di GRETA
#define GRETA_LH 239.963974   //Lip Height
#define GRETA_UL 479.682892   //Upper Lip Height
#define GRETA_LL 465.752380   //Lower Lip Height
#define GRETA_JH 166.034653	  //Jaw
#define GRETA_MNS 153.415222  //Distance from the nose
#define GRETA_LW 83.249252    //LIP Width
#define GRETA_ULP -340.974121 //Upper Lip protrusion
#define GRETA_LLP 753.426880  //Lower Lip protrusion


extern GLfloat **vdata;
extern	"C" int isEmptyQ(void);
extern	"C" queue* headQ(void);
extern	"C" int load_wave(char *,char*);
extern	"C" int play_wave(int,char*);
extern	"C" void stop_wave(char*);
extern	"C" void close_wave(char*);
extern	"C" int get_wave_time(char*);
extern  "C" int get_audio_len(char*);
//extern	GLint HairGen3DObjectList(void);
extern	GLint Gen_Sup_lashes();
extern	GLint Gen_Inf_lashes();
extern	GLint Gen_Sim_Sup_lashes();
extern	GLint Gen_Sim_Inf_lashes();
//extern	void create_skin_surface_list(pointtype*);
extern	void create_misc_surface_list(pointtype*);
extern	void initialize_array_misc_point(pointtype*);
extern	void copy_vdata2_into_misc_coordinate(pointtype*);
extern	void create_and_initialize_array_misc_surfaces_indices(void);
extern	void initialize_misc_normals(pointtype*);
extern	void initializes_misc_surfaces(int surface_number,pointtype*);
extern	void InitKeyFrames(int);
extern	int header_file_init(char *filename,int*,int*);
extern	int header_buffer_init(char *filename,int*,int*);
extern	void initialize_array_flag_surface1_flag_surface2(GLint*,GLint*);
extern	void calculate_misc_normals_at_runtime(pointtype*,GLint*);
extern	void display_teeths_eyes_tongue(FaceData* facedata,FAPdata *FAPs,int fapframe_index,GLuint* texture,int use_eyes_textures,pointtype* misc_point);

using namespace std;

AgentAspect::AgentAspect(std::string name,std::string character, bool realtime)
{
	this->realtime=realtime;

	int i;

	this->name=name;
	this->character=character;

	flag_faceskintexturemapping=true;

	agent_created=clock();

	facedata=new FaceData();
	FAPs = new FAPdata();
	BAPs = new BAPData();

	//int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
	//tmpFlag |= _CRTDBG_CHECK_ALWAYS_DF;
	//_CrtSetDbgFlag( tmpFlag );

	if(character=="")
	{
		facedata->ReadFaceGeometry(inimanager.GetValueString("CHARACTER_SPEAKER"));
		character=inimanager.GetValueString("CHARACTER_SPEAKER");
	}
	else
	{
		facedata->ReadFaceGeometry(character);
	}

	

	this->applyfaps = new ApplyFAPs();
	this->applyfaps->init(this);
	
	if((facedata->tdata==0)||(facedata->textures_indices==0))
		flag_faceskintexturemapping=false;
	
	if(inimanager.GetValueInt("PLAYER_ONLYFACE")==0)
	{
		bodydata=new BodyData(&conv);
		bodydata->GetFileNames(inimanager.Program_Path+"characters/"+character+".xml");
		bodydata->ReadBodyGeometry();
		skeletonroot=new PlayerJoint("HumanoidRoot",inimanager.Program_Path+"characters/"+bodydata->bonesfilename);
		skeletonroot->ScanForSons(inimanager.Program_Path+"characters/"+bodydata->bonesfilename,
			inimanager.Program_Path+"characters/"+bodydata->hierarchyfilename);
		skeletonroot->ScanForConstraints();
		skeletonroot->SetWorldMatrix();
		skeletonroot->SaveInitialTranslation();
		
		//fill direct joint access table
		for(i=0; i<NUMJOINTS; i++)
		{
			skeletonroot->GetPointer(conv.JointTypeToJointString(i).c_str(),&(pJoints[i]));
		}
		//fill direct group access table:
		
	}
	else
		skeletonroot=NULL;

	WRITEAVI=false;

	status="idle";


	maxframes=0;
	use_eyes_textures=1;
	enable_sound = 0;
	
	hair_list=HairGen3DObjectList();

	LoadGLTextures(facedata->eyebulbs_texture.image,facedata->eyebulbs_texture.alpha,&eyes_texture);
	LoadGLTextures(facedata->eyelashes_texture.image,facedata->eyelashes_texture.alpha,&eyelashes_texture);
	LoadGLTextures(facedata->head_texture.image,facedata->head_texture.alpha,&head_texture);
	LoadGLTextures(facedata->lips_texture.image,facedata->lips_texture.alpha,&lips_texture);
	if(inimanager.GetValueInt("PLAYER_ONLYFACE")==0)
	{
		LoadGLTextures(bodydata->body_texture.image,bodydata->body_texture.alpha,&body_texture);
	}

	flag_shade[0]=flag_shade[1]=flag_shade[2]=1;
	flag_wire[0]=flag_wire[1]=0;
	flag_hair=1;
	flag_skeleton=0;

	flag_rotation=1;
	rot_head_pitch=0;
	rot_head_yaw=0;
	rot_head_roll=0;

	AnimationActualFrame=0;

	openedfile="";

	//FAPs->faps=0;

//#include "body files/jackie.h";

//End of body setup
	this->SetSkinColor(facial_skin.diffuse[0],facial_skin.diffuse[1],facial_skin.diffuse[2]);

	//this->SetHairColor(0.26f,0.1f,0.0f);
	this->SetHairColor(0.6f,0.3f,0.1f);

	this->AssignFile("empty");

	PerlinStep1  = 1.f/40.f;
	PerlinFactor1= 0.01f;
	noisecounter1=0;
	PerlinStep2  = 1.f/40.f;
	PerlinFactor2= 0.01f;
	noisecounter2=100;


}

void AgentAspect::SetWriteAVI()
{
	WRITEAVI=true;
}

AgentAspect::~AgentAspect()
{
	StopTalking();
	if(skeletonroot!=NULL)
	{
		delete bodydata;
	}
	delete facedata;
}


char* AgentAspect::GetStatus()
{
	return (char*)status.c_str();
}

int AgentAspect::GetCurrentFrame()
{
	return AnimationActualFrame;
}

void AgentAspect::SetCurrentFrame(int c)
{
	AnimationActualFrame=c;
}

void AgentAspect::StartTalking()
{
	if(((status=="ready")||(status=="idle"))&&(maxframes>1))
	{
		//sprintf(status,"%s","starting");
		//restartingtime=local_time;
		RestartAnimation();
	}
}

void AgentAspect::RestartAnimation()
{
	//if((strcmp(status,"starting")==0)&&(maxframes>1))
	//{
		if(audioenabled)
		{
			if(!WRITEAVI)
			{
				if(load_wave((char*)wav_filename.c_str(),(char*)name.c_str())==1)
					audio=true;
				else
					audio=false;
			}
			else
				audio=false;
		}
		else
		{
			audio=false;
		}
		status="talking";
		if(!WRITEAVI)
		{
			if(skeletonroot!=NULL)
			{
				skeletonroot->ResetRotation();
			}
			if(audio==true)
				play_wave(AnimationActualFrame*((1.0f/animationfps)*1000),(char*)name.c_str());
			else
				started_animation=local_time-(AnimationActualFrame/animationfps);
		}
	//}
}

void AgentAspect::StopTalking()
{
	if(status!="talking")
		return;
	//sprintf(status,"%s","stopping");
	status="ready";
	stoppingtime=local_time;
	if(!WRITEAVI)
	{
		stop_wave((char*)name.c_str());
		close_wave((char*)name.c_str());
	}
}



void AgentAspect::EnableAudio(bool audio)
{
	this->audioenabled=audio;
}

void AgentAspect::OneFrameUp()
{
	if(status!="ready")
		return;
	if(AnimationActualFrame<(maxframes-1))
	{
		AnimationActualFrame++;
	}
}

void AgentAspect::OneFrameDown()
{
	if((status=="ready")
		||((status=="idle")&&(maxframes>1)))
	if(AnimationActualFrame>0)
	{
		AnimationActualFrame--;
		if(status=="idle")
			status="ready";
	}
}

int AgentAspect::AssignFile(char* filename)
{
	//does a full init_face()
	StopTalking();
	
	openedfile=std::string(filename);
	status="readingfile";
	
	//a file has both a fap and a wav associated with it
	fap_filename=openedfile+".fap";
	bap_filename=openedfile+".bap";
	wav_filename=openedfile+".wav";


	if(skeletonroot!=NULL)
	{
		if(BAPs->ReadBAPFile((char*)bap_filename.c_str())==0)
		{
			BAPs->ReadBAPFile("empty.bap");
			BAPs->fps=0;
		}
	}
	else
		BAPs->fps=0;

	if (!init_face())
    {
       printf("\nFace initialisation failed...\n");
       //return 0;
    }
    else
        printf("\nFace initialisation complete\n");


	if (!check_fps())
	{
		printf("\nNo animation to show...");
		return 0;
	}

	AnimationActualFrame=0;

	if(strcmp(filename,"empty")!=0)
		status="ready";
	else
		status="idle";

	if(skeletonroot!=NULL)
		skeletonroot->ResetRotation();

	return 1;
}


int AgentAspect::AssignFAPBuffer(char *buffer)
{
	//does a full init_face()
	StopTalking();
	
	status="readingfile";
	
	//a file has both a fap and a wav associated with it
	fap_filename=openedfile+".fap";
	//sprintf(wav_filename,"%s.wav",openedfile.c_str());

	FAPs->ReadFapBuffer(buffer);
/*
	if (!init_face())
    {
       printf("\nFace initialisation failed...");
       return 0;
    }
    else
        printf("\nFace initialisation complete");
*/
	if (!check_fps())
	{
		printf("\nNo animation to show...");
		return 0;
	}

	AnimationActualFrame=0;

	status="ready";

	return 1;
}

bool AgentAspect::check_fps()
{
	animationfps = MAX(FAPs->fps, BAPs->fps);
	
	if (animationfps == 0)
		return false;

	maxframes = 0;

	if (FAPs->fps > 0)
		maxframes = FAPs->numberofframes * animationfps / FAPs->fps;

	if (BAPs->fps > 0)
		maxframes = MAX(maxframes, BAPs->numberofframes * animationfps / BAPs->fps);

	if (maxframes == 0)
		return false;

	return true;
}

bool AgentAspect::initFAPs(void)
{
	//deallocate the current FAPs data
	//FAPs->deallocate_FAP_data();

	//see if the header file checks out ...
	if(FAPs->header_file_init((char*)fap_filename.c_str())==0)
	{
		FAPs->numberofframes=1;
		FAPs->fps=0;
		return false;
	}
	//go ahead and initialise the rest of the structure
	//FAPs->allocate_FAP_data();
	//read in the full fap file
	if(FAPs->ReadFapFile((char*)fap_filename.c_str())==0)
		return false;

	FAPs->print();
	return true;
}

bool AgentAspect::init_face(void)
{
    int i;

	facedata->CopyVertexData();

	//init_faps
	if (!initFAPs()) return false;

	//funzioni in tools.c
	initialize_array_flag_surface1_flag_surface2(&flag_surface1[0],&flag_surface2[0]); 
	facedata->InitializeSkinPointArray();
	initialize_array_misc_point(&misc_point[0]); 
	facedata->CopyVdata2SkinPoint();
	copy_vdata2_into_misc_coordinate(&misc_point[0]);
	create_and_initialize_array_misc_surfaces_indices();
	facedata->InitializeSkinNormals();
	initialize_misc_normals(&misc_point[0]);

	for(i=1; i<69; i++){
		facedata->InitializeSkinSurfaces(i);
	}

	for(i=1; i<12; i++){
		initializes_misc_surfaces(i,&misc_point[0]);
	}

	facedata->CalculateSkinDistanceLwrin();
	facedata->CalculateSkinDistanceInfeyelid();
	facedata->CalculateSkinDistanceSupeyelid();

	facedata->CreateSkinSurfaceList();
	create_misc_surface_list(&misc_point[0]);

#ifdef EYELID_MAKEUP
	facedata->CalculateColorEyelid();
#endif

	return true;
}


int AgentAspect::AssignBAPBuffer(char *bapbuffer)
{
	if(status=="talking")
	{
		StopTalking();
	}

	close_wave((char*)name.c_str());
	status="readingfile";
	
	if(skeletonroot!=NULL)
	{
		if(BAPs->ReadBAPBuffer(bapbuffer)==0)
		{
			BAPs->ReadBAPFile("empty.bap");
			BAPs->fps=0;
		}
	}
	else
		BAPs->fps=0;
	
	if((FAPs->fps!=0)&&(BAPs->fps!=0))
		if(FAPs->fps!=BAPs->fps)
		{
			printf("WARNING: BAP fps (%d) and FAP fps (%d) are different! Setting them to default value of 25\n",BAPs->fps,FAPs->fps);
			FAPs->fps=BAPs->fps=25;
		}
	if((FAPs->fps==0)&&(BAPs->fps!=0))
		FAPs->fps=BAPs->fps;
	if((BAPs->fps==0)&&(FAPs->fps!=0))
		BAPs->fps=FAPs->fps;
	//load_wave(wav_filename,name);

	if(FAPs->numberofframes>BAPs->numberofframes)
		this->maxframes=FAPs->numberofframes;
	else
		this->maxframes=BAPs->numberofframes;


	AnimationActualFrame=0;

	status="ready";

	if(skeletonroot!=NULL)
		skeletonroot->ResetRotation();

	return 1;
}


//--------------------------------------------------------------------------------------------------------------
// 			Funzione per convertire il valore di un FAP nel formato locale
//--------------------------------------------------------------------------------------------------------------



char* AgentAspect::GetWavFileName()
{
	return (char*)wav_filename.c_str();
}

void AgentAspect::ReloadFile()
{
	StopTalking();
	AnimationActualFrame=0;
	AssignFile((char*)openedfile.c_str());
}

void AgentAspect::draw()
{
	int audioat;

	int fapframe_index = 0;//frame counter for the face
	int BAPFrame_index = 0;//frame counter for the body


	local_time=((float)clock()-agent_created)/CLOCKS_PER_SEC;

	if(!WRITEAVI)
	{
		if((status=="talking")&&(audio==true))
		{
			audioat=get_wave_time((char*)name.c_str());
			if(audioat<get_audio_len((char*)name.c_str()))
				AnimationActualFrame=get_wave_time((char*)name.c_str())/(1000.0f/animationfps);
			else
			{
				if(maxframes>(get_audio_len((char*)name.c_str())/(1000.0f/animationfps)))
				{
					started_animation=local_time-((float)AnimationActualFrame/animationfps);
					audio=false;
				}
			}
			//printf("audio len %d and pos %d\n",get_audio_len((char*)name.c_str()),AnimationActualFrame);
		}
		if((status=="talking")&&(audio==false))
		{
			AnimationActualFrame=(local_time-started_animation)*animationfps;
		}
	}
	else
	{
		//AnimationActualFrame=AnimationActualFrame+(FAPs->fps/25.0f);
		if(AnimationActualFrame>=(maxframes-1))
		{
			AnimationActualFrame=maxframes-1;
			StopTalking();
		}
		else
			AnimationActualFrame++;
	}

	
	if(AnimationActualFrame>(maxframes-1) && (status!="readingfile") && (status!="ready"))
	{
		AnimationActualFrame=maxframes-1;
		StopTalking();
	}


	if(animationfps!=0)
		BAPFrame_index = AnimationActualFrame * BAPs->fps / animationfps;
	else
		BAPFrame_index = 0;
	
	if(BAPFrame_index >= BAPs->numberofframes)
		BAPFrame_index = BAPs->numberofframes - 1;

	if(animationfps!=0)
		fapframe_index = AnimationActualFrame * FAPs->fps / animationfps;
	else
		fapframe_index = 0;

	if(fapframe_index >= FAPs->numberofframes)
		fapframe_index = FAPs->numberofframes - 1;

	drawGreta(fapframe_index, BAPFrame_index);
}

void AgentAspect::drawGreta(int fapframe_index, int BAPFrame_index)
{
	glPushMatrix();

	glTranslatef(agentpos.x,agentpos.y,agentpos.z);
	glRotatef(agentrot.x,1,0,0);
	glRotatef(agentrot.y,0,1,0);
	glRotatef(agentrot.z,0,0,1);
		
	float r,g,b;
	r=this->skincolor_red-facial_skin.diffuse[0];
	g=this->skincolor_green-facial_skin.diffuse[1];
	b=this->skincolor_blue-facial_skin.diffuse[2];
	
	glColor3f(facial_skin.diffuse[0]+r,facial_skin.diffuse[1]+g,facial_skin.diffuse[2]+b);
	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	//GLfloat spec[]={1,1,1,1};

	//glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,&spec[0]);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, &facial_skin.ambient[0]); 
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &facial_skin.shininess[0]); 
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , &facial_skin.specular[0]);
	
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);

	//printf("agent %s frame %d\n",this->name.c_str(),AnimationActualFrame);

	//printf("frame count %d   total bap frames %d\r",AnimationActualFrame,baptotalframes);

	if(skeletonroot!=NULL)
	{
		if(!realtime)
			LoadBAPFrame(BAPFrame_index,0);
		else
		{
			skeletonroot->SetChangedRecursive(true);
			DrawBody();
		}
	}
	FAPs->ScaleFAPs(fapframe_index, facedata->FDPs, facedata->FAPUs);

	//printf("frame count %d   total fap frames %d\r",fapframe_index,faptotalframes);

	//glScalef(SCALING,SCALING,SCALING);

	////////////////////////////////////////////////
	// face translation code (experimental)
	// bjoern 6 feb 2004

	//printf("frame %.2f\n",AnimationActualFrame);
	
	if(skeletonroot!=NULL)
	{
		glPushMatrix();
		skeletonroot->LoadGLWorldMatrix("skullbase");
		DrawFace(fapframe_index);
		glPopMatrix();
	}
	else
	{
		glTranslatef(0,80,0);
		DrawFace(fapframe_index);
	}
	//////////////////////////////////////////////////
	
	glColor3f(1,1,1);
	
	glDisable(GL_CULL_FACE);

	if(skeletonroot!=NULL)
		if(flag_shade[2])
			DrawBody();

	glDisable(GL_COLOR_MATERIAL);

	glRotatef(-agentrot.z,0,0,1);
	glRotatef(-agentrot.y,0,1,0);
	glRotatef(-agentrot.x,1,0,0);
	glTranslatef(-agentpos.x,-agentpos.y,-agentpos.z);


	glPopMatrix();
	//printf("status %s\n",status);
}

void AgentAspect::MoveForward(float quantity)
{
	agentpos.x=agentpos.x+sin((agentrot.y/360.0f)*6.28)*quantity;
	agentpos.z=agentpos.z+cos((agentrot.y/360.0f)*6.28)*quantity;
}

void AgentAspect::RotateOf(float x,float y,float z)
{
	agentrot.x=agentrot.x+x;
	agentrot.y=agentrot.y+y;
	agentrot.z=agentrot.z+z;
}

void AgentAspect::MoveAgent(float x,float y,float z)
{
	agentpos.x=x;
	agentpos.y=y;
	agentpos.z=z;
}

void AgentAspect::RotateAgent(float x,float y,float z)
{
	agentrot.x=x;
	agentrot.y=y;
	agentrot.z=z;
}

point3d AgentAspect::GetPosition()
{
	point3d temp;
	temp.x=agentpos.x;
	temp.y=agentpos.y;
	temp.z=agentpos.z;
	return temp;
}

point3d AgentAspect::GetRotation()
{
	point3d temp;
	temp.x=agentrot.x;
	temp.y=agentrot.y;
	temp.z=agentrot.z;
	return temp;
}

bool AgentAspect::init_face(char *buffer)
{
	return true;
}

void AgentAspect::SwitchSkin()
{
	if(flag_shade[0])
		flag_shade[0]=false;
	else
		flag_shade[0]=true;
}
void AgentAspect::SwitchIFAPU()
{
	applyfaps->flag_ifapu = 1 - applyfaps->flag_ifapu;
}

void AgentAspect::SwitchEyes()
{
	if(flag_shade[1])
		flag_shade[1]=false;
	else
		flag_shade[1]=true;
}

void AgentAspect::SwitchSkeleton()
{
	if(flag_skeleton)
		flag_skeleton=false;
	else
		flag_skeleton=true;
}

void AgentAspect::SwitchHair()
{
	if(flag_hair)
		flag_hair=false;
	else
		flag_hair=true;
}


void AgentAspect::SwitchWire1()
{
	if(flag_wire[0])
		flag_wire[0]=false;
	else
		flag_wire[0]=true;
}

void AgentAspect::SwitchWire2()
{
	if(flag_wire[1])
		flag_wire[1]=false;
	else
		flag_wire[1]=true;
}

bool AgentAspect::GetSkinState()
{
	return flag_shade[0];
}

bool AgentAspect::GetEyesState()
{
	return flag_shade[1];
}

bool AgentAspect::GetBodyState()
{
	return flag_shade[2];
}

bool AgentAspect::GetHairState()
{
	return flag_hair;
}

void AgentAspect::DrawBody()
{
	//memcpy(normals2,normals,(num_v*3)*sizeof(GLfloat));
	
	//for(int i=0; i<(num_v*3); i++){
	//	vertices2[i]=vertices[i];
	//}

	if(flag_skeleton==true)
	{
		skeletonroot->Draw("");
	}
	else
	{


		glEnable(GL_TEXTURE_2D);

		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY); 
		glEnableClientState(GL_TEXTURE_COORD_ARRAY );

		glNormalPointer(GL_FLOAT,0,bodydata->normals2);
		glVertexPointer(3,GL_FLOAT,0,bodydata->vertices2);
		glTexCoordPointer(2,GL_FLOAT,0,bodydata->bodyUV);

		skeletonroot->Draw3(bodydata->vertices,bodydata->vertices2,
			bodydata->normals,bodydata->normals2,bodydata->bodyvertexgroups);

		glBindTexture(GL_TEXTURE_2D, body_texture);
		//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST); // ( NEW )
		//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST); // ( NEW )

		glEnable(GL_NORMALIZE);

		glDrawElements(GL_TRIANGLES,bodydata->num_f*3,GL_UNSIGNED_INT,bodydata->indices);

		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY); 
		glDisableClientState(GL_TEXTURE_COORD_ARRAY );

		glDisable(GL_TEXTURE_2D);
	}
}

void AgentAspect::DrawFace(int fapframe_index)
{
	float black_color[3]={0.000000f, 0.000000f, 0.000000f};
	float white_color[3]={1.000000f, 1.000000f, 1.000000f};

	int i;

	//devo azzerare il flag delle superfici
	if (fapframe_index == 0){
		for (i=0; i<NUMBER_OF_FAPS; i++){
			flag_surface1[i] = 1; //mettendo 0 dico che le normali non sono da calcolare, se metto 1 è il contrario
		}
	

		for (i=0; i<12; i++){
			flag_surface2[i] = 1; //mettendo 0 dico che le normali non sono da calcolare, se metto 1 è il contrario
		}

	}
	else{
		initialize_array_flag_surface1_flag_surface2(&flag_surface1[0],&flag_surface2[0]); //metto i flag a 0 (saranno poi le funzioni a porli pari ad 1)
	}
	//fine azzeramento del flag


	glPushMatrix();


//	glTranslatef(0.0, -5.0, 0.0); //per centrare un po' meglio l'oggetto
	glScalef(facedata->HEADSCALE,facedata->HEADSCALE,facedata->HEADSCALE);//(0.6,0.6,0.6);
	
	glTranslatef(facedata->HEADPOS_X,facedata->HEADPOS_Y,facedata->HEADPOS_Z);
	
	if((status=="idle")||(status=="ready"))
	{
		rot_head_pitch = 180/M_PI*FAPs->lfaps[48];
		rot_head_yaw   = 180/M_PI*FAPs->lfaps[49]; 
		rot_head_roll  = 180/M_PI*FAPs->lfaps[50];
		/*
		float noiseoffset=noise1.noise1(noisecounter1*PerlinStep1)*PerlinFactor1;
		rot_head_pitch = 180/M_PI*(FAPs->lfaps[48]+noiseoffset);
		noiseoffset=noise2.noise1(noisecounter2*PerlinStep2)*PerlinFactor2;
		rot_head_yaw   = 180/M_PI*(FAPs->lfaps[49]+noiseoffset); 
		rot_head_roll  = 180/M_PI*FAPs->lfaps[50];*/

		glTranslatef(0.0, -11.5, -8.0); //questo e il punto chiave della rotazione
		glRotatef(rot_head_yaw,0,1,0); //rotazione attorno all'asse y  //sono indeciso sul segno della rotazione 
		glRotatef(rot_head_pitch,1,0,0); //rotazione attorno all'asse x //sono indeciso sul segno della rotazione 
		glRotatef(rot_head_roll,0,0,1); //rotazione attorno all'asse z
		glTranslatef(0.0, 11.5, 8.0); 
		//fine rotazione della testa
		
		//applico le FAPs che modificano il modello
		applyfaps->apply_faps();
		noisecounter1++;
		noisecounter2++;
	
	}
	else
	if(status=="starting")
	{
		float timeoffset=1.0f-(local_time-restartingtime);
		//printf("timeoffset=%.2f\n",timeoffset);
		if(timeoffset<=0)
		{
			timeoffset=0;
		}
		//float noiseoffset=noise1.noise1(noisecounter1*PerlinStep1)*PerlinFactor1;
		//noiseoffset=noiseoffset*timeoffset;
		//rot_head_pitch = 180/M_PI*(FAPs->lfaps[48]+noiseoffset);
		rot_head_pitch = 180/M_PI*(FAPs->lfaps[48]);
		//noiseoffset=noise2.noise1(noisecounter2*PerlinStep2)*PerlinFactor2;
		//noiseoffset=noiseoffset*timeoffset;
		//rot_head_yaw   = 180/M_PI*(FAPs->lfaps[49]+noiseoffset); 
		rot_head_yaw   = 180/M_PI*(FAPs->lfaps[49]); 
		rot_head_roll  = 180/M_PI*FAPs->lfaps[50];
		glTranslatef(0.0, -11.5, -8.0); //questo e il punto chiave della rotazione
		glRotatef(rot_head_yaw,0,1,0); //rotazione attorno all'asse y  //sono indeciso sul segno della rotazione 
		glRotatef(rot_head_pitch,1,0,0); //rotazione attorno all'asse x //sono indeciso sul segno della rotazione 
		glRotatef(rot_head_roll,0,0,1); //rotazione attorno all'asse z
		glTranslatef(0.0, 11.5, 8.0); 
		//fine rotazione della testa
		
		//applico le FAPs che modificano il modello
		applyfaps->apply_faps();
		if(timeoffset==0)
		{
			RestartAnimation();
		}

	}
	else
	if(status=="stopping")
	{
		float timeoffset=(local_time-stoppingtime);
		//printf("timeoffset=%.2f\n",timeoffset);
		if(timeoffset>=1)
		{
			timeoffset=1;
			status="ready";
		}
		float noiseoffset=noise1.noise1(noisecounter1*PerlinStep1)*PerlinFactor1;
		noiseoffset=noiseoffset*timeoffset;
		rot_head_pitch = 180/M_PI*(FAPs->lfaps[48]+noiseoffset);
		noiseoffset=noise2.noise1(noisecounter2*PerlinStep2)*PerlinFactor2;
		noiseoffset=noiseoffset*timeoffset;
		rot_head_yaw   = 180/M_PI*(FAPs->lfaps[49]+noiseoffset); 
		rot_head_roll  = 180/M_PI*FAPs->lfaps[50];
		glTranslatef(0.0, -11.5, -8.0); //questo e il punto chiave della rotazione
		glRotatef(rot_head_yaw,0,1,0); //rotazione attorno all'asse y  //sono indeciso sul segno della rotazione 
		glRotatef(rot_head_pitch,1,0,0); //rotazione attorno all'asse x //sono indeciso sul segno della rotazione 
		glRotatef(rot_head_roll,0,0,1); //rotazione attorno all'asse z
		glTranslatef(0.0, 11.5, 8.0); 
		//fine rotazione della testa
		
		//applico le FAPs che modificano il modello
		applyfaps->apply_faps();

	}
	else
	{
		//per la rotazione automatica
		if (flag_rotation&&fapframe_index<=FAPs->numberofframes){
			//rot = rot + 60*M_PI100; //rotazione automatica
			//moltiplico per 180/M_PI perché devo passare da radianti a gradi
				rot_head_pitch = 180/M_PI*FAPs->lfaps[48];
				rot_head_yaw   = 180/M_PI*FAPs->lfaps[49]; 
				rot_head_roll  = 180/M_PI*FAPs->lfaps[50];
		}
		//fine per la rotazione automatica


		//rotazione della testa dovuta alle FAP
		if(fapframe_index<=FAPs->numberofframes)
		{
			glTranslatef(0.0, -11.5, -8.0); //questo e il punto chiave della rotazione
			if(FAPs->frames[fapframe_index].FAPs[49].active)	glRotatef(rot_head_yaw,0,1,0); //rotazione attorno all'asse y  //sono indeciso sul segno della rotazione 
			if(FAPs->frames[fapframe_index].FAPs[48].active)	glRotatef(rot_head_pitch,1,0,0); //rotazione attorno all'asse x //sono indeciso sul segno della rotazione 
			if(FAPs->frames[fapframe_index].FAPs[50].active)	glRotatef(rot_head_roll,0,0,1); //rotazione attorno all'asse z
			glTranslatef(0.0, 11.5, 8.0); 
			//fine rotazione della testa
			
			//applico le FAPs che modificano il modello
			applyfaps->apply_faps();
			//fine applicazione FAPs
		}

	}


	if (flag_shade[0]){
		//per il calcolo runtime delle normali
		facedata->CalculateSkinNormalsRuntime(&flag_surface1[0]);
		facedata->ForeheadBump(FAPs,fapframe_index);
		//fine per il calcolo delle normali
		/*
		glRotatef(90,0.0,1.0,0.0);
		glRotatef(-90,1.0,0.0,0.0);
		glTranslatef(0.0,-20,0.0);
		glScalef(500,500,500);*/
		glCullFace(GL_FRONT);
		if(this->flag_faceskintexturemapping)
			DisplayFaceSkinWithTexture(); //costruisco i poligoni da visualizzare
		else
			DisplayFaceSkin();
		/*glScalef(0.001,0.001,0.001);
		glTranslatef(0.0,20,0.0);
		glRotatef(90,1.0,0.0,0.0);
		glRotatef(-90,0.0,1.0,0.0);*/
	}

	glEnable(GL_COLOR_MATERIAL);

	if (flag_shade[1]){	
		//per il calcolo runtime delle normali
		calculate_misc_normals_at_runtime(&misc_point[0],&flag_surface2[0]);
		//fine per il calcolo delle normali
 		display_teeths_eyes_tongue(fapframe_index);
	}


	//Giochino wireframe
	if (flag_wire[0] || flag_wire[1])
	{
		glPushMatrix();
		glPushAttrib(GL_COLOR_MATERIAL);
		glPolygonMode(GL_FRONT_AND_BACK ,GL_LINE);//GL_POINT);//GL_FILL);//GL_LINE);
		glScalef(1.002, 1.002, 1.002);//(0.6,0.6,0.6);
		glDisable(GL_COLOR_MATERIAL);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, &white_color[0]); 
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, &white_color[0]); 
		if (flag_wire[0])
		{
			DisplayFaceSkin(); //costruisco i poligoni da visualizzare
		}
		if (flag_wire[1])
		{
               display_teeths_eyes_tongue(fapframe_index);
		}
		glEnable(GL_COLOR_MATERIAL);
		glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);//GL_POINT);//GL_FILL);//GL_LINE);
		glPopAttrib();
		glPopMatrix();
	}

	//Fine Giochino

// Disegna i capelli: non e' definitivo
	if (flag_hair)
	{
		glCullFace(GL_BACK);
		glPushMatrix();
		glRotatef(facedata->HAIR_rx,1.0,0.0,0.0);
		glRotatef(facedata->HAIR_ry,0.0,1.0,0.0);
		glRotatef(facedata->HAIR_rz,0.0,0.0,1.0);
		glTranslatef(facedata->HAIR_tx,facedata->HAIR_ty,facedata->HAIR_tz);
		glScalef(facedata->HAIR_sx,facedata->HAIR_sy,facedata->HAIR_sz);
		glPushAttrib(GL_COLOR_MATERIAL);
		//glDisable(GL_COLOR_MATERIAL);
		//red hair
		//glColor3f(0.57f,0.28f,0.10f);
		//brown hair
		glColor3f(this->haircolor_red,this->haircolor_green,this->haircolor_blue);
		glEnable(GL_COLOR_MATERIAL);
		glCallList(hair_list);
		//glEnable(GL_COLOR_MATERIAL);
		glPopAttrib();
		glPopMatrix();
		//printf("\rsx:%.1f sy:%.1f sz:%.1f rx:%.1f ry:%.1f rz:%.1f tx:%.1f ty:%.1f tz:%.1f",sx,sy,sz,rx,ry,rz,tx,ty,tz);
	}
	// fine disegno capelli

	glPopMatrix();

}

void AgentAspect::display_teeths_eyes_tongue(int fapframe_index)
{
	float rot_l_eye_pitch, rot_l_eye_yaw, rot_r_eye_pitch, rot_r_eye_yaw; //variabili per la rotazione degli occhi

	int i, j;
//	float vertexdata[SKIN_VERTEX_NUMBER2][3];
	float trasl_x=0.0, trasl_y=0.0, trasl_z=0.0;//for the movement of the jaw

	glDisable(GL_CULL_FACE);
	
	glPushMatrix();

	//FAPs for the eyes rotation
	rot_l_eye_pitch = 180/M_PI*this->FAPs->lfaps[25];
	rot_l_eye_yaw   = 180/M_PI*this->FAPs->lfaps[23];

	rot_r_eye_pitch = 180/M_PI*this->FAPs->lfaps[26];
	rot_r_eye_yaw   = 180/M_PI*this->FAPs->lfaps[24];

	//FAPs for the jaw
	trasl_x = -this->FAPs->lfaps[15];
	trasl_y = -this->FAPs->lfaps[3];
	trasl_z = this->FAPs->lfaps[14];

	//vdata2 is recycled(!) to hold the final value of the misc points obtained
	//by summing the starting value misc_point.coordinate with misc_point.increment
	for(i=0; i < SKIN_VERTEX_NUMBER2; i++)
		for(j=0; j < 3; j++)
			vdata2[i][j] = this->misc_point[i].coordinate[j] + this->misc_point[i].increment[j];

	//glDepthFunc(GL_LESS);

	//draws the eyes without textures
	if (!this->use_eyes_textures)
		RenderEyes(fapframe_index);
	
	else //uses texture for eyes
		RenderEyesTextures();

	//teeth are moved a little bit back
	glPushMatrix();
	glTranslatef(0.0, -0.0, -1.0);
	glTranslatef(this->facedata->TEETHS_X,this->facedata->TEETHS_Y,this->facedata->TEETHS_Z);

	glPushAttrib(GL_COLOR_MATERIAL);
	glColor3fv(&gum.diffuse[0]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &gum.shininess[0]); 
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , &gum.specular[0]);
	for(i=7; i < 9; i++){
		glPushMatrix();

		//jaw
		if(i==7)
		{	
			if(this->FAPs->frames[fapframe_index].FAPs[3].active || this->FAPs->frames[fapframe_index].FAPs[14].active || this->FAPs->frames[fapframe_index].FAPs[15].active)
				glTranslatef(trasl_x, trasl_y, trasl_z); 
		}

		j=0;
		glBegin(GL_TRIANGLES);
		while (j < surface_num_polygons2[i]){
			glNormal3fv(&(this->misc_point[misc_surfaces_indices[i][j][0]].normal[0]));
			glVertex3fv(&vdata2[misc_surfaces_indices[i][j][0]][0]);
			glNormal3fv(&(this->misc_point[misc_surfaces_indices[i][j][1]].normal[0]));
			glVertex3fv(&vdata2[misc_surfaces_indices[i][j][1]][0]);
			glNormal3fv(&(this->misc_point[misc_surfaces_indices[i][j][2]].normal[0]));
			glVertex3fv(&vdata2[misc_surfaces_indices[i][j][2]][0]);
			j++;
		}
		glEnd();
		glPopMatrix();
	}
	glPopAttrib();

	glDepthFunc(GL_LEQUAL); 
	
	//teeths
	glPushAttrib(GL_COLOR_MATERIAL);
	glColor3fv(&teeth.diffuse[0]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &teeth.shininess[0]); 
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , &teeth.specular[0]);

	for(i=9; i < 11; i++){
		glPushMatrix();

		//by the movement of the jaw
		if(i==9){
			if(this->FAPs->frames[fapframe_index].FAPs[3].active || this->FAPs->frames[fapframe_index].FAPs[14].active || this->FAPs->frames[fapframe_index].FAPs[15].active)
				glTranslatef(trasl_x, trasl_y, trasl_z);
		}

		j=0;
		glBegin(GL_TRIANGLES);
		while (j < surface_num_polygons2[i]){
			glNormal3fv(&(this->misc_point[misc_surfaces_indices[i][j][0]].normal[0]));
			glVertex3fv(&vdata2[misc_surfaces_indices[i][j][0]][0]);
			glNormal3fv(&(this->misc_point[misc_surfaces_indices[i][j][1]].normal[0]));
			glVertex3fv(&vdata2[misc_surfaces_indices[i][j][1]][0]);
			glNormal3fv(&(this->misc_point[misc_surfaces_indices[i][j][2]].normal[0]));
			glVertex3fv(&vdata2[misc_surfaces_indices[i][j][2]][0]);
			j++;
		}
		glEnd();
		glPopMatrix();
	}
	glPopAttrib();

	//tongue
	glPushAttrib(GL_COLOR_MATERIAL);
	glColor3fv(&tongue_mat.diffuse[0]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &tongue_mat.shininess[0]); 
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , &tongue_mat.specular[0]);
	
	for(i=11; i < 12; i++){

		glPushMatrix();

		//by the movement of the jaw
		if(this->FAPs->frames[fapframe_index].FAPs[3].active || this->FAPs->frames[fapframe_index].FAPs[14].active || this->FAPs->frames[fapframe_index].FAPs[15].active)
			glTranslatef(trasl_x, trasl_y, trasl_z); 

		glBegin(GL_TRIANGLES);
		for (j=0;j < surface_num_polygons2[i];j++)
		{
			glNormal3fv(&(this->misc_point[misc_surfaces_indices[i][j][0]].normal[0]));
			glVertex3fv(&vdata2[misc_surfaces_indices[i][j][0]][0]);
			glNormal3fv(&(this->misc_point[misc_surfaces_indices[i][j][1]].normal[0]));
			glVertex3fv(&vdata2[misc_surfaces_indices[i][j][1]][0]);
			glNormal3fv(&(this->misc_point[misc_surfaces_indices[i][j][2]].normal[0]));
			glVertex3fv(&vdata2[misc_surfaces_indices[i][j][2]][0]);
		}
		glEnd();
		glPopMatrix();
	}
	glPopAttrib();

	glPopMatrix(); //questo è relativo allo spostamento un po' indietro dei denti

	glPopMatrix();

	glEnable(GL_CULL_FACE);

	glDepthFunc(GL_LEQUAL);
}

#define IRIS_SCALE 1.2

void AgentAspect::RenderEyes(int fapframe_index)
{
	float rot_l_eye_pitch, rot_l_eye_yaw, rot_r_eye_pitch, rot_r_eye_yaw; //variabili per la rotazione degli occhi

	rot_l_eye_pitch = 180/M_PI*this->FAPs->lfaps[25];
	rot_l_eye_yaw   = 180/M_PI*this->FAPs->lfaps[23];

	rot_r_eye_pitch = 180/M_PI*this->FAPs->lfaps[26];
	rot_r_eye_yaw   = 180/M_PI*this->FAPs->lfaps[24];
	//pupils
	glPushAttrib(GL_COLOR_MATERIAL);
	glColor3fv(&pupil.diffuse[0]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &pupil.shininess[0]); 
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , &pupil.specular[0]);

	//CHRIS fix
	int i = 0;
	for(i=1; i < 3; i++)
	{
		glPushMatrix();
		if(i==1)
		{
			//left eye
			glTranslatef(7.2, 0.05, 11.05);
			glScalef(IRIS_SCALE,IRIS_SCALE,1.0);
			if(this->FAPs->frames[fapframe_index].FAPs[25].active)
				glRotatef(rot_l_eye_pitch,1,0,0);
			if(this->FAPs->frames[fapframe_index].FAPs[23].active)
				glRotatef(rot_l_eye_yaw,0,1,0);
			glTranslatef(-7.2, -0.05, -11.05);
		}
		else{
			//right eye
			glTranslatef(-7.2, 0.05, 11.05);
			glScalef(IRIS_SCALE,IRIS_SCALE,1.0);
			if(this->FAPs->frames[fapframe_index].FAPs[26].active)
				glRotatef(rot_r_eye_pitch,1,0,0);
			if(this->FAPs->frames[fapframe_index].FAPs[24].active)
				glRotatef(rot_r_eye_yaw,0,1,0);
			glTranslatef(7.2, -0.05, -11.05);
		}

		//draws the eye
		glBegin(GL_TRIANGLES);
		for (int j=0;j < surface_num_polygons2[i];j++)
		{
			glNormal3fv(&(this->misc_point[misc_surfaces_indices[i][j][0]].normal[0]));
			glVertex3fv(&vdata2[misc_surfaces_indices[i][j][0]][0]);
			glNormal3fv(&(this->misc_point[misc_surfaces_indices[i][j][1]].normal[0]));
			glVertex3fv(&vdata2[misc_surfaces_indices[i][j][1]][0]);
			glNormal3fv(&(this->misc_point[misc_surfaces_indices[i][j][2]].normal[0]));
			glVertex3fv(&vdata2[misc_surfaces_indices[i][j][2]][0]);
		}
		glEnd();
		glPopMatrix();
	}
	glPopAttrib();

	//iris
	glPushMatrix();
	
	glPushAttrib(GL_COLOR_MATERIAL);

	glColor3fv(&iris.diffuse[0]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &iris.shininess[0]); 
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , &iris.specular[0]);

	for(i=3; i < 5; i++)
	{
		glPushMatrix();

		if(i==3){
			//left iris
			glTranslatef(7.2, 0.05, 11.05);
			glScalef(IRIS_SCALE,IRIS_SCALE,1.0);
			if(this->FAPs->frames[fapframe_index].FAPs[25].active)
				glRotatef(rot_l_eye_pitch,1,0,0);
			if(this->FAPs->frames[fapframe_index].FAPs[23].active)
				glRotatef(rot_l_eye_yaw,0,1,0);
			glTranslatef(-7.2, -0.05, -11.05);
		}
		else{
			//right iris
			glTranslatef(-7.2, 0.05, 11.05);
			glScalef(IRIS_SCALE,IRIS_SCALE,1.0);
			if(this->FAPs->frames[fapframe_index].FAPs[26].active)
				glRotatef(rot_r_eye_pitch,1,0,0);
			if(this->FAPs->frames[fapframe_index].FAPs[24].active)
				glRotatef(rot_r_eye_yaw,0,1,0);
			glTranslatef(7.2, -0.05, -11.05);
		}

		glBegin(GL_TRIANGLES);
		for (int j=0;j < surface_num_polygons2[i];j++)
		{
			glNormal3fv(&(this->misc_point[misc_surfaces_indices[i][j][0]].normal[0]));
			glVertex3fv(&vdata2[misc_surfaces_indices[i][j][0]][0]);
			glNormal3fv(&(this->misc_point[misc_surfaces_indices[i][j][1]].normal[0]));
			glVertex3fv(&vdata2[misc_surfaces_indices[i][j][1]][0]);
			glNormal3fv(&(this->misc_point[misc_surfaces_indices[i][j][2]].normal[0]));
			glVertex3fv(&vdata2[misc_surfaces_indices[i][j][2]][0]);
		}
		glEnd();
		glPopMatrix();
	}

	glPopAttrib();

	glPopMatrix();
	
	//bulb
	glPushAttrib(GL_COLOR_MATERIAL);
	glColor3fv(&eye_bulb.diffuse[0]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &eye_bulb.shininess[0]); 
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , &eye_bulb.specular[0]);
	for(i=5; i < 7; i++)
	{
		glPushMatrix();
		if(i==5)
		{
			//left bulb
			glTranslatef(7.2, 0.05, 11.05);
			if(this->FAPs->frames[fapframe_index].FAPs[25].active)
				glRotatef(rot_l_eye_pitch,1,0,0);
			if(this->FAPs->frames[fapframe_index].FAPs[23].active)
				glRotatef(rot_l_eye_yaw,0,1,0);
			glTranslatef(-7.2, -0.05, -11.05);
		}
		else
		{
			//right bulb
			glTranslatef(-7.2, 0.05, 11.05);
			if(this->FAPs->frames[fapframe_index].FAPs[26].active)
				glRotatef(rot_r_eye_pitch,1,0,0);
			if(this->FAPs->frames[fapframe_index].FAPs[24].active)
				glRotatef(rot_r_eye_yaw,0,1,0);
			glTranslatef(7.2, -0.05, -11.05);
		}

		glBegin(GL_TRIANGLES);
		for (int j=0;j < surface_num_polygons2[i];j++)
		{
			glNormal3fv(&(this->misc_point[misc_surfaces_indices[i][j][0]].normal[0]));
			glVertex3fv(&vdata2[misc_surfaces_indices[i][j][0]][0]);
			glNormal3fv(&(this->misc_point[misc_surfaces_indices[i][j][1]].normal[0]));
			glVertex3fv(&vdata2[misc_surfaces_indices[i][j][1]][0]);
			glNormal3fv(&(this->misc_point[misc_surfaces_indices[i][j][2]].normal[0]));
			glVertex3fv(&vdata2[misc_surfaces_indices[i][j][2]][0]);
		}
		glEnd();
		glPopMatrix();
	}
	glPopAttrib();

}
void AgentAspect::RenderEyesTextures()
{
	float rot_l_eye_pitch, rot_l_eye_yaw, rot_r_eye_pitch, rot_r_eye_yaw; //variabili per la rotazione degli occhi

	static GLfloat white[] = {1.0f, 1.0f, 1.0f, 1.0f};
    static GLfloat black[] = {0.0f, 0.0f, 0.0f, 1.0f};
	//printf("GL_COLOR_MATERIAL=%d (true=%d)\n\n",glIsEnabled(GL_COLOR_MATERIAL),GL_TRUE );
	
	rot_l_eye_pitch = 180/M_PI*this->FAPs->lfaps[25];
	rot_l_eye_yaw   = 180/M_PI*this->FAPs->lfaps[23];

	rot_r_eye_pitch = 180/M_PI*this->FAPs->lfaps[26];
	rot_r_eye_yaw   = 180/M_PI*this->FAPs->lfaps[24];
	 // Render a sphere with texture coordinates
	glPushAttrib(GL_COLOR_MATERIAL);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	glColor3fv(&eye_bulb.diffuse[0]);

	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &eye_bulb.shininess[0]); 
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , &eye_bulb.specular[0]);

	// occhio dx
	glPushMatrix();
	glTranslatef(this->facedata->EYES_RX,this->facedata->EYES_RY,this->facedata->EYES_RZ);
	glRotatef(rot_r_eye_pitch,1,0,0); //rotazione attorno all'asse x  
	glRotatef(rot_r_eye_yaw,0,1,0);

	glScalef(-this->facedata->EYES_SCALE,this->facedata->EYES_SCALE,this->facedata->EYES_SCALE);
	
	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, eyes_texture);
	RenderSphere( 0.0f, 0.0f, 0.0f, 1.5f, G_NRESOLUTION );
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glColor3fv(black);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , white);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE , black);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT , black);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128.0f);
	glBlendFunc(GL_ONE,GL_ONE);
	RenderSphere( 0.0f, 0.0f, 0.0f, 1.51f, G_NRESOLUTION);
	glDisable(GL_BLEND);
	
	glPopMatrix();
	// occhio sx
	glPushMatrix();

	glColor3fv(&eye_bulb.diffuse[0]);

	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &eye_bulb.shininess[0]); 
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , &eye_bulb.specular[0]);

	glTranslatef(this->facedata->EYES_LX,this->facedata->EYES_LY,this->facedata->EYES_LZ);
	glRotatef(rot_l_eye_pitch,1,0,0); //rotazione attorno all'asse x 
	glRotatef(rot_l_eye_yaw,0,1,0); //rotazione attorno all'asse y   
	glScalef(this->facedata->EYES_SCALE,this->facedata->EYES_SCALE,this->facedata->EYES_SCALE);

	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, eyes_texture);
	RenderSphere( 0.0f, 0.0f, 0.0f, 1.5f, G_NRESOLUTION );
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glColor3fv(black);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , white);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE , black);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , white);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100.0f);
	glBlendFunc(GL_ONE,GL_ONE);
	RenderSphere( 0.0f, 0.0f, 0.0f, 1.51f, G_NRESOLUTION );
	glDisable(GL_BLEND);

	glPopMatrix();
	glPopAttrib();	
}

#define PI     3.14159265358979
#define TWOPI  6.28318530717958
#define PIDIV2 1.57079632679489

void AgentAspect::RenderSphere( float c_x, float c_y, float c_z, double r, int n )
{
   int i = 0;
   int j = 0;

   double theta1 = 0.0;
   double theta2 = 0.0;
   double theta3 = 0.0;

   float e_x = 0.0f;
   float e_y = 0.0f;
   float e_z = 0.0f;

   float p_x = 0.0f;
   float p_y = 0.0f;
   float p_z = 0.0f;

   if( r < 0 )
      r = -r;

   if( n < 0 )
      n = -n;

   if( n < 4 || r <= 0 ) 
   {
      glBegin(GL_POINTS);
      glVertex3f( c_x, c_y, c_z );
      glEnd();
      return;
   }

   for( j = 0; j < n/2; ++j )
   {
      theta1 = j * TWOPI / n - PIDIV2;
      theta2 = (j + 1) * TWOPI / n - PIDIV2;

      glBegin(GL_QUAD_STRIP);

      for( i = 0; i <= n; i++ ) 
      {
//         theta3 = i *TWOPI / n;
         theta3 = i * PI / n;

         e_x = cos(theta2) * cos(theta3);
         e_y = sin(theta2);
         e_z = cos(theta2) * sin(theta3);
         p_x = c_x + r * e_x;
         p_y = c_y + r * e_y;
         p_z = c_z + r * e_z;

         glNormal3f( e_x, e_y, e_z );
         glTexCoord2f( (i)/(double)n,2*(j+1)/(double)n );
         glVertex3f( p_x, p_y, p_z );

         e_x = cos(theta1) * cos(theta3);
         e_y = sin(theta1);
         e_z = cos(theta1) * sin(theta3);
         p_x = c_x + r * e_x;
         p_y = c_y + r * e_y;
         p_z = c_z + r * e_z;

         glNormal3f( e_x, e_y, e_z );
         glTexCoord2f( (i)/(double)n,2*j/(double)n );
         glVertex3f( p_x, p_y, p_z );
      }
      glEnd();
   }
}

extern void SearchLips(float*,float*,float*,int*);
extern void SearchLips2(int,int,int*);
extern GLfloat lips_textures_coordinates[1230][2];

int AgentAspect::ExportFace(std::string NameFile, int fapframe_index)
{
	int i, j, k=0;
	pointtype* skin_point;
	std::string line;
	
	skin_point=facedata->skin_point;

	std::ofstream outputfile(NameFile.c_str());
	
	if(outputfile.is_open())
	{
		outputfile << "file \" Untitled.vtx \"" << "\n"
			<< "viewport height -1 width -1" << "\n"
			<< "view width 1.000000 gaze ( 0.000000, 0.000000, -1.000000)" << "\n"
			<< "view2 eye ( 0.000000 0.000000 0.000000 )" << "\n"
			<< "view2 up ( 0.000000 1.000000 0.000000 )" << "\n"
			<< "objects count 1" << "\n"
			<< "object 1 photometry 1 selected 1 extents_computed 0" << "\n"
			<< "extends xlo  0.000000 xhi  0.000000 ylo  0.000000 yhi  0.000000 zlo  0.000000 zhi  0.000000" << "\n"
			<< "tmap -1 -1 -1 -1 -1" << "\n"
			<< "tmapvec -1 -1 -1 -1 -1" << "\n"
			<< "tmap 0 0 0 0 0" << "\n"
			<< "userstring \"Figure 1\"" << "\n"
			<< "endobject 1" << "\n"
			<< "photometry_count 1" << "\n"
			<< "photometry 1 diffuse  -1.000000 specular  -1.000000 reflection  0.000000 color (255, 255, 255)" << "\n"
			<< "lighting count 1 ambient (-1.000000, -1.000000, -1.000000) exposure  1.000000" << "\n"
			<< "light 0 brightness  1.000000 direction ( -1.000000, -1.000000, -1.000000) color ( -1.000000, -1.000000, -1.000000) conc 4" << "\n"
			<< "background color (0, 0, 0) shown 0 method 2 xoff 0 yoff 0" << "\n"
			<< "flat_photometry 0" << "\n"
			<< "max_map_index 5" << "\n";
		
		outputfile << "mesh tri " << facedata->TrianglesNumber << " co " << facedata->FaceVerticesNumber << " vn " << facedata->VerticesNormalsNumber << " tm 0" << "\n";
		for(i=1; i < NUMBER_OF_FAPS; i++){
			j=0;
			while (j < facedata->surface_num_polygons1[i])
			{
				outputfile << "tri " << k << " obj 1 eb -1 co " << facedata->regions_indices[i][j][0] << " " << facedata->regions_indices[i][j][1] << " " << facedata->regions_indices[i][j][2] << " vn " << facedata->regions_indices[i][j][0] << " " << facedata->regions_indices[i][j][1] << " " << facedata->regions_indices[i][j][2] << " tm -1 -1 -1" << "\n"; 
				j++;
				k++;
			}
		}	
		for(i=0; i < facedata->FaceVerticesNumber; i++)
			outputfile << "coord " << i << " (" << skin_point[i].coordinate[0]+skin_point[i].increment[0] << ", " << skin_point[i].coordinate[1]+skin_point[i].increment[1] << ", " << skin_point[i].coordinate[2]+skin_point[i].increment[2] << ")\n";	
		for(i=0; i < facedata->VerticesNormalsNumber; i++)
			outputfile << "vnormal " << i << " (" << skin_point[i].normal[0] << ", " << skin_point[i].normal[1] << ", " << skin_point[i].normal[2] << ")\n";			
	}
	else
		return 0;

	outputfile.close();
	return 1;	
}

int AgentAspect::ExportFaceVRML(std::string NameFile, int fapframe_index)
{
	int i, j, k=0;
	pointtype* skin_point;
	std::string line;
	std::string nf;
//	int last;
//	int point;
	int NumIndexFace=0;
	float trasl_x=0.0, trasl_y=0.0, trasl_z=0.0;
//	pointtype* misc_point_i_j;

	nf = NameFile.substr(0,NameFile.find_last_of(".")).c_str();
	nf = nf.substr(nf.find_last_of("/")+1).c_str();
	
	skin_point=facedata->skin_point;

	std::ofstream outputfile(NameFile.c_str());
	
	if(outputfile.is_open())
	{
		outputfile << "#VRML V2.0 utf8" << "\n"
			<< "# VRML File: " << NameFile.c_str() << "\n"
			<< "DEF " << nf.c_str() << " Transform {" << "\n\n"
			<< "children [" << "\n"
			<< "DEF " << nf.c_str() << "-TIMER TimeSensor { loop TRUE cycleInterval 3.333 }," << "\n"
			<< "	Shape {" << "\n"
			<< "	appearance Appearance {" << "\n"
			<< "		material Material {" << "\n"
			<< "		diffuseColor " << facial_skin.diffuse[0] << " " << facial_skin.diffuse[1] << " " << facial_skin.diffuse[2] << "\n"
			<< "		ambientIntensity 10" << "\n"
			<< "		specularColor 0.1 0.1 0.1" << "\n"
			<< "		shininess 5" << "\n"
			<< "		transparency 0" << "\n"
			<< "		}" << "\n"
			//<< "	 texture ImageTexture { url \"lips.jpg\" }" << "\n" 
			<< "	}" << "\n"
			<< "	geometry DEF " << nf.c_str() << "-FACES IndexedFaceSet {" << "\n"
			<< "		ccw TRUE" << "\n"
			<< "		solid TRUE" << "\n"
			<< "		colorPerVertex FALSE" << "\n"
			<< "		coord DEF " << nf.c_str() << "-COORD Coordinate { point [" << "\n";


		for(i=0; i < facedata->FaceVerticesNumber; i++)
			if (i<facedata->FaceVerticesNumber-1)
				outputfile << "			" << skin_point[i].coordinate[0]+skin_point[i].increment[0] << ", " << skin_point[i].coordinate[1]+skin_point[i].increment[1] << ", " << skin_point[i].coordinate[2]+skin_point[i].increment[2] << ",\n";	
			else
				outputfile << "			" << skin_point[i].coordinate[0]+skin_point[i].increment[0] << ", " << skin_point[i].coordinate[1]+skin_point[i].increment[1] << ", " << skin_point[i].coordinate[2]+skin_point[i].increment[2] << "]\n";
		outputfile << "		}" << "\n"
/*				<< "		texCoord TextureCoordinate { point [" << "\n";
		
		for(i=33; i < 45; i++)
		{
			j=0;
			int ti[3];
			while (j < facedata->surface_num_polygons1[i])
			{
			
				ti[0]=-1;
				ti[1]=-1;
				ti[2]=-1;
				SearchLips2(i,j,ti);
				outputfile << "			" << lips_textures_coordinates[ti[0]][0] << " " << lips_textures_coordinates[ti[0]][1] << "," << "\n";

				outputfile << "			" << lips_textures_coordinates[ti[1]][0] << " " << lips_textures_coordinates[ti[1]][1] << "," << "\n";
				if (i<44 && j < facedata->surface_num_polygons1[i]-1)
					outputfile << "			" << lips_textures_coordinates[ti[2]][0] << " " << lips_textures_coordinates[ti[2]][1] << "," << "\n";
				else
					outputfile << "			" << lips_textures_coordinates[ti[2]][0] << " " << lips_textures_coordinates[ti[2]][1] << "]" << "\n";

				j++;
			}
		}
	
		outputfile  << "		}" << "\n"
*/
					<< "		coordIndex [" << "\n";

		for(i=1; i < NUMBER_OF_FAPS; i++){
			j=0;
			while (j < facedata->surface_num_polygons1[i])
			{
				NumIndexFace+=1;
				if (i==68 && j==facedata->surface_num_polygons1[i]-1)
					outputfile << "			" << facedata->regions_indices[i][j][2] << ", " << facedata->regions_indices[i][j][1] << ", " << facedata->regions_indices[i][j][0] << ", -1]" << "\n"; 
				else
					outputfile << "			" << facedata->regions_indices[i][j][2] << ", " << facedata->regions_indices[i][j][1] << ", " << facedata->regions_indices[i][j][0] << ", -1, " << "\n"; 
				j++;
			}
		}	

		outputfile 	<< "		}" << "\n"
		<< "		}" << "\n"
		<< "	]" << "\n"
		<< "}" << "\n";

		outputfile << "DEF " << nf.c_str() << " Transform {" << "\n\n"
			<< "scale 54 62 62" << "\n"
			<< "rotation 1 0 0 4.85" << "\n"
			<< "translation 0 0 -5.2" << "\n"
			//<< "translation -1.714 171.2 5.565" << "\n"
			<< "children [" << "\n"
			<< "DEF " << nf.c_str() << "-TIMER TimeSensor { loop TRUE cycleInterval 3.333 }," << "\n"
			<< "	Shape {" << "\n"
			<< "	appearance Appearance {" << "\n"
			<< "		material Material {" << "\n"
			<< "		diffuseColor 0.26 0.1 0.0" << "\n"
			<< "		ambientIntensity 0.4" << "\n"
			<< "		specularColor 0.1 0.1 0.1" << "\n"
			<< "		shininess 20" << "\n"
			<< "		transparency 0" << "\n"
			<< "		}" << "\n"
			<< "	}" << "\n"
			<< "	geometry DEF " << nf.c_str() << "-HAIR IndexedFaceSet {" << "\n"
			<< "		ccw TRUE" << "\n"
			<< "		solid TRUE" << "\n"
			<< "		colorPerVertex FALSE" << "\n"
			<< "		coord DEF " << nf.c_str() << "-COORD Coordinate { point [" << "\n";


		for(i=0; i < facedata->HairVerticesNumber; i++)
			if (i<facedata->HairVerticesNumber-1)
				outputfile << "			" << facedata->haircoords[i][0] << ", " << facedata->haircoords[i][1] << ", " << facedata->haircoords[i][2] << ",\n";	
			else
				outputfile << "			" << facedata->haircoords[i][0] << ", " << facedata->haircoords[i][1] << ", " << facedata->haircoords[i][2] << "]\n";
		outputfile << "		}" << "\n"
			<< "		coordIndex [" << "\n";

		for(i=0;i<facedata->HairIndicesNumber;i++)
       {
		   if (i<facedata->HairIndicesNumber-1)
				outputfile << "			" << facedata->hairindices[i][0] << ", " << facedata->hairindices[i][1] << ", " << facedata->hairindices[i][2] << ", -1," << "\n";
		   else
			   outputfile << "			" << facedata->hairindices[i][0] << ", " << facedata->hairindices[i][1]  << ", " << facedata->hairindices[i][2] << ", -1]" << "\n";
	   }	

		outputfile 	<< "		}" << "\n";

		outputfile  << "		}" << "\n"
		<< "	]" << "\n"
		<< "}" << "\n";

		outputfile << "DEF " << nf.c_str() << " Transform {" << "\n\n"
			<< "translation 7.4 0 11.9" << "\n"
			<< "children [" << "\n"
			<< "DEF " << nf.c_str() << "-TIMER TimeSensor { loop TRUE cycleInterval 3.333 }," << "\n"
			<< "		Shape {" << "\n"
			<< "			appearance Appearance {" << "\n"
			<< "			material Material { emissiveColor 1 1 1 }" << "\n"
			<< "			}" << "\n"
			<< "			geometry Sphere { radius 4.5 }" << "\n"
			<< "		}" << "\n"
	
		<< "	]" << "\n"
		<< "}" << "\n";

		outputfile << "DEF " << nf.c_str() << " Transform {" << "\n\n"
			<< "translation -7.4 0 11.9" << "\n"
			<< "children [" << "\n"
			<< "DEF " << nf.c_str() << "-TIMER TimeSensor { loop TRUE cycleInterval 3.333 }," << "\n"
			<< "		Shape {" << "\n"
			<< "			appearance Appearance {" << "\n"
			<< "			material Material { emissiveColor 1 1 1 }" << "\n"
			<< "			}" << "\n"
			<< "			geometry Sphere { radius 4.5 }" << "\n"
			<< "		}" << "\n"
	
		<< "	]" << "\n"
		<< "}" << "\n";		


		//FAPs for the jaw
		trasl_x = -this->FAPs->lfaps[15];
		trasl_y = -this->FAPs->lfaps[3];
		trasl_z = this->FAPs->lfaps[14];


		for(i=7; i < 9; i++){
			outputfile << "DEF " << nf.c_str() << " Transform {" << "\n\n"
				//<< "scale 50 56 62" << "\n"
				<< "translation 0 0 0.0" << "\n";
			if(i==7)
				if(this->FAPs->frames[fapframe_index].FAPs[3].active || this->FAPs->frames[fapframe_index].FAPs[14].active || this->FAPs->frames[fapframe_index].FAPs[15].active)
					outputfile << "translation " << trasl_x << " " << trasl_y << " " << trasl_y*0.18 << "\n"; 
			outputfile << "children [" << "\n"
				<< "DEF " << nf.c_str() << "-TIMER TimeSensor { loop TRUE cycleInterval 3.333 }," << "\n"
				<< "	Shape {" << "\n"
				<< "	appearance Appearance {" << "\n"
				<< "		material Material {" << "\n"
				<< "		diffuseColor 0.6 0.3 0.3" << "\n"
				<< "		ambientIntensity 0.2" << "\n"
				<< "		specularColor 0.8 0.8 0.8" << "\n"
				<< "		shininess 80" << "\n"
				<< "		transparency 0" << "\n"
				<< "		}" << "\n"
				<< "	}" << "\n"
				<< "	geometry DEF " << nf.c_str() << "-GUM IndexedFaceSet {" << "\n"
				<< "		ccw TRUE" << "\n"
				<< "		solid TRUE" << "\n"
				<< "		colorPerVertex FALSE" << "\n"
				<< "		coord DEF " << nf.c_str() << "-COORD Coordinate { point [" << "\n";

			
			for(j=0; j <  surface_num_polygons2[i]; j++)
				for (k=0; k<3; k++)
					if (j == surface_num_polygons2[i]-1 && k==2)
						outputfile << "			" << vdata2[misc_surfaces_indices[i][j][k]][0] << ", " << vdata2[misc_surfaces_indices[i][j][k]][1] << ", " << vdata2[misc_surfaces_indices[i][j][k]][2] << "]\n";
					else
						outputfile << "			" << vdata2[misc_surfaces_indices[i][j][k]][0] << ", " << vdata2[misc_surfaces_indices[i][j][k]][1] << ", " << vdata2[misc_surfaces_indices[i][j][k]][2] << ",\n";	
						
			outputfile << "		}" << "\n"
				<< "		coordIndex [" << "\n";

			for(j=0;j<surface_num_polygons2[i];j++)
			{
				if (j<surface_num_polygons2[i]-1)
					outputfile << "			" << j*3+2 << ", " << j*3+1 << ", " << j*3 << ", -1," << "\n";
				else
				outputfile << "			" << j*3+2 << ", " << j*3+1  << ", " << j*3 << ", -1]" << "\n";
			}	

			outputfile 	<< "		}" << "\n";

			outputfile  << "		}" << "\n"
			<< "	]" << "\n"
			<< "}" << "\n";
		}

		//Teeth
		for(i=9; i < 11; i++){
			outputfile << "DEF " << nf.c_str() << " Transform {" << "\n\n"
				<< "translation 0 0 0" << "\n";
			if(i==9)
				if(this->FAPs->frames[fapframe_index].FAPs[3].active || this->FAPs->frames[fapframe_index].FAPs[14].active || this->FAPs->frames[fapframe_index].FAPs[15].active)
					outputfile << "translation " << trasl_x << " " << trasl_y << " " << trasl_y*0.18 << "\n"; 
			outputfile << "children [" << "\n"
				<< "DEF " << nf.c_str() << "-TIMER TimeSensor { loop TRUE cycleInterval 3.333 }," << "\n"
				<< "	Shape {" << "\n"
				<< "	appearance Appearance {" << "\n"
				<< "		material Material {" << "\n"
				<< "		diffuseColor 0.8 0.8 0.8" << "\n"
				<< "		ambientIntensity 0.2" << "\n"
				<< "		specularColor 1.2 1.2 1.2" << "\n"
				<< "		shininess 80" << "\n"
				<< "		transparency 0" << "\n"
				<< "		}" << "\n"
				<< "	}" << "\n"
				<< "	geometry DEF " << nf.c_str() << "-TEETH IndexedFaceSet {" << "\n"
				<< "		ccw TRUE" << "\n"
				<< "		solid TRUE" << "\n"
				<< "		colorPerVertex FALSE" << "\n"
				<< "		coord DEF " << nf.c_str() << "-COORD Coordinate { point [" << "\n";

			
			for(j=0; j <  surface_num_polygons2[i]; j++)
				for (k=0; k<3; k++)
					if (j == surface_num_polygons2[i]-1 && k==2)
						outputfile << "			" << vdata2[misc_surfaces_indices[i][j][k]][0] << ", " << vdata2[misc_surfaces_indices[i][j][k]][1] << ", " << vdata2[misc_surfaces_indices[i][j][k]][2] << "]\n";
					else
						outputfile << "			" << vdata2[misc_surfaces_indices[i][j][k]][0] << ", " << vdata2[misc_surfaces_indices[i][j][k]][1] << ", " << vdata2[misc_surfaces_indices[i][j][k]][2] << ",\n";	
						
			outputfile << "		}" << "\n"
				<< "		coordIndex [" << "\n";

			for(j=0;j<surface_num_polygons2[i];j++)
			{
				if (j<surface_num_polygons2[i]-1)
					outputfile << "			" << j*3+2 << ", " << j*3+1 << ", " << j*3 << ", -1," << "\n";
				else
				outputfile << "			" << j*3+2 << ", " << j*3+1  << ", " << j*3 << ", -1]" << "\n";
			}	

			outputfile 	<< "		}" << "\n";

			outputfile  << "		}" << "\n"
			<< "	]" << "\n"
			<< "}" << "\n";
		}

		//tongue
		for(i=11; i < 12; i++){
			outputfile << "DEF " << nf.c_str() << " Transform {" << "\n\n"
				//<< "scale 50 56 62" << "\n"
				<< "translation 0 0 -1.0" << "\n";
				if(this->FAPs->frames[fapframe_index].FAPs[3].active || this->FAPs->frames[fapframe_index].FAPs[14].active || this->FAPs->frames[fapframe_index].FAPs[15].active)
					outputfile << "translation " << trasl_x << " " << trasl_y << " " << trasl_z << "\n"; 
				
			outputfile << "children [" << "\n"
				<< "DEF " << nf.c_str() << "-TIMER TimeSensor { loop TRUE cycleInterval 3.333 }," << "\n"
				<< "	Shape {" << "\n"
				<< "	appearance Appearance {" << "\n"
				<< "		material Material {" << "\n"
				<< "		diffuseColor 0.6 0.3 0.3" << "\n"
				<< "		ambientIntensity 0.2" << "\n"
				<< "		specularColor 0.2 0.2 0.2" << "\n"
				<< "		shininess 80" << "\n"
				<< "		transparency 0" << "\n"
				<< "		}" << "\n"
				<< "	}" << "\n"
				<< "	geometry DEF " << nf.c_str() << "-TONGUE IndexedFaceSet {" << "\n"
				<< "		ccw TRUE" << "\n"
				<< "		solid TRUE" << "\n"
				<< "		colorPerVertex FALSE" << "\n"
				<< "		coord DEF " << nf.c_str() << "-COORD Coordinate { point [" << "\n";

			for(j=0; j <  surface_num_polygons2[i]; j++)
			{
				for (k=0; k<3; k++)
					if (j == surface_num_polygons2[i]-1 && k==2)
						outputfile << "			" << vdata2[misc_surfaces_indices[i][j][k]][0] << ", " << vdata2[misc_surfaces_indices[i][j][k]][1] << ", " << vdata2[misc_surfaces_indices[i][j][k]][2] << "]\n";
					else
						outputfile << "			" << vdata2[misc_surfaces_indices[i][j][k]][0] << ", " << vdata2[misc_surfaces_indices[i][j][k]][1] << ", " << vdata2[misc_surfaces_indices[i][j][k]][2] << ",\n";	
			}

			outputfile << "		}" << "\n"
				<< "		coordIndex [" << "\n";

			for(j=0;j<surface_num_polygons2[i];j++)
			{
				if (j<surface_num_polygons2[i]-1)
					outputfile << "			" << j*3+2 << ", " << j*3+1 << ", " << j*3 << ", -1," << "\n";
				else
				outputfile << "			" << j*3+2 << ", " << j*3+1  << ", " << j*3 << ", -1]" << "\n";
			}	

			outputfile 	<< "		}" << "\n";

			outputfile  << "		}" << "\n"
			<< "	]" << "\n"
			<< "}" << "\n";
		}
	}
	else
		return 0;

	outputfile.close();
	return 1;	
}



//********************************************************************************************************************
//
//  the function adds the increments to each vertex and displays the face on the screen
//
//********************************************************************************************************************
//OLDNAME:build_polygons1
void AgentAspect::DisplayFaceSkin()
{	
	int i, j,k;
//	int *temp_indices[NUMBER_OF_FAPS];
	pointtype* skin_point;
	GLfloat **vdata;
	float r,g,b;
	r=this->skincolor_red-facial_skin.diffuse[0];
	g=this->skincolor_green-facial_skin.diffuse[1];
	b=this->skincolor_blue-facial_skin.diffuse[2];
	
	skin_point=facedata->skin_point;
	vdata=facedata->vdatafordrawing;

//	float vertexdata[this->FaceVerticesNumber][3];

	glPushMatrix();

	//array vdata[][] is recycled(!) to hold the values of each skin_point after the addition with its increment
	for(i=0; i < facedata->FaceVerticesNumber; i++){
		for(j=0; j < 3; j++){
			//if(skin_point[i].increment[j]!=0)
			//	printf("skin_point[%d].increment[%d]=%f\n",i,j,skin_point[i].increment[j]);
			vdata[i][j] = skin_point[i].coordinate[j]+skin_point[i].increment[j];
		}
	}
	
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, &facial_skin.ambient[0]); 
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &facial_skin.shininess[0]); 
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , &facial_skin.specular[0]);

	//this is a plane behind the mouth to mask the back of the skin of the head
	glPushAttrib(GL_COLOR_MATERIAL);
	
	glColor3f(0.3, 0.1, 0.1);

	glBegin(GL_POLYGON);
		glVertex3f(-10.0, 0.0, 0.0);
		glVertex3f(10.0, 0.0, 0.0);
		glVertex3f(10.0, -18.0, 0.0);
		glVertex3f(-10.0, -18.0, 0.0);
	glEnd();
	glBegin(GL_POLYGON);
		glVertex3f(-10.0, -18.0, 0.0);
		glVertex3f(10.0, -18.0, 0.0);
		glVertex3f(5.0, -18.0, 10.0);
		glVertex3f(-5.0, -18.0, 10.0);
	glEnd();
	glBegin(GL_TRIANGLES);
		glVertex3f(-10.0, 0.0, 0.0);
		glVertex3f(-10.0, -18.0, 0.0);
		glVertex3f(-5.0, -18.0, 10.0);
	glEnd();
	glBegin(GL_TRIANGLES);
		glVertex3f(10.0, 0.0, 0.0);
		glVertex3f(10.0, -18.0, 0.0);
		glVertex3f(5.0, -18.0, 10.0);
	glEnd();

	glPopAttrib();

	glPushAttrib(GL_COLOR_MATERIAL);

	k=0;

	//nose and cheecks
	for(i=1; i < 5; i++)
	{
		j=0;
		glBegin(GL_TRIANGLES);
		while (j < facedata->surface_num_polygons1[i])
		{
			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][0]].normal[0]));
			glVertex3fv(&vdata[facedata->regions_indices[i][j][0]][0]);

			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][1]].normal[0]));
			glVertex3fv(&vdata[facedata->regions_indices[i][j][1]][0]);

			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][2]].normal[0]));
			glVertex3fv(&vdata[facedata->regions_indices[i][j][2]][0]);

			j++;
		}
		glEnd();
	}
	
	//jaw
	for(i=45; i < 69; i++){
		j=0;
		glBegin(GL_TRIANGLES);
		while (j < facedata->surface_num_polygons1[i])
		{
			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][0]].normal[0]));
			glVertex3fv(&vdata[facedata->regions_indices[i][j][0]][0]);
			
			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][1]].normal[0]));
			glVertex3fv(&vdata[facedata->regions_indices[i][j][1]][0]);

			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][2]].normal[0]));
			glVertex3fv(&vdata[facedata->regions_indices[i][j][2]][0]);
			j++;
		}
		glEnd();
	}

	
	//lower eyelid
	glPushAttrib(GL_COLOR_MATERIAL);
	for(i=5; i < 9; i++){
		j=0;
		glBegin(GL_TRIANGLES);
		while (j < facedata->surface_num_polygons1[i])
		{
			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][0]].normal[0]));
			glColor3f(skin_point[facedata->regions_indices[i][j][0]].color[0]+r,
				skin_point[facedata->regions_indices[i][j][0]].color[1]+g,
				skin_point[facedata->regions_indices[i][j][0]].color[2]+b);
			glVertex3fv(&vdata[facedata->regions_indices[i][j][0]][0]);

			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][1]].normal[0]));
			glColor3f(skin_point[facedata->regions_indices[i][j][1]].color[0]+r,
				skin_point[facedata->regions_indices[i][j][1]].color[1]+g,
				skin_point[facedata->regions_indices[i][j][1]].color[2]+b);
			glVertex3fv(&vdata[facedata->regions_indices[i][j][1]][0]);

			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][2]].normal[0]));
			glColor3f(skin_point[facedata->regions_indices[i][j][2]].color[0]+r,
				skin_point[facedata->regions_indices[i][j][2]].color[1]+g,
				skin_point[facedata->regions_indices[i][j][2]].color[2]+b);
			glVertex3fv(&vdata[facedata->regions_indices[i][j][2]][0]);
			j++;
		}
		glEnd();
	}
	glPopAttrib();


	//upper eyelid
	glPushAttrib(GL_COLOR_MATERIAL);
	for(i=9; i < 13; i++){
		j=0;
		glBegin(GL_TRIANGLES);
		while (j < facedata->surface_num_polygons1[i])
		{
			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][0]].normal[0]));
			glColor3f(skin_point[facedata->regions_indices[i][j][0]].color[0]+r,
				skin_point[facedata->regions_indices[i][j][0]].color[1]+g,
				skin_point[facedata->regions_indices[i][j][0]].color[2]+b);
			glVertex3fv(&vdata[facedata->regions_indices[i][j][0]][0]);

			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][1]].normal[0]));
			glColor3f(skin_point[facedata->regions_indices[i][j][1]].color[0]+r,
				skin_point[facedata->regions_indices[i][j][1]].color[1]+g,
				skin_point[facedata->regions_indices[i][j][1]].color[2]+b);
			glVertex3fv(&vdata[facedata->regions_indices[i][j][1]][0]);

			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][2]].normal[0]));
			glColor3f(skin_point[facedata->regions_indices[i][j][2]].color[0]+r,
				skin_point[facedata->regions_indices[i][j][2]].color[1]+g,
				skin_point[facedata->regions_indices[i][j][2]].color[2]+b);
			glVertex3fv(&vdata[facedata->regions_indices[i][j][2]][0]);
			j++;
		}
		glEnd();
	}
	glPopAttrib();
 
	//eyebrows
	glPushAttrib(GL_COLOR_MATERIAL);

	glColor3f(this->haircolor_red*0.8f,this->haircolor_green*0.8f,this->haircolor_blue*0.8f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &eyebrow.shininess[0]); 
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , &eyebrow.specular[0]);

	for(i=13; i < 19; i++)
	{
		j=0;
		glBegin(GL_TRIANGLES);
		while (j < facedata->surface_num_polygons1[i])
		{
			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][0]].normal[0])); //ottengo una cosa irregolare tipo peluria
			glVertex3fv(&vdata[facedata->regions_indices[i][j][0]][0]);
			
			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][1]].normal[0]));
			glVertex3fv(&vdata[facedata->regions_indices[i][j][1]][0]);

			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][2]].normal[0]));
			glVertex3fv(&vdata[facedata->regions_indices[i][j][2]][0]);
			j++;
		}
		glEnd();
	}
	glPopAttrib();
 	
	glPushAttrib(GL_COLOR_MATERIAL);

	//forehead
	for(i=19; i < 33; i++){
		j=0;
		glBegin(GL_TRIANGLES);
		while (j < facedata->surface_num_polygons1[i])
		{
			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][0]].normal[0]));
			glVertex3fv(&vdata[facedata->regions_indices[i][j][0]][0]);

			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][1]].normal[0]));
			glVertex3fv(&vdata[facedata->regions_indices[i][j][1]][0]);

			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][2]].normal[0]));
			glVertex3fv(&vdata[facedata->regions_indices[i][j][2]][0]);
			j++;
		}
		glEnd();
	}
	glPopAttrib();

	//lips

	glPushAttrib(GL_COLOR_MATERIAL);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, &lips.ambient[0]); 
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &lips.shininess[0]); 
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , &lips.specular[0]);

	if(name=="greta")
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, lips_texture);

		for(i=33; i < 45; i++){
			j=0;
			glBegin(GL_TRIANGLES);
			int ti[3];
			while (j < facedata->surface_num_polygons1[i])
			{
			
				ti[0]=-1;
				ti[1]=-1;
				ti[2]=-1;
				SearchLips2(i,j,ti);

				glNormal3fv(&(skin_point[facedata->regions_indices[i][j][0]].normal[0]));
				glTexCoord2f(lips_textures_coordinates[ti[0]][0],lips_textures_coordinates[ti[0]][1]);
				glVertex3fv(&vdata[facedata->regions_indices[i][j][0]][0]);

				glNormal3fv(&(skin_point[facedata->regions_indices[i][j][1]].normal[0]));
				glTexCoord2f(lips_textures_coordinates[ti[1]][0],lips_textures_coordinates[ti[1]][1]);
				glVertex3fv(&vdata[facedata->regions_indices[i][j][1]][0]);

				glNormal3fv(&(skin_point[facedata->regions_indices[i][j][2]].normal[0]));
				glTexCoord2f(lips_textures_coordinates[ti[2]][0],lips_textures_coordinates[ti[2]][1]);
				glVertex3fv(&vdata[facedata->regions_indices[i][j][2]][0]);
	//		}
				j++;
			}
			glEnd();


		}

		glPopAttrib();
		glDisable(GL_TEXTURE_2D);
	}
	else
	{
		glColor3f(0.8, 0.5, 0.4);
		for(i=33; i < 45; i++){
			j=0;
			glBegin(GL_TRIANGLES);
//			int ti[3];
			while (j < facedata->surface_num_polygons1[i])
			{
				glNormal3fv(&(skin_point[facedata->regions_indices[i][j][0]].normal[0]));
				glVertex3fv(&vdata[facedata->regions_indices[i][j][0]][0]);

				glNormal3fv(&(skin_point[facedata->regions_indices[i][j][1]].normal[0]));
				glVertex3fv(&vdata[facedata->regions_indices[i][j][1]][0]);

				glNormal3fv(&(skin_point[facedata->regions_indices[i][j][2]].normal[0]));
				glVertex3fv(&vdata[facedata->regions_indices[i][j][2]][0]);
	//		}
				j++;
			}
			glEnd();
		}

		glPopAttrib();
	}

	glPopMatrix();
}


void AgentAspect::DisplayFaceSkinWithTexture()
{	
	int i, j,k;
//	int *temp_indices[NUMBER_OF_FAPS];
	pointtype* skin_point;
	GLfloat **vdata;
	GLfloat **tdata;

	skin_point=facedata->skin_point;
	vdata=facedata->vdatafordrawing;
	tdata=facedata->tdata;

//	float vertexdata[this->FaceVerticesNumber][3];

	glPushMatrix();

	//array vdata[][] is recycled(!) to hold the values of each skin_point after the addition with its increment
	for(i=0; i < facedata->FaceVerticesNumber; i++){
		for(j=0; j < 3; j++){
			//if(skin_point[i].increment[j]!=0)
			//	printf("skin_point[%d].increment[%d]=%f\n",i,j,skin_point[i].increment[j]);
			vdata[i][j] = skin_point[i].coordinate[j]+skin_point[i].increment[j];
		}
	}

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, &facial_skin.ambient[0]); 
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &facial_skin.shininess[0]); 
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , &facial_skin.specular[0]);
	//this is a plane behind the mouth to mask the back of the skin of the head
	glPushAttrib(GL_COLOR_MATERIAL);
	
	glColor3f(0.5, 0.2, 0.2);

	glBegin(GL_POLYGON);
		glVertex3f(-10.0, 0.0, 0.0);
		glVertex3f(10.0, 0.0, 0.0);
		glVertex3f(10.0, -18.0, 0.0);
		glVertex3f(-10.0, -18.0, 0.0);
	glEnd();
	glBegin(GL_POLYGON);
		glVertex3f(-10.0, -18.0, 0.0);
		glVertex3f(10.0, -18.0, 0.0);
		glVertex3f(5.0, -18.0, 10.0);
		glVertex3f(-5.0, -18.0, 10.0);
	glEnd();
	glBegin(GL_TRIANGLES);
		glVertex3f(-10.0, 0.0, 0.0);
		glVertex3f(-10.0, -18.0, 0.0);
		glVertex3f(-5.0, -18.0, 10.0);
	glEnd();
	glBegin(GL_TRIANGLES);
		glVertex3f(10.0, 0.0, 0.0);
		glVertex3f(10.0, -18.0, 0.0);
		glVertex3f(5.0, -18.0, 10.0);
	glEnd();

	glPopAttrib();

	glPushAttrib(GL_COLOR_MATERIAL);


	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, head_texture);
	glColor3f(1, 1, 1);

	k=0;

	//nose and cheecks
	for(i=1; i < 5; i++)
	{
		j=0;
		glBegin(GL_TRIANGLES);
		while (j < facedata->surface_num_polygons1[i])
		{
			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][0]].normal[0]));
			glTexCoord2fv(&tdata[facedata->textures_indices[i][j][0]][0]);
			glVertex3fv(&vdata[facedata->regions_indices[i][j][0]][0]);

			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][1]].normal[0]));
			glTexCoord2fv(&tdata[facedata->textures_indices[i][j][1]][0]);
			glVertex3fv(&vdata[facedata->regions_indices[i][j][1]][0]);

			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][2]].normal[0]));
			glTexCoord2fv(&tdata[facedata->textures_indices[i][j][2]][0]);
			glVertex3fv(&vdata[facedata->regions_indices[i][j][2]][0]);

			j++;
		}
		glEnd();
	}
	
	//jaw
	for(i=45; i < 69; i++){
		j=0;
		glBegin(GL_TRIANGLES);
		while (j < facedata->surface_num_polygons1[i])
		{
			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][0]].normal[0]));
			glTexCoord2fv(&tdata[facedata->textures_indices[i][j][0]][0]);
			glVertex3fv(&vdata[facedata->regions_indices[i][j][0]][0]);
			
			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][1]].normal[0]));
			glTexCoord2fv(&tdata[facedata->textures_indices[i][j][1]][0]);
			glVertex3fv(&vdata[facedata->regions_indices[i][j][1]][0]);

			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][2]].normal[0]));
			glTexCoord2fv(&tdata[facedata->textures_indices[i][j][2]][0]);
			glVertex3fv(&vdata[facedata->regions_indices[i][j][2]][0]);
			j++;
		}
		glEnd();
	}

	//lower eyelid
	glPushAttrib(GL_COLOR_MATERIAL);
	for(i=5; i < 9; i++){
		j=0;
		glBegin(GL_TRIANGLES);
		while (j < facedata->surface_num_polygons1[i])
		{
			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][0]].normal[0]));
			glTexCoord2fv(&tdata[facedata->textures_indices[i][j][0]][0]);
			glVertex3fv(&vdata[facedata->regions_indices[i][j][0]][0]);

			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][1]].normal[0]));
			glTexCoord2fv(&tdata[facedata->textures_indices[i][j][1]][0]);
			glVertex3fv(&vdata[facedata->regions_indices[i][j][1]][0]);

			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][2]].normal[0]));
			glTexCoord2fv(&tdata[facedata->textures_indices[i][j][2]][0]);
			glVertex3fv(&vdata[facedata->regions_indices[i][j][2]][0]);
			j++;
		}
		glEnd();
	}
	glPopAttrib();


	//upper eyelid
	glPushAttrib(GL_COLOR_MATERIAL);
	for(i=9; i < 13; i++){
		j=0;
		glBegin(GL_TRIANGLES);
		while (j < facedata->surface_num_polygons1[i])
		{
			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][0]].normal[0]));
			glTexCoord2fv(&tdata[facedata->textures_indices[i][j][0]][0]);
			glVertex3fv(&vdata[facedata->regions_indices[i][j][0]][0]);

			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][1]].normal[0]));
			glTexCoord2fv(&tdata[facedata->textures_indices[i][j][1]][0]);
			glVertex3fv(&vdata[facedata->regions_indices[i][j][1]][0]);

			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][2]].normal[0]));
			glTexCoord2fv(&tdata[facedata->textures_indices[i][j][2]][0]);
			glVertex3fv(&vdata[facedata->regions_indices[i][j][2]][0]);
			j++;
		}
		glEnd();
	}
	glPopAttrib();
 
	//eyebrows
	glPushAttrib(GL_COLOR_MATERIAL);

	if(!flag_faceskintexturemapping)
	{
		glColor3fv(&eyebrow.diffuse[0]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &eyebrow.shininess[0]); 
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , &eyebrow.specular[0]);
	}

	for(i=13; i < 19; i++)
	{
		j=0;
		glBegin(GL_TRIANGLES);
		while (j < facedata->surface_num_polygons1[i])
		{
			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][0]].normal[0])); //ottengo una cosa irregolare tipo peluria
			glTexCoord2fv(&tdata[facedata->textures_indices[i][j][0]][0]);
			glVertex3fv(&vdata[facedata->regions_indices[i][j][0]][0]);
			
			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][1]].normal[0]));
			glTexCoord2fv(&tdata[facedata->textures_indices[i][j][1]][0]);
			glVertex3fv(&vdata[facedata->regions_indices[i][j][1]][0]);

			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][2]].normal[0]));
			glTexCoord2fv(&tdata[facedata->textures_indices[i][j][2]][0]);
			glVertex3fv(&vdata[facedata->regions_indices[i][j][2]][0]);
			j++;
		}
		glEnd();
	}
	glPopAttrib();
 	
	glPushAttrib(GL_COLOR_MATERIAL);

	//forehead
	for(i=19; i < 33; i++){
		j=0;
		glBegin(GL_TRIANGLES);
		while (j < facedata->surface_num_polygons1[i])
		{
			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][0]].normal[0]));
			glTexCoord2fv(&tdata[facedata->textures_indices[i][j][0]][0]);
			glVertex3fv(&vdata[facedata->regions_indices[i][j][0]][0]);

			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][1]].normal[0]));
			glTexCoord2fv(&tdata[facedata->textures_indices[i][j][1]][0]);
			glVertex3fv(&vdata[facedata->regions_indices[i][j][1]][0]);

			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][2]].normal[0]));
			glTexCoord2fv(&tdata[facedata->textures_indices[i][j][2]][0]);
			glVertex3fv(&vdata[facedata->regions_indices[i][j][2]][0]);
			j++;
		}
		glEnd();
	}
	glPopAttrib();

	//lips


   for(i=33; i < 45; i++){
		j=0;
		glBegin(GL_TRIANGLES);
		int ti[3];
		while (j < facedata->surface_num_polygons1[i])
		{
			if(!flag_faceskintexturemapping)
			{
				ti[0]=-1;
				ti[1]=-1;
				ti[2]=-1;
				SearchLips2(i,j,ti);
			}

			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][0]].normal[0]));
			glTexCoord2fv(&tdata[facedata->textures_indices[i][j][0]][0]);
			glVertex3fv(&vdata[facedata->regions_indices[i][j][0]][0]);

			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][1]].normal[0]));
			glTexCoord2fv(&tdata[facedata->textures_indices[i][j][1]][0]);
			glVertex3fv(&vdata[facedata->regions_indices[i][j][1]][0]);

			glNormal3fv(&(skin_point[facedata->regions_indices[i][j][2]].normal[0]));
			glTexCoord2fv(&tdata[facedata->textures_indices[i][j][2]][0]);
			glVertex3fv(&vdata[facedata->regions_indices[i][j][2]][0]);
//		}
			j++;
		}
		glEnd();


	}

    


	glPopAttrib();

	glDisable(GL_TEXTURE_2D);

	

	glPopMatrix();
}


AUX_RGBImageRec *AgentAspect::LoadBMP(char *Filename)
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
unsigned char* AgentAspect::AddAlpha(unsigned char *src, int size) // 0 - 255
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

int AgentAspect::LoadGLTextures(std::string filename,bool alpha,GLuint *texture_dest)
{
	std::string path;
//	int i;

	AUX_RGBImageRec *TextureImage;

	TextureImage=0;

	path=inimanager.Program_Path+"characters/"+filename;
	// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
	if (TextureImage = LoadBMP((char*)path.c_str()))
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
#ifdef LINEARFILTERING
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
#else
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
#endif

		free(TextureImage->data);
		free(TextureImage);
		return 1;
	}
	else
		return 0;
}

void AgentAspect::InitGLTextures()
{

    glEnable(GL_DEPTH_TEST); // enable the z-buffer
    glEnable(GL_TEXTURE_2D); // Enable Texture Mapping

}




// LoadBAPFrame: 
// -Bjoern 13-Feb-2004
void AgentAspect::LoadBAPFrame(int index,BAPFrame *inputBAPFrame)
{
	//should index be unsigned?
	bool found;
	//char aux[255],jointname[255];
	float value;
	int j;
	
	if(skeletonroot==0)
		return;
	if((index<2)||realtime)
		skeletonroot->SetChangedRecursive(true);
	else
		skeletonroot->SetChangedRecursive(false);
	BAPFrame *thisframe;
	//assumption: vector index corresponds to frame number!
	if(inputBAPFrame==0)
		
	{
		if (BAPs->numberofframes==0) {
			found=false;
		} else {
			found=true;
			if(index<BAPs->numberofframes) {
				thisframe=BAPs->frames[index];	
			} else {
				thisframe=BAPs->frames[BAPs->numberofframes-1];
			}
		}
	}
	else
	{
		thisframe=inputBAPFrame;
		found=true;
	}
	
	if(found) //MODIFIED by bjoern 6 feb 04
	{

		string mJoint;
		string jointName;
		string axisName;

		int dot_pos;
		///////////////////////////////////////////
		// DEBUG: IK TRYOUT 2 - write to BAP frame
		/*
		GestureKinematics IK;
		static int debugX=0;
		if (++debugX > 50) debugX=0;
		IK.IkSolveBySector(int(debugX/10)%5,2,2,1,*thisframe);
		*/
		/////////////////////////////////////////////
		
		int jointIndex; //index into Joint pointer array;
		
		for(int i=0; i<=NUMBAPS; i++) {
			
			//if((i>97))
			//	continue;

			value= thisframe->GetBAPAngle(i,degrees);
			jointIndex=(int)conv.BAPToJointType(i);
			
			//if(value!=0 && thisframe->GetMask(i)) 
			if(thisframe->GetMask(i))
			{ 

				mJoint=conv.IntToMJoint(i); //of form "joint.rotateaxis"

				dot_pos=mJoint.find_first_of('.'); //position of dot in mjointstring
				jointName=mJoint.substr(0, dot_pos); 
				axisName=mJoint.substr(dot_pos+1, mJoint.length()-(dot_pos+1)); //"rotateX" or ..."Y" or ..."Z"

				pJoints[jointIndex]->SetChanged(true);

				// copy angle value into the appropriate joint
				if(axisName.compare("rotateX")==0) 
					pJoints[jointIndex]->SetRotationX(jointName.c_str(),value);
				else if(axisName.compare("rotateY")==0) 
					pJoints[jointIndex]->SetRotationY(jointName.c_str(),value);
				else if (axisName.compare("rotateZ")==0) 
					pJoints[jointIndex]->SetRotationZ(jointName.c_str(),value);
			}
		}
		////////////////////////////////////////////////////////////////////////////////
		// DEBUG: IK TRYOUT
		/*
		skeletonroot->SetRotationY("r_sternoclavicular",0.0);
		skeletonroot->SetRotationZ("r_sternoclavicular",0.0);
		skeletonroot->SetRotationY("l_sternoclavicular",0.0);
		skeletonroot->SetRotationZ("l_sternoclavicular",0.0);
		skeletonroot->SetRotationY("r_acromioclavicular",0.0);
		skeletonroot->SetRotationZ("r_acromioclavicular",0.0);
		skeletonroot->SetRotationY("l_acromioclavicular",0.0);
		skeletonroot->SetRotationZ("l_acromioclavicular",0.0);
		skeletonroot->SetRotationX("r_shoulder",0.0);
		skeletonroot->SetRotationY("r_shoulder",0.0);
		skeletonroot->SetRotationZ("r_shoulder",0.0);
		skeletonroot->SetRotationX("l_shoulder",0.0);
		skeletonroot->SetRotationY("l_shoulder",0.0);
		skeletonroot->SetRotationZ("l_shoulder",0.0);
		skeletonroot->SetRotationX("r_elbow",0.0);
		skeletonroot->SetRotationY("r_elbow",0.0);
		skeletonroot->SetRotationX("l_elbow",0.0);
		skeletonroot->SetRotationY("l_elbow",0.0);
		skeletonroot->SetRotationX("r_wrist",0.0);
		skeletonroot->SetRotationY("r_wrist",0.0);
		skeletonroot->SetRotationZ("r_wrist",0.0);
		skeletonroot->SetRotationX("l_wrist",0.0);
		skeletonroot->SetRotationY("l_wrist",0.0);
		skeletonroot->SetRotationZ("l_wrist",0.0);
		
		GestureKinematics IK;
		static float debugX=0.0;
		if (++debugX > 50) debugX=0.0;
		coord3d goal,solution;
		float flexion;
		//goal.x=-21.009;
		//goal.y=60.78;
		//goal.z=-6.41;
		goal.x=debugX;
		goal.y=40.0;
		goal.z=20.0;

		solution=IK.IkSolveByPosition(goal, &flexion,1);
		skeletonroot->SetRotationX("l_shoulder",solution.x);
		skeletonroot->SetRotationY("l_shoulder",solution.y);
		skeletonroot->SetRotationZ("l_shoulder",solution.z);
		skeletonroot->SetRotationX("l_elbow",flexion);
		*/
		// END DEBUG IK
		///////////////////////////////////////////
		
	}

	skeletonroot->PropagateChanged();

	for(j=0; j<NUMJOINTS; j++) {	
		if(bodydata->pJointBVGs[j]!=NULL)
			bodydata->pJointBVGs[j]->changed=pJoints[j]->GetChanged();
	}

	skeletonroot->ExecuteRotations();
	skeletonroot->SetWorldMatrix();
}


std::string AgentAspect::GetBaseFileName()
{
	return openedfile;
}

int AgentAspect::GetTotalNumberOfFrames()
{
	return maxframes;
}

int AgentAspect::GetFapFps()
{
	return FAPs->fps;
}

GLint AgentAspect::HairGen3DObjectList()
{
	
 int i;
// int j;
 int mindex=0;

 GLint lid=glGenLists(1);


	glNewList(lid, GL_COMPILE);



	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &hair.shininess[0]); 
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , &hair.specular[0]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, &hair.ambient[0]); 
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION , &hair.emission[0]);


    glBegin (GL_TRIANGLES);

	for(i=0;i<facedata->HairIndicesNumber;i++)
       {
		  glNormal3f (facedata->hairnormals[facedata->hairindices[i][3]][0],
					  facedata->hairnormals[facedata->hairindices[i][3]][1],
					  facedata->hairnormals[facedata->hairindices[i][3]][2]);

		  glVertex3f (facedata->haircoords[facedata->hairindices[i][0]][0],
					  facedata->haircoords[facedata->hairindices[i][0]][1],
					  facedata->haircoords[facedata->hairindices[i][0]][2]);

		  glNormal3f (facedata->hairnormals[facedata->hairindices[i][4]][0],
					  facedata->hairnormals[facedata->hairindices[i][4]][1],
					  facedata->hairnormals[facedata->hairindices[i][4]][2]);

		  glVertex3f (facedata->haircoords[facedata->hairindices[i][1]][0],
					  facedata->haircoords[facedata->hairindices[i][1]][1],
					  facedata->haircoords[facedata->hairindices[i][1]][2]);

		  glNormal3f (facedata->hairnormals[facedata->hairindices[i][5]][0],
					  facedata->hairnormals[facedata->hairindices[i][5]][1],
					  facedata->hairnormals[facedata->hairindices[i][5]][2]);

		  glVertex3f (facedata->haircoords[facedata->hairindices[i][2]][0],
					  facedata->haircoords[facedata->hairindices[i][2]][1],
					  facedata->haircoords[facedata->hairindices[i][2]][2]);
        }
    glEnd ();

 glEndList();
 
 return lid;
}

void AgentAspect::SetSkinColor(float r,float g,float b)
{
	//printf("setskin %f %f %f\n",r,g,b);
	this->skincolor_red=r;
	this->skincolor_green=g;
	this->skincolor_blue=b;
}

void AgentAspect::SetHairColor(float r,float g,float b)
{
	this->haircolor_red=r;
	this->haircolor_green=g;
	this->haircolor_blue=b;
}

void AgentAspect::LoadFAPFrame(FAPFrame *f)
{
	FAPs->LoadFAPFrame(f,this->facedata->FDPs,this->facedata->FAPUs);
}
