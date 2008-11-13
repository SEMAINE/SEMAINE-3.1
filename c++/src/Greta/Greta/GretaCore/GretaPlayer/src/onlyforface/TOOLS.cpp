//Copyright 1999-2008 Catherine Pelachaud - c.pelachaud@iut.univ-paris8.fr
//
//This file is part of Greta.
//
//Greta is free software; you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation; either version 2 of the License, or
//(at your option) any later version.
//
//Greta is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with Greta; if not, write to the Free Software
//Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include "init.h"
#include <malloc.h>
#include "FaceData.h"


extern GLfloat init_vdata2[SKIN_VERTEX_NUMBER2][3];
//extern GLfloat init_vdata[SKIN_VERTEX_NUMBER1][3];

//flag per settare il calcolo di tutte le normali ( se posto ad 1)

int all_normals_flag=1;

//functions for setting up the color of some parts of the face
void SetEyesColor(float r,float g,float b)
{
	iris.diffuse[0]=r;
	iris.diffuse[1]=g;
	iris.diffuse[2]=b;
}

void SetLipsColor(float r,float g,float b)
{
	lips.diffuse[0]=r;
	lips.diffuse[1]=g;
	lips.diffuse[2]=b;
}

void SetBrowColor(float r,float g,float b)
{
	eyebrow.diffuse[0]=r;
	eyebrow.diffuse[1]=g;
	eyebrow.diffuse[2]=b;
}

void SetSkinColor(float r,float g,float b)
{
	facial_skin.diffuse[0]=r;
	facial_skin.diffuse[1]=g;
	facial_skin.diffuse[2]=b;
}

//********************************************************************************************************************
//
//                     Initializes the flag_surface1&flag_surface2 arrays
//
//********************************************************************************************************************

//OLDNAME: init_flag_surface
void initialize_array_flag_surface1_flag_surface2(GLint* flag_surface1,GLint* flag_surface2)
{
	int i;

	for (i=0; i<69; i++){
		flag_surface1[i] = all_normals_flag; //0 means that normals are not to be calculated, 1 means yes
	}


	for (i=0; i<12; i++){
		flag_surface2[i] = 0; //the same here
	}

}


#define FLT_EPSILON_X 0.40

#define FLOAT_EQ_X(x,v) (((x-(v))<FLT_EPSILON_X)&&((x-(v))>-FLT_EPSILON_X))

#define FLT_EPSILON_YZ 0.10

#define FLOAT_EQ_YZ(x,v) (((x-(v))<FLT_EPSILON_YZ)&&((x-(v))>-FLT_EPSILON_YZ))



//********************************************************************************************************************
//
//             Function to calculate the normal of a given triangle
//
//********************************************************************************************************************

static __inline void calc_normal ( float *p1, float *p2,  float *p3, float *norm )
{
  float coa, cob, coc ;
  float px1, py1, pz1 ;
  float px2, py2, pz2 ;
  float px3, py3, pz3 ;
  
  float absvec ;
 
#ifdef FACE_POSER
  px3 = p1[0] ;
  py3 = p1[1] ;
  pz3 = p1[2] ;
  
  px2 = p2[0] ;
  py2 = p2[1] ;
  pz2 = p2[2] ;
  
  px1 = p3[0] ;
  py1 = p3[1] ;
  pz1 = p3[2] ;

#else
  px1 = p1[0] ;
  py1 = p1[1] ;
  pz1 = p1[2] ;
  
  px2 = p2[0] ;
  py2 = p2[1] ;
  pz2 = p2[2] ;
  
  px3 = p3[0] ;
  py3 = p3[1] ;
  pz3 = p3[2] ;
#endif

  

  coa = -(py1 * (pz2-pz3) + py2*(pz3-pz1) + py3*(pz1-pz2)) ;
  cob = -(pz1 * (px2-px3) + pz2*(px3-px1) + pz3*(px1-px2)) ;
  coc = -(px1 * (py2-py3) + px2*(py3-py1) + px3*(py1-py2)) ;
  
  absvec = sqrt ((double) ((coa*coa) + (cob*cob) + (coc*coc))) ;
  //absvec = 1.0;	
  
  norm[0] = coa/absvec ;
  norm[1] = cob/absvec ;
  norm[2] = coc/absvec ;
}





