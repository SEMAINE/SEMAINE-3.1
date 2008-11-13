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

// WoZWindow.h: interface for the WoZWindow class.
//
//////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Slider.H>
#include <fl/Fl_Box.h>
#include "Psydule.h"
#include "IniManager.h"
#include "AgentState.h"

using namespace cmlabs;

class WoZWindow : public Fl_Window  
{
public:

	WoZWindow();
	virtual ~WoZWindow();
	int handle(int e);
	void draw();
	std::string WoZWindow::LoadAgentState(std::string filepath);
	void WoZWindow::SendAgentState(std::string SAL);
	void WoZWindow::SendMimicryLevel();

	Psydule *psydule;
	AgentState *Poppy;
	AgentState *Prudence;
	AgentState *Obadiah;
	AgentState *Spyke;

	Fl_Slider *mimic_level;
	Fl_Output *mimic_level_output;
	Fl_Button *sendMimicryLevel;

	Fl_Button *sendPoppy;
	Fl_Button *sendPrudence;
	Fl_Button *sendObadiah;
	Fl_Button *sendSpyke;
};
