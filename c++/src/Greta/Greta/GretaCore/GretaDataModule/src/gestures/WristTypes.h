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

/// @file WristTypes.h
/// enumeration types for specifying wrist orientation
///
/////////////////////////////////////////////////////////////////////
#ifndef WRIST_TYPES_H
#define WRIST_TYPES_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "GestureEngineGlobals.h"

namespace GestureSpace {
	/** Palm normal vector type */
	enum PalmType {PalmDefault, PalmUp, PalmDown, PalmInwards, PalmOutwards, PalmAway, PalmTowards, PalmNone};
	/** Finger base vector type */
	enum FingerBaseType {FBDefault, FBUp, FBDown, FBInwards, FBOutwards, FBAway, FBTowards, FBNone};
	
};
#endif
