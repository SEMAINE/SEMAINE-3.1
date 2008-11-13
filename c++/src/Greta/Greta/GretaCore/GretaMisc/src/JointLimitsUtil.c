/**************************************************************************
 *
 *  Joint Limits Utility implementation - derived from comments in headern file
 *
 *  This file contains headers for utility functions needed by the joint
 *  limits system. We do not provide the actual funtions because it is
 *  expected that most people will already have these functions implemented
 *  in their own code. However, we do provide reference implementations for
 *  them in the comments, just in case.
 *
 **************************************************************************/


#ifndef M_PI
#define M_PI            3.14159265358979323846
#endif
#ifndef DTOR
#define DTOR(x) ((x)*M_PI/180.0)
#endif
#ifndef RTOD
#define RTOD(x) ((x)*180.0/M_PI)
#endif

#include "jointLimits.h"
#include "jointLimitsUtil.h"
#include <math.h>
#include <malloc.h>
#include <stdio.h>
/* Range types. */

/**
 * Initialize the minimum and maximum Euler angles.
 */
void jtl_initMaxMin(Jtl_MaxMinPtr mm) {
mm->mm[JTL_XMIN] = mm->mm[JTL_YMIN] = mm->mm[JTL_ZMIN ] = 1.e31;
mm->mm[JTL_XMAX] = mm->mm[JTL_YMAX] = mm->mm[JTL_ZMAX ] = -1.e31;
}

//void jtl_normv(float v1[3], float v2[3]);

/***************** Matrix manipulation *************************/

/**
 * Vector dot Vector
 */
float jtl_vdv(float v1[3], float v2[3]) {
	return( v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2]);
}




/**
 * Vector cross Vector
 */
void jtl_vxv(float pt1[3], float pt2[3], float pt3[3])
{
	pt3[0] = pt1[1] * pt2[2] - pt1[2] * pt2[1];
	pt3[1] = pt1[2] * pt2[0] - pt1[0] * pt2[2];
	pt3[2] = pt1[0] * pt2[1] - pt1[1] * pt2[0];
}



/**
 * Vector plus Vector
 */
void jtl_vpv(float v1[3], float v2[3], float v3[3])
{
	v3[0] = v1[0] + v2[0];
	v3[1] = v1[1] + v2[1];
	v3[2] = v1[2] + v2[2];
}





/**
 * Copy v1 to v2 - 3 dimensions
 */
void jtl_cpv(float v1[3], float v2[3]) {
	v2[0] = v1[0];
	v2[1] = v1[1];
	v2[2] = v1[2];
}




/**
 * Copy v1 to v2 - 2 dimensions 
 */
void jtl_cpv2(float v1[2], float v2[2]) {
	v2[0] = v1[0];
	v2[1] = v1[1];
}




/**
 * Vector times a scaler
  
 */
void jtl_vxs(float v1[3], float s, float v2[3]) 
{
	v2[0] = s * v1[0];
	v2[1] = s * v1[1];
	v2[2] = s * v1[2];
}




/**
 * Vector minus Vector
 */
void jtl_vmv(float v1[3], float v2[3], float v3[3]){
	v3[0] = v1[0] - v2[0];
	v3[1] = v1[1] - v2[1];
	v3[2] = v1[2] - v2[2];
}



/**
 * Average of two vectors
 */
void jtl_vave(float v1[3], float v2[3], float v3[3]) {
	v3[0] = (v1[0]+v2[0])/2.0;
	v3[1] = (v1[1]+v2[1])/2.0;
	v3[2] = (v1[2]+v2[2])/2.0;
}





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
 
 
 */
float jtl_vxvdv(float pt1[3], float pt2[3], float pt3[3]) {
	float   xprod[3];
	jtl_vxv(pt1, pt2, xprod);
	return ( jtl_vdv(xprod, pt3) );
}




/**
 * The length of a vector
 
 
 */
float jtl_vlength(float v[3]) 
{
	float val;

	val = (float)sqrt((double)( v[0]*v[0] + v[1]*v[1] + v[2]*v[2]));
	return(val);
}




/**
 * Rotates the z-axis of a given frame (given as a matrix) into
 * another vector (v) by rotating about an axis perpendicular to both,
 * and returns both the matrix that is the composite of this new
 * rotation and the frame rotation, and the zyx euler rotation that
 * the matrix codifies. This is usual, for example, if you want
 * to rotate a segment so that it aligns with another axis. 
 * assumes only rotation is important in the matrices.
 *
 * See below for additional helper functions.*/
