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

// ScoreWindow.cpp: implementation of the ScoreWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "ScoreWindow.h"
#include "XMLGenericParser.h"
//#include "MSSL_AgentEngine.h"
#include "BML_AgentEngine.h"
#include <stdio.h>
#include <string.h>
#include <list>
#include <string>
#include <fl/fl_widget.h>
#include <FL/fl_draw.H>
#include "FL/Fl_File_Chooser.h"
#include "FileNames.h"
#include "IniManager.h"

extern FILE* face_log;
extern IniManager inimanager;
extern FileNames filenames;

void selectedmenubar(Fl_Widget* w, void* v)
{
	int pos;
	std::string s;
	
	if(strcmp(((ScoreWindow*)v)->menubar->text(),"New")==0)
	{
		((ScoreWindow*)v)->DeleteAll();
		((ScoreWindow*)v)->TestoVec.clear();
		((ScoreWindow*)v)->SignalsVec.clear();
		((ScoreWindow*)v)->PhonemesVec.clear();
		((ScoreWindow*)v)->speech->Clear();
		((ScoreWindow*)v)->speech->modality="speech";
		((ScoreWindow*)v)->speech->type="application/wav";
		((ScoreWindow*)v)->speech->voice="mary.female";
		((ScoreWindow*)v)->speech->language="english";
		((ScoreWindow*)v)->maxtimevalue->value("5.0");
		((ScoreWindow*)v)->SetMaxTimeValue();
		((ScoreWindow*)v)->tm=0;
		((ScoreWindow*)v)->bmlfilename=inimanager.Program_Path+"bml/empty.xml";
		((ScoreWindow*)v)->LoadSpeaker();
		if(((ScoreWindow*)v)->loadlistener==1)
			((ScoreWindow*)v)->LoadListener();
		((ScoreWindow*)v)->label("BML Editor 2.0");
		
	}

	if(strcmp(((ScoreWindow*)v)->menubar->text(),"Load")==0)
	{
		Fl_File_Chooser *chooser;
		chooser= new Fl_File_Chooser((inimanager.Program_Path+"bml").c_str(),"XML files (*.xml)",0,"select file");
		chooser->show();
		while (chooser->shown())
			Fl::wait();
		if(chooser->value()!=0)
		{
			((ScoreWindow*)v)->maxtimevalue->value("5.0");
			((ScoreWindow*)v)->SetMaxTimeValue();
			((ScoreWindow*)v)->bmlfilename=chooser->value();
			((ScoreWindow*)v)->DeleteAll();
			((ScoreWindow*)v)->TestoVec.clear();
			((ScoreWindow*)v)->SignalsVec.clear();
			((ScoreWindow*)v)->PhonemesVec.clear();
			((ScoreWindow*)v)->speech->Clear();
			((ScoreWindow*)v)->speech->modality="speech";
			((ScoreWindow*)v)->speech->type="application/wav";
			((ScoreWindow*)v)->speech->voice="mary.female";
			((ScoreWindow*)v)->speech->language="english";
			((ScoreWindow*)v)->tm=0;
			((ScoreWindow*)v)->LoadSpeaker();
			if(((ScoreWindow*)v)->loadlistener==1)
				((ScoreWindow*)v)->LoadListener();
			pos = ((ScoreWindow*)v)->bmlfilename.find_last_of("/");
			s="BML Editor 2.0 - " + ((ScoreWindow*)v)->bmlfilename.substr(pos+1, ((ScoreWindow*)v)->bmlfilename.length());
			((ScoreWindow*)v)->label(s.c_str());
		}
		else
			((ScoreWindow*)v)->bmlfilename="";
		//delete chooser;
	}

	if(strcmp(((ScoreWindow*)v)->menubar->text(),"Save")==0)
	{
		Fl_File_Chooser *chooser;
		if(((ScoreWindow*)v)->bmlfilename==inimanager.Program_Path+"bml/empty.xml")
		{
			chooser = new Fl_File_Chooser((inimanager.Program_Path+"bml").c_str(),"XML files (*.xml)",0|2,"select file");
			chooser->show();
			while (chooser->shown())
				Fl::wait();
			if(chooser->value()!=0)
				((ScoreWindow*)v)->bmlfilename=chooser->value();
			else
				((ScoreWindow*)v)->bmlfilename="";
			//delete chooser;
		}
		if(((ScoreWindow*)v)->bmlfilename!="")
		{
			pos = ((ScoreWindow*)v)->bmlfilename.find_last_of("/");
			((ScoreWindow*)v)->Save();
			s="BML Editor 2.0 - " + ((ScoreWindow*)v)->bmlfilename.substr(pos+1, ((ScoreWindow*)v)->bmlfilename.length());
			((ScoreWindow*)v)->label(s.c_str());
		}
	}

	if(strcmp(((ScoreWindow*)v)->menubar->text(),"Save As...")==0)
	{
	Fl_File_Chooser *chooser;
		chooser = new Fl_File_Chooser((inimanager.Program_Path+"bml").c_str(),"XML files (*.xml)",0|2,"select file");
		chooser->show();
		while (chooser->shown())
			Fl::wait();
		if(chooser->value()!=0)
			((ScoreWindow*)v)->bmlfilename=chooser->value();
		else
			((ScoreWindow*)v)->bmlfilename="";
		//delete chooser;
		if(((ScoreWindow*)v)->bmlfilename!="")
		{
			((ScoreWindow*)v)->Save();
			pos = ((ScoreWindow*)v)->bmlfilename.find_last_of("/");
			s="BML Editor 2.0 - " + ((ScoreWindow*)v)->bmlfilename.substr(pos+1, ((ScoreWindow*)v)->bmlfilename.length());
			((ScoreWindow*)v)->label(s.c_str());
		}
	}

	if(strcmp(((ScoreWindow*)v)->menubar->text(),"Quit")==0)
	{
		((ScoreWindow*)v)->hide();
		if(((ScoreWindow*)v)->glcontainer!=0)
			((ScoreWindow*)v)->glcontainer->hide();
		//delete ((ScoreWindow*)v);
	}

	if(strcmp(((ScoreWindow*)v)->menubar->text(),"Show Greta")==0)
	{
		if(((ScoreWindow*)v)->glcontainer==0)
		{
			((ScoreWindow*)v)->glcontainer=new GLContainer(700,10,200,400, ((ScoreWindow*)v)->loadlistener);
			((ScoreWindow*)v)->glcontainer->show();
		}
	}

	if(strcmp(((ScoreWindow*)v)->menubar->text(),"Whiteboard Connection")==0)
	{
		if(((ScoreWindow*)v)->bp==0)
		{
//			((ScoreWindow*)v)->bp=(BMLSender*)new BMLSender("Greta.BMLSender","127.0.0.1",10000);
//			if(((BMLSender*)((ScoreWindow*)v)->bp)->Register()==0)
//			{
//				delete ((ScoreWindow*)v)->bp;
//				((ScoreWindow*)v)->bp=0;
//			}
		}
	}

	if(strcmp(((ScoreWindow*)v)->menubar->text(),"Show Listener")==0)
	{
		if(((ScoreWindow*)v)->loadlistener==1)
		{
			//delete listener
			((ScoreWindow*)v)->loadlistener=0;
		}
		else
		{
			//create listener
			//((ScoreWindow*)v)->w(600);
			((ScoreWindow*)v)->LoadListener();
			((ScoreWindow*)v)->loadlistener=1;
		}
	}

	((ScoreWindow*)v)->redraw();
}

