/*F******************************************************************************
 *
 * openSMILE - open Speech and Music Interpretation by Large-space Extraction
 *       the open-source Munich Audio Feature Extraction Toolkit
 * Copyright (C) 2008-2009  Florian Eyben, Martin Woellmer, Bjoern Schuller
 *
 *
 * Institute for Human-Machine Communication
 * Technische Universitaet Muenchen (TUM)
 * D-80333 Munich, Germany
 *
 *
 * If you use openSMILE or any code from openSMILE in your research work,
 * you are kindly asked to acknowledge the use of openSMILE in your publications.
 * See the file CITING.txt for details.
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
 *
 ******************************************************************************E*/


/*  openSMILE type definitions */


#ifndef __SMILE_TYPES_HPP
#define __SMILE_TYPES_HPP

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* int types, platform independent (hopefully...) */
#ifdef HAVE_INTTYPES_H  // prefer inttypes.h if available
#include <inttypes.h>
#else
#ifdef _MSC_VER // Visual Studio specific macro, use MSVC built-in int types
#define uint8_t unsigned __int8
#define uint16_t unsigned __int16
#define uint32_t unsigned __int32
#define uint64_t unsigned __int64
#define int8_t signed __int8
#define int16_t signed __int16
#define int32_t signed __int32
#define int64_t signed __int64
#else
#ifdef __WINDOWS  // UINT8, etc., constants should be defined in Windows Platform SDK 2003
#define uint8_t UINT8
#define uint16_t UINT16
#define uint32_t UINT32
#define uint64_t UINT64
#define int8_t INT8
#define int16_t INT16
#define int32_t INT32
#define int64_t INT64
#else  // last resort, use C standards.. warning, int is not very portable, also long long, etc...
#define uint8_t unsigned char
#define uint16_t unsigned short
#define uint32_t unsigned int
#define uint64_t unsigned long long
#define int8_t signed char
#define int16_t signed short
#define int32_t signed int
#define int64_t signed long long
#endif
#endif

#endif

/* opensmile internal types */
#define FLOAT_DMEM_FLOAT  0
#define FLOAT_DMEM_DOUBLE  1

// this defines the float type used throughout the data memory, either 'float' or 'double'
#define FLOAT_DMEM  float
#define FLOAT_DMEM_NUM  FLOAT_DMEM_FLOAT // this numeric constant MUST equal the float type set above ...
                                           // 0 = float, 1 = double:
// this defines the int type used throughout the data memory, either 'short', 'int' or 'long'
#define INT_DMEM    int








#endif // __SMILE_TYPES_HPP

