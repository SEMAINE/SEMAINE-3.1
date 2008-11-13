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


// FeatureVectorList.h: interface for the FeatureVectorList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__FEATUREVECTORLIST_H__)
#define __FEATUREVECTORLIST_H__

#include "thirdparty/FeatureVector.h"

namespace cmlabs {

class FeatureVectorList : public Object
{
public:
	FeatureVectorList();
	FeatureVectorList(JString xml);
	FeatureVectorList(XMLNode* node);
	virtual ~FeatureVectorList();

	Object* clone() const;

	ObjectCollection list;

	bool addVector(FeatureVector* vector);
	bool removeVector(FeatureVector* vector);
	bool removeVector(int pos);
	bool replaceVector(FeatureVector* oldvector, FeatureVector* newvector);
	bool replaceVector(int pos, FeatureVector* vector);

	FeatureVector* getVector(int pos);
	FeatureVector* operator[](int pos);

	JString print() ;
	JString toXML() ;
	bool fromXML(XMLNode* node);
};

} // namespace cmlabs

#endif // !defined(__FEATUREVECTORLIST_H__)
