// Cube.cpp: implementation of the CCube class.
//
//////////////////////////////////////////////////////////////////////

#include "Mesh.h"
#include "tools.h"
#include "Math.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMesh::CMesh()
{
	// initialiser les donnees
	vertices=0;
	triangles=0;
	normals=0;
	texture_vertices=0;
	texture_triangles=0;
	TRIANGLE_NUMBER=0;
	min=new VECTOR3D();
	max=new VECTOR3D();
	readFile(DETAILER);
	init_normal();
	init_color();
	init_maxima();
}

CMesh::~CMesh()
{
	for(int i=0;i<VERTEX_NUMBER;i++)
		free(vertices[i]);
	free(vertices);
	for(i=0;i<TRIANGLE_NUMBER;i++)
		free(triangles[i]);
	free(triangles);
	for(i=0;i<TRIANGLE_NUMBER;i++)
		free(normals[i]);
	free(normals);
}

void CMesh::readDetailerWithTextures(char* path)
{
	//	Lecture du fichier de directives
	char* name1=path;
	FILE *geofile;

	if(!(geofile = fopen(path,"r")))
	{
		printf("ERROR: can't read \"%s\"\n", name1);
	}
	else
		fclose(geofile);

	// Lecture du fichier de données
	char cbuf[1024];
	int tri,co,vn,tm;
	int i;
	char* name2=DETAILER_PATH;

	if(!(geofile = fopen(path,"r")))
	{
		printf("ERROR: can't read \"%s\"\n", name2);
		return;
	}
	
	for(i=0;i<=20;i++)						// A changer !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		fgets( cbuf, 1024, geofile );		//	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Read the line: "mesh tri 12278 co 6264 vn 6264 tm 6566"
	sscanf(cbuf, "mesh tri %i co %i vn %i tm %i",  &tri, &co, &vn, &tm);
	printf(cbuf);
	TRIANGLE_NUMBER=tri;
	VERTEX_NUMBER=co;
	//MAURIZIO
	NORMAL_NUMBER=vn;
	TEXTURE_VERTEX_NUMBER=tm;
	printf("tri:%i co:%i vn:%i\n tn:%i\n", TRIANGLE_NUMBER, VERTEX_NUMBER, NORMAL_NUMBER, TEXTURE_VERTEX_NUMBER);
	
	//initializes the array of triangles and vertices
	if(!(this->triangles = (GLint**)malloc(tri*sizeof(GLint*))))
	{
		printf("ERROR: malloc failed on triangles\n");
		exit(2);
	}
	for(i=0;i<tri;i++)
		if(!(this->triangles[i] = (GLint *)malloc(3*sizeof(GLint))))
		{
			printf("ERROR: malloc failed on triangles[%i]", i);
			exit(2);
		}
	if(!(this->vertices=(GLfloat**)malloc(vn*sizeof(GLfloat*))))
	{
		printf("ERROR: malloc failed on vertices\n");
		exit(2);
	}
	for(i=0;i<vn;i++)
		if(!(this->vertices[i] = (GLfloat*)malloc(3*sizeof(GLfloat))))
		{
			printf("ERROR: malloc failed on vertices[%i]", i);
			exit(2);
		}
	if(!(this->normals=(GLfloat**)malloc(vn*sizeof(GLfloat*))))
	{
		printf("ERROR: malloc failed on normals\n");
		exit(2);
	}
	for(i=0;i<vn;i++)
		if(!(this->normals[i] = (GLfloat*)malloc(3*sizeof(GLfloat))))
		{
			printf("ERROR: malloc failed on normals[%i]", i);
			exit(2);
		}

	//MAURIZIO START
//initializes the array of triangles and vertices
	if(!(this->texture_triangles = (GLint**)malloc(tri*sizeof(GLint*))))
	{
		printf("ERROR: malloc failed on texture_triangles\n");
		exit(2);
	}
	for(i=0;i<tri;i++)
		if(!(this->texture_triangles[i] = (GLint *)malloc(3*sizeof(GLint))))
		{
			printf("ERROR: malloc failed on texture_triangles[%i]", i);
			exit(2);
		}
	if(!(this->texture_vertices=(GLfloat**)malloc(TEXTURE_VERTEX_NUMBER*sizeof(GLfloat*))))
	{
		printf("ERROR: malloc failed on texture_vertices\n");
		exit(2);
	}
	for(i=0;i<TEXTURE_VERTEX_NUMBER;i++)
		if(!(this->texture_vertices[i] = (GLfloat*)malloc(2*sizeof(GLfloat))))
		{
			printf("ERROR: malloc failed on vertices[%i]", i);
			exit(2);
		}


	//MAURIZIO END
	//reads verts & triangles coords and stores them in the corresponding array
	int tri_n, obj_n, eb;
	int co0, co1, co2;
	int vn0, vn1, vn2;
	int tm0, tm1, tm2;
	// LECTURE DES TRIANGLES
	for(i=0;i<tri;i++)
	{
		// On vérifie déjà qu'on ne soit pas en fin de fichier
		if(fgets( cbuf, 256, geofile ));
		else
		{
			printf("ERROR: bad geometry file\n");
			return;
		}
		// On vérifie ensuite qu'on ne lit pas une ligne de commentaires (#...)
		if(cbuf[0] == '#') while(getc(geofile) != '\n');
		else
		{
			sscanf(cbuf,"  tri %i obj %i eb %i co %i %i %i vn %i %i %i tm %i %i %i",
						   &tri_n, &obj_n, &eb, &co0, &co1, &co2, &vn0, &vn1, &vn2, &tm0, &tm1, &tm2);
			this->triangles[i][0]=co0;
			this->triangles[i][1]=co1;
			this->triangles[i][2]=co2;
			//MAURIZIO
			this->texture_triangles[i][0]=tm0;
			this->texture_triangles[i][1]=tm1;
			this->texture_triangles[i][2]=tm2;
//printf("  tri %i co %i %i %i tm %i %i %i\n",
//						   i, this->triangles[i][0], this->triangles[i][1], this->triangles[i][2],
//						   this->texture_triangles[i][0], this->texture_triangles[i][1],
//						   this->texture_triangles[i][2]);
		}
	}
	int v_n;
	GLfloat v0, v1, v2;
	// LECTURE DES SOMMETS
	for(i=0;i<vn;i++)
	{
		// On vérifie déjà qu'on ne soit pas en fin de fichier
		if(fgets( cbuf, 256, geofile ));
		else
		{
			printf("ERROR: bad geometry file\n");
			return;
		}
		// On vérifie ensuite qu'on ne lit pas une ligne de commentaires (#...)
		if(cbuf[0] == '#') while(getc(geofile) != '\n');
		else
		{
			sscanf(cbuf,"  coord %i (%f,  %f,  %f)",
						   &v_n, &v0, &v1, &v2);
			this->vertices[i][0]=v0*50;
			this->vertices[i][1]=v1*50;
			this->vertices[i][2]=v2*50;
		}
	}

	//MAURIZIO START
	// LECTURE DES NORMALS
	for(i=0;i<NORMAL_NUMBER;i++)
	{
		// On vérifie déjà qu'on ne soit pas en fin de fichier
		if(fgets( cbuf, 256, geofile ));
		else
		{
			printf("ERROR: bad geometry file\n");
			return;
		}
		// On vérifie ensuite qu'on ne lit pas une ligne de commentaires (#...)
		if(cbuf[0] == '#') while(getc(geofile) != '\n');
		else
		{
			sscanf(cbuf,"  vnormal %i (%f,  %f,  %f)",
						   &v_n, &v0, &v1, &v2);
			this->normals[i][0]=-v0;
			this->normals[i][1]=-v1;
			this->normals[i][2]=-v2;
		}
	}

	// LECTURE DES TEXTURE VERTICES COORDS
	for(i=0;i<TEXTURE_VERTEX_NUMBER;i++)
	{
		// On vérifie déjà qu'on ne soit pas en fin de fichier
		if(fgets( cbuf, 256, geofile ));
		else
		{
			printf("ERROR: bad geometry file\n");
			return;
		}
		// On vérifie ensuite qu'on ne lit pas une ligne de commentaires (#...)
		if(cbuf[0] == '#') while(getc(geofile) != '\n');
		else
		{
			sscanf(cbuf,"  tmcoord %i (%f,  %f)",
						   &v_n, &v0, &v1);
			this->texture_vertices[i][0]=v0;
			this->texture_vertices[i][1]=1.0-v1;
		}
	}
	//MAURIZIO END
  
	fclose(geofile);
	printf("Data read\n");
}


