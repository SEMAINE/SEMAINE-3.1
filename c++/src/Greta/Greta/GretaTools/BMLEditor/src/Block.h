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

// Block.h: interface for the Block class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Block_H__FE981880_170B_11D9_9C92_FEB87D89E47F__INCLUDED_)
#define AFX_Block_H__FE981880_170B_11D9_9C92_FEB87D89E47F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4786)

#include <FL/Fl_Box.h>
#include <FL/Fl.H>
#include <string>
#include "Signal.h"

class Block
{
public:
	//float start,end;
	Block(std::string chname,MMSystemSpace::Signal *sig,int bx,int by);
	void draw(void *container);
	MMSystemSpace::Signal *signal;
	// name,
	std::string classname,instancename, channelname;
	//float SPC, TMP, FLD, PWR, REP;
	void SetName(std::string n);
	void Block::SetTimes(float s, float e);
	void Block::SetSymbolicTimes(std::string s, std::string e);
	void SetClassInstance(std::string c, std::string i);
	void SetExpressivity(float spc, float tmp,float fld,float pwr,float rep);
private:
	void Line(int x,int y,int x1,int y1);
	void Text(char *s,int x,int y);
	void Box(int x,int y,int w,int h);
	void Rect(int x,int y,int w,int h);
	int basex,basey;
};

#endif // !defined(AFX_Block_H__FE981880_170B_11D9_9C92_FEB87D89E47F__INCLUDED_)
