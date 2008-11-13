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
#include <ctype.h>
#include ".\DialogWin.h"

void OK_CB(Fl_Widget* w, void* v)
{
	char emo[256];
	int i=0;
	char msg[256];
	char *p;

	sprintf(msg,"%s",((DialogWin*)v)->nameEmotion->value());
	
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
			MessageBox(NULL, "Name not valid!", "Emotion Mixer", MB_OK | MB_ICONSTOP);
			((DialogWin*)v)->nameEmotion->value("");
		}
		else
		{
			//for(i=0; 
			((DialogWin*)v)->hide();
		}
	}
}


void CANCEL_CB(Fl_Widget* w, void* v)
{
	((DialogWin*)v)->hide();
}


DialogWin::DialogWin(void):Fl_Window(10,20,300,120,"Emotion Mixer")
{
	nameEmotion=new Fl_Input(50,30,200,20,"Emotion Name:");
	nameEmotion->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
	ok = new Fl_Button(50, 80, 60, 20, "OK");
	ok->callback((Fl_Callback *)OK_CB,this);
	cancel = new Fl_Button(190,80, 60, 20, "Cancel");
	cancel->callback((Fl_Callback *)CANCEL_CB,this);
    this->hide();	
}



int DialogWin::handle(int event)
{
	char v[10];
	if(Fl::event_button()==FL_LEFT_MOUSE)
	{
		if(event==FL_PUSH)
		{
			
		}
	}
	return Fl_Window::handle(event);
}

DialogWin::~DialogWin(void)
{
}