void CMesh::readFile(int type)
{
	char* path;
	path=(char*)malloc(256*sizeof(char));
	switch(type)
	{
		case DEFAULT:
			break;
		case DIRECTIVES:
			//readDirectives(DIRECTIVES_PATH);
			if(!getPath(path, "mesh files (*.vtx)\0*.vtx\0"))
			{
				printf("ERROR: failed to get path\n");
				MessageBox(NULL, "ERROR: malloc failed to get path", "Access error", MB_OK|MB_ICONERROR);
				return;
			}
			else printf("Succeed\n");
			readDirectives(path);
			break;
		case OWN:
			//readOwn(OWN_PATH);
			if(!getPath(path, "mesh files (*.vtx)\0*.vtx\0"))
			{
				printf("ERROR: failed to get path\n");
				MessageBox(NULL, "ERROR: malloc failed to get path", "Access error", MB_OK|MB_ICONERROR);
				return;
			}
			else printf("Succeed\n");
			readOwn(path);
			break;
		case VRML:
			//readVRML(VRML_PATH);
			if(!getPath(path, "mesh files (*.vtx)\0*.vtx\0"))
			{
				printf("ERROR: failed to get path\n");
				MessageBox(NULL, "ERROR: malloc failed to get path", "Access error", MB_OK|MB_ICONERROR);
				return;
			}
			else printf("Succeed\n");
			readVRML(path);
			break;
		case WAVEFRONT:
			//readWavefront(WAVEFRONT_PATH);
			if(!getPath(path, "mesh files (*.vtx)\0*.vtx\0"))
			{
				printf("ERROR: failed to get path\n");
				MessageBox(NULL, "ERROR: malloc failed to get path", "Access error", MB_OK|MB_ICONERROR);
				return;
			}
			else printf("Succeed\n");
			readWavefront(path);
			break;
		case DETAILER:
			//readDetailer(DETAILER_PATH);
			if(!getPath(path, "mesh files (*.vtx)\0*.vtx\0"))
			{
				printf("ERROR: failed to get path\n");
				MessageBox(NULL, "ERROR: malloc failed to get path", "Access error", MB_OK|MB_ICONERROR);
				return;
			}
			else printf("Succeed\n");
			readDetailerWithTextures(path);
			break;
		default:
			return;
	}
}
void CMesh::readDirectives(char* path)
{
}
void CMesh::readOwn(char* path)
{
	char cbuf[1024];
	GLint t0,t1,t2;
	GLfloat v0,v1,v2;
	int i;
	int num_t,num_v,num_tmp;
	FILE *f;

	if(!(f = fopen(path,"r")))
	{
		printf("ERROR: can't read %s\n",path);
		MessageBox(NULL, "ERROR: cant read vertices file", "Access error", MB_OK|MB_ICONERROR);
		return;
	}
		
	//reads the number of verts,edges,faces
	
	sprintf(cbuf,"");
	while(strcmp(cbuf,"VALUES\n")!=0)
		fgets( cbuf, 256, f );
	
	if(fgets(cbuf,256,f ));
	else
	{
		printf("ERROR: bad geometry file: %s\n",cbuf);
		return;
	}
	if(cbuf[0] == '#') while(getc(f) != '\n');
	else
	{
		sscanf(cbuf,"%d %d %d",&num_t,&num_v,&num_tmp);
	}
	this->TRIANGLE_NUMBER=num_t;
	this->VERTEX_NUMBER=num_v;

	//initializes the array of verts
	if(!(this->triangles = (GLint**)malloc(this->TRIANGLE_NUMBER*sizeof(GLint*))))
	{
		printf("ERROR: malloc failed on triangles\n");
		MessageBox(NULL, "ERROR: malloc failed on triangles", "Access error", MB_OK|MB_ICONERROR);
		exit(2);
	}
	for (i=0;i<this->TRIANGLE_NUMBER;i++)
	{
		if(!(this->triangles[i] = (GLint *)malloc(3*sizeof(GLint))))
		{
			printf("ERROR: malloc failed on triangles[%i]", &i);
			MessageBox(NULL, "ERROR: malloc failed on triangles", "Access error", MB_OK|MB_ICONERROR);
			exit(2);
		}
	}
	if(!(this->vertices = (GLfloat**)malloc(this->VERTEX_NUMBER*sizeof(GLfloat*))))
	{
		printf("ERROR: malloc failed on vertices\n");
		MessageBox(NULL, "ERROR: malloc failed on vertices", "Access error", MB_OK|MB_ICONERROR);
		exit(2);
	}
	for (i=0;i<this->VERTEX_NUMBER;i++)
	{
		if(!(this->vertices[i] = (GLfloat *)malloc(3*sizeof(GLfloat))))
		{
			printf("ERROR: malloc failed on vertices[%i]", &i);
		MessageBox(NULL, "ERROR: malloc failed on vertices", "Access error", MB_OK|MB_ICONERROR);
			exit(2);
		}
	}

	//reads verts coords and stores them in the vertices array
	// TRIANGLES
	sprintf(cbuf,"");
	while(strcmp(cbuf,"TRIANGLES\n")!=0)
		fgets( cbuf, 256, f );

	for(i=0; i<this->TRIANGLE_NUMBER; i++)
	{
		if(fgets( cbuf, 256, f ));
		else
		{
			printf("ERROR: bad geometry file\n");
			return;
		}
		if(cbuf[0] == '#') while(getc(f) != '\n');
		else
		{
			sscanf(cbuf,"%i %i %i",&t0,&t1,&t2);
			this->triangles[i][0]=t0;
			this->triangles[i][1]=t1;
			this->triangles[i][2]=t2;
		}
	}
	// VERTICES
	sprintf(cbuf,"");
	while(strcmp(cbuf,"VERTICES\n")!=0)
		fgets( cbuf, 256, f );

	for(i=0; i<this->VERTEX_NUMBER; i++)
	{
		if(fgets( cbuf, 256, f ));
		else
		{
			printf("ERROR: bad geometry file\n");
			return;
		}
		if(cbuf[0] == '#') while(getc(f) != '\n');
		else
		{
			sscanf(cbuf,"%f %f %f",&v0,&v1,&v2);
			this->vertices[i][0]=v0;
			this->vertices[i][1]=v1;
			this->vertices[i][2]=v2;
		}
	}  
	fclose(f);
}
void CMesh::readVRML(char* path)
{
}
void CMesh::readWavefront(char* path)
{
}
void CMesh::readDetailer(char* path)
{
	//	Lecture du fichier de directives
	char* name1=path;
	FILE *geofile;

	if(!(geofile = fopen(path,"r")))
	{
		printf("ERROR: can't read \"%s\"\n", name1);
		MessageBox(NULL, "ERROR: cant read this file", "Access error", MB_OK|MB_ICONERROR);
	}
	else
		fclose(geofile);

	// Lecture du fichier de données
	char cbuf[1024];
	int tri,co,vn,tm;
	int i;
	char* name2=DETAILER_PATH;

	if(!(geofile = fopen(path,"r")))
	{
		printf("ERROR: can't read \"%s\"\n", name2);
		MessageBox(NULL, "ERROR: cant read this file", "Access error", MB_OK|MB_ICONERROR);
		return;
	}
	
	for(i=0;i<=20;i++)						// A changer !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		fgets( cbuf, 256, geofile );		//	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Read the line: "mesh tri 12278 co 6264 vn 6264 tm 6566"
	sscanf(cbuf, "mesh tri %i co %i vn %i tm %i",  &tri, &co, &vn, &tm);
	printf(cbuf);
	TRIANGLE_NUMBER=tri;
	VERTEX_NUMBER=co;
	//MAURIZIO
	NORMAL_NUMBER=vn;
	TEXTURE_VERTEX_NUMBER=tm;
	printf("tri:%i co:%i vn:%i\n tn:%i\n", TRIANGLE_NUMBER, VERTEX_NUMBER, NORMAL_NUMBER, TEXTURE_VERTEX_NUMBER);
	
	//initializes the array of triangles and vertices
	if(!(this->triangles = (GLint**)malloc(tri*sizeof(GLint*))))
	{
		printf("ERROR: malloc failed on triangles\n");
		MessageBox(NULL, "ERROR: malloc failed on triangles", "Access error", MB_OK|MB_ICONERROR);
		exit(2);
	}
	for(i=0;i<tri;i++)
		if(!(this->triangles[i] = (GLint *)malloc(3*sizeof(GLint))))
		{
			printf("ERROR: malloc failed on triangles[%i]", i);
			MessageBox(NULL, "ERROR: malloc failed on triangles", "Access error", MB_OK|MB_ICONERROR);
			exit(2);
		}
	if(!(this->vertices=(GLfloat**)malloc(vn*sizeof(GLfloat*))))
	{
		printf("ERROR: malloc failed on vertices\n");
		MessageBox(NULL, "ERROR: malloc failed on vertices", "Access error", MB_OK|MB_ICONERROR);
		exit(2);
	}
	for(i=0;i<vn;i++)
		if(!(this->vertices[i] = (GLfloat*)malloc(3*sizeof(GLfloat))))
		{
			printf("ERROR: malloc failed on vertices[%i]", i);
			MessageBox(NULL, "ERROR: malloc failed on vertices", "Access error", MB_OK|MB_ICONERROR);
			exit(2);
		}
	if(!(this->normals=(GLfloat**)malloc(vn*sizeof(GLfloat*))))
	{
		printf("ERROR: malloc failed on normals\n");
		MessageBox(NULL, "ERROR: malloc failed on normals", "Access error", MB_OK|MB_ICONERROR);
		exit(2);
	}
	for(i=0;i<vn;i++)
		if(!(this->normals[i] = (GLfloat*)malloc(3*sizeof(GLfloat))))
		{
			printf("ERROR: malloc failed on normals[%i]", i);
			MessageBox(NULL, "ERROR: malloc failed on normals", "Access error", MB_OK|MB_ICONERROR);
			exit(2);
		}

	//MAURIZIO START
//initializes the array of triangles and vertices
	if(!(this->texture_triangles = (GLint**)malloc(tri*sizeof(GLint*))))
	{
		printf("ERROR: malloc failed on texture_triangles\n");
		MessageBox(NULL, "ERROR: malloc failed on texture_triangles", "Access error", MB_OK|MB_ICONERROR);
		exit(2);
	}
	for(i=0;i<tri;i++)
		if(!(this->texture_triangles[i] = (GLint *)malloc(3*sizeof(GLint))))
		{
			printf("ERROR: malloc failed on texture_triangles[%i]", i);
			MessageBox(NULL, "ERROR: malloc failed on texture_triangles", "Access error", MB_OK|MB_ICONERROR);
			exit(2);
		}
	if(!(this->texture_vertices=(GLfloat**)malloc(TEXTURE_VERTEX_NUMBER*sizeof(GLfloat*))))
	{
		printf("ERROR: malloc failed on texture_vertices\n");
		MessageBox(NULL, "ERROR: malloc failed on texture_vertices", "Access error", MB_OK|MB_ICONERROR);
		exit(2);
	}
	for(i=0;i<TEXTURE_VERTEX_NUMBER;i++)
		if(!(this->texture_vertices[i] = (GLfloat*)malloc(2*sizeof(GLfloat))))
		{
			printf("ERROR: malloc failed on texture_vertices[%i]", i);
			MessageBox(NULL, "ERROR: malloc failed on texture_vertices", "Access error", MB_OK|MB_ICONERROR);
			exit(2);
		}


	//MAURIZIO END
	//reads verts & triangles coords and stores them in the corresponding array
	int tri_n, obj_n, eb;
	int co0, co1, co2;
	int vn0, vn1, vn2;
	int tm0, tm1, tm2;
	// LECTURE DES TRIANGLES
	for(i=0;i<tri;i++)
	{
		// On vérifie déjà qu'on ne soit pas en fin de fichier
		if(fgets( cbuf, 256, geofile ));
		else
		{
			printf("ERROR: bad geometry file\n");
			return;
		}
		// On vérifie ensuite qu'on ne lit pas une ligne de commentaires (#...)
		if(cbuf[0] == '#') while(getc(geofile) != '\n');
		else
		{
			sscanf(cbuf,"  tri %i obj %i eb %i co %i %i %i vn %i %i %i tm %i %i %i",
						   &tri_n, &obj_n, &eb, &co0, &co1, &co2, &vn0, &vn1, &vn2, &tm0, &tm1, &tm2);
			this->triangles[i][0]=co0;
			this->triangles[i][1]=co1;
			this->triangles[i][2]=co2;
			//MAURIZIO
			this->texture_triangles[i][0]=tm0;
			this->texture_triangles[i][1]=tm1;
			this->texture_triangles[i][2]=tm2;
//printf("  tri %i co %i %i %i tm %i %i %i\n",
//						   i, this->triangles[i][0], this->triangles[i][1], this->triangles[i][2],
//						   this->texture_triangles[i][0], this->texture_triangles[i][1],
//						   this->texture_triangles[i][2]);
		}
	}
	int v_n;
	GLfloat v0, v1, v2;
	// LECTURE DES SOMMETS
	for(i=0;i<vn;i++)
	{
		// On vérifie déjà qu'on ne soit pas en fin de fichier
		if(fgets( cbuf, 256, geofile ));
		else
		{
			printf("ERROR: bad geometry file\n");
			return;
		}
		// On vérifie ensuite qu'on ne lit pas une ligne de commentaires (#...)
		if(cbuf[0] == '#') while(getc(geofile) != '\n');
		else
		{
			sscanf(cbuf,"  coord %i (%f,  %f,  %f)",
						   &v_n, &v0, &v1, &v2);
			this->vertices[i][0]=v0*50;
			this->vertices[i][1]=v1*50;
			this->vertices[i][2]=v2*50;
		}
	}

	//MAURIZIO START
	// LECTURE DES NORMALS
	for(i=0;i<NORMAL_NUMBER;i++)
	{
		// On vérifie déjà qu'on ne soit pas en fin de fichier
		if(fgets( cbuf, 256, geofile ));
		else
		{
			printf("ERROR: bad geometry file\n");
			return;
		}
		// On vérifie ensuite qu'on ne lit pas une ligne de commentaires (#...)
		if(cbuf[0] == '#') while(getc(geofile) != '\n');
		else
		{
			sscanf(cbuf,"  vnormal %i (%f,  %f,  %f)",
						   &v_n, &v0, &v1, &v2);
			this->normals[i][0]=-v0;
			this->normals[i][1]=-v1;
			this->normals[i][2]=-v2;
		}
	}

	// LECTURE DES TEXTURE VERTICES COORDS
	for(i=0;i<TEXTURE_VERTEX_NUMBER;i++)
	{
		// On vérifie déjà qu'on ne soit pas en fin de fichier
		if(fgets( cbuf, 256, geofile ));
		else
		{
			printf("ERROR: bad geometry file\n");
			return;
		}
		// On vérifie ensuite qu'on ne lit pas une ligne de commentaires (#...)
		if(cbuf[0] == '#') while(getc(geofile) != '\n');
		else
		{
			sscanf(cbuf,"  tmcoord %i (%f,  %f)",
						   &v_n, &v0, &v1);
			this->texture_vertices[i][0]=v0;
			this->texture_vertices[i][1]=1.0-v1;
		}
	}
	//MAURIZIO END
  
	fclose(geofile);
	printf("Data read\n");
}
int CMesh::init_normal()
{
	/*
	int i;
	if(!(normals=(GLfloat**)malloc(TRIANGLE_NUMBER*sizeof(GLfloat*))))
	{
		printf("ERROR: malloc failed on normals\n");
		exit(2);
	}
	for(i=0;i<TRIANGLE_NUMBER;i++)
	{
		if(!(normals[i]=(GLfloat*)malloc(3*sizeof(GLfloat))))
		{
			printf("ERROR: malloc failed on normals[%i]\n",i);
			exit(2);
		}
	}
	for(i=0;i<TRIANGLE_NUMBER;i++)
	{
		float* p1=vertices[triangles[i][0]];
		float* p2=vertices[triangles[i][1]];
		float* p3=vertices[triangles[i][2]];
		//float* normali;
		calc_normal (p1,p2,p3,normals[i]);
	}
	printf("Normals computed\n");
	*/
	return 1;
}
void CMesh::calc_normal(float *p1,float *p2,float *p3,float *norm)
// p1,p2,p3 : each vertex of the triangle
{
	float coa, cob, coc ;
	float px1, py1, pz1 ;
	float px2, py2, pz2 ;
	float px3, py3, pz3 ;
	float absvec ;

	px1 = p1[0] ;
	py1 = p1[1] ;
	pz1 = p1[2] ;

	px2 = p2[0] ;
	py2 = p2[1] ;
	pz2 = p2[2] ;

	px3 = p3[0] ;
	py3 = p3[1] ;
	pz3 = p3[2] ;

	coa = -(py1 * (pz2-pz3) + py2*(pz3-pz1) + py3*(pz1-pz2)) ;
	cob = -(pz1 * (px2-px3) + pz2*(px3-px1) + pz3*(px1-px2)) ;
	coc = -(px1 * (py2-py3) + px2*(py3-py1) + px3*(py1-py2)) ;

	absvec = sqrt ((double) ((coa*coa) + (cob*cob) + (coc*coc))) ;
	//absvec = 1.0;	

	norm[0] = coa/absvec ;
	norm[1] = cob/absvec ;
	norm[2] = coc/absvec ;
}

