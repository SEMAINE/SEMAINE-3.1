/***************************** License **********************************

 Copyright (C) 2006 by Communicative Machines
 http://www.cmlabs.com   All rights reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

************************************************************************/


/*

  This is the file where we define all we need for different OSes,
  like the Darwin system, that does not have various normal OS functions.

*/

#if !defined(__standard_h)              // Sentry, use file only if it's not already included.
#define __standard_h

// #pragma warning( disable : 4995 )

#define MSGTIMEOUT 5000

#ifdef __GNUC__
	#ifdef __GNUC_PATCHLEVEL__
		#define GCC_VERSION (__GNUC__ * 10000 \
                     + __GNUC_MINOR__ * 100 \
                     + __GNUC_PATCHLEVEL__)
	#else
		#define GCC_VERSION (__GNUC__ * 10000 \
                     + __GNUC_MINOR__ * 100)
	#endif
#else
	#define GCC_VERSION 0
#endif

#ifdef WIN32
	#ifdef _WIN32_WCE
		#define WIN32_POCKETPC
		#define NATIVESYSTEMNAME "Win32PPC"
		#define COMPILERVERSION _MSC_VER
	#else
		#define WIN32_DESKTOP
		#ifdef CYGWIN
			#define WIN32_CYGWIN
			#define NATIVESYSTEMNAME "Win32Cygwin"
			#define COMPILERVERSION GCC_VERSION
		#else
			#define WIN32_VS
			#if _MSC_VER < 1300
				#define WIN32_VS6
				#define NATIVESYSTEMNAME "Win32VS6"
			#else
				#define WIN32_VSNET
				#define NATIVESYSTEMNAME "Win32VSNET"
			#endif // _MSC_VER < 1300
			#define COMPILERVERSION _MSC_VER
		#endif
	#endif
#else
	#define NONWIN32
	#ifdef Darwin
		#define NATIVESYSTEMNAME "Darwin"
	#else
		#define NATIVESYSTEMNAME "Unix"
	#endif
	#define COMPILERVERSION GCC_VERSION
#endif

#ifdef Darwin
	#if GCC_VERSION < 40000
	#endif // GCC_VERSION < 40000
#endif // Darwin

// Test for GCC > 3.2.0
#if GCC_VERSION > 30200
#endif

//3.1415926535
#define clPI 3.14159265358979323846264 
#define clmax(a,b) (((a) > (b)) ? (a) : (b))
#define clmin(a,b) (((a) < (b)) ? (a) : (b))
#define clround(x) (int) (((x) - floor(x)) >= 0.5 ? (floor(x) + 1) : (floor(x)))
#define clsqr(x) ((x) * (x))
#define clsign(x) (((x) < 0) ? (-1) : (1))
#define clabs(a) ((a)<0?-(a):(a))

#ifndef WIN32
#ifdef _WIN32
#define WIN32
#endif
#endif

#ifdef WIN32
	#if defined(CYGWIN)
		#include <winsock2.h>
	#endif

	#define longlong signed __int64
	// typedef signed __int64 longlong;
	#include <windows.h>
	#include <stdlib.h>
	#include <typeinfo>
	#include <RpcDce.h>

	#if !defined(_WIN32_WCE)
		#if !defined(CYGWIN)
			#ifdef _DEBUG
				#ifdef DEVELOPMENT
					#include <crtdbg.h>
					#define _CRTDBG_MAP_ALLOC
					#define MY_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__) 
					#define new MY_NEW 
				#endif
			#endif
		#endif
	#endif

#else

	typedef long long longlong;
	#ifdef Darwin
	#else
	#endif // Darwin

	#include <unistd.h>
	#include <typeinfo>

/*	typedef void *HANDLE;
	typedef unsigned int DWORD;
	typedef unsigned short WORD;
	typedef WORD *LPWORD;
*/
	#ifndef __int64
		typedef long long __int64;
	#endif 

/*	typedef struct _FILETIME {
		DWORD dwLowDateTime;
		DWORD dwHighDateTime;
	} FILETIME, *PFILETIME, *LPFILETIME;

	typedef struct _SYSTEMTIME {
		WORD wYear;
		WORD wMonth;
		WORD wDayOfWeek;
		WORD wDay;
		WORD wHour;
		WORD wMinute;
		WORD wSecond;
		WORD wMilliseconds;
	} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;
*/
	#define HAVE_TIMEZONE_VAR 1
#endif

#if !defined(_WIN32_WCE)
	#include <signal.h>
#endif


// For Darwin, we need to do quite a lot
#ifdef Darwin


#endif

#endif // __standard_h