void ScoreWindowclosed(void* v)
{
	((ScoreWindow*)v)->hide();
	if (((ScoreWindow*)v)->glcontainer!=0)
		((ScoreWindow*)v)->glcontainer->hide();
}


void selectedSpeech(Fl_Widget*w, void* v)
{
	SpeechWindow *sw;
	sw= new SpeechWindow((ScoreWindow*)v);
	sw->set_modal();
	sw->show();
}


void selectedInsert(Fl_Widget*w, void* v)
{
	((ScoreWindow*)v)->channelsview->inserting=true;
}


void selectedPlay(Fl_Widget*w, void* v)
{	
	Fl_File_Chooser *chooser;
	if(((ScoreWindow*)v)->bmlfilename==inimanager.Program_Path+"bml/empty.xml")
	{
		chooser = new Fl_File_Chooser((inimanager.Program_Path+"bml").c_str(),"XML files (*.xml)",0|2,"select file");
		chooser->show();
		while (chooser->shown())
			Fl::wait();
		if(chooser->value()!=0)
			((ScoreWindow*)v)->bmlfilename=chooser->value();
		else
			((ScoreWindow*)v)->bmlfilename="";
		//delete chooser;
	}
	if(((ScoreWindow*)v)->bmlfilename!="")
	{
		int pos=-1;
		std::string s;
		pos = ((ScoreWindow*)v)->bmlfilename.find_last_of("/");
		((ScoreWindow*)v)->Save();
		s="BML Editor 2.0 - " + ((ScoreWindow*)v)->bmlfilename.substr(pos+1, ((ScoreWindow*)v)->bmlfilename.length());
		((ScoreWindow*)v)->label(s.c_str());
	}

	if(((ScoreWindow*)v)->glcontainer==0)
	{
		int msg = MessageBox(NULL, "Visualize the Feedback Windows!", "BML Editor", MB_OK|MB_ICONINFORMATION);
	}
	else
		if(((ScoreWindow*)v)->glcontainer->visible()==0)
		{
			((ScoreWindow*)v)->glcontainer=0;
			int msg = MessageBox(NULL, "Visualize the Feedback Windows!", "BML Editor", MB_OK|MB_ICONINFORMATION);
		}
		else
		{
			if(((ScoreWindow*)v)->GenerateAnimations()==1)
			{
				std::string s;
				s=filenames.Fap_File;
				s=s.substr(0,s.find_last_of("."));
				((PlayerFLTKGLWindow*)(((ScoreWindow*)v)->glcontainer->glwindow))->agent->AssignFile((char*)s.c_str());//(char*)filenames.Base_File.c_str());
				((PlayerFLTKGLWindow*)(((ScoreWindow*)v)->glcontainer->glwindow))->agent->SetCurrentFrame(0);
				((PlayerFLTKGLWindow*)(((ScoreWindow*)v)->glcontainer->glwindow))->agent->StartTalking();

				if(((ScoreWindow*)v)->loadlistener==1)
				{
					((PlayerFLTKGLWindow*)(((ScoreWindow*)v)->glcontainer->glwindow))->listener->AssignFile((char*)(((ScoreWindow*)v)->listenerfile.c_str()));//(char*)filenames.Base_File.c_str());
					((PlayerFLTKGLWindow*)(((ScoreWindow*)v)->glcontainer->glwindow))->listener->StartTalking();
				}
			}
		}
}

