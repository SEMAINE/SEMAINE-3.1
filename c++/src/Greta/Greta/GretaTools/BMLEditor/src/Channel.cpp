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

// Channel.cpp: implementation of the Channel class.
//
//////////////////////////////////////////////////////////////////////

#include "Channel.h"
#include "Signal.h"
#include "ScoreWindow.h"
#include <stdio.h>
#include <FL/fl_draw.H>
#include "Block.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void Channel::draw(void *container)
{
	fl_color(0,0,0);
	fl_line_style(FL_SOLID);
	Box(0,0,width,height);
	fl_font(FL_HELVETICA,12);
	fl_color(color_r,color_g,color_b);
	Rect(2,2,width-4,18);

	if(type=="speech")
		fl_color(0,0,0);
	else
		fl_color(255,255,255);
	Text((char*)(name.c_str()),5,15);
	std::vector<Block>::iterator iter;
	for(iter=blocks.begin();iter!=blocks.end();iter++)
		(*iter).draw(container);
	fl_color(0,0,0);
	fl_font(FL_HELVETICA,12);
	Text("modality:",5,30);
	Text((char*)type.c_str(),5,40);

	//if(typechoice->value()==0)
	//	type="face";
	//if(typechoice->value()==1)
	//	type="gesture";

	//typechoice->redraw();
}

void Channel::Line(int x,int y,int x1,int y1)
{
	fl_line(position_x+x,position_y+y,position_x+x1,position_y+y1);
}

void Channel::Text(char *s,int x,int y)
{
	fl_draw(s,position_x+x,position_y+y);
}

void Channel::Box(int x,int y,int w,int h)
{
	fl_rect(position_x+x,position_y+y,w,h);
}

void Channel::Rect(int x,int y,int w,int h)
{
	fl_rectf(position_x+x,position_y+y,w,h);
}


Channel::Channel(int x, int y, int w, int h,char *name,char *type,void *window)
{
	position_x=x;
	position_y=y;
	width=w;
	height=h;
	this->name=name;
	blocks.clear();
	SetColor(0,0,255);
	this->type=type;
	this->window=(ScoreWindow*)window;
	//typechoice=new Fl_Choice(position_x+5,position_y+25,70,20,"");
	//typechoice->add("face",0,0,0,0);
	//typechoice->add("gesture",0,0,0,0);
	//if(strcmp(type,"face")==0)
	//{
	//	typechoice->value(0);
	//}
	//if(strcmp(type,"gesture")==0)
	//{
	//	typechoice->value(1);
	//}
}



//DEL void Channel::resize(int x, int y, int w, int h)
//DEL {
//DEL 	position_x=x;
//DEL 	position_y=y;
//DEL 	width=w;
//DEL 	height=h;
//DEL }

void Channel::AddBlock(std::string chname,MMSystemSpace::Signal *sig,int bx,int by)
{
	Block *b;
	b=new Block(chname,sig,bx,by);
	blocks.push_back(*b);
}

void Channel::SetColor(int r, int g, int b)
{
	color_r=r;
	color_g=g;
	color_b=b;
}

Block* Channel::GetTheBlockName(std::string blkname)
{
	std::vector<Block>::iterator iter;
	for(iter=blocks.begin();iter!=blocks.end();iter++)
		//if(((*iter).name==blkname))
		if(((*iter).signal->id==blkname))
			return &(*iter);
	return 0;
}

bool Channel::IsThereBlockAt(float t)
{
	std::vector<Block>::iterator iter;
	for(iter=blocks.begin();iter!=blocks.end();iter++)
		//if(((*iter).start<t)&&((*iter).end>t))
		if(((*iter).signal->start<t)&&(((*iter).signal->start+(*iter).signal->duration)>t))
			return true;
	return false;
}

Block* Channel::GetTheBlockAt(float t)
{
	std::vector<Block>::iterator iter;
	for(iter=blocks.begin();iter!=blocks.end();iter++)
		//if(((*iter).start<t)&&((*iter).end>t))
		if(((*iter).signal->start<t)&&(((*iter).signal->start+(*iter).signal->duration)>t))
			return &(*iter);
	return 0;
}

Block* Channel::GetLastBlock()
{
	if(blocks.size()>0)
		return &blocks[blocks.size()-1];
	else
		return 0;
}

Block * Channel::GetBlockN(int n)
{
	int count=0;
	std::vector<Block>::iterator iter;
	if(n>blocks.size())
		return 0;
	for(iter=blocks.begin();iter!=blocks.end();iter++)
	{
		if(count==n)
			return &(*iter);
		count++;
	}
	return 0;
}

void Channel::DeleteBlock(std::string nameblock)
{
	std::vector<Block>::iterator iterblock;
	for(iterblock=blocks.begin();iterblock!=blocks.end();iterblock++)
	{
		if((*iterblock).signal->id==nameblock)
		{

			blocks.erase(iterblock,iterblock+1);
			break;
		}
	}
}


void Channel::DeleteAll()
{
	blocks.clear();
	//typechoice->hide();
	//delete typechoice;
}
