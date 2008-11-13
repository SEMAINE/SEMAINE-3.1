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


// GroupList.h: interface for the GroupList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__GROUPLIST_H__)
#define __GROUPLIST_H__

#include "thirdparty/Group.h"

namespace cmlabs {

class GroupList : public Object
{
public:
	GroupList();
	GroupList(JString xml);
	GroupList(XMLNode* node);
	virtual ~GroupList();

	Object* clone() const;

	ObjectCollection list;

	bool addGroup(Group* group);
	bool removeGroup(Group* group);
	bool removeGroup(int pos);
	bool replaceGroup(Group* oldGroup, Group* newGroup);
	bool replaceGroup(int pos, Group* group);

	Group* getGroup(int pos);
	Group* operator[](int pos);

	JString print() ;
	JString toXML() ;
	bool fromXML(XMLNode* node);
};

class SubGroupPairList : public Object
{
public:
	SubGroupPairList();
	SubGroupPairList(JString xml);
	SubGroupPairList(XMLNode* node);
	virtual ~SubGroupPairList();

	Object* clone() const;

	Dictionary list;

	bool addPair(JString parentGroupID, JString childGroupID);
	JString getChildGroupID(int pos);
	Collection getChildGroupIDs();
	JString getParentGroupOf(JString groupID);

	JString print() ;
	JString toXML() ;
	bool fromXML(XMLNode* node);
};

} // namespace cmlabs

#endif // !defined(__GROUPLIST_H__)
