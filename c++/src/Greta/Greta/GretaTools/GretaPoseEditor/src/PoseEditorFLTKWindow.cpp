//Copyright 1999-2005 Catherine Pelachaud - c.pelachaud@iut.univ-paris8.fr
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

// PoseEditorFLTKWindow.cpp: implementation of the PoseEditorFLTKWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "PoseEditorFLTKWindow.h"
#include "BAPConverter.h"
#include <stdio.h>
#include <string.h>
#include <list>
#include <string>
#include <fl/fl_widget.h>
#include <FL/Fl_File_Chooser.H>

#include "IniManager.h"

extern IniManager inimanager;

	static LPRECT rRegion;
	static    HDIB     hDib; 
	static RECT captRegion; 


void gretaplayerwindowclosed(void* v)
{
	((PoseEditorFLTKWindow*)v)->hide();
	((PoseEditorFLTKWindow*)v)->glutw->RemoveIdle();
}

void selectedrotatex(Fl_Widget* w, void* v)
{
	char number[10];
	std::string joint;
	if(((PoseEditorFLTKWindow*)v)->modifyingright==true)
	{
		joint=((PoseEditorFLTKWindow*)v)->jointnames_right[((PoseEditorFLTKWindow*)v)->jointname_right->value()];
	}
	if(((PoseEditorFLTKWindow*)v)->modifyingleft==true)
	{
		joint=((PoseEditorFLTKWindow*)v)->jointnames_left[((PoseEditorFLTKWindow*)v)->jointname_left->value()];
	}
	if(((PoseEditorFLTKWindow*)v)->modifyingspine==true)
	{
		joint=((PoseEditorFLTKWindow*)v)->jointnames_spine[((PoseEditorFLTKWindow*)v)->jointname_spine->value()];
	}
	joint=joint+".rotateX";
	sprintf(number,"%.2f",((Fl_Slider*)w)->value());
	BAPConverter bc;
	BAPType resultbap;
	resultbap=bc.MJointToBAP(joint);
	((PoseEditorFLTKWindow*)v)->numrotatex->value(number);
	((PoseEditorFLTKWindow*)v)->glutw->agent->BAPs->frames[0]->SetBAP(resultbap,(((PoseEditorFLTKWindow*)v)->rotatex->value()/180*3.14)*100000);
	((PoseEditorFLTKWindow*)v)->redraw();
}

void selectednumrotatex(Fl_Widget* w, void* v)
{
	((PoseEditorFLTKWindow*)v)->rotatex->value(atof(((Fl_Input*)w)->value()));
	selectedrotatex(w,v);
}

void selectedrotatey(Fl_Widget* w, void* v)
{
	char number[10];
	std::string joint;
	if(((PoseEditorFLTKWindow*)v)->modifyingright==true)
	{
		joint=((PoseEditorFLTKWindow*)v)->jointnames_right[((PoseEditorFLTKWindow*)v)->jointname_right->value()];
	}
	if(((PoseEditorFLTKWindow*)v)->modifyingleft==true)
	{
		joint=((PoseEditorFLTKWindow*)v)->jointnames_left[((PoseEditorFLTKWindow*)v)->jointname_left->value()];
	}
	if(((PoseEditorFLTKWindow*)v)->modifyingspine==true)
	{
		joint=((PoseEditorFLTKWindow*)v)->jointnames_spine[((PoseEditorFLTKWindow*)v)->jointname_spine->value()];
	}
	joint=joint+".rotateY";
	sprintf(number,"%.2f",((Fl_Slider*)w)->value());
	BAPConverter bc;
	BAPType resultbap;
	resultbap=bc.MJointToBAP(joint);
	((PoseEditorFLTKWindow*)v)->numrotatey->value(number);
	((PoseEditorFLTKWindow*)v)->glutw->agent->BAPs->frames[0]->SetBAP(resultbap,(((PoseEditorFLTKWindow*)v)->rotatey->value()/180*3.14)*100000);
	((PoseEditorFLTKWindow*)v)->redraw();
}

void selectednumrotatey(Fl_Widget* w, void* v)
{
	((PoseEditorFLTKWindow*)v)->rotatey->value(atof(((Fl_Input*)w)->value()));
	selectedrotatey(w,v);
}

