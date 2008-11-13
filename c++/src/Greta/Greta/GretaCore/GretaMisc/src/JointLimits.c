#include <stdio.h>
#include <math.h>

#include "jointLimits.h"
#include "jointLimitsUtil.h"

/***************************************************************************
 *** Local constants.
 ***************************************************************************/
#define CLOSE_ENUF 0.0001
#define WKSARRAYCHUNK 10 /* size of basic allocations of array */

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/***************************************************************************
 *** Local function prototypes.
 ***************************************************************************/
static int check_wks(Jtl_WorkSpacePtr wks, float newPt[3], float okPt[3],
  float bary[3] );

static int check_angle(float angle, float min, float max, float *okAngle);
static int check_wksz(Jtl_WorkSpacePtr wks, float *okRotZ, float bary[3]);

static int isIntersect(Jtl_WorkSpacePtr wks, float nowpt[3], float newpt[3],
  float intsec[3], int slice, float bary[3]);

static int linePlane(float pt1[3], float pt2[3], float plane[3],
  float inter[3]);

static void updateVisiblePoint(Jtl_LimitPtr jtl);
static void calculateJtNormals(Jtl_LimitPtr jtl);
static void recalculateVisiblePoint(Jtl_LimitPtr jtl);

static int isCounterClockwise(float p1[3],float p2[3],float p3[3]);

static void subdivide_wks_points(float pp[3], float p1[3], float p2[3],
  float fp[3], int numPoints, float v[3]);


/****************************************************************************/
/* See description in jointLimits.h.                                        */
void jtl_init(Jtl_LimitPtr jtl)
{
jtl_initMaxMin(&(jtl->xyzlimits));

jtl->xyzlimits.mm[JTL_XMIN] = jtl->xyzlimits.mm[JTL_YMIN] =
  jtl->xyzlimits.mm[JTL_ZMIN] = -360.0;
jtl->xyzlimits.mm[JTL_XMAX] = jtl->xyzlimits.mm[JTL_YMAX] =
  jtl->xyzlimits.mm[JTL_ZMAX] = 360.0;
  
jtl->wks.visiblept[0]=jtl->wks.visiblept[1]=jtl->wks.visiblept[2]= 0.0;
jtl->wks.vpt_zlim[0] = -360.0;
jtl->wks.vpt_zlim[1] = 360.0;
jtl->wks.pts = NULL;
jtl->wks.n = NULL;
jtl->wks.zlim = NULL;
jtl->wks.numpts = 0;
jtl->wks.sizearray = 0;
jtl->wks.inslice = 0; /* slices are numbered from 1 */
jtl->wks.activept = 0;

jtl->active = 0;
jtl->userDefVisPt = FALSE;
}


/****************************************************************************/
/* See description in jointLimits.h.                                        */
int jtl_validate(Jtl_LimitPtr jtl)
{
int valid = TRUE;
int i;

if (jtl == NULL)
  {
  fprintf(stderr, "Cannot validate NULL joint limits.\n");
  valid = FALSE;
  }
else if (jtl->wks.numpts < 3)
  {
  printf("There are less than three boundary points in the workspace.\n");
  valid = FALSE;
  }
else
  {
  for (i = 1; i < jtl->wks.numpts; ++i)
    {
    if (!isCounterClockwise(
        jtl->wks.visiblept, jtl->wks.pts[i-1], jtl->wks.pts[i]))
      {
      printf("The points at indices %d and %d are not clockwise.\n", i-1, i);
      valid = FALSE;
      }
    }
  }

return(valid);
}

/****************************************************************************/
/* Set the workspace active point. */
void jtl_setActivePoint(Jtl_LimitPtr jtl, int pointIndex)
{
/* Enforce active point wraparound. If the new point is larger than the
   number of points, set the new active point to zero. If the new point is
   less than zero, set it to the number of points. If the new point is
   >= zero and <= the number of points, use it as the new active point. */
if (pointIndex > jtl->wks.numpts)
  {
  jtl->wks.activept = 0;
  }
else if (pointIndex < 0)
  {
  jtl->wks.activept = jtl->wks.numpts;
  }
else
  {
  jtl->wks.activept = pointIndex;
  }
}


/****************************************************************************/
/* See description in jointLimits.h.                                        */
int jtl_getActivePoint(Jtl_LimitPtr jtl)
{
return(jtl->wks.activept);
}


/****************************************************************************/
/* See description in jointLimits.h.                                        */
int jtl_setAllowableFlag(Jtl_LimitPtr jtl, int flag)
{
int setAllowed = FALSE;

switch(flag)
  {
  case JTL_X:
  case JTL_Y:
    {
    /* Euler X and Y can only be active if the workspace is not. */
    if(!jtl_isAllowableFlagSet(jtl, JTL_WKS))
      {
      setAllowed = TRUE;
      }
    break;
    }
    
  case JTL_Z:
    {
    /* Euler Z can only be active if the workspace z is not. */
    if(!jtl_isAllowableFlagSet(jtl, JTL_WKSZ))
      {
      setAllowed = TRUE;
      }
    break;
    }
    
  case JTL_WKS:
    {
    setAllowed = TRUE;
    
    /* When the workspace is active, euler x and y can not be. */
    jtl_unsetAllowableFlag(jtl, JTL_X);
    jtl_unsetAllowableFlag(jtl, JTL_Y);
    break;
    }
  
  case JTL_WKSZ:
    {
    setAllowed = TRUE;
    
    /* When the workspace z is active, euler z can not be. */
    jtl_unsetAllowableFlag(jtl, JTL_Z);
    break;
    }
    
  default:
    {
    fprintf(stderr, "Unable to set invalid flag (%d).\n", flag);
    break;
    }
  }

if(setAllowed)
  {
  jtl->active = jtl->active | flag;
  }

return(setAllowed);
}