void jtl_rotztov(float framemat[4][4], float* v, float newmat[4][4],
				 float *x, float *y, float *z, float hint[3]) {

float zaxis[4],vnew[4],n[4],mat[4][4],len;
float angle,cosangle,sinangle;
float cross[3];
/*** the zaxis is the third row of the matrix ***/
/*** should be safe to assume this is length 1 ***/

zaxis[0] = framemat[2][0];
zaxis[1] = framemat[2][1];
zaxis[2] = framemat[2][2];
zaxis[3] = 1.0;


/*** find angle between ***/

len = normvlen(v,vnew);
vnew[3] = 1.0;

if (len==0) return;



/*** acos not very smart ***/

// THINK THIS IS THE SECTION THAT CAUSES JUMPS IN ROTATIONS
/*
cosangle = jtl_vdv(zaxis,v)/len;

if (cosangle > 1.0) cosangle = 1.0;
if (cosangle < -1.0) cosangle = -1.0;
angle = (float)acos((double)cosangle);

sinangle = (float)sin((double)angle);

*/

cosangle = jtl_vdv(zaxis,v)/len;

jtl_vxv(zaxis,v,cross);
sinangle = jtl_vlength(cross)/len;
angle=b_computeAngle(cosangle,sinangle);


/*** find perpendicular ***/

find_perpaxis(zaxis,vnew,n); n[3] = 1.0;


/*** find matrix that rotates zaxis into vnew ***/

jtl_make_mat(n,cosangle,sinangle,mat);


/*** find composite rotation of both matrices ***/

jtl_multiplyfMatrices(newmat,mat,framemat);

//NOT FINISHED HERE!
//EXTRACT euler angles zyx based on hint. HOW?
get_zyx_rot_hint(newmat,z,y,x,hint);
}