void selectedrotatez(Fl_Widget* w, void* v)
{
	char number[10];
	std::string joint;
	if(((PoseEditorFLTKWindow*)v)->modifyingright==true)
	{
		joint=((PoseEditorFLTKWindow*)v)->jointnames_right[((PoseEditorFLTKWindow*)v)->jointname_right->value()];
	}
	if(((PoseEditorFLTKWindow*)v)->modifyingleft==true)
	{
		joint=((PoseEditorFLTKWindow*)v)->jointnames_left[((PoseEditorFLTKWindow*)v)->jointname_left->value()];
	}
	if(((PoseEditorFLTKWindow*)v)->modifyingspine==true)
	{
		joint=((PoseEditorFLTKWindow*)v)->jointnames_spine[((PoseEditorFLTKWindow*)v)->jointname_spine->value()];
	}
	joint=joint+".rotateZ";
	sprintf(number,"%.2f",((Fl_Slider*)w)->value());
	BAPConverter bc;
	BAPType resultbap;
	resultbap=bc.MJointToBAP(joint);
	((PoseEditorFLTKWindow*)v)->numrotatez->value(number);
	((PoseEditorFLTKWindow*)v)->glutw->agent->BAPs->frames[0]->SetBAP(resultbap,(((PoseEditorFLTKWindow*)v)->rotatez->value()/180*3.14)*100000);
	((PoseEditorFLTKWindow*)v)->redraw();
}

void selectednumrotatez(Fl_Widget* w, void* v)
{
	((PoseEditorFLTKWindow*)v)->rotatez->value(atof(((Fl_Input*)w)->value()));
	selectedrotatez(w,v);
}

void selectedjointname_right(Fl_Widget* w, void* v)
{
	((PoseEditorFLTKWindow*)v)->modifyingright=true;
	((PoseEditorFLTKWindow*)v)->modifyingleft=false;
	((PoseEditorFLTKWindow*)v)->modifyingspine=false;
	((PoseEditorFLTKWindow*)v)->jointname_left->value(0);
	((PoseEditorFLTKWindow*)v)->jointname_spine->value(0);
	std::string joint;
	BAPConverter bc;
	char number[10];
	joint=((PoseEditorFLTKWindow*)v)->jointnames_right[((PoseEditorFLTKWindow*)v)->jointname_right->value()];
	joint=joint+".rotateX";
	BAPType resultbap;
	resultbap=bc.MJointToBAP(joint);
	((PoseEditorFLTKWindow*)v)->rotatex->value(((PoseEditorFLTKWindow*)v)->glutw->agent->BAPs->frames[0]->GetBAP(resultbap)/100000.0f/3.14f*180);
	sprintf(number,"%.2f",((PoseEditorFLTKWindow*)v)->glutw->agent->BAPs->frames[0]->GetBAP(resultbap)/100000.0f/3.14f*180);
	((PoseEditorFLTKWindow*)v)->numrotatex->value(number);
	joint=((PoseEditorFLTKWindow*)v)->jointnames_right[((PoseEditorFLTKWindow*)v)->jointname_right->value()];
	joint=joint+".rotateY";
	resultbap=bc.MJointToBAP(joint);
	((PoseEditorFLTKWindow*)v)->rotatey->value(((PoseEditorFLTKWindow*)v)->glutw->agent->BAPs->frames[0]->GetBAP(resultbap)/100000.0f/3.14f*180);
	sprintf(number,"%.2f",((PoseEditorFLTKWindow*)v)->glutw->agent->BAPs->frames[0]->GetBAP(resultbap)/100000.0f/3.14f*180);
	((PoseEditorFLTKWindow*)v)->numrotatey->value(number);
	joint=((PoseEditorFLTKWindow*)v)->jointnames_right[((PoseEditorFLTKWindow*)v)->jointname_right->value()];
	joint=joint+".rotateZ";
	resultbap=bc.MJointToBAP(joint);
	((PoseEditorFLTKWindow*)v)->rotatez->value(((PoseEditorFLTKWindow*)v)->glutw->agent->BAPs->frames[0]->GetBAP(resultbap)/100000.0f/3.14f*180);
	sprintf(number,"%.2f",((PoseEditorFLTKWindow*)v)->glutw->agent->BAPs->frames[0]->GetBAP(resultbap)/100000.0f/3.14f*180);
	((PoseEditorFLTKWindow*)v)->numrotatez->value(number);
	((PoseEditorFLTKWindow*)v)->redraw();
}