/****************************************************************************/
/* See description in jointLimits.h.                                        */
void jtl_unsetAllowableFlag(Jtl_LimitPtr jtl, int flag)
{
if((flag == JTL_X) || (flag == JTL_Y) || (flag == JTL_Z) ||
    (flag == JTL_WKS) || (flag == JTL_WKSZ))
  {
  jtl->active = jtl->active & ~flag;
  }
else
  {
  fprintf(stderr, "Unable to unset invalid flag.\n");
  }
}


/****************************************************************************/
/* Returns TRUE if the specified point is set. */
int jtl_isAllowableFlagSet(Jtl_LimitPtr jtl, int flag)
{
return(jtl->active & flag);
}


/****************************************************************************/
/* See description in jointLimits.h.                                        */
/* See description in jointLimits.h.                                        */
float jtl_getMinMaxSetting(Jtl_LimitPtr jtl, int valueFlag)
{
if((valueFlag != JTL_XMIN) && (valueFlag != JTL_XMAX) &&
    (valueFlag != JTL_YMIN) && (valueFlag != JTL_YMAX) &&
    (valueFlag != JTL_ZMIN) && (valueFlag != JTL_ZMAX))
  {
  fprintf(stderr, "Unable to get setting for invalid min/max value flag.");
  return(0);
  }

return(jtl->xyzlimits.mm[valueFlag]);
}

/****************************************************************************/
/* See description in jointLimits.h.                                        */
void jtl_setMinMaxSetting(Jtl_LimitPtr jtl, int valueFlag, float value)
{
if((valueFlag != JTL_XMIN) && (valueFlag != JTL_XMAX) &&
    (valueFlag != JTL_YMIN) && (valueFlag != JTL_YMAX) &&
    (valueFlag != JTL_ZMIN) && (valueFlag != JTL_ZMAX))
  {
  fprintf(stderr, "Unable to get setting for invalid min/max value flag.");
  }
else
  {
  jtl->xyzlimits.mm[valueFlag] = value;
  }
}

/****************************************************************************/
/* See description in jointLimits.h.                                        */
void jtl_setMinMax(Jtl_LimitPtr jtl, float xmin, float xmax, float ymin,
  float ymax, float zmin, float zmax)
{
jtl->xyzlimits.mm[JTL_XMIN] = xmin;
jtl->xyzlimits.mm[JTL_XMAX] = xmax;

jtl->xyzlimits.mm[JTL_YMIN] = ymin;
jtl->xyzlimits.mm[JTL_YMAX] = ymax;

jtl->xyzlimits.mm[JTL_ZMIN] = zmin;
jtl->xyzlimits.mm[JTL_ZMAX] = zmax;
}


/****************************************************************************/
/* See description in jointLimits.h.                                        */
void jtl_setVisiblePoint(Jtl_LimitPtr jtl, float newVP[3])
{
Jtl_WorkSpacePtr wks = &(jtl->wks);

if((newVP == NULL) && (wks->numpts != 0)) /* auto set vispt */
  {
  jtl->userDefVisPt = FALSE;
  recalculateVisiblePoint(jtl);
  }
else  /* user defined vispt */
  {
  jtl->userDefVisPt = TRUE;
  jtl_cpv(newVP, wks->visiblept);
  }
  
/* when visiblept changes, recalculate normal vectors for all pts */
calculateJtNormals(jtl);
}

/****************************************************************************/
/* See description in jointLimits.h.                                        */
void jtl_addPoint(Jtl_LimitPtr jtl, float newPt[3])
{
	float twst[2]={-360.f,360.f};
	jtl_addPoint_withTwist(jtl,newPt,twst);
}

/****************************************************************************/

