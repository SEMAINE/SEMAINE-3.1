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

// GestureTesterWindow.cpp: implementation of the GestureTesterWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "GestureTesterWindow.h"
#include "IniManager.h"
#include "GestureEngine.h"
#include "gestures/GestureFileNames.h"
#include "FaceExpressionDictionary.h"
#include "BML_AgentEngine.h"
#include <stdio.h>
#include <string.h>
#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <fl/fl_widget.h>
#include <FL/fl_draw.H>
#include <FL/Fl_File_Chooser.H>
#include <crtdbg.h>

IniManager inimanager;
std::string ini_filename;
extern FILE* face_log;
extern BAPConverter GestureGlobalBAPconverter;

void GestureTesterWindowclosed(void* v)
{
}

GestureTesterWindow::GestureTesterWindow():Fl_Window(100,140,300,150,"gesture tester 0.1b")
{
	

	
	//duration=new Fl_Output(80,50,40,17,"duration:");
	//duration->align(FL_ALIGN_LEFT);
	//duration->labelfont(FL_HELVETICA);
	//duration->labelsize(11);
	//duration->textfont(FL_HELVETICA);
	//duration->textsize(11);
	//duration->value("1.0s");

	//durationminus=new Fl_Button(130,50,17,17,"-");
	//durationplus=new Fl_Button(147,50,17,17,"+");


	testgesture=new Fl_Button(40,110,100,30,"test");
	testgesture->labelfont(FL_HELVETICA);
	testgesture->labelsize(11);

	quit=new Fl_Button(150,110,100,30,"quit");
	quit->labelfont(FL_HELVETICA);
	quit->labelsize(11);

	gesturename=new Fl_Choice(100,15,190,20,"gesture:");
	//gesturename->add("");

	this->callback((Fl_Callback *)GestureTesterWindowclosed,this);

	actualframe=0;
	
	if(inimanager.initialized==false)
		inimanager.ReadIniFile("greta.ini");

	GestureFileNames filenames;
	bool flag=false;
	std::string gesturefilename,gestureclass,gestureinstance;
	while(flag==false)
	{

		gesturefilename=inimanager.Program_Path+"gestures/"+filenames.GetFileName();
		//Insert(mReader.LoadGesture(string("gestures\\")+filenames.GetFileName()));
		
		ifstream infile(gesturefilename.c_str());

		std::string theLine,classifier,tempString;
		std::vector<string>words;

		while (getline(infile,theLine,'\n'))
		{
		
			// SKIP COMMENT LINES AND EMTPY LINES
			if( theLine.compare(0,2,string("//"))==0)
			{
					continue;
			}
			if (theLine.size()==0)
			{
				continue;
			}
			// TOKENIZE LINE INTO WORDS
			theLine=theLine+ " ";
			words.clear();
			string::size_type pos=0, prev_pos=0;
			while ((pos=theLine.find_first_of(' ',pos))!=string::npos)
			{
				tempString = theLine.substr(prev_pos, pos-prev_pos);
				
				// CONVERT EACH WORD TO UPPERCASE
				tempString = string(_strupr((char *)tempString.c_str()));
				
				if (tempString.length() != 0)
					words.push_back(tempString);
				prev_pos= ++pos;
			}
			
			int tokens = words.size();
			
			// BRANCH ACCORDING TO LINE CLASSIFIER
			classifier = words[0];
			if (classifier.compare("GESTURECLASS")==0) 
			{
				gestureclass=words[1];
			} 
			else if (classifier.compare("GESTUREINSTANCE")==0) 
			{
				gestureinstance=words[1];
				gesturename->add((gestureclass+"="+gestureinstance).c_str());
			} 
		
		}		
		
		if(filenames.Next()==0)flag=true;
	}

	gesturename->value(0);

	expressionname=new Fl_Choice(100,45,190,20,"expression:");
	//expressionname->add("");

	face_log=fopen((inimanager.Program_Path+"logs/face_log.txt").c_str(),"w");
	FaceExpressionDictionary facelibrary;
	
	facelibrary.Init("fd/facelibrary.xml");

	std::string s;
	std::list<FaceExpression>::iterator iter;
	for(iter=facelibrary.FExpressions.begin();iter!=facelibrary.FExpressions.end();iter++)
	{
		//printf("cerco %s e trovo %s\n",(*class_iter).c_str(),(*iter).classname.c_str());
		if((*iter).classname=="affect")
			expressionname->add((*iter).instance.c_str());
	}

	expressionname->value(0);
	
	fclose(face_log);

	expressionfilename=new Fl_Input(100,75,190,20,"FAP file:");

	agentwindow=0;

	lastbf=new BAPFrame(&GestureGlobalBAPconverter);
}

GestureTesterWindow::~GestureTesterWindow()
{
	delete agentwindow;
	delete expressivitywindow;
}

