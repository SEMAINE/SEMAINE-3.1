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


// CVML.h: CVML related classes
//
//////////////////////////////////////////////////////////////////////

#if !defined(__CVML_H__)
#define __CVML_H__

#include "Collections.h"
#include "XMLParser.h"
#include "MathClasses.h"

namespace cmlabs {

class CVMLHypothesis : public Object
{
public:
	CVMLHypothesis();
	CVMLHypothesis(JString xml);
	CVMLHypothesis(XMLNode* node);
	virtual ~CVMLHypothesis();

	Object* clone() const;

	JString id;
	JString previous;
	double evaluation;
	Dictionary parameters;
	Dictionary evaluations;

	bool setID(JString newID);
	bool setPrevious(JString prev);
	bool setEvaluation(double eval);
	bool setLabel(JString label, JString value, double eval);

	JString getID();
	JString getPrevious();
	double getEvaluation();
	Collection getLabels();
	JString getLabelValue(JString label);
	double getLabelEval(JString label);

	JString print() ;
	JString toXML() ;
	bool fromXML(XMLNode* node);
};






class CVMLObjectBase : public Object
{
public:
	CVMLObjectBase();
	CVMLObjectBase(JString xml);
	CVMLObjectBase(XMLNode* node);
	virtual ~CVMLObjectBase();

	Object* clone() const;

	JString id;
	JString type;
	JString appearance;
	Box box;
	ObjectDictionary hypotheses;
	Dictionary features;
	Collection members;

	virtual bool setID(JString newID);
	virtual bool setBox(Box b);
	virtual bool setBox(double x, double y, double w, double h, double orientation);
	virtual bool setAppearance(JString appear);
	virtual bool addHypothesis(CVMLHypothesis* hyp);
	virtual bool addFeature(JString featurename, JString content);

	virtual JString getID();
	virtual Box getBox();
	virtual JString getAppearance();
	virtual CVMLHypothesis* getHypothesis(JString hypID);
	virtual Collection getHypothesesIDs();
	virtual Dictionary getFeatureList();

	virtual JString print() ;
	virtual JString toXML() ;
	virtual bool fromXML(XMLNode* node);
};





class CVMLObject : public CVMLObjectBase
{
public:
	CVMLObject();
	CVMLObject(JString xml);
	CVMLObject(XMLNode* node);
	virtual ~CVMLObject();
};







class CVMLGroup : public CVMLObjectBase
{
public:
	CVMLGroup();
	CVMLGroup(JString xml);
	CVMLGroup(XMLNode* node);
	virtual ~CVMLGroup();

	bool addMember(JString id);
	Collection getMembers();
};













class CVMLFrame : public Object
{
public:
	CVMLFrame();
	CVMLFrame(JString xml);
	CVMLFrame(XMLNode* node);
	virtual ~CVMLFrame();

	Object* clone() const;

	JString id;
	ObjectDictionary objects;
	ObjectDictionary groups;

	bool addObject(CVMLObject* object);
	bool addGroup(CVMLGroup* group);
	bool removeObject(JString id);
	bool removeGroup(JString id);

	JString print() ;
	JString toXML() ;
	bool fromXML(XMLNode* node);
};










class CVMLDataSet : public Object
{
public:
	CVMLDataSet();
	CVMLDataSet(JString xml);
	CVMLDataSet(XMLNode* node);
	virtual ~CVMLDataSet();

	Object* clone() const;

	JString name;
	ObjectDictionary frames;

	bool addFrame(CVMLFrame* frame);
	bool removeFrame(JString id);

	int getCount();

	JString print() ;
	JString toXML() ;
	bool fromXML(XMLNode* node);
	bool unitTest(int max);
};


} // namespace cmlabs


#endif // !defined(__CVML_H__)
