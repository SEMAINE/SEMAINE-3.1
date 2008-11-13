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

#ifndef INCLUDED_INIT
#define INCLUDED_INIT

#include "../playerglobals.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>       
#include <gl/glut.h>
#include <gl/glaux.h>

//********************************************************************************************************************
//
//                             Definiamo i tipi 
//
//********************************************************************************************************************


//structures that have to contain FDPs and FAPs
/*
typedef struct {
//	int **isfeatpnt;//[16][20] set if featpnt read from fdp file
	float ***featpts;//[16][20][5] group.num.dimension (including u,v)
	//float MWFAPU,MNSFAPU,ENSFAPU,ESFAPU,IRISDFAPU;
	float dispscale;
	float version;
} FDPtype;
*/

//structure that has to contain point properties
typedef struct {
	float coordinate[3]; // contain the original point's coordinates
	float increment[3]; // contain the point's increment that has to be computed
	float normal[3]; //contain the normal vector of the point
	int surface; //contain information about the point's surface (integer: 1-68)
	float distance; //contain special distance data 
	//int simmetric; //contiene l'indice del punto simmetrico rispetto all'asse y
	float color[3]; //contiene il colore del pto
} pointtype;
//End of structure that has to contain point properties


//structures that have to contain lights and materials
typedef struct {
   float *position;
   float *ambient;
   float *diffuse;
   float *specular;
} LightSource;

//End structures that have to contain lights and materials




//********************************************************************************************************************
//
//      Definiamo le variabili globali (attenzione!!! le dichiariamo con "extern", dopo dovrò "definirle") 
//
//********************************************************************************************************************


//Dichiariamo le FDPs & FAPs che verranno definite in polygon.h
//extern FDPtype FDPs1;
//extern FAPtype FAPs1;
//extern FDPtype FDPs2;
//extern FAPtype FAPs2;
//fine dichiarazione FDPs & FAPs


//Dichiaro la variabile che contiene i pti della pelle della faccia
//extern pointtype skin_point[SKIN_VERTEX_NUMBER1];
//Dichiaro la variabile che contiene i pti degli occhi, denti, etc.
//extern pointtype misc_point[SKIN_VERTEX_NUMBER2];


// variabile che indica il numero dei frame della mia animazione, da definire in "facial.c"
//extern int maxframes;
// variabile che indica il framerate
//extern int frate;

/*
//variabile che contiene i dati sui vertici, questi ultimi vengono caricati in skin_point[].coordinate[]
//extern GLfloat vdata[SKIN_VERTEX_NUMBER1][3];
*/
extern GLfloat vdata2[SKIN_VERTEX_NUMBER2][3];

//variabile che contiene i punti della ruga alla guancia sinistra che saranno utilizzati per calcolare skin_point[].distance
extern GLfloat vdata_l_wrin[WRIN_VERTEX_NUMBER][3];

//variabili che contengono i punti delle palpebre che servono per calcolare le distanze 
extern GLfloat vdata_l_sup_eyelid[SUP_EYELID_VERTEX_NUMBER][3];
extern GLfloat vdata_l_inf_eyelid[INF_EYELID_VERTEX_NUMBER][3];