void jtl_addPoint_withTwist(Jtl_LimitPtr jtl, float newPt[3], float twst[2]) 
{
int i,newsize;
Jtl_WorkSpacePtr wks;
Jtl_Vector3D *newPoints,*newNormals;
Jtl_Vector2D *newZLimits;

if (newPt == NULL)
  {
  fprintf(stderr, "Could not add null point.\n");
  }

wks = &(jtl->wks);

if ((wks->pts == NULL)||(wks->sizearray == 0))
  {
  wks->pts = (Jtl_Vector3D *) jtl_malloc(sizeof(Jtl_Vector3D)*WKSARRAYCHUNK);
  wks->n = (Jtl_Vector3D *) jtl_malloc(sizeof(Jtl_Vector3D)*WKSARRAYCHUNK);
  
  /* for zlim, it has to have one more than numpts,for visiblept */
  wks->zlim = (Jtl_Vector2D *) jtl_malloc(sizeof(Jtl_Vector2D)*(WKSARRAYCHUNK+1));
  wks->sizearray = WKSARRAYCHUNK;
  }

/* if there isn't enough room in array, allocate array of size
   (WKSARRAYCHUNK + old size) and move items to new space. */
if (wks->numpts == wks->sizearray-1)
  {       
  /* Allocate new space for the point data. */
  newsize = wks->sizearray + WKSARRAYCHUNK;
  newPoints = (Jtl_Vector3D *) jtl_malloc(sizeof(Jtl_Vector3D)*newsize);
  newNormals = (Jtl_Vector3D *) jtl_malloc(sizeof(Jtl_Vector3D)*newsize);
  newZLimits = (Jtl_Vector2D *) jtl_malloc(sizeof(Jtl_Vector2D)*(newsize+1));
  
  /* Copy the existing point data to the new array. */
  for (i=0; i<wks->sizearray; i++)
    {
    jtl_cpv(wks->pts[i],newPoints[i]);
    jtl_cpv(wks->n[i],newNormals[i]);
    jtl_cpv2(wks->zlim[i],newZLimits[i]);
    }
    
  jtl_cpv2(wks->zlim[wks->sizearray],newZLimits[wks->sizearray]);
  
  /* Free the old point data. */
  jtl_free(wks->pts,sizeof(Jtl_Vector3D)*wks->sizearray);
  jtl_free(wks->n,sizeof(Jtl_Vector3D)*wks->sizearray);
  jtl_free(wks->zlim,sizeof(Jtl_Vector2D)*(wks->sizearray+1));
  
  /* Put the new point data into the workspace. */ 
  wks->sizearray = newsize;
  
  wks->pts = newPoints;
  wks->n = newNormals;
  wks->zlim = newZLimits;
  }

/*always add after the active pt. the active pt should normally be
the last point added. The active pt should usually be the last pt added*/

/* don't move around if activept is visiblept */
for (i=wks->numpts; i>wks->activept+1 && wks->activept < wks->numpts; i--)
  {
  jtl_cpv(wks->pts[i-1],wks->pts[i]);
  jtl_cpv(wks->n[i-1],wks->n[i]);
  jtl_cpv2(wks->zlim[i-1],wks->zlim[i]);
  }

/* initial value of activept is zero.When add the first pt,active 
   remain zero, but when add more, active pt should be incremented*/
if ((wks->numpts > 0) && (wks->activept < wks->numpts ))
  {
  wks->activept = wks->activept+1;
  }

wks->numpts++;

if (wks->activept < wks->numpts)
  {
  jtl_cpv(newPt,wks->pts[wks->activept]);
  }
  
wks->zlim[wks->activept][0] = twst[0]; /* setting open joint limit */
wks->zlim[wks->activept][1] = twst[1];

if ((jtl->userDefVisPt == FALSE) && (wks->numpts > 0)) /* auto set vispt*/
  {
  recalculateVisiblePoint(jtl);
  
  /* when visiblept changes, recalculate normal vectors for all pts */
  calculateJtNormals(jtl);
  }
/* if user defined vis pt, only calculate the normal vec for the new added pt */
else if ((jtl->userDefVisPt == TRUE) && (wks->numpts > 0)) 
  {
  jtl_vxv(wks->visiblept,wks->pts[wks->activept],wks->n[wks->activept]);    
  }
}

/****************************************************************************/
/* See description in jointLimits.h.                                        */
void jtl_deleteActivePoint(Jtl_LimitPtr jtl)
{
int index;
int i,newsize;
float d;
Jtl_WorkSpacePtr wks;
Jtl_Vector3D *temp,*tempn;
Jtl_Vector2D *tempz;

if (jtl->wks.activept == jtl->wks.numpts)
  { 
  fprintf(stderr, "Can't cut the centroid.\n");
  return;
  }
  
/* if there is one or more than one point left and the active
   pt is not visiblept,we can cut the activept*/
if (jtl->wks.numpts > 0)
  {
  index = jtl->wks.activept;

  wks = &(jtl->wks);

  /* move points from position index forwards overriding the points before them */
  for (i=index; i<wks->numpts-1; i++)
    {
    jtl_cpv(wks->pts[i+1],wks->pts[i]);
    jtl_cpv(wks->n[i+1],wks->n[i]);
    jtl_cpv2(wks->zlim[i+1],wks->zlim[i]);
    }
    
  wks->numpts--;
  
  /*after cut this pt, the active point is set as the one after this point*/
  /*if there is no pt left after this cut, set activept as 0 */
  if (wks->numpts > 0)
    {
    wks->activept = wks->activept%wks->numpts;
    }
  else
    {
    wks->activept = 0;
    }
  
  /*if the number of points afterward is 10 spaces less than the size of the array,
    reallocate an array of 10 less spaces and copy the pts over and jtl_free
    the larger array.*/
  if ((wks->numpts == wks->sizearray - WKSARRAYCHUNK)&&(wks->numpts > 0))
    {
    newsize = wks->sizearray - WKSARRAYCHUNK;    
    temp = (Jtl_Vector3D *) jtl_malloc(sizeof(Jtl_Vector3D)*newsize);
    tempn = (Jtl_Vector3D *) jtl_malloc(sizeof(Jtl_Vector3D)*newsize);
    tempz = (Jtl_Vector2D *) jtl_malloc(sizeof(Jtl_Vector2D)*(newsize+1));

    for (i=0; i<newsize; i++)
      {
      jtl_cpv(wks->pts[i],temp[i]);
      jtl_cpv(wks->n[i],tempn[i]);
      jtl_cpv2(wks->zlim[i],tempz[i]);
      }
    
    /* this is for visiblept's zlim */
    jtl_cpv2(wks->zlim[newsize],tempz[newsize]);

    jtl_free(wks->pts,sizeof(Jtl_Vector3D)*wks->sizearray);
    jtl_free(wks->n,sizeof(Jtl_Vector3D)*wks->sizearray);
    jtl_free(wks->zlim,sizeof(Jtl_Vector2D)*(wks->sizearray+1));

    wks->pts = (Jtl_Vector3D *) jtl_malloc(sizeof(Jtl_Vector3D)*newsize);
    wks->n = (Jtl_Vector3D *) jtl_malloc(sizeof(Jtl_Vector3D)*newsize);
    wks->zlim = (Jtl_Vector2D *) jtl_malloc(sizeof(Jtl_Vector2D)*(newsize+1));

    for (i=0; i<newsize; i++)
      {
      jtl_cpv(temp[i],wks->pts[i]);
      jtl_cpv(tempn[i],wks->n[i]);
      jtl_cpv2(tempz[i],wks->zlim[i]);
      }
    jtl_cpv2(tempz[newsize],wks->zlim[newsize]);

    jtl_free(temp,sizeof(Jtl_Vector3D)*newsize);
    jtl_free(tempn,sizeof(Jtl_Vector3D)*newsize);
    jtl_free(tempz,sizeof(Jtl_Vector2D)*(newsize+1));

    wks->sizearray = newsize;
    }

  /* Recalculate the visible point if needed. */
  if ((jtl->userDefVisPt == FALSE) && (wks->numpts > 0)) 
    {
      recalculateVisiblePoint(jtl);
    }

  /* recalculate normal vectors for all pts */
  calculateJtNormals(jtl);
  }
}

