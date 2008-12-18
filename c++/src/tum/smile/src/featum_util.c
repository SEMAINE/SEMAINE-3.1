/*******************************************************************************
 * openSMILE
 *  - open Speech and Music Interpretation by Large-space Extraction -
 * Copyright (C) 2008  Florian Eyben, Martin Woellmer, Bjoern Schuller
 * 
 * Institute for Human-Machine Communication
 * Technische Universitaet Muenchen (TUM)
 * D-80333 Munich, Germany
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
 *******************************************************************************/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Utility functions shared among all modules and classes:
        
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "featum_common.h"

#include <stdio.h>
#include <inttypes.h>
#include "featum_util.h"

// defaults:
int loglevel; //??  
int loglevel_print_debug = 10; 
int loglevel_print_warning = 5;
int loglevel_print_fatal_err = 3;
int loglevel_print_err = 3;
int loglevel_print_message = 3;

// TODO: more extensive error handling capabilities and log-file support

void featum_function_enter(const char *module, const char *function) 
{
  printf("\n---##>>> ENTER [%s::%s]  ##---\n",module,function); fflush(stdout);
}

void featum_function_return(const char *module, const char *function, long retval) 
{
  printf("<<<---  Return [%s::%s] (%i) ---\n\n",module,function,retval); fflush(stdout);
}

void featum_function_returnV(const char *module, const char *function) 
{
  printf("<<<---  Return [%s::%s] (%i) ---\n\n",module,function); fflush(stdout);
}

#ifdef OPTIMIZE_SIZE_MSGS
void featum_err(int level, const char *module, const char *function, const char *format, ...) 
#else
inline void featum_err(int level, const char *module, const char *function, const char *format, ...) 
#endif
{
  va_list ap;
  
  if (level <= loglevel_print_err) {
    va_start(ap, format);
    #ifdef DEBUG_PRINT_NEWLINE
    printf("ERROR [in %s::%s]\n ",module,function);
    #else
    printf("ERROR [in %s::%s]: ",module,function);
    #endif
    vprintf(format, ap);
    printf("\n");
    va_end(ap);
    fflush(stdout);
  }
}

#ifdef OPTIMIZE_SIZE_MSGS
void featum_err_fatal(int level, const char *module, const char *function, const char *format, ...) 
#else
inline void featum_err_fatal(int level, const char *module, const char *function, const char *format, ...) 
#endif
{
  va_list ap;
  
  if (level <= loglevel_print_fatal_err) {
    va_start(ap, format);
    printf("FATAL ERROR [in %s::%s]: ",module,function);
    vprintf(format, ap);
    printf("\n");
    va_end(ap);
    fflush(stdout);
  } 
}

#ifdef OPTIMIZE_SIZE_MSGS
void featum_warning(int level, const char *module, const char *function, const char *format, ...) 
#else
inline void featum_warning(int level, const char *module, const char *function, const char *format, ...) 
#endif
{
  va_list ap;
  
  if (level <= loglevel_print_warning) {
    va_start(ap, format);
    printf("Warning [%s::%s]: ",module,function);
    vprintf(format, ap);
    printf("\n");
    va_end(ap);
    fflush(stdout);
  }
}

#ifdef OPTIMIZE_SIZE_MSGS
void featum_debug(int level, const char *module, const char *function, const char *format, ...) 
#else
inline void featum_debug(int level, const char *module, const char *function, const char *format, ...) 
#endif
{
  va_list ap;
  
  if (level <= loglevel_print_debug) {
    va_start(ap, format);
    #ifdef DEBUG_PRINT_NEWLINE
    printf("feaTUM dbg [%s::%s]\n   ",module,function);
    #else
    printf("feaTUM dbg [%s::%s]: ",module,function);
    #endif
    vprintf(format, ap);
    printf("\n");
    va_end(ap);
    fflush(stdout);
  }
}