void selectedMaxtime(Fl_Widget*w, void* v)
{
	((ScoreWindow*)v)->SetMaxTimeValue();
}


ScoreWindow::ScoreWindow():Fl_Window(10,20,700,540,"BML Editor 2.0")
{
	this->size_range(375,250,1280,580,1,1,0);
	channelsview=new ChannelsImage(100,40,590,425,"",this);

	menubar = new Fl_Menu_Bar(0, 0, this->w(),20, "File");
	menubar->box(FL_NO_BOX);
	menubar->add("File/New", FL_ALT+'n',(Fl_Callback *)selectedmenubar, this, 0);
	menubar->add("File/Load", FL_ALT+'o',(Fl_Callback *)selectedmenubar, this, 0);
	menubar->add("File/Save", FL_ALT+'s',(Fl_Callback *)selectedmenubar, this, 0);
	menubar->add("File/Save As...", FL_ALT+'s',(Fl_Callback *)selectedmenubar, this, FL_MENU_DIVIDER);
	//menubar->add("File/Load Phoneme File", FL_ALT+'p',(Fl_Callback *)selectedmenubar, this, FL_MENU_DIVIDER);
    menubar->add("File/Quit", FL_ALT+'q',(Fl_Callback *)selectedmenubar, this, 0);
	menubar->add("Tools/Show Greta", FL_ALT+'g',(Fl_Callback *)selectedmenubar, this, 0);
	//menubar->add("Tools/Whiteboard Connection", FL_ALT+'c',(Fl_Callback *)selectedmenubar, this, 0);
	//menubar->add("View/Show Listener", FL_ALT+'l',(Fl_Callback *)selectedmenubar, this, 0);
    menubar->add("Help/Help", '?',0);
	loadlistener=0;

	viewstarttime=new TimeSlider(100,475,450,20,channelsview);
	viewstarttime->type(FL_HOR_NICE_SLIDER);
	viewstarttime->range(0,99);
	viewstarttime->value(0);
	viewstarttime->step(1);

	maxtimevalue=new Fl_Input(560,475,45,20,"");
	maxtimevalue->value("5.0");
	setmaxtime=new Fl_Button(605,475,20,20,"ok");
	setmaxtime->labelfont(FL_HELVETICA);
	setmaxtime->labelsize(9);
	maxtime=atof(maxtimevalue->value());
	setmaxtime->callback((Fl_Callback *)selectedMaxtime,this);
	channelsview->maxtime=maxtime;
	
	zoom=new Fl_Output(100,505,0,20,"Zoom:");
	zoom->align(FL_ALIGN_RIGHT);

	viewzoom=new ZoomSlider(150,505,80,20,channelsview);
	viewzoom->type(FL_HOR_NICE_SLIDER);
	viewzoom->range(1,5);
	viewzoom->step(0.01);
	viewzoom->value(1);

	channels.clear();
	insertbutton=new Fl_Button(10,40,80,40,"new block@->");
	insertbutton->labelfont(FL_HELVETICA);
	insertbutton->labelsize(9);
	insertbuttonimage=new Fl_JPEG_Image("new.jpg");
	insertbutton->image(insertbuttonimage);
	insertbutton->callback((Fl_Callback *)selectedInsert,this);
	insertbutton->align(FL_ALIGN_INSIDE|FL_ALIGN_CENTER);


	playerbutton=new Fl_Button(650,475,40,40);
	playerbutton->labelfont(FL_HELVETICA);
	playerbutton->labelsize(12);
	playerbuttonimage=new Fl_JPEG_Image("face.jpg");
	playerbutton->image(playerbuttonimage);
	playerbutton->callback((Fl_Callback *)selectedPlay,this);
	playerbutton->align(FL_ALIGN_INSIDE|FL_ALIGN_CENTER);

	/*
	playerpsydulebutton=new Fl_Button(600,510,40,40, "PSY");
	playerpsydulebutton->labelfont(FL_HELVETICA);
	playerpsydulebutton->labelsize(9);
	//playerpsydulebutton->image(playerbuttonimage);
	//playerpsydulebutton->callback((Fl_Callback *)selectedPlayPsydule,this);
	playerpsydulebutton->align(FL_ALIGN_INSIDE|FL_ALIGN_CENTER);
	*/

	speechbutton=new Fl_Button(10,475,80,40,"Set speech");
	speechbutton->labelfont(FL_HELVETICA);
	speechbutton->labelsize(12);
	speechbutton->callback((Fl_Callback *)selectedSpeech,this);
	speechbutton->align(FL_ALIGN_INSIDE|FL_ALIGN_CENTER);

	speech=new MMSystemSpace::Signal();
	speech->modality="speech";
	speech->type="application/xml";
	speech->voice="mary.female";
	speech->language="english";
	te=new TorsoSpace::TorsoEngine();

	player=0;

	glcontainer=0;

	bmlfilename=inimanager.Program_Path+"bml/empty.xml";

	this->callback((Fl_Callback *)ScoreWindowclosed,this);

	LoadSpeaker();

	
	tm=0;
	bp=0;

//	LoadListener();
	LoadLibrary();
	
}