/****************************************************************************/
/* See description in jointLimits.h.                                        */
void jtl_deleteAllPoints(Jtl_LimitPtr jtl)
{
Jtl_WorkSpacePtr wks;

wks = &(jtl->wks);

if (wks->numpts > 0)
  {
  jtl_free(wks->pts,sizeof(Jtl_Vector3D)*wks->sizearray);
  jtl_free(wks->n,sizeof(Jtl_Vector3D)*wks->sizearray);
  jtl_free(wks->zlim,sizeof(Jtl_Vector2D)*(wks->sizearray+1));
  
  jtl_init(jtl);
  }
}

/****************************************************************************/
/* See description in jointLimits.h.                                        */
void jtl_subdivide(Jtl_LimitPtr jtl)
{
int i,oldsize=-1;
float d;
Jtl_Vector3D *newpts,*newn;
Jtl_Vector2D *newzlim;
Jtl_WorkSpacePtr wks;

if (jtl == NULL)
{
  fprintf(stderr,"cannot subdivide null joint limits\n");
  return;
}

wks = &(jtl->wks);

if (wks->numpts < 3)
  {
  fprintf(stderr,"cannot subdivide workspace of less than 3 points\n");
  return;
  }

d = jtl_vlength(wks->visiblept);

/* we're doubling size, make array bigger if necessary */
oldsize = wks->sizearray;
if (wks->sizearray < 2*wks->numpts)
  { 
  wks->sizearray *= 2;
  }

newpts = (Jtl_Vector3D *) jtl_malloc(sizeof(Jtl_Vector3D)*wks->sizearray);
newn = (Jtl_Vector3D *) jtl_malloc(sizeof(Jtl_Vector3D)*wks->sizearray);
newzlim = (Jtl_Vector2D *) jtl_malloc(sizeof(Jtl_Vector2D)*(wks->sizearray+1));

/* find averages and copy over */
for (i=0; i < wks->numpts; i++)
  {
  d = jtl_vlength(wks->pts[i]);
  jtl_cpv(wks->pts[i],newpts[i*2]); /* copy present point etc. */
  jtl_cpv(wks->n[i],newn[i*2]); 
  newzlim[i*2][0] = wks->zlim[i][0];
  newzlim[i*2][1] = wks->zlim[i][1];
  
  subdivide_wks_points(
    wks->pts[(i-1+wks->numpts) % wks->numpts],
    wks->pts[i],
    wks->pts[(i+1) % wks->numpts],
    wks->pts[(i+2) % wks->numpts],
    wks->numpts,
    newpts[i*2+1] );
  
  /* find new normal */
  jtl_vxv(wks->visiblept,newpts[i*2+1],newn[i*2+1]);
  
   /* give default zlim - interpolated between the two existing points. */
  newzlim[i*2+1][0] = wks->zlim[i][0] +
    ( ( wks->zlim[(i+1) % wks->numpts][0] - wks->zlim[i][0] ) / 2 );
  newzlim[i*2+1][1] = wks->zlim[i][1] +
    ( ( wks->zlim[(i+1) % wks->numpts][1] - wks->zlim[i][1] ) / 2 );
  }

/* free old space and move new space to old pointers */
jtl_free(wks->pts,sizeof(Jtl_Vector3D)*oldsize);
jtl_free(wks->n,sizeof(Jtl_Vector3D)*oldsize);
jtl_free(wks->zlim,sizeof(Jtl_Vector2D)*oldsize);

wks->pts = newpts;
wks->n = newn;
wks->zlim = newzlim;
wks->numpts *= 2;
}

