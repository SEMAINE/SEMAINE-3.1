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

#ifndef INCUDED_PLAYEGLOBALS
#define INCUDED_PLAYEGLOBALS

#define EYELID_MAKEUP

#define SKIN_VERTEX_NUMBER2 3182 //pti di vdata2 fissato il giorno 14/02/00 da polygons.h

#pragma warning( disable : 4305 )  //disabilita il warning sul troncamento del double in float
//#pragma warning( disable : 4024 )  //da riabilitare in seguito perché questi warning servono
//#pragma warning( disable : 4047 )  //da riabilitare in seguito perché questi warning servono

#ifndef M_PI
#define M_PI 3.14159265 
#endif

//********************************************************************************************************************
//
//                             Definiamo le macro 
//
//********************************************************************************************************************
/*

//Definisco le FAPU, fissate il giorno 14/02/00
#define MWFAPU0 12.3
#define MNSFAPU0 4.9
#define ENSFAPU0 10.1
#define ESFAPU0 15.0
#define IRISDFAPU0 2.4
//Fine definizione FAPU 


//Definisco le superfici: ad ogni nome convenzionale faccio corrispondere un intero
#define L_CHEECK         1
#define R_CHEECK         2
#define L_NOSE           3
#define R_NOSE           4
#define L_INF_EYELID_A   5
#define R_INF_EYELID_A   6
#define L_INF_EYELID_B   7
#define R_INF_EYELID_B   8
#define L_SUP_EYELID_A   9
#define R_SUP_EYELID_A   10
#define L_SUP_EYELID_B   11
#define R_SUP_EYELID_B   12
#define L_EYEBROW_A      13
#define R_EYEBROW_A      14
#define L_EYEBROW_B      15
#define R_EYEBROW_B      16
#define L_EYEBROW_C      17
#define R_EYEBROW_C      18
#define L_INF_FOREHEAD_A 19
#define R_INF_FOREHEAD_A 20
#define L_INF_FOREHEAD_B 21
#define R_INF_FOREHEAD_B 22
#define L_INF_FOREHEAD_C 23
#define R_INF_FOREHEAD_C 24
#define L_INF_FOREHEAD_D 25
#define R_INF_FOREHEAD_D 26
#define L_INF_FOREHEAD_E 27
#define R_INF_FOREHEAD_E 28
#define L_INF_FOREHEAD_F 29
#define R_INF_FOREHEAD_F 30
#define L_SUP_FOREHEAD   31
#define R_SUP_FOREHEAD   32
#define L_SUP_LIP_A      33
#define R_SUP_LIP_A      34
#define L_SUP_LIP_B      35
#define R_SUP_LIP_B      36
#define L_SUP_LIP_C      37
#define R_SUP_LIP_C      38
#define L_INF_LIP_A      39
#define R_INF_LIP_A      40
#define L_INF_LIP_B      41
#define R_INF_LIP_B      42
#define L_INF_LIP_C      43
#define R_INF_LIP_C      44
#define L_SUP_MOUTH_A    45
#define R_SUP_MOUTH_A    46
#define L_SUP_MOUTH_B    47
#define R_SUP_MOUTH_B    48
#define L_SUP_MOUTH_C    49
#define R_SUP_MOUTH_C    50
#define L_MOUTH          51
#define R_MOUTH          52
#define L_INF_MOUTH_A    53
#define R_INF_MOUTH_A    54
#define L_INF_MOUTH_B    55
#define R_INF_MOUTH_B    56
#define L_INF_MOUTH_C    57
#define R_INF_MOUTH_C    58
#define L_CHIN           59
#define R_CHIN           60
#define L_EAR            61
#define R_EAR            62
#define L_SKIN_A         63
#define R_SKIN_A         64
#define L_SKIN_B         65
#define R_SKIN_B         66
#define L_SKIN_C         67
#define R_SKIN_C         68
//Fine definizione superfici


//Definisco le superfici: ad ogni nome convenzionale faccio corrispondere un intero
#define L_PUPIL    1
#define R_PUPIL    2
#define L_IRIS     3
#define R_IRIS     4
#define L_BULB     5
#define R_BULB     6
#define INF_GUM    7
#define SUP_GUM    8
#define INF_TEETH  9
#define SUP_TEETH 10
#define TONGUE    11
//Fine definizione superfici


*/

//Numero dei pti che costituiscono la ruga
#define WRIN_VERTEX_NUMBER 56 

//Numero dei pti delle palpebre
#define SUP_EYELID_VERTEX_NUMBER 14
#define INF_EYELID_VERTEX_NUMBER 11

typedef struct {
   char *name;
   float ambient[4];
   float diffuse[4];
   float specular[4];
   float emission[4];
   float shininess[1];
} Material;


#endif