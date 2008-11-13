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

// EmotionMixer.cpp: implementation of the EmotionMixer class.
//
//////////////////////////////////////////////////////////////////////

#include "EmotionMixer.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <list>
#include <string>
#include <fl/Fl_File_Chooser.h>
#include <FL/Fl.H>
#include "IniManager.h"
#include "EmotionMixer.h"
#include "APML_AgentEngine.h"

extern IniManager inimanager;

extern FILE *face_log;

void selectedplay(Fl_Widget* w, void* v)
{
	((EmotionMixer*)v)->glutw->agent->StartTalking();
}

void selectedstop(Fl_Widget* w, void* v)
{
	((EmotionMixer*)v)->glutw->agent->StopTalking();
}

void selectedframeback(Fl_Widget* w, void* v)
{
	((EmotionMixer*)v)->glutw->agent->StopTalking();
	((EmotionMixer*)v)->glutw->agent->OneFrameDown();
}

void selectedframenext(Fl_Widget* w, void* v)
{
	((EmotionMixer*)v)->glutw->agent->StopTalking();
	((EmotionMixer*)v)->glutw->agent->OneFrameUp();
}

void selectedrewind(Fl_Widget* w, void* v)
{
	((EmotionMixer*)v)->glutw->agent->StopTalking();
	((EmotionMixer*)v)->glutw->agent->SetCurrentFrame(0);
}

void LWDX_CB(Fl_Widget* w, void* v)
{
	if(((EmotionMixer*)v)->liphWidthDx->value()==1)
		((EmotionMixer*)v)->liphWidthSx->value(0);
}

void LWSX_CB(Fl_Widget* w, void* v)
{
	if(((EmotionMixer*)v)->liphWidthSx->value()==1)
		((EmotionMixer*)v)->liphWidthDx->value(0);
}

void CORNERDX_CB(Fl_Widget* w, void* v)
{
	if(((EmotionMixer*)v)->cornerDx->value()==1)
		((EmotionMixer*)v)->cornerSx->value(0);
}

void CORNERSX_CB(Fl_Widget* w, void* v)
{
	if(((EmotionMixer*)v)->cornerSx->value()==1)
		((EmotionMixer*)v)->cornerDx->value(0);
}

void PARLAB_CB(Fl_Widget* w, void* v)
{	
	for (int i=0; i<7; i++)
		((EmotionMixer*)v)->basicEmotions[i]->value(((EmotionMixer*)v)->MatrixEmotion[((EmotionMixer*)v)->labialParameters->value()][i]);
}

void ALLPAR_CB(Fl_Widget* w, void* v)
{
	if(((EmotionMixer*)v)->allParameters->value()==1)
	{
		((EmotionMixer*)v)->labialParameters->deactivate();
		for (int j=0; j<7; j++)
			for (int i=0; i<7; i++)
				((EmotionMixer*)v)->MatrixEmotion[j][i]=((EmotionMixer*)v)->basicEmotions[i]->value();
		((EmotionMixer*)v)->modified=1;
	}
	else
		((EmotionMixer*)v)->labialParameters->activate();
}

void BASIC_EMOTIONS_CB(Fl_Widget* w, void* v)
{
	if(((EmotionMixer*)v)->allParameters->value()==0)
	{
		if (((EmotionMixer*)v)->IndexEmo!=-1)
		{
			((EmotionMixer*)v)->MatrixEmotion[((EmotionMixer*)v)->labialParameters->value()][((EmotionMixer*)v)->IndexEmo]=((EmotionMixer*)v)->basicEmotions[((EmotionMixer*)v)->IndexEmo]->value();
			((EmotionMixer*)v)->modified=1;
		}
	}
	else
	{
		if (((EmotionMixer*)v)->IndexEmo!=-1)
		{
			for (int i=0; i<7; i++)
				((EmotionMixer*)v)->MatrixEmotion[i][((EmotionMixer*)v)->IndexEmo]=((EmotionMixer*)v)->basicEmotions[((EmotionMixer*)v)->IndexEmo]->value();
			((EmotionMixer*)v)->modified=1;
		}
	}
}