ScoreWindow::~ScoreWindow()
{
	delete channelsview;
	delete viewstarttime;
	delete viewzoom;
	delete insertbutton;
	delete glcontainer;
	channels.clear();
}

int ScoreWindow::handle(int e)
{
	/*if((Fl::event_inside(playerpsydulebutton))&&(e==FL_PUSH))
	{
		if(bp==0)
		{
			int msg = MessageBox(NULL, "Connect to the Whitebord!", "BML Editor", MB_OK|MB_ICONINFORMATION);
		}
		else
		{
			WriteBML();
			//printf("BML %s\n", bml.c_str());
			printf("Message Posted...\n");
//			((BMLSender*)bp)->PostString(bml);
		}
		redraw();
	}*/
	
	return Fl_Window::handle(e);
}

void ScoreWindow::LoadLibrary()
{
	inimanager.ReadIniFile("greta.ini");
	std::vector<Action*>::iterator iterAct;
	Action *newAct;

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
				if(GestureVec.size()!=0)
				{
					bool flag1=false;
					for(iterAct=GestureVec.begin();iterAct!=GestureVec.end();iterAct++)
					{
						if((*iterAct)->classname==gestureclass)
						{ 
							flag1=true;
							(*iterAct)->InstanceVec.push_back(gestureinstance.c_str());
							break;
						}
					}

					if(flag1==false)
					{
						newAct=new Action;
						newAct->classname=gestureclass;
						newAct->InstanceVec.push_back(gestureinstance.c_str());
						GestureVec.push_back(newAct);
					}
				}
				else
				{
					newAct=new Action;
					newAct->classname=gestureclass;
					newAct->InstanceVec.push_back(gestureinstance.c_str());
					GestureVec.push_back(newAct);;
				}
			} 
		
		}		
		
		if(filenames.Next()==0)flag=true;
	}

	face_log=fopen((inimanager.Program_Path+"logs/face_log.txt").c_str(),"w");
	FaceExpressionDictionary facelibrary;
	
	facelibrary.Init("fd/facelibrary.xml");

	// DEBUG VERSION HEAP CORRUPTION CHECK 
  
