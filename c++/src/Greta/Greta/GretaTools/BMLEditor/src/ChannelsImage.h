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

// ChannelsImage.h: interface for the ChannelsImage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHANNELSIMAGE_H__FE981880_170B_11D9_9C92_FEB87D89E47F__INCLUDED_)
#define AFX_CHANNELSIMAGE_H__FE981880_170B_11D9_9C92_FEB87D89E47F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <FL/Fl_Box.h>
#include <FL/Fl.H>
#include "Block.h"
#include "Signal.h"

class ChannelsImage : public Fl_Box
{
public:
	void AddBlock(std::string n);
	void EndInserting();
	ChannelsImage(int x, int y, int w, int h, const char* label,void *wind);
	void draw();
	int handle(int e);
	int startingfrom;
	float zoom;
	bool inserting,possibleresize,resizing,editingblock;
	void Line(int x,int y,int x1,int y1);
	void Text(char *s,int x,int y);
	int position_x;
	int position_y;
	float dragstarttime,pushattime,movingoffset;
	std::string dragstartchannel,pushatchannel;
	Block *tempblock,*movingblock;
	void *window;
	float maxtime;
	float starttime;
	float endtime;
	float incr;

private:
	float dragendtime;
};

#endif // !defined(AFX_CHANNELSIMAGE_H__FE981880_170B_11D9_9C92_FEB87D89E47F__INCLUDED_)