void selectedjointname_left(Fl_Widget* w, void* v)
{
	((PoseEditorFLTKWindow*)v)->modifyingright=false;
	((PoseEditorFLTKWindow*)v)->modifyingleft=true;
	((PoseEditorFLTKWindow*)v)->modifyingspine=false;
	((PoseEditorFLTKWindow*)v)->jointname_right->value(0);
	((PoseEditorFLTKWindow*)v)->jointname_spine->value(0);
	std::string joint;
	char number[10];
	BAPConverter bc;
	BAPType resultbap;
	joint=((PoseEditorFLTKWindow*)v)->jointnames_left[((PoseEditorFLTKWindow*)v)->jointname_left->value()];
	joint=joint+".rotateX";
	resultbap=bc.MJointToBAP(joint);
	((PoseEditorFLTKWindow*)v)->rotatex->value(((PoseEditorFLTKWindow*)v)->glutw->agent->BAPs->frames[0]->GetBAP(resultbap)/100000.0f/3.14f*180);
	sprintf(number,"%.2f",((PoseEditorFLTKWindow*)v)->glutw->agent->BAPs->frames[0]->GetBAP(resultbap)/100000.0f/3.14f*180);
	((PoseEditorFLTKWindow*)v)->numrotatex->value(number);
	joint=((PoseEditorFLTKWindow*)v)->jointnames_left[((PoseEditorFLTKWindow*)v)->jointname_left->value()];
	joint=joint+".rotateY";
	resultbap=bc.MJointToBAP(joint);
	((PoseEditorFLTKWindow*)v)->rotatey->value(((PoseEditorFLTKWindow*)v)->glutw->agent->BAPs->frames[0]->GetBAP(resultbap)/100000.0f/3.14f*180);
	sprintf(number,"%.2f",((PoseEditorFLTKWindow*)v)->glutw->agent->BAPs->frames[0]->GetBAP(resultbap)/100000.0f/3.14f*180);
	((PoseEditorFLTKWindow*)v)->numrotatey->value(number);
	joint=((PoseEditorFLTKWindow*)v)->jointnames_left[((PoseEditorFLTKWindow*)v)->jointname_left->value()];
	joint=joint+".rotateZ";
	resultbap=bc.MJointToBAP(joint);
	((PoseEditorFLTKWindow*)v)->rotatez->value(((PoseEditorFLTKWindow*)v)->glutw->agent->BAPs->frames[0]->GetBAP(resultbap)/100000.0f/3.14f*180);
	sprintf(number,"%.2f",((PoseEditorFLTKWindow*)v)->glutw->agent->BAPs->frames[0]->GetBAP(resultbap)/100000.0f/3.14f*180);
	((PoseEditorFLTKWindow*)v)->numrotatez->value(number);
	((PoseEditorFLTKWindow*)v)->redraw();
}

