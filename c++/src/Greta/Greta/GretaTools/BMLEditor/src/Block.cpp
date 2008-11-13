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

// Block.cpp: implementation of the Block class.
//
//////////////////////////////////////////////////////////////////////

#include "Block.h"
#include <stdio.h>
#include <FL/fl_draw.H>
#include "ChannelsImage.h"
#include "Channel.h"
#include "ScoreWindow.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void Block::draw(void *container)
{
	Channel *ch;
	int startx,endx;
	ch=((ScoreWindow*)(((ChannelsImage*)container)->window))->GetChannel(this->channelname);
	if(ch!=0)
	{
		fl_font(FL_HELVETICA,9);
		fl_color(ch->color_r,ch->color_g,ch->color_b);
		
		startx=(this->signal->start-((ChannelsImage*)container)->starttime)*(((ChannelsImage*)container)->w())/(((ChannelsImage*)container)->endtime-((ChannelsImage*)container)->starttime);
		//endx=(this->signal->duration+((ChannelsImage*)container)->starttime)*(((ChannelsImage*)container)->w())/(((ChannelsImage*)container)->endtime-((ChannelsImage*)container)->starttime);
		endx=((this->signal->start+this->signal->duration)-((ChannelsImage*)container)->starttime)*(((ChannelsImage*)container)->w())/(((ChannelsImage*)container)->endtime-((ChannelsImage*)container)->starttime);
		
		Rect(startx,ch->position_y-this->basey+1,endx-startx+1,ch->height-1);
		
		//fl_color(0,0,0);
		//Text((char*)this->name.c_str(),startx+5-1,ch->position_y-this->basey+20+1);
		//Text((char*)this->name.c_str(),startx+5-1,ch->position_y-this->basey+20-1);
		//Text((char*)this->name.c_str(),startx+5+1,ch->position_y-this->basey+20+1);
		//Text((char*)this->name.c_str(),startx+5+1,ch->position_y-this->basey+20-1);
		if(ch->type=="speech")
			fl_color(0,0,0);
		else
			fl_color(255,255,255);
		Box(startx,ch->position_y-this->basey+1,endx-startx+1,ch->height-1);
		Text((char*)signal->id.c_str(),startx+5,ch->position_y-this->basey+15);
		
		// old stuff
		//std::string s;
		//s=this->classname+" - ";
		//s=s+this->instancename;
		
		Text((char*)signal->reference.c_str(),startx+5,ch->position_y-this->basey+25);
	}
}

void Block::Line(int x,int y,int x1,int y1)
{
	fl_line(basex+x,basey+y,basex+x1,basey+y1);
}

void Block::Text(char *s,int x,int y)
{
	fl_draw(s,basex+x,basey+y);
}

void Block::Box(int x,int y,int w,int h)
{
	fl_rect(basex+x,basey+y,w,h);
}

void Block::Rect(int x,int y,int w,int h)
{
	fl_rectf(basex+x,basey+y,w,h);
}

Block::Block(std::string chname,MMSystemSpace::Signal *sig,int bx,int by)
{
	signal=sig;
	channelname=chname;
	classname="";
	instancename="";
	basex=bx;
	basey=by; 
}

void Block::SetName(std::string n)
{
	signal->id=n;
}

void Block::SetClassInstance(std::string c,std::string i)
{
	if(c!="torso")
		signal->reference = c + "=" + i;
	else
		signal->reference = i;
	this->classname=c;
	this->instancename=i;
}

void Block::SetExpressivity(float spc, float tmp,float fld,float pwr,float rep)
{
	std::string parname="";

	parname="SPC.value";
	signal->SetParameterf(parname, spc);
	parname="TMP.value";
	signal->SetParameterf(parname, tmp);
	parname="FLD.value";
	signal->SetParameterf(parname, fld);
	parname="PWR.value";
	signal->SetParameterf(parname, pwr);
	parname="REP.value";
	signal->SetParameterf(parname, rep);
}

void Block::SetTimes(float s, float e)
{
	signal->start=s;
	signal->duration=e;
}

void Block::SetSymbolicTimes(std::string s, std::string e)
{
	signal->start_sym=s;
	signal->duration_sym=e;
}

//DEL void Block::resize(int x, int y, int w, int h)
//DEL {
//DEL 	position_x=x;
//DEL 	position_y=y;
//DEL 	width=w;
//DEL 	height=h;
//DEL }
