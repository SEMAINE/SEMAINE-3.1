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


// Parameter.h: interface for the Parameter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARAM_H__476CF3D1_7C74_4619_B288_79D9E3637683__INCLUDED_)
#define AFX_PARAM_H__476CF3D1_7C74_4619_B288_79D9E3637683__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// #include "Object.h"
#include "standard.h"
#include "XMLParser.h"
#include "Collections.h"

namespace cmlabs {

#define NOVALUE -87654321
#define PARAM_ERROR -99999999

//! Parameters can hold a value and you can get, set, increment, decrement
class Parameter : public Object  
{
public:
	static ObjectDictionary* getParameterCollectionFromNode(XMLNode* node);
	static JString getParameterValueFromCollection(ObjectDictionary* parameters, const JString& name);

	Parameter(const JString& parametername, const JString& type);
	Parameter(const JString& xml);
	Parameter(XMLNode* node);
	Parameter(const JString& parametername, const JString& type, const Collection& collValues);
	Parameter(const JString& parametername, const JString& type, const JString& val, const JString& defaultVal = "");
//	Parameter(JString parametername, double min, double max, double incr = 0);

	virtual ~Parameter();

	Object* clone() const;
	bool unitTest();
	bool fromXML(const JString& xml);
	bool fromXML(XMLNode* node);
	JString toXML();
	JString print();

	JString name;
	JString value;
	JString defaultValue;
	JString valueType;
	JString parameterType;
	JString notify;
	double step;

	bool locked;
	double minValue, maxValue;

	bool reset();
	bool increment(int steps);
	bool decrement(int steps);

	JString getName();
	bool setType(const JString& type);
	JString getType();

	int getInteger();
	double getDouble();
	JString getString();
	void* getPointer();
	JString getAsString();

	bool set(const JString& val);
	bool setString(const JString& val);
	bool setInteger(int val);
	bool setDouble(double val);
	bool setPointer(void* val);
	bool setMinMax(double min, double max);

	bool addItem(const JString& value);
	bool removeItem(const JString& value);

	Collection values;

protected:

	int current;
	int def;
};

} // namespace cmlabs

#endif // !defined(AFX_PARAM_H__476CF3D1_7C74_4619_B288_79D9E3637683__INCLUDED_)