//	int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
//	tmpFlag |= _CRTDBG_CHECK_ALWAYS_DF;
//	_CrtSetDbgFlag( tmpFlag );


	std::map<std::string, TorsoSpace::TorsoGesture>::iterator itertorso;
	for(itertorso=te->gestuary.begin();itertorso!=te->gestuary.end();itertorso++)
		TorsoVec.push_back((*itertorso).second.id);

	std::string s;
	
	newAct=new Action;
	newAct->classname="head";
	HeadVec.push_back(newAct);

	newAct=new Action;
	newAct->classname="gaze";
	GazeVec.push_back(newAct);
	
	std::list<FaceExpression>::iterator iter;
	for(iter=facelibrary.FExpressions.begin();iter!=facelibrary.FExpressions.end();iter++)
	{
		if((*iter).classname=="head")
		{
			HeadVec[0]->InstanceVec.push_back((*iter).instance.c_str());
		}
		else
		if((*iter).classname=="gaze")
		{
			GazeVec[0]->InstanceVec.push_back((*iter).instance.c_str());
		}
		else
		{
			if(FacialExpressionVec.size()!=0)
			{
				flag=false;
				for(iterAct=FacialExpressionVec.begin();iterAct!=FacialExpressionVec.end();iterAct++)
				{
					if((*iterAct)->classname==(*iter).classname)
					{ 
						flag=true;
						(*iterAct)->InstanceVec.push_back((*iter).instance.c_str());
						break;
					}
				}


				if(flag==false)
				{
					newAct=new Action;
					newAct->classname=(*iter).classname;
					newAct->InstanceVec.push_back((*iter).instance.c_str());
					FacialExpressionVec.push_back(newAct);
				}
			}
			else
			{
				newAct=new Action;
				newAct->classname=(*iter).classname;
				newAct->InstanceVec.push_back((*iter).instance.c_str());
				FacialExpressionVec.push_back(newAct);
			}
		}
	}
	fclose(face_log);
}


void ScoreWindow::draw()
{
	Fl_Window::draw();
	channelsview->resize(100,40,this->w()-110,425);
	channelsview->redraw();
	std::vector<Channel>::iterator iter;
	for(iter=channels.begin();iter!=channels.end();iter++)
		(*iter).draw(channelsview);
/*	if(glcontainer==0)
	{
		glcontainer=new GLContainer(700,10,200,400, loadlistener);
		glcontainer->show();
	}*/
}

Channel * ScoreWindow::GetChannel(std::string n)
{
	std::vector<Channel>::iterator iter;
	for(iter=channels.begin();iter!=channels.end();iter++)
		if(((*iter).name)==n)
			return &(*iter);
	return 0;
}

int ScoreWindow::GenerateAnimations()
{
	std::string s;
	char* t;
	BML_AgentEngine *engine;
	
	engine = new BML_AgentEngine();
	if(engine->Execute(bmlfilename,t,1)==0)
		return 0;
	//speakerfile=filenames.Base_File;
	//delete(engine);

	return 1;
}

void ScoreWindow::StartPlayer()
{
	inimanager.ReadIniFile("greta.ini");

	Fl::gl_visual(FL_RGB|FL_DOUBLE|FL_DEPTH);

	if(player!=0)
	{
		if(!player->shown())
		{
			delete player;
			player=0;
		}
		else
		{
			player->LoadFile("output");
		}
	}
	if(player==0)
	{
		player=new PlayerFLTKWindow();
		player->show();
		//player->SetQuiet(true);
		//player->LoadFile("output");
	}
	
}


