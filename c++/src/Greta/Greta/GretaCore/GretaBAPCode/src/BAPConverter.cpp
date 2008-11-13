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

// BAPConverter.cpp: implementation of the BAPConverter class.
//
//////////////////////////////////////////////////////////////////////
//include "stdafx.h"
#include "BAPConverter.h"


// simplified spine DOF mapping acc. to SanJose MPEG4 doc:
// L5 -> VL1 (tilt)
// L3 -> VL2 (roll,tilt)
// L1 -> VL3 (roll,tilt)
// T10-> VT4 (roll,torsion,tilt)
// T6 -> VT5 (roll,torsion)
// T1 -> VT6 (roll,torsion,tilt)
// C4 -> VC7 (roll,torsion,tilt)
// C2 -> VC8 (roll,torsion,tilt)

// spine tilt => rotateX
// spine torsion => rotateY
// spine roll => rotateZ

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BAPConverter::BAPConverter()
{
// LIST OF BAP numbers and respective names;
// 06.13.2001;
// added simple spine mapping comments 06 feb 2004
#ifdef USEOLDBAPS

	a[0]=   null_bap;
	a[1]=	Pelvic_tilt ;
	a[2]=	Pelvic_torsion;
	a[3]=	Pelvic_roll ;
	a[4]=	l_hip_flexion ;
	a[5]=	r_hip_flexion ;
	a[6]=	l_hip_abduct  ;
	a[7]=	r_hip_abduct  ;
	a[8]=	l_hip_twisting ;
	a[9]=	r_hip_twisting ;
	a[10]=	l_knee_flexion ;
	a[11]=	r_knee_flexion ;
	a[12]=	l_knee_twisting ;
	a[13]=	r_knee_twisting ;
	a[14]=	l_ankle_flexion ;
	a[15]=	r_ankle_flexion ;
	a[16]=	l_ankle_twisting;
	a[17]=	r_ankle_twisting;
	a[18]=	l_subtalar_flexion ;
	a[19]=	r_subtalar_flexion ;
	a[20]=	l_midtarsal_twisting ;
	a[21]=	r_midtarsal_twisting ;
	a[22]=	l_metatarsal_flexion ;
	a[23]=	r_metatarsal_flexion ;
	a[24]=	l_clav_abduct ;
	a[25]=	r_clav_abduct ;
	a[26]=	l_clav_rotate ;
	a[27]=	r_clav_rotate ;
	a[28]=	l_scap_abduct ;
	a[29]=	r_scap_abduct ;
	a[30]=	l_scap_rotate ;
	a[31]=	r_scap_rotate ;
	a[32]=	l_shoulder_flexion ;
	a[33]=	r_shoulder_flexion ;
	a[34]=	l_shoulder_abduct ;
	a[35]=	r_shoulder_abduct ;
	a[36]=	l_shoulder_twisting ;
	a[37]=	r_shoulder_twisting ;
	a[38]=	l_elbow_flexion ;
	a[39]=	r_elbow_flexion ;
	a[40]=	l_elbow_twisting ;
	a[41]=	r_elbow_twisting ;
	a[42]=	l_wrist_flexion ;
	a[43]=	r_wrist_flexion ;
	a[44]=	l_wrist_pivot  ;
	a[45]=	r_wrist_pivot  ;
	a[46]=	l_wrist_twisting;
	a[47]=	r_wrist_twisting;
	a[48]=	l_pinky_flexion1;
	a[49]=	r_pinky_flexion1;
	a[50]=	l_pinky_abduct;
	a[51]=	r_pinky_abduct;
	a[52]=	l_pinky_twisting;
	a[53]=	r_pinky_twisting;
	a[54]=	l_pinky_flexion2;
	a[55]=	r_pinky_flexion2;
	a[56]=	l_pinky_flexion3;
	a[57]=	r_pinky_flexion3;
	a[58]=	l_ring_flexion1;
	a[59]=	r_ring_flexion1;
	a[60]=	l_ring_abduct;
	a[61]=	r_ring_abduct;
	a[62]=	l_ring_twisting;
	a[63]=	r_ring_twisting;
	a[64]=	l_ring_flexion2;
	a[65]=	r_ring_flexion2;
	a[66]=	l_ring_flexion3;
	a[67]=	r_ring_flexion3;
	a[68]=	l_middle_flexion1;
	a[69]=	r_middle_flexion1;
	a[70]=	l_middle_abduct;
	a[71]=	r_middle_abduct;
	a[72]=	l_middle_twisting;
	a[73]=	r_middle_twisting;
	a[74]=	l_middle_flexion2;
	a[75]=	r_middle_flexion2;
	a[76]=	l_middle_flexion3;
	a[77]=	r_middle_flexion3;
	a[78]=	l_index_flexion1;
	a[79]=	r_index_flexion1;
	a[80]=	l_index_abduct;
	a[81]=	r_index_abduct;
	a[82]=	l_index_twisting;
	a[83]=	r_index_twisting;
	a[84]=	l_index_flexion2;
	a[85]=	r_index_flexion2;
	a[86]=	l_index_flexion3;
	a[87]=	r_index_flexion3;
	a[88]=	l_thumb_flexion1;
	a[89]=	r_thumb_flexion1;
	a[90]=	l_thumb_abduct;
	a[91]=	r_thumb_abduct;
	a[92]=	l_thumb_twisting;
	a[93]=	r_thumb_twisting;
	a[94]=	l_thumb_flexion2;
	a[95]=	r_thumb_flexion2;
	a[96]=	l_thumb_flexion3;
	a[97]=	r_thumb_flexion3;
	a[98]=	c1roll;
	a[99]=	c1torsion;
	a[100]=	c1tilt;
	a[101]=	c2roll;		//simple spine DOF vc8_roll	
	a[102]=	c2torsion;  //simple spine DOF vc8_torsion
	a[103]=	c2tilt;		//simple spine DOF vc8_tilt
	a[104]=	c3roll;
	a[105]=	c3torsion;
	a[106]=	c3tilt;
	a[107]=	c4roll;		//simple spine DOF vc7_roll
	a[108]=	c4torsion;	//simple spine DOF vc7_torsion
	a[109]=	c4tilt;		//simple spine DOF vc7_tilt
	a[110]=	c5roll;
	a[111]=	c5torsion;
	a[112]=	c5tilt;
	a[113]=	c6roll;
	a[114]=	c6torsion;
	a[115]=	c6tilt;
	a[116]=	c7roll;
	a[117]=	c7torsion;
	a[118]=	c7tilt;
	a[119]=	t1roll;		//simple spine DOF vt6_roll
	a[120]=	t1torsion;	//simple spine DOF vt6_torsion
	a[121]=	t1tilt;		//simple spine DOF vt6_tilt
	a[122]=	t2roll;
	a[123]=	t2torsion;
	a[124]=	t2tilt;
	a[125]=	t3roll;		
	a[126]=	t3torsion;
	a[127]=	t3tilt;
	a[128]=	t4roll;		
	a[129]=	t4torsion;	
	a[130]=	t4tilt;		
	a[131]=	t5roll;		
	a[132]=	t5torsion;	
	a[133]=	t5tilt;
	a[134]=	t6roll;		//simple spine DOF vt5_roll		
	a[135]=	t6torsion;	//simple spine DOF vt5_torsion
	a[136]=	t6tilt;		
	a[137]=	t7roll;
	a[138]=	t7torsion;
	a[139]=	t7tilt;
	a[140]=	t8roll;
	a[141]=	t8torsion;
	a[142]=	t8tilt;
	a[143]=	t9roll;
	a[144]=	t9torsion;
	a[145]=	t9tilt;
	a[146]=	t10roll;	//simple spine DOF vt4_roll	
	a[147]=	t10torsion; //simple spine DOF vt4_torsion
	a[148]=	t10tilt;	//simple spine DOF vt4_tilt
	a[149]=	t11roll;
	a[150]=	t11torsion;
	a[151]=	t11tilt;
	a[152]=	t12roll;
	a[153]=	t12torsion;
	a[154]=	t12tilt;
	a[155]=	l1roll;		//simple spine DOF vl3_roll	
	a[156]=	l1torsion;
	a[157]=	l1tilt;		//simple spine DOF vl3_tilt	
	a[158]=	l2roll;		
	a[159]=	l2torsion;
	a[160]=	l2tilt;		
	a[161]=	l3roll;		//simple spine DOF vl2_roll
	a[162]=	l3torsion;
	a[163]=	l3tilt;		//simple spine DOF vl2_tilt	
	a[164]=	l4roll;		
	a[165]=	l4torsion;
	a[166]=	l4tilt;
	a[167]=	l5roll;
	a[168]=	l5torsion;	
	a[169]=	l5tilt;		//simple spine DOF vl1_tilt
#else
	//newbacs
	a[0]= null_bap;
	a[1]= sacroiliac_tilt; //1 //*
	a[2]= sacroiliac_torsion; //*
	a[3]= sacroiliac_roll; //*
	a[4]= l_hip_flexion ;
	a[5]= r_hip_flexion ;
	a[6]= l_hip_abduct  ;
	a[7]= r_hip_abduct  ;
	a[8]= l_hip_twisting ;
	a[9]= r_hip_twisting ;
	a[10]= l_knee_flexion ; //10
	a[11]= r_knee_flexion ;
	a[12]= l_knee_twisting ;
	a[13]= r_knee_twisting ;
	a[14]= l_ankle_flexion ;
	a[15]= r_ankle_flexion ;
	a[16]= l_ankle_twisting;
	a[17]= r_ankle_twisting;
	a[18]= l_subtalar_flexion ; //*
	a[19]= r_subtalar_flexion ; //*
	a[20]= l_midtarsal_twisting ; //20 //*
	a[21]= r_midtarsal_twisting ; //*
	a[22]= l_metatarsal_flexion ; //*
	a[23]= r_metatarsal_flexion ; //*
	a[24]= l_sternoclavicular_abduct ;
	a[25]= r_sternoclavicular_abduct ;
	a[26]= l_sternoclavicular_rotate ;
	a[27]= r_sternoclavicular_rotate ;
	a[28]= l_acromioclavicular_abduct ;
	a[29]= r_acromioclavicular_abduct ;
	a[30]= l_acromioclavicular_rotate ; //30
	a[31]= r_acromioclavicular_rotate ;
	a[32]= l_shoulder_flexion ;
	a[33]= r_shoulder_flexion ;
	a[34]= l_shoulder_abduct ;
	a[35]= r_shoulder_abduct ;
	a[36]= l_shoulder_twisting ;
	a[37]= r_shoulder_twisting ;
	a[38]= l_elbow_flexion ;
	a[39]= r_elbow_flexion ;
	a[40]= l_elbow_twisting ; //40
	a[41]= r_elbow_twisting ;
	a[42]= l_wrist_flexion ;
	a[43]= r_wrist_flexion ;
	a[44]= l_wrist_pivot  ;
	a[45]= r_wrist_pivot  ;
	a[46]= l_wrist_twisting;
	a[47]= r_wrist_twisting; //------------ below is different
	a[48]= skullbase_roll;
	a[49]= skullbase_torsion;
	a[50]= skullbase_tilt; //50
	a[51]= vc1_roll;
	a[52]= vc1_torsion;
	a[53]= vc1_tilt;
	a[54]= vc2_roll; //simple spine DOF vc8_roll	
	a[55]= vc2_torsion; //simple spine DOF vc8_torsion
	a[56]= vc2_tilt; //simple spine DOF vc8_tilt
	a[57]= vc3_roll;
	a[58]= vc3_torsion;
	a[59]= vc3_tilt;
	a[60]= vc4_roll; //simple spine DOF vc7_roll //60
	a[61]= vc4_torsion; //simple spine DOF vc7_torsion
	a[62]= vc4_tilt; //simple spine DOF vc7_tilt
	a[63]= vc5_roll;
	a[64]= vc5_torsion;
	a[65]= vc5_tilt;
	a[66]= vc6_roll;
	a[67]= vc6_torsion;
	a[68]= vc6_tilt;
	a[69]= vc7_roll;
	a[70]= vc7_torsion; //70
	a[71]= vc7_tilt;
	a[72]= vt1_roll; //simple spine DOF vt6_roll
	a[73]= vt1_torsion; //simple spine DOF vt6_torsion
	a[74]= vt1_tilt; //simple spine DOF vt6_tilt
	a[75]= vt2_roll;
	a[76]= vt2_torsion;
	a[77]= vt2_tilt;
	a[78]= vt3_roll;
	a[79]= vt3_torsion;
	a[80]= vt3_tilt; //80
	a[81]= vt4_roll;
	a[82]= vt4_torsion;
	a[83]= vt4_tilt;
	a[84]= vt5_roll;
	a[85]= vt5_torsion;
	a[86]= vt5_tilt;
	a[87]= vt6_roll; //simple spine DOF vt5_roll
	a[88]= vt6_torsion; 	//simple spine DOF vt5_torsion
	a[89]= vt6_tilt;
	a[90]= vt7_roll;//90
	a[91]= vt7_torsion;
	a[92]= vt7_tilt;
	a[93]= vt8_roll;
	a[94]= vt8_torsion;
	a[95]= vt8_tilt;
	a[96]= vt9_roll;
	a[97]= vt9_torsion;
	a[98]= vt9_tilt;
	a[99]= vt10_roll; //simple spine DOF vt4_roll	
	a[100]= vt10_torsion; //simple spine DOF vt4_torsion //100
	a[101]= vt10_tilt; //simple spine DOF vt4_tilt
	a[102]= vt11_roll;
	a[103]= vt11_torsion;
	a[104]= vt11_tilt;
	a[105]= vt12_roll;
	a[106]= vt12_torsion;
	a[107]= vt12_tilt;
	a[108]= vl1_roll; //simple spine DOF vl3_roll	
	a[109]= vl1_torsion;
	a[110]= vl1_tilt; //simple spine DOF vl3_tilt	//110
	a[111]= vl2_roll;
	a[112]= vl2_torsion;
	a[113]= vl2_tilt;
	a[114]= vl3_roll; //simple spine DOF vl2_roll
	a[115]= vl3_torsion;
	a[116]= vl3_tilt; //simple spine DOF vl2_tilt	
	a[117]= vl4_roll;
	a[118]= vl4_torsion;
	a[119]= vl4_tilt;
	a[120]= vl5_roll; //120
	a[121]= vl5_torsion;
	a[122]= vl5_tilt;	
	a[123]= l_pinky0_flexion;
	a[124]= r_pinky0_flexion;
	a[125]= l_pinky1_flexion; 
	a[126]= r_pinky1_flexion;
	a[127]= l_pinky1_pivot; 
	a[128]= r_pinky1_pivot;
	a[129]= l_pinky1_twisting;
	a[130]= r_pinky1_twisting; //130
	a[131]= l_pinky2_flexion;
	a[132]= r_pinky2_flexion;
	a[133]= l_pinky3_flexion;
	a[134]= r_pinky3_flexion;
	a[135]= l_ring0_flexion;
	a[136]= r_ring0_flexion;
	a[137]= l_ring1_flexion;
	a[138]= r_ring1_flexion;
	a[139]= l_ring1_pivot;
	a[140]= r_ring1_pivot; //140
	a[141]= l_ring1_twisting;
	a[142]= r_ring1_twisting;
	a[143]= l_ring2_flexion;
	a[144]= r_ring2_flexion;
	a[145]= l_ring3_flexion;
	a[146]= r_ring3_flexion;
	a[147]= l_middle0_flexion;
	a[148]= r_middle0_flexion;
	a[149]= l_middle1_flexion;
	a[150]= r_middle1_flexion; //150
	a[151]= l_middle1_pivot;
	a[152]= r_middle1_pivot;
	a[153]= l_middle1_twisting;
	a[154]= r_middle1_twisting;
	a[155]= l_middle2_flexion;
	a[156]= r_middle2_flexion;
	a[157]= l_middle3_flexion;
	a[158]= r_middle3_flexion;
	a[159]= l_index0_flexion;
	a[160]= r_index0_flexion; //160
	a[161]= l_index1_flexion;
	a[162]= r_index1_flexion;
	a[163]= l_index1_pivot;
	a[164]= r_index1_pivot;
	a[165]= l_index1_twisting;
	a[166]= r_index1_twisting;
	a[167]= l_index2_flexion;
	a[168]= r_index2_flexion;
	a[169]= l_index3_flexion;
	a[170]= r_index3_flexion; //170
	a[171]= l_thumb1_flexion;
	a[172]= r_thumb1_flexion;
	a[173]= l_thumb1_pivot;
	a[174]= r_thumb1_pivot;
	a[175]= l_thumb1_twisting;
	a[176]= r_thumb1_twisting;
	a[177]= l_thumb2_flexion;
	a[178]= r_thumb2_flexion;
	a[179]= l_thumb3_flexion;
	a[180]= r_thumb3_flexion; //180
	a[181]= HumanoidRoot_tr_vertical;
	a[182]= HumanoidRoot_tr_lateral;
	a[183]= HumanoidRoot_tr_frontal;
	a[184]= HumanoidRoot_rt_body_turn;
	a[185]= HumanoidRoot_rt_body_roll;
	a[186]= HumanoidRoot_rt_body_tilt;
#endif

	int i = 0;
	// BAP->Maya initialization;
	for (i=0; i<=NUMBAPS; i++) {
		s[i]=string("");
	}
#ifdef USEOLDBAPS
	s[l_clav_abduct]=string("l_sternoclavicular.rotateZ");	
	s[r_clav_abduct]=string("r_sternoclavicular.rotateZ");
	s[l_clav_rotate]=string("l_sternoclavicular.rotateY");
	s[r_clav_rotate]=string("r_sternoclavicular.rotateY");
	s[l_scap_abduct]=string("l_acromioclavicular.rotateZ");
	s[r_scap_abduct]=string("r_acromioclavicular.rotateZ");
	s[l_scap_rotate]=string("l_acromioclavicular.rotateY");
	s[r_scap_rotate]=string("r_acromioclavicular.rotateY");
	s[l_shoulder_flexion]=string("l_shoulder.rotateX");
	s[r_shoulder_flexion]=string("r_shoulder.rotateX");
	s[l_shoulder_abduct]=string("l_shoulder.rotateZ");
	s[r_shoulder_abduct]=string("r_shoulder.rotateZ");
	s[l_shoulder_twisting]=string("l_shoulder.rotateY");
	s[r_shoulder_twisting]=string("r_shoulder.rotateY");
	
	s[l_elbow_flexion]=string("l_elbow.rotateX");
	s[l_elbow_twisting]=string("l_elbow.rotateY");
	s[l_wrist_flexion]=string("l_wrist.rotateZ");
	s[l_wrist_pivot]=string("l_wrist.rotateX");
	s[l_wrist_twisting]=string("l_wrist.rotateY");
	
	s[l_pinky_flexion1]=string("l_pinky1.rotateZ");
	s[l_pinky_abduct]=string("l_pinky1.rotateX");
	s[l_pinky_twisting]=string("l_pinky1.rotateY");
	s[l_pinky_flexion2]=string("l_pinky2.rotateZ");
	s[l_pinky_flexion3]=string("l_pinky3.rotateZ");
	
	s[l_ring_flexion1]=string("l_ring1.rotateZ");
	s[l_ring_abduct]=string("l_ring1.rotateX");
	s[l_ring_twisting]=string("l_ring1.rotateY");
	s[l_ring_flexion2]=string("l_ring2.rotateZ");
	s[l_ring_flexion3]=string("l_ring3.rotateZ");

	s[l_middle_flexion1]=string("l_middle1.rotateZ");
	s[l_middle_abduct]=string("l_middle1.rotateX");
	s[l_middle_twisting]=string("l_middle1.rotateY");
	s[l_middle_flexion2]=string("l_middle2.rotateZ");
	s[l_middle_flexion3]=string("l_middle3.rotateZ");
	
	s[l_index_flexion1]=string("l_index1.rotateZ");
	s[l_index_abduct]=string("l_index1.rotateX");
	s[l_index_twisting]=string("l_index1.rotateY");
	s[l_index_flexion2]=string("l_index2.rotateZ");
	s[l_index_flexion3]=string("l_index3.rotateZ");

	s[l_thumb_flexion1]=string("l_thumb1.rotateX");
	s[l_thumb_abduct]=string("l_thumb1.rotateY");
	s[l_thumb_twisting]=string("l_thumb1.rotateZ");
	s[l_thumb_flexion2]=string("l_thumb2.rotateX");
	s[l_thumb_flexion3]=string("l_thumb3.rotateX");

	s[r_elbow_flexion]=string("r_elbow.rotateX");
	s[r_elbow_twisting]=string("r_elbow.rotateY");
	s[r_wrist_flexion]=string("r_wrist.rotateZ");
	s[r_wrist_pivot]=string("r_wrist.rotateX");
	s[r_wrist_twisting]=string("r_wrist.rotateY");
	
	s[r_pinky_flexion1]=string("r_pinky1.rotateZ");
	s[r_pinky_abduct]=string("r_pinky1.rotateX");
	s[r_pinky_twisting]=string("r_pinky1.rotateY");
	s[r_pinky_flexion2]=string("r_pinky2.rotateZ");
	s[r_pinky_flexion3]=string("r_pinky3.rotateZ");
	
	s[r_ring_flexion1]=string("r_ring1.rotateZ");
	s[r_ring_abduct]=string("r_ring1.rotateX");
	s[r_ring_twisting]=string("r_ring1.rotateY");
	s[r_ring_flexion2]=string("r_ring2.rotateZ");
	s[r_ring_flexion3]=string("r_ring3.rotateZ");

	s[r_middle_flexion1]=string("r_middle1.rotateZ");
	s[r_middle_abduct]=string("r_middle1.rotateX");
	s[r_middle_twisting]=string("r_middle1.rotateY");
	s[r_middle_flexion2]=string("r_middle2.rotateZ");
	s[r_middle_flexion3]=string("r_middle3.rotateZ");
	
	s[r_index_flexion1]=string("r_index1.rotateZ");
	s[r_index_abduct]=string("r_index1.rotateX");
	s[r_index_twisting]=string("r_index1.rotateY");
	s[r_index_flexion2]=string("r_index2.rotateZ");
	s[r_index_flexion3]=string("r_index3.rotateZ");

	s[r_thumb_flexion1]=string("r_thumb1.rotateX");
	s[r_thumb_abduct]=string("r_thumb1.rotateY");
	s[r_thumb_twisting]=string("r_thumb1.rotateZ");
	s[r_thumb_flexion2]=string("r_thumb2.rotateX");
	s[r_thumb_flexion3]=string("r_thumb3.rotateX");

	s[l1tilt]=string("vl1.rotateX");
	s[l1roll]=string("vl1.rotateZ");
	s[l1torsion]=string("vl1.rotateY");

	s[l2tilt]=string("vl2.rotateX");
	s[l2roll]=string("vl2.rotateZ");
	s[l2torsion]=string("vl2.rotateY");

	s[l3tilt]=string("vl3.rotateX");
	s[l3roll]=string("vl3.rotateZ");
	s[l3torsion]=string("vl3.rotateY");
	
	s[l4tilt]=string("vl4.rotateX");
	s[l4roll]=string("vl4.rotateZ");
	s[l4torsion]=string("vl4.rotateY");

	s[l5tilt]=string("vl5.rotateX");
	s[l5roll]=string("vl5.rotateZ");
	s[l5torsion]=string("vl5.rotateY");

	
	s[t1tilt]=string("vt1.rotateX");
	s[t1torsion]=string("vt1.rotateY");
	s[t1roll]=string("vt1.rotateZ");

	s[t2tilt]=string("vt2.rotateX");
	s[t2torsion]=string("vt2.rotateY");
	s[t2roll]=string("vt2.rotateZ");

	s[t3tilt]=string("vt3.rotateX");
	s[t3torsion]=string("vt3.rotateY");
	s[t3roll]=string("vt3.rotateZ");

	s[t4tilt]=string("vt4.rotateX");
	s[t4torsion]=string("vt4.rotateY");
	s[t4roll]=string("vt4.rotateZ");

	s[t5tilt]=string("vt5.rotateX");
	s[t5torsion]=string("vt5.rotateY");
	s[t5roll]=string("vt5.rotateZ");

	s[t6tilt]=string("vt6.rotateX");
	s[t6torsion]=string("vt6.rotateY");
	s[t6roll]=string("vt6.rotateZ");

	s[t7tilt]=string("vt7.rotateX");
	s[t7torsion]=string("vt7.rotateY");
	s[t7roll]=string("vt7.rotateZ");

	s[t8tilt]=string("vt8.rotateX");
	s[t8torsion]=string("vt8.rotateY");
	s[t8roll]=string("vt8.rotateZ");

	s[t9tilt]=string("vt9.rotateX");
	s[t9torsion]=string("vt9.rotateY");
	s[t9roll]=string("vt9.rotateZ");

	s[t10tilt]=string("vt10.rotateX");
	s[t10torsion]=string("vt10.rotateY");
	s[t10roll]=string("vt10.rotateZ");

	s[t11tilt]=string("vt11.rotateX");
	s[t11torsion]=string("vt11.rotateY");
	s[t11roll]=string("vt11.rotateZ");

	s[t12tilt]=string("vt12.rotateX");
	s[t12torsion]=string("vt12.rotateY");
	s[t12roll]=string("vt12.rotateZ");


	s[c1tilt]=string("vc1.rotateX");
	s[c1torsion]=string("vc1.rotateY");
	s[c1roll]=string("vc1.rotateZ");

	s[c2tilt]=string("vc2.rotateX");
	s[c2torsion]=string("vc2.rotateY");
	s[c2roll]=string("vc2.rotateZ");

	s[c3tilt]=string("vc3.rotateX");
	s[c3torsion]=string("vc3.rotateY");
	s[c3roll]=string("vc3.rotateZ");

	s[c4tilt]=string("vc4.rotateX");
	s[c4torsion]=string("vc4.rotateY");
	s[c4roll]=string("vc4.rotateZ");

	s[c5tilt]=string("vc5.rotateX");
	s[c5torsion]=string("vc5.rotateY");
	s[c5roll]=string("vc5.rotateZ");

	s[c6tilt]=string("vc6.rotateX");
	s[c6torsion]=string("vc6.rotateY");
	s[c6roll]=string("vc6.rotateZ");

	s[c7tilt]=string("vc7.rotateX");
	s[c7torsion]=string("vc7.rotateY");
	s[c7roll]=string("vc7.rotateZ");
	

	// bjoern's hms spine+leg joints:
	
	
	s[Pelvic_tilt]=string("sacroiliac.rotateX");
	s[Pelvic_torsion]=string("sacroiliac.rotateY");
	s[Pelvic_roll]=string("sacroiliac.rotateZ");

	s[l_hip_flexion]=string("l_hip.rotateX");
	s[r_hip_flexion]=string("r_hip.rotateX");
	s[l_hip_abduct]=string("l_hip.rotateZ");
	s[r_hip_abduct]=string("r_hip.rotateZ");
	s[l_hip_twisting]=string("l_hip.rotateY");
	s[r_hip_twisting]=string("r_hip.rotateY");
	s[l_knee_flexion]=string("l_knee.rotateX"); //10
	s[r_knee_flexion]=string("r_knee.rotateX");
	s[l_knee_twisting]=string("l_knee.rotateY");
	s[r_knee_twisting]=string("r_knee.rotateY");
	s[l_ankle_flexion]=string("l_ankle.rotateX");
	s[r_ankle_flexion]=string("r_ankle.rotateX");
	s[l_ankle_twisting]=string("l_ankle.rotateY");
	s[r_ankle_twisting]=string("r_ankle.rotateY");

	s[l_subtalar_flexion]=string("l_subtalar.rotateX");
	s[r_subtalar_flexion]=string("r_subtalar.rotateX");

	s[l_midtarsal_twisting]=string("l_midtarsal.rotateY");
	s[r_midtarsal_twisting]=string("r _midtarsal.rotateY");

	s[l_metatarsal_flexion]=string("l_metatarsal.rotateX");
	s[r_metatarsal_flexion]=string("r _metatarsal.rotateX");

#else
	
	s[l_sternoclavicular_abduct]=string("l_sternoclavicular.rotateZ");	
	s[r_sternoclavicular_abduct]=string("r_sternoclavicular.rotateZ");
	s[l_sternoclavicular_rotate]=string("l_sternoclavicular.rotateY");
	s[r_sternoclavicular_rotate]=string("r_sternoclavicular.rotateY");
	s[l_acromioclavicular_abduct]=string("l_acromioclavicular.rotateZ");
	s[r_acromioclavicular_abduct]=string("r_acromioclavicular.rotateZ");
	s[l_acromioclavicular_rotate]=string("l_acromioclavicular.rotateY");
	s[r_acromioclavicular_rotate]=string("r_acromioclavicular.rotateY");
	s[l_shoulder_flexion]=string("l_shoulder.rotateX");
	s[r_shoulder_flexion]=string("r_shoulder.rotateX");
	s[l_shoulder_abduct]=string("l_shoulder.rotateZ");
	s[r_shoulder_abduct]=string("r_shoulder.rotateZ");
	s[l_shoulder_twisting]=string("l_shoulder.rotateY");
	s[r_shoulder_twisting]=string("r_shoulder.rotateY");
	
	s[l_elbow_flexion]=string("l_elbow.rotateX");
	s[l_elbow_twisting]=string("l_elbow.rotateY");
	s[l_wrist_flexion]=string("l_wrist.rotateZ");
	s[l_wrist_pivot]=string("l_wrist.rotateX");
	s[l_wrist_twisting]=string("l_wrist.rotateY");
	
	s[l_pinky1_flexion]=string("l_pinky1.rotateZ");
	s[l_pinky1_pivot]=string("l_pinky1.rotateX");
	s[l_pinky1_twisting]=string("l_pinky1.rotateY");
	s[l_pinky2_flexion]=string("l_pinky2.rotateZ");
	s[l_pinky3_flexion]=string("l_pinky3.rotateZ");
	
	s[l_ring1_flexion]=string("l_ring1.rotateZ");
	s[l_ring1_pivot]=string("l_ring1.rotateX");
	s[l_ring1_twisting]=string("l_ring1.rotateY");
	s[l_ring2_flexion]=string("l_ring2.rotateZ");
	s[l_ring3_flexion]=string("l_ring3.rotateZ");

	s[l_middle1_flexion]=string("l_middle1.rotateZ");
	s[l_middle1_pivot]=string("l_middle1.rotateX");
	s[l_middle1_twisting]=string("l_middle1.rotateY");
	s[l_middle2_flexion]=string("l_middle2.rotateZ");
	s[l_middle3_flexion]=string("l_middle3.rotateZ");
	
	s[l_index1_flexion]=string("l_index1.rotateZ");
	s[l_index1_pivot]=string("l_index1.rotateX");
	s[l_index1_twisting]=string("l_index1.rotateY");
	s[l_index2_flexion]=string("l_index2.rotateZ");
	s[l_index3_flexion]=string("l_index3.rotateZ");

	s[l_thumb1_flexion]=string("l_thumb1.rotateX");
	s[l_thumb1_pivot]=string("l_thumb1.rotateY");
	s[l_thumb1_twisting]=string("l_thumb1.rotateZ");
	s[l_thumb2_flexion]=string("l_thumb2.rotateX");
	s[l_thumb3_flexion]=string("l_thumb3.rotateX");

	s[r_elbow_flexion]=string("r_elbow.rotateX");
	s[r_elbow_twisting]=string("r_elbow.rotateY");
	s[r_wrist_flexion]=string("r_wrist.rotateZ");
	s[r_wrist_pivot]=string("r_wrist.rotateX");
	s[r_wrist_twisting]=string("r_wrist.rotateY");
	
	s[r_pinky1_flexion]=string("r_pinky1.rotateZ");
	s[r_pinky1_pivot]=string("r_pinky1.rotateX");
	s[r_pinky1_twisting]=string("r_pinky1.rotateY");
	s[r_pinky2_flexion]=string("r_pinky2.rotateZ");
	s[r_pinky3_flexion]=string("r_pinky3.rotateZ");
	
	s[r_ring1_flexion]=string("r_ring1.rotateZ");
	s[r_ring1_pivot]=string("r_ring1.rotateX");
	s[r_ring1_twisting]=string("r_ring1.rotateY");
	s[r_ring2_flexion]=string("r_ring2.rotateZ");
	s[r_ring3_flexion]=string("r_ring3.rotateZ");

	s[r_middle1_flexion]=string("r_middle1.rotateZ");
	s[r_middle1_pivot]=string("r_middle1.rotateX");
	s[r_middle1_twisting]=string("r_middle1.rotateY");
	s[r_middle2_flexion]=string("r_middle2.rotateZ");
	s[r_middle3_flexion]=string("r_middle3.rotateZ");
	
	s[r_index1_flexion]=string("r_index1.rotateZ");
	s[r_index1_pivot]=string("r_index1.rotateX");
	s[r_index1_twisting]=string("r_index1.rotateY");
	s[r_index2_flexion]=string("r_index2.rotateZ");
	s[r_index3_flexion]=string("r_index3.rotateZ");

	s[r_thumb1_flexion]=string("r_thumb1.rotateX");
	s[r_thumb1_pivot]=string("r_thumb1.rotateY");
	s[r_thumb1_twisting]=string("r_thumb1.rotateZ");
	s[r_thumb2_flexion]=string("r_thumb2.rotateX");
	s[r_thumb3_flexion]=string("r_thumb3.rotateX");
	
	// simplified spine:

	s[vl5_tilt]=string("vl5.rotateX");
	s[vl3_tilt]=string("vl3.rotateX");
	s[vl3_roll]=string("vl3.rotateZ");
	s[vl1_tilt]=string("vl1.rotateX");
	s[vl1_roll]=string("vl1.rotateZ");
	s[vt10_tilt]=string("vt10.rotateX");
	s[vt10_torsion]=string("vt10.rotateY");
	s[vt10_roll]=string("vt10.rotateZ");
	s[vt6_torsion]=string("vt6.rotateY");
	s[vt6_roll]=string("vt6.rotateZ");
	s[vt1_tilt]=string("vt1.rotateX");
	s[vt1_torsion]=string("vt1.rotateY");
	s[vt1_roll]=string("vt1.rotateZ");
	s[vc4_tilt]=string("vc4.rotateX");
	s[vc4_torsion]=string("vc4.rotateY");
	s[vc4_roll]=string("vc4.rotateZ");
	s[vc2_tilt]=string("vc2.rotateX");
	s[vc2_torsion]=string("vc2.rotateY");
	s[vc2_roll]=string("vc2.rotateZ");

	// bjoern's hms spine+leg joints:
	s[vt9_tilt]=string("vt9.rotateX");
	s[vt9_torsion]=string("vt9.rotateY");
	s[vt9_roll]=string("vt9.rotateZ");
	s[vc3_tilt]=string("vc3.rotateX");
	s[vc3_torsion]=string("vc3.rotateY");
	s[vc3_roll]=string("vc3.rotateZ");
	s[sacroiliac_tilt]=string("sacroiliac.rotateX");
	s[sacroiliac_torsion]=string("sacroiliac.rotateY");
	s[sacroiliac_roll]=string("sacroiliac.rotateZ");

	s[l_hip_flexion]=string("l_hip.rotateX");
	s[r_hip_flexion]=string("r_hip.rotateX");
	s[l_hip_abduct]=string("l_hip.rotateZ");
	s[r_hip_abduct]=string("r_hip.rotateZ");
	s[l_hip_twisting]=string("l_hip.rotateY");
	s[r_hip_twisting]=string("r_hip.rotateY");
	s[l_knee_flexion]=string("l_knee.rotateX"); //10
	s[r_knee_flexion]=string("r_knee.rotateX");
	s[l_knee_twisting]=string("l_knee.rotateY");
	s[r_knee_twisting]=string("r_knee.rotateY");
	s[l_ankle_flexion]=string("l_ankle.rotateX");
	s[r_ankle_flexion]=string("r_ankle.rotateX");
	s[l_ankle_twisting]=string("l_ankle.rotateY");
	s[r_ankle_twisting]=string("r_ankle.rotateY");

	//s[]=string("");
#endif

	// BAP->joint_type initialization:
	for (i=0; i<=NUMBAPS; i++) {
		b2j[i]=null_joint;
	}

#ifdef USEOLDBAPS
	b2j[null_bap]=null_joint;
	b2j[l_clav_abduct]=l_sternoclavicular;	
	b2j[r_clav_abduct]=r_sternoclavicular;
	b2j[l_clav_rotate]=l_sternoclavicular;
	b2j[r_clav_rotate]=r_sternoclavicular;
	b2j[l_scap_abduct]=l_acromioclavicular;
	b2j[r_scap_abduct]=r_acromioclavicular;
	b2j[l_scap_rotate]=l_acromioclavicular;
	b2j[r_scap_rotate]=r_acromioclavicular;
	b2j[l_shoulder_flexion]=l_shoulder;
	b2j[r_shoulder_flexion]=r_shoulder;
	b2j[l_shoulder_abduct]=l_shoulder;
	b2j[r_shoulder_abduct]=r_shoulder;
	b2j[l_shoulder_twisting]=l_shoulder;
	b2j[r_shoulder_twisting]=r_shoulder;
	
	b2j[l_elbow_flexion]=l_elbow;
	b2j[l_elbow_twisting]=l_elbow;
	b2j[l_wrist_flexion]=l_wrist;
	b2j[l_wrist_pivot]=l_wrist;
	b2j[l_wrist_twisting]=l_wrist;
	
	b2j[l_pinky_flexion1]=l_pinky1;
	b2j[l_pinky_abduct]=l_pinky1;
	b2j[l_pinky_twisting]=l_pinky1;
	b2j[l_pinky_flexion2]=l_pinky2;
	b2j[l_pinky_flexion3]=l_pinky3;
	
	b2j[l_ring_flexion1]=l_ring1;
	b2j[l_ring_abduct]=l_ring1;
	b2j[l_ring_twisting]=l_ring1;
	b2j[l_ring_flexion2]=l_ring2;
	b2j[l_ring_flexion3]=l_ring3;

	b2j[l_middle_flexion1]=l_middle1;
	b2j[l_middle_abduct]=l_middle1;
	b2j[l_middle_twisting]=l_middle1;
	b2j[l_middle_flexion2]=l_middle2;
	b2j[l_middle_flexion3]=l_middle3;
	
	b2j[l_index_flexion1]=l_index1;
	b2j[l_index_abduct]=l_index1;
	b2j[l_index_twisting]=l_index1;
	b2j[l_index_flexion2]=l_index2;
	b2j[l_index_flexion3]=l_index3;

	b2j[l_thumb_flexion1]=l_thumb1;
	b2j[l_thumb_abduct]=l_thumb1;
	b2j[l_thumb_twisting]=l_thumb1;
	b2j[l_thumb_flexion2]=l_thumb2;
	b2j[l_thumb_flexion3]=l_thumb3;

	b2j[r_elbow_flexion]=r_elbow;
	b2j[r_elbow_twisting]=r_elbow;
	b2j[r_wrist_flexion]=r_wrist;
	b2j[r_wrist_pivot]=r_wrist;
	b2j[r_wrist_twisting]=r_wrist;
	
	b2j[r_pinky_flexion1]=r_pinky1;
	b2j[r_pinky_abduct]=r_pinky1;
	b2j[r_pinky_twisting]=r_pinky1;
	b2j[r_pinky_flexion2]=r_pinky2;
	b2j[r_pinky_flexion3]=r_pinky3;
	
	b2j[r_ring_flexion1]=r_ring1;
	b2j[r_ring_abduct]=r_ring1;
	b2j[r_ring_twisting]=r_ring1;
	b2j[r_ring_flexion2]=r_ring2;
	b2j[r_ring_flexion3]=r_ring3;

	b2j[r_middle_flexion1]=r_middle1;
	b2j[r_middle_abduct]=r_middle1;
	b2j[r_middle_twisting]=r_middle1;
	b2j[r_middle_flexion2]=r_middle2;
	b2j[r_middle_flexion3]=r_middle3;
	
	b2j[r_index_flexion1]=r_index1;
	b2j[r_index_abduct]=r_index1;
	b2j[r_index_twisting]=r_index1;
	b2j[r_index_flexion2]=r_index2;
	b2j[r_index_flexion3]=r_index3;

	b2j[r_thumb_flexion1]=r_thumb1;
	b2j[r_thumb_abduct]=r_thumb1;
	b2j[r_thumb_twisting]=r_thumb1;
	b2j[r_thumb_flexion2]=r_thumb2;
	b2j[r_thumb_flexion3]=r_thumb3;

	
	b2j[l1tilt]=vl1;
	b2j[l1roll]=vl1;
	b2j[l1torsion]=vl1;

	b2j[l2tilt]=vl2;
	b2j[l2roll]=vl2;
	b2j[l2torsion]=vl2;

	b2j[l3tilt]=vl3;
	b2j[l3roll]=vl3;
	b2j[l3torsion]=vl3;

	b2j[l4tilt]=vl4;
	b2j[l4roll]=vl4;
	b2j[l4torsion]=vl4;

	b2j[l5tilt]=vl5;
	b2j[l5roll]=vl5;
	b2j[l5torsion]=vl5;

	b2j[t1tilt]=vt1;
	b2j[t1torsion]=vt1;
	b2j[t1roll]=vt1;

	b2j[t2tilt]=vt2;
	b2j[t2torsion]=vt2;
	b2j[t2roll]=vt2;

	b2j[t3tilt]=vt3;
	b2j[t3torsion]=vt3;
	b2j[t3roll]=vt3;

	b2j[t4tilt]=vt4;
	b2j[t4torsion]=vt4;
	b2j[t4roll]=vt4;

	b2j[t5tilt]=vt5;
	b2j[t5torsion]=vt5;
	b2j[t5roll]=vt5;

	b2j[t6tilt]=vt6;
	b2j[t6torsion]=vt6;
	b2j[t6roll]=vt6;

	b2j[t7tilt]=vt7;
	b2j[t7torsion]=vt7;
	b2j[t7roll]=vt7;

	b2j[t8tilt]=vt8;
	b2j[t8torsion]=vt8;
	b2j[t8roll]=vt8;

	b2j[t9tilt]=vt9;
	b2j[t9torsion]=vt9;
	b2j[t9roll]=vt9;

	b2j[t10tilt]=vt10;
	b2j[t10torsion]=vt10;
	b2j[t10roll]=vt10;

	b2j[t11tilt]=vt11;
	b2j[t11torsion]=vt11;
	b2j[t11roll]=vt11;

	b2j[t12tilt]=vt12;
	b2j[t12torsion]=vt12;
	b2j[t12roll]=vt12;

	b2j[c1tilt]=vc1;
	b2j[c1torsion]=vc1;
	b2j[c1roll]=vc1;
	
	b2j[c2tilt]=vc2;
	b2j[c2torsion]=vc2;
	b2j[c2roll]=vc2;

	b2j[c3tilt]=vc3;
	b2j[c3torsion]=vc3;
	b2j[c3roll]=vc3;

	b2j[c4tilt]=vc4;
	b2j[c4torsion]=vc4;
	b2j[c4roll]=vc4;

	b2j[c5tilt]=vc5;
	b2j[c5torsion]=vc5;
	b2j[c5roll]=vc5;

	b2j[c6tilt]=vc6;
	b2j[c6torsion]=vc6;
	b2j[c6roll]=vc6;

	b2j[c7tilt]=vc7;
	b2j[c7torsion]=vc7;
	b2j[c7roll]=vc7;
	// bjoern's hms spine+leg joints:
	
	
	b2j[Pelvic_tilt]=sacroiliac;
	b2j[Pelvic_torsion]=sacroiliac;
	b2j[Pelvic_roll]=sacroiliac;

	b2j[l_hip_flexion]=l_hip;
	b2j[r_hip_flexion]=r_hip;
	b2j[l_hip_abduct]=l_hip;
	b2j[r_hip_abduct]=r_hip;
	b2j[l_hip_twisting]=l_hip;
	b2j[r_hip_twisting]=r_hip;
	b2j[l_knee_flexion]=l_knee; //10
	b2j[r_knee_flexion]=r_knee;
	b2j[l_knee_twisting]=l_knee;
	b2j[r_knee_twisting]=r_knee;
	b2j[l_ankle_flexion]=l_ankle;
	b2j[r_ankle_flexion]=r_ankle;
	b2j[l_ankle_twisting]=l_ankle;
	b2j[r_ankle_twisting]=r_ankle;

	b2j[l_subtalar_flexion]=l_subtalar;
	b2j[r_subtalar_flexion]=r_subtalar;
	b2j[l_midtarsal_twisting]=l_midtarsal;
	b2j[r_midtarsal_twisting]=r_midtarsal;
	b2j[l_metatarsal_flexion]=l_metatarsal;
	b2j[r_metatarsal_flexion]=r_metatarsal;
#else
	b2j[null_bap]=null_joint;
	b2j[l_sternoclavicular_abduct]=l_sternoclavicular;	
	b2j[r_sternoclavicular_abduct]=r_sternoclavicular;
	b2j[l_sternoclavicular_rotate]=l_sternoclavicular;
	b2j[r_sternoclavicular_rotate]=r_sternoclavicular;
	b2j[l_acromioclavicular_abduct]=l_acromioclavicular;
	b2j[r_acromioclavicular_abduct]=r_acromioclavicular;
	b2j[l_acromioclavicular_rotate]=l_acromioclavicular;
	b2j[r_acromioclavicular_rotate]=r_acromioclavicular;
	b2j[l_shoulder_flexion]=l_shoulder;
	b2j[r_shoulder_flexion]=r_shoulder;
	b2j[l_shoulder_abduct]=l_shoulder;
	b2j[r_shoulder_abduct]=r_shoulder;
	b2j[l_shoulder_twisting]=l_shoulder;
	b2j[r_shoulder_twisting]=r_shoulder;
	
	b2j[l_elbow_flexion]=l_elbow;
	b2j[l_elbow_twisting]=l_elbow;
	b2j[l_wrist_flexion]=l_wrist;
	b2j[l_wrist_pivot]=l_wrist;
	b2j[l_wrist_twisting]=l_wrist;
	
	b2j[l_pinky1_flexion]=l_pinky1;
	b2j[l_pinky1_pivot]=l_pinky1;
	b2j[l_pinky1_twisting]=l_pinky1;
	b2j[l_pinky2_flexion]=l_pinky2;
	b2j[l_pinky3_flexion]=l_pinky3;
	
	b2j[l_ring1_flexion]=l_ring1;
	b2j[l_ring1_pivot]=l_ring1;
	b2j[l_ring1_twisting]=l_ring1;
	b2j[l_ring2_flexion]=l_ring2;
	b2j[l_ring3_flexion]=l_ring3;

	b2j[l_middle1_flexion]=l_middle1;
	b2j[l_middle1_pivot]=l_middle1;
	b2j[l_middle1_twisting]=l_middle1;
	b2j[l_middle2_flexion]=l_middle2;
	b2j[l_middle3_flexion]=l_middle3;
	
	b2j[l_index1_flexion]=l_index1;
	b2j[l_index1_pivot]=l_index1;
	b2j[l_index1_twisting]=l_index1;
	b2j[l_index2_flexion]=l_index2;
	b2j[l_index3_flexion]=l_index3;

	b2j[l_thumb1_flexion]=l_thumb1;
	b2j[l_thumb1_pivot]=l_thumb1;
	b2j[l_thumb1_twisting]=l_thumb1;
	b2j[l_thumb2_flexion]=l_thumb2;
	b2j[l_thumb3_flexion]=l_thumb3;

	b2j[r_elbow_flexion]=r_elbow;
	b2j[r_elbow_twisting]=r_elbow;
	b2j[r_wrist_flexion]=r_wrist;
	b2j[r_wrist_pivot]=r_wrist;
	b2j[r_wrist_twisting]=r_wrist;
	
	b2j[r_pinky1_flexion]=r_pinky1;
	b2j[r_pinky1_pivot]=r_pinky1;
	b2j[r_pinky1_twisting]=r_pinky1;
	b2j[r_pinky2_flexion]=r_pinky2;
	b2j[r_pinky3_flexion]=r_pinky3;
	
	b2j[r_ring1_flexion]=r_ring1;
	b2j[r_ring1_pivot]=r_ring1;
	b2j[r_ring1_twisting]=r_ring1;
	b2j[r_ring2_flexion]=r_ring2;
	b2j[r_ring3_flexion]=r_ring3;

	b2j[r_middle1_flexion]=r_middle1;
	b2j[r_middle1_pivot]=r_middle1;
	b2j[r_middle1_twisting]=r_middle1;
	b2j[r_middle2_flexion]=r_middle2;
	b2j[r_middle3_flexion]=r_middle3;
	
	b2j[r_index1_flexion]=r_index1;
	b2j[r_index1_pivot]=r_index1;
	b2j[r_index1_twisting]=r_index1;
	b2j[r_index2_flexion]=r_index2;
	b2j[r_index3_flexion]=r_index3;

	b2j[r_thumb1_flexion]=r_thumb1;
	b2j[r_thumb1_pivot]=r_thumb1;
	b2j[r_thumb1_twisting]=r_thumb1;
	b2j[r_thumb2_flexion]=r_thumb2;
	b2j[r_thumb3_flexion]=r_thumb3;
	
	// simplified spine:

	b2j[vl5_tilt]=vl5;
	b2j[vl3_tilt]=vl3;
	b2j[vl3_roll]=vl3;
	b2j[vl1_tilt]=vl1;
	b2j[vl1_roll]=vl1;
	b2j[vt10_tilt]=vt10;
	b2j[vt10_torsion]=vt10;
	b2j[vt10_roll]=vt10;
	b2j[vt6_torsion]=vt6;
	b2j[vt6_roll]=vt6;
	b2j[vt1_tilt]=vt1;
	b2j[vt1_torsion]=vt1;
	b2j[vt1_roll]=vt1;
	b2j[vc4_tilt]=vc4;
	b2j[vc4_torsion]=vc4;
	b2j[vc4_roll]=vc4;
	b2j[vc2_tilt]=vc2;
	b2j[vc2_torsion]=vc2;
	b2j[vc2_roll]=vc2;

	// bjoern's hms spine+leg joints:
	b2j[vt9_tilt]=vt9;
	b2j[vt9_torsion]=vt9;
	b2j[vt9_roll]=vt9;
	b2j[vc3_tilt]=vc3;
	b2j[vc3_torsion]=vc3;
	b2j[vc3_roll]=vc3;
	b2j[sacroiliac_tilt]=sacroiliac;
	b2j[sacroiliac_torsion]=sacroiliac;
	b2j[sacroiliac_roll]=sacroiliac;

	b2j[l_hip_flexion]=l_hip;
	b2j[r_hip_flexion]=r_hip;
	b2j[l_hip_abduct]=l_hip;
	b2j[r_hip_abduct]=r_hip;
	b2j[l_hip_twisting]=l_hip;
	b2j[r_hip_twisting]=r_hip;
	b2j[l_knee_flexion]=l_knee; //10
	b2j[r_knee_flexion]=r_knee;
	b2j[l_knee_twisting]=l_knee;
	b2j[r_knee_twisting]=r_knee;
	b2j[l_ankle_flexion]=l_ankle;
	b2j[r_ankle_flexion]=r_ankle;
	b2j[l_ankle_twisting]=l_ankle;
	b2j[r_ankle_twisting]=r_ankle
	//b2j[]=string("");
#endif
	
	
	//setup joint mapping
	j2j[l_hip]=string("l_hip");
	j2j[l_knee]=string("l_knee");
	j2j[l_ankle]=string("l_ankle");
	j2j[l_subtalar]=string("l_subtalar");
	j2j[l_midtarsal]=string("l_midtarsal");
	j2j[l_metatarsal]=string("l_metatarsal");
	j2j[r_hip]=string("r_hip");
	j2j[r_knee]=string("r_knee");
	j2j[r_ankle]=string("r_ankle");
	j2j[r_subtalar]=string("r_subtalar");
	j2j[r_midtarsal]=string("r_midtarsal");
	j2j[r_metatarsal]=string("r_metatarsal");
	j2j[HumanoidRoot]=string("HumanoidRoot");
	j2j[sacroiliac]=string("sacroiliac");
	j2j[vl5]=string("vl5"); // named vl1 in simplified spine
	j2j[vl4]=string("vl4");
	j2j[vl3]=string("vl3"); // named vl2 in simplified spine
	j2j[vl2]=string("vl2");
	j2j[vl1]=string("vl1"); // named vl3 in simplified spine
	j2j[vt12]=string("vt12");
	j2j[vt11]=string("vt11");
	j2j[vt10]=string("vt10"); // named vt4 in simplified spine
	j2j[vt9]=string("vt9");
	j2j[vt8]=string("vt8");
	j2j[vt7]=string("vt7");
	j2j[vt6]=string("vt6"); // named vt5 in simplified spine
	j2j[vt5]=string("vt5");
	j2j[vt4]=string("vt4");
	j2j[vt3]=string("vt3");
	j2j[vt2]=string("vt2");
	j2j[vt1]=string("vt1"); // named vt6 in simplified spine
	j2j[vc7]=string("vc7");
	j2j[vc6]=string("vc6");
	j2j[vc5]=string("vc5");
	j2j[vc4]=string("vc4"); // named vc7 in simplified spine
	j2j[vc3]=string("vc3");
	j2j[vc2]=string("vc2"); // named vc8 in simplified spine
	j2j[vc1]=string("vc1");
	j2j[skullbase]=string("skullbase");
	j2j[l_eyeball_joint]=string("l_eyeball_joint");
	j2j[r_eyeball_joint]=string("r_eyeball_joint");
	j2j[l_sternoclavicular]=string("l_sternoclavicular");
	j2j[l_acromioclavicular]=string("l_acromioclavicular");
	j2j[l_shoulder]=string("l_shoulder");
	j2j[l_elbow]=string("l_elbow");
	j2j[l_wrist]=string("l_wrist");
	j2j[l_thumb1]=string("l_thumb1");
	j2j[l_thumb2]=string("l_thumb2");
	j2j[l_thumb3]=string("l_thumb3");
	j2j[l_index0]=string("l_index0");
	j2j[l_index1]=string("l_index1");
	j2j[l_index2]=string("l_index2");
	j2j[l_index3]=string("l_index3");
	j2j[l_middle0]=string("l_middle0");
	j2j[l_middle1]=string("l_middle1");
	j2j[l_middle2]=string("l_middle2");
	j2j[l_middle3]=string("l_middle3");
	j2j[l_ring0]=string("l_ring0");
	j2j[l_ring1]=string("l_ring1");
	j2j[l_ring2]=string("l_ring2");
	j2j[l_ring3]=string("l_ring3");
	j2j[l_pinky0]=string("l_pinky0");
	j2j[l_pinky1]=string("l_pinky1");
	j2j[l_pinky2]=string("l_pinky2");
	j2j[l_pinky3]=string("l_pinky3");
	j2j[r_sternoclavicular]=string("r_sternoclavicular");
	j2j[r_acromioclavicular]=string("r_acromioclavicular");
	j2j[r_shoulder]=string("r_shoulder");
	j2j[r_elbow]=string("r_elbow");
	j2j[r_wrist]=string("r_wrist");
	j2j[r_thumb1]=string("r_thumb1");
	j2j[r_thumb2]=string("r_thumb2");
	j2j[r_thumb3]=string("r_thumb3");
	j2j[r_index0]=string("r_index0");
	j2j[r_index1]=string("r_index1");
	j2j[r_index2]=string("r_index2");
	j2j[r_index3]=string("r_index3");
	j2j[r_middle0]=string("r_middle0");
	j2j[r_middle1]=string("r_middle1");
	j2j[r_middle2]=string("r_middle2");
	j2j[r_middle3]=string("r_middle3");
	j2j[r_ring0]=string("r_ring0");
	j2j[r_ring1]=string("r_ring1");
	j2j[r_ring2]=string("r_ring2");
	j2j[r_ring3]=string("r_ring3");
	j2j[r_pinky0]=string("r_pinky0");
	j2j[r_pinky1]=string("r_pinky1");
	j2j[r_pinky2]=string("r_pinky2");
	j2j[r_pinky3]=string("r_pinky3");
	j2j[null_joint]=string("");

}