int GestureTesterWindow::handle(int e)
{
	int i,k;

	/*	
	if(Fl::event_inside(durationminus))
		if(Fl::event_button()==FL_LEFT_MOUSE)
			if(e==FL_PUSH)
			{
				if(gesture->GetDuration()>0.05)
				{
					char d[10];
					gesture->SetDuration(gesture->GetDuration()-0.05);
					sprintf(d,"%.2fs",gesture->GetDuration());
					duration->value(d);
				}
			}

	if(Fl::event_inside(durationplus))
		if(Fl::event_button()==FL_LEFT_MOUSE)
			if(e==FL_PUSH)
			{
				char d[10];
				gesture->SetDuration(gesture->GetDuration()+0.05);
				sprintf(d,"%.2fs",gesture->GetDuration());
				duration->value(d);
			}
 */
	
	if(Fl::event_inside(testgesture))
		if(Fl::event_button()==FL_LEFT_MOUSE)
			if(e==FL_PUSH)
			{
				Test();
			}

	if(Fl::event_inside(quit))
		if(Fl::event_button()==FL_LEFT_MOUSE)
			if(e==FL_PUSH)
			{
				agentwindow->hide();
				expressivitywindow->hide();
				this->hide();
			}

	return Fl_Window::handle(e);
}

void GestureTesterWindow::draw()
{
	Fl_Window::draw();

	if(agentwindow==0)
	{
		agentwindow=new GLContainer();
		agentwindow->show();
		//agentwindow->clear_border();

		expressivitywindow=new ExpressivityWindow();
		expressivitywindow->show();

		expressivitywindow->SPC->value(inimanager.GetValueFloat("ENGINE_EXPR_SPC"));
		expressivitywindow->TMP->value(inimanager.GetValueFloat("ENGINE_EXPR_TMP"));
		expressivitywindow->FLD->value(inimanager.GetValueFloat("ENGINE_EXPR_FLD"));
		expressivitywindow->PWR->value(inimanager.GetValueFloat("ENGINE_EXPR_PWR"));
	}

	
}

void GestureTesterWindow::ShowActualFrame()
{
	
}

void GestureTesterWindow::Test()
{
	
	char s[10];

	GestureSpace::MotorPlanner	*mMPlanner;
	GestureSpace::GesturePlanner	*mGPlanner;


	inimanager.SetValueFloat("ENGINE_EXPR_SPC",expressivitywindow->SPC->value());
	inimanager.SetValueFloat("ENGINE_EXPR_TMP",expressivitywindow->TMP->value());
	inimanager.SetValueFloat("ENGINE_EXPR_FLD",expressivitywindow->FLD->value());
	inimanager.SetValueFloat("ENGINE_EXPR_PWR",expressivitywindow->PWR->value());

	inimanager.WriteIniFile("greta.ini");

	float duration;
	duration=1-(inimanager.GetValueFloat("ENGINE_EXPR_TMP")-1);

	sprintf(s,"%f",duration);

	std::string line;

	std::ofstream outputfile((inimanager.Program_Path+"bml/gesture_tester.xml").c_str());

	if(outputfile.is_open())
	{
		outputfile << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n<!DOCTYPE bml SYSTEM \"bml.dtd\" []>\n";
		outputfile << "<bml>\n";
		
		outputfile << "<face ";
		outputfile << "id='face_expression' ";
		outputfile << "type='FACS' ";
		outputfile << "start='0.0' ";
		outputfile << "end='5.0'>\n";
		outputfile << "<description level=\"1\" type=\"gretabml\">\n";
		outputfile << "<reference>";
		if(strcmp(expressionfilename->value(),"")==0)
			outputfile << "affect=" << expressionname->text();
		else
			outputfile << "fapfile:" << expressionfilename->value();
		outputfile << "</reference>";
		outputfile << "</description>\n";
		outputfile << "</face>\n";
		
		outputfile << "<gesture ";
		outputfile << "id='gesture' ";
		outputfile << "type='BEAT' ";
		outputfile << "start='1.0' ";
		outputfile << "end='" << s << "'>\n";

		outputfile << "<description level=\"1\" type=\"gretabml\">\n";
		outputfile << "<reference>" << gesturename->text() << "</reference>";

		float incr,stroke;
		incr=duration/(expressivitywindow->REP->value()+2);

		stroke=0;
		
		for(int i=0;i<=expressivitywindow->REP->value();i++)
		{
			stroke=stroke+incr;
			sprintf(s,"%f",stroke);
			outputfile << "<stroke ";
			outputfile << "time='" << s << "'/>\n";
		}

		outputfile << "</description>\n";
		outputfile << "</gesture>\n";

		outputfile << "</bml>\n";


		outputfile.close();

		BML_AgentEngine BAE;
		char t[10];
		sprintf(t,"");

		BAE.Execute(inimanager.Program_Path+"bml/gesture_tester.xml",t,1,lastbf);

		BAE.GetMPlanner()->GetCopyLastBAPFrame(lastbf);
	}

	agentwindow->Play(inimanager.Program_Path+"output/gesture_tester");
}

