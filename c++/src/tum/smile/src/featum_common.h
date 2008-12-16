/*******************************************************************************
 * feaTUM, fast, efficient audio feature extractor by TUM
 * Copyright (C) 2008  Florian Eyben, Martin Woellmer
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *******************************************************************************/

#ifndef __FEATUM_COMMON_H
#define __FEATUM_COMMON_H
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Common defines and typedefs

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/



#ifdef HAVE_CONFIG_H
// include configuration from autoconf, if we're compiling with autoconf/automake
#include <config.h>
#endif

// compile on windows
//#define __WINDOWS
#define USE_FOPEN 1

// enable printing of featum total runtime at end
#define ENABLE_PROFILER   1

//#define SHOW_WARNINGS  1
#define SHOW_ERRORS    1
#define SHOW_FATAL_ERRORS  1
#define DEBUG 1
//#define DEBUG_SHOW_FUNCTION_ENTRY 1
#define FTMEM_NO_FUNCTION_ENTRY_DEBUG 1 

// enable this for better readability of debug messages
#define DEBUG_PRINT_NEWLINE 1     

// print values of all given commandline and config file parameters
// DEBUG must be defined for this to work (at min. loglevel 0)
#define ENABLE_PRINT_OPTIONS   

#define COMPILE_CHECKS 1
//#define COMPILE_CHECKS_LOOPS 1   // compile checks in functions called often in loops
                                  // should be disabled by default for better performance
                                  // use only for debugging or if security is an issue
#define COMPILE_BASIC_OPTIMISATIONS 1

//#define OPTIMIZE_SIZE 1    // do not compile inline functions
//#define OPTIMIZE_SIZE_MSGS  1  // enable this to not compile message printing funcitons as inline

// use the portaudio library for sound recording (and possibly later playback)

#ifdef WITH_LIVEREC
  #define LIVE_REC 1
  #ifdef HAVE_PORTAUDIO
    #define USE_PORTAUDIO 1
  #endif
#endif

// TODO: add --with-XXX autoconf options for these:
#define ENABLE_FUNCTIONALS
//#define DEBUG_SILDET

//************ ERROR codes ****************************
#define NO_ERROR      0
#define ERR_INPUT   100
#define ERR_OUTPUT  200
#define ERR_MEMORY  500
#define ERR_CMDLINE 900
#define ERR_CONFIG  1000
//-----------------------------------------------------

// maximum string size of fixed size filename and path variables
// (not including the trailing NULL termination character)
#define PATH_STRING_SIZE 255

// set either to float or double for single or double precision
#define FLOAT_TYPE float
#define FLOAT_TYPE_TIME double
#define FLOAT_TYPE_FFT  float
#define FLOAT_TYPE_FTMEM FLOAT_TYPE
// TODO: float does not work.... brings segfault!! ??

// type for long indicies (file indicies, buffer indicies)
#define LONG_IDX   long
// for 64 bit file seek:  define off_t to be 64-bit


// maximal wave input internal ring-buffer size in BYTES 
#define WAVE_INPUT_BUFSIZE  16000   // max 64MB for stereo wave files
#define LIVE_INPUT_BUFSIZE  16000   // max 64MB for stereo wave files


// save internal data vectors for debugging ....
// TODO: make this configurable
//#define DATASAVE_ALL 1
#ifdef DATASAVE_ALL
#define DATASAVE_MFCC 1
#define DATASAVE_CEPSTRUM 1
#define DATASAVE_ENERGY 1
#define DATASAVE_FFT 1
#define DATASAVE_F0ACF 1
#define DATASAVE_LPC 1
#endif

/****************************************/
#endif //__FEATUM_COMMON_H
