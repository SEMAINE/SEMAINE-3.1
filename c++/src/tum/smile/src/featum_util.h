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


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Utility functions shared among all modules and classes:
        
        

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef __FEATUM_UTIL_H
#define __FEATUM_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MODULE
#define MODULE "unknown"
#endif
#ifndef FUNCTION
#define FUNCTION ""
#endif

#include <stdarg.h>
#include "featum_common.h"

extern int loglevel_print_debug; 
extern int loglevel_print_warning;
extern int loglevel_print_fatal_err;
extern int loglevel_print_err;
extern int loglevel_print_message;

// TODO: more extensive error handling capabilities and log-file support

void featum_err(int level, const char *module, const char *function, const char *format, ...);
void featum_err_fatal(int level, const char *module, const char *function, const char *format, ...);
void featum_warning(int level, const char *module, const char *function, const char *format, ...);
void featum_debug(int level, const char *module, const char *function, const char *format, ...);
void featum_message(int level, const char *module, const char *function, const char *format, ...);

void featum_function_enter(const char *module, const char *function);
void featum_function_return(const char *module, const char *function, long retval);
void featum_function_returnV(const char *module, const char *function);

/***** error handling functions: *****/

// Non-fatal error: level = severity level (for logging: = error log-level)
#ifdef SHOW_ERRORS
#define FEATUM_ERR(level, rest...)       featum_err(level, MODULE, FUNCTION, ## rest)
#define FEATUM_ERROR(level, rest...)     featum_err(level, MODULE, FUNCTION, ## rest)
#else
#define FEATUM_ERR
#define FEATUM_ERROR
#endif

#ifdef SHOW_FATAL_ERRORS
// FATAL error: always abort program, and log error, level can always be 0
#define FEATUM_ERR_FATAL(level, rest...)   featum_err_fatal(level, MODULE, FUNCTION, ## rest)
#define FEATUM_FATAL_ERR(level, rest...)   featum_err_fatal(level, MODULE, FUNCTION, ## rest)
#define FEATUM_ERROR_FATAL(level, rest...) featum_err_fatal(level, MODULE, FUNCTION, ## rest)
#define FEATUM_FATAL_ERROR(level, rest...) featum_err_fatal(level, MODULE, FUNCTION, ## rest)
#else
#define FEATUM_ERR_FATAL
#define FEATUM_FATAL_ERR
#define FEATUM_ERROR_FATAL
#define FEATUM_FATAL_ERROR
#endif

#ifdef SHOW_WARNINGS
// warnings, non-fatal, no real errors:  level = warn log-level
#define FEATUM_WARNING(level, rest...)   featum_warning(level, MODULE, FUNCTION, ## rest)
#else
#define FEATUM_WARNING
#endif


#ifdef DEBUG 
// debug messages : level = debug log-level
#define FEATUM_DEBUG(level, rest...)     featum_debug(level, MODULE, FUNCTION, ## rest)
#else
#define FEATUM_DEBUG
#endif

#ifdef DEBUG_SHOW_FUNCTION_ENTRY
#define _FUNCTION_ENTER_ {featum_function_enter(MODULE, FUNCTION);}
#define _FUNCTION_RETURN_(retval) {featum_function_return(MODULE, FUNCTION, (long)(retval)); return retval; }
#define _FUNCTION_RETURNV_ {featum_function_returnV(MODULE, FUNCTION); return; }
#else
#define _FUNCTION_ENTER_
#define _FUNCTION_RETURN_(retval) {return retval;}
#define _FUNCTION_RETURNV_ {return;}
#endif

// all general messages printed to screen
#define FEATUM_MESSAGE(level, rest...)   featum_message(level, MODULE, FUNCTION, ## rest)

/************** other functions **************************/

#define MINIMUM(A,B) (A<B)?A:B

LONG_IDX min_l(LONG_IDX a, LONG_IDX b);
LONG_IDX max_l(LONG_IDX a, LONG_IDX b);
int min_i(int a, int b);
int max_i(int a, int b);
double min_d(double a, double b);
double max_d(double a, double b);

int isPowerOf2(int x);
int isPowerOf2_long(LONG_IDX x);
int isPosPowerOf2(int x);
LONG_IDX ceilPosPow2_long(LONG_IDX x);

/* for arrays: */
LONG_IDX min_lv(LONG_IDX *a, int n);
LONG_IDX max_lv(LONG_IDX *a, int n);
// optionally returns index of maximum of idx is not NULL
double max_dv(double *a, int n, LONG_IDX *idx);
// optionally returns index of minimum of idx is not NULL
double min_dv(double *a, int n, LONG_IDX *idx);

FLOAT_TYPE_FFT max_dvfft(FLOAT_TYPE_FFT *a, int n, LONG_IDX *idx);
FLOAT_TYPE_FFT mean_dvfft(FLOAT_TYPE_FFT *a, int n);
FLOAT_TYPE_FFT mean_abs_dvfft(FLOAT_TYPE_FFT *a, int n);

// abs function for feature memory data type
 FLOAT_TYPE_FTMEM ftAbs(FLOAT_TYPE_FTMEM a);

#ifdef __WINDOWS
#include <windows.h>
#define bzero(rest...) memzero(## rest)
#define sleep(x) Sleep((x)*1000)
#define usleep(x) Sleep((x)/1000)
#endif

int file_exists(const char * filename);
void memzero(void *, long num);

/* last minute exit code: this is executed before the application terminates */
// call it as "return exitApp(CODE);" in your int main() { ... }  
int exitApp(int code);

int datasave_appendvec_d(char *filename, double *data, int N);

#ifdef __WINDOWS
#ifndef __cplusplus
typedef struct {
       long tv_sec;        /* seconds */
       long tv_usec;  /* microseconds */
} timeval;
#else
typedef struct timeval timeval;
#endif

/*
typedef struct _FILETIME {
  unsigned long dwLowDateTime;
  unsigned long dwHighDateTime;
} FILETIME;
*/
void __stdcall GetSystemTimeAsFileTime(FILETIME*);

void gettimeofday(timeval* p, void* tz /* IGNORED */);
#else  /* for Linux: */
#include <sys/time.h>      /* UNTESTED ! */
typedef struct timeval timeval;
#endif /* __WINDOWS */

void profiler_init(timeval *timer);
void profiler_printTime(timeval *timer);

#ifdef __cplusplus
}
#endif

#endif //__FEATUM_UTIL_H
