#ifndef _MPEG4ELEMENTS_H
#define _MPEG4ELEMENTS_H

// Objects
class SelectedFDP
{
	// This class allows us to select one of the 'n' vertices
	//	by switching the one with the following one
	// In practice, n=3
public:
	SelectedFDP(int v0, int v1, int v2);
	virtual ~SelectedFDP();

public:
	int pointer;
	int* vertices;
	int next();
};

// Functions
int sizeOfGroup(int group);
int groupOfRegion(int region);
void init_fdps();
int fdp_value(int region, int indice);
char** getFDPs(int group);
char* getName(int fdp);
char* nameOfFAP(int fap_value);

/// FAPUs
/*
#define getMWFAPU0			// 6.3 & 6.4								// selon l'axe horizontal
#define getMNSFAPU0			// 2.2 & 9.15								// selon l'axe vertical
#define getENSFAPU0			// 9.15 & (9.6 ou 9.7 ou 3.5 ou 3.6)		// selon l'axe vertical
#define getESFAPU0			// 3.5 & 3.6								// selon l'axe horizontal
#define getIRISDFAPU0		// (3.1 & 3.3) ou (3.2 & 3.4)				// selon l'axe vertical
*/

// Constantes

/////////////
///////////// REGIONS
/////////////

/// Regions
#define REGION			 68
#define REGION_NUMBER	 (REGION+1)	// FAP 0 is not used

/// Regions indices
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
// The corresponding array
static int region_indices[REGION_NUMBER]= {
	 -1,
	 L_CHEECK,
	 R_CHEECK,
	 L_NOSE,
	 R_NOSE,
	 L_INF_EYELID_A,
	 R_INF_EYELID_A,
	 L_INF_EYELID_B,
	 R_INF_EYELID_B,
	 L_SUP_EYELID_A,
	 R_SUP_EYELID_A,
	 L_SUP_EYELID_B,
	 R_SUP_EYELID_B,
	 L_EYEBROW_A,
	 R_EYEBROW_A,
	 L_EYEBROW_B,
	 R_EYEBROW_B,
	 L_EYEBROW_C,
	 R_EYEBROW_C,
	 L_INF_FOREHEAD_A,
	 R_INF_FOREHEAD_A,
	 L_INF_FOREHEAD_B,
	 R_INF_FOREHEAD_B,
	 L_INF_FOREHEAD_C,
	 R_INF_FOREHEAD_C,
	 L_INF_FOREHEAD_D,
	 R_INF_FOREHEAD_D,
	 L_INF_FOREHEAD_E,
	 R_INF_FOREHEAD_E,
	 L_INF_FOREHEAD_F,
	 R_INF_FOREHEAD_F,
	 L_SUP_FOREHEAD,
	 R_SUP_FOREHEAD,
	 L_SUP_LIP_A,
	 R_SUP_LIP_A,
	 L_SUP_LIP_B,
	 R_SUP_LIP_B,
	 L_SUP_LIP_C,
	 R_SUP_LIP_C,
	 L_INF_LIP_A,
	 R_INF_LIP_A,
	 L_INF_LIP_B,
	 R_INF_LIP_B,
	 L_INF_LIP_C,
	 R_INF_LIP_C,
	 L_SUP_MOUTH_A,
	 R_SUP_MOUTH_A,
	 L_SUP_MOUTH_B,
	 R_SUP_MOUTH_B,
	 L_SUP_MOUTH_C,
	 R_SUP_MOUTH_C,
	 L_MOUTH,
	 R_MOUTH,
	 L_INF_MOUTH_A,
	 R_INF_MOUTH_A,
	 L_INF_MOUTH_B,
	 R_INF_MOUTH_B,
	 L_INF_MOUTH_C,
	 R_INF_MOUTH_C,
	 L_CHIN,
	 R_CHIN,
	 L_EAR,
	 R_EAR,
	 L_SKIN_A,
	 R_SKIN_A,
	 L_SKIN_B,
	 R_SKIN_B,
	 L_SKIN_C,
	 R_SKIN_C
};

