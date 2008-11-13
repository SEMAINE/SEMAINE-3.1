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


// MathClasses.h: interface for the Math type classes.
//
//////////////////////////////////////////////////////////////////////

#if !defined(MATHCLASSES_H)
#define MATHCLASSES_H

#define _USE_MATH_DEFINES
// #define __USE_GNU
#include <math.h>

#include "Object.h"
#include "Collection.h"
#include "ObjectCollection.h"
#include "XMLParser.h"

namespace cmlabs {

/////////////////////////////////////////////////////////////////////
// Size Class
/////////////////////////////////////////////////////////////////////

class Size : public Object
{
public:
	Size();
	Size(JString xml);
	Size(XMLNode* node);
	Size(double width, double height, double depth = 0);
	virtual ~Size();

	Object* clone() const;

	double w;
	double h;
	double d;

	bool equals(const Size& size) const;
	bool operator==(const Size &p) const;

	double getHeight() const;
	double getWidth() const;
	double getDepth() const;

	bool setHeight(double height);
	bool setWidth(double width);
	bool setDepth(double depth);

	bool isNonZero() const;
	double getArea() const;
	double getDiagonalLenth() const;

	JString print() ;
	JString toXML() ;
	bool fromXML(XMLNode* node);
};


/////////////////////////////////////////////////////////////////////
// Point Class
/////////////////////////////////////////////////////////////////////

class PointFloat;

class Point : public Object
{
public:
	Point();
	Point(JString xml);
	Point(XMLNode* node);
	Point(int px, int py, int pz=0, Size psize=Size());
	virtual ~Point();

	Object* clone() const;
	operator PointFloat() const;

	int x;
	int y;
	int z;
	Size size;

	int operator[](int n) const;
	bool operator==(const Point &p) const;
	bool operator==(const PointFloat &p) const;
	Point operator-(const Point &p) const;
	PointFloat operator-(const PointFloat &p) const;
	Point operator+(const Point &p) const;
	PointFloat operator+(const PointFloat &p) const;
	Point operator*(const Point &p) const;
	PointFloat operator*(const PointFloat &p) const;
	Point operator-(double a) const;
	Point operator+(double a) const;
	Point operator*(double a) const;

	int getX() const;
	int getY() const;
	int getZ() const;
	Size getSize() const;

	bool set(int x, int y, int z=0);
	bool setX(int n);
	bool setY(int n);
	bool setZ(int n);
	bool setSize(Size s);

	double getDistanceTo(Point &p) const;
	double getDistanceTo(PointFloat &p) const;

	JString print() ;
	JString toXML() ;
	bool fromXML(XMLNode* node);
};


/////////////////////////////////////////////////////////////////////
// PointFloat Class
/////////////////////////////////////////////////////////////////////

class PointFloat : public Object
{
public:
	PointFloat();
	PointFloat(JString xml);
	PointFloat(XMLNode* node);
	PointFloat(double px, double py, double pz=0, Size psize=Size());
	virtual ~PointFloat();

	Object* clone() const;
	operator Point() const;

	double x;
	double y;
	double z;
	Size size;

	double operator[](int n) const;
	bool operator==(const Point &p) const;
	bool operator==(const PointFloat &p) const;
	PointFloat operator-(const Point &p) const;
	PointFloat operator-(const PointFloat &p) const;
	PointFloat operator+(const Point &p) const;
	PointFloat operator+(const PointFloat &p) const;
	PointFloat operator*(const Point &p) const;
	PointFloat operator*(const PointFloat &p) const;
	PointFloat operator-(double a) const;
	PointFloat operator+(double a) const;
	PointFloat operator*(double a) const;

	double getX() const;
	double getY() const;
	double getZ() const;
	Size getSize() const;

	bool set(double x, double y, double z=0);
	bool setX(double v);
	bool setY(double v);
	bool setZ(double v);
	bool setSize(Size s);

	double getDistanceTo(Point &p) const;
	double getDistanceTo(PointFloat &p) const;

	JString print() ;
	JString toXML() ;
	bool fromXML(XMLNode* node);
};


/////////////////////////////////////////////////////////////////////
// Line Class
/////////////////////////////////////////////////////////////////////

class Line : public Object
{
public:
	Line();
	Line(JString xml);
	Line(XMLNode* node);
	Line(PointFloat startpoint, PointFloat endpoint, double width = 0);
	virtual ~Line();

	Object* clone() const;

	PointFloat startPoint;
	PointFloat endPoint;
	double lineWidth;

	PointFloat getStartPoint() const;
	PointFloat getEndPoint() const;
	double getLineWidth() const;

