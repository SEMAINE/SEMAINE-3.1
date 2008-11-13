#include "tools.h"
#include "math.h"

DoElement::DoElement(int triangle, int oldRegion, int newRegion)
{
	this->triangle=triangle;
	this->oldRegion=oldRegion;
	this->newRegion=newRegion;
}
Undo::Undo()
{
	this->c=0;
	this->errorCount=0;
}
void Undo::add(int triangle, int oldRegion, int newRegion)
{
	printf("c_add_ -> %i\n", c);
	this->e[this->c] = new DoElement(triangle, oldRegion, newRegion);
	if(this->c>=MAX_ELEMENTS-1)
		this->c=0;
	else this->c++;
	this->errorCount++;
}
void Undo::undo()
{
	printf("c_undo -> %i\n", this->c);
	if(!this->error())
	{
		if(this->c<=0)
			this->c=MAX_ELEMENTS-1;
		else this->c--;
		this->errorCount--;
	}
}
int Undo::triangle()
{
	return (this->e[this->c])->triangle;
}
int Undo::oldRegion()
{
	return (this->e[this->c])->oldRegion;
}
int Undo::newRegion()
{
	return (this->e[this->c])->newRegion;
}
int Undo::error()
{
	printf("Error_ -> %i\n", this->errorCount);
	return !this->errorCount;
}
VECTOR3D getBarycentre3D(CMesh* mesh, int triangle)
{
	int p0,p1,p2;
	float x0,y0,z0;
	float x1,y1,z1;
	float x2,y2,z2;
	p0=mesh->triangles[triangle][0];
	p1=mesh->triangles[triangle][1];
	p2=mesh->triangles[triangle][2];
	x0=mesh->vertices[p0][0];
	x1=mesh->vertices[p1][0];
	x2=mesh->vertices[p2][0];
	y0=mesh->vertices[p0][1];
	y1=mesh->vertices[p1][1];
	y2=mesh->vertices[p2][1];
	z0=mesh->vertices[p0][2];
	z1=mesh->vertices[p1][2];
	z2=mesh->vertices[p2][2];
	// x et y sont les coordonnees (2D) du barycentre du triangle
	float x,y,z;
	x=(x0+x1+x2)/3;
	y=(y0+y1+y2)/3;
	z=(z0+z1+z2)/3;
	VECTOR3D tmp;
	tmp.x=x;
	tmp.y=y;
	tmp.z=z;

	return tmp;
}
VECTOR2D getBarycentre2D(CMesh* mesh, int triangle)
{
	int p0,p1,p2;
	float x0,y0;
	float x1,y1;
	float x2,y2;
	p0=mesh->triangles[triangle][0];
	p1=mesh->triangles[triangle][1];
	p2=mesh->triangles[triangle][2];
	x0=mesh->vertices[p0][0];
	x1=mesh->vertices[p1][0];
	x2=mesh->vertices[p2][0];
	y0=mesh->vertices[p0][1];
	y1=mesh->vertices[p1][1];
	y2=mesh->vertices[p2][1];
	// x et y sont les coordonnees (2D) du barycentre du triangle
	float x,y;
	x=(x0+x1+x2)/3;
	y=(y0+y1+y2)/3;
	VECTOR2D tmp;
	tmp.x=x;
	tmp.y=y;

	return tmp;
}
float dist(VECTOR3D* v1, VECTOR3D* v2)
{
	return sqrt(pow(v1->x-v1->x, 2) + pow(v1->y-v2->y, 2) + pow(v1->z-v2->z, 2));
}
int getPath(char* filename, char* filter)
{
	//*
	OPENFILENAME ofn;
	filename[0]='.';
	filename[1]='/';
	filename[2]='\0';

	memset (&ofn, 0, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = GetActiveWindow();
	ofn.lpstrFile = (LPSTR)filename;
	ofn.nMaxFile = 255;
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.lpstrInitialDir = NULL;

	ofn.Flags = OFN_EXPLORER;

	return ( GetOpenFileName(&ofn) );
	//*/
}