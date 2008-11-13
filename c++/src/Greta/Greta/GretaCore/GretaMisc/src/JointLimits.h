/**************************************************************************
 *
 *  Joint Limits sub-system header file.
 *
 *  Algorithms by Jane Wilhelms, and Allen Van Gelder
 *
 *  Original code by Jane Wilhelms and Alison Luo (2000)
 *  Additions by Jennifer Bevan and Mark Slater (2001)
 *
 *  University of California, Santa Cruz
 *
 *
 * Note: In the joint limits code, the Z axis is the longitudinal axis.
 *       Euler rotations are applied in the order Z, then Y, then X when
 *       rotating a segment relative to its parent.
 *
 **************************************************************************/

#ifndef JOINT_LIMITS_H
#define JOINT_LIMITS_H

/***************************************************************************
 *** Constants.
 ***************************************************************************/
/* Flags for accessing Euler min/max values. */
#define JTL_XMIN  0
#define JTL_XMAX  1
#define JTL_YMIN  2
#define JTL_YMAX  3
#define JTL_ZMIN  4
#define JTL_ZMAX  5

/* Joint limit allowable active flags. */
#define JTL_X     1
#define JTL_Y     2
#define JTL_Z     4
#define JTL_WKS   8
#define JTL_WKSZ 16

