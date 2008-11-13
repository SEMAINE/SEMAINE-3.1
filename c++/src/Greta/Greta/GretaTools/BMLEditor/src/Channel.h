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

// Channel.h: interface for the Channel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Channel_H__FE981880_170B_11D9_9C92_FEB87D89E47F__INCLUDED_)
#define AFX_Channel_H__FE981880_170B_11D9_9C92_FEB87D89E47F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <FL/Fl_Box.h>
#include <FL/Fl.H>
#include <FL/Fl_Choice.H>
#include "Block.h"
#include "Signal.h"
#include <vector>
#include <string>

class Channel
{
public:
	bool IsThereBlockAt(float t);
	Block * Channel::GetTheBlockAt(float t);
	Block * GetLastBlock();
	Block * GetBlockN(int n);
	Block * GetTheBlockName(std::string blkname);
	void SetColor(int r,int g,int b);
	int color_r,color_g,color_b;
	void AddBlock(std::string chname,MMSystemSpace::Signal *sig,int bx,int by);
	void DeleteBlock(std::string nameblock);
	Channel(int x, int y, int w, int h,char *name,char *type,void *window);
	void draw(void *window);
	int position_y;
	int height;
	std::string name;
	std::vector<Block> blocks;
	std::string type;
	Fl_Choice *typechoice;
	void DeleteAll();
	void *window;
	
private:
	void Line(int x,int y,int x1,int y1);
	void Text(char *s,int x,int y);
	void Box(int x,int y,int w,int h);
	void Rect(int x,int y,int w,int h);
	int position_x;
	int width;
};

#endif // !defined(AFX_Channel_H__FE981880_170B_11D9_9C92_FEB87D89E47F__INCLUDED_)