void get_zyx_rot_hint(float m_aafEntry[4][4],
					  float *rfZAngle,
					  float *rfYAngle,
					  float *rfXAngle,
					  float hint[3]) {
    
	if ( m_aafEntry[2][0] < 1.0f )
    {
        if ( m_aafEntry[2][0] > -1.0f )
        {
            *rfXAngle = (float)RTOD(atan2(-m_aafEntry[2][1],m_aafEntry[2][2]));
            *rfYAngle = (float)RTOD(asin(m_aafEntry[2][0]));
            *rfZAngle = (float)RTOD(atan2(-m_aafEntry[1][0],m_aafEntry[0][0]));
            return;// true;
        }
        else
        {
            // WARNING.  Not unique.  XA - ZA = -atan2(r10,r11)
            *rfXAngle = (float)RTOD(-atan2(m_aafEntry[0][1],m_aafEntry[1][1]));
            *rfYAngle = (float)RTOD(-M_PI/2.0f);
            *rfZAngle = 0.0f;
            return;//  false;
        }
    }
    else
    {
        // WARNING.  Not unique.  XAngle + ZAngle = atan2(r10,r11)
        *rfXAngle = (float)RTOD(atan2(m_aafEntry[0][1],m_aafEntry[1][1]));
        *rfYAngle = (float)RTOD(M_PI/2.0f);
        *rfZAngle = 0.0f;
        return;//  false;
    }

						  /*
						  //from FreeMagicSDK: need to flip indices most likely!
// rot =  cy*cz           cz*sx*sy-cx*sz  cx*cz*sy+sx*sz
//        cy*sz           cx*cz+sx*sy*sz -cz*sx+cx*sy*sz
//       -sy              cy*sx           cx*cy

    if ( m_aafEntry[2][0] < 1.0f )
    {
        if ( m_aafEntry[2][0] > -1.0f )
        {
            *rfZAngle = (float)RTOD(atan2(m_aafEntry[1][0],m_aafEntry[0][0]));
            *rfYAngle = (float)RTOD(asin(-m_aafEntry[2][0]));
            *rfXAngle = (float)RTOD(atan2(m_aafEntry[2][1],m_aafEntry[2][2]));
            return;//true;
        }
        else
        {
            // WARNING.  Not unique.  ZA - XA = -atan2(r01,r02)
            *rfZAngle = (float)RTOD(-atan2(m_aafEntry[0][1],m_aafEntry[0][2]));
            *rfYAngle = RTOD(M_PI/2.0f);
            *rfXAngle = 0.0f;
            return;// false;
        }
    }
    else
    {
        // WARNING.  Not unique.  ZA + XA = atan2(-r01,-r02)
        *rfZAngle = (float)RTOD(atan2(-m_aafEntry[0][1],-m_aafEntry[0][2]));
        *rfYAngle = RTOD(-M_PI/2.0f);
        *rfXAngle = 0.0f;
        return;// false;
    }*/
}
/*
bool Matrix3::ToEulerAnglesZYX (float& rfZAngle, float& rfYAngle,
    float& rfXAngle) const
{
    // rot =  cy*cz           cz*sx*sy-cx*sz  cx*cz*sy+sx*sz
    //        cy*sz           cx*cz+sx*sy*sz -cz*sx+cx*sy*sz
    //       -sy              cy*sx           cx*cy

    if ( m_aafEntry[2][0] < 1.0f )
    {
        if ( m_aafEntry[2][0] > -1.0f )
        {
            rfZAngle = atan2f(m_aafEntry[1][0],m_aafEntry[0][0]);
            rfYAngle = asinf(-m_aafEntry[2][0]);
            rfXAngle = atan2f(m_aafEntry[2][1],m_aafEntry[2][2]);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  ZA - XA = -atan2(r01,r02)
            rfZAngle = -atan2f(m_aafEntry[0][1],m_aafEntry[0][2]);
            rfYAngle = Math::HALF_PI;
            rfXAngle = 0.0f;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  ZA + XA = atan2(-r01,-r02)
        rfZAngle = atan2f(-m_aafEntry[0][1],-m_aafEntry[0][2]);
        rfYAngle = -Math::HALF_PI;
        rfXAngle = 0.0f;
        return false;
    }
}
bool Matrix3::ToEulerAnglesXYZ (float& rfXAngle, float& rfYAngle,
    float& rfZAngle) const
{
    // rot =  cy*cz          -cy*sz           sy
    //        cz*sx*sy+cx*sz  cx*cz-sx*sy*sz -cy*sx
    //       -cx*cz*sy+sx*sz  cz*sx+cx*sy*sz  cx*cy

    if ( m_aafEntry[0][2] < 1.0f )
    {
        if ( m_aafEntry[0][2] > -1.0f )
        {
            rfXAngle = atan2f(-m_aafEntry[1][2],m_aafEntry[2][2]);
            rfYAngle = asinf(m_aafEntry[0][2]);
            rfZAngle = atan2f(-m_aafEntry[0][1],m_aafEntry[0][0]);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  XA - ZA = -atan2(r10,r11)
            rfXAngle = -atan2f(m_aafEntry[1][0],m_aafEntry[1][1]);
            rfYAngle = -Math::HALF_PI;
            rfZAngle = 0.0f;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  XAngle + ZAngle = atan2(r10,r11)
        rfXAngle = atan2f(m_aafEntry[1][0],m_aafEntry[1][1]);
        rfYAngle = Math::HALF_PI;
        rfZAngle = 0.0f;
        return false;
    }
}
*/

/*** normalize a vector and return in second vector. also returns len */