#ifdef OPTIMIZE_SIZE_MSGS
void featum_message(int level, const char *module, const char *function, const char *format, ...) 
#else
inline void featum_message(int level, const char *module, const char *function, const char *format, ...) 
#endif
{
  va_list ap;
  
  if (level <= loglevel_print_message) {
    if (module != NULL) 
      printf("(%s) ",module);
    va_start(ap, format);
    vprintf(format, ap);
    printf("\n");
    va_end(ap);
    fflush(stdout);
  }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef OPTIMIZE_SIZE
void memzero(void * data, long num)
#else
inline void memzero(void * data, long num)
#endif
{
  int i;

  #ifdef COMPILE_BASIC_OPTIMISATIONS  
  if (num%4 == 0) {
    uint32_t *tmp = (uint32_t *)data;
    uint32_t *end = tmp + (num>>2);
    while(tmp < end) { *tmp = 0; tmp++; }
  } else if( num%2 == 0) {
    uint16_t *tmp = (uint16_t *)data;
    uint16_t *end = tmp + (num>>1);
    while(tmp < end) { *tmp = 0; tmp++; }
  } else {
  #endif
    uint8_t *tmp = (uint8_t *)data;
    uint8_t *end = tmp + num;
    while(tmp < end) { *tmp = 0; tmp++; }
  #ifdef COMPILE_BASIC_OPTIMISATIONS  
  }
  #endif  
}


/* last minute exit code: this is executed before the application terminates */
int exitApp(int code) {
  #ifdef __WINDOWS
  //system("PAUSE");
  #endif
  return code;    
}

/*
int string_length(char str[])
{
   int i;
   for(i=0; i&lt80; i++)
   {
	 if(str[i]=='\0')
	 {
	    return(i);
	 }
   }
}
*/

inline LONG_IDX min_l(LONG_IDX a, LONG_IDX b)
{
  if (a < b) return a;
  else return b;         
}


inline LONG_IDX max_l(LONG_IDX a, LONG_IDX b)
{
  if (a > b) return a;
  else return b;         
}

inline LONG_IDX min_lv(LONG_IDX *a, int n)
{
  int i;
  LONG_IDX min = *a;
  for (i=1; i<n; i++) {
    *a++;
    if (*a < min) min = *a;
  }
  return min;
}


inline LONG_IDX max_lv(LONG_IDX *a, int n)
{
  int i;
  LONG_IDX max = *a;
  for (i=1; i<n; i++) {
    *a++;
    if (*a > max) max = *a;
  }
  return max;
}

// optionally returns index of maximum of idx is not NULL
inline double max_dv(double *a, int n, LONG_IDX *idx)
{
  int i;
  double max = *a;
  LONG_IDX ix = 0;
  for (i=1; i<n; i++) {
    *a++;
    if (*a > max) { max = *a; ix = i; }
  }
  if (idx != NULL) *idx = ix;
  return max;
}

// optionally returns index of maximum of idx is not NULL
inline FLOAT_TYPE_FFT max_dvfft(FLOAT_TYPE_FFT *a, int n, LONG_IDX *idx)
{
  int i;
  FLOAT_TYPE_FFT max = *a;
  LONG_IDX ix = 0;
  for (i=1; i<n; i++) {
    *a++;
    if (*a > max) { max = *a; ix = i; }
  }
  if (idx != NULL) *idx = ix;
  return max;
}

inline FLOAT_TYPE_FFT mean_dvfft(FLOAT_TYPE_FFT *a, int n)
{
  int i;
  FLOAT_TYPE_FFT sum = *a;
  for (i=1; i<n; i++) {
    sum += *(++a);
  }
  return sum/(FLOAT_TYPE_FFT)n;
}

inline FLOAT_TYPE_FFT mean_abs_dvfft(FLOAT_TYPE_FFT *a, int n)
{
  int i;
  FLOAT_TYPE_FFT sum = *a;
  for (i=1; i<n; i++) {
    a++;
    sum += *a * *a;
  }
  return sqrt(sum/(FLOAT_TYPE_FFT)n);
}

// optionally returns index of minimum of idx is not NULL
inline double min_dv(double *a, int n, LONG_IDX *idx)
{
  int i;
  double min = *a;
  LONG_IDX ix = 0;
  for (i=1; i<n; i++) {
    *a++;
    if (*a < min) { min = *a; ix = i; }
  }
  if (idx != NULL) *idx = ix;
  return min;
}

inline int min_i(int a, int b)
{
  if (a < b) return a;
  else return b;         
}

inline int max_i(int a, int b)
{
  if (a > b) return a;
  else return b;         
}

inline double min_d(double a, double b)
{
  if (a < b) return a;
  else return b;         
}

inline double max_d(double a, double b)
{
  if (a > b) return a;
  else return b;         
}

// check if number is power of 2 (positive or negative)
int isPowerOf2(int x)
{
  if (x==1) return 1;  // 1 is a power of 2
  if (((x&1) == 0)&&(x != 0)) { // only even numbers > 1
    x=x>>1;
    while ((x&1) == 0) { x=x>>1;  }
    return ((x==1)||(x==-1));
  } 
  return 0;
}

// check if number is power of 2 (positive or negative)
int isPowerOf2_long(LONG_IDX x)
{
  if (x==1) return 1;  // 1 is a power of 2
  if (((x&1) == 0)&&(x != 0)) { // only even numbers > 1
    x=x>>1;
    while ((x&1) == 0) { x=x>>1;  }
    return ((x==1)||(x==-1));
  } 
  return 0;
}

// round up to the nearest power of 2 
// works only for positive numbers
long ceilPosPow2_long(long x) 
{
  unsigned long first = (unsigned long)(1)<<(sizeof(unsigned long)*8-1) ;
  if (x==0) return 0;
  if (x<0) {
    long mask = first;
    if (x != 0) while( (x&first) == first ) { x=x<<1; mask=mask>>1; }
    if (x-first == 0) return mask;
    else return mask<<1;
  } else {
    unsigned long mask = first;
    if (x != 0) while( (x&first) == 0 ) { x=x<<1; mask=mask>>1; }
    if (x-first == 0) return mask;
    else return mask<<1;
  }
}
	   
// check if number is positive power of 2
int isPosPowerOf2(int x)
{
  if (x==1) return 1;  // 1 is a power of 2
  if (((x&1) == 0)&&(x>1)) { // only even numbers > 1
    x=x>>1;
    while ((x&1) == 0) { x=x>>1;  }
    return (x==1);
  } 
  return 0;
}


inline FLOAT_TYPE_FTMEM ftAbs(FLOAT_TYPE_FTMEM a) 
{
  if (a >= 0) return a;
  else return -a;       
}

/******* datasave code ***********/

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int datasave_appendvec_d(char *filename, double *data, int N) 
{
  int first = 0;
  if ((data != NULL)&&(filename != NULL)) {
    int FH = open(filename, O_RDONLY);
    if (FH < 0) { first = 1; }
    else { close(FH); }
    
    FH = open(filename, O_APPEND|O_WRONLY|O_CREAT,  S_IRWXU);
    if (FH >= 0) {
      if (first) { // write header: one double containing the vector length
        double vlen = (double)N;
//    fprintf(stderr,"FUUUUUUUUUUUUCK! %f\n",vlen);
        write(FH,&vlen,sizeof(double));
      }
      int written = write(FH,data,N*sizeof(double));
      close(FH);
      if (written > 0) { return 1; }
      else {FEATUM_ERROR(0,"0 bytes written to file %s",filename); return 0;}
    } else {    perror("open"); return 0; } // error opening file...
  }
  return 0;
}

#ifdef __WINDOWS
double exp10(double a){
  return pow(a,10.0);
}
float exp10f(float a){
  return powf(a,10.0);
}
#endif

int file_exists(const char * filename)
{
    FILE * file;
    if (file = fopen(filename, "r"))
    {
        fclose(file);
        return 1;
    }
    return 0;
}
/****** profiling functions ******/



#ifdef __WINDOWS
void gettimeofday(timeval* p, void* tz /* IGNORED */){
  union {
    long long ns100; /*time since 1 Jan 1601 in 100ns units */
    FILETIME ft;
  } _now;

  GetSystemTimeAsFileTime( &(_now.ft) );
  p->tv_usec=(long)((_now.ns100 / 10LL) % 1000000LL );
  p->tv_sec= (long)((_now.ns100-(116444736000000000LL))/10000000LL);
  return;
}
#endif /* __WINDOWS */



//#include <sys/time.h>

// start the timer
void profiler_init(timeval *timer)
{
//  struct timeval {
//             time_t       tv_sec;   /* seconds since Jan. 1, 1970 */
//             suseconds_t  tv_usec;  /* and microseconds */
//  };    
  if (timer != NULL) {
    gettimeofday(timer,NULL);          
  }
}

// output timer status, i.e. current time
void profiler_printTime(timeval *timer)
{
  if (timer != NULL) {
    timeval now;
    gettimeofday(&now,NULL);
    now.tv_sec -= timer->tv_sec;
    if (now.tv_usec < timer->tv_usec) {
      now.tv_sec--;        
      now.tv_usec += (1000000 - timer->tv_usec);
    } else {
      now.tv_usec -= timer->tv_usec;
    }
    double runtime = (double)now.tv_sec + ((double)now.tv_usec)/1000000.0;
//    printf("FEATUM profiler:  runtime = %is:%ius\n",now.tv_sec,now.tv_usec);
    printf("FEATUM profiler:  runtime = %.4fs\n",runtime);
  }
}