void EXAMPLE_CB(Fl_Widget* w, void* v)
{
	((EmotionMixer*)v)->PlayExample();
}

void MENU_NEW_CB(Fl_Widget* w, void* v)
{
	//yes=6	no=7 cancel=2
	char val[5];
	int i,j;
	
	if (((EmotionMixer*)v)->modified==1) {
		((EmotionMixer*)v)->msg = MessageBox(NULL, "Do you want to save the emotion?", "Emotion Mixer", MB_YESNOCANCEL|MB_ICONEXCLAMATION);
		if (((EmotionMixer*)v)->msg==6)
		{
			if (((EmotionMixer*)v)->AddEmo(0))
			{

			}
		}
		else
			if (((EmotionMixer*)v)->msg==7)
			{
				((EmotionMixer*)v)->CleanAll();
				((EmotionMixer*)v)->modified=0;
				((EmotionMixer*)v)->newEmo="";
				for(i=0; i<7; i++)
					for(j=0; j<7; j++)
					{
						if(j==0)
							((EmotionMixer*)v)->MatrixEmotion[i][j]=1.00;
						else
							((EmotionMixer*)v)->MatrixEmotion[i][j]=0.00;
					}
			}
	}
	else
		{
			((EmotionMixer*)v)->CleanAll();
			((EmotionMixer*)v)->modified=0;
			((EmotionMixer*)v)->newEmo="";
			for(i=0; i<7; i++)
				for(j=0; j<7; j++)
				{
					if(j==0)
						((EmotionMixer*)v)->MatrixEmotion[i][j]=1.00;
					else
						((EmotionMixer*)v)->MatrixEmotion[i][j]=0.00;
				}
		}
}

void MENU_SAVE_CB(Fl_Widget* w, void* v)
{
	if (((EmotionMixer*)v)->modified==1)
		((EmotionMixer*)v)->AddEmo(0);
}

void CLOSE_ALL_CB(Fl_Widget* w, void* v)
{
	if (((EmotionMixer*)v)->modified==1)
	{
		((EmotionMixer*)v)->msg = MessageBox(NULL, "Do you want to save the emotion before quit?", "Emotion Mixer", MB_YESNOCANCEL|MB_ICONEXCLAMATION);
		if (((EmotionMixer*)v)->msg==6)
		{
			if (((EmotionMixer*)v)->AddEmo(0))
			{
				((EmotionMixer*)v)->hide();
			}
		}
		else
			if (((EmotionMixer*)v)->msg==7)
				((EmotionMixer*)v)->hide();
	}
	else
		((EmotionMixer*)v)->hide();
}

void MENU_OPEN_CB(Fl_Widget* w, void* v)
{	
	if (((EmotionMixer*)v)->modified==1)
	{
		((EmotionMixer*)v)->msg = MessageBox(NULL, "Do you want to save the emotion?", "Emotion Mixer", MB_YESNOCANCEL|MB_ICONEXCLAMATION);
		if (((EmotionMixer*)v)->msg==6)
		{
			if (((EmotionMixer*)v)->AddEmo(0))
			{
				((EmotionMixer*)v)->ow = new OpenWin(&(((EmotionMixer*)v)->emotionData));
				((EmotionMixer*)v)->ow->set_modal();
				((EmotionMixer*)v)->ow->show();
				while (((EmotionMixer*)v)->ow->shown())
						Fl::wait();
				((EmotionMixer*)v)->nameEmotion->value(((EmotionMixer*)v)->ow->nameEmotion->value());

				((EmotionMixer*)v)->LoadEmo();
			}
		}
		else 
			if (((EmotionMixer*)v)->msg==7)
			{
				((EmotionMixer*)v)->ow = new OpenWin(&(((EmotionMixer*)v)->emotionData));
				((EmotionMixer*)v)->ow->set_modal();
				((EmotionMixer*)v)->ow->show();
				while (((EmotionMixer*)v)->ow->shown())
						Fl::wait();
				((EmotionMixer*)v)->nameEmotion->value(((EmotionMixer*)v)->ow->nameEmotion->value());

				((EmotionMixer*)v)->LoadEmo();
			}
	}
	else
		{
			((EmotionMixer*)v)->ow = new OpenWin(&(((EmotionMixer*)v)->emotionData));
			((EmotionMixer*)v)->ow->set_modal();
			((EmotionMixer*)v)->ow->show();
			while (((EmotionMixer*)v)->ow->shown())
					Fl::wait();
			((EmotionMixer*)v)->nameEmotion->value(((EmotionMixer*)v)->ow->nameEmotion->value());

			((EmotionMixer*)v)->LoadEmo();
		}
}

