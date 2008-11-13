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

// FacePartsToolWindow.cpp: implementation of the FacePartsToolWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "FacePartsToolWindow.h"
#include <stdio.h>
#include <string.h>
#include <list>
#include <string>
#include <fl/Fl_File_Chooser.h>
#include <FL/Fl.H>
#include "IniManager.h"
#include ".\facepartstoolwindow.h"

IniManager inimanager;

FILE *face_log;

FacePartsToolWindow::FacePartsToolWindow():Fl_Window(10,20,700,600,"agent score 0.1")
{
	this->size_range(375,250,1280,1024,1,1,0);

	inimanager.ReadIniFile("greta.ini");

	//face_log=fopen("logs/face_dictionary_log.txt","w");
	//this->size_range(800,600,800,600,1,1,0);
	//channels=new ChannelsImage(5,35,100,500-45,"");

	glutw=new PlayerFLTKGLWindow(220,35,470,500,"test");

	box_eyes= new Fl_Box(FL_BORDER_BOX, 3, 18, 200, 200,"Eyes");
	box_eyes->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);

	leyexplus=new Fl_Button(10,30,20,20,"@+");
	leyexminus=new Fl_Button(10,50,20,20,"@line");

	leyeyplus=new Fl_Button(40,30,20,20,"@+");
	leyeyminus=new Fl_Button(40,50,20,20,"@line");

	leyezplus=new Fl_Button(70,30,20,20,"@+");
	leyezminus=new Fl_Button(70,50,20,20,"@line");

	reyexplus=new Fl_Button(120,30,20,20,"@+");
	reyexminus=new Fl_Button(120,50,20,20,"@line");

	reyeyplus=new Fl_Button(150,30,20,20,"@+");
	reyeyminus=new Fl_Button(150,50,20,20,"@line");

	reyezplus=new Fl_Button(180,30,20,20,"@+");
	reyezminus=new Fl_Button(180,50,20,20,"@line");

	leyex=new Fl_Output(30,90,30,16,"L X");
	leyex->align(FL_ALIGN_LEFT);
	leyey=new Fl_Output(90,90,30,16,"L Y");
	leyey->align(FL_ALIGN_LEFT);
	leyez=new Fl_Output(150,90,30,16,"L Z");
	leyez->align(FL_ALIGN_LEFT);

	reyex=new Fl_Output(30,120,30,16,"R X");
	reyex->align(FL_ALIGN_LEFT);
	reyey=new Fl_Output(90,120,30,16,"R Y");
	reyey->align(FL_ALIGN_LEFT);
	reyez=new Fl_Output(150,120,30,16,"R Z");
	reyez->align(FL_ALIGN_LEFT);

	eyescaleplus=new Fl_Button(10,160,20,20,"@+");
	eyescaleminus=new Fl_Button(10,190,20,20,"@line");

	eyescale=new Fl_Output(100,160,30,16,"SCALE");
	eyescale->align(FL_ALIGN_LEFT);
	
	box_teeth= new Fl_Box(FL_BORDER_BOX, 3, 240, 200, 100,"Teeth");
	box_teeth->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);

	teethsxplus=new Fl_Button(10,250,20,20,"@+");
	teethsxminus=new Fl_Button(10,280,20,20,"@line");

	teethsyplus=new Fl_Button(40,250,20,20,"@+");
	teethsyminus=new Fl_Button(40,280,20,20,"@line");

	teethszplus=new Fl_Button(70,250,20,20,"@+");
	teethszminus=new Fl_Button(70,280,20,20,"@line");

	teethsx=new Fl_Output(30,310,30,16,"T X");
	teethsx->align(FL_ALIGN_LEFT);
	teethsy=new Fl_Output(90,310,30,16,"T Y");
	teethsy->align(FL_ALIGN_LEFT);
	teethsz=new Fl_Output(150,310,30,16,"T Z");
	teethsz->align(FL_ALIGN_LEFT);

	box_head= new Fl_Box(FL_BORDER_BOX, 3, 360, 120, 70,"Head Translation");
	box_head->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);

	headxplus=new Fl_Button(15,370,20,20,"@+");
	headxminus=new Fl_Button(15,400,20,20,"@line");

	headyplus=new Fl_Button(50,370,20,20,"@+");
	headyminus=new Fl_Button(50,400,20,20,"@line");

	headzplus=new Fl_Button(85,370,20,20,"@+");
	headzminus=new Fl_Button(85,400,20,20,"@line");

	box_head_scale=new Fl_Box(FL_BORDER_BOX, 140, 360, 60, 70,"Head Scale");
	box_head_scale->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);

	headscaleplus=new Fl_Button(160,370,20,20,"@+");
	headscaleminus=new Fl_Button(160,400,20,20,"@line");

	box_hair=new Fl_Box(FL_BORDER_BOX, 5, 465, 200, 60,"Hair position and scale");
	box_hair->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);

	hairpxminus=new Fl_Button(10,500,20,20,"@line");
	hairpxplus=new Fl_Button(10,470,20,20,"@+");
	hairpyminus=new Fl_Button(40,500,20,20,"@line");
	hairpyplus=new Fl_Button(40,470,20,20,"@+");
	hairpzminus=new Fl_Button(70,500,20,20,"@line");
	hairpzplus=new Fl_Button(70,470,20,20,"@+");

	hairsxminus=new Fl_Button(120,500,20,20,"@line");
	hairsxplus=new Fl_Button(120,470,20,20,"@+");
	hairsyminus=new Fl_Button(150,500,20,20,"@line");
	hairsyplus=new Fl_Button(150,470,20,20,"@+");
	hairszminus=new Fl_Button(180,500,20,20,"@line");
	hairszplus=new Fl_Button(180,470,20,20,"@+");

	switchhair=new Fl_Button(10,540,40,20,"hair");
	switchwireframe=new Fl_Button(60,540,80,20,"wireframe");

	save=new Fl_Button(10,570,40,20,"save");
	

}

