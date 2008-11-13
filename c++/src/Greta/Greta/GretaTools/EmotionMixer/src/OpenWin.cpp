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

#include ".\openwin.h"
#include ".\EmotionMixer.h"

void CLOSE_CB(Fl_Widget* w, void* v)
{
	((OpenWin*)v)->hide();
}

void OPEN_CB(Fl_Widget* w, void* v)
{
	if(strcmp(((OpenWin*)v)->nameEmotion->value(),"")==0)
		MessageBox(NULL, "Choose an emotion or write down a new name!", "Emotion Mixer", MB_OK | MB_ICONINFORMATION);
	else
	{
		((OpenWin*)v)->hide();
	}
}
void LIST_CB(Fl_Widget* w, void* v)
{
	int i;
	i=((OpenWin*)v)->listEmotion->value();
	((OpenWin*)v)->nameEmotion->value(((OpenWin*)v)->listEmotion->text(i));
}


OpenWin::OpenWin(void *listEmo):Fl_Window(10,20,400,300,"Open Emotion")
{
	char *pippo;
	pippo="ciccio";
	EmoDataVector *ciccio = (EmoDataVector*)listEmo;

	listEmotion = new Fl_Browser(50, 40, 300, 100, "Emotion Library");
	listEmotion->type(4);
	listEmotion->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
	listEmotion->clear();
	listEmotion->callback((Fl_Callback *)LIST_CB,this);
	
	
	vector<EmoData>::iterator iter;
	for(iter=(*ciccio).begin();iter!=(*ciccio).end();iter++)
		listEmotion->add(iter->name.c_str());

	nameEmotion=new Fl_Input(50,180,300,20,"Name Emotion");
	nameEmotion->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
	nameEmotion->value("");
	open = new Fl_Button(80, 250, 80, 25, "Open");
	open->callback((Fl_Callback *)OPEN_CB,this);
	close = new Fl_Button(240, 250, 80, 25, "Close");
	close->callback((Fl_Callback *)CLOSE_CB,this);

    this->hide();	
}

OpenWin::~OpenWin(void)
{
}

