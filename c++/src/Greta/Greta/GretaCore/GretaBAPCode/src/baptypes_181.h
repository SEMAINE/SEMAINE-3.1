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

#define NUMBAPS 181
//#define DEBUG 1

/**@#-*/
namespace GestureSpace { 
/**@#+*/
	
	/** different independent arm animation groups, correspond to enumeration SplineTypes */
	enum ArmGroupType {
		GROUP_SHOULDER,
		GROUP_ELBOW,
		GROUP_WRIST,
		GROUP_FINGERS, GROUP_ALL};

	/** List of BAPnames. Their order within the enumeration corresponds to their MPEG value
	*
	* NOTE: this is an outdated list of BAPs, the standard has changed in the meantime.
	* Renumbering/ expansion may be needed for proper export/exchange of BAP files.
	* This does not matter for the internal BAP Player 
	*/
	enum BAPType {
	null_bap,
	Pelvic_tilt,
	Pelvic_torsion,
	Pelvic_roll,
	l_hip_flexion ,
	r_hip_flexion ,
	l_hip_abduct  ,
	r_hip_abduct  ,
	l_hip_twisting ,
	r_hip_twisting ,
	l_knee_flexion ,
	r_knee_flexion ,
	l_knee_twisting ,
	r_knee_twisting ,
	l_ankle_flexion ,
	r_ankle_flexion ,
	l_ankle_twisting,
	r_ankle_twisting,
	l_subtalar_bap ,
	r_subtalar_bap ,
	l_mid_foot ,
	r_mid_foot ,
	l_toe_flexion ,
	r_toe_flexion ,
	l_clav_abduct ,
	r_clav_abduct ,
	l_clav_rotate ,
	r_clav_rotate ,
	l_scap_abduct ,
	r_scap_abduct ,
	l_scap_rotate ,
	r_scap_rotate ,
	l_shoulder_flexion ,
	r_shoulder_flexion ,
	l_shoulder_abduct ,
	r_shoulder_abduct ,
	l_shoulder_twisting ,
	r_shoulder_twisting ,
	l_elbow_flexion ,
	r_elbow_flexion ,
	l_elbow_twisting ,
	r_elbow_twisting ,
	l_wrist_flexion ,
	r_wrist_flexion ,
	l_wrist_pivot  ,
	r_wrist_pivot  ,
	l_wrist_twisting,
	r_wrist_twisting,
	l_pinky_flexion1,
	r_pinky_flexion1,
	l_pinky_abduct,
	r_pinky_abduct,
	l_pinky_twisting,
	r_pinky_twisting,
	l_pinky_flexion2,
	r_pinky_flexion2,
	l_pinky_flexion3,
	r_pinky_flexion3,
	l_ring_flexion1,
	r_ring_flexion1,
	l_ring_abduct,
	r_ring_abduct,
	l_ring_twisting,
	r_ring_twisting,
	l_ring_flexion2,
	r_ring_flexion2,
	l_ring_flexion3,
	r_ring_flexion3,
	l_middle_flexion1,
	r_middle_flexion1,
	l_middle_abduct,
	r_middle_abduct,
	l_middle_twisting,
	r_middle_twisting,
	l_middle_flexion2,
	r_middle_flexion2,
	l_middle_flexion3,
	r_middle_flexion3,
	l_index_flexion1,
	r_index_flexion1,
	l_index_abduct,
	r_index_abduct,
	l_index_twisting,
	r_index_twisting,
	l_index_flexion2,
	r_index_flexion2,
	l_index_flexion3,
	r_index_flexion3,
	l_thumb_flexion1,
	r_thumb_flexion1,
	l_thumb_abduct,
	r_thumb_abduct,
	l_thumb_twisting,
	r_thumb_twisting,
	l_thumb_flexion2,
	r_thumb_flexion2,
	l_thumb_flexion3,
	r_thumb_flexion3,
	c1roll,
	c1torsion,
	c1tilt,
	c2roll,
	c2torsion,
	c2tilt,
	c3roll,
	c3torsion,
	c3tilt,
	c4roll,
	c4torsion,
	c4tilt,
	c5roll,
	c5torsion,
	c5tilt,
	c6roll,
	c6torsion,
	c6tilt,
	c7roll,
	c7torsion,
	c7tilt,
	t1roll,
	t1torsion,
	t1tilt,
	t2roll,
	t2torsion,
	t2tilt,
	t3roll,
	t3torsion,
	t3tilt,
	t4roll,
	t4torsion,
	t4tilt,
	t5roll,
	t5torsion,
	t5tilt,
	t6roll,
	t6torsion,
	t6tilt,
	t7roll,
	t7torsion,
	t7tilt,
	t8roll,
	t8torsion,
	t8tilt,
	t9roll,
	t9torsion,
	t9tilt,
	t10roll,
	t10torsion,
	t10tilt,
	t11roll,
	t11torsion,
	t11tilt,
	t12roll,
	t12torsion,
	t12tilt,
	l1roll,
	l1torsion,
	l1tilt,
	l2roll,
	l2torsion,
	l2tilt,
	l3roll,
	l3torsion,
	l3tilt,
	l4roll,
	l4torsion,
	l4tilt,
	l5roll,
	l5torsion,
	l5tilt,
	HumanoidRoot_tr_vertical,
	HumanoidRoot_tr_lateral,
	HumanoidRoot_tr_frontal,
	HumanoidRoot_rt_turn,
	HumanoidRoot_rt_roll,
	HumanoidRoot_rt_tilt,
	custom_r_knee_3rd,		//3rd dof for 2-dof joints
	custom_l_knee_3rd,
	custom_r_elbow_3rd,
	custom_l_elbow_3rd,
	custom_r_ankle_3rd,
	custom_l_ankle_3rd};//ADDED 12/26/02

	
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
	vl5,
	vl4,
	vl3,
	vl2,
	vl1,
	vt12,
	vt11,
	vt10,
	vt9,
	vt8,
	vt7,
	vt6,
	vt5,
	vt4,
	vt3,
	vt2,
	vt1,
	vc7,
	vc6,
	vc5,
	vc4,
	vc3,
	vc2,
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
	
	/** For use with GesturePhase::TCBParams */
	enum TCBParamType {Tension,Continuity,Bias};

/**@#-*/
} // END NAMESPACE GESTURESPACE
/**@#+*/

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
