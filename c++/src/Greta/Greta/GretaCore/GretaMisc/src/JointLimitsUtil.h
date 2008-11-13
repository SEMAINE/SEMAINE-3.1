/**************************************************************************
 *
 *  Joint Limits Utility header file.
 *
 *  This file contains headers for utility functions needed by the joint
 *  limits system. We do not provide the actual funtions because it is
 *  expected that most people will already have these functions implemented
 *  in their own code. However, we do provide reference implementations for
 *  them in the comments, just in case.
 *
 **************************************************************************/

#ifndef JOINT_LIMITS_UTIL_H
#define JOINT_LIMITS_UTIL_H

#include "jointLimits.h"


#ifdef __cplusplus
extern "C" {
#endif

/* Range types. */

/**
 * Initialize the minimum and maximum Euler angles.
 
mm->mm[XMIN] = mm->mm[YMIN] = mm->mm[ZMIN ] = 1.e31;
mm->mm[XMAX] = mm->mm[YMAX] = mm->mm[ZMAX ] = -1.e31;

 */
void jtl_initMaxMin(Jtl_MaxMinPtr mm);



/***************** Matrix manipulation *************************/

/**
 * Vector dot Vector
 
return( v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2]);
 
 */
float jtl_vdv(float v1[3], float v2[3]);




/**
 * Vector cross Vector
 
v3[0] = v1[1] * v2[2] - v1[2] * v2[1];
v3[1] = v1[2] * v2[0] - v1[0] * v2[2];
v3[2] = v1[0] * v2[1] - v1[1] * v2[0];
 
 */
void jtl_vxv(float pt1[3], float pt2[3], float pt3[3]); 




/**
 * Vector plus Vector
 
v3[0] = v1[0] + v2[0];
v3[1] = v1[1] + v2[1];
v3[2] = v1[2] + v2[2];
 
 */
void jtl_vpv(float v1[3], float v2[3], float v3[3]);




/**
 * Copy v1 to v2 - 3 dimensions
 
v2[0] = v1[0];
v2[1] = v1[1];
v2[2] = v1[2];
 
 */
void jtl_cpv(float v1[3], float v2[3]);




/**
 * Copy v1 to v2 - 2 dimensions
 
v2[0] = v1[0];
v2[1] = v1[1];
 
 */
void jtl_cpv2(float v1[2], float v2[2]);




/**
 * Vector times a scaler
 
v2[0] = s * v1[0];
v2[1] = s * v1[1];
v2[2] = s * v1[2];
 
 */
void jtl_vxs(float v1[3], float s, float v2[3]);




/**
 * Vector minus Vector
 
v3[0] = v1[0] - v2[0];
v3[1] = v1[1] - v2[1];
v3[2] = v1[2] - v2[2];
 
 */
void jtl_vmv(float v1[3], float v2[3], float v3[3]);




/**
 * Average of two vectors
 
v3[0] = (v1[0]+v2[0])/2.0;
v3[1] = (v1[1]+v2[1])/2.0;
v3[2] = (v1[2]+v2[2])/2.0;
 
 */
void jtl_vave(float v1[3], float v2[3], float v3[3]);




/**
 * Triple scalar product
 *
 * vxvdv() is also known as ``triple scalar product'' and performs
 * (pt1 X pt2) dot pt3.  This is also the determinant of a 3x3 matrix
 * in which the rows are pt1, pt2, pt3 in that order.
 *
 * If the vertices of a triangle are 0 (origin), pt1 and pt2 in
 * counter-clockwise order, then vxvdv() is positive if and only if pt3
 * is on the same side of the plane of the triangle as the normal vector
 * points.  This may be faster than finding the plane equation first.
 *
 * If one vertex of a tetrahedron is at 0, and pt1, pt2, pt3 are the
 * other vertices in CLOCKwise order as viewed from 0, then vxvdv() / 6.0
 * is the volume.  For a paralleepiped, do not divide by 6.0.
 *
 
float   xprod[3];

jtl_vxv(pt1, pt2, xprod);
return ( jtl_vdv(xprod, pt3) );
 
 */
float jtl_vxvdv(float pt1[3], float pt2[3], float pt3[3]); 




/**
 * The length of a vector
 
float val;

val = (float)sqrt((double)( v[0]*v[0] + v[1]*v[1] + v[2]*v[2]));
return(val);
 
 */
float jtl_vlength(float v[3]);




/**
 * Rotates the z-axis of a given frame (given as a matrix) into
 * another vector (v) by rotating about an axis perpendicular to both,
 * and returns both the matrix that is the composite of this new
 * rotation and the frame rotation, and the zyx euler rotation that
 * the matrix codifies. This is usual, for example, if you want
 * to rotate a segment so that it aligns with another axis. 
 * assumes only rotation is important in the matrices.
 *
 * See below for additional helper functions.
 
float zaxis[4],vnew[4],n[4],mat[4][4],len;
float angle,cosangle,sinangle;
float result[3];
int   i,j;

/*** the zaxis is the third row of the matrix ***/
/*** should be safe to assume this is length 1 ***/
/*
zaxis[0] = framemat[2][0];
zaxis[1] = framemat[2][1];
zaxis[2] = framemat[2][2];
zaxis[3] = 1.0;
*/

/*** find angle between ***/
/*
len = normvlen(v,vnew);
vnew[3] = 1.0;

if (len==0) return;

cosangle = jtl_vdv(zaxis,v)/len;
*/

/*** acos not very smart ***/
/*
if (cosangle > 1.0) cosangle = 1.0;
if (cosangle < -1.0) cosangle = -1.0;
angle = (float)acos((double)cosangle);

sinangle = (float)sin((double)angle);
*/

/*** find perpendicular ***/
/*
find_perpaxis(zaxis,vnew,n); n[3] = 1.0;
*/

/*** find matrix that rotates zaxis into vnew ***/
/*
jtl_make_mat(n,cosangle,sinangle,mat);
*/

/*** find composite rotation of both matrices ***/
/*
jtl_multiplyfMatrices(newmat,mat,framemat);

get_zyx_rot_hint(newmat,x,y,z,hint);
 
 */
void jtl_rotztov(float framemat[4][4], float* v, float newmat[4][4],
  float *x, float *y, float *z, float hint[3]);


void get_zyx_rot_hint(float m_aafEntry[4][4],
					  float *rfZAngle,
					  float *rfYAngle,
					  float *rfXAngle,
					  float hint[3]);

/*** normalize a vector and return in second vector. also returns len
*/
float normvlen(float v1[3], float v2[3]);
/*{
float len;
len = (float)sqrt((double)(v1[0]*v1[0] + v1[1]*v1[1] + v1[2]*v1[2]));
if (len != 0)
  {
  v2[0] = v1[0]/len;
  v2[1] = v1[1]/len;
  v2[2] = v1[2]/len;
  }
return(len);
}
*/

/*** find a perpendicular axis to two vectors. Also normalizes it and checks
     lengths, unlike make_perp*/
    
int find_perpaxis(float u[4], float v[4], float n[3]);
/*{
float c,s,r,lenu,lenv;

lenu = jtl_vlength(u);
lenv = jtl_vlength(v);
if (lenu == 0.0 || lenv == 0.0)
  {
  fprintf(stderr,"find_axis() can't find perpendicular to 0 vector\n");
  return(0);
  }

jtl_vxv(u,v,n);

/*** if you can't find normal guess ***/
/*
if (n[0] == 0 && n[1] == 0 && n[2] == 0)
  {
    n[0] = 1.0; /*** try the x-axis ***/
/*    
    if (jtl_vdv(n,u) == 0)*/ /*** if u is the x-axis ***/
/*      {
      n[0] = 0.0;
      n[1] = 1.0;*/ /*** try the y-axis ***/
/*      }
  }
else
  jtl_normv(n,n);
  
return(1);
}

*/


/* normalize a vector and return in second vector*/

void jtl_normv(float v1[3], float v2[3]);
/*
{
float len;
len = (float)sqrt((double)(v1[0]*v1[0] + v1[1]*v1[1] + v1[2]*v1[2]));
v2[0] = v1[0]/len;
v2[1] = v1[1]/len;
v2[2] = v1[2]/len;
}

*/


/* given the sine and cosine of the desired angle, and an arbitrary
   rotation vector p, return the matrix that causes that amount of
   rotation around that axis p
   */
void jtl_make_mat(float p[3], float c, float s, float mat[4][4]);
/*{
mat[0][0] = p[0]*p[0] + c*(1 - p[0]*p[0]);
mat[1][0] = p[0]*p[1]*(1 - c) - p[2]*s;
mat[2][0] = p[2]*p[0]*(1 - c) + p[1]*s;
mat[3][0] = 0;

mat[0][1] = p[0]*p[1]*(1 - c) + p[2]*s;
mat[1][1] =  p[1]*p[1] + c*(1 - p[1]*p[1]);
mat[2][1] = p[1]*p[2]*(1 - c) - p[0]*s;
mat[3][1] = 0;

mat[0][2] = p[2]*p[0]*(1 - c) - p[1]*s;
mat[1][2] = p[1]*p[2]*(1 - c) + p[0]*s;
mat[2][2] =  p[2]*p[2] + c*(1 - p[2]*p[2]);
mat[3][2] = 0;

mat[0][3] = mat[1][3] = mat[2][3] = 0;
mat[3][3] = 1.0;
}

*/


/*** MATRIX type definition. 4 X 4 matrix!
*/
typedef float *MATRIX;
/**/

/*** Get a matrix element.

#define MATELEM(a, b, c) ((a)[MATDIM * (b) + (c)])

*/

/*** MultiplyMatrices: result = m1*m2
     result may be same matrix as either input
*/
void jtl_multiplyfMatrices (MATRIX result, MATRIX m1, MATRIX m2);
/*
{
	int i, j, k;

	for (i = 0; i < MATDIM; i++) 
		for (j = 0; j < MATDIM; j++)
			{
			MATELEM (tempMat, i, j) = 0.0;
			for (k = 0; k < MATDIM; k++)
				MATELEM (tempMat, i, j) += 
					MATELEM (m1, i, k) * MATELEM (m2, k, j);
			}

	copyMatrix(result, tempMat);
}

/*** copyMatrix: result = copy of m
*/
void copyMatrix (MATRIX result, MATRIX m);
/*{
int i, j;

	for (i = 0; i < MATDIM; i++)
		for (j = 0; j < MATDIM; j++)
			MATELEM (result, i, j) = MATELEM (m, i, j);
}
*/

/*
*
 * Get the (0,0,1) axis after given rotation
 
float cx,sx,cy;

cx = (float)cos((double)(DEG_TO_RAD*rot[0]));
cy = (float)cos((double)(DEG_TO_RAD*rot[1]));
sx = (float)sin((double)(DEG_TO_RAD*rot[0]));

axis[0] = (float)sin((double)(DEG_TO_RAD*rot[1]));;
axis[1] = (-cy*sx);
axis[2] = (cy*cx);

 */
void jtl_get_zaxis_rotated(float rot[3], float axis[3]);


/* Memory handling. */
/**
 * Vector dot Vector
 
return( v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2]);
 
 */
void *jtl_malloc(unsigned long num_bytes);

/**
 * Vector dot Vector
 
return( v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2]);
 
 */
void jtl_free(void *p, unsigned long num_bytes);

float b_computeAngle(float c, float s);

#ifdef __cplusplus
} //extern "C" 
#endif
#endif /* JOINT_LIMITS_UTIL_H */


/* $Id: jointLimitsUtil.h,v 1.1 2007/05/14 11:05:45 mancini Exp $*/
/*
 * $Log: jointLimitsUtil.h,v $
 * Revision 1.1  2007/05/14 11:05:45  mancini
 * *** empty log message ***
 *
 * Revision 1.1  2005/01/21 12:27:32  mancini
 * starting version
 *
 * Revision 1.1  2004/05/25 08:30:11  bjoern
 * joint limit module
 *
 * Revision 1.1  2001/05/04 04:05:29  mslater
 * Initial revision
 *
 */
