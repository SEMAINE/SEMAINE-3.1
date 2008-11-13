/***************************** License **********************************

 Copyright (C) 2006 by Communicative Machines
 http://www.cmlabs.com   All rights reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

************************************************************************/


// Color.h: interface for the Color class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLOR_H__1682DCA5_6086_49F9_8026_B40B6B373A56__INCLUDED_)
#define AFX_COLOR_H__1682DCA5_6086_49F9_8026_B40B6B373A56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef WIN32
	#include <windows.h>
#endif

#include <math.h>
#include "Object.h"
#include "XMLNode.h"
#include "JString.h"

namespace cmlabs {

class Color : public Object  
{
public:
	Color();
	Color(int red, int green, int blue);
	Color(JString name);
	Color(const Color &color);
	Color(XMLNode* node);
	virtual ~Color();

	Object* clone() const;
	bool fromXML(XMLNode* node);
	JString toXML(JString name);
	JString toXML();

	Color mixOn(Color& color, double weight);
	Color getReverseColor();
	int getGrayValue();

	int r, g, b;
	int size;

	double distance(Color &otherColor);
	bool setNoColor();
	bool isValid();
	bool isNoColor();

	static ObjectCollection* createColorsMaxDifference(int count);

#ifdef WIN32
	Color& operator =(COLORREF ref);
	operator COLORREF();
	operator HPEN();
	operator HBRUSH();
#endif // WIN32

private:
	void init(int red, int green, int blue);
	void init(JString colorName);
#ifdef WIN32
	HPEN pen;
	HBRUSH brush;
#else
	void* pen;
	void* brush;
#endif // WIN32
};

} // namespace cmlabs

#endif // !defined(AFX_COLOR_H__1682DCA5_6086_49F9_8026_B40B6B373A56__INCLUDED_)
