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


#if !defined(_IMAGESPACES_H_)
#define _IMAGESPACES_H_

#include "ImageFeatures.h"

namespace cmlabs {

//! One Plane of ImageFeatures in one image
class FeaturePlane : public Object {
public:
	FeaturePlane(JString creator, JTime t, double depth);
	~FeaturePlane();
	FeaturePlane(JString xml);
	FeaturePlane(XMLNode* node);

	Object* clone() const;

	JString creator;
	JTime frameTime;
	double planeDepth;
	ObjectCollection features;

	JString print();
	JString toXML();
	bool fromXML(XMLNode* node);
};

//! Many Planes of ImageFeatures in one image
class FeatureSpace : public Object {
public:
	FeatureSpace(JString creator, JTime t, Bitmap* img = NULL);
	~FeatureSpace();
	FeatureSpace(JString xml);
	FeatureSpace(XMLNode* node);

	Object* clone() const;

	JString creator;
	JTime frameTime;
	Bitmap* image;
	ObjectDictionary planes;
	FeaturePlane* defaultPlane;

	//! Creating/adding new PointFeature
	ImagePointFeature* addPointFeature();
	//! Creating/adding new LineFeature
	ImageLineFeature* addLineFeature();
	//! Creating/adding new PolyLineFeature
	ImagePolyLineFeature* addPolyLineFeature();

	//! Create a new frame at depth btw -1.0 and +1.0
	bool addPlane(double depth);

	JString print();
	JString toXML();
	bool fromXML(XMLNode* node);
};

//! Combined ImageFeatures in one image
class FeatureCombiSpace : public Object {
public:
	FeatureCombiSpace();
	~FeatureCombiSpace();
	FeatureCombiSpace(JString xml);
	FeatureCombiSpace(XMLNode* node);

	Object* clone() const;

	//! Dictionary "owner" = FeatureSpace
	ObjectDictionary spaces;
	//! Dictionary "plane" = CombinedPlane
	ObjectDictionary planes;

	//! Add another FeatureSpace to be combined with current CombiSpace
	bool addFeatureSpace(FeatureSpace* space);

	JString print();
	JString toXML();
	bool fromXML(XMLNode* node);
};

//! Feature Space of ImageFeatures from many image
class FeatureSpaceTime : public Object {
public:
	FeatureSpaceTime();
	~FeatureSpaceTime();
	FeatureSpaceTime(JString xml);
	FeatureSpaceTime(XMLNode* node);

	Object* clone() const;

	JString print();
	JString toXML();
	bool fromXML(XMLNode* node);
};

} // namespace cmlabs


#endif // _IMAGESPACES_H_

