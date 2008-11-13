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

// IniWriter.h: interface for the IniWriter class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_JPEG_Image.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Toggle_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Slider.h>
#include <fl/fl_counter.h>
#include <fl/fl_value_output.h>
#include <FL/Fl_Box.h>
#include <FL/Fl_Choice.h>
#include <vector>
#include "IniManager.h"

class IniWriter : public Fl_Window  
{
public:

	IniWriter();
	virtual ~IniWriter();
	int handle(int e);
	Fl_Check_Button *conflicts;
	Fl_Check_Button *festival;
	Fl_Check_Button *mary;
	Fl_Toggle_Button *mary_englishmale;
	Fl_Toggle_Button *mary_englishfemale;
	Fl_Toggle_Button *mary_germanmale;
	Fl_Toggle_Button *mary_germanfemale;
	Fl_Check_Button *euler;
	Fl_Toggle_Button *euler_frenchmale;
	Fl_Toggle_Button *euler_frenchfemale;
	Fl_Check_Button *realspeech;
	Fl_Toggle_Button *liptensionlight;
	Fl_Toggle_Button *liptensionnormal;
	Fl_Toggle_Button *liptensionstrong;
	Fl_Toggle_Button *articulationhypo;
	Fl_Toggle_Button *articulationmedium;
	Fl_Toggle_Button *articulationhyper;
	Fl_Toggle_Button *lipaddemotion;
	Fl_Toggle_Button *lipdataemotion;
	Fl_Toggle_Button *english_female1;
	Fl_Toggle_Button *english_male1;
	Fl_Toggle_Button *english_male2;
	Fl_Toggle_Button *italian;
	Fl_Toggle_Button *polish;
	Fl_Toggle_Button *swedish;
	Fl_Input *headnoise;
	Fl_Input *speechstretch;
	Fl_Toggle_Button *lipparLSO;
	Fl_Toggle_Button *lipparJAW;
	Fl_Toggle_Button *lipparLSW;
	Fl_Toggle_Button *lipparULP;
	Fl_Toggle_Button *lipparLLP;
	Fl_Toggle_Button *lipparCRL;
	Fl_Toggle_Button *lipparCRR;
	Fl_Check_Button *gazebn;
	Fl_Check_Button *gazenew;
	Fl_Input *MAX_T_S1_L1;
	Fl_Input *MAX_T_S1;
	Fl_Input *MAX_T_L1;
	Fl_Input *MAX_T_S0;
	Fl_Input *MAX_T_L0;
	Fl_Input *DT;
	Fl_Input *LIMIT;
	Fl_Input *SCALE;
	Fl_Check_Button *torsonoise;
	Fl_Check_Button *jointslimits;
	Fl_Toggle_Button *arminterpeuler;
	Fl_Toggle_Button *arminterpquat;
	Fl_Toggle_Button *arminterppath;
	Fl_Toggle_Button *wristinterpeuler;
	Fl_Toggle_Button *wristinterpquat;
	Fl_Slider *OAC;
	Fl_Slider *SPC;
	Fl_Slider *TMP;
	Fl_Slider *FLD;
	Fl_Slider *PWR;
	Fl_Slider *REP;
	Fl_Output *OACoutput;
	Fl_Output *SPCoutput;
	Fl_Output *TMPoutput;
	Fl_Output *FLDoutput;
	Fl_Output *PWRoutput;
	Fl_Output *REPoutput;
	Fl_Input *MARYhost;
	Fl_Input *PSYhost;
	Fl_Input *PSYport;

	Fl_Input *baselinefilename;
	Fl_Button *browsebaseline;
	Fl_Input *playerwidth;
	Fl_Input *playerheight;
	Fl_Check_Button *onlyface;
	Fl_Check_Button *showlistenersubwindow;
	Fl_Check_Button *capturevideo;
	Fl_Button *startengine;
	Fl_Button *startplayer;
	Fl_Button *quit;
	Fl_Button *cancel;
	Fl_Box *speechbox;
	Fl_Box *lipbox;
	Fl_Box *PSYbox;
	Fl_Box *MARYbox;
	Fl_Box *gazebox;
	Fl_Box *armsbox;
	Fl_Box *expressivitybox;
	Fl_Box *playerbox;
	Fl_Choice *character_speaker;
	Fl_Choice *character_listener;
	Fl_Toggle_Button *faceanimation_normal;
	Fl_Toggle_Button *faceanimation_TestMau;
	Fl_Button *MARYConnect;
	Fl_Check_Button *showlistener;
	Fl_Check_Button *gazediagram;
	void *player;

private:
	void draw();
	
public:
	void SaveParameters(void);
};
