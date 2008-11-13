// Mesh.h: interface for the CMesh class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _MESH_H
#define _MESH_H
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <gl/glut.h>
#include <gl/glaux.h>
#include "Colors.h"
#include "Mpeg4elements.h"
//#include "tools.h"

// Where are the data read
#define DEFAULT_PATH		"_INPUT/"
#define DIRECTIVES_PATH		"_INPUT/directives"
#define OWN_PATH			"_INPUT/Untitled.own"
#define VRML_PATH			"_INPUT/Untitled.wrl"
#define WAVEFRONT_PATH		"_INPUT/Untitled.obj"
#define DETAILER_PATH		"_INPUT/Untitled.vtx"
#define DEFAULT				0
#define DIRECTIVES			1
#define OWN					2
#define VRML				3
#define WAVEFRONT			4
#define DETAILER			5

struct VECTOR2D;
struct VECTOR3D;
typedef struct VECTOR2D
{
	float x;
	float y;
} VECTOR2D;
typedef struct VECTOR3D
{
	float x;
	float y;
	float z;
} VECTOR3D;

class CMesh
{
public:
	CMesh();
	virtual ~CMesh();

public:
	/////////////
	///////////// PARAMETERS
	/////////////
	GLfloat** vertices;
	GLint** triangles;
	GLfloat** normals;
	GLfloat** texture_vertices;
	GLint** texture_triangles;

	int* colors;
	int VERTEX_NUMBER;
	int TRIANGLE_NUMBER;
	int NORMAL_NUMBER;
	int TEXTURE_VERTEX_NUMBER;
	VECTOR3D* min;
	VECTOR3D* max;
	float EPSILON;

	/////////////
	///////////// INITIALIZATION FUNCTIONS
	/////////////
	void readFile(int type);
	void readDetailerWithTextures(char* path);
	void readDirectives(char* path);
	void readOwn(char* path);
	void readVRML(char* path);
	void readWavefront(char* path);
	void readDetailer(char* path);

	int init_normal();					// initializes : GLfloat** normals  AND int NORMAL_NUMBER
	void calc_normal(float* p1,float* p2,float* p3,float* normal);	// computes a normal at a triangle
	int init_color();
	void init_maxima();

	/////////////
	///////////// OTHER FUNCTIONS & PARAMETERS
	/////////////
	void setColor(int triangle, int value);
	int trianglesNumberInRegion(int region);
};

#endif	// ifndef _MESH_H