/*******************************************************************************/
/* Assume there are k boundary points and you want to interpolate k more.
 * In the projected plane, say they are w_0, ... w_{k-1},
 * To put a new vertex between w_i and w_{i+1} use this formula
 * 
 * w = (w_i + w_{i+1}) / 2 + A*(w_{i+1} - w_{i-1}) + A*(w_i - w_{i+2})
 * 
 * with [] it is
 * 
 * w = (w[i] + w[i+1]) / 2 + A*(w[i+1] - w[i-1]) + A*(w[i] - w[i+2])
 * 
 * where A is scalar that depends on k.  Notice that we use l_interp, vmv,
 * vxs, and vpv to compute this.
 * 
 * If k==3, A = 1/6                = .1667.
 * If k==4, A = (sqrt(2) - 1)/4    = .1036.
 * If k==5, A =                      .0850.
 * If k==6, A = (4*sqrt(3) - 6)/12 = .0773.
 * If k==7, A =                      .0700.
 * If k >= 8, A = 1/16             = .0625.
 *
 *
 * In the following descriptions, i is the current index in the list of points. The
 * new point is begin added between the points at i and i+1 in the original list.
 *    pp - The point that originally came prior to the point that that is before the
 *         new point (i - 1).
 *    p1 - Point before the new point (i).
 *    p2 - Point after the new point (i + 1).
 *    fp - The point that was originally after the point after the new point (i + 2).
 *    numPoints - The number of points in the workspace.
 *    v - The new point.
 */
static void subdivide_wks_points(float pp[3], float p1[3], float p2[3],
  float fp[3], int numPoints, float v[3])
{
float tempVec[3]; /* A vector to store temporary results. */

/* First calculate a smoothing factor. By default, use 1/16. */
float smoothFactor = 1.0f / 16.0f;

switch( numPoints )
  {
  /* Since a workspace must have a minimum of 3 points, ignore case 1 and 2. */
  case 3:
    smoothFactor = 1.0 / 6.0;
    break;
      
  case 4:
    smoothFactor = ( sqrt( 2.0 ) - 1.0 ) / 4.0;
    break;
  
  case 5:
    smoothFactor = 0.0850f;
    break;
  
  case 6:
    smoothFactor = ( 4.0 * sqrt( 3.0 ) - 6.0 ) / 12.0;
    break;
  
  case 7:
    smoothFactor = 0.0700f;
    break;
  
  default:
    /* Use the default value given by the initializer. */
    break;
  }

/* Now calculate the new vector according to the following equation:
 * v = ((p1 + p2) / 2) + (smoothFactor * (p2 - pp)) + 
 *         (smoothFactor * (p1 - fp)) */

jtl_vave( p1, p2, v ); /* (p1 + p2) / 2 */

jtl_vmv( p2, pp, tempVec ); /* p2 - pp */
jtl_vxs( tempVec, smoothFactor, tempVec ); /* smoothFactor * (p2 - pp) */

jtl_vpv( v, tempVec, v ); /* v = ((p1 + p2) / 2) + (smoothFactor * (p2 - pp)) */

jtl_vmv( p1, fp, tempVec ); /* p1 - fp */
jtl_vxs( tempVec, smoothFactor, tempVec ); /* smoothFactor * (p1 - fp) */

jtl_vpv( v, tempVec, v ); /* completed equation. */
}

/****************************************************************************/
/*  Checks to see if the given angle is within its min/max range. If the
    angle is valid, it is returned in okAngle and the function returns TRUE.
    If the angle is outside the range, the closest range boundary is returned
    in okAngle and the function returns FALSE. */
static int check_angle(float angle, float min, float max, float *okAngle)
{
int angleInRange = TRUE;

/* If the angle is less than the minimum range, return the minimum range. */
if (angle < min)
  {
  angleInRange = FALSE;
  *okAngle = min;
  }
/* If the angle is more than the maximum range, return the maximum range. */
else if (angle > max)
  {
  angleInRange = FALSE;
  *okAngle = max;
  }
/* If the angle is within the range, return the angle. */
else
  {
  *okAngle = angle;
  }

return (angleInRange);
}