#ifdef __cplusplus
extern "C" {
#endif
/* Return values of the jtl_allowable*() functions. */
typedef enum _allowable_check_enum {
  JTL_FAIL_WITH_INTERSECT = -1,
  JTL_FAIL_NO_INTERSECT = 0,
  JTL_OK = 1
  } Jtl_Allowable;




/***************************************************************************
 *** Custom type definitions.
 ***************************************************************************/
typedef float Jtl_Vector2D[2];
typedef float Jtl_Vector3D[3];

/***************************************************************************/
/* Euler min/max rotations. */
typedef struct _jtl_maxminStruct {
  float mm[6];     /* xmin, xmax, ymin, ymax, zmin, zmax */
  } Jtl_MaxMinType;

typedef Jtl_MaxMinType* Jtl_MaxMinPtr;

/***************************************************************************/
/* workspace is a list of points  */
typedef struct __jtl_wkspaceStruct {
  float visiblept[3]; /* point which can be "seen" by all boundary points */ 
  float vpt_zlim[2]; /* z-limits at visible point */
  Jtl_Vector3D *pts; /* location of 3d points in cartesian coordinates (x,y,z) */
  Jtl_Vector3D *n;   /* normal to slices */
  Jtl_Vector2D *zlim; /* z limits at pts */
  int activept; /* which of wks points is active */
  int numpts; /* number of workspace points */
  int sizearray; /* how big an array was allocated for wks pts */
  int inslice; /* -1 if none, else between planes inslice-1 and inslice */
  float intsec[3];
  float nowpt[3];
  float newpt[3];
  float okpt[3];
  } Jtl_WorkSpaceType;

typedef Jtl_WorkSpaceType* Jtl_WorkSpacePtr;


/***************************************************************************/
/* Joint limits that can use either Euler angles or reach cones.           */
typedef struct _jtl_jtlimitStruct {
  Jtl_MaxMinType xyzlimits; /* limits for each cartesian direction, x,y,z */
  Jtl_WorkSpaceType wks; /* definition of workspace */
  int active; /* whether limits active [x,y,z,wks] bits */
  int userDefVisPt; /* whether the visible point is user defined. */
  } Jtl_LimitType;

typedef Jtl_LimitType* Jtl_LimitPtr;





/***************************************************************************
 *** Function prototypes.
 ***************************************************************************/
/**
 * Initialize the specified joint limits.
 *
 * @param jtl   Pointer to the joint limits.
 */
void jtl_init(Jtl_LimitPtr jtl);


/**
 * Validate the workspace. All boundary points must be counter-clockwise
 * from the visible point. This function checks the workspace for validity,
 * and returns TRUE if it is valid, and FALSE otherwise. A NULL joint limits
 * pointer is invalid. A reach cone with less than 3 points is always invalid.
 *
 * @param jtl   Pointer to the joint limits.
 *
 */
int jtl_validate(Jtl_LimitPtr jtl);

/**
 * Set the active point for the specified joint limits. When new points are
 * added to the reach cone, they are always added after the current active
 * point..
 *
 * @param jtl   Pointer to the joint limits.
 *
 * @param pointIndex    The index of the active point. If this value is out of
 *                      bounds, the active point will wrap to the opposite
 *                      boundary. In other words, if the requested index is
 *                      negative, the new active point will be the last point
 *                      in the workspace, and if the index is greater than the
 *                      index of the last point in the workspace, the new active
 *                      point index will be zero.
 */
void jtl_setActivePoint(Jtl_LimitPtr jtl, int pointIndex);

/**
 * Returns the index of the current active point.
 *
 * @param jtl   Pointer to the joint limits.
 */
int jtl_getActivePoint(Jtl_LimitPtr jtl);



/**
 * Sets the visible point of the specified joint limits. The visible point
 * is a point which can be seen by all boundary points of the reach cone with
 * the condition that for each boundary point, the arc from the boundary point
 * to the visible point lies entirely within the reach cone.
 * 
 * The reach cone can be automatically calculated from the boundary points or
 * it can be user defined.
 *
 * Note: Some reach cone shapes will not be able to calculate a valid visible
 *       point automatically. The function jtl_validate() can be used to check
 *       the workspace for this condition.
 *
 * @param jtl   Pointer to the joint limits.
 *
 * @param newVP     The new visible point. If this point is NULL, the visible
 *                  point will be automatically calculated from the boundary
 *                  points.
 */
void jtl_setVisiblePoint(Jtl_LimitPtr jtl, float newVP[3]);



/**
 * Adds a new boundary point to the reach cone.
 *
 * @param jtl   Pointer to the joint limits.
 *
 * @param newPt     The new boundary point.
 */
void jtl_addPoint(Jtl_LimitPtr jtl, float newPt[3]);

/**
 * Adds a new boundary point to the reach cone along with 
 * z-Axis twist limits. added by Bjoern Hartmann 2004-05-07
 *
 * @param jtl   Pointer to the joint limits.
 *
 * @param newPt     The new boundary point.
 */
void jtl_addPoint_withTwist(Jtl_LimitPtr jtl, float newPt[3], float twst[2]);


/**
 * Deletes the active point from the reach cone. While the visible point can
 * be made the active point, it cannot be deleted. Calling this function
 * while the visible point is the active point will have no effect.
 *
 * @param jtl   Pointer to the joint limits.
 */
void jtl_deleteActivePoint(Jtl_LimitPtr jtl);

/**
 * Deletes all points and resets the state of the specified joint limits
 * to the default values.
 *
 * @param jtl   Pointer to the joint limits.
 */
void jtl_deleteAllPoints(Jtl_LimitPtr jtl);



/**
 * Checks to see if the new point is within the joint limits. When the workspace
 * is active, the function checks the new point against the workspace reach
 * cone limits. The workspace Z angle is made active separately from the
 * position, so both the workspace and the workspace Z must be active for the Z
 * angle to be checked against the workspace limits. The Euler Z rotation will
 * only be checked if either the overall workspace or the workspace Z are
 * inactive. If okZRot is NULL, the z rotation will not be checked even if it
 * is active.
 *
 * @param jtl   Pointer to the joint limits.
 *
 * @param matrix    The local transformation including scale, rotation,
 *                  and translation.
 *
 * @param rotation  The current segment rotation.
 *
 * @param newPt     The desired position of the segment's endpoint. This point
 *                  will be checked against the active joint limits.
 *
 * @param newZRot   The desired Z axis (logitudinal axis) rotation at the new
 *                  endpoint.
 *
 * @param okPt  A point that is valid according to the active joint
 *              limits will be placed here. If newPt is valid, okPt
 *              will equal newPt. If newPt is not valid and no intersection
 *              is requested, okPt will equal the current position. If newPt
 *              is not valid and an intersection is requested, okPt will equal
 *              a point that intersects with the boundaries of the reach cone
 *              on the path to the suggested newPt.
 *
 * @param okZRot    A Z axis rotation which is within the active joint limits
 *                  at the okPt. If this parameter is NULL, Z axis rotation
 *                  will be ignored.
 *
 * @param intersection  FLAG: If this variable is true, and newPt is outside
 *                      the joint limits, the closest rotation that is within
 *                      the joint limits will be returned in okPt.
 *
 * @return  JTL_OK if newPt is within the active joint limits.
 *          JTL_FAIL_WITH_INTERSECTION if newPt is not within the active
 *              joint limits, but an intersection point was found.
 *          JTL_FAIL_NO_INTERSECTION if newPt is not within the active joint
 *              limits and an intersection point was not requested or could
 *              not be found.
 */
Jtl_Allowable jtl_allowablePosition(Jtl_LimitPtr jtl, float matrix[4][4],
  float rotation[3], float newPt[3], float newZRot, float okPt[3],
  float *okZRot, int intersection);

/**
 * Checks to see if the new angle is within the joint limits. When the workspace
 * is active, the function checks the new rotation against the workspace reach
 * cone limits, otherwise it checks against the Euler angles defined for the
 * segment. The workspace Z is made active separately from the X and Y, so
 * both the workspace and the workspace Z must be active for the Z angle
 * to be checked against the workspace limits. If either the overall workspace
 * or the workspace Z is inactive, the Z Euler angle will be checked (if it is
 * active).
 *
 * @param jtl   Pointer to the joint limits.
 *
 * @param matrix    The local transformation including scale, rotation,
 *                  and translation.
 *
 * @param rotation  The current segment rotation.
 *
 * @param newRot    The desired new rotation. This rotation will be checked
 *                  against the active joint limits.
 *
 * @param okRot     A rotation that is valid according to the active joint
 *                  limits will be placed here. If newRot is valid, okRot
 *                  will equal newRot. If newRot is not valid and no intersection
 *                  is requested, okRot will equal rotation. If newRot is not
 *                  valid and an intersection is requested, okRot will equal
 *                  a rotation that intersects with the boundaries of the
 *                  reach cone.
 *
 * @param intersection  FLAG: If this variable is true, and newRot is outside
 *                      the joint limits, the closest rotation that is within
 *                      the joint limits will be returned in okRot.
 *
 * @return  JTL_OK if newRot is within the active joint limits.
 *          JTL_FAIL_WITH_INTERSECTION if newRot is not within the active
 *              joint limits, but an intersecting rotation was found.
 *          JTL_FAIL_NO_INTERSECTION if newRot is not within the active joint
 *              limits and an intersecting rotation was not requested or could
 *              not be found.
 */
Jtl_Allowable jtl_allowableRotation(Jtl_LimitPtr jtl, float matrix[4][4],
  float rotation[3], float newRot[3], float okRot[3], int intersection);


/**
 * Sets the specified allowable flag active. The allowable flags are checked
 * by the jtl_allowable*() functions to see which joint limits are active.
 *
 * @param jtl   Pointer to the joint limits.
 *
 * @param flag  The allowable flag. This must be one of:
 *                  JTL_X, JTL_Y, JTL_Z, JTL_WKS, JTL_WKSZ
 *
 * @return  TRUE if the flag was successfully set. Some flags are mutually 
 *          exclusive. If the JTL_WKS flag is set, JTL_X and JTL_Y cannot be
 *          set. In addition, if the JTL_WKSZ flag is set, JTL_Z cannot be set.
 *          Since the JTL_WKS and JTL_WKSZ flags take precedence over the
 *          JTL_X, JTL_Y, and JTL_Z flags, setting a workspace flag will
 *          unset the corresponding Euler angle flags.
 */
int jtl_setAllowableFlag(Jtl_LimitPtr jtl, int flag);

/**
 * Unsets the specified allowable flag active. The allowable flags are checked
 * by the jtl_allowable*() functions to see which joint limits are active.
 * Unset flags are not active. By default, all flags are unset.
 *
 * @param jtl   Pointer to the joint limits.
 *
 * @param flag  The allowable flag. This must be one of:
 *                  JTL_X, JTL_Y, JTL_Z, JTL_WKS, JTL_WKSZ
 */
void jtl_unsetAllowableFlag(Jtl_LimitPtr jtl, int flag);

/**
 * Returns TRUE if the specified flag is set.
 *
 * @param jtl   Pointer to the joint limits.
 *
 * @param flag  The allowable flag. This must be one of:
 *                  JTL_X, JTL_Y, JTL_Z, JTL_WKS, JTL_WKSZ
 */
int jtl_isAllowableFlagSet(Jtl_LimitPtr jtl, int flag);


/**
 * Returns the current setting of the specified Euler min/max flag.
 *
 * @param jtl   Pointer to the joint limits.
 *
 * @param valueFlag  The value flag. This must be one of:
 *                  JTL_XMIN, JTL_XMAX, JTL_YMIN, JTL_YMAX, JTL_ZMIN, JTL_ZMAX
 */
float jtl_getMinMaxSetting(Jtl_LimitPtr jtl, int valueFlag);

/**
 * Changes the current setting of the specified Euler min/max flag.
 *
 * @param jtl   Pointer to the joint limits.
 *
 * @param valueFlag  The value flag. This must be one of:
 *                      JTL_XMIN, JTL_XMAX, JTL_YMIN, JTL_YMAX, JTL_ZMIN, JTL_ZMAX
 *
 * @param value     The new setting for the value.
 */
void jtl_setMinMaxSetting(Jtl_LimitPtr jtl, int valueFlag, float value);

/**
 * Changes all Euler min/max settings.
 *
 * @param jtl   Pointer to the joint limits.
 *
 * @param xmin  The minimum Euler X rotation.
 *
 * @param xmax  The maximum Euler X rotation.
 *
 * @param ymin  The minimum Euler Y rotation.
 *
 * @param ymax  The maximum Euler Y rotation.
 *
 * @param zmin  The minimum Euler Z rotation.
 *
 * @param zmax  The maximum Euler Z rotation.
 */
void jtl_setMinMax(Jtl_LimitPtr jtl, float xmin, float xmax, float ymin,
  float ymax, float zmin, float zmax);


/**
 * Subdivides the boundary points of the reach cone, resulting in a smoother
 * boundary. At the end of this function there will be twice as many boundary
 * points as before.
 *
 * @param jtl   Pointer to the joint limits.
 */
void jtl_subdivide(Jtl_LimitPtr jtl);
#ifdef __cplusplus
} //extern "C"
#endif


#endif /* JOINT_LIMITS_H */


/* $Id: jointLimits.h,v 1.1 2007/05/14 11:05:45 mancini Exp $*/
/*
 * $Log: jointLimits.h,v $
 * Revision 1.1  2007/05/14 11:05:45  mancini
 * *** empty log message ***
 *
 * Revision 1.1  2005/01/21 12:27:32  mancini
 * starting version
 *
 * Revision 1.1  2004/05/25 08:30:11  bjoern
 * joint limit module
 *
 * Revision 1.1  2001/05/04 04:02:11  mslater
 * Initial revision
 *
 */
