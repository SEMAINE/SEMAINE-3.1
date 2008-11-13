#include "stdlib.h"
#include "stdio.h"
#include "Polygon.h"
#include "Math.h"

////// ONLY TESTED WITH THE HullPolygon(VECTOR3D, int) CONSTRUCTOR

HullPolygon::HullPolygon(VECTOR2D* vertices, int verticesNo)
{
	// Initialisation de vertices
	this->vertices=vertices;
	this->verticesNo=verticesNo;
	// Initialisation de edges
	this->edgesNo=0;
	if(!(this->edges=(int**)malloc(MAX_EDGES*sizeof(int*))))
	{
		printf("ERROR: malloc failed on edges\n");
		MessageBox(NULL, "ERROR: malloc failed on edges", "Access error", MB_OK|MB_ICONERROR);
		exit(2);
	}
	for(int i=0;i<MAX_EDGES;i++)
		if(!(this->edges[i]=(int*)malloc(2*sizeof(int))))
		{
			printf("ERROR: malloc failed on edges[%i]\n", i);
			MessageBox(NULL, "ERROR: malloc failed on edges", "Access error", MB_OK|MB_ICONERROR);
			exit(2);
		}
	Algo2();
}
HullPolygon::HullPolygon(VECTOR3D* vertices, int verticesNo)
{
	// Avant d'initialiser les differents champs
	//	on calcule les deux dimensions les "plus pertinentes":
	// ce sont celles qui ont la plus grande amplitude de valeurs
	float xMin=vertices[0].x,xMax=vertices[0].x;
	float yMin=vertices[0].y,yMax=vertices[0].y;
	float zMin=vertices[0].z,zMax=vertices[0].z;
	for(int i=0;i<verticesNo;i++)
	{
		float x=vertices[i].x;
		float y=vertices[i].y;
		float z=vertices[i].z;
		if(x<xMin)	xMin=x;
		if(x>xMax)	xMax=x;
		if(y<yMin)	yMin=y;
		if(y>yMax)	yMax=y;
		if(z<zMin)	zMin=z;
		if(z>zMax)	zMax=z;
	}
	float xDif=xMax-xMin;
	float yDif=xMax-yMin;
	float zDif=xMax-zMin;
	int unusedDimension;
	if( (xDif<yDif) && (xDif<zDif))
		unusedDimension=X;
	if( (yDif<xDif) && (yDif<zDif))
		unusedDimension=Y;
	if( (zDif<yDif) && (zDif<xDif))
		unusedDimension=Z;
	IGNORED_DIMENSION=unusedDimension;

	// Initialisation de vertices
	this->verticesNo=verticesNo;
	if(!(this->vertices=(VECTOR2D*)malloc(this->verticesNo*sizeof(VECTOR2D))))
	{
		printf("ERROR: malloc failed on vertices\n");
		MessageBox(NULL, "ERROR: malloc failed on vertices", "Access error", MB_OK|MB_ICONERROR);
		exit(2);
	}
	for(i=0;i<this->verticesNo;i++)
	{
		switch(unusedDimension)
		{
			case X:
				X_DIMENSION=Y;
				Y_DIMENSION=Z;
				IGNORED_DIMENSION_MIN=xMin;
				IGNORED_DIMENSION_MAX=xMax;
				this->vertices[i].x=vertices[i].y;
				this->vertices[i].y=vertices[i].z;
				break;
			case Y:
				X_DIMENSION=X;
				Y_DIMENSION=Z;
				IGNORED_DIMENSION_MIN=yMin;
				IGNORED_DIMENSION_MAX=yMax;
				this->vertices[i].x=vertices[i].x;
				this->vertices[i].y=vertices[i].z;
				break;
			case Z:
				X_DIMENSION=X;
				Y_DIMENSION=Y;
				IGNORED_DIMENSION_MIN=zMin;
				IGNORED_DIMENSION_MAX=zMax;
				this->vertices[i].x=vertices[i].x;
				this->vertices[i].y=vertices[i].y;
				break;
			default:;
		}
	}

	// Initialisation de edges
	this->edgesNo=0;
	if(!(this->edges=(int**)malloc(MAX_EDGES*sizeof(int*))))
	{
		printf("ERROR: malloc failed on edges\n");
		MessageBox(NULL, "ERROR: malloc failed on edges", "Access error", MB_OK|MB_ICONERROR);
		exit(2);
	}
	for(i=0;i<MAX_EDGES;i++)
		if(!(this->edges[i]=(int*)malloc(2*sizeof(int))))
		{
			printf("ERROR: malloc failed on edges[%i]\n", i);
			MessageBox(NULL, "ERROR: malloc failed on edges", "Access error", MB_OK|MB_ICONERROR);
			exit(2);
		}

	// On peut maintenant lancer l'algo
	Algo2();
}
HullPolygon::~HullPolygon()
{
	for(int i=0;i<MAX_EDGES;i++)
		free(this->edges[i]);
	free(edges);
	free(vertices);
}
void HullPolygon::Algo1()
{
	// On construit un triangle de départ
	if(this->verticesNo>=3)
	{
		this->addEdge(0,1);
		this->addEdge(0,2);
		this->addEdge(1,2);
		if(this->verticesNo==3)
			return;
	}
	else
		return;

	// A partir d'ici, on a un premier HullPolygon composé
	//	d'un triangle formé par les 3 premiers sommets

	// On construit sommet par sommet le HullPolygon
	for(int i=0;i<this->verticesNo;i++)
		if(!this->contain(this->vertices[i]))
		{
			this->deleteEdgesSeenBy(i);
			this->addEdgesToConnectTo(i);
		}
}
void HullPolygon::Algo2()
{
	// On part d'un sommet dont on est sur qu'il fera parti du HullPolygon final.
	//	C'est le cas avec celui qui a la plus grande abscisse
	// Attention! En fonction de la valeur de UNUSED_DIMENSION,
	//	vertices->x et ->y ne correspondent pas forcément aux dimensions x et y
	int maxOrd=-1;
	float Max=0.0;
	for(int i=0;i<this->verticesNo;i++)
		if(abs(this->vertices[i].y)>Max)
		{
			maxOrd=i;
			Max=abs(this->vertices[i].y);
		}

	// Algo:
	VECTOR2D start;
	start.x=this->vertices[maxOrd].x;
	start.y=this->vertices[maxOrd].y;
	int added=-1;
	while(added!=maxOrd)
	{
		VECTOR2D x, y, p;
		int lastEdgeAdded_x;
		int lastEdgeAdded_p;
		if(this->edgesNo>0)
		{
			lastEdgeAdded_x=this->edges[this->edgesNo-1][0];
			lastEdgeAdded_p=this->edges[this->edgesNo-1][1];
			x=this->vertices[lastEdgeAdded_x];
			p=this->vertices[lastEdgeAdded_p];
		}
		else
		{
			lastEdgeAdded_x=-1;
			lastEdgeAdded_p=maxOrd;
			x=start;
			p.x=start.x+1;
			p.y=start.y;
		}
		/////
		int indexMaxAngle=-1;
		float maxAngle=0.0;
		for(int i=0;i<this->verticesNo;i++)
			if(i!=lastEdgeAdded_x && i!=lastEdgeAdded_p)
			{
				y=this->vertices[i];
				float thisAngle=this->angle(p, x, y);
				if(thisAngle>maxAngle)
				{
					indexMaxAngle=i;
					maxAngle=thisAngle;
				}
			}
		/////
		this->addEdge(lastEdgeAdded_p, indexMaxAngle);
		added=indexMaxAngle;
	}
	this->printEdges();
}
float HullPolygon::dist(VECTOR2D v0, VECTOR2D v1)
{
	return sqrt(pow(v1.x-v0.x,2)+pow(v1.y-v0.y,2));
}
float HullPolygon::angle(VECTOR2D p, VECTOR2D x, VECTOR2D y)
{
	return acos((pow(dist(p,x),2) + pow(dist(p,y),2) - pow(dist(x,y),2))/(2*dist(p,x)*dist(p,y)));
}
void HullPolygon::addEdge(int p0, int p1)
{
	if(this->edgesNo==MAX_EDGES)
	{
		printf("There is too many edges in the Hull Polygon\n");
		printf(" the maximum capacity is: %i edges\n", MAX_EDGES);
		MessageBox(NULL, "There is too many edges that are selected\nThe Convex Hull Algorithm cant be launch", "Access error", MB_OK|MB_ICONERROR);
		return;
	}
	// On vérifie que le coté n'existe pas déjà
	for(int i=0;i<this->edgesNo;i++)
		if( (this->edges[i][0]==p0 && this->edges[i][1]==p1) || (this->edges[i][0]==p1 && this->edges[i][1]==p0) )
			return;
	// Ajout du coté
	int j=this->edgesNo++;
	edges[j][0]=p0;
	edges[j][1]=p1;
	//printf("edges[%i][0]: %f %f\n", j, this->vertices[this->edges[j][0]].x,this->vertices[this->edges[j][0]].y);
	//printf("edges[%i][1]: %f %f\n", j, this->vertices[this->edges[j][1]].x,this->vertices[this->edges[j][1]].y);
}
void HullPolygon::addEdgesToConnectTo(int i)
{
	// Les deux cotés a ajouter ont pour sommet commun i et sont tels que:
	//	D0:(p0,i), D1:(p1,i) forme le plus grand angle possible
	VECTOR2D pi, pj;
	pi=this->vertices[this->edges[i][0]];
	int best0=0, best1=0;
	float maxangle=0.0;
	
	for(int j=0;j<this->edgesNo;j++)
	{
		for(int k=0;k<2;k++)
		{
			pj=this->vertices[this->edges[j][k]];

			float angle0,angle1;
			VECTOR2D tmp;
			tmp=this->vertices[best0];
			angle0=angle(pi,pj,tmp);
			tmp=this->vertices[best1];
			angle1=angle(pi,pj,tmp);
			if(angle0>angle1 && angle0>maxangle)
			{
				best1=this->edges[j][k];
				maxangle=angle0;
			}
			else if(angle1>angle0 && angle1>maxangle)
			{
				best0=this->edges[j][k];
				maxangle=angle1;
			}
		}
	}
	// best0 et best1 sont les deux sommets a relier à i
	addEdge(best0, i);
	addEdge(best1, i);
}
bool HullPolygon::contain(int i)
{
	// On vérifie que le point ne soit pas déjà un sommet d'un des cotés
	for(int j=0;j<this->edgesNo;j++)
		if(this->edges[j][0]==i || this->edges[j][1]==i)
			return true;	//////////////////////////// TRUE ou FALSE ???
	// Si le HullPolygon contient moins de 2 cotés, alors le point est forcément à l'extérieur
	if(this->edgesNo<2)
		return false;
	// On vérifie si, pour chaque coté du HullPolygon, le point se trouve dans le bon demi plan défini par ce coté
	//	c'est à dire: s'il existe déjà un autre point du HullPolygon qui se trouve du meme coté que le point testé
	for(j=0;j<this->edgesNo;j++)
	{
		// Trois 3 droites sont considérées:
		//	- D:	celle définie par le coté j
		//	- Di:	celle parallele à D, passant par i
		//	- Dp:	celle parallele à D, passant par un autre point appartenant déjà au HullPolygon
		// Equations du type: y=ax+b
		VECTOR2D p0, p1;
		VECTOR2D pi;
		VECTOR2D pp;
		p0=this->vertices[this->edges[j][0]];
		p1=this->vertices[this->edges[j][1]];
		pi=this->vertices[this->edges[i][0]];
		// Choix arbitraire du point: si j=0, alors on prend le point edges[1][0], sinon on prend edges[0][0]
		if(j==0)
			pp=this->vertices[this->edges[1][0]];
		else
			pp=this->vertices[this->edges[0][0]];

		// Equation de D
		float aD, bD;
		if( (p1.x-p0.x)==0 )
			if((p0.x>pi.x && p0.x>pp.x) || (p0.x<pi.x && p0.x<pp.x))
				continue;
			else
				return false;
		aD=(p1.y-p0.y)/(p1.x-p0.x);
		bD=p0.y-aD*p0.x;

		// Equation de Di
		float aDi, bDi;
		aDi=aD;
		bDi=pi.y-aDi*pi.x;

		// Equation de Dp
		float aDp, bDp;
		aDp=aD;
		bDp=pp.y-aDp*pp.x;

		if((bD>bDi && bD<bDp) || (bD<bDi && bD>bDp))
			return false;
	}
	return true;
}
bool HullPolygon::contain(VECTOR2D v)
{
	// Si le HullPolygon contient moins de 2 cotés, alors le point est forcément à l'extérieur
	if(this->edgesNo<2)
		return false;
	// On vérifie si, pour chaque coté du HullPolygon, le point se trouve dans le bon demi plan défini par ce coté
	//	c'est à dire: s'il existe déjà un autre point du HullPolygon qui se trouve du meme coté que le point testé
	for(int j=0;j<this->edgesNo;j++)
	{
		// Trois 3 droites sont considérées:
		//	- D:	celle définie par le coté j
		//	- Di:	celle parallele à D, passant par i
		//	- Dp:	celle parallele à D, passant par un autre point appartenant déjà au HullPolygon
		// Equations du type: y=ax+b
		VECTOR2D p0, p1;
		VECTOR2D pi=v;
		VECTOR2D pp;
		p0=this->vertices[this->edges[j][0]];
		p1=this->vertices[this->edges[j][1]];
		// Choix arbitraire du point: si j=0, alors on prend le point edges[1][0], sinon on prend edges[0][0]
		if(j==0)
			pp=this->vertices[this->edges[1][0]];
		else
			pp=this->vertices[this->edges[0][0]];

		// Equation de D
		float aD, bD;
		if( (p1.x-p0.x)==0 )
			if((p0.x>pi.x && p0.x>pp.x) || (p0.x<pi.x && p0.x<pp.x))
				continue;
			else
				return false;
		aD=(p1.y-p0.y)/(p1.x-p0.x);
		bD=p0.y-aD*p0.x;

		// Equation de Di
		float aDi, bDi;
		aDi=aD;
		bDi=pi.y-aDi*pi.x;

		// Equation de Dp
		float aDp, bDp;
		aDp=aD;
		bDp=pp.y-aDp*pp.x;

		if((bD>bDi && bD<bDp) || (bD<bDi && bD>bDp))
			return false;
	}
	return true;
}
bool HullPolygon::contain(VECTOR3D v)
{
	// Si le HullPolygon contient moins de 2 cotés, alors le point est forcément à l'extérieur
	if(this->edgesNo<2)
		return false;
	// On verifie si le point est bien dans l'intervalle [IGNORED_DIMENSION_MIN, IGNORED_DIMENSION_MAX]
	switch(IGNORED_DIMENSION)
	{
		case X:
			if(v.x<IGNORED_DIMENSION_MIN || v.x>IGNORED_DIMENSION_MAX)
				return false;
			break;
		case Y:
			if(v.y<IGNORED_DIMENSION_MIN || v.y>IGNORED_DIMENSION_MAX)
				return false;
			break;
		case Z:
			if(v.z<IGNORED_DIMENSION_MIN || v.z>IGNORED_DIMENSION_MAX)
				return false;
			break;
		default:;
	}
	// On vérifie si, pour chaque coté du HullPolygon, le point se trouve dans le bon demi plan défini par ce coté
	//	c'est à dire: s'il existe déjà un autre point du HullPolygon qui se trouve du meme coté que le point testé
	for(int j=0;j<this->edgesNo;j++)
	{
		// Trois 3 droites sont considérées:
		//	- D:	celle définie par le coté j
		//	- Di:	celle parallele à D, passant par i
		//	- Dp:	celle parallele à D, passant par un autre point appartenant déjà au HullPolygon
		// Equations du type: y=ax+b
		VECTOR2D p0, p1;
		VECTOR2D pi;
		VECTOR2D pp;
		switch(X_DIMENSION)
		{
			case X:
				pi.x=v.x;
				break;
			case Y:
				pi.x=v.y;
				break;
			case Z:
				pi.x=v.z;
				break;
			default:;
		}
		switch(Y_DIMENSION)
		{
			case X:
				pi.y=v.x;
				break;
			case Y:
				pi.y=v.y;
				break;
			case Z:
				pi.y=v.z;
				break;
			default:;
		}
		p0=this->vertices[this->edges[j][0]];
		p1=this->vertices[this->edges[j][1]];
		// Choix arbitraire du point: si j=0, alors on prend le point edges[1][0], sinon on prend edges[0][0]
		if(j==0)
			pp=this->vertices[this->edges[1][0]];
		else
			pp=this->vertices[this->edges[0][0]];

		// Equation de D
		float aD, bD;
		if( (p1.x-p0.x)==0 )
			if((p0.x>pi.x && p0.x>pp.x) || (p0.x<pi.x && p0.x<pp.x))
				continue;
			else
				return false;
		aD=(p1.y-p0.y)/(p1.x-p0.x);
		bD=p0.y-aD*p0.x;

		// Equation de Di
		float aDi, bDi;
		aDi=aD;
		bDi=pi.y-aDi*pi.x;

		// Equation de Dp
		float aDp, bDp;
		aDp=aD;
		bDp=pp.y-aDp*pp.x;

		if((bD>bDi && bD<bDp) || (bD<bDi && bD>bDp))
			return false;
	}
	return true;
}
void HullPolygon::deleteEdge(int i)
{
	for(int j=i+1;j<this->edgesNo;j++)
		this->edges[j-1]=this->edges[j];
	this->edgesNo--;
}
void HullPolygon::deleteEdgesSeenBy(int i)
{
	// L'idée est ici de supprimer tous les segments du HullPolygon
	//	que le point i peut voir														////////////// -> faire schéma
	// Il s'agit de résoudre le problème du "Polygon visibility"
	// La méthode proposée ici utilise
	//	(ETAPE1)
	//	en premier lieu l'algorithme utilisé dans addEdgesToConnecTo(i):
	//	elle permet d'obtenir les deux points P1 et P2 tels que les droites (P1, i) et (P2, i) soient tangentes au HullPolygon
	//	(ETAPE2)
	// La seconde partie de la méthode doit permettre, pour chaque autre point appartenant à un des segments du HullPolygon,
	//	de dire s'il se trouve du meme coté que i par rapport à la droite (P1, P2).		////////////// -> faire schéma
	//	Si c'est le cas, alors les segments du HullPolygon incluant ce point sont des segments visibles par i.


	// (ETAPE1): cf addEdgesToConnectTo(i)
	VECTOR2D pi, pj;
	pi=this->vertices[this->edges[i][0]];
	int best0=0, best1=0;
	float maxangle=0.0;
	
	for(int j=0;j<this->edgesNo;j++)
	{
		for(int k=0;k<2;k++)
		{
			pj=this->vertices[this->edges[j][k]];

			float angle0,angle1;
			VECTOR2D tmp;
			tmp=this->vertices[best0];
			angle0=angle(pi,pj,tmp);
			tmp=this->vertices[best1];
			angle1=angle(pi,pj,tmp);
			if(angle0>angle1 && angle0>maxangle)
			{
				best1=this->edges[j][k];
				maxangle=angle0;
			}
			else if(angle1>angle0 && angle1>maxangle)
			{
				best0=this->edges[j][k];
				maxangle=angle1;
			}
		}
	}

	// (ETAPE2):
	//	droite: y=ax+b
	float a,b;
	float ai,bi;
	VECTOR2D p0,p1;
	p0=this->vertices[best0];
	p1=this->vertices[best1];
	a=(p1.y-p0.y)/(p1.x-p0.x);
	b=p0.y-a*p0.x;
	ai=a;
	bi=pi.y-ai*pi.x;
	for(j=0;j<this->edgesNo;j++)
	{
		bool deleteThisEdge=false;
		for(int k=0;k<2;k++)
		{
			int index=this->edges[j][k];
			if(index==best0 || index==best1)
				continue;
			float ak,bk;
			VECTOR2D pk;
			pk=this->vertices[index];
			ak=a;
			bk=pk.y-ak*pk.x;
			// On teste si Dk (droite parallele à D passant par k)
			//	est du meme cote que Di (droite parallele à D passant par i).
			// Si c'est le cas, alors i "peut voir" le point k
			if((b>bk&&b>bi)||(b<bk&&b<bi))
				deleteThisEdge=true;
		}
		if(deleteThisEdge)
			deleteEdge(j--);
	}
}
void HullPolygon::printEdges()
{
	printf("\n\n");
	for(int i=0;i<this->edgesNo;i++)
		for(int j=0;j<2;j++)
			printf("Edges[%i][%i]:%i\n", i, j, this->edges[i][j]);
}
void HullPolygon::printVertices()
{
	printf("\n\n");
	for(int i=0;i<this->verticesNo;i++)
		printf("Vertex[%i]: %f %f\n", i, this->vertices[i].x, this->vertices[i].y);
}