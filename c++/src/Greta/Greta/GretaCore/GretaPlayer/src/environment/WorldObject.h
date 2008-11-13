#pragma once

#include <gl/glaux.h>
#include <string>
#include <vector>



/**
*
* class :WorldObject
*
*/

class WorldObject
{

	struct threeDpoint
	{
		float x;
		float y;
		float z;
	};
	struct twoDpoint
	{
		float x;
		float y;
	};

	struct triangle
	{
		int v1,v2,v3,n1,n2,n3,uv1,uv2,uv3;
	};

public:

	/**
	* constructor 
	*
	* @param  name
	* @param  objectshape
	*/

	WorldObject(std::string name,std::string objectshape);

	/**
	*
	* destructor 
	*/

	~WorldObject(void);

	/**
	*  
	* 
	*
	*/

	void draw();
	GLuint texture;

	/**
	*  
	* 
	*
	* @return 
	* @param Filename
	*/

	AUX_RGBImageRec *LoadBMP(char *Filename);

	/**
	*  
	* 
	*
	* @return 
	* @param src
	* @param  int size
	*/

	unsigned char* AddAlpha(unsigned char *src, int size);

	/**
	*  
	* 
	*
	* @return 
	* @param  filename
	* @param  alpha
	* @param texture_dest
	*/

	int LoadGLTextures(std::string filename,bool alpha,GLuint *texture_dest);

	/**
	*  
	* 
	*
	*/

	void InitGLTextures();

	/**
	*  
	* 
	*
	* @return 
	* @param  filename
	*/

	int ReadMesh(std::string filename);

	/**
	*  
	* 
	*
	* @param  x
	* @param  y
	* @param  z
	*/

	void Move(float x,float y,float z);

	/**
	*  
	* 
	*
	* @param  x
	* @param  y
	* @param  z
	*/

	void Rotate(float x,float y,float z);

	std::string name,shape;
	std::vector<threeDpoint> vertices;
	std::vector<threeDpoint> normals;
	std::vector<twoDpoint> uvs;
	std::vector<triangle> triangles;
	int num_v,num_n,num_uv,num_t;
	float x,y,z,rotx,roty,rotz;
	GLfloat material_ambient[4];
	GLfloat material_specular[4];
	GLfloat material_shininess[1];
	bool linearmapping;
};
