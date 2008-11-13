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

#ifndef BAPTYPES
#define BAPTYPES
// LIST of BAPs, acc. to w2068 p.22ff 

//////////////////////////////////////
// USEOLDBAPS: 
// define this parameter to use 169 BAPs
// otherwise, the engine will use the newer list of 186 BAPs
#define USEOLDBAPS

#ifndef NUMBAPS
#ifdef USEOLDBAPS
#define NUMBAPS 169
#else 
#define NUMBAPS 186
#endif
#endif

//#define DEBUG 1


/** different independent arm animation groups, correspond to enumeration SplineTypes */
enum ArmGroupType {
	GROUP_SHOULDER,
	GROUP_ELBOW,
	GROUP_WRIST,
	GROUP_FINGERS, GROUP_ALL};

#ifdef USEOLDBAPS
	/** List of BAPnames. Their order within the enumeration corresponds to their MPEG value
	*
	* NOTE: this is an outdated list of BAPs, the standard has changed in the meantime.
	* Renumbering/ expansion may be needed for proper export/exchange of BAP files.
	* This does not matter for the internal BAP Player 
	*/

	enum BAPType {
		null_bap,
		Pelvic_tilt, //1
		Pelvic_torsion,
		Pelvic_roll,
		l_hip_flexion ,
		r_hip_flexion ,
		l_hip_abduct  ,
		r_hip_abduct  ,
		l_hip_twisting ,
		r_hip_twisting ,
		l_knee_flexion , //10
		r_knee_flexion ,
		l_knee_twisting ,
		r_knee_twisting ,
		l_ankle_flexion ,
		r_ankle_flexion ,
		l_ankle_twisting,
		r_ankle_twisting,
		l_subtalar_flexion , //*
		r_subtalar_flexion , //*
		l_midtarsal_twisting , //20 //*
		r_midtarsal_twisting , //*
		l_metatarsal_flexion , //*
		r_metatarsal_flexion , //*
		l_clav_abduct ,
		r_clav_abduct ,
		l_clav_rotate ,
		r_clav_rotate ,
		l_scap_abduct ,
		r_scap_abduct ,
		l_scap_rotate , //30
		r_scap_rotate ,
		l_shoulder_flexion ,
		r_shoulder_flexion ,
		l_shoulder_abduct ,
		r_shoulder_abduct ,
		l_shoulder_twisting ,
		r_shoulder_twisting ,
		l_elbow_flexion ,
		r_elbow_flexion ,
		l_elbow_twisting , //40
		r_elbow_twisting ,
		l_wrist_flexion ,
		r_wrist_flexion ,
		l_wrist_pivot  ,
		r_wrist_pivot  ,
		l_wrist_twisting,
		r_wrist_twisting,
		l_pinky_flexion1,
		r_pinky_flexion1,
		l_pinky_abduct, //50
		r_pinky_abduct,
		l_pinky_twisting,
		r_pinky_twisting,
		l_pinky_flexion2,
		r_pinky_flexion2,
		l_pinky_flexion3,
		r_pinky_flexion3,
		l_ring_flexion1,
		r_ring_flexion1,
		l_ring_abduct,//60
		r_ring_abduct,
		l_ring_twisting,
		r_ring_twisting,
		l_ring_flexion2,
		r_ring_flexion2,
		l_ring_flexion3,
		r_ring_flexion3,
		l_middle_flexion1,
		r_middle_flexion1,
		l_middle_abduct,//70
		r_middle_abduct,
		l_middle_twisting,
		r_middle_twisting,
		l_middle_flexion2,
		r_middle_flexion2,
		l_middle_flexion3,
		r_middle_flexion3,
		l_index_flexion1,
		r_index_flexion1,
		l_index_abduct,//80
		r_index_abduct,
		l_index_twisting,
		r_index_twisting,
		l_index_flexion2,
		r_index_flexion2,
		l_index_flexion3,
		r_index_flexion3,
		l_thumb_flexion1,
		r_thumb_flexion1,
		l_thumb_abduct,//90
		r_thumb_abduct,
		l_thumb_twisting,
		r_thumb_twisting,
		l_thumb_flexion2,
		r_thumb_flexion2,
		l_thumb_flexion3,
		r_thumb_flexion3,
		c1roll,
		c1torsion,
		c1tilt,//100
		c2roll, //simple spine DOF vc8_roll	
		c2torsion, //simple spine DOF vc8_torsion
		c2tilt, //simple spine DOF vc8_tilt
		c3roll,
		c3torsion,
		c3tilt,
		c4roll, //simple spine DOF vc7_roll
		c4torsion, //simple spine DOF vc7_torsion
		c4tilt, //simple spine DOF vc7_tilt
		c5roll,//110
		c5torsion,
		c5tilt,
		c6roll,
		c6torsion,
		c6tilt,
		c7roll,
		c7torsion,
		c7tilt,
		t1roll, //simple spine DOF vt6_roll
		t1torsion, //simple spine DOF vt6_torsion //120
		t1tilt, //simple spine DOF vt6_tilt
		t2roll,
		t2torsion,
		t2tilt,
		t3roll,
		t3torsion,
		t3tilt,
		t4roll,
		t4torsion,
		t4tilt,//130
		t5roll,
		t5torsion,
		t5tilt,
		t6roll, //simple spine DOF vt5_roll
		t6torsion, 	//simple spine DOF vt5_torsion
		t6tilt,
		t7roll,
		t7torsion,
		t7tilt,
		t8roll,//140
		t8torsion,
		t8tilt,
		t9roll,
		t9torsion,
		t9tilt,
		t10roll, //simple spine DOF vt4_roll	
		t10torsion, //simple spine DOF vt4_torsion
		t10tilt, //simple spine DOF vt4_tilt
		t11roll,
		t11torsion,///150
		t11tilt,
		t12roll,
		t12torsion,
		t12tilt,
		l1roll, //simple spine DOF vl3_roll	
		l1torsion,
		l1tilt, //simple spine DOF vl3_tilt	
		l2roll,
		l2torsion,
		l2tilt,//160
		l3roll, //simple spine DOF vl2_roll
		l3torsion,
		l3tilt, //simple spine DOF vl2_tilt	
		l4roll,
		l4torsion,
		l4tilt,
		l5roll, 
		l5torsion,
		l5tilt}; //simple spine DOF vl1_tilt
