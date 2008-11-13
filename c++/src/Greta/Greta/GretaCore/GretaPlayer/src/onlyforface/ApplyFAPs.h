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

#pragma once
#ifndef COMPILED_APPLYFAPS
#define COMPILED_APPLYFAPS

#include "FaceData.h"
#include "FAPData.h"

#define MIN(A,B) ((A) < (B) ? (A) : (B))
#define MAX(A,B) ((A) > (B) ? (A) : (B))

#define RAC_2		1.414

class ApplyFAPs;

class ApplyFAPs
{
public:
	FaceData *facedata;
	void *agent;
	FAPdata *FAPs;
	FDPdata *FDPs;
	FAPUdata *FAPUs;
	int *frame_count;
	pointtype* skin_point;
	pointtype* misc_point;
	GLint* flag_surface1;
	GLint* flag_surface2;
	GLint** surface_list1;
	int flag_ifapu;

	ApplyFAPs(void);
	~ApplyFAPs(void);

public:
	void init(void* ag);
	void apply_faps();

	float COEF(float fapuModel, float ifapuModel, float fapuGreta, float ifapuGreta);
	void apply_fap_31_32();
	void apply_fap_33_34();
	void apply_fap_35_36();
	void apply_aux_fap_1();
	void forehead_bump();
	void apply_fap_53_54_59_60();
	void apply_fap_6_7_12_13();
	void apply_fap_55_56();
	void apply_fap_8_9();
	void apply_fap_51_17();
	void apply_fap_4_17();
	void apply_fap_57_58();
	void apply_fap_10_11();
	void apply_fap_52_16();
	void apply_fap_5_16();
	void apply_fap_3_14_15();
	void apply_fap_18();
	void apply_fap_39_40();
	void apply_fap_41_42();
	void cheek_deformation();
	void apply_aux_fap_2();
	void apply_fap_61_62();
	void apply_fap_63_64();
	void apply_fap_65_66();
	void apply_fap_67_68();
	void apply_fap_19_20();
	void apply_fap_21_22();
	void apply_fap_29_30();
	void apply_fap_43_44();
	void apply_fap_45();
	void apply_fap_46();
	void apply_fap_47();
	void prot(float val, int opt);
	void prot2(float val, int opt);
	void nouse();
	void press81(float val);
	void press85(float val);
	void press86(float val);
};

#endif	// COMPILED_APPLYFAPS