void MENU_QUIT_CB(Fl_Widget* w, void* v)
{
	if (((EmotionMixer*)v)->modified==1)
		((EmotionMixer*)v)->AddEmo(0);
	((EmotionMixer*)v)->WriteEmoFile();
	((EmotionMixer*)v)->hide();
}

EmotionMixer::EmotionMixer():Fl_Window(10,20,1024,700,"agent score 0.1")
{
	int i,j;
	char v[10];

	inimanager.ReadIniFile("greta.ini");

	this->size_range(400,350,1280,1024,1,1,0);
	this->callback((Fl_Callback *)CLOSE_ALL_CB,this);

	face_log=fopen("logs/face_log_em.txt","w");

	for(i=0;i<7;i++) 
		for(j=0;j<7;j++)
		{
			if(j==0)
				MatrixEmotion[i][j]=1.00;
			else
				MatrixEmotion[i][j]=0.00;
		}
 
	modified=0;
	allPar=0;

	menubar = new Fl_Menu_Bar(0, 0, this->w(),20, "File");
	menubar->box(FL_NO_BOX);
	menubar->add("File/New", FL_ALT+'n',(Fl_Callback *)MENU_NEW_CB, this, 0);
	menubar->add("File/Open", FL_ALT+'o',(Fl_Callback *)MENU_OPEN_CB, this, 0);
	menubar->add("File/Save", FL_ALT+'s',(Fl_Callback *)MENU_SAVE_CB, this, FL_MENU_DIVIDER);
    menubar->add("File/Quit", FL_ALT+'q',(Fl_Callback *)MENU_QUIT_CB, this, 0);
    menubar->add("Help/Help", '?',0);

	this->ReadEmoFile();
	newEmo="";
	nameEmotion=new Fl_Input(25,80,160,20,"Name Emotion:");
	nameEmotion->align(FL_ALIGN_TOP);

	boxEmotions = new Fl_Box(FL_BORDER_BOX, 12, 140, 185, 450,"Basic Emotions");
	boxEmotions->align(FL_ALIGN_TOP);

	labialParameters=new Fl_Choice(25,170,160,20,"Labial Parameter");
	labialParameters->align(FL_ALIGN_TOP);
	labialParameters->callback((Fl_Callback *)PARLAB_CB,this);
	labialParameters->add("Upper Lip Opening");
	labialParameters->add("Lower Lip Opening");
	labialParameters->add("Jaw");
	labialParameters->add("Lip Width");
	labialParameters->add("Upper Lip Protrusion");
	labialParameters->add("Lower Lip Protrusion");
	labialParameters->add("Corners");
	labialParameters->value(0);
	allParameters=new Fl_Check_Button(25, 200, 20, 20, "All Parameters");
	allParameters->align(FL_ALIGN_RIGHT);
	allParameters->callback((Fl_Callback *)ALLPAR_CB,this);
	allParameters->value(0);

	EmotionName[0]="Neutral";
	EmotionName[1]="Joy";
	EmotionName[2]="Surprise";
	EmotionName[3]="Fear";
	EmotionName[4]="Anger";
	EmotionName[5]="Disgust";
	EmotionName[6]="Sadness";

	for (i=0; i<7; i++)
	{
		basicEmotions[i]=new Fl_Value_Slider(25,250+i*50,160,20,EmotionName[i]);
		basicEmotions[i]->align(FL_ALIGN_TOP);
		basicEmotions[i]->type(FL_HOR_NICE_SLIDER);
		basicEmotions[i]->bounds(0,1);
		basicEmotions[i]->callback((Fl_Callback *)BASIC_EMOTIONS_CB,this);
		basicEmotions[i]->step(0.1);
	}

	boxQualifiers = new Fl_Box(FL_BORDER_BOX, 207, 140, 175, 100,"Expressivity Qualifiers");
	boxQualifiers->align(FL_ALIGN_TOP);

	tension=new Fl_Value_Slider(220,180,150,20,"Tension");
	tension->align(FL_ALIGN_TOP);
	tension->type(FL_HOR_NICE_SLIDER);
	tension->bounds(-0,3);
	tension->step(0.1);

	boxAsimmetry = new Fl_Box(FL_BORDER_BOX, 207, 290, 175, 190,"Asymmetry");
	boxAsimmetry->align(FL_ALIGN_TOP);

	liphWidthDx=new Fl_Check_Button(220, 310, 20, 20, "Right Up");
	liphWidthDx->align(FL_ALIGN_RIGHT);
	liphWidthDx->callback((Fl_Callback *)LWDX_CB,this);
	liphWidthDx->value(0);
	liphWidthSx=new Fl_Check_Button(220, 340, 20, 20, "Right Down");
	liphWidthSx->align(FL_ALIGN_RIGHT);
	liphWidthSx->callback((Fl_Callback *)LWSX_CB,this);
	liphWidthSx->value(0);
	cornerDx=new Fl_Check_Button(220, 410, 20, 20, "Left Up");
	cornerDx->align(FL_ALIGN_RIGHT);
	cornerDx->callback((Fl_Callback *)CORNERDX_CB,this);
	cornerDx->value(0);
	cornerSx=new Fl_Check_Button(220, 440, 20, 20, "Left Down");
	cornerSx->align(FL_ALIGN_RIGHT);
	cornerSx->callback((Fl_Callback *)CORNERSX_CB,this);
	cornerSx->value(0);

	this->CleanAll();

	inimanager.SetValueString("CHARACTER_SPEAKER","greta");
	inimanager.SetValueInt("PLAYER_ONLYFACE",1);
	inimanager.SetValueInt("PLAYER_SHOWLISTENER",0);

	glutw=new PlayerFLTKGLWindow(420,80,100,100,"test");

	example=new Fl_Button(70,620,80,30,"Example >>");
	example->callback((Fl_Callback *)EXAMPLE_CB, this);
	   
	button_rewind=new Fl_Button(620,618,30,30,"@|<");
	button_rewind->callback((Fl_Callback *)selectedrewind,this);
	button_frameback=new Fl_Button(655,618,30,30,"@<<");
	button_frameback->callback((Fl_Callback *)selectedframeback,this);
	button_stop=new Fl_Button(690,618,30,30,"@square");
	button_stop->callback((Fl_Callback *)selectedstop,this);
	button_play=new Fl_Button(725,618,30,30,"@>");
	button_play->callback((Fl_Callback *)selectedplay,this);
	button_framenext=new Fl_Button(760,618,30,30,"@>>");
	button_framenext->callback((Fl_Callback *)selectedframenext,this);

    this->end();
    this->show();
//    return(Fl::run());

}