void ScoreWindow::Save()
{
	int pos=-1;

	WriteBML();

	pos=bmlfilename.find_first_of(".", 0);
	if(pos==-1)
		bmlfilename = bmlfilename+".xml";

	std::ofstream outputfile(bmlfilename.c_str());
	outputfile << bml;
	outputfile.close();
}


int ScoreWindow::LoadSpeaker()
{
	XMLGenericParser *p;
	XMLGenericTree *t;
	std::list<XMLGenericTree*>::iterator iterch;
	std::list<XMLGenericTree*>::iterator itertm;
	Channel *channel;
	Block *block;
	MMSystemSpace::Signal *newsignal;
	int numtm=1;
	char app[20];

	std::string parname="";
	std::string txt="";

	p=new XMLGenericParser();
	p->SetValidating(false);
	t=p->ParseFile(bmlfilename);

	if(t!=0)
	{
		channel=new Channel(10,100,80,45,"speech1","speech", this);
		channel->SetColor(255,255,255);
		channels.push_back(*channel);

		channel=new Channel(10,170,80,45,"head1","head", this);
		channel->SetColor(128,0,128);
		channels.push_back(*channel);

		channel=new Channel(10,230,80,45,"gaze1","gaze", this);
		channel->SetColor(0,0,128);
		channels.push_back(*channel);

		channel=new Channel(10,290,80,45,"face1","face", this);
		channel->SetColor(128,0,0);
		channels.push_back(*channel);
		
		channel=new Channel(10,350,80,45,"gesture1","gesture", this);
		channel->SetColor(0,128,0);
		channels.push_back(*channel);

		channel=new Channel(10,410,80,45,"torso1","torso", this);
		channel->SetColor(0,128,128);
		channels.push_back(*channel);

		if(!t->child.empty())
			for(iterch=t->child.begin();iterch!=t->child.end();iterch++)
			{
				if((*iterch)->name=="speech")
				{
					speech->StoreBML(*iterch, inimanager);
					if(speech->TemporizeTimeMarkers()==false)
					{
						int msg = MessageBox(NULL, "Time markers cannot be defined since the Mary server is off!", "BML Editor", MB_OK|MB_ICONINFORMATION);
					}
				
					for(itertm=(*iterch)->child.begin();itertm!=(*iterch)->child.end();itertm++)
					{
						if((*itertm)->name=="tm")
						{
							TestoVec.push_back("$");
							numtm+=1;
						}
						if((*itertm)->name=="text")
						{
							txt=CleanString((*itertm)->value);
							TestoVec.push_back(txt);
						}
					}

					LoadPhonemes();
				}

				std::vector<Channel>::iterator iterchannels;
				for(iterchannels=channels.begin();iterchannels!=channels.end();iterchannels++)
				{

					if((*iterchannels).name=="face1" || (*iterchannels).name=="head1" || (*iterchannels).name=="gaze1" || (*iterchannels).name=="gesture1" || (*iterchannels).name=="torso1")
					{
						if((*iterchannels).type==(*iterch)->name) //trovare il canale giusto
						{
							std::string id=(*iterch)->GetAttribute("id");
							block=(*iterchannels).GetTheBlockName(id);
							if(block==0)
							{
								newsignal=new MMSystemSpace::Signal();
								newsignal->StoreBML(*iterch, inimanager);

								(*iterchannels).AddBlock((*iterchannels).name,newsignal,
									channelsview->position_x,channelsview->position_y);


								if((*iterchannels).name!="torso1")
								{
									(*iterchannels).GetLastBlock()->SetClassInstance(
										newsignal->reference.substr(0,newsignal->reference.find_first_of("=")),
										newsignal->reference.substr(newsignal->reference.find_first_of("=")+1));
								}
								else
									(*iterchannels).GetLastBlock()->SetClassInstance("torso",newsignal->reference);
								
								SignalsVec.push_back(newsignal);
							}
						}
					}
				}
			}
	}
	else
	{
		this->redraw();
		return 0;
	}

	int ln;

	std::vector<MMSystemSpace::Signal*>::iterator itersig;
	for(itersig=SignalsVec.begin();itersig!=SignalsVec.end();itersig++)
	{
		ln=0;
		(*itersig)->Temporize(SignalsVec, speech, &ln);
		if((*itersig)->concretized==false)
		{
			/*
			std::vector<Channel>::iterator iterchannels;
			for(iterchannels=channels.begin();iterchannels!=channels.end();iterchannels++)
			{
				block=(*iterchannels).GetTheBlockName((*itersig)->id);
				if(block!=NULL)
					printf("Devo cancellare il blocco %s\n", block->signal->id.c_str());
				(*iterchannels).DeleteBlock((*itersig)->id);
				//itersig-=1;
				//break;
			}
			*/
			printf("Mary is not running...\n");
		}
		else
			if(maxtime<(*itersig)->start+(*itersig)->duration)
				maxtime=(*itersig)->start+(*itersig)->duration;
	}

	if(maxtime<speech->start+speech->duration)
			maxtime=speech->start+speech->duration;

	maxtime=ceil(maxtime);
	this->maxtimevalue->value(itoa(maxtime, app, 10));
	this->SetMaxTimeValue();
	delete p;
	if(t!=0)
		delete t;
	this->redraw();
	return 1;
}

