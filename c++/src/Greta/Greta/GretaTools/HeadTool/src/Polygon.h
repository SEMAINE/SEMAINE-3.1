// With polygons, we are working with 2D objects

#ifndef _POLYGON_H
#define _POLYGON_H
#include "tools.h"
#define MAX_EDGES	1000	// Le nombre maximum de cotés accepté pour un HullPolygon
							// Change this value if the is really a high number of vertices for the mesh
#define X			0
#define Y			1
#define Z			2

//////////////
////////////// The structures
//////////////

class HullPolygon
{
public:
	HullPolygon(VECTOR2D* vertices, int verticesNo);
	HullPolygon(VECTOR3D* vertices, int verticesNo);
	virtual ~HullPolygon();
	void Algo1();		//////////////////////// DOESNT WORK
	void Algo2();		//////////////////////// WORKS

	// Details a propos de l'algorithme de Hull:
	//	http://www.cse.unsw.edu.au/~lambert/java/3d/incremental.html
public:
	VECTOR2D*	vertices;
	int			verticesNo;					// Nombre de sommets
	int**		edges;
	int			edgesNo;					// Nombre de cotes
	float dist(VECTOR2D v0, VECTOR2D v1);	// Distance entre les 2 points
	float angle(VECTOR2D p, VECTOR2D x, VECTOR2D y);	// Calcule la valeur de l'angle (xpy)
	void addEdge(int p0, int p1);			// Ajoute le coté (p0, p1) à la liste des cotés de HullPolygon
	void addEdgesToConnectTo(int i);		// Ajoute les 2 cotes ayant comme sommet commun i, et tels que, vu du sommet i, tous les autres sommets soient a l'interieur des 2 autres
	bool contain(int i);					// Teste si "vertices[i]" est inclu dans le polygone défini par l'ensemble des cotes "edges"
	bool contain(VECTOR2D v);				// Meme test, mais directement avec les coordonnees du point
	void deleteEdge(int i);
	void deleteEdgesSeenBy(int i);			// Du point de vue du sommet i, le polygone présente certains cotés, que cette fonction enleve
	void printEdges();
	void printVertices();
	// Dans le mode d'analyse des meilleures dimensions, d'autres ressources sont utilisees:
public:
	int IGNORED_DIMENSION;
	float IGNORED_DIMENSION_MIN;
	float IGNORED_DIMENSION_MAX;
	int X_DIMENSION;
	int Y_DIMENSION;
	bool contain(VECTOR3D v);
};

#endif	// _POLYGON_H