#else 
	//new bap definition
	enum BAPType {
		null_bap,
		sacroiliac_tilt, //1 //*
		sacroiliac_torsion, //*
		sacroiliac_roll, //*
		l_hip_flexion ,
		r_hip_flexion ,
		l_hip_abduct  ,
		r_hip_abduct  ,
		l_hip_twisting ,
		r_hip_twisting ,
		l_knee_flexion , //10
		r_knee_flexion ,
		l_knee_twisting ,
		r_knee_twisting ,
		l_ankle_flexion ,
		r_ankle_flexion ,
		l_ankle_twisting,
		r_ankle_twisting,
		l_subtalar_flexion , //*
		r_subtalar_flexion , //*
		l_midtarsal_twisting , //20 //*
		r_midtarsal_twisting , //*
		l_metatarsal_flexion , //*
		r_metatarsal_flexion , //*
		l_sternoclavicular_abduct ,
		r_sternoclavicular_abduct ,
		l_sternoclavicular_rotate ,
		r_sternoclavicular_rotate ,
		l_acromioclavicular_abduct ,
		r_acromioclavicular_abduct ,
		l_acromioclavicular_rotate , //30
		r_acromioclavicular_rotate ,
		l_shoulder_flexion ,
		r_shoulder_flexion ,
		l_shoulder_abduct ,
		r_shoulder_abduct ,
		l_shoulder_twisting ,
		r_shoulder_twisting ,
		l_elbow_flexion ,
		r_elbow_flexion ,
		l_elbow_twisting , //40
		r_elbow_twisting ,
		l_wrist_flexion ,
		r_wrist_flexion ,
		l_wrist_pivot  ,
		r_wrist_pivot  ,
		l_wrist_twisting,
		r_wrist_twisting, //------------ below is different
		skullbase_roll,
		skullbase_torsion,
		skullbase_tilt, //50
		vc1_roll,
		vc1_torsion,
		vc1_tilt,
		vc2_roll, //simple spine DOF vc8_roll	
		vc2_torsion, //simple spine DOF vc8_torsion
		vc2_tilt, //simple spine DOF vc8_tilt
		vc3_roll,
		vc3_torsion,
		vc3_tilt,
		vc4_roll, //simple spine DOF vc7_roll //60
		vc4_torsion, //simple spine DOF vc7_torsion
		vc4_tilt, //simple spine DOF vc7_tilt
		vc5_roll,
		vc5_torsion,
		vc5_tilt,
		vc6_roll,
		vc6_torsion,
		vc6_tilt,
		vc7_roll,
		vc7_torsion, //70
		vc7_tilt,
		vt1_roll, //simple spine DOF vt6_roll
		vt1_torsion, //simple spine DOF vt6_torsion
		vt1_tilt, //simple spine DOF vt6_tilt
		vt2_roll,
		vt2_torsion,
		vt2_tilt,
		vt3_roll,
		vt3_torsion,
		vt3_tilt, //80
		vt4_roll,
		vt4_torsion,
		vt4_tilt,
		vt5_roll,
		vt5_torsion,
		vt5_tilt,
		vt6_roll, //simple spine DOF vt5_roll
		vt6_torsion, 	//simple spine DOF vt5_torsion
		vt6_tilt,
		vt7_roll,//90
		vt7_torsion,
		vt7_tilt,
		vt8_roll,
		vt8_torsion,
		vt8_tilt,
		vt9_roll,
		vt9_torsion,
		vt9_tilt,
		vt10_roll, //simple spine DOF vt4_roll	
		vt10_torsion, //simple spine DOF vt4_torsion //100
		vt10_tilt, //simple spine DOF vt4_tilt
		vt11_roll,
		vt11_torsion,
		vt11_tilt,
		vt12_roll,
		vt12_torsion,
		vt12_tilt,
		vl1_roll, //simple spine DOF vl3_roll	
		vl1_torsion,
		vl1_tilt, //simple spine DOF vl3_tilt	//110
		vl2_roll,
		vl2_torsion,
		vl2_tilt,
		vl3_roll, //simple spine DOF vl2_roll
		vl3_torsion,
		vl3_tilt, //simple spine DOF vl2_tilt	
		vl4_roll,
		vl4_torsion,
		vl4_tilt,
		vl5_roll, //120
		vl5_torsion,
		vl5_tilt,	
		l_pinky0_flexion,
		r_pinky0_flexion,
		l_pinky1_flexion, 
		r_pinky1_flexion,
		l_pinky1_pivot, 
		r_pinky1_pivot,
		l_pinky1_twisting,
		r_pinky1_twisting, //130
		l_pinky2_flexion,
		r_pinky2_flexion,
		l_pinky3_flexion,
		r_pinky3_flexion,
		l_ring0_flexion,
		r_ring0_flexion,
		l_ring1_flexion,
		r_ring1_flexion,
		l_ring1_pivot,
		r_ring1_pivot, //140
		l_ring1_twisting,
		r_ring1_twisting,
		l_ring2_flexion,
		r_ring2_flexion,
		l_ring3_flexion,
		r_ring3_flexion,
		l_middle0_flexion,
		r_middle0_flexion,
		l_middle1_flexion,
		r_middle1_flexion, //150
		l_middle1_pivot,
		r_middle1_pivot,
		l_middle1_twisting,
		r_middle1_twisting,
		l_middle2_flexion,
		r_middle2_flexion,
		l_middle3_flexion,
		r_middle3_flexion,
		l_index0_flexion,
		r_index0_flexion, //160
		l_index1_flexion,
		r_index1_flexion,
		l_index1_pivot,
		r_index1_pivot,
		l_index1_twisting,
		r_index1_twisting,
		l_index2_flexion,
		r_index2_flexion,
		l_index3_flexion,
		r_index3_flexion, //170
		l_thumb1_flexion,
		r_thumb1_flexion,
		l_thumb1_pivot,
		r_thumb1_pivot,
		l_thumb1_twisting,
		r_thumb1_twisting,
		l_thumb2_flexion,
		r_thumb2_flexion,
		l_thumb3_flexion,
		r_thumb3_flexion, //180
		HumanoidRoot_tr_vertical,
		HumanoidRoot_tr_lateral,
		HumanoidRoot_tr_frontal,
		HumanoidRoot_rt_body_turn,
		HumanoidRoot_rt_body_roll,
		HumanoidRoot_rt_body_tilt
	};