void selectedjointname_spine(Fl_Widget* w, void* v)
{
	((PoseEditorFLTKWindow*)v)->modifyingright=false;
	((PoseEditorFLTKWindow*)v)->modifyingleft=false;
	((PoseEditorFLTKWindow*)v)->modifyingspine=true;
	((PoseEditorFLTKWindow*)v)->jointname_left->value(0);
	((PoseEditorFLTKWindow*)v)->jointname_right->value(0);
	BAPConverter bc;
	BAPType resultbap;
	std::string joint;
	char number[10];
	joint=((PoseEditorFLTKWindow*)v)->jointnames_spine[((PoseEditorFLTKWindow*)v)->jointname_spine->value()];
	joint=joint+".rotateX";
	resultbap=bc.MJointToBAP(joint);
	((PoseEditorFLTKWindow*)v)->rotatex->value(((PoseEditorFLTKWindow*)v)->glutw->agent->BAPs->frames[0]->GetBAP(resultbap)/100000.0f/3.14f*180);
	sprintf(number,"%.2f",((PoseEditorFLTKWindow*)v)->glutw->agent->BAPs->frames[0]->GetBAP(resultbap)/100000.0f/3.14f*180);
	((PoseEditorFLTKWindow*)v)->numrotatex->value(number);
	joint=((PoseEditorFLTKWindow*)v)->jointnames_spine[((PoseEditorFLTKWindow*)v)->jointname_spine->value()];
	joint=joint+".rotateY";
	resultbap=bc.MJointToBAP(joint);
	((PoseEditorFLTKWindow*)v)->rotatey->value(((PoseEditorFLTKWindow*)v)->glutw->agent->BAPs->frames[0]->GetBAP(resultbap)/100000.0f/3.14f*180);
	sprintf(number,"%.2f",((PoseEditorFLTKWindow*)v)->glutw->agent->BAPs->frames[0]->GetBAP(resultbap)/100000.0f/3.14f*180);
	((PoseEditorFLTKWindow*)v)->numrotatey->value(number);
	joint=((PoseEditorFLTKWindow*)v)->jointnames_spine[((PoseEditorFLTKWindow*)v)->jointname_spine->value()];
	joint=joint+".rotateZ";
	resultbap=bc.MJointToBAP(joint);
	((PoseEditorFLTKWindow*)v)->rotatez->value(((PoseEditorFLTKWindow*)v)->glutw->agent->BAPs->frames[0]->GetBAP(resultbap)/100000.0f/3.14f*180);
	sprintf(number,"%.2f",((PoseEditorFLTKWindow*)v)->glutw->agent->BAPs->frames[0]->GetBAP(resultbap)/100000.0f/3.14f*180);
	((PoseEditorFLTKWindow*)v)->numrotatez->value(number);
	((PoseEditorFLTKWindow*)v)->redraw();
}

void selectedload(Fl_Widget* w, void* v)
{
	((PoseEditorFLTKWindow*)v)->Load();
}