int ScoreWindow::LoadListener()
{
	int pos=-1;
	XMLGenericParser *p;
	XMLGenericTree *t;
	std::list<XMLGenericTree*>::iterator iterch;
	std::list<XMLGenericTree*>::iterator iterbl;
	Channel *channel;
	Block *block;
	MMSystemSpace::Signal *newsignal;

	/*
	listener=new Fl_Output(10,440,0,20,"Listener");
	listener->align(FL_ALIGN_RIGHT);
	listener->labeltype(FL_EMBOSSED_LABEL);
	*/

	if(bmlfilename=="") 
		scorelistenerfilename="";
	else
	{
		pos=bmlfilename.find_first_of(".", 0);
		scorelistenerfilename=bmlfilename.substr(0,pos);
		scorelistenerfilename+="_listener.xml";
	}

	p=new XMLGenericParser();
	p->SetValidating(false);
	t=p->ParseFile(scorelistenerfilename);

	if(t!=0)
	{
		channel=new Channel(10,470,80,55,"head2","head", this);
		channel->SetColor(128,0,128);
		channels.push_back(*channel);

		channel=new Channel(10,540,80,55,"gaze2","gaze", this);
		channel->SetColor(0,0,128);
		channels.push_back(*channel);

		channel=new Channel(10,610,80,55,"face2","face", this);
		channel->SetColor(128,0,0);
		channels.push_back(*channel);
		
		channel=new Channel(10,680,80,55,"gesture2","gesture", this);
		channel->SetColor(0,128,0);
		channels.push_back(*channel);
		if(!t->child.empty())
			for(iterch=t->child.begin();iterch!=t->child.end();iterch++)
			{

				if(!(*iterch)->HasAttribute("stroke"))
				{
					std::string type=(*iterch)->GetAttribute("type");

					std::vector<Channel>::iterator iterchannels;
					for(iterchannels=channels.begin();iterchannels!=channels.end();iterchannels++)
					{
						if((*iterchannels).name=="face2" || (*iterchannels).name=="head2" || (*iterchannels).name=="gaze2" || (*iterchannels).name=="gesture2" || (*iterchannels).name=="torso2")
						{
							if((*iterchannels).type==(*iterch)->name)
							{
								std::string id=(*iterch)->GetAttribute("id");
								block=(*iterchannels).GetTheBlockName(id);
								if(block==0)
								{
									newsignal=new MMSystemSpace::Signal();
									newsignal->StoreBML(*iterch, inimanager);
	
									(*iterchannels).AddBlock((*iterchannels).name,newsignal,
										channelsview->position_x,channelsview->position_y);

									(*iterchannels).GetLastBlock()->SetClassInstance(
										newsignal->reference.substr(0,newsignal->reference.find_first_of("=")),
										newsignal->reference.substr(newsignal->reference.find_first_of("=")+1));
									
									SignalsVec.push_back(newsignal);
								}
							}
						}
					}
				}
				else
				{
					//TODO: register strokes
					/*
					std::vector<singlesignal>::iterator itergestures;
					for(itergestures=gesturesignals.begin();itergestures!=gesturesignals.end();itergestures++)
					{
						if((*itergestures).id==((*iterch)->GetAttribute("id")))
						{
							//TODO: add strokes
						}
					}
					*/
				}
			}
	}
	else
	{
		this->redraw();
		return 0;
	}

	delete p;
	delete t;
	this->redraw();
	return 1;
}

