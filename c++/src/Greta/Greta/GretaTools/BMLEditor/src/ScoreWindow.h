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

// ScoreWindow.h: interface for the ScoreWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ScoreWindow_H__FE981889_170B_11D9_9C92_FEB87D89E47F__INCLUDED_)
#define AFX_ScoreWindow_H__FE981889_170B_11D9_9C92_FEB87D89E47F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <string>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_JPEG_Image.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Output.H>
#include <fl/fl_counter.h>
#include <FL/fl_draw.H>
//#include "FL/Fl_File_Chooser.h"
#include "ChannelsImage.h"
#include "TimeSlider.h"
#include "ZoomSlider.h"
#include "Channel.h"
#include "GLContainer.h"
#include "IniManager.h"
#include "GestureEngine.h"
#include "GestureFileNames.h"
#include "FaceExpressionDictionary.h"
#include "BMLSender.h"
#include "PlayerFLTKWindow.h"
#include "SpeechWindow.h"
#include "Signal.h"
#include "TorsoEngine.h"
#include "TorsoGesture.h"

typedef struct act 
{
	std::string classname;
	std::vector<std::string> InstanceVec;
} Action;


class ScoreWindow : public Fl_Window  
{
public:
	Channel * GetChannel(std::string n);

	void *bp;
	ScoreWindow();
	virtual ~ScoreWindow();
	int handle(int e);
	std::vector<Channel> channels;
	ChannelsImage *channelsview;
	GLContainer *glcontainer;
	std::string bmlfilename;
	std::string scorelistenerfilename;
	std::string phofilename;
	void ScoreWindow::LoadLibrary();
	int ScoreWindow::LoadSpeaker();
	int ScoreWindow::LoadListener();
	void Save();
	void DeleteAll();
	void ScoreWindow::DeleteOneBlock(std::string namechannel, std::string nameblock);
	void ScoreWindow::SetMaxTimeValue();
	int ScoreWindow::WriteBML();
	int ScoreWindow::DeleteSignal(MMSystemSpace::Signal *sig);
	int ScoreWindow::LoadPhonemes();
	float ScoreWindow::FindTMValue(std::string timemarker);
	std::string ScoreWindow::CleanString(std::string str);
	Fl_Menu_Bar *menubar;
	Fl_Output *zoom;
	std::vector<Action*> HeadVec;
	std::vector<Action*> GazeVec; 
	std::vector<Action*> FacialExpressionVec;
	std::vector<Action*> GestureVec;
	std::vector<string> TorsoVec; 

	std::vector<MMSystemSpace::Signal*> SignalsVec;
	std::vector<MMSystemSpace::Signal> PhonemesVec;
	std::vector<std::string> TestoVec;

	int GenerateAnimations();
	float maxtime;
	Fl_Input *maxtimevalue;
	//std::string speakerfile;
	std::string listenerfile;
	int loadlistener;

	std::string bml;

	MMSystemSpace::Signal *speech;
	TorsoSpace::TorsoEngine *te;

	//old stuff
	std::string type;
	std::string reference;
	std::string voice;
	std::string language;
	std::string text;
	int tm; // total number of time marker

private:
	
	Fl_Button *speechbutton;
	TimeSlider *viewstarttime;
	Fl_Button *setmaxtime;
	ZoomSlider *viewzoom;
	Fl_Button *insertbutton;
	Fl_JPEG_Image *insertbuttonimage;
	Fl_JPEG_Image *playerbuttonimage;
	Fl_Button *gobutton;
	//Fl_Input *fapfilename;
	//Fl_Input *bapfilename;
	Fl_Button *loadbutton;
	Fl_Button *savebutton;
	Fl_Button *gofapbutton;
	Fl_Button *gobapbutton;
	Fl_Button *playerbutton;
	Fl_Button *playerpsydulebutton;

	void draw();
	void ScoreWindow::StartPlayer();
	PlayerFLTKWindow *player;
	std::string ToUpper(std::string s);
};

#endif // !defined(AFX_ScoreWindow_H__FE981889_170B_11D9_9C92_FEB87D89E47F__INCLUDED_)