//extern GLfloat textures[1230][2];

//extern GLuint texture[3];

typedef struct tagMATRIX										// A Structure To Hold An OpenGL Matrix ( NEW )
{
	float Data[16];												// We Use [16] Due To OpenGL's Matrix Format ( NEW )
}
MATRIX;

typedef struct tagVECTOR										// A Structure To Hold A Single Vector ( NEW )
{
	float X, Y, Z;												// The Components Of The Vector ( NEW )
}
VECTOR;



//********************************************************************************************************************
//
// initializes misc_point, that contains all the points that form surfaces different from
// the skin (for example teeths, eyes, tongue)
//
//********************************************************************************************************************

//OLDNAME:init_misc_members
void initialize_array_misc_point(pointtype* misc_point)
{
	int i, j;

	for (i=0; i<SKIN_VERTEX_NUMBER2; i++){
		for (j=0; j<3; j++){
			misc_point[i].coordinate[j] = 0.0;
			misc_point[i].increment[j] = 0.0;
			misc_point[i].normal[j] = 0.0;
		}
		misc_point[i].surface = 0;
		misc_point[i].distance = -1.0;
	}
}

//********************************************************************************************************************
//
//                          loads vdata2 into misc_point.coordinate
//
//********************************************************************************************************************

//OLDNAME:assign_misc_coordinates
void copy_vdata2_into_misc_coordinate(pointtype* misc_point)
{
	int i,j;
	for (i=0; i<SKIN_VERTEX_NUMBER2; i++){
		for (j=0; j<3; j++){
			misc_point[i].coordinate[j] = vdata2[i][j];
		}
	}
}

//********************************************************************************************************************
//
// allocates and initialize and array of some arrays that contain the arrays of the traingles of each area
//
//********************************************************************************************************************

//OLDNAME:assign_surface_indices
void create_and_initialize_array_misc_surfaces_indices(void){
	int i,j;
	int *temp_indices[12];

	for(i=1; i<12; i++)//index 0 is not used
	{
		if(!(misc_surfaces_indices[i]= (int **)malloc(surface_num_polygons2[i]*sizeof(int *))))
		{
		    printf("ERROR: malloc failed on misc_surfaces_indices[]\n");
		    exit(2);
		}
		for(j=0; j<surface_num_polygons2[i]; j++)
		{
			if(!(misc_surfaces_indices[i][j]= (int *)malloc(3*sizeof(int ))))
			{
				printf("ERROR: malloc failed on misc_surfaces_indices[][]\n");
				exit(2);
			}
		}
	}
	
	//temp_indices[] è una variabile locale 
	temp_indices[0] = 0; //not used
	temp_indices[1] = &l_pupil_indices[0][0];
	temp_indices[2] = &r_pupil_indices[0][0];
	temp_indices[3] = &l_iris_indices[0][0];
	temp_indices[4] = &r_iris_indices[0][0];
	temp_indices[5] = &l_bulb_indices[0][0];
	temp_indices[6] = &r_bulb_indices[0][0];
	temp_indices[7] = &inf_gum_indices[0][0];
	temp_indices[8] = &sup_gum_indices[0][0];
	temp_indices[9] = &inf_teeth_indices[0][0];
	temp_indices[10] = &sup_teeth_indices[0][0];
	temp_indices[11] = &tongue_indices[0][0];

	for(i=1; i<12; i++)
	{
		j=0;
		while(j < surface_num_polygons2[i])
		{
			misc_surfaces_indices[i][j] = temp_indices[i]+3*j;
			j++;
		}
	}
}

//********************************************************************************************************************
//
//             initializes the normal of the points in the misc_point array
//
//********************************************************************************************************************