void ScoreWindow::DeleteOneBlock(std::string namechannel, std::string nameblock)
{
	std::vector<Channel>::iterator iter;
	for(iter=channels.begin();iter!=channels.end();iter++)
	{
		if((*iter).name==namechannel)
		{
			(*iter).DeleteBlock(nameblock);
			break;
		}
	}
	this->redraw();
}

void ScoreWindow::DeleteAll()
{
	std::vector<Channel>::iterator iter;
	for(iter=channels.begin();iter!=channels.end();iter++)
	{
		(*iter).DeleteAll();
	}
	channels.clear();
}

int ScoreWindow::WriteBML()
{	
	std::vector<Channel>::iterator iter;
	int count=0;
	bool flag;
	int uid=0;

	Block *b;

	bml="";
	bml+="<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
	bml+="<!DOCTYPE bml SYSTEM \"bml.dtd\" []>\n";

	/*
	bml+="<bml xmlns=\"http://www.mindmakers.org/projects/BML\"\n";
	bml+="xmlns:ssml=\"http://www.w3.org/2001/10/synthesis\"\n";
	bml+="xmlns:gretabml=\"http://www.iut.univ-paris8.fr/greta\">\n";
	*/

	bml+="<bml>\n";

	if(speech->text!="")
		bml+=speech->BMLcode; //GetBML(0);

	//printf("%s\n", speech->BMLcode.c_str());

	for(iter=channels.begin();iter!=channels.end();iter++)
	{
		if((*iter).name!="speech1" && (*iter).name!="head2" && (*iter).name!="gaze2" && (*iter).name!="face2" && (*iter).name!="gesture2"  && (*iter).name!="torso2")
		{
			flag=true;
			count=0;

			while(flag)
			{
				b=(*iter).GetBlockN(count);
				if(b!=0)
				{
					uid+=1;
					bml+=b->signal->GetBML(0);
				}
				else
					flag=false;
				count++;
			}
		}
	}

	bml+= "</bml>\n";

	return 1;
}

int ScoreWindow::DeleteSignal(MMSystemSpace::Signal *sig)
{
	std::vector<MMSystemSpace::Signal*>::iterator itersignal;
	
	for(itersignal=SignalsVec.begin();itersignal!=SignalsVec.end();itersignal++)
	{
		if((*itersignal)->id==sig->id)
		{
			SignalsVec.erase(itersignal);
			break;
		}
	}

	return 1;
}

std::string ScoreWindow::CleanString(std::string str)
{
	int i, j=0;
	std::string str1="";

	i=0;
	while(str[i]!='\0')
	{
		if(str[i]!='\n' && str[i]!='\t')
		{
			str1=str1+str[i];
		}
		i+=1;
	}

	return(str1);
}

void ScoreWindow::SetMaxTimeValue()
{
	this->maxtime=atof(this->maxtimevalue->value());
	this->channelsview->maxtime=this->maxtime;
	this->channelsview->redraw();
}

int ScoreWindow::LoadPhonemes()
{
	int pos;
	float phostart=0.0;
	std::string line, filename;
	std::vector<Channel>::iterator iterchannels;
	filename=inimanager.Program_Path+speech->reference;
	std::ifstream inputfile(filename.c_str());
	MMSystemSpace::Signal *newsignal;

	if(inputfile.is_open())
	{
		for(iterchannels=channels.begin();iterchannels!=channels.end();iterchannels++)
		if((*iterchannels).name=="speech1") break;
		(*iterchannels).DeleteAll();

		while((inputfile.rdstate()&std::ios::eofbit)==0)
		{
			std::getline(inputfile,line,'\n');
			if(line!="")
			{ 
				newsignal=new MMSystemSpace::Signal();
				newsignal->modality="phoneme";
				pos=line.find_first_of(" ");
				newsignal->id=line.substr(0, pos);
				newsignal->start=phostart;
				newsignal->duration=atof(line.substr(pos+1).c_str());
				phostart+=newsignal->duration;
				(*iterchannels).AddBlock((*iterchannels).name,newsignal,
					channelsview->position_x,channelsview->position_y);
				PhonemesVec.push_back(*newsignal);
			}
		}
		inputfile.close();
		return(1);
	}
	else
		return 0;
}