EmotionMixer::~EmotionMixer()
{
}

void EmotionMixer::CleanAll(void)
{
	basicEmotions[0]->value(1);
	for(int i=1; i<7; i++)
		basicEmotions[i]->value(0);

	tension->value(0);
}


int EmotionMixer::handle(int event)
{
	if(Fl::event_button()==FL_LEFT_MOUSE)
	{
			
		if(event==FL_PUSH)
		{
			for (int i=0; i<7; i++)
			{
				if(Fl::event_inside(basicEmotions[i]))
				{
					IndexEmo=i;
					break;
				}
				else
					IndexEmo=-1;
			}
			if(Fl::event_inside(labialParameters))
			{
				for (int i=0; i<7; i++)
					MatrixEmotion[labialParameters->value()][i]=basicEmotions[i]->value();
			}
		}
	}
	return Fl_Window::handle(event);
}

void EmotionMixer::draw()
{
	glutw->size(this->w()-450,this->h()-200);
	glutw->invalidate();
	Fl_Window::draw();
}

void EmotionMixer::PlayExample(void)
{
	glutw->agent->StopTalking();
	if (this->WriteTestFile())
	{	
		APML_AgentEngine *fe;
		fe=new APML_AgentEngine();
		fe->CalculateTurn("input/Prova.xml");
		glutw->agent->AssignFile("output/Prova");
		glutw->agent->StartTalking();
	}
}


