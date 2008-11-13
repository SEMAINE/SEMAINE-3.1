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


#if !defined(_IMAGEFEATURES_H_)
#define _IMAGEFEATURES_H_

#include "JTime.h"
#include "Bitmap.h"
#include "Collections.h"
#include "MathClasses.h"

namespace cmlabs {

#define FEATURE_POINT 1
#define FEATURE_LINE 2
#define FEATURE_POLYLINE 3

//! Base class for Image Features like Lines, Points, etc.
class ImageFeature : public Object {
public:
	ImageFeature();
	virtual ~ImageFeature();

	ImageFeature* createImageFeatureFromXML(JString xml);
	ImageFeature* createImageFeatureFromXML(XMLNode* node);

	int featureType;
	JString creator;
	JTime frameTime;
	double planeDepth;
};


//! The Point type ImageFeature
class ImagePointFeature : public ImageFeature {
public:
	ImagePointFeature(JString creator, JTime t, double depth);
	ImagePointFeature(JString xml);
	ImagePointFeature(XMLNode* node);
	~ImagePointFeature();

	Object* clone() const;

	PointFloat point;
	Color color;

	PointFloat getPoint();
	Color getColor();

	bool setPoint(PointFloat p);
	bool setColor(Color c);

	JString print();
	JString toXML();
	bool fromXML(XMLNode* node);
};


//! The Line type ImageFeature
class ImageLineFeature : public ImageFeature {
public:
	ImageLineFeature(JString creator, JTime t, double depth);
	~ImageLineFeature();
	ImageLineFeature(JString xml);
	ImageLineFeature(XMLNode* node);

	Object* clone() const;

	Line line;
	Color colorAverage;
	Color colorStartPoint;
	Color colorEndPoint;
	Color colorLeftEdge;
	Color colorRightEdge;

	Line getLine();
	Color getAverageColor();
	Color getColorStartPoint();
	Color getColorEndPoint();
	Color getColorLeftEdge();
	Color getColorRightEdge();

	bool setColor(Color c);
	bool setColor(Color colorAtStartPoint, Color colorAtEndPoint);
	bool setColor(Color colorAtStartPoint, Color colorAtEndPoint, Color colorAtLeftEdge, Color colorAtRightEdge);
	
	JString print();
	JString toXML();
	bool fromXML(XMLNode* node);
};

//! The PolyLine type ImageFeature
class ImagePolyLineFeature : public ImageFeature {
public:
	ImagePolyLineFeature(JString creator, JTime t, double depth);
	~ImagePolyLineFeature();
	ImagePolyLineFeature(JString xml);
	ImagePolyLineFeature(XMLNode* node);

	Object* clone() const;

	PolyLine polyLine;
	Color color;

	PolyLine getPolyLine();
	Color getColor();

	bool setPolyLine(PolyLine polyline);
	bool setColor(Color c);

	JString print();
	JString toXML();
	bool fromXML(XMLNode* node);
};

} // namespace cmlabs


#endif // _IMAGEFEATURES_H_

