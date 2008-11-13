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
#include ".\FaceLibraryViewerWindow.h"

void EXPRgroup(Fl_Widget*w, void* v)
{	
	int i;

	if(((OpenWin*)v)->ExprInd!=-1)
	{
		if(((OpenWin*)v)->expression[((OpenWin*)v)->ExprInd]->value()==1)
		{
			for(i=0; i<10; i++)
				if(i!=((OpenWin*)v)->ExprInd)
					((OpenWin*)v)->expression[i]->value(0);
		}
		else
			((OpenWin*)v)->expression[((OpenWin*)v)->ExprInd]->value(1);
		((OpenWin*)v)->redraw();
	}
}

void CLOSED_CB(Fl_Widget* w, void* v)
{
	((OpenWin*)v)->nameEmotion->value("");
	((OpenWin*)v)->hide();
}

void SAVE_CB(Fl_Widget* w, void* v)
{
	int i;

	if(strcmp(((OpenWin*)v)->nameEmotion->value(),"")==0)
		MessageBox(NULL, "Write a name!", "Face Library", MB_OK | MB_ICONINFORMATION);
	else
	{
		if(((OpenWin*)v)->typeWin==0)
		{
			for(i=0; i<10; i++)
				if(((OpenWin*)v)->expression[i]->value()==1)
					((OpenWin*)v)->classexpression=((OpenWin*)v)->expr[i];
		}
		((OpenWin*)v)->hide();
	}
}


OpenWin::OpenWin(char* name):Fl_Window(10,20,400,150,"")
{
	int i;
	
	this->label(name);
	this->classexpression="";

	if(strcmp(name, "Save Faps for APML format")==0)
	{
		initExpr();

		typeWin=0;
		this->size(400,400);
		boxEXPR = new Fl_Box(FL_BORDER_BOX, 20, 30, 360, 220,"Expression class");
		boxEXPR->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
		for(i=0; i<10; i++)
		{
			expression[i]=new Fl_Round_Button(50+((i/5)*170),50+((i%5)*40),16,16,expr[i]);
			expression[i]->align(FL_ALIGN_RIGHT);
			expression[i]->callback((Fl_Callback *)EXPRgroup,this);
			if(i==7)
				expression[i]->value(1);
			else
				expression[i]->value(0);
		}

		nameEmotion=new Fl_Input(50,300,300,20,"Name Expression");
		nameEmotion->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
		nameEmotion->value("");
		save = new Fl_Button(80, 350, 80, 25, "Save");
		save->callback((Fl_Callback *)SAVE_CB,this);
		close = new Fl_Button(240, 350, 80, 25, "Close");
		close->callback((Fl_Callback *)CLOSED_CB,this);
	}
	else
	{
		this->size(400,150);
		typeWin=1;
		nameEmotion=new Fl_Input(50,50,300,20,"Name Expression");
		nameEmotion->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
		nameEmotion->value("");
		save = new Fl_Button(80, 100, 80, 25, "Save");
		save->callback((Fl_Callback *)SAVE_CB,this);
		close = new Fl_Button(240, 100, 80, 25, "Close");
		close->callback((Fl_Callback *)CLOSED_CB,this);
	}
    this->hide();	
}

OpenWin::~OpenWin(void)
{
}

void OpenWin::initExpr()
{
	expr[0]="certainty";
	expr[1]="adjectival";
	expr[2]="belief-relation";
	expr[3]="performative";
	expr[4]="topic-comment";
	expr[5]="turnallocation";
	expr[6]="meta_cognitive";
	expr[7]="affect";
	expr[8]="emphasis";
	expr[9]="boundary";
}

int OpenWin::handle(int event)
{
	int i;
	if(typeWin==0)
	{
		if(Fl::event_button()==FL_LEFT_MOUSE)
		{
			if(event==FL_PUSH)
			{
				for (i=0; i<10; i++)
				{
					if(Fl::event_inside(expression[i]))
					{
						ExprInd=i;
						break;
					}
					else
					ExprInd=-1;
				}
			}
		}
	}

	return Fl_Window::handle(event);
}

