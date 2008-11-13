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

// EmotionMixer.h: interface for the EmotionMixer class.
//
//////////////////////////////////////////////////////////////////////


#if !defined(AFX_EmotionMixer_H__FE981889_170B_11D9_9C92_FEB87D89E47F__INCLUDED_)
#define AFX_EmotionMixer_H__FE981889_170B_11D9_9C92_FEB87D89E47F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include <string.h>
#include <vector>
#include <stdlib.h>
#include <windows.h>
#include <FL/Fl.H>
//#include <FL/Fl_Slider.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_JPEG_Image.H>
#include <FL/Fl_Output.H>
#include <fl/fl_counter.h>
#include <fl/fl_value_output.h>
#include "PlayerFLTKGLWindow.h"
#include ".\openwin.h"
#include ".\dialogwin.h"

using namespace std;

struct EmoData{
	string name;
	float tension;
	float artic;
	int cornerAs;
	int lipWidthAs;
	float value[7][7];
};

typedef std::vector<EmoData> EmoDataVector;


class EmotionMixer : public Fl_Window  
{
public:
	EmotionMixer();
	int msg;
	virtual ~EmotionMixer();
	PlayerFLTKGLWindow *glutw;
	int handle(int event);
	int ReadEmoFile(void);
	int WriteEmoFile(void);
	int WriteTestFile(void);
	int AddEmo(int inI);
	void PlayExample();
	void LoadEmo();
	void CleanAll(void);
	void draw();
	void Menu_CB(Fl_Widget*w);
	int modified;
	string newEmo;
	vector<EmoData> emotionData;
	char *EmotionName[7];
	Fl_Box *boxEmotions;
	Fl_Box *boxQualifiers;
	Fl_Box *boxAsimmetry;

	Fl_Choice *labialParameters;
	Fl_Value_Slider *basicEmotions[7];
	int IndexEmo;

	Fl_Input *lemotion;
	Fl_Slider *neutral;		//0		NEUTRAL
	Fl_Output *lneutral;
	Fl_Slider *joy;			//1		JOY
	Fl_Output *ljoy;
	Fl_Slider *surprise;	//2		SURPRISE
	Fl_Output *lsurprise;
	Fl_Slider *fear;		//3		FEAR
	Fl_Output *lfear;
	Fl_Value_Slider *anger;		//4		ANGER
	Fl_Output *langer;
	Fl_Value_Slider *disgust;		//5		DISGUST
	Fl_Output *ldisgust;
	Fl_Slider *sadness;		//6		SADNESS
	Fl_Output *lsadness;
	Fl_Check_Button *allParameters;
	Fl_Check_Button *cornerSx;
	Fl_Check_Button *cornerDx;
	Fl_Check_Button *liphWidthSx;
	Fl_Check_Button *liphWidthDx;

	Fl_Value_Slider *tension;
	Fl_Button *button_play;
	Fl_Button *button_stop;
	Fl_Button *button_frameback;
	Fl_Button *button_framenext;
	Fl_Button *button_rewind;
	
	Fl_Menu_Bar *menubar;
	Fl_Button *example;
	Fl_Input *nameEmotion;
	OpenWin *ow;
	DialogWin *dw;
	int allPar;

	float MatrixEmotion[7][7];
	void WriteFile(void);
};

#endif // !defined(AFX_EmotionMixer_H__FE981889_170B_11D9_9C92_FEB87D89E47F__INCLUDED_)
