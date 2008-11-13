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


// OutputText.h: interface for the OutputText class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OUTPUTTEXT_H__9DF0ADE9_5298_4A1C_920E_7EEDCEFD0D8F__INCLUDED_)
#define AFX_OUTPUTTEXT_H__9DF0ADE9_5298_4A1C_920E_7EEDCEFD0D8F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Object.h"
#include "JString.h"
#include "Color.h"
#include <string.h>
#include <stdio.h>

#ifdef WIN32
	#include <windows.h>
	#if !defined(CYGWIN)
		#include <tchar.h> 
	#endif
#else
	typedef struct _RECT {
		long left; 
		long top; 
		long right; 
		long bottom; 
	} RECT;
	#define DT_LEFT 0
	#define DT_RIGHT 1
	#define DT_TOP 2
	#define DT_BOTTOM 3
	#define DT_CENTER 4
#endif // WIN32

namespace cmlabs {

class OutputText : public Object  
{
public:
	OutputText();
	virtual ~OutputText();

	Object* clone() const;
	JString fontname;
	int fontsize;
	unsigned int justification;

	RECT rect;

	JString text;
	
	Color bgColor;
	Color fgColor;

	bool setFont(JString name, int size, unsigned int just = DT_LEFT);

	#ifdef WIN32
		HFONT font;
		bool useFontOn(HDC hdc);
	#else
		void* font;
	#endif // WIN32
};

} // namespace cmlabs

#endif // !defined(AFX_OUTPUTTEXT_H__9DF0ADE9_5298_4A1C_920E_7EEDCEFD0D8F__INCLUDED_)
