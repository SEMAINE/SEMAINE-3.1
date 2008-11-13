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


// Group.h: interface for the Group class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__GROUP_H__)
#define __GROUP_H__

#include "Object.h"
#include "Collection.h"
#include "ObjectCollection.h"
#include "XMLParser.h"
#include "MathClasses.h"

namespace cmlabs {

class Group : public Object
{
public:
	Group();
	Group(JString xml);
	Group(XMLNode* node);
	Group(JString idstring, JTime t, int cmx, int cmy, int w, int h, int angle);
	virtual ~Group();

	Object* clone() const;

	JString id;
	JTime time;
	Point cm;
	Size size;
	double direction;
	VectorOfPoints pointVector;
	VectorOfDoubles vector;

	JString print() ;
	JString toXML() ;
	bool fromXML(XMLNode* node);
};

} // namespace cmlabs

#endif // !defined(__GROUP_H__)