	bool setStartPoint(PointFloat point);
	bool setEndPoint(PointFloat point);
	bool setLineWidth(double width);

	JString print() ;
	JString toXML() ;
	bool fromXML(XMLNode* node);
};


/////////////////////////////////////////////////////////////////////
// PolyLine Class
/////////////////////////////////////////////////////////////////////

class PolyLine : public Object
{
public:
	PolyLine();
	PolyLine(JString xml);
	PolyLine(XMLNode* node);
	virtual ~PolyLine();

	Object* clone() const;

	ObjectCollection lines;

	int getLineCount() const;
	Line getLine(int pos) const;
	bool addLine(Line line);
	bool replaceLine(int pos, Line newline);
	bool removeLine(int pos);

	JString print() ;
	JString toXML() ;
	bool fromXML(XMLNode* node);
};


/////////////////////////////////////////////////////////////////////
// VectorOfInts Class
/////////////////////////////////////////////////////////////////////

class VectorOfInts : public Object
{
public:
	VectorOfInts();
	VectorOfInts(JString xml);
	VectorOfInts(XMLNode* node);
	virtual ~VectorOfInts();

	Object* clone() const;

	Collection coll;

	int operator[](int n) const;

	int get(int pos) const;
	bool set(int pos, int val);
	bool add(int val);
	bool remove(int pos);
	
	int getSize() const;

	JString print() ;
	JString toXML() ;
	bool fromXML(XMLNode* node);
};


/////////////////////////////////////////////////////////////////////
// VectorOfDoubles Class
/////////////////////////////////////////////////////////////////////

class VectorOfDoubles : public Object
{
public:
	VectorOfDoubles();
	VectorOfDoubles(JString xml);
	VectorOfDoubles(XMLNode* node);
	virtual ~VectorOfDoubles();

	Object* clone() const;

	Collection coll;

	double operator[](int n) const;

	double get(int pos) const;
	bool set(int pos, double val);
	bool add(double val);
	bool remove(int pos);
	
	int getSize() const;

	JString print() ;
	JString toXML() ;
	bool fromXML(XMLNode* node);
};


/////////////////////////////////////////////////////////////////////
// VectorOfPoints Class
/////////////////////////////////////////////////////////////////////

class VectorOfPoints : public Object
{
public:
	VectorOfPoints();
	VectorOfPoints(JString xml);
	VectorOfPoints(XMLNode* node);
	virtual ~VectorOfPoints();

	Object* clone() const;

	ObjectCollection coll;

	Point operator[](int n) const;

	Point get(int pos) const;
	bool set(int pos, Point p);
	bool add(Point p);
	bool remove(int pos);
	
	int getSize() const;

	JString print() ;
	JString toXML() ;
	bool fromXML(XMLNode* node);
};

/////////////////////////////////////////////////////////////////////
// Box Class
/////////////////////////////////////////////////////////////////////

class Box : public Object
{
public:
	Box();
	Box(JString xml);
	Box(XMLNode* node);
	Box(PointFloat upperleft, Size boxsize, double linewidth = 0);
	Box(PointFloat upperleft, PointFloat lowerright, double linewidth = 0);
	Box(double x, double y, double w, double h, double linewidth = 0);
	virtual ~Box();

	Object* clone() const;

	PointFloat upperLeft;
	Size size;
	double lineWidth;
	JString name;
	JString comment;
	double orientation;

	double getUpperY() const;
	double getLowerY() const;
	double getLeftX() const;
	double getRightX() const;
	double getArea() const;

	double getCMX() const;
	double getCMY() const;
	PointFloat getCM() const;

	PointFloat getUpperLeft() const;
	PointFloat getUpperRight() const;
	PointFloat getLowerLeft() const;
	PointFloat getLowerRight() const;
	double getLineWidth() const;
	Size getSize() const;
	double getWidth() const;
	double getHeight() const;

	bool move(double dx, double dy);
	bool moveTo(double x, double y);
	bool set(double x, double y, double w, double h, double linewidth = 0);
	bool setUpperLeft(const PointFloat& point);
	bool setSize(const Size& boxsize);
	bool setLineWidth(double width);
	bool grow(double dw, double dh);

	bool isPointWithin(const PointFloat& point) const;
	bool isPointWithin(int x, int y) const;
	PointFloat getCentreMass() const;

	bool hasZeroSize() const;
	bool equals(const Box &otherbox) const;
	bool equals(const Box &otherbox, double maxerror) const;
	Box getBoundingBox(const Box &otherbox) const;
	bool growToBoundingBox(const Box &otherbox);
	Box getOverlapBox(const Box &otherbox) const;
	double percentOverlap(const Box &otherbox) const;

