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

// TimeSlider.h: interface for the TimeSlider class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMESLIDER_H__266CF102_01A9_476E_A1B6_06A96605AF54__INCLUDED_)
#define AFX_TIMESLIDER_H__266CF102_01A9_476E_A1B6_06A96605AF54__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <fl/Fl_Scrollbar.h>
#include "ChannelsImage.h"

class TimeSlider : public Fl_Slider 
{
public:
	TimeSlider(int x, int y, int w, int h, ChannelsImage *c);
	virtual ~TimeSlider();
	int handle(int e);
	ChannelsImage *channels;
};

#endif // !defined(AFX_TIMESLIDER_H__266CF102_01A9_476E_A1B6_06A96605AF54__INCLUDED_)