//OLDNAME:assign_misc_normals
void initialize_misc_normals(pointtype* misc_point)
{
	int i, j;
	float temp_normal[3];
	float *aux_pointer[3];

	//initialize them to zero
	for(i=0; i < SKIN_VERTEX_NUMBER2; i++) {
		for (j=0; j < 3; j++) { 
			misc_point[i].normal[j] = 0.0;
		}				
	}

    //calculates the normals
	for(i=1; i < 12; i++)//for all the surfaces
	{
		j = 0;

		while(j < surface_num_polygons2[i])//for all its polygons
		{
			aux_pointer[0] = &(misc_point[misc_surfaces_indices[i][j][0]].coordinate[0]);
			aux_pointer[1] = &(misc_point[misc_surfaces_indices[i][j][1]].coordinate[0]);
			aux_pointer[2] = &(misc_point[misc_surfaces_indices[i][j][2]].coordinate[0]);

			calc_normal (aux_pointer[0], aux_pointer[1], aux_pointer[2], &temp_normal[0] );	

			misc_point[misc_surfaces_indices[i][j][0]].normal[0] += temp_normal[0];
			misc_point[misc_surfaces_indices[i][j][1]].normal[0] += temp_normal[0];
			misc_point[misc_surfaces_indices[i][j][2]].normal[0] += temp_normal[0];

			misc_point[misc_surfaces_indices[i][j][0]].normal[1] += temp_normal[1];
			misc_point[misc_surfaces_indices[i][j][1]].normal[1] += temp_normal[1];
			misc_point[misc_surfaces_indices[i][j][2]].normal[1] += temp_normal[1];
	
			misc_point[misc_surfaces_indices[i][j][0]].normal[2] += temp_normal[2];
			misc_point[misc_surfaces_indices[i][j][1]].normal[2] += temp_normal[2];
			misc_point[misc_surfaces_indices[i][j][2]].normal[2] += temp_normal[2];

			j++;
		}
	}					
}

//********************************************************************************************************************
//
// initializes the "surface" field of the elemts of misc_point
//
//********************************************************************************************************************

//OLDNAME:assign_misc_surfaces
void initializes_misc_surfaces(int surface_number,pointtype* misc_point)
{
	int i, j, temp;

	i=0;

	while (i < surface_num_polygons2[surface_number])
	{
		for(j=0; j < 3; j++){
			temp = misc_surfaces_indices[surface_number][i][j];
			misc_point[temp].surface = surface_number;
		}
		i++;
	}
}

//********************************************************************************************************************
//
// creates one list for each surface containing the indices of the vertices for that surface
//
//********************************************************************************************************************

//it should be called after all the fields of the misc_point are updated
//each list is terminated by an index "-1"

//OLDNAME:create_surface_list2
void create_misc_surface_list(pointtype* misc_point)
{
	int i, num_points[12];
	int *aux_pointer[12];

	//this array contains the number of point for each surface
	for(i=0; i < 12; i++){
		num_points[i] = 0;
	}

	//for each vertex of the misc_point array
	for(i=0; i < SKIN_VERTEX_NUMBER2; i++){
		num_points[misc_point[i].surface] += 1;//counts the vertex
	}

	for(i=1; i < 12; i++)
	{
		if(!(surface_list2[i] = (int *)malloc((num_points[i]+1)*sizeof(int))))
		{
			printf("ERROR: malloc failed on surface_list2[]\n");
			exit(2);
		}
	}

	//some temporary pointers to move into the lists
	for(i=1; i < 12; i++){
		aux_pointer[i] = surface_list2[i];
	}

	//the lists are filled with the vertices indices
	for(i=8; i < SKIN_VERTEX_NUMBER2; i++)//the 0-7 points contain the coordinated of the cube surrounding the face model
	{
		*(aux_pointer[misc_point[i].surface]++) = i;
		//the instruction corrisponds to the following two intructions
		//*aux_pointer[misc_point[i].surface] = i; //riempio una cella della lista
		//aux_pointer[misc_point[i].surface] += 1; //sposto in avanti il puntatore 
	}

	//adds the "-1" values at the end of each list
	for(i=1; i < 12; i++){ 
		*(aux_pointer[i]) = -1;
	}
}


//*****************************************************************************************************************
//
//						Funzione temporanea che legge l'header di alisha.fap
//
//*****************************************************************************************************************