#endif 

#ifdef USEOLDBAPS
	/** BAPTypes without a side specification (NOT standardized, my own invention)
	*
	* Using a simple conversion routine, a SidelessBAPType and a SideType
	* uniquely define a BAPType.
	*/

	enum SidelessBAPType{
		clav_abduct = int(l_clav_abduct),
		clav_rotate = int(l_clav_rotate),
		scap_abduct = int(l_scap_abduct),
		scap_rotate = int(l_scap_rotate ),
		shoulder_flexion = int(l_shoulder_flexion ),
		shoulder_abduct = int(l_shoulder_abduct ),
		shoulder_twisting = int(l_shoulder_twisting ),
		elbow_flexion = int(l_elbow_flexion ),
		elbow_twisting = int(l_elbow_twisting ),
		wrist_flexion = int(l_wrist_flexion ),
		wrist_pivot  = int(l_wrist_pivot  ),
		wrist_twisting= int(l_wrist_twisting),
		pinky_flexion1= int(l_pinky_flexion1),
		pinky_abduct= int(l_pinky_abduct),
		pinky_twisting= int(l_pinky_twisting),
		pinky_flexion2= int(l_pinky_flexion2),
		pinky_flexion3= int(l_pinky_flexion3),
		ring_flexion1= int(l_ring_flexion1),
		ring_abduct= int(l_ring_abduct),
		ring_twisting= int(l_ring_twisting),
		ring_flexion2= int(l_ring_flexion2),
		ring_flexion3= int(l_ring_flexion3),
		middle_flexion1= int(l_middle_flexion1),
		middle_abduct= int(l_middle_abduct),
		middle_twisting= int(l_middle_twisting),
		middle_flexion2= int(l_middle_flexion2),
		middle_flexion3= int(l_middle_flexion3),
		index_flexion1= int(l_index_flexion1),
		index_abduct= int(l_index_abduct),
		index_twisting= int(l_index_twisting),
		index_flexion2= int(l_index_flexion2),
		index_flexion3= int(l_index_flexion3),
		thumb_flexion1= int(l_thumb_flexion1),
		thumb_abduct= int(l_thumb_abduct),
		thumb_twisting= int(l_thumb_twisting),
		thumb_flexion2= int(l_thumb_flexion2),
		thumb_flexion3= int(l_thumb_flexion3),
	};