void selectedsave(Fl_Widget* w, void* v)
{
	((PoseEditorFLTKWindow*)v)->Save();
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PoseEditorFLTKWindow::PoseEditorFLTKWindow():Fl_Window(10,20,inimanager.GetValueInt("PLAYER_WIN_WIDTH"),inimanager.GetValueInt("PLAYER_WIN_HEIGHT"),"Greta player")
{
	int i;
	this->color(fl_rgb_color(204,204,204));
	this->size_range(375,250,1280,1024,1,1,0);
	//channels=new ChannelsImage(5,35,100,500-45,"");
	glutw=new PlayerFLTKGLWindow(200,35,490,430,"test");
	
	rotatex=new Fl_Slider(10,115,180,20,"X");
	rotatex->type(FL_HOR_NICE_SLIDER);
	rotatex->align(FL_ALIGN_LEFT);
	rotatex->labelfont(FL_HELVETICA);
	rotatex->labelsize(8);
	rotatex->range(-180,180);
	rotatex->step(1);
	rotatex->value(0);
	rotatex->callback((Fl_Callback *)selectedrotatex,this);

	numrotatex=new Fl_Input(150,140,40,15,"");
	numrotatex->value("0");
	numrotatex->callback((Fl_Callback *)selectednumrotatex,this);

	rotatey=new Fl_Slider(10,175,180,20,"Y");
	rotatey->type(FL_HOR_NICE_SLIDER);
	rotatey->align(FL_ALIGN_LEFT);
	rotatey->labelfont(FL_HELVETICA);
	rotatey->labelsize(8);
	rotatey->range(-180,180);
	rotatey->step(1);
	rotatey->value(0);
	rotatey->callback((Fl_Callback *)selectedrotatey,this);

	numrotatey=new Fl_Input(150,200,40,15,"");
	numrotatey->value("0");
	numrotatey->callback((Fl_Callback *)selectednumrotatey,this);

	rotatez=new Fl_Slider(10,235,180,20,"Z");
	rotatez->type(FL_HOR_NICE_SLIDER);
	rotatez->align(FL_ALIGN_LEFT);
	rotatez->labelfont(FL_HELVETICA);
	rotatez->labelsize(8);
	rotatez->range(-180,180);
	rotatez->step(1);
	rotatez->value(0);
	rotatez->callback((Fl_Callback *)selectedrotatez,this);

	numrotatez=new Fl_Input(150,260,40,15,"");
	numrotatez->value("0");
	numrotatez->callback((Fl_Callback *)selectednumrotatez,this);

	jointnames_right[0]=" ";
	jointnames_right[1]="r_acromioclavicular";
	jointnames_right[2]="r_shoulder";
	jointnames_right[3]="r_elbow";
	jointnames_right[4]="r_wrist";
    jointnames_right[5]="r_thumb1";
    jointnames_right[6]="r_thumb2";
    jointnames_right[7]="r_thumb3";
    jointnames_right[8]="r_index0";
    jointnames_right[9]="r_index1";
    jointnames_right[10]="r_index2";
    jointnames_right[11]="r_index3";
    jointnames_right[12]="r_middle0";
    jointnames_right[13]="r_middle1";
    jointnames_right[14]="r_middle2";
    jointnames_right[15]="r_middle3";
    jointnames_right[16]="r_ring0";
    jointnames_right[17]="r_ring1";
    jointnames_right[18]="r_ring2";
    jointnames_right[19]="r_ring3";
    jointnames_right[20]="r_pinky0";
    jointnames_right[21]="r_pinky1";
    jointnames_right[22]="r_pinky2";
    jointnames_right[23]="r_pinky3";

	jointnames_left[0]=" ";
	jointnames_left[1]="l_acromioclavicular";
	jointnames_left[2]="l_shoulder";
	jointnames_left[3]="l_elbow";
	jointnames_left[4]="l_wrist";
    jointnames_left[5]="l_thumb1";
    jointnames_left[6]="l_thumb2";
    jointnames_left[7]="l_thumb3";
    jointnames_left[8]="l_index0";
    jointnames_left[9]="l_index1";
    jointnames_left[10]="l_index2";
    jointnames_left[11]="l_index3";
    jointnames_left[12]="l_middle0";
    jointnames_left[13]="l_middle1";
    jointnames_left[14]="l_middle2";
    jointnames_left[15]="l_middle3";
    jointnames_left[16]="l_ring0";
    jointnames_left[17]="l_ring1";
    jointnames_left[18]="l_ring2";
    jointnames_left[19]="l_ring3";
    jointnames_left[20]="l_pinky0";
    jointnames_left[21]="l_pinky1";
    jointnames_left[22]="l_pinky2";
    jointnames_left[23]="l_pinky3";

	jointnames_spine[0]=" ";
	jointnames_spine[1]="vc1";
	jointnames_spine[2]="vc2";
	jointnames_spine[3]="vc3";
	jointnames_spine[4]="vc4";
    jointnames_spine[5]="vc5";
    jointnames_spine[6]="vc6";
	jointnames_spine[7]="vc7";
	jointnames_spine[8]="vt1";
	jointnames_spine[9]="vt2";
	jointnames_spine[10]="vt3";
	jointnames_spine[11]="vt4";
	jointnames_spine[12]="vt5";
	jointnames_spine[13]="vt6";
	jointnames_spine[14]="vt7";
    jointnames_spine[15]="vt8";
    jointnames_spine[16]="vt9";
	jointnames_spine[17]="vt10";
	jointnames_spine[18]="vt11";
	jointnames_spine[19]="vt12";
	jointnames_spine[20]="vl1";
	jointnames_spine[21]="vl2";
	jointnames_spine[22]="vl3";
	jointnames_spine[23]="vl4";
	jointnames_spine[24]="vl5";

	jointname_right=new Fl_Choice(60,20,130,25,"right joints");
	jointname_right->align(FL_ALIGN_LEFT);
	jointname_right->labelfont(FL_HELVETICA);
	jointname_right->labelsize(9);
	for(i=0;i<24;i++)
		jointname_right->add(jointnames_right[i].c_str());
	jointname_right->value(0);
	jointname_right->callback((Fl_Callback *)selectedjointname_right,this);

	jointname_left=new Fl_Choice(60,50,130,25,"left joints");
	jointname_left->align(FL_ALIGN_LEFT);
	jointname_left->labelfont(FL_HELVETICA);
	jointname_left->labelsize(9);
	for(i=0;i<24;i++)
		jointname_left->add(jointnames_left[i].c_str());
	jointname_left->value(0);
	jointname_left->callback((Fl_Callback *)selectedjointname_left,this);

	jointname_spine=new Fl_Choice(60,80,130,25,"spine joints");
	jointname_spine->align(FL_ALIGN_LEFT);
	jointname_spine->labelfont(FL_HELVETICA);
	jointname_spine->labelsize(9);
	for(i=0;i<25;i++)
		jointname_spine->add(jointnames_spine[i].c_str());
	jointname_spine->value(0);
	jointname_spine->callback((Fl_Callback *)selectedjointname_spine,this);


	load=new Fl_Button(10,300,40,30,"load");
	load->labelfont(FL_HELVETICA);
	load->labelsize(12);
	load->callback((Fl_Callback *)selectedload,this);
	save=new Fl_Button(60,300,40,30,"save");
	save->labelfont(FL_HELVETICA);
	save->labelsize(12);
	save->callback((Fl_Callback *)selectedsave,this);

	this->callback((Fl_Callback *)gretaplayerwindowclosed,this);

	position_x=0;
	position_y=0;
	width=700;
	height=400;
	
	quietversion=false;

	modifyingright=false;
	modifyingleft=false;
	modifyingspine=false;
}

PoseEditorFLTKWindow::~PoseEditorFLTKWindow()
{
	delete glutw;
}

PoseEditorFLTKWindow::Load()
{
	std::string line;
	std::string filename;

	Fl_File_Chooser *chooser;
	chooser= new Fl_File_Chooser((inimanager.Program_Path+"poses").c_str(),"txt files (*.txt)",0,"select file");
	chooser->show();
	while (chooser->shown())
		Fl::wait();
	if(chooser->value()!=0)
		filename=chooser->value();
	else
		filename="";
	delete chooser;
	BAPConverter bc;
	BAPType resultbap;
	if(filename!="")
	{
		std::ifstream inputfile(filename.c_str());
		if(inputfile.is_open())
		{
			while((inputfile.rdstate()&std::ios::eofbit)==0)
			{
//				Parameter *p;
				std::getline(inputfile,line,'\n');
				if((line!="")&&(line.find("//")==std::string::npos))
				{
					std::string name;
					std::string s,t;
					float rotx,roty,rotz;
					name=line.substr(line.find_first_not_of(" "));
					name=name.substr(0,name.find_first_of(" "));
					s=line.substr(line.find_first_not_of(" "));
					if(line.find_first_of(" ")!=std::string::npos)
					{
						s=line.substr(line.find_first_of(" ")+1,line.length()-line.find_first_of(" ")-1);
						if(s.find_first_of(" ")!=std::string::npos)
							rotx=atof(s.substr(0,s.find_first_of(" ")).c_str());
						else
							rotx=atof(s.c_str());
						t=name+".rotateX";
						resultbap=bc.MJointToBAP(t);
						glutw->agent->BAPs->frames[0]->SetBAP(resultbap,rotx/180*3.14*100000);
						if(s.find_first_of(" ")!=std::string::npos)
						{
							t=s.substr(s.find_first_of(" ")+1,s.length()-s.find_first_of(" ")-1);
							if(t.find_first_of(" ")!=std::string::npos)
								roty=atof(t.substr(0,t.find_first_of(" ")).c_str());
							else
								roty=atof(t.c_str());
							s=name+".rotateY";
							resultbap=bc.MJointToBAP(s);
							glutw->agent->BAPs->frames[0]->SetBAP(resultbap,roty/180*3.14*100000);
							if(t.find_first_of(" ")!=std::string::npos)
							{
								s=t.substr(t.find_first_of(" ")+1,t.length()-t.find_first_of(" ")-1);
								if(s.find_first_of(" ")!=std::string::npos)
									rotz=atof(s.substr(0,s.find_first_of(" ")).c_str());
								else
									rotz=atof(s.c_str());
								t=name+".rotateZ";
								resultbap=bc.MJointToBAP(t);
								glutw->agent->BAPs->frames[0]->SetBAP(resultbap,rotz/180*3.14*100000);
							}
						}
					}
				}
			}
		}
		else
			return 0;
		inputfile.close();
		this->rotatex->value(0);
		this->rotatey->value(0);
		this->rotatez->value(0);
	}
}

PoseEditorFLTKWindow::Save()
{

	std::string filename;

	Fl_File_Chooser *chooser;
	chooser= new Fl_File_Chooser((inimanager.Program_Path+"poses").c_str(),"txt files (*.txt)",0|2,"select file");
	chooser->show();
	while (chooser->shown())
		Fl::wait();
	if(chooser->value()!=0)
		filename=chooser->value();
	else
		filename="";
	delete chooser;
	BAPConverter bc;
	BAPType resultbap;
	if(filename!="")
	{
		std::ofstream outputfile(filename.c_str());

		if(outputfile.is_open())
		{
			outputfile << "//pose file generated by the pose editor\n";
			outputfile << "posename\n";
			outputfile << "r_sternoclavicular"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_sternoclavicular.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_sternoclavicular.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_sternoclavicular.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "r_acromioclavicular"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_acromioclavicular.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_acromioclavicular.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_acromioclavicular.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "r_shoulder" 
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_shoulder.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_shoulder.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_shoulder.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "r_elbow"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_elbow.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_elbow.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_elbow.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "r_wrist"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_wrist.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_wrist.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_wrist.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "r_thumb1"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_thumb1.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_thumb1.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_thumb1.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "r_thumb2"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_thumb2.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_thumb2.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_thumb2.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "r_thumb3"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_thumb3.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_thumb3.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_thumb3.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "r_index0"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_index0.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_index0.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_index0.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "r_index1"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_index1.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_index1.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_index1.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "r_index2"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_index2.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_index2.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_index2.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "r_index3"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_index3.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_index3.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_index3.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "r_middle0"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_middle0.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_middle0.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_middle0.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "r_middle1"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_middle1.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_middle1.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_middle1.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "r_middle2"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_middle2.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_middle2.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_middle2.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "r_middle3"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_middle3.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_middle3.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_middle3.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "r_ring0"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_ring0.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_ring0.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_ring0.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "r_ring1"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_ring1.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_ring1.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_ring1.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "r_ring2"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_ring2.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_ring2.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_ring2.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "r_ring3"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_ring3.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_ring3.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_ring3.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "r_pinky0"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_pinky0.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_pinky0.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_pinky0.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "r_pinky1"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_pinky1.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_pinky1.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_pinky1.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "r_pinky2"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_pinky2.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_pinky2.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_pinky2.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "r_pinky3"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_pinky3.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_pinky3.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("r_pinky3.rotateZ")/100000.0f/3.14f*180 << "\n";

			outputfile << "l_sternoclavicular"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_sternoclavicular.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_sternoclavicular.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_sternoclavicular.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "l_acromioclavicular"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_acromioclavicular.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_acromioclavicular.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_acromioclavicular.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "l_shoulder" 
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_shoulder.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_shoulder.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_shoulder.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "l_elbow"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_elbow.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_elbow.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_elbow.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "l_wrist"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_wrist.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_wrist.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_wrist.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "l_thumb1"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_thumb1.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_thumb1.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_thumb1.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "l_thumb2"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_thumb2.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_thumb2.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_thumb2.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "l_thumb3"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_thumb3.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_thumb3.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_thumb3.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "l_index0"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_index0.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_index0.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_index0.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "l_index1"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_index1.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_index1.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_index1.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "l_index2"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_index2.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_index2.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_index2.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "l_index3"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_index3.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_index3.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_index3.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "l_middle0"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_middle0.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_middle0.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_middle0.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "l_middle1"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_middle1.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_middle1.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_middle1.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "l_middle2"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_middle2.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_middle2.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_middle2.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "l_middle3"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_middle3.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_middle3.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_middle3.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "l_ring0"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_ring0.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_ring0.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_ring0.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "l_ring1"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_ring1.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_ring1.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_ring1.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "l_ring2"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_ring2.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_ring2.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_ring2.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "l_ring3"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_ring3.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_ring3.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_ring3.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "l_pinky0"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_pinky0.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_pinky0.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_pinky0.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "l_pinky1"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_pinky1.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_pinky1.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_pinky1.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "l_pinky2"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_pinky2.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_pinky2.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_pinky2.rotateZ")/100000.0f/3.14f*180 << "\n";
			outputfile << "l_pinky3"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_pinky3.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_pinky3.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("l_pinky3.rotateZ")/100000.0f/3.14f*180 << "\n";

			outputfile << "vc1"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vc1.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vc1.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vc1.rotateZ")/100000.0f/3.14f*180 << "\n";

			outputfile << "vc2"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vc2.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vc2.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vc2.rotateZ")/100000.0f/3.14f*180 << "\n";

			outputfile << "vc3"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vc3.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vc3.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vc3.rotateZ")/100000.0f/3.14f*180 << "\n";

			outputfile << "vc4"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vc4.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vc4.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vc4.rotateZ")/100000.0f/3.14f*180 << "\n";

			outputfile << "vc5"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vc5.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vc5.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vc5.rotateZ")/100000.0f/3.14f*180 << "\n";

			outputfile << "vc6"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vc6.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vc6.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vc6.rotateZ")/100000.0f/3.14f*180 << "\n";

			outputfile << "vc7"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vc7.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vc7.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vc7.rotateZ")/100000.0f/3.14f*180 << "\n";

			outputfile << "vt1"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vt1.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vt1.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vt1.rotateZ")/100000.0f/3.14f*180 << "\n";

			outputfile << "vt2"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vt2.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vt2.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vt2.rotateZ")/100000.0f/3.14f*180 << "\n";

			outputfile << "vt3"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vt3.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vt3.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vt3.rotateZ")/100000.0f/3.14f*180 << "\n";

			outputfile << "vt4"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vt4.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vt4.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vt4.rotateZ")/100000.0f/3.14f*180 << "\n";

			outputfile << "vt5"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vt5.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vt5.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vt5.rotateZ")/100000.0f/3.14f*180 << "\n";

			outputfile << "vt6"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vt6.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vt6.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vt6.rotateZ")/100000.0f/3.14f*180 << "\n";

			outputfile << "vt7"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vt7.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vt7.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vt7.rotateZ")/100000.0f/3.14f*180 << "\n";

			outputfile << "vt8"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vt8.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vt8.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vt8.rotateZ")/100000.0f/3.14f*180 << "\n";

			outputfile << "vt9"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vt9.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vt9.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vt9.rotateZ")/100000.0f/3.14f*180 << "\n";

			outputfile << "vt10"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vt10.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vt10.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vt10.rotateZ")/100000.0f/3.14f*180 << "\n";

			outputfile << "vt11"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vt11.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vt11.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vt11.rotateZ")/100000.0f/3.14f*180 << "\n";

			outputfile << "vt12"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vt12.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vt12.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vt12.rotateZ")/100000.0f/3.14f*180 << "\n";

			outputfile << "vl1"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vl1.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vl1.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vl1.rotateZ")/100000.0f/3.14f*180 << "\n";

			outputfile << "vl2"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vl2.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vl2.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vl2.rotateZ")/100000.0f/3.14f*180 << "\n";

			outputfile << "vl3"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vl3.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vl3.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vl3.rotateZ")/100000.0f/3.14f*180 << "\n";

			outputfile << "vl4"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vl4.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vl4.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vl4.rotateZ")/100000.0f/3.14f*180 << "\n";

			outputfile << "vl5"
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vl5.rotateX")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vl5.rotateY")/100000.0f/3.14f*180
			<< " " << glutw->agent->BAPs->frames[0]->GetBAP("vl5.rotateZ")/100000.0f/3.14f*180 << "\n";

			outputfile.close();
		}
	}
}

//DEL void PoseEditorFLTKWindow::draw()
//DEL {
//DEL 	channels->resize(5,5,width-10,height-10);
//DEL }

//DEL void PoseEditorFLTKWindow::resize(int x, int y, int w, int h)
//DEL {
//DEL 	position_x=x;
//DEL 	position_y=y;
//DEL 	width=w;
//DEL 	height=h;
//DEL 	channels->resize(5,5,width-10,height-10);
//DEL }

int PoseEditorFLTKWindow::handle(int event)
{

	return Fl_Window::handle(event);
}

void PoseEditorFLTKWindow::draw()
{
	Fl_Window::draw();
//	menu->size(this->w(),30);
	glutw->size(this->w()-200,this->h()-70);
	///glutw->invalidate();

	
}