float normvlen(float v1[3], float v2[3])
{
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


/*** find a perpendicular axis to two vectors. Also normalizes it and checks
     lengths, unlike make_perp */
    
int find_perpaxis(float u[4], float v[4], float n[3])
{
float lenu,lenv;

lenu = jtl_vlength(u);
lenv = jtl_vlength(v);
if (lenu == 0.0 || lenv == 0.0)
  {
  fprintf(stderr,"find_axis() can't find perpendicular to 0 vector\n");
  return(0);
  }

jtl_vxv(u,v,n);

/*** if you can't find normal guess ***/

if (n[0] == 0 && n[1] == 0 && n[2] == 0)
  {
    n[0] = 1.0; /*** try the x-axis ***/
    
    if (jtl_vdv(n,u) == 0) /*** if u is the x-axis ***/
      {
      n[0] = 0.0;
      n[1] = 1.0; /*** try the y-axis ***/
      }
  }
else
  jtl_normv(n,n);
  
return(1);
}



/* normalize a vector and return in second vector */

void jtl_normv(float v1[3], float v2[3])
{
float len;
len = (float)sqrt((double)(v1[0]*v1[0] + v1[1]*v1[1] + v1[2]*v1[2]));
v2[0] = v1[0]/len;
v2[1] = v1[1]/len;
v2[2] = v1[2]/len;
}



/* given the sine and cosine of the desired angle, and an arbitrary
   rotation vector p, return the matrix that causes that amount of
   rotation around that axis p */
   
void jtl_make_mat(float p[3], float c, float s, float mat[4][4])
{
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




/*** MATRIX type definition. 4 X 4 matrix!*/

/*** Get a matrix element.*/
#define MATDIM 4
#define MATELEM(a, b, c) ((a)[MATDIM * (b) + (c)])



/*** MultiplyMatrices: result = m1*m2
     result may be same matrix as either input*/

void jtl_multiplyfMatrices (MATRIX result, MATRIX m1, MATRIX m2)
{
	int i, j, k;
	MATRIX tempMat=(float *)malloc(MATDIM*MATDIM*sizeof(float));
	for (i = 0; i < MATDIM; i++) 
		for (j = 0; j < MATDIM; j++)
			{
			MATELEM (tempMat, i, j) = 0.0;
			for (k = 0; k < MATDIM; k++)
				MATELEM (tempMat, i, j) += 
					MATELEM (m1, i, k) * MATELEM (m2, k, j);
			}
	//ERROR: lines were reversed!
	copyMatrix(result, tempMat);
	free(tempMat);
}

/*** copyMatrix: result = copy of m */

void copyMatrix (MATRIX result, MATRIX m)
{
int i, j;

	for (i = 0; i < MATDIM; i++)
		for (j = 0; j < MATDIM; j++)
			MATELEM (result, i, j) = MATELEM (m, i, j);
}


/*
*
 * Get the (0,0,1) axis after given rotation
 * TODO: check when angle is 180
 * appended by bjoern 2004-05-07 to account for near zero results.
 */
void jtl_get_zaxis_rotated(float rot[3], float axis[3]) {
	float cx,sx,cy;
	float r0,r1;
	float err;
	err = 0.1f;
	r0=rot[0];
	r1=rot[1];

	if(fabs(fabs(r1)-180.0f)<err) {
		r1=180.f+err;
	} else if(fabs(r1)<err) {
		r1=err;
	}

	if(fabs(fabs(r0)-180.0f)<err) {
		r0=180.f+err;
	} else if(fabs(r0)<err) {
		r0=err;
	}

	cx = (float)cos((double)(DTOR(r0)));
	cy = (float)cos((double)(DTOR(r1)));
	sx = (float)sin((double)(DTOR(r0)));
	
	axis[0] = (float)sin((double)(DTOR(r1)));
	axis[1] = (-cy*sx);
	axis[2] = (cy*cx);

}


/* Memory handling. */
void *jtl_malloc(unsigned long num_bytes) {
	return malloc(num_bytes);
}

void jtl_free(void *p, unsigned long num_bytes) {
	free(p);
}

///////////////////////////////////////////////////////////////////////////////
// Function:	computeAngle
// Purpose:		compute an angle from its cosin and sin value
// Arguments:	c and s are the cosin and sin value of the angle, respectively.
///////////////////////////////////////////////////////////////////////////////
float b_computeAngle(float c, float s)
{
	float angle;
	
	if (c >= 0.0f && s >= 0.0f) 
		angle = (float) (asin(s) * 180.0 / M_PI);
	else if (c >= 0.0f && s <= 0.0f)
		angle = (float) (asin(s) * 180.0 / M_PI);
	else if (c <= 0.0f && s >= 0.0f)
		angle = (float) (acos(c) * 180.0 / M_PI);
	else
		angle = (float) (360 - acos(c) * 180.0 / M_PI);

	return angle;
}



/* $Id: jointLimitsUtil.c,v 1.1 2007/05/14 11:05:45 mancini Exp $*/
/*
 * $Log: jointLimitsUtil.c,v $
 * Revision 1.1  2007/05/14 11:05:45  mancini
 * *** empty log message ***
 *
 * Revision 1.2  2007/02/01 11:57:15  hirschberg
 * remove some unused variables
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