int header_file_init(char *filename, int *maxframes, int *frate)
{
	char cbuf[256],cdummy[256];
	float version;
	FILE *fapfile;

	//legge l'inizio di alisha.fap
	if(!(fapfile = fopen(&filename[0],"r"))){
		printf("WARNING: can't read %s\n", filename);
		return 0;
	}
	while(1) {//just read header line for mallocs below
		if(fscanf(fapfile,"%s",cbuf));
		else {
		  printf("ERROR: bad fapfile: %s\n",cbuf);
		  return 0;
		}
		if(cbuf[0] == '#') while(getc(fapfile) != '\n');
		else {
		  sscanf(cbuf,"%f ",&version);
		if(fscanf(fapfile,"%s %d %d",cdummy,frate,maxframes));
		else {
		  printf("ERROR: bad fapfile: %s\n",cbuf);
		  return 0;
		}
		if(fabs(version - 2.1) > 0.001) printf("WARNING: Expecting FAP file version 2.1 not %f\n",version);
		while(getc(fapfile) != '\n');
		break;
		}
	}
	fclose(fapfile);
	return 1;
	//fine legge l'inizio di alisha.fap
}

int header_buffer_init(char *buffer, int *maxframes, int *frate)
{
	char cbuf[256],cdummy[256];
	float version;
		
	while(1) {//just read header line for mallocs below
		if(sscanf(buffer,"%s",cbuf));
		else {
		  printf("ERROR: bad fapbuffer: %s\n",cbuf);
		  return 0;
		}
		if(cbuf[0] == '#')
		{}
		else {
		  sscanf(cbuf,"%f ",&version);
		if(sscanf(buffer,"%s %d %d",cdummy,frate,maxframes));
		else {
		  printf("ERROR: bad fapbuffer: %s\n",cbuf);
		  return 0;
		}
		if(fabs(version - 2.1) > 0.001) printf("WARNING: Expecting FAP file version 2.1 not %f\n",version);
		}
		break;
	}
	return 1;
	//fine legge l'inizio di alisha.fap
}



//********************************************************************************************************************
//
//						  calculates the noemal for the points in misc_point during animation
//
//********************************************************************************************************************

//OLDNAME:assign_misc_normals_runtime
void calculate_misc_normals_at_runtime(pointtype* misc_point,GLint* flag_surface2)
{
	int i, j;
//	float vertexdata[SKIN_VERTEX_NUMBER2][3];
	float temp_normal[3];
	float *aux_pointer[3];


/*
	for(i=0; i < SKIN_VERTEX_NUMBER2; i++)
	{
		for(j=0; j < 3; j++)
		{
			vertexdata[i][j] = misc_point[i].coordinate[j]+misc_point[i].increment[j];
		}
	}
*/
	//reset the normals to zero
	for(i=1; i < 12; i++)
	{
		if(flag_surface2[i])
		{
			j = 0;
			while(surface_list2[i][j]!=-1){
				misc_point[surface_list2[i][j]].normal[0] = 0;
				misc_point[surface_list2[i][j]].normal[1] = 0;
				misc_point[surface_list2[i][j]].normal[2] = 0;
				j++;
			}
		}
	}

	//vdata2 contains the updated coordinates values for the points in misc_point
	for(i=1; i < 12; i++)
	{
		if(flag_surface2[i])
		{
			j = 0;
			while(j < surface_num_polygons2[i])
			{
				aux_pointer[0] = &(vdata2[misc_surfaces_indices[i][j][0]][0]);
				aux_pointer[1] = &(vdata2[misc_surfaces_indices[i][j][1]][0]);
				aux_pointer[2] = &(vdata2[misc_surfaces_indices[i][j][2]][0]);
				
				calc_normal (aux_pointer[0], aux_pointer[1], aux_pointer[2], &temp_normal[0] );	

				misc_point[misc_surfaces_indices[i][j][0]].normal[0] += temp_normal[0];
				misc_point[misc_surfaces_indices[i][j][1]].normal[0] += temp_normal[0];
				misc_point[misc_surfaces_indices[i][j][2]].normal[0] += temp_normal[0];

				misc_point[misc_surfaces_indices[i][j][0]].normal[1] += temp_normal[1];
				misc_point[misc_surfaces_indices[i][j][1]].normal[1] += temp_normal[1];
				misc_point[misc_surfaces_indices[i][j][2]].normal[1] += temp_normal[1];
		
				misc_point[misc_surfaces_indices[i][j][0]].normal[2] += temp_normal[2];
				misc_point[misc_surfaces_indices[i][j][1]].normal[2] += temp_normal[2];
				misc_point[misc_surfaces_indices[i][j][2]].normal[2] += temp_normal[2];

				j++;
			}
		}
	}					
}


