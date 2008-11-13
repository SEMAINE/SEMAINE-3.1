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

#ifndef FACEENGINEGLOBALS

#pragma warning(disable : 4244)

#define LOGFILE

//#define OAA //still used?

//#define DEBUG_GAZE

//#define SKIPFRENCH

#define PHONEME_RHEME 1
#define PHONEME_THEME 2
#define PHONEME_NO_RT 0

#define NUM_PAR 12
#define NUM_VOW 13
#define LIP_PAR 7
#define TONGUE_PAR 5 
#define NUM_EMOT 7
#define NUM_POINTS 1

//constants to define minimum and maximum values for onset and offset phases of facial expressions
#define MIN_ONSET 0.4
#define MIN_OFFSET 0.4
#define MIN_HEAD_ONSET 0.5 
#define MIN_HEAD_OFFSET 0.5

#define FRAMERATE 25

#define NEUTRAL -1


//for faps calculation

#define min(a, b)  (((a) < (b)) ? (a) : (b))

#define INT(x) ((int)(x + 0.5))

#define KEY_PARAMS 75

#define MIN_TIME 0.30

#define MIN_HEAD_TIME 0.30

#define MAX_EXP_ATT_TIME 2.0
#define MAX_LIN_ATT_TIME 1.0

#define MAX_NOISE_EYES 3000.0

#define persistence 0.15
#define Number_Of_Octaves 4

#define HEAD_M_NOD 0x0F
#define HEAD_M_SHAKE 0x1F

#define HEADX 69
#define HEADY 70
#define HEADZ 71

#ifndef M_PI
#define M_PI            3.14159265358979323846
#endif

#define EXP_ATT(a,s) min((exp(s)/MAX_EXP_ATT_TIME)*abs(a),abs(a))==abs(a)?a:(exp(s)/MAX_EXP_ATT_TIME)*a
#define NO_ATT(a,s) a
#define LIN_ATT(a,s) min((s/MAX_LIN_ATT_TIME)*abs(a),abs(a))==abs(a)?a:(s/MAX_LIN_ATT_TIME)*a

//gaze
#define _LESS 1
#define _GREATER 0
#define MIN_HEAD_AWAY_TIME 1.5

#define NEUTRAL -1

#define RAISING 0  
#define FROWN 1
#define UP_DOWN 2

#define ASIDE 0
#define LOOK_DOWN 1
#define LOOK_UP 2
#define DEICTIC 3

#define LIP_CORNER_UP 0
#define LIP_CORNER_DOWN 1
#define TENSE 2

#define HEAD_NOTHING -1
#define HEAD_NOD 0
#define HEAD_SHAKE 1

#define LOOK_AWAY 0
#define LOOK_AT 1

#define S1L1 0
#define S0L1 1
#define S1L0 2
#define S0L0 3


//!
//! Structure used for resolving
//! conflicts on facial channels.
//! They correspond to the leaf nodes
//! of belief network './bn/apml_face.net'.
//!
typedef struct {
	//! Possible values: RAISING, FROWN, UP_DOWN 
	int eyebrows; 
	//! Possible values: LOOK_AT, LOOK_AWAY
	int gaze;	  
	//! Possible values: LIP_CORNER_UP, LIP_CORNER_DOWN, TENSE
	int mouth;	  
	//! Possible values: ASIDE, LOOK_DOWN, LOOK_UP
	int head_direction; 
	//! Possible values: HEAD_NOD, HEAD_SHAKE
	int head_movement; 
} channel;


typedef struct{
	//! Text string
	char *s;
	//! Next item in the list
	void* next;
}text_l;

typedef struct{
	//! phoneme name
	char *pho; 
	//! phoneme duration
	float dur; 
	//! next phoneme
	void* next; 
}phoneme;

//!
//! Struct for building keyframes list
//!
typedef struct
{
	//! keyframe number
	int keyframenumber; 
	//! flag (0/1) to indicate if 'keyframe' is a neutral keyframe (neutral face)
	int neutral;
	//! next keyframe
	void* next; 
	//! previous keyframe
	void* prev; 
}keyframe;

typedef struct
{
	float start_time;
	float end_time;
	int fapmask[69];
	int faps[69];
}ExprAni;

//! 
//! Structure of a Facial Display (FD) used 
//! for building  our Facial Display Dictionary 
//! by parsing the './fd/apml_facelibrary.txt' file.
//!

typedef struct{
	//! Facial Display class (affective, performative, etc etc)
	char class_type[256]; 
	//! Meaning of this facial display (example for affective 'joy')
	char id[256]; 
	//! Faps values for this facial display in the form of 2x69 matrix
	//! where faps[0][i]) is the fapmask for the fap 'i' and
	//! faps[1][i] is the value for the fap 'i'
	int **faps;
	//! Probability of this facial display
	float prob;
	//! head movement type (HEAD_M_NOD or HEAD_M_SHAKE)
	int head_m_type;
	//! head movement amplitude (in degree)
	float head_m_amplitude;
	//! head movement period (in seconds)
	float head_m_period;
	//! Facial channels used in this facial display
	channel* fs;
	//! Next facial display in our Facial Display Dictionary
	void *next;
}fap_id_list;

#define FACEENGINEGLOBALS
#endif