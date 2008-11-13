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

/// @file GestureEngineGlobals.h
/// global variables for the gesture generation module
///
//////////////////////////////////////////////////////////////////////

#if !defined GESTURE_ENGINE_GLOBALS
#define GESTURE_ENGINE_GLOBALS
#ifndef UNIX
#pragma once
#endif
	
#define LOGFILE

/* interpolation type - for choosing which interpolator to use
   inside the motorplanner */
enum InterpType {EulerInterpType=0,QuatInterpType=1,PathInterpType=2};

#define WITH_EXPRESSIVITY

/** Global FRAMES-PER-SECOND rate */
const int	GLOBAL_FPS = 25; 

/** Global milliseconds per frame constant */
const float GLOBAL_MSEC_PER_FRAME = float(1000.0/float(GLOBAL_FPS));

/* Global: Play back wave file or not? 
	*:TODO: seems in the wrong place - this is a playback variable, not a generation variable
	*/
//const bool	GLOBAL_PLAYSOUND = false;

#endif