/****************************************************************************/
/* See description in jointLimits.h.                                        */
Jtl_Allowable jtl_allowablePosition(Jtl_LimitPtr jtl,
  float matrix[4][4], float rotation[3], float newPt[3], float newZRot,
  float okPt[3], float *okZRot, int intersection)
{
int oki, okz;
Jtl_Allowable ok = JTL_OK;
float inter[3], bary[3];

//added: 
okz=0;

/* make sure newPt and okPt are valid */
if (okPt == NULL)
  {
  fprintf(stderr, "Cannot return in null point.\n");
  return(JTL_FAIL_NO_INTERSECT);
  }

if (newPt == NULL)
  {
  fprintf(stderr, "could not examine null point\n");
  
  /* safe since we already checked okPt. */
  jtl_cpv(matrix[2], okPt);
  
  return(JTL_FAIL_NO_INTERSECT);
  }

/* check workspace */
if (jtl->active & JTL_WKS)
  {
  if (jtl->wks.numpts >= 3)
    { /* only consider a triangle or more a valid workspace for now */
    /* this ok is 0 for not ok and 1 for ok */
    if (ok = check_wks(&jtl->wks,newPt,okPt,bary)) 
      {
      jtl_cpv(newPt, okPt);
      }
    else /* new position is not in range */
      {
      if (intersection)
        { /* calculate the intersection,returns to inter below*/
        oki = isIntersect(&(jtl->wks), matrix[2], newPt, inter,
          -jtl->wks.inslice, bary); 

        if (!oki) /* bad intersect */
          {
          fprintf(stderr,"pts no intersect slice %d\n",jtl->wks.inslice);
          
          /* No intersection was found, use the old values. */
          ok = JTL_FAIL_NO_INTERSECT;
          jtl_cpv(matrix[2], okPt);
          }
        else /* good intersection found */
          {
          jtl_cpv(inter,okPt);
          ok = JTL_FAIL_WITH_INTERSECT;
          }
        }
      else  /* do not care intersection */
        {  /* stay where it was */
        jtl_cpv(matrix[2],okPt);
        }
      }
    } /* end if not a valid workspace of 3 or more pts*/
  } /* end workspace check */

/* if an okZRot was specified, check zlimit */
if (okZRot != NULL)
  {
  /* Only check the z workspace if both it and the overall workspace are
     active. */
  if ((jtl->active & JTL_WKSZ) && (jtl->active & JTL_WKS))
    {
    /* If the intersection has already failed and/or is not requested,
       there is no point in checking the z workspace now. */
    if (ok != JTL_FAIL_NO_INTERSECT)
      {
      okz = check_wksz(&(jtl->wks), okZRot, bary);
      }
    else /* Return the current rotation. */
      {
      *okZRot = rotation[2];
      }
    }
  /* If the Z Euler angle is active, check it. Note that the Z Euler angle
     cannot be active when the workspace Z is active. */
  else if (jtl->active & JTL_Z)
    {
    /* If the angle is within the axis range, it is valid and used.
       Otherwise, the range boundary closest to the proposed angle is used. */
    okz = check_angle(newZRot, jtl->xyzlimits.mm[JTL_ZMIN],
      jtl->xyzlimits.mm[JTL_ZMAX], okZRot);
    }
  /* If no Z checks are active, the new rotation is always fine. */
  else
    {
    *okZRot = newZRot;
    }
  
  /* If the z angle was not okay, then the result is not okay. */
  if (!okz)
    {
    /* If an intersection was requested, it has already been found. */
    if (intersection)
      {
      ok = JTL_FAIL_WITH_INTERSECT;
      }
    else /* If no intersection was requested, return the current rotation. */
      {
      ok = JTL_FAIL_NO_INTERSECT;
      *okZRot = rotation[2];
      }
    }
  }
  
return(ok);
}


/****************************************************************************/
/* See description in jointLimits.h.                                        */
Jtl_Allowable jtl_allowableRotation(Jtl_LimitPtr jtl, float matrix[4][4],
  float rotation[3], float newRot[3], float okRot[3], int intersection)
{
int okx,oky,okz;
Jtl_Allowable ok = JTL_OK;
float axis[3],okAxis[3],newmat[4][4];
float okZRot;

// added:
okZRot=newRot[2];

if (okRot == NULL)
  {
  fprintf(stderr, "could not return in null angle\n");
  return(JTL_FAIL_NO_INTERSECT);
  }

if (newRot == NULL)
  {
  fprintf(stderr, "could not examine null angle\n");
  
  /* safe since we already checked okRot. */
  jtl_cpv(rotation, okRot);
  
  return(JTL_FAIL_NO_INTERSECT);
  }

/* By default, assume the angles are fine. */
okx = oky = okz = TRUE;

/* check workspace */
if (jtl->active & JTL_WKS)
  {
  if (jtl->wks.numpts >= 3)
    { /* only consider a triangle or more a valid workspace for now */
    jtl_get_zaxis_rotated(newRot,axis);
    
    ok = jtl_allowablePosition(jtl, matrix, rotation, axis, newRot[2],
      okAxis, &okZRot, intersection);
    
    switch( ok )
      {
      case JTL_FAIL_NO_INTERSECT:
        {
        /* Either the intersection was not requested or it was not
           found. Return the current rotation. */
        jtl_cpv(rotation, okRot);
        break;
        }
        
      case JTL_FAIL_WITH_INTERSECT:
        {
        /* The new rotation was not allowed, but an intersection
           was returned. Find the rotation and return it. */
        jtl_rotztov(matrix, okAxis, newmat, &okRot[0], &okRot[1],
          &okRot[2], rotation);
        
        /* If the workspace Z is active, use the z angle returned by
           jtl_allowablePosition() because it has been checked against the
           valid range. Otherwise, use the current rotation. */
        if (jtl->active & JTL_WKSZ)
          {
          okRot[2] = okZRot;
          }
        else
          {
          okRot[2] = rotation[2];
          }
        break;
        }
        
      case JTL_OK:
        {
        /* The new rotation is fine so return it. */
        jtl_cpv(newRot, okRot);
        break;
        }
        
      default:
        {
        /* An invalid value was returned. This should never happen. */
        fprintf(stderr, "Unexpected runtime error - Could not determine rotation validity.\n");
        break;
        }
      }
    } /* end if not a valid workspace of 3 or more pts*/
  }
else /* x, y, and z can only be active if workspace is not */
  {
  /* Check each active angle. If the axis is active, check the proposed angle
     against the range for that axis. If an axis is not active, the proposed
     angle for that axis is always okay. */
  if (jtl->active & JTL_X)
    {
    /* If the angle is within the axis range, it is valid and returned.
       Otherwise, the range boundary closest to the proposed angle is returned. */
    okx = check_angle(newRot[0], jtl->xyzlimits.mm[JTL_XMIN],
        jtl->xyzlimits.mm[JTL_XMAX], &okRot[0]);
    }
  else
    {
    okRot[0] = newRot[0];
    }

  if (jtl->active & JTL_Y)
    {
    /* If the angle is within the axis range, it is valid and returned.
       Otherwise, the range boundary closest to the proposed angle is returned. */
    oky = check_angle(newRot[1], jtl->xyzlimits.mm[JTL_YMIN],
        jtl->xyzlimits.mm[JTL_YMAX], &okRot[1]);
    }
  else
    {
    okRot[1] = newRot[1];
    }

  if (jtl->active & JTL_Z)
    {
    /* If the angle is within the axis range, it is valid and returned.
       Otherwise, the range boundary closest to the proposed angle is returned. */
    okz = check_angle(newRot[2], jtl->xyzlimits.mm[JTL_ZMIN],
      jtl->xyzlimits.mm[JTL_ZMAX], &okRot[2]);
    }
  else
    {
    okRot[2] = newRot[2];
    }
  
  /* If any of the angles were not okay, then the result is not okay. */
  if (!okx || !oky || !okz)
    {
    /* If an intersection was requested, it has already been found. */
    if (intersection)
      {
      ok = JTL_FAIL_WITH_INTERSECT;
      }
    else /* If no intersection was requested, return the current rotation. */
      {
      ok = JTL_FAIL_NO_INTERSECT;
      jtl_cpv(rotation, okRot);
      }
    }
  } /* checking x, y,, and z instead of workspace */

return(ok);
}