int CMesh::init_color()
{
	int i;
	if(!(colors=(int*)malloc(TRIANGLE_NUMBER*sizeof(int))))
	{
		printf("ERROR: malloc failed on colors\n");
		MessageBox(NULL, "ERROR: malloc failed on colors", "Access error", MB_OK|MB_ICONERROR);
		exit(2);
	}
	for(i=0;i<TRIANGLE_NUMBER;i++)
		setColor(i, WHITE);
	return 1;
}
void CMesh::init_maxima()
{
	// on suppose que les listes ne sont pas vides...
	GLfloat* vertex=this->vertices[0];
	GLfloat mX=vertex[0], mY=vertex[1], mZ=vertex[2];
	GLfloat MX=vertex[0], MY=vertex[1], MZ=vertex[2];
	for(int i=1;i<this->VERTEX_NUMBER;i++)
	{
		GLfloat x, y, z;
		vertex=this->vertices[i];
		x=vertex[0];
		y=vertex[1];
		z=vertex[2];
		mX=(x<mX?x:mX);
		mY=(y<mY?y:mY);
		mZ=(z<mX?z:mZ);
		MX=(x>MX?x:MX);
		MY=(y>MY?y:MY);
		MZ=(z>MX?z:MZ);
	}
	this->max->x=MX;
	this->max->y=MY;
	this->max->z=MZ;
	this->min->x=mX;
	this->min->y=mY;
	this->min->z=mZ;
	this->EPSILON=dist(max, min)/250;
	printf("EPS: %f\n", this->EPSILON);
}
void CMesh::setColor(int triangle, int value)
{
	colors[triangle]=value;
}
int CMesh::trianglesNumberInRegion(int region)
{
	int cpt=0;
	for(int i=0;i<TRIANGLE_NUMBER;i++)
		if(colors[i]==region) cpt++;
	return cpt;
}