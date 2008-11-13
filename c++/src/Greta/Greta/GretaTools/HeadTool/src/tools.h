#ifndef _TOOLS_H
#define _TOOLS_H
#include "Mesh.h"

#define MAX_ELEMENTS	1024	//
class DoElement;
class Undo;
class Redo;

class DoElement
{
public:
	DoElement(int triangle, int oldRegion, int newRegion);
	int triangle;
	int oldRegion;
	int newRegion;
};

class Undo
{
public:
	int errorCount;
	int c;				// counter
	DoElement	*e[MAX_ELEMENTS];	// list of elements

	Undo();
	void add(int triangle, int oldRegion, int newRegion);
	void undo();
	int triangle();
	int oldRegion();
	int newRegion();
	int error();
};

VECTOR2D getBarycentre2D(CMesh* mesh, int triangle);
VECTOR3D getBarycentre3D(CMesh* mesh, int triangle);
float dist(VECTOR3D* v1, VECTOR3D* v2);
int getPath(char* filename, char* filter);

#endif	// _TOOLS_H