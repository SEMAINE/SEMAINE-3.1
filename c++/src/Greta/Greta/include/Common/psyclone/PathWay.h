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


// PathWay.h: interface for the PathWay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PATHWAY_H__7A206E91_94AF_4171_A04B_3CAAC34FACD9__INCLUDED_)
#define AFX_PATHWAY_H__7A206E91_94AF_4171_A04B_3CAAC34FACD9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Object.h"
#include "Dictionary.h"
#include "ObjectCollection.h"

namespace cmlabs {

class PathWay : public Object  
{
public:
	PathWay();
	PathWay(JString xml);
	virtual ~PathWay();

	Object* clone() const;
	JString toXML();
	bool add(Dictionary entry);
	Dictionary get(int n);
	JString getName(int n);
	int getNextFreePos();
	
	int getCount();

protected:

	ObjectCollection entries;

};

} // namespace cmlabs

#endif // !defined(AFX_PATHWAY_H__7A206E91_94AF_4171_A04B_3CAAC34FACD9__INCLUDED_)