int FacePartsToolWindow::handle(int event)
{
	char v[10];
	if(Fl::event_button()==FL_LEFT_MOUSE)
	{
		if(event==FL_PUSH)
		{
			if(Fl::event_inside(leyexplus))
			{
				glutw->agent->facedata->EYES_LX+=0.1f;
				sprintf(v,"%.2f",glutw->agent->facedata->EYES_LX);
				leyex->value(v);
			}
			if(Fl::event_inside(leyexminus))
			{
				glutw->agent->facedata->EYES_LX-=0.1f;
				sprintf(v,"%.2f",glutw->agent->facedata->EYES_LX);
				leyex->value(v);
			}
			if(Fl::event_inside(leyeyplus))
			{
				glutw->agent->facedata->EYES_LY+=0.1f;
				sprintf(v,"%.2f",glutw->agent->facedata->EYES_LY);
				leyey->value(v);
			}
			if(Fl::event_inside(leyeyminus))
			{
				glutw->agent->facedata->EYES_LY-=0.1f;
				sprintf(v,"%.2f",glutw->agent->facedata->EYES_LY);
				leyey->value(v);
			}
			if(Fl::event_inside(leyezplus))
			{
				glutw->agent->facedata->EYES_LZ+=0.1f;
				sprintf(v,"%.2f",glutw->agent->facedata->EYES_LZ);
				leyez->value(v);
			}
			if(Fl::event_inside(leyezminus))
			{
				glutw->agent->facedata->EYES_LZ-=0.1f;
				sprintf(v,"%.2f",glutw->agent->facedata->EYES_LZ);
				leyez->value(v);
			}
			if(Fl::event_inside(reyexplus))
			{
				glutw->agent->facedata->EYES_RX+=0.1f;
				sprintf(v,"%.2f",glutw->agent->facedata->EYES_RX);
				reyex->value(v);
			}
			if(Fl::event_inside(reyexminus))
			{
				glutw->agent->facedata->EYES_RX-=0.1f;
				sprintf(v,"%.2f",glutw->agent->facedata->EYES_RX);
				reyex->value(v);
			}
			if(Fl::event_inside(reyeyplus))
			{
				glutw->agent->facedata->EYES_RY+=0.1f;
				sprintf(v,"%.2f",glutw->agent->facedata->EYES_RY);
				reyey->value(v);
		}
			if(Fl::event_inside(reyeyminus))
			{
				glutw->agent->facedata->EYES_RY-=0.1f;
				sprintf(v,"%.2f",glutw->agent->facedata->EYES_RY);
				reyey->value(v);
			}
			if(Fl::event_inside(reyezplus))
			{
				glutw->agent->facedata->EYES_RZ+=0.1f;
				sprintf(v,"%.2f",glutw->agent->facedata->EYES_RZ);
				reyez->value(v);
			}
			if(Fl::event_inside(reyezminus))
			{
				glutw->agent->facedata->EYES_RZ-=0.1f;
				sprintf(v,"%.2f",glutw->agent->facedata->EYES_RZ);
				reyez->value(v);
			}
			if(Fl::event_inside(eyescaleminus))
			{
				glutw->agent->facedata->EYES_SCALE-=0.1;
				sprintf(v,"%.2f",glutw->agent->facedata->EYES_SCALE);
				eyescale->value(v);
			}
			if(Fl::event_inside(eyescaleplus))
			{
				glutw->agent->facedata->EYES_SCALE+=0.1;
				sprintf(v,"%.2f",glutw->agent->facedata->EYES_SCALE);
				eyescale->value(v);
			}
			if(Fl::event_inside(teethsxminus))
			{
				glutw->agent->facedata->TEETHS_X-=0.1;
				sprintf(v,"%.2f",glutw->agent->facedata->TEETHS_X);
				teethsx->value(v);
			}
			if(Fl::event_inside(teethsxplus))
			{
				glutw->agent->facedata->TEETHS_X+=0.1;
				sprintf(v,"%.2f",glutw->agent->facedata->TEETHS_X);
				teethsx->value(v);
			}
			if(Fl::event_inside(teethsyminus))
			{
				glutw->agent->facedata->TEETHS_Y-=0.1;
				sprintf(v,"%.2f",glutw->agent->facedata->TEETHS_Y);
				teethsy->value(v);
			}
			if(Fl::event_inside(teethsyplus))
			{
				glutw->agent->facedata->TEETHS_Y+=0.1;
				sprintf(v,"%.2f",glutw->agent->facedata->TEETHS_Y);
				teethsy->value(v);
			}
			if(Fl::event_inside(teethszminus))
			{
				glutw->agent->facedata->TEETHS_Z-=0.1;
				sprintf(v,"%.2f",glutw->agent->facedata->TEETHS_Z);
				teethsz->value(v);
			}
			if(Fl::event_inside(teethszplus))
			{
				glutw->agent->facedata->TEETHS_Z+=0.1;
				sprintf(v,"%.2f",glutw->agent->facedata->TEETHS_Z);
				teethsz->value(v);
			}

			if(Fl::event_inside(headxminus))
			{
				glutw->agent->facedata->HEADPOS_X-=1;
			}
			if(Fl::event_inside(headxplus))
			{
				glutw->agent->facedata->HEADPOS_X+=1;
			}
			if(Fl::event_inside(headyminus))
			{
				glutw->agent->facedata->HEADPOS_Y-=1;
			}
			if(Fl::event_inside(headyplus))
			{
				glutw->agent->facedata->HEADPOS_Y+=1;
			}
			if(Fl::event_inside(headzminus))
			{
				glutw->agent->facedata->HEADPOS_Z-=1;
			}
			if(Fl::event_inside(headzplus))
			{
				glutw->agent->facedata->HEADPOS_Z+=1;
			}
			if(Fl::event_inside(headscaleminus))
			{
				glutw->agent->facedata->HEADSCALE-=0.02;
			}
			if(Fl::event_inside(headscaleplus))
			{
				glutw->agent->facedata->HEADSCALE+=0.02;
			}

			if(Fl::event_inside(hairpxplus))
			{
				glutw->agent->facedata->HAIR_tx+=0.2;
			}
			if(Fl::event_inside(hairpyplus))
			{
				glutw->agent->facedata->HAIR_ty+=0.2;
			}
			if(Fl::event_inside(hairpzplus))
			{
				glutw->agent->facedata->HAIR_tz+=0.2;
			}

			if(Fl::event_inside(hairpxminus))
			{
				glutw->agent->facedata->HAIR_tx-=0.2;
			}
			if(Fl::event_inside(hairpyminus))
			{
				glutw->agent->facedata->HAIR_ty-=0.2;
			}
			if(Fl::event_inside(hairpzminus))
			{
				glutw->agent->facedata->HAIR_tz-=0.2;
			}

			if(Fl::event_inside(hairsxplus))
			{
				glutw->agent->facedata->HAIR_sx+=1;
			}
			if(Fl::event_inside(hairsyplus))
			{
				glutw->agent->facedata->HAIR_sy+=1;
			}
			if(Fl::event_inside(hairszplus))
			{
				glutw->agent->facedata->HAIR_sz+=1;
			}

			if(Fl::event_inside(hairsxminus))
			{
				glutw->agent->facedata->HAIR_sx-=1;
			}
			if(Fl::event_inside(hairsyminus))
			{
				glutw->agent->facedata->HAIR_sy-=1;
			}
			if(Fl::event_inside(hairszminus))
			{
				glutw->agent->facedata->HAIR_sz-=1;
			}


			if(Fl::event_inside(switchhair))
			{
				glutw->agent->SwitchHair();
			}
			if(Fl::event_inside(switchwireframe))
			{
				glutw->agent->SwitchWire1();
				glutw->agent->SwitchSkin();
			}
			if(Fl::event_inside(save))
			{
				this->WriteFile();
			}
		}
	}
	return Fl_Window::handle(event);
}