// Give the name of each region
static char* region_names[REGION_NUMBER]= {
	"skin",						// 0
	"l_cheeck_indices",			// 1
	"r_cheeck_indices",			// 2
	"l_nose_indices",			// 3
	"r_nose_indices",			// 4
	"l_inf_eyelid_a_indices",	// 5
	"r_inf_eyelid_a_indices",	// 6
	"l_inf_eyelid_b_indices",	// 7
	"r_inf_eyelid_b_indices",	// 8
	"l_sup_eyelid_a_indices",	// 9
	"r_sup_eyelid_a_indices",	// 10
	"l_sup_eyelid_b_indices",	// 11
	"r_sup_eyelid_b_indices",	// 12
	"l_eyebrow_a_indices",		// 13
	"r_eyebrow_a_indices",		// 14
	"l_eyebrow_b_indices",		// 15
	"r_eyebrow_b_indices",		// 16
	"l_eyebrow_c_indices",		// 17
	"r_eyebrow_c_indices",		// 18
	"l_inf_forehead_a_indices",	// 19
	"r_inf_forehead_a_indices",	// 20
	"l_inf_forehead_b_indices",	// 21
	"r_inf_forehead_b_indices",	// 22
	"l_inf_forehead_c_indices",	// 23
	"r_inf_forehead_c_indices",	// 24
	"l_inf_forehead_d_indices",	// 25
	"r_inf_forehead_d_indices",	// 26
	"l_inf_forehead_e_indices",	// 27
	"r_inf_forehead_e_indices",	// 28
	"l_inf_forehead_f_indices",	// 29
	"r_inf_forehead_f_indices",	// 30
	"l_sup_forehead_indices",	// 31
	"r_sup_forehead_indices",	// 32
	"l_sup_lip_a_indices",		// 33
	"r_sup_lip_a_indices",		// 34
	"l_sup_lip_b_indices",		// 35
	"r_sup_lip_b_indices",		// 36
	"l_sup_lip_c_indices",		// 37
	"r_sup_lip_c_indices",		// 38
	"l_inf_lip_a_indices",		// 39
	"r_inf_lip_a_indices",		// 40
	"l_inf_lip_b_indices",		// 41
	"r_inf_lip_b_indices",		// 42
	"l_inf_lip_c_indices",		// 43
	"r_inf_lip_c_indices",		// 44
	"l_sup_mouth_a_indices",	// 45
	"r_sup_mouth_a_indices",	// 46
	"l_sup_mouth_b_indices",	// 47
	"r_sup_mouth_b_indices",	// 48
	"l_sup_mouth_c_indices",	// 49
	"r_sup_mouth_c_indices",	// 50
	"l_mouth_indices",			// 51
	"r_mouth_indices",			// 52
	"l_inf_mouth_a_indices",	// 53
	"r_inf_mouth_a_indices",	// 54
	"l_inf_mouth_b_indices",	// 55
	"r_inf_mouth_b_indices",	// 56
	"l_inf_mouth_c_indices",	// 57
	"r_inf_mouth_c_indices",	// 58
	"l_chin_indices",			// 59
	"r_chin_indices",			// 60
	"l_ear_indices",			// 61
	"r_ear_indices",			// 62
	"l_skin_a_indices",			// 63
	"r_skin_a_indices",			// 64
	"l_skin_b_indices",			// 65
	"r_skin_b_indices",			// 66
	"l_skin_c_indices",			// 67
	"r_skin_c_indices"			// 68
};

/////////////
///////////// Groups
/////////////

/// FAP groups
#define GROUP				11
#define GROUP_NUMBER		(GROUP+1)

// groups 0, 1 not used
#define GROUP_2		2
#define GROUP_3		3
#define GROUP_4		4
#define GROUP_5		5
#define GROUP_6		6
#define GROUP_7		7
#define GROUP_8		8
#define GROUP_9		9
#define GROUP_10	10
#define GROUP_11	11

// Nombre de FAPs pour chaque groupe	// 66
/*
#define JAW_FAPSIZE					16
#define EYE_FAPSIZE					12
#define EYEBROW_FAPSIZE				8
#define CHEEKS_FAPSIZE				4
#define TONGUE_FAPSIZE				5
#define HEAD_ROTATION_FAPSIZE		3
#define OUTER_LIP_POSITIONS_FAPSIZE	10
#define NOSE_FAPSIZE				4
#define EARS_FAPSIZE				4
*/
// Nombre de FDPs pour chaque groupe	// 84
#define GROUP_2_SIZE	14		// Jaw, chin, inner lowerlip, cornerlips, midlip
#define GROUP_3_SIZE	14		// Eyeballs, pupils, eyelids
#define GROUP_4_SIZE	6		// Eyebrow
#define GROUP_5_SIZE	4		// Cheeks
#define GROUP_6_SIZE	4		// Tongue
#define GROUP_7_SIZE	1		// Head rotation
#define GROUP_8_SIZE	10		// Outer-lip positions
#define GROUP_9_SIZE	15		// Nose
#define GROUP_10_SIZE	10		// Ears
#define GROUP_11_SIZE	6		// 

// Group names
static char* group_name[GROUP_NUMBER]= {
	"no name",
	"no name",
	"Jaw",
	"Eye",
	"Eyebrow",
	"Cheeks",
	"Tongue",
	"Head rotation",
	"Outer lip",
	"Nose",
	"Ears"
};

/////////////
///////////// FDPs
/////////////

// Nb: to index a FDP, we need two informations:
//	- its region
//	- its indice value

/// FDP
#define FDP					(GROUP_2_SIZE  + GROUP_3_SIZE + GROUP_4_SIZE  + GROUP_5_SIZE + GROUP_6_SIZE  + GROUP_7_SIZE + GROUP_8_SIZE  + GROUP_9_SIZE + GROUP_10_SIZE + GROUP_11_SIZE)
#define FDP_NUMBER			(FDP+1)

static char** fdp_names[GROUP_NUMBER];
#define MAX_LENGTH_OF_NAME	5	// Example: "10.11"
#define MAX_ITEMS			20	// Le nombre maximum de FDPs pour un groupe

// init_fdps
#endif	// _MPEG4ELEMENTS_H