//dischiaro le variabili degli indici delle superfici della pelle (chissà se va bene tenere i vettori senza dimensione)
/*
extern GLint l_cheeck_indices[][3]; //1
extern GLint r_cheeck_indices[][3]; //2
extern GLint l_nose_indices[][3]; //3
extern GLint r_nose_indices[][3]; //4
extern GLint l_inf_eyelid_a_indices[][3]; //5
extern GLint r_inf_eyelid_a_indices[][3]; //6
extern GLint l_inf_eyelid_b_indices[][3]; //7 
extern GLint r_inf_eyelid_b_indices[][3]; //8
extern GLint l_sup_eyelid_a_indices[][3]; //9
extern GLint r_sup_eyelid_a_indices[][3]; //10
extern GLint l_sup_eyelid_b_indices[][3]; //11
extern GLint r_sup_eyelid_b_indices[][3]; //12
extern GLint l_eyebrow_a_indices[][3]; //13
extern GLint r_eyebrow_a_indices[][3]; //14
extern GLint l_eyebrow_b_indices[][3]; //15
extern GLint r_eyebrow_b_indices[][3]; //16
extern GLint l_eyebrow_c_indices[][3]; //17
extern GLint r_eyebrow_c_indices[][3]; //18
extern GLint l_inf_forehead_a_indices[][3]; //19
extern GLint r_inf_forehead_a_indices[][3]; //20
extern GLint l_inf_forehead_b_indices[][3]; //21
extern GLint r_inf_forehead_b_indices[][3]; //22
extern GLint l_inf_forehead_c_indices[][3]; //23
extern GLint r_inf_forehead_c_indices[][3]; //24
extern GLint l_inf_forehead_d_indices[][3]; //25
extern GLint r_inf_forehead_d_indices[][3]; //26
extern GLint l_inf_forehead_e_indices[][3]; //27
extern GLint r_inf_forehead_e_indices[][3]; //28
extern GLint l_inf_forehead_f_indices[][3]; //29
extern GLint r_inf_forehead_f_indices[][3]; //30
extern GLint l_sup_forehead_indices[][3]; //31
extern GLint r_sup_forehead_indices[][3]; //32
extern GLint l_sup_lip_a_indices[][3]; //33
extern GLint r_sup_lip_a_indices[][3]; //34
extern GLint l_sup_lip_b_indices[][3]; //35
extern GLint r_sup_lip_b_indices[][3]; //36
extern GLint l_sup_lip_c_indices[][3]; //37
extern GLint r_sup_lip_c_indices[][3]; //38
extern GLint l_inf_lip_a_indices[][3]; //39
extern GLint r_inf_lip_a_indices[][3]; //40
extern GLint l_inf_lip_b_indices[][3]; //41
extern GLint r_inf_lip_b_indices[][3]; //42
extern GLint l_inf_lip_c_indices[][3]; //43
extern GLint r_inf_lip_c_indices[][3]; //44
extern GLint l_sup_mouth_a_indices[][3]; //45
extern GLint r_sup_mouth_a_indices[][3]; //46
extern GLint l_sup_mouth_b_indices[][3]; //47
extern GLint r_sup_mouth_b_indices[][3]; //48
extern GLint l_sup_mouth_c_indices[][3]; //49
extern GLint r_sup_mouth_c_indices[][3]; //50
extern GLint l_mouth_indices[][3]; //51
extern GLint r_mouth_indices[][3]; //52
extern GLint l_inf_mouth_a_indices[][3]; //53
extern GLint r_inf_mouth_a_indices[][3]; //54
extern GLint l_inf_mouth_b_indices[][3]; //55
extern GLint r_inf_mouth_b_indices[][3]; //56
extern GLint l_inf_mouth_c_indices[][3]; //57
extern GLint r_inf_mouth_c_indices[][3]; //58
extern GLint l_chin_indices[][3]; //59
extern GLint r_chin_indices[][3]; //60
extern GLint l_ear_indices[][3]; //61
extern GLint r_ear_indices[][3]; //62
extern GLint l_skin_a_indices[][3]; //63
extern GLint r_skin_a_indices[][3]; //64
extern GLint l_skin_b_indices[][3]; //65
extern GLint r_skin_b_indices[][3]; //66
extern GLint l_skin_c_indices[][3]; //67
extern GLint r_skin_c_indices[][3]; //68
*/
//dischiaro le variabili degli indici delle superfici dei degli occhi denti etc.
extern GLint l_pupil_indices[46][3]; //1
extern GLint r_pupil_indices[46][3]; //2
extern GLint l_iris_indices[70][3]; //3
extern GLint r_iris_indices[70][3]; //4
extern GLint l_bulb_indices[240][3]; //5
extern GLint r_bulb_indices[240][3]; //6
extern GLint inf_gum_indices[1132][3]; //7
extern GLint sup_gum_indices[1016][3]; //8
extern GLint inf_teeth_indices[1196][3]; //9
extern GLint sup_teeth_indices[1142][3]; //10
extern GLint tongue_indices[356][3]; //11

//dichiariamo un array di puntatori agli indici delle superfici della pelle del volto
//verrà definito in facial.c e i puntatori saranno assegnati da una funzione in tools.c
//extern GLint **skin_surfaces_indices[69]; //il termine surface_indices[0] non punterà a nulla, però gli altri punteranno ad altri vettori di interi bidimensionali
extern GLint **misc_surfaces_indices[12]; //questo è analogo ma serve per misc_point

//in questo vettore è scritto il numero dei poligoni contenuti in ogni superficie
//verrà definito ed inizializzato in polygons.h che a sua volta sarà incluso in facial.c
//extern GLint surface_num_polygons1[69];
extern GLint surface_num_polygons2[12]; //per misc_point

//questo è un vettore di cui ogni elemento è un vettore che punta ad una lista di interi
// che contiene gli indici dei punti che appartengono alle relative superfici,
//verrà definito in facial.c, le liste saranno allocate in void create_surface_list(int surface_number)

extern GLint *surface_list2[12]; //per misc_point

//questo è un vettore di flag che indica se i punti della superficie sono stati modificati oppure no,
//viene inizializzato a 0, quando interviene una FAP setta ad 1 tutti flag delle superfici su cui interviene;
//va definito in polygons.h
extern GLint flag_surface1[69];
extern GLint flag_surface2[12]; //per misc_point



//Questi struct contengono i vari materiali
extern Material facial_skin;

//per contare i frame
extern int frame_count;

//Questi struct contengono le definizioni dei vari materiali
extern Material facial_skin;
extern Material eyebrow;
extern Material eye_bulb;
extern Material eye_highlight;
extern Material iris;
extern Material pupil;
extern Material lips;
extern Material teeth;
extern Material gum;
extern Material tongue_mat;
extern Material hair;

#endif