#else 
	//new baps:
	enum SidelessBAPType{
		clav_abduct = int(l_sternoclavicular_abduct),
		clav_rotate = int(l_sternoclavicular_rotate),
		scap_abduct = int(l_acromioclavicular_abduct),
		scap_rotate = int(l_acromioclavicular_rotate ),
		shoulder_flexion = int(l_shoulder_flexion ),
		shoulder_abduct = int(l_shoulder_abduct ),
		shoulder_twisting = int(l_shoulder_twisting ),
		elbow_flexion = int(l_elbow_flexion ),
		elbow_twisting = int(l_elbow_twisting ),
		wrist_flexion = int(l_wrist_flexion ),
		wrist_pivot  = int(l_wrist_pivot  ),
		wrist_twisting= int(l_wrist_twisting),
		pinky_flexion1= int(l_pinky1_flexion),
		pinky_abduct= int(l_pinky1_pivot),
		pinky_twisting= int(l_pinky1_twisting),
		pinky_flexion2= int(l_pinky2_flexion),
		pinky_flexion3= int(l_pinky3_flexion),
		ring_flexion1= int(l_ring1_flexion),
		ring_abduct= int(l_ring1_pivot),
		ring_twisting= int(l_ring1_twisting),
		ring_flexion2= int(l_ring2_flexion),
		ring_flexion3= int(l_ring3_flexion),
		middle_flexion1= int(l_middle1_flexion),
		middle_abduct= int(l_middle1_pivot),
		middle_twisting= int(l_middle1_twisting),
		middle_flexion2= int(l_middle2_flexion),
		middle_flexion3= int(l_middle3_flexion),
		index_flexion1= int(l_index1_flexion),
		index_abduct= int(l_index1_pivot),
		index_twisting= int(l_index1_twisting),
		index_flexion2= int(l_index2_flexion),
		index_flexion3= int(l_index3_flexion),
		thumb_flexion1= int(l_thumb1_flexion),
		thumb_abduct= int(l_thumb1_pivot),
		thumb_twisting= int(l_thumb1_twisting),
		thumb_flexion2= int(l_thumb2_flexion),
		thumb_flexion3= int(l_thumb3_flexion)
	};