	Box getDoubleSizeSameCenter();
	bool constrainTo(const Box& box);
	bool constrainTo(double x, double y, double w, double h);
	Box getConstrainedCopy(const Box& box);
	Box getConstrainedCopy(double x, double y, double w, double h);

	JString print() ;
	JString toXML() ;
	bool fromXML(XMLNode* node);
	bool fromXML(XMLNode* node, const Box &parentBox);
};


/////////////////////////////////////////////////////////////////////
// StatSample Class
/////////////////////////////////////////////////////////////////////

class StatSample : public Object
{
public:
	StatSample() {}
	Object* clone() const { StatSample* s = new StatSample(); *s = *this; return s; }

	bool add(double val);
	bool removeAll();

	int getCount() const;
	double getMax() const;
	double getMin() const;
	double getMean() const;
	double getRobustMean() const;
	double getVariance() const;
	double getStandardDeviation() const;
	double getRobustStandardDeviation() const;

	JString print() ;
	JString printList() ;

	Collection values;
};

/////////////////////////////////////////////////////////////////////
// Vector2D Class
/////////////////////////////////////////////////////////////////////

class Vector2D : public Object
{
public:
	Vector2D();
	Vector2D(JString xml);
	Vector2D(XMLNode* node);
	Vector2D(double x, double y);
	virtual ~Vector2D();

	Object* clone() const;

	double x;
	double y;

	double operator[](int n) const;
	bool operator==(const Vector2D &v) const;
	Vector2D operator-(const Vector2D &v) const;
	Vector2D operator+(const Vector2D &v) const;
	double operator*(const Vector2D &v) const;
	Vector2D operator-(double a) const;
	Vector2D operator+(double a) const;
	Vector2D operator*(double a) const;
	const Vector2D& operator-=(const Vector2D &v);
	const Vector2D& operator+=(const Vector2D &v);
	const Vector2D& operator-=(double a);
	const Vector2D& operator+=(double a);
	const Vector2D& operator*=(double a);

	double getX() const;
	double getY() const;

	bool set(const PointFloat& p1, const PointFloat& p2);
	bool set(double x1, double y1, double x2, double y2);
	bool set(double x, double y);
	bool setX(double v);
	bool setY(double v);

	double length() const;
	double det(const Vector2D &v) const;
	bool isOrthogonalWith(const Vector2D &v) const;
	bool isParallelWith(const Vector2D &v) const;
	Vector2D getProjectionOn(const Vector2D &v) const;
	Vector2D getUnitVector() const;
	Vector2D getOrthogonalVector() const;
	double getAngle(const Vector2D &v) const;
	double getArea(const Vector2D &v) const;
	


	JString print() ;
	JString toXML() ;
	bool fromXML(XMLNode* node);
};


/////////////////////////////////////////////////////////////////////
// Vector3D Class
/////////////////////////////////////////////////////////////////////

class Vector3D : public Object
{
public:
	Vector3D();
	Vector3D(JString xml);
	Vector3D(XMLNode* node);
	Vector3D(double x, double y, double z);
	virtual ~Vector3D();

	Object* clone() const;

	double x;
	double y;
	double z;

	double operator[](int n) const;
	bool operator==(const Vector3D &v) const;
	Vector3D operator-(const Vector3D &v) const;
	Vector3D operator+(const Vector3D &v) const;
	double operator*(const Vector3D &v) const;
	Vector3D operator-(double a) const;
	Vector3D operator+(double a) const;
	Vector3D operator*(double a) const;
	const Vector3D& operator-=(const Vector3D &v);
	const Vector3D& operator+=(const Vector3D &v);
	const Vector3D& operator-=(double a);
	const Vector3D& operator+=(double a);
	const Vector3D& operator*=(double a);

	double getX() const;
	double getY() const;
	double getZ() const;

	bool set(const PointFloat& p1, const PointFloat& p2);
	bool set(double x1, double y1, double z1, double x2, double y2, double z2);
	bool set(double x, double y, double z);
	bool setX(double v);
	bool setY(double v);
	bool setZ(double v);

	double length() const;
	Vector3D product(const Vector3D &v) const;
	bool isOrthogonalWith(const Vector3D &v) const;
	bool isParallelWith(const Vector3D &v) const;
	Vector3D getProjectionOn(const Vector3D &v) const;
	Vector3D getUnitVector() const;
	double getAngle(const Vector3D &v) const;
	double getArea(const Vector3D &v) const;
	


	JString print() ;
	JString toXML() ;
	bool fromXML(XMLNode* node);
};

} // namespace cmlabs

#endif // !defined(MATHCLASSES_H)