int EmotionMixer::ReadEmoFile(void)
{
	string line;
	int j, i;
	EmoData *p;

	ifstream inputfile("coart/emotion.txt");
	if(inputfile.is_open())
	{
		while((inputfile.rdstate()&ios::eofbit)==0)
		{
			getline(inputfile,line,'\n');
			if(line!="")
			{	
				p=new EmoData();
				p->name=line.c_str();
				getline(inputfile,line,'\n');
				p->tension=atof(line.c_str());
				getline(inputfile,line,'\n');
				p->artic=atof(line.c_str());
				getline(inputfile,line,'\n');
				p->cornerAs=atof(line.c_str());
				getline(inputfile,line,'\n');
				p->lipWidthAs=atof(line.c_str());
				for(i=0; i<7; i++)
				{
					getline(inputfile,line,'\n');
					for(j=0; j<7; j++) 
					{
						p->value[i][j] = atof(line.substr(0,line.find_first_of(" ")).c_str());
						line = line.substr(line.find_first_of(" ")+1,line.length()-line.find_first_of(" ")-1);
					}
				}
				emotionData.push_back(*p);
			}
		}
		//DEBUG
		/*
		vector<EmoData>::iterator iter;
		for(iter=emotionData.begin();iter!=emotionData.end();iter++)
		{
			printf("%s\n", (iter->name).c_str());
			printf("%f\n", iter->tension);
			printf("%f\n", iter->artic);
			for(i=0; i<7; i++){
				for(j=0; j<7; j++)
					printf("%f ", iter->value[i][j]);
				printf("\n");
			}
		}
		*/
	}
	else
	{
		printf("File emotion.txt not found!\n");
		return 0;
	}
	inputfile.close();
	return 1;
}

void EmotionMixer::LoadEmo()
{
	int flag=0;
	char val[10];
	vector<EmoData>::iterator iter;
	
	for(iter=emotionData.begin();iter!=emotionData.end();iter++)
	{
		if (!strcmp((iter->name).c_str(), nameEmotion->value()))
		{
			flag=1;
			break;
		}
	}

	if (flag==1)
	{
		tension->value(iter->tension);
		if(iter->cornerAs==1)
		{
			cornerDx->value(1);
			cornerSx->value(0);
		}
		else
			if(iter->cornerAs==2)
			{
				cornerDx->value(0);
				cornerSx->value(1);
			}
			else
				{
					cornerDx->value(0);
					cornerSx->value(0);
				}

		if(iter->lipWidthAs==1)
		{
			liphWidthDx->value(1);
			liphWidthSx->value(0);
		}
		else
			if(iter->lipWidthAs==2)
			{
				liphWidthDx->value(0);
				liphWidthSx->value(1);
			}
			else
				{
					liphWidthDx->value(0);
					liphWidthSx->value(0);
				}

		for(int i=0; i<7; i++)
		{
			for(int j=0; j<7; j++)
			{
				MatrixEmotion[i][j]=iter->value[j][i];
				if (i==0)
				{
					for(int k=0; k<7; k++)
							basicEmotions[k]->value(MatrixEmotion[i][k]);
				}
				labialParameters->value(0);
			}
		}
	}
}