/**************************************************************************/
/* checks whether workspace is ok, and returns true or false. 
   it is checking the axis (0,0,1) (state) in the default coordinate frame. 
   bary returns triple cross products if need for barycentric coord. 
   also resets wks->inslice to the positive slice number for the new
   point if the point is inside the wks, or the negative slice number if
   the point is outside the wks. remember slices count from 1, not 0. */
static int check_wks(Jtl_WorkSpacePtr wks, float newpt[3], float okpt[3],
    float bary[3])
{
int find,slice,times;
float side,next_side,firstside; 
float plane[3];

/* Find which slice contain the present point */ 
if (wks->inslice > 0) /* was calculated before, use that to start */
  slice = wks->inslice;
else
  slice = 1; 
find = FALSE;

/* by starting with previous, you only need to do each plane once */
firstside = side = jtl_vdv(newpt,wks->n[slice -1]);

times = 0;
while ((times < wks->numpts) && (!find))
    {
    /* if finish a round trip among the jtlimit points */ 
    if (times == wks->numpts-1)
      { /* remembered answer from first test */
      next_side = firstside;
      }
    else
      {	
      next_side = jtl_vdv(newpt,wks->n[slice%wks->numpts]);
      }         

    /* if positive, newpt is on the same side as normal vec of the plane */
    /* if negative, newrot is on the opposite side of the plane */
    /* if zero, newrot is on the plane */
    if (side >= 0 && next_side < 0)
      {
      find = TRUE;
      bary[0] = -next_side; /* scaled barycentric coordinates */
      bary[1] = side; 
      }
    else 
      {
      if (slice < wks->numpts)
	slice++;
      else /* real slice must be in range 1 to numpts */
        slice = (slice + 1)%wks->numpts;
      side = next_side;
      }
    times++;
    }
    
/* this should never happen, if we see it  */
if (!find)
  {
  fprintf(stderr,"jtl loc never found, last slice %d\n",slice);
  wks->inslice = 0;
  return(FALSE);
  }

/* now test against final edge */
if (slice == wks->numpts)
  jtl_vxv(wks->pts[slice-1],wks->pts[0],plane);
else
  jtl_vxv(wks->pts[slice-1],wks->pts[slice],plane);
  
side = jtl_vdv(newpt,plane);

if (side >= 0) /* inside or on */
  {
  wks->inslice = slice; /* debugging */
  bary[2] = side;
  jtl_cpv(newpt,okpt);
  return(TRUE);
  }
else /* negative slice indicates the point is outside the wks */
  {
  wks->inslice = -slice; /* debugging */  
  bary[2] = -side;
  return(FALSE);
  }
}


/**************************************************************************/
/* checks if z value from okrot is within values defined in wks zlim.
   okrot is hypothetical new rot. at this point we know okrot[0],[1]
   are legitimate, we just need to check [z].  bary give the
   triple cross product (volume) of slice-pt, (slice-1)-pt, and
   outerboundary-pt tetrahedrons, the ratio of which to the their
   sum is the barycentric coordinate of the slice, slice-1, and
   vpt respectively. */
static int check_wksz(Jtl_WorkSpacePtr wks, float *okRotZ, float bary[3])
{
int slice,ok = TRUE;
float zmin,zmax,sum;

slice = wks->inslice;
if (slice < 0) 
  {
  fprintf(stderr,"checking z on outside point?! error\n");
  slice = -slice; /* shouldn't be here outside */
  }

/* make the barys ratios */
/*printf("bary %f %f %f sum %f\n",bary[0],bary[1],bary[2],sum);*/
sum = bary[0] + bary[1] + bary[2];
jtl_vxs(bary,1.0/sum,bary);

/* find the value of z from wks triangle zlim values */
zmin = bary[0]*wks->zlim[slice%wks->numpts][0]
  + bary[1]*wks->zlim[slice -1][0] + bary[2]*wks->vpt_zlim[0];
zmax = bary[0]*wks->zlim[slice%wks->numpts][1]
  + bary[1]*wks->zlim[slice -1][1] + bary[2]*wks->vpt_zlim[1];

ok = check_angle(*okRotZ, zmin, zmax, okRotZ);
  
return(ok);
}