BAPConverter::~BAPConverter()
{

}

BAPType BAPConverter::MJointToBAP(string m)
{
	int i = 0;
	//new version when array s is filled:
	for (i=0; i<=NUMBAPS; i++) {
		if (m.compare(s[i])==0) return a[i];
	}
	return null_bap;
}

BAPType BAPConverter::DiGuyJointToBAP(string m) {
// this function needs to differentiate between the three BAP formats in use, too.
// commented out for now.
/*	
	if (m.compare("q.base_rx")==0) return Pelvic_roll;
	if (m.compare("q.base_ry")==0) return Pelvic_tilt;
	if (m.compare("q.base_rz")==0) return Pelvic_torsion;
	if (m.compare("q.base_tx")==0) return HumanoidRoot_tr_lateral;
	if (m.compare("q.base_ty")==0) return HumanoidRoot_tr_frontal;
	if (m.compare("q.base_tz")==0) return HumanoidRoot_tr_vertical;
	
	// TODO: base translation
	// TODO: map back,cervical
	
	//check axes
	if (m.compare("q.back_rx")==0) return l5roll;
	if (m.compare("q.back_ry")==0) return l5tilt;
	if (m.compare("q.back_rz")==0) return l5torsion;
	if (m.compare("q.cervical_rx")==0) return c3roll;
	if (m.compare("q.cervical_ry")==0) return c3tilt;
	if (m.compare("q.cervical_rz")==0) return c3torsion;

	//if (m.compare("vt9.rotateX")==0) return t9roll;
	//if (m.compare("vt9.rotateY")==0) return t9tilt;
	//if (m.compare("vt9.rotateZ")==0) return t9torsion;
	//if (m.compare("vc3.rotateX")==0) return c3roll;
	//if (m.compare("vc3.rotateY")==0) return c3tilt;
	//if (m.compare("vc3.rotateZ")==0) return c3torsion;

	if (m.compare("q.hip_r_rx")==0) return r_hip_abduct;
	if (m.compare("q.hip_l_rx")==0) return l_hip_abduct;
	if (m.compare("q.hip_r_ry")==0) return r_hip_flexion;
	if (m.compare("q.hip_l_ry")==0) return l_hip_flexion;
	if (m.compare("q.hip_r_rz")==0) return r_hip_twisting;
	if (m.compare("q.hip_l_rz")==0) return l_hip_twisting;

	if (m.compare("q.knee_r_rx")==0) return custom_r_knee_3rd;
	if (m.compare("q.knee_l_rx")==0) return custom_l_knee_3rd;
	if (m.compare("q.knee_r_ry")==0) return r_knee_flexion;
	if (m.compare("q.knee_l_ry")==0) return l_knee_flexion;
	if (m.compare("q.knee_r_rz")==0) return r_knee_twisting;
	if (m.compare("q.knee_l_rz")==0) return l_knee_twisting;

	if (m.compare("q.ankle_r_rx")==0) return custom_r_ankle_3rd;
	if (m.compare("q.ankle_l_rx")==0) return custom_l_ankle_3rd;
	if (m.compare("q.ankle_r_ry")==0) return r_ankle_flexion;
	if (m.compare("q.ankle_l_ry")==0) return l_ankle_flexion;
	if (m.compare("q.ankle_r_rz")==0) return r_ankle_twisting;
	if (m.compare("q.ankle_l_rz")==0) return l_ankle_twisting;
	
	if (m.compare("q.shoulder_l_ry")==0) return l_shoulder_flexion;
	if (m.compare("q.shoulder_r_ry")==0) return r_shoulder_flexion;
	if (m.compare("q.shoulder_l_rx")==0) return l_shoulder_abduct;
	if (m.compare("q.shoulder_r_rx")==0) return r_shoulder_abduct;
	if (m.compare("q.shoulder_l_rz")==0) return l_shoulder_twisting;
	if (m.compare("q.shoulder_r_rz")==0) return r_shoulder_twisting;
	
	if (m.compare("q.elbow_r_rx")==0) return custom_r_elbow_3rd;
	if (m.compare("q.elbow_l_rx")==0) return custom_l_elbow_3rd;
	if (m.compare("q.elbow_l_ry")==0) return l_elbow_flexion;
	if (m.compare("q.elbow_r_ry")==0) return r_elbow_flexion;
	if (m.compare("q.elbow_l_rz")==0) return l_elbow_twisting;
	if (m.compare("q.elbow_r_rz")==0) return r_elbow_twisting;
	if (m.compare("q.wrist_l_rx")==0) return l_wrist_flexion;
	if (m.compare("q.wrist_r_rx")==0) return r_wrist_flexion;
	if (m.compare("q.wrist_l_ry")==0) return l_wrist_pivot;
	if (m.compare("q.wrist_r_ry")==0) return r_wrist_pivot;
	if (m.compare("q.wrist_l_rz")==0) return l_wrist_twisting;
	if (m.compare("q.wrist_r_rz")==0) return r_wrist_twisting;
	
*/		
	return null_bap;
}