int EmotionMixer::AddEmo(int inI)
{
	int i=0;
	int flag, j;
	float line;
	char msg[256];
	char emo[256];
	char *p;
	EmoData *n;

	sprintf(msg,"%s",nameEmotion->value());
	printf("nameEmotion: %s		newEmo: %s\n", msg, newEmo.c_str());
	
	if (strcmp(msg,"")==0)
		MessageBox(NULL, "Choose a name!", "Emotion Mixer", MB_OK | MB_ICONINFORMATION);
	else
	{
		for(p=msg; p<msg+strlen(msg); p++)
		{
			if( isupper( *p ) )
				emo[i] = _tolower(*p);
			else 
				emo[i] = *p ;
			i++;
		}
		emo[strlen(msg)]='\0';

		if (!strcmp("neutral", emo) || !strcmp("anger", emo) || 
			!strcmp("sadness", emo) || !strcmp("surprise", emo) || 
			!strcmp("joy", emo) || !strcmp("disgust", emo) || 
			!strcmp("fear", emo))
		{
			if (inI==0)
			{
				MessageBox(NULL, "Name not valid!", "Emotion Mixer", MB_OK | MB_ICONSTOP);
				nameEmotion->value("");
				return(0);
			}
			else
			{
				for(i=0; i<7; i++)
				{
					line=0;
					for(j=0; j<7; j++)
						line+=MatrixEmotion[i][j];
					if (line==0)
					{
						MessageBox(NULL, "The emotion is not well defined!", "Emotion Mixer", MB_OK | MB_ICONEXCLAMATION);
						return(0);
					}	
				}
				return(1);
			}
		}
		else
		{
			for(i=0; i<7; i++)
			{
				line=0;
				for(j=0; j<7; j++)
					line+=MatrixEmotion[i][j];
				if (line==0)
				{
					MessageBox(NULL, "The emotion is not well defined!", "Emotion Mixer", MB_OK | MB_ICONEXCLAMATION);
					return(0);
				}	
			}

			flag=0;
			vector<EmoData>::iterator iter;
			for(iter=emotionData.begin();iter!=emotionData.end();iter++)
			{
				if(iter->name==msg)
				{
					flag=1;
					break;
				}
			}
			
			if(flag==1 && msg!=newEmo)
			{
				this->msg = MessageBox(NULL, "L'emozione esiste già. Sicuro di volerla sovrascrivere?", "Emotion Mixer", MB_YESNOCANCEL | MB_ICONQUESTION);
				if (this->msg==6)
				{
					printf("entro qui\n");
					iter->tension = tension->value();
					iter->artic = 0;
					if(liphWidthDx->value()==1)
							iter->lipWidthAs=1;
						else
							if (liphWidthSx->value()==1)
								iter->lipWidthAs=2;
							else
								iter->lipWidthAs=0;

					if(cornerDx->value()==1)
							iter->cornerAs=1;
						else
							if (cornerSx->value()==1)
								iter->cornerAs=2;
							else
								iter->cornerAs=0;
					for(i=0; i<7; i++)
						for(j=0; j<7; j++)
							iter->value[i][j]=MatrixEmotion[j][i];
					newEmo=msg;
					modified=0;
					printf("esco da qui\n");
					return 1;
				}
			}
			else 
				if(flag==0  || (flag==1 && msg==newEmo))
				{
					if(flag==0)
					{
						n=new EmoData();
						n->name = msg;
						n->tension = tension->value();
						n->artic = 0;
						if(liphWidthDx->value()==1)
							n->lipWidthAs=1;
						else
							if (liphWidthSx->value()==1)
								n->lipWidthAs=2;
							else
								n->lipWidthAs=0;

						if(this->cornerDx->value()==1)
							n->cornerAs=1;
						else
							if (cornerSx->value()==1)
								n->cornerAs=2;
							else
								n->cornerAs=0;

						for(i=0; i<7; i++)
							for(j=0; j<7; j++)
								n->value[i][j]=MatrixEmotion[j][i];
						emotionData.push_back(*n);
					}
					else 
					{
						iter->tension = tension->value();
						iter->artic = 0;
						if(liphWidthDx->value()==1)
							n->lipWidthAs=1;
						else
							if (liphWidthSx->value()==1)
								n->lipWidthAs=2;
							else
								n->lipWidthAs=0;

						if(this->cornerDx->value()==1)
							n->cornerAs=1;
						else
							if (cornerSx->value()==1)
								n->cornerAs=2;
							else
								n->cornerAs=0;

						for(i=0; i<7; i++)
							for(j=0; j<7; j++)
								iter->value[i][j]=MatrixEmotion[j][i];
					}
					newEmo=msg;
					modified=0;
					return 1;
				}
		}
	}	
	return 0;
}