#endif
	/** mapping from simplified spine DOFs to normal BAPs */
	/*enum SimpleSpineBAPType {
	vl1_tilt= int(l5tilt),
	vl2_roll= int(l3roll),
	vl2_tilt= int(l3tilt),
	vl3_roll= int(l1roll),
	vl3_tilt= int(l1tilt),
	vt4_roll= int(t10roll),
	vt4_torsion= int(t10torsion),
	vt4_tilt= int(t10tilt),
	vt5_roll= int(t6roll),
	vt5_torsion= int(t6torsion),
	vt6_roll= int(t1roll),
	vt6_torsion= int(t1torsion),
	vt6_tilt= int(t1tilt),
	vc7_roll= int(c4roll),
	vc7_torsion= int(c4torsion),
	vc7_tilt= int(c4tilt),
	vc8_roll= int(c2roll),
	vc8_torsion= int(c2torsion),
	vc8_tilt= int(c2tilt)
	};*/

	/** Joint names acc. to H-Anim specification (version 1.0? not sure) */
#define NUMJOINTS 89
	enum joint_type {l_hip,
		l_knee,
		l_ankle,
		l_subtalar,
		l_midtarsal,
		l_metatarsal,
		r_hip,
		r_knee,
		r_ankle,
		r_subtalar,
		r_midtarsal,
		r_metatarsal,
		HumanoidRoot,
		sacroiliac,
		vl5, // named vl1 in simplified spine
		vl4,
		vl3, // named vl2 in simplified spine
		vl2,
		vl1, // named vl3 in simplified spine
		vt12,
		vt11,
		vt10, // named vt4 in simplified spine
		vt9,
		vt8,
		vt7,
		vt6, // named vt5 in simplified spine
		vt5,
		vt4,
		vt3,
		vt2,
		vt1, // named vt6 in simplified spine
		vc7,
		vc6,
		vc5,
		vc4, // named vc7 in simplified spine
		vc3,
		vc2, // named vc8 in simplified spine
		vc1,
		skullbase,
		l_eyeball_joint,
		r_eyeball_joint,
		l_sternoclavicular,
		l_acromioclavicular,
		l_shoulder,
		l_elbow,
		l_wrist,
		l_thumb1,
		l_thumb2,
		l_thumb3,
		l_index0,
		l_index1,
		l_index2,
		l_index3,
		l_middle0,
		l_middle1,
		l_middle2,
		l_middle3,
		l_ring0,
		l_ring1,
		l_ring2,
		l_ring3,
		l_pinky0,
		l_pinky1,
		l_pinky2,
		l_pinky3,
		r_sternoclavicular,
		r_acromioclavicular,
		r_shoulder,
		r_elbow,
		r_wrist,
		r_thumb1,
		r_thumb2,
		r_thumb3,
		r_index0,
		r_index1,
		r_index2,
		r_index3,
		r_middle0,
		r_middle1,
		r_middle2,
		r_middle3,
		r_ring0,
		r_ring1,
		r_ring2,
		r_ring3,
		r_pinky0,
		r_pinky1,
		r_pinky2,
		r_pinky3,
		null_joint};

	/** NO LONGER USED */
	enum AxisType {x_axis, y_axis, z_axis,null_axis};

	/** For use with conversion functions in class BAP */
	enum AngleType {degrees, radians};

	/** For use with conversion functions in class BAPconverter */
	enum LengthType {meters,centimeters};

	/** Used in almost every class to specify sides */
	enum SideType {r,l,no_side,both_sides,assym};

#ifndef TCBPARAMS
#define TCBPARAMS
	/** For use with GesturePhase::TCBParams */
	enum TCBParamType {Tension,Continuity,Bias};
#endif

	/**@#-*/
	// NEVER USED
	namespace BasicLocation {
		enum {upper,central,lower,left,right,close,distant};
	}

	// NEVER USED
	namespace BasicDirection {
		enum {neutral, up, down, left, right, towards, away};
	}
	// NEVER USED
	namespace Axis {
		enum {x,y,z};
	}
	/**@#+*/

#ifndef RTOD
	/** Radians-to-Degrees multiplication constant */
	const double RTOD = 57.29578;
	/** Degrees-to-Radians multiplication constant */
	const double DTOR = 0.017453293;
#endif

#endif
