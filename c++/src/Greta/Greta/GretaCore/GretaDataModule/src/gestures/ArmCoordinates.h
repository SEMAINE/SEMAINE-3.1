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

/// @file ArmCoordinates.h
/// Defines enumeration types for arm position sectors in the reach space.
/// @see ArmPosition
/// @see ReachSpace
//////////////////////////////////////////////////////////////////////
#include "GestureEngineGlobals.h"

#ifndef ARMCOORDINATES_H
#define ARMCOORDINATES_H

/** "concrete" X coordinate for file-based joint angle lookup. OBSOLETE */
enum ArmPositionX {X_outside, X_same_shoulder, X_spine, X_opposite_shoulder, X_default};

/** "concrete" Y coordinate for file-based joint angle lookup. OBSOLETE */
enum ArmPositionY {Y_leg=-1,Y_hip, Y_chest, Y_shoulder, Y_head  ,Y_default};

/** "concrete" Z coordinate for file-based joint angle lookup. OBSOLETE */
enum ArmPositionZ {Z_near, Z_middle, Z_far, Z_default};

/** "abstract" X coordinate (horizontal) following McNeill */
enum ArmX {XEP=0, XP, XC, XCC, XOppC, XDefault};

/** "abstract" Y coordinate (vertical) following McNeill */
enum ArmY {YUpperEP=0, YUpperP, YUpperC, YCC, YLowerC, YLowerP, YLowerEP,YDefault};

/** "abstract" Z coordinate (frontal) following McNeill */
enum ArmZ {ZNear=0, ZMiddle, ZFar,ZDefault};

/** int to enum ArmX mapping (lookup array) */
static ArmX ArmXLookup[]={XEP, XP, XC, XCC, XOppC};

/** int to enum ArmY mapping */
static ArmY ArmYLookup[]={YUpperEP, YUpperP, YUpperC, YCC, YLowerC, YLowerP, YLowerEP};

/** int to enum ArmZ mapping */
static ArmZ ArmZLookup[]={ZNear, ZMiddle, ZFar};
#endif