void FacePartsToolWindow::draw()
{
	glutw->size(this->w()-200,this->h()-70);
	glutw->invalidate();
	Fl_Window::draw();
}


FacePartsToolWindow::~FacePartsToolWindow()
{
}

void FacePartsToolWindow::WriteFile(void)
{
	Fl_File_Chooser *chooser;
	chooser= new Fl_File_Chooser(inimanager.Program_Path.c_str(),"face definition files (*.xml)",2,"select a face definition file");
	chooser->show();
	while (chooser->shown())
		Fl::wait();
	std::string s;
	s="";
	if(chooser->value()!=0)
		s=chooser->value();
	if(s!="")
	{
		std::string line;

		std::ofstream outputfile(s.c_str());

		if(outputfile.is_open())
		{
			outputfile << "<?xml version=\"1.0\"?>\n";
			outputfile << "<!DOCTYPE PARTS_DATA SYSTEM \"../facedefinition.dtd\" []>\n";

			outputfile << "<PARTS_DATA>\n";
			outputfile << "<HEADPOSITION x=\"" << glutw->agent->facedata->HEADPOS_X;
			outputfile << "\" y=\"" << glutw->agent->facedata->HEADPOS_Y;
			outputfile << "\" z=\"" << glutw->agent->facedata->HEADPOS_Z;
			outputfile << "\" scale=\"" << glutw->agent->facedata->HEADSCALE;
			outputfile << "\"/>\n";
			outputfile << "<LEFT_EYE x=\""<< glutw->agent->facedata->EYES_LX;
			outputfile << "\" y=\"" << glutw->agent->facedata->EYES_LY;
			outputfile << "\" z=\"" <<glutw->agent->facedata->EYES_LZ <<"\"/>\n";
			outputfile << "<RIGHT_EYE x=\""<< glutw->agent->facedata->EYES_RX;
			outputfile << "\" y=\"" <<glutw->agent->facedata->EYES_RY;
			outputfile << "\" z=\"" <<glutw->agent->facedata->EYES_RZ <<"\"/>\n";
			outputfile << "<EYE_SCALE factor=\"" << glutw->agent->facedata->EYES_SCALE<<"\"/>\n";
			outputfile << "<TEETHS-TONGUE x=\"" << glutw->agent->facedata->TEETHS_X;
			outputfile << "\" y=\"" << glutw->agent->facedata->TEETHS_Y;
			outputfile << "\" z=\"" << glutw->agent->facedata->TEETHS_Z << "\"/>\n";
			outputfile << "<HAIRPOSITION x=\"" << glutw->agent->facedata->HAIR_tx;
			outputfile << "\" y=\"" << glutw->agent->facedata->HAIR_ty;
			outputfile << "\" z=\"" << glutw->agent->facedata->HAIR_tz << "\"";
			outputfile << " scalex=\"" << glutw->agent->facedata->HAIR_sx;
			outputfile << "\" scaley=\"" << glutw->agent->facedata->HAIR_sy;
			outputfile << "\" scalez=\"" << glutw->agent->facedata->HAIR_sz << "\"/>\n";
			outputfile << "</PARTS_DATA>\n";
			outputfile.close();
		}
	}
	delete chooser;
}