BAPType BAPConverter::IntToBAP(int i)
{
	return a[i];
}

BAPType BAPConverter::GetSideBAP(SidelessBAPType b, SideType s)
{
	if (s==l) return a[b];
	if (s==r) return a[b+1];
	return null_bap;
}

BAPType BAPConverter::BVHJointToBAP(string m)
{
	return(MJointToBAP(m));

}

int BAPConverter::AngleToInt(AngleType t, double f)
{
	if(t==degrees)
		return int((DTOR*f)*100000.0);
	else
		return int(f*100000.0);
		
}

double BAPConverter::IntToAngle(AngleType t, int i)
{
	if(t==degrees)
		return (double(i)/100000.0)*RTOD;
	else
		return double(i)/100000.0;
}

int BAPConverter::LengthToInt(LengthType t, double f)
{
	if(t==meters)
		return int(f*100000.0);
	else
		return int(f*1000.0);
		
}

double BAPConverter::IntToLength(LengthType t, int i)
{
	if(t==meters)
		return (double(i)/100000.0);
	else
		return double(i)/1000.0;
}

string BAPConverter::IntToMJoint(int i)
{
	return s[i];
}

joint_type BAPConverter::BAPToJointType(int i)
{
	return b2j[i];
}

joint_type BAPConverter::JointStringToJointType(string s)
{
	//CHRIS fix
	int i = 0;
	//new version when array s is filled:
	for (i=0; i<=NUMJOINTS; i++) {
		if (s.compare(j2j[i])==0) return joint_type(i);
	}
	return null_joint;
}

string BAPConverter::JointTypeToJointString(int i)
{
  return j2j[i];
}