/*****************************************************************************/
/* check which slice the line from present position to the desired position  */
/* intersects. check against the slice newpt lies in first, then the slice   */
/* nowpt lies in. If failed both, check all other slices. return TRUE if get */
/* the intersection. FALSE if not find any (probably never).                 */
static int isIntersect(Jtl_WorkSpacePtr wks, float nowpt[3], float newpt[3],
  float intsec[3], int slice, float bary[3])
{
int i,j,np;
float plane[3],side1,side2;

i = slice; /* start where intersection is likely to be */
np = wks->numpts;
for (j= 0; j < np; j++, i++)
  {
  jtl_vxv(wks->pts[(i-1)%np],wks->pts[i%np],plane);
  
  if (linePlane(nowpt,newpt,plane,intsec)==1)
	{
	/* check if we are in the right slice, else go on */
	/* might as well use the triple scalar product */
	side1 = jtl_vdv(wks->n[(i-1)%np],intsec);
	side2 = jtl_vdv(wks->n[i%np],intsec);
    
	if (side1 >= 0 && side2 < 0)
	  {
	  jtl_cpv(nowpt,wks->nowpt);
	  jtl_cpv(newpt,wks->newpt);
	  jtl_cpv(intsec,wks->intsec);
	  wks->inslice = (i % np); /* reset slice */
	  bary[0] = side2;
      bary[1] = side1;
      bary[2] = 0.0;  /* intersection should be in that plane */
	  return(TRUE);
	  }
	}
    
  i++; /* go to next slice */
  }
  
fprintf(stderr,"no isIntersect, \n");
return FALSE;
}

/************************************************************************/
/* compute the intersection of a line and plane. The line is given as   */
/* two points. if line is in the plane, return -1; parallel return -2,  */
/* intersection within range return 1. out of range return 0.           */
/* inter is the intersection                                     */ 
static int linePlane(float pt1[3], float pt2[3], float plane[3], float inter[3])
{
float t,dir[3];
float den;

/* direction of the line */
jtl_vmv(pt2,pt1,dir);

den = jtl_vdv(plane,dir);
/* if line is in plane, the z-axis is already on the boundary and
   the new pt is in the same plane but out of the slice (otherwise
   you wouldn't be here, because the newpt would be ok). if the
   newpt is out of the slice, a later workspace edge should catch it.
   it is possible due to floating point problems that the program
   won't catch the newpt in the plane as ok, but i'll worry about that
   if it is a problem, later. */
if (den == 0) /* line in plane */
  { 
  printf("intersect line in plane\n");
  return(-1);
  }
else
  { /* suggested by foley and van dam appendix. */
  t = - jtl_vdv(pt1,plane);

  /* if point is really close, let it stay there */
  if ((t > 0.0 &&  t < CLOSE_ENUF) ||
      (t < 0.0 && t > -CLOSE_ENUF))
    {
      /*printf("pt1 close enough t=%f\n",t);*/
    jtl_cpv(pt1,inter);
    return(TRUE);
    }

  t /= den; 

  /* this is ugly but ok for now */
  if (t >= - CLOSE_ENUF && t < 0.0)
    { /* close enough to use old, to ensure not outside */
    printf("t close enough to inside ends t=%f\n",t);
    jtl_cpv(pt1,inter);
    }
  else {  /* calculate intersection */
    jtl_vxs(dir,t,inter);
    jtl_vpv(inter,pt1,inter);
    }
  return(TRUE);
  }
}
 
 
/********************************************************/
/* given three points in order of p1,p2,p3. if they're  */
/* counter clockwise from the view of origin, return    */
/* TRUE. Otherwise return FALSE.                        */
static int isCounterClockwise(float p1[3],float p2[3],float p3[3])
{
return (jtl_vxvdv(p1,p2,p3) > 0);
}

/*********************************************************/
/* Calculates the visible point from the average of all points
   in the workspace. */
static void recalculateVisiblePoint(Jtl_LimitPtr jtl)
{
int i;
float d;
Jtl_WorkSpacePtr wks = &(jtl->wks);

wks->visiblept[0] = wks->visiblept[1] = wks->visiblept[2] = 0.0;

for (i=0; i < wks->numpts; i++) 
  {
  wks->visiblept[0] += wks->pts[i][0];
  wks->visiblept[1] += wks->pts[i][1];
  wks->visiblept[2] += wks->pts[i][2];
  }
  
wks->visiblept[0] /= wks->numpts;
wks->visiblept[1] /= wks->numpts;
wks->visiblept[2] /= wks->numpts;

d = jtl_vlength(wks->visiblept);

jtl_vxs(wks->visiblept, 1.0 / d, wks->visiblept);
}

/*********************************************************/
/* calculate normal vectors for every slice.    */
static void calculateJtNormals(Jtl_LimitPtr jtl)
{
  int i;
  
  for (i=0; i<jtl->wks.numpts; i++)
    {
    jtl_vxv(jtl->wks.visiblept,jtl->wks.pts[i],jtl->wks.n[i]);
    }
}

   
/* $Id: jointLimits.c,v 1.1 2007/05/14 11:05:44 mancini Exp $*/
/*
 * $Log: jointLimits.c,v $
 * Revision 1.1  2007/05/14 11:05:44  mancini
 * *** empty log message ***
 *
 * Revision 1.2  2007/02/01 11:57:14  hirschberg
 * remove some unused variables
 *
 * Revision 1.1  2005/01/21 12:27:31  mancini
 * starting version
 *
 * Revision 1.1  2004/05/25 08:30:11  bjoern
 * joint limit module
 *
 * Revision 1.1  2001/05/04 04:03:22  mslater
 * Initial revision
 *
 */