int EmotionMixer::WriteEmoFile(void)
{
	int i, j;
	string line;

	ofstream outputfile("coart/emotion.txt");

	if(outputfile.is_open())
	{
		vector<EmoData>::iterator iter;
		for(iter=emotionData.begin();iter!=emotionData.end();iter++)
		{
			outputfile << ((iter)->name).c_str() << "\n";
			outputfile << ((iter)->tension) << "\n";
			outputfile << ((iter)->artic) << "\n";
			outputfile << ((iter)->cornerAs) << "\n";
			outputfile << ((iter)->lipWidthAs) << "\n";
			for(j=0; j<7; j++)
			{
				for(i=0; i<7; i++)
				{
					if (i!=6) 
						outputfile << (iter)->value[j][i] << " ";
					else
						outputfile << (iter)->value[j][i] << "\n";
				}
			}
		}
		outputfile.close();
	}
	else
		return 0;

	return 1;
}

int EmotionMixer::WriteTestFile(void)
{
	int i, j;
	float line;

	ofstream outputfile("input/Prova.xml");

	//preliminary check
	if (!this->AddEmo(1))
		return(0);
	else
		this->WriteEmoFile();
	
	if(outputfile.is_open())
	{
		outputfile << "<?xml version=\"1.0\"?>" << "\n" << "<!DOCTYPE apml SYSTEM \"apml.dtd\" []>" << "\n" << "<apml>" << "\n\n"
				   << "<performative type=\"announce\">" << "\n" << "<rheme affect=\"" << nameEmotion->value() << "\">" << "\n"
				   << "This is the <emphasis x-pitchaccent=\"Hstar\" deictic=\"u\" intensity=\"0.4\">new</emphasis><boundary type=\"LH\"/>"
				   << "expression <emphasis x-pitchaccent=\"LplusHstar\" intensity=\"0.4\">" << nameEmotion->value() << "</emphasis><boundary type=\"LL\"/>"
				   << "\n" << "</rheme>" << "\n" << "</performative>" << "\n\n" 
				    << "<performative type=\"suggest\">" << "\n" << "<rheme affect=\"" << nameEmotion->value() << "\">" << "\n"
				   << "You like<emphasis x-pitchaccent=\"Hstar\" deictic=\"u\" intensity=\"0.4\">it</emphasis><boundary type=\"LL\"/>"
				   << "dont <emphasis x-pitchaccent=\"LplusHstar\" intensity=\"0.4\">you" << "</emphasis><boundary type=\"LH\"/>"
				   << "\n" << "</rheme>" << "\n" << "</performative>" << "\n" << "<pause sec=\"2.0\"/>" << "\n" << "</apml>";
		outputfile.close();
	}
	else
		return 0;

	return 1;
}


