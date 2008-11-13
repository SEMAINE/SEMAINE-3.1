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


// DotString.h: interface for the DotString class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOTSTRING_H__3EAB9C48_F4ED_4D8B_80F1_692D4A877E64__INCLUDED_)
#define AFX_DOTSTRING_H__3EAB9C48_F4ED_4D8B_80F1_692D4A877E64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Object.h"
#include "Collection.h"
#include "Debug.h"

namespace cmlabs {

class DotString : public Object  
{
public:
	DotString(const JString& text = "");
	DotString(const Collection& oc);
//	DotString(DotString &ds);
	virtual ~DotString();

	operator char*();
	operator JString();

	Object* clone() const;
	JString toXML();
	JString toText();
	JString print();

//	DotString& operator =(DotString& ds);
	bool operator ==(const DotString& ds) const;
	bool operator !=(const DotString& ds) const;
	JString operator [](int n);
	int getCount() const;
	DotString getFirst(int n);
	DotString getLast(int n);

	bool isPartOf(const DotString& ds) const;
	bool isSupersededBy(const DotString& ds) const;
	bool equals(const DotString& ds) const;
	bool equals(const JString& str) const;
	bool equals(const Object* o2) const;
	bool unitTest();

	Collection entries;
	JString comment;
};

} // namespace cmlabs

#endif // !defined(AFX_DOTSTRING_H__3EAB9C48_F4ED_4D8B_80F1_692D4A877E64__INCLUDED_)
