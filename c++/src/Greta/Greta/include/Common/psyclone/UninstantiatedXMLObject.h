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


// UninstantiatedXMLObject.h: interface for the UninstantiatedXMLObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__UNINSTANTIATEDXMLOBJECT_H__)
#define __UNINSTANTIATEDXMLOBJECT_H__

#include "Object.h"
#include "XMLParser.h"

namespace cmlabs {

class UninstantiatedXMLObject : public Object
{
public:
	UninstantiatedXMLObject(JString xml);
	UninstantiatedXMLObject(XMLNode* node);
	~UninstantiatedXMLObject();
	Object* clone() const;

	JString getXML();
	bool setXML(JString xmlString);
	JString print();
	JString toXML();
	bool fromXML(const JString& xmlString);
	bool fromXML(XMLNode* node);
	JString getClass();

private:
	JString xml;
};

} // namespace cmlabs

#endif // !defined(__UNINSTANTIATEDXMLOBJECT_H__)
