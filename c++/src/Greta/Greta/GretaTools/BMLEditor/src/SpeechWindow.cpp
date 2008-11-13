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

// SpeechWindow.cpp: implementation of the SpeechWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "SpeechWindow.h"
#include "ScoreWindow.h"
#include "Channel.h"
#include <stdio.h>
#include <string.h>
#include <list>
#include <string>
#include <fl/fl_widget.h>
#include <FL/fl_draw.H>



void selectedAddTM(Fl_Widget*w, void* v)
{
	std::string timemarker;
	char app[5];
	timemarker="\n<tm id=\"tm";
	timemarker+=itoa(((SpeechWindow*)v)->tm,app,10);
	timemarker+="\"/>\n";
	((SpeechWindow*)v)->text->insert(timemarker.c_str(), timemarker.length());
	((SpeechWindow*)v)->tm+=1;
	((SpeechWindow*)v)->text->take_focus();
}

void selectedOK(Fl_Widget*w, void* v)
{	
	int pos;
	int flag=1;
	int tm=1;
	std::string simpletext;
	std::vector<Channel>::iterator iterchannels;
	std::string bml;
	std::string text, text1;
	std::string tx;
	ScoreWindow *win;
	
	win=(ScoreWindow*)((SpeechWindow*)v)->scw;


	if(((SpeechWindow*)v)->type->value()!="")
		win->speech->type=((SpeechWindow*)v)->type->value();
	if(((SpeechWindow*)v)->reference->value()!="")
		win->speech->reference=((SpeechWindow*)v)->reference->value();
	if(((SpeechWindow*)v)->voice->value()!="")
		win->speech->voice=((SpeechWindow*)v)->voice->value();
	if(((SpeechWindow*)v)->language->value()!="")
		win->speech->language=((SpeechWindow*)v)->language->value();
	
	text1="";
	win->TestoVec.clear();
	text=((SpeechWindow*)v)->text->value();
	pos=text.find_first_of("<");
	while (pos!=-1)
	{
		tx=text.substr(0,pos);
		win->TestoVec.push_back(win->CleanString(tx));
		text1+=win->CleanString(tx);  
		win->TestoVec.push_back("$");	
		tm+=1;
		pos=text.find_first_of(">");
		text=text.substr(pos+1);
		pos=text.find_first_of("<");
	}

	win->TestoVec.push_back(win->CleanString(text));
	text1+=text;
	win->speech->text=text1;

	//stringa BML per speech
	bml="";
	bml+="<speech id=\"s1\" start=\"0.0\" type=\"" + win->speech->type + "\" voice=\"" + win->speech->voice +
		"\" language=\"" + win->speech->language + "\" text=\"" + text1 + "\">\n";
	bml+="<description level=\"1\" type=\"gretabml\">\n";
	bml+="<reference>" + win->speech->reference + "</reference>\n";
	bml+="</description>\n";
	bml+=((SpeechWindow*)v)->text->value();
	bml+="\n</speech>";
	win->speech->BMLcode=bml;
	win->tm=((SpeechWindow*)v)->tm;

	if(text1!="")
	//if(((SpeechWindow*)v)->text->value()!="")
	{
		if(win->speech->TemporizeTimeMarkers()==0)
			int msg = MessageBox(NULL, "Time markers cannot be defined since the Mary server is off!", "BML Editor", MB_OK|MB_ICONINFORMATION);
		else
		{
			win->PhonemesVec.clear();
			win->LoadPhonemes();
		}
	}
	else
	{
		win->PhonemesVec.clear();
		for(iterchannels=win->channels.begin();iterchannels!=win->channels.end();iterchannels++)
		if((*iterchannels).name=="speech1") break;
		(*iterchannels).DeleteAll();
	}
	((SpeechWindow*)v)->hide();
}

void selectedCancelButton(Fl_Widget*w, void* v)
{
	((SpeechWindow*)v)->hide();
}

SpeechWindow::SpeechWindow(void *window):Fl_Window(10,20,350,450,"speech window")
{
	this->size_range(350,400,350,450,1,1,0);

	id=new Fl_Input(280,10,50,20,"ID:");
	id->align(FL_ALIGN_LEFT);
	id->deactivate();
	reference=new Fl_Input(20,50,310,20,"Reference");
	reference->align(FL_ALIGN_TOP|FL_ALIGN_LEFT);
	type=new Fl_Input(20,100,310,20,"Type");
	type->align(FL_ALIGN_TOP|FL_ALIGN_LEFT);
	voice=new Fl_Input(20,150,180,20,"Voice");
	voice->align(FL_ALIGN_TOP|FL_ALIGN_LEFT);
	language=new Fl_Input(210,150,120,20,"Language");
	language->align(FL_ALIGN_TOP|FL_ALIGN_LEFT);
	
	text=new Fl_Multiline_Input(20,200,310,150,"Text");
	text->align(FL_ALIGN_TOP|FL_ALIGN_LEFT);

	addTMbutton=new Fl_Button(115,360,120,30,"Add Time Marker");
	addTMbutton->labelfont(FL_HELVETICA);
	addTMbutton->callback((Fl_Callback *)selectedAddTM,this);
	addTMbutton->align(FL_ALIGN_INSIDE|FL_ALIGN_CENTER);

	OKbutton=new Fl_Button(50,410,80,30,"OK");
	OKbutton->labelfont(FL_HELVETICA);
	OKbutton->callback((Fl_Callback *)selectedOK,this);
	OKbutton->align(FL_ALIGN_INSIDE|FL_ALIGN_CENTER);

	cancelbutton=new Fl_Button(220,410,80,30,"Cancel");
	cancelbutton->labelfont(FL_HELVETICA);
	cancelbutton->callback((Fl_Callback *)selectedCancelButton,this);
	cancelbutton->align(FL_ALIGN_INSIDE|FL_ALIGN_CENTER);

	scw=window;
	ScoreWindow *win;
	win=(ScoreWindow*)scw;
	if(win->speech->id!="")
		this->id->value(win->speech->id.c_str());
	else
		this->id->value("s1");
	if(win->speech->type!="")
		this->type->value(win->speech->type.c_str());
	if(win->speech->reference!="")
		this->reference->value(win->speech->reference.c_str());
	if(win->speech->voice!="")
		this->voice->value(win->speech->voice.c_str());
	if(win->speech->language!="")
		this->language->value(win->speech->language.c_str());
	this->tm=1;

	InsertText();
}

SpeechWindow::~SpeechWindow()
{

}

void SpeechWindow::InsertText()
{
	std::string timemarker;
	char app[5];

	ScoreWindow *win;
	win=(ScoreWindow*)scw;

	if(win->TestoVec.size()!=0)
	{
		this->text->clear_output();
		std::vector<std::string>::iterator itertv;
		for(itertv=win->TestoVec.begin();itertv!=win->TestoVec.end();itertv++)
		{
			
			if((*itertv)=="$")
			{
				timemarker="<tm id=\"tm";
				timemarker+=itoa(this->tm,app,10);
				timemarker+="\"/>\n";
				this->text->insert(timemarker.c_str(), timemarker.length());
				this->tm+=1;
			}
			else
			{
				this->text->insert((*itertv).c_str(), (*itertv).length());
				//this->text->insert("\n", 1);
			}
		}
	}
}

int SpeechWindow::handle(int e)
{	
	return Fl_Window::handle(e);
}

void SpeechWindow::draw()
{
	Fl_Window::draw();
}
