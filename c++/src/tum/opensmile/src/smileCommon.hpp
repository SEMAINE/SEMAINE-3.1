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


/*

Common defines, config, etc.

this file also contains openSMILE's small and dirty platform independence layer 
and thread implementations

*/


#ifndef __SMILE_COMMON_H
#define __SMILE_COMMON_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdlib>
#include <limits>

//we enable GSL_ZSOLVE here, if we are building the GPL version:
#define HAVE_GSL_ZSOLVE

// enable debugging output....
#ifdef _DEBUG
#define DEBUG
#endif
#ifdef __DEBUG
#define DEBUG
#endif


#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#ifdef __WINDOWS
 
#ifdef __VISTA
#define WINVER 0x0600
#define _WIN32_WINNT 0x0600
#else
 // NT & XP compatibility
#define WINVER 0x0400
#define _WIN32_WINNT 0x0400
#endif
#include <windows.h>
/* winspool.h defines these as macros, 
   however they confilct with member varibale definitions 
   in Thread.h of the APR library,
   so we undef them for compatibility here */
#ifdef MIN_PRIORITY
#undef MIN_PRIORITY
#endif
#ifdef MAX_PRIORITY
#undef MAX_PRIORITY
#endif

#endif

#include <smileTypes.hpp>

#ifdef __WINDOWS
#ifndef __MINGW32
inline double round(double x) { return floor(x + 0.5); }
#define strdup _strdup
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#include <float.h>
#define isnan _isnan
#define isinf(x) ((!_finite(x))&&(!_isnan(x)))
#define finite _finite
#endif
#endif

/*
#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#else
#define uint8_t UINT8
#define uint16_t UINT16
#define uint32_t UINT32
#define uint64_t UINT64
#define int8_t INT8
#define int16_t INT16
#define int32_t INT32
#define int64_t INT64
#endif
*/

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

#ifdef _DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif

#define AUTO_EXCEPTION_LOGGING   1     // automatically log exceptions to smileLog global object

#ifndef __STATIC_LINK
#define SMILE_SUPPORT_PLUGINS
#endif


#ifdef _MSC_VER // Visual Studio specific macro
  #ifdef BUILDING_DLL
    #define DLLEXPORT __declspec(dllexport)
    #define class class __declspec(dllexport)
  #else
    #define DLLEXPORT __declspec(dllimport)
    #define class class __declspec(dllimport)
  #endif
  #define DLLLOCAL 
#else 
    #define DLLEXPORT 
    #define DLLLOCAL  
#endif


#define APPNAME  "openSMILE"
#define APPVERSION  "1.0.1"
//#define APPVERSION  "svn_trunk"
#define APPCPAUTHOR   "Florian Eyben, Martin Woellmer, Bjoern Schuller"
#define APPCPYEAR   "2008-2010"
#define APPDATE    "23-May-2010"
#define APPCPINST  "TUM Institute for Human-Machine Communication"
#define NEWLINE "\n"


#define EXIT_SUCCESS   0
#define EXIT_ERROR    -1
#define EXIT_CTRLC   -10

DLLEXPORT void smilePrintHeader();
DLLEXPORT int checkVectorFinite(FLOAT_DMEM *x, long N);

// realloc and initialize additional memory with zero,like calloc
DLLEXPORT void * crealloc(void *a, size_t size, size_t old_size);
#define MIN_LOG_STRLEN   255
DLLEXPORT char *myvprint(const char *fmt, ...);
#define FMT(...)   myvprint(__VA_ARGS__)
DLLEXPORT void * memdup(const void *in, size_t size);

#ifdef __WINDOWS
// for the windows MINGW32 environment, we have to implement some GNU library functions ourselves...
//void bzero(void *s, size_t n);
#define bzero(s,n) memset(s,0,n)
#define __HAVENT_GNULIBS
#ifndef _MSC_VER
void GetSystemTimeAsFileTime(FILETIME*);
#endif
DLLEXPORT void gettimeofday(timeval* p, void* tz /* IGNORED */);
//typedef 
//struct timeval {
//     long tv_sec;     /* seconds */
//     long tv_usec;    /* microseconds */
//};
#endif

#ifdef __HAVENT_GNULIBS
// for the windows MINGW32 environment and Mac, we have to implement some GNU library functions ourselves...
DLLEXPORT long getline (char **lineptr, size_t *n, FILE *stream);
DLLEXPORT long getstr (char **lineptr, size_t *n, FILE *stream, char terminator, int offset);
#endif

#ifndef __WINDOWS
#include <sys/time.h>
#endif

#ifndef INFINITY
#define INFINITY (std::numeric_limits<double>::infinity())
#endif

#ifdef __MACOS

#include <time.h>
//#include <sys/time.h>

DLLEXPORT int clock_gettime(int clock_id, struct timespec *tp);
#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME 0
#endif

#endif

// THREADS & MUTEXES:

//#include <windows.h>
//#include <process.h>
#ifdef HAVE_PTHREAD

#include <pthread.h>

#define SMILE_THREAD_RETVAL void *
#define SMILE_THREAD_RET return NULL

#define smileThread  pthread_t
#define smileThreadCreate(THRD,T_MAIN,ARG) (pthread_create(&(THRD), NULL, T_MAIN, ARG)==0)
#define smileThreadJoinWret(THRD,RET) pthread_join(THRD,&RET)
#define smileThreadJoin(THRD) pthread_join(THRD,NULL)

#define smileMutex pthread_mutex_t
#define smileMutexInitVar(mtx) mtx = PTHREAD_MUTEX_INITIALIZER
#define smileMutexCreate(mtx) pthread_mutex_init(&(mtx), NULL)
#define smileMutexLock(mtx) (pthread_mutex_lock(&(mtx))==0)
#define smileMutexUnlock(mtx) pthread_mutex_unlock(&(mtx))
#define smileMutexTryLock(mtx) pthread_mutex_trylock(&(mtx))
#define smileMutexDestroy(mtx) pthread_mutex_destroy(&(mtx))

typedef struct {
  pthread_mutex_t mtx;
  pthread_cond_t  cond;
} smileCond;

#define smileCondInitVar(COND) COND.mtx = PTHREAD_MUTEX_INITIALIZER; COND.cond = PTHREAD_COND_INITIALIZER
#define smileCondCreate(COND) pthread_mutex_init(&(COND.mtx), NULL); pthread_cond_init(&(COND.cond), NULL)
#define smileCondWait(COND) { pthread_mutex_lock(&(COND.mtx)); \
                              pthread_cond_wait(&(COND.cond), &(COND.mtx)) \
                              pthread_mutex_unlock(&(COND.mtx)); }
#define smileCondWaitWMtx(COND,MTX) pthread_cond_wait(&(COND.cond), &(MTX))

// this function returns !=0 upon timeout and 0 upon success (i.e. signal!)
#include <time.h>
#define smileCondTimedWait(COND,MSEC) { pthread_mutex_lock(&(COND.mtx)); \
                                        struct timespec __TOut; \
                                        clock_gettime(CLOCK_REALTIME, &__TOut); \
                                        __TOut.tv_sec += (long)MSEC/1000; \
                                        __TOut.tv_nsec += ( (long)MSEC-1000*((long)MSEC/1000) )*1000000; \
                                        pthread_cond_timedwait(&(COND.cond), &(COND.mtx), &__TOut); \
                                        pthread_mutex_unlock(&(COND.mtx)); }
#define smileCondTimedWaitWMtx(COND,MSEC,MTX) { struct timespec __TOut; \
                                        clock_gettime(CLOCK_REALTIME, &__TOut); \
                                        __TOut.tv_sec += (long)MSEC/1000; \
                                        __TOut.tv_nsec += ( (long)MSEC-1000*((long)MSEC/1000) )*1000000; \
                                        pthread_cond_timedwait(&(COND.cond), &(MTX), &__TOut); \
                                         }

#define smileCondSignal(COND) { pthread_mutex_lock(&(COND.mtx)); \
                                pthread_cond_signal(&(COND.cond)); \
                                pthread_mutex_unlock(&(COND.mtx)); }
#define smileCondSignalRaw(COND) pthread_cond_signal(&(COND.cond))
#define smileCondBroadcast(COND) { pthread_mutex_lock(&(COND.mtx)); \
                                   pthread_cond_broadcast(&(COND.cond)); \
                                   pthread_mutex_unlock(&(COND.mtx)); }
#define smileCondBroadcastRaw(COND) pthread_cond_broadcast(&(COND.cond))
#define smileCondDestroy(COND) pthread_cond_destroy(&(COND.cond)); pthread_mutex_destroy(&(COND.mtx));

#define smileSleep(msec)  usleep((msec)*1000)
#define smileYield()      sched_yield()

#else //not HAVE_PTHREAD

#ifdef __WINDOWS

#include <process.h>

#define SMILE_THREAD_RETVAL DWORD
#define SMILE_THREAD_RET return 0

#define smileThread  HANDLE
/*
HANDLE CreateThread(
  LPSECURITY_ATTRIBUTES lpThreadAttributes,  // pointer to security attributes
  DWORD dwStackSize,                         // initial thread stack size
  LPTHREAD_START_ROUTINE lpStartAddress,     // pointer to thread function
  LPVOID lpParameter,                        // argument for new thread
  DWORD dwCreationFlags,                     // creation flags
  LPDWORD lpThreadId                         // pointer to receive thread ID
);
*/
//#define smileThreadCreate(THRD,T_MAIN,ARG) ((long)(THRD = (HANDLE)_beginthread(T_MAIN,0,ARG)) != -1)
#define smileThreadCreate(THRD,T_MAIN,ARG) ((long)(THRD = (HANDLE)CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)T_MAIN,ARG,0,NULL)) != NULL)
//#define smileThreadJoinWret(THRD,RET) pthread_join(THRD,&RET)
#define smileThreadJoinWret(THRD,RET) WaitForSingleObject(THRD, INFINITE)
//#define smileThreadJoin(THRD) pthread_join(THRD,NULL)
#define smileThreadJoin(THRD) WaitForSingleObject(THRD, INFINITE)


#define smileMutex CRITICAL_SECTION
// #define smileMutex CRITICAL_SECTION
//#define smileMutexInitVar(mtx) mtx = PTHREAD_MUTEX_INITIALIZER
#define smileMutexInitVar(mtx) 
// #define smileMutexInitVar(mtx) mtx = ?????
//#define smileMutexCreate(mtx) ( (mtx = CreateMutex( NULL, FALSE, NULL )) != NULL )
#define smileMutexCreate(mtx) InitializeCriticalSection(&mtx)
//#define smileMutexLock(mtx) (pthread_mutex_lock(&(mtx))==0)
//#define smileMutexLock(mtx) WaitForSingleObject( mtx, INFINITE )
inline int enterCSstub(smileMutex *mtx) {
  EnterCriticalSection(mtx);
  return 1;
}
#define smileMutexLock(mtx) enterCSstub(&mtx) //EnterCriticalSection(&mtx)
//#define smileMutexUnlock(mtx) ReleaseMutex( mtx )
#define smileMutexUnlock(mtx) LeaveCriticalSection(&mtx)
//#define smileMutexTryLock(mtx) pthread_mutex_trylock(&(mtx))
//#define smileMutexTryLock(mtx) WaitForSingleObject( mtx, 0 )
#define smileMutexTryLock(mtx) (TryEnterCriticalSection( &mtx ) == TRUE)
//#define smileMutexDestroy(mtx) CloseHandle( mtx )
#define smileMutexDestroy(mtx) DeleteCriticalSection( &mtx )


//------ condition variables:
/*
typedef struct
{
  int waiters_count_;
  // Number of waiting threads.

  CRITICAL_SECTION waiters_count_lock_;
  // Serialize access to <waiters_count_>.

  HANDLE sema_;
  // Semaphore used to queue up threads waiting for the condition to
  // become signaled. 

  HANDLE waiters_done_;
  // An auto-reset event used by the broadcast/signal thread to wait
  // for all the waiting thread(s) to wake up and be released from the
  // semaphore. 

  size_t was_broadcast_;
  // Keeps track of whether we were broadcasting or signaling.  This
  // allows us to optimize the code if we're just signaling.
} pthread_cond_t;

typedef HANDLE pthread_mutex_t;


typedef struct {
  pthread_mutex_t mtx;
  pthread_cond_t  cond;
} smileCond;


inline int 
pthread_cond_init (pthread_cond_t *cv
                   )
{
  cv->waiters_count_ = 0;
  cv->was_broadcast_ = 0;
  cv->sema_ = CreateSemaphore (NULL,       // no security
                                0,          // initially 0
                                0x7fffffff, // max count
                                NULL);      // unnamed 
  InitializeCriticalSection (&cv->waiters_count_lock_);
  cv->waiters_done_ = CreateEvent (NULL,  // no security
                                   FALSE, // auto-reset
                                   FALSE, // non-signaled initially
                                   NULL); // unnamed

  return 1;
}

inline int
pthread_cond_wait (pthread_cond_t *cv, 
                   pthread_mutex_t *external_mutex)
{
  // Avoid race conditions.
  EnterCriticalSection (&cv->waiters_count_lock_);
  cv->waiters_count_++;
  LeaveCriticalSection (&cv->waiters_count_lock_);

  // This call atomically releases the mutex and waits on the
  // semaphore until <pthread_cond_signal> or <pthread_cond_broadcast>
  // are called by another thread.
  SignalObjectAndWait (*external_mutex, cv->sema_, INFINITE, FALSE);

  // Reacquire lock to avoid race conditions.
  EnterCriticalSection (&cv->waiters_count_lock_);

  // We're no longer waiting...
  cv->waiters_count_--;

  // Check to see if we're the last waiter after <pthread_cond_broadcast>.
  int last_waiter = cv->was_broadcast_ && cv->waiters_count_ == 0;

  LeaveCriticalSection (&cv->waiters_count_lock_);

  // If we're the last waiter thread during this particular broadcast
  // then let all the other threads proceed.
  if (last_waiter)
    // This call atomically signals the <waiters_done_> event and waits until
    // it can acquire the <external_mutex>.  This is required to ensure fairness. 
    SignalObjectAndWait (cv->waiters_done_, *external_mutex, INFINITE, FALSE);
  else
    // Always regain the external mutex since that's the guarantee we
    // give to our callers. 
    WaitForSingleObject (*external_mutex, INFINITE);

  return 1;
}

inline int
wpthread_cond_timedwait (pthread_cond_t *cv, 
                   pthread_mutex_t *external_mutex, long Msec)
{
  // Avoid race conditions.
  EnterCriticalSection (&cv->waiters_count_lock_);
  cv->waiters_count_++;
  LeaveCriticalSection (&cv->waiters_count_lock_);

  // This call atomically releases the mutex and waits on the
  // semaphore until <pthread_cond_signal> or <pthread_cond_broadcast>
  // are called by another thread.
  SignalObjectAndWait (*external_mutex, cv->sema_, Msec, FALSE);

  // Reacquire lock to avoid race conditions.
  EnterCriticalSection (&cv->waiters_count_lock_);

  // We're no longer waiting...
  cv->waiters_count_--;

  // Check to see if we're the last waiter after <pthread_cond_broadcast>.
  int last_waiter = cv->was_broadcast_ && cv->waiters_count_ == 0;

  LeaveCriticalSection (&cv->waiters_count_lock_);

  // If we're the last waiter thread during this particular broadcast
  // then let all the other threads proceed.
  if (last_waiter)
    // This call atomically signals the <waiters_done_> event and waits until
    // it can acquire the <external_mutex>.  This is required to ensure fairness. 
    SignalObjectAndWait (cv->waiters_done_, *external_mutex, INFINITE, FALSE);
  else
    // Always regain the external mutex since that's the guarantee we
    // give to our callers. 
    WaitForSingleObject (*external_mutex, INFINITE);

  return 1;
}

inline int
pthread_cond_signal (pthread_cond_t *cv)
{
  EnterCriticalSection (&cv->waiters_count_lock_);
  int have_waiters = cv->waiters_count_ > 0;
  LeaveCriticalSection (&cv->waiters_count_lock_);

  // If there aren't any waiters, then this is a no-op.  
  if (have_waiters)
    ReleaseSemaphore (cv->sema_, 1, 0);
}

inline int
pthread_cond_broadcast (pthread_cond_t *cv)
{
  // This is needed to ensure that <waiters_count_> and <was_broadcast_> are
  // consistent relative to each other.
  EnterCriticalSection (&cv->waiters_count_lock_);
  int have_waiters = 0;

  if (cv->waiters_count_ > 0) {
    // We are broadcasting, even if there is just one waiter...
    // Record that we are broadcasting, which helps optimize
    // <pthread_cond_wait> for the non-broadcast case.
    cv->was_broadcast_ = 1;
    have_waiters = 1;
  }

  if (have_waiters) {
    // Wake up all the waiters atomically.
    ReleaseSemaphore (cv->sema_, cv->waiters_count_, 0);

    LeaveCriticalSection (&cv->waiters_count_lock_);

    // Wait for all the awakened threads to acquire the counting
    // semaphore. 
    WaitForSingleObject (cv->waiters_done_, INFINITE);
    // This assignment is okay, even without the <waiters_count_lock_> held 
    // because no other waiter threads can wake up to access it.
    cv->was_broadcast_ = 0;
  }
  else
    LeaveCriticalSection (&cv->waiters_count_lock_);
  
  return 1;
}
*/

// if vista or > , use native windows condition variables
#ifdef __VISTA

typedef struct {
  CRITICAL_SECTION mtx;
  CONDITION_VARIABLE  cond;
} smileCond;

#define smileCondInitVar(COND) InitializeCriticalSection(&(COND.mtx)); InitializeConditionVariable(&(COND.cond));
#define smileCondCreate(COND) InitializeCriticalSection(&(COND.mtx)); InitializeConditionVariable(&(COND.cond));
#define smileCondWait(COND) { EnterCriticalSection(&(COND.mtx)); \
                              SleepConditionVariableCS(&(COND.cond), &(COND.mtx), INFINITE); \
							                LeaveCriticalSection(&(COND.mtx)); }

#define smileCondWaitWMtx(COND,MTX) { SleepConditionVariableCS(&(COND.cond), &(MTX), INFINITE); }

// this function returns !=0 upon timeout and 0 upon success (i.e. signal!)
//#include <time.h>
#define smileCondTimedWait(COND,MSEC) {  EnterCriticalSection(&(COND.mtx)); \
                                         SleepConditionVariableCS(&(COND.cond), &(COND.mtx), MSEC); \
                                         LeaveCriticalSection(&(COND.mtx)); }

#define smileCondTimedWaitWMtx(COND,MSEC,MTX) { SleepConditionVariableCS(&(COND.cond), &(COND.mtx), MSEC); }

#define smileCondSignal(COND) { EnterCriticalSection(&(COND.mtx)); \
                                WakeConditionVariable(&(COND.cond)); \
                                LeaveCriticalSection(&(COND.mtx)); }
#define smileCondSignalRaw(COND) { WakeConditionVariable(&(COND.cond)); }
#define smileCondBroadcast(COND) { EnterCriticalSection(&(COND.mtx)); \
                                   WakeAllConditionVariable(&(COND.cond)); \
                                   LeaveCriticalSection(&(COND.mtx)); }
#define smileCondBroadcastRaw(COND) { WakeAllConditionVariable(&(COND.cond)); }
#define smileCondDestroy(COND)  { DeleteCriticalSection(&(COND.mtx)); }

#else // provide a little hacked implementation for older windowses ...

typedef struct
{
  int waiters_count_;
  // Count of the number of waiters.

  CRITICAL_SECTION waiters_count_lock_;
  // Serialize access to <waiters_count_>.

  int release_count_;
  // Number of threads to release via a <pthread_cond_broadcast> or a
  // <pthread_cond_signal>. 
  
  int wait_generation_count;
  // Keeps track of the current "generation" so that we don't allow
  // one thread to steal all the "releases" from the broadcast.

  HANDLE event_;
  // A manual-reset event that's used to block and release waiting
  // threads. 
} pthread_cond_t;

typedef CRITICAL_SECTION pthread_mutex_t;

typedef struct {
  pthread_mutex_t mtx;
  pthread_cond_t  cond;
} smileCond;



inline int 
pthread_cond_init (pthread_cond_t *cv)
{
  cv->waiters_count_ = 0;
  cv->wait_generation_count = 0;
  cv->release_count_ = 0;

  // Create a manual-reset event.
  cv->event_ = CreateEvent (NULL,  // no security
                            TRUE,  // manual-reset
                            FALSE, // non-signaled initially
                            NULL); // unnamed

  InitializeCriticalSection(&(cv->waiters_count_lock_));

  return 1;
}

inline int
pthread_cond_wait (pthread_cond_t *cv,
                   pthread_mutex_t *external_mutex)
{
  // Avoid race conditions.
  EnterCriticalSection (&(cv->waiters_count_lock_));

  // Increment count of waiters.
  cv->waiters_count_++;

  // Store current generation in our activation record.
  int my_generation = cv->wait_generation_count;

  LeaveCriticalSection (&(cv->waiters_count_lock_));
  LeaveCriticalSection (external_mutex);

  for (;;) {
    // Wait until the event is signaled.
    WaitForSingleObject (cv->event_, INFINITE);

    EnterCriticalSection (&(cv->waiters_count_lock_));
    // Exit the loop when the <cv->event_> is signaled and
    // there are still waiting threads from this <wait_generation>
    // that haven't been released from this wait yet.
    int wait_done = cv->release_count_ > 0
                    && cv->wait_generation_count != my_generation;
    LeaveCriticalSection (&(cv->waiters_count_lock_));

    if (wait_done)
      break;
  }

  EnterCriticalSection (external_mutex);
  EnterCriticalSection (&(cv->waiters_count_lock_));
  cv->waiters_count_--;
  cv->release_count_--;
  int last_waiter = cv->release_count_ == 0;
  LeaveCriticalSection (&(cv->waiters_count_lock_));

  if (last_waiter)
    // We're the last waiter to be notified, so reset the manual event.
    ResetEvent (cv->event_);

  return 1;
}

inline int
pthread_cond_timedwait (pthread_cond_t *cv,
                   pthread_mutex_t *external_mutex,
                   long msec)
{
  // Avoid race conditions.
  EnterCriticalSection (&(cv->waiters_count_lock_));

  // Increment count of waiters.
  cv->waiters_count_++;

  // Store current generation in our activation record.
  int my_generation = cv->wait_generation_count;

  LeaveCriticalSection (&(cv->waiters_count_lock_));
  LeaveCriticalSection (external_mutex);

  for (;;) {
    // Wait until the event is signaled.
    int ret = WaitForSingleObject (cv->event_, msec);

    EnterCriticalSection (&(cv->waiters_count_lock_));
    // Exit the loop when the <cv->event_> is signaled and
    // there are still waiting threads from this <wait_generation>
    // that haven't been released from this wait yet.
    int wait_done = cv->release_count_ > 0
                    && cv->wait_generation_count != my_generation;
    LeaveCriticalSection (&(cv->waiters_count_lock_));

	if (ret == WAIT_TIMEOUT) break;

    if (wait_done)
      break;
  }

  EnterCriticalSection (external_mutex);
  EnterCriticalSection (&(cv->waiters_count_lock_));
  cv->waiters_count_--;
  cv->release_count_--;
  int last_waiter = cv->release_count_ == 0;
  LeaveCriticalSection (&(cv->waiters_count_lock_));

  if (last_waiter)
    // We're the last waiter to be notified, so reset the manual event.
    ResetEvent (cv->event_);

  return 1;
}

inline int
pthread_cond_signal (pthread_cond_t *cv)
{
  EnterCriticalSection (&(cv->waiters_count_lock_));
  if (cv->waiters_count_ > cv->release_count_) {
    SetEvent (cv->event_); // Signal the manual-reset event.
    cv->release_count_++;
    cv->wait_generation_count++;
  }
  LeaveCriticalSection (&(cv->waiters_count_lock_));
  return 1;
}

inline int
pthread_cond_broadcast (pthread_cond_t *cv)
{
  EnterCriticalSection (&(cv->waiters_count_lock_));
  if (cv->waiters_count_ > 0) {  
    SetEvent (cv->event_);
    // Release all the threads in this generation.
    cv->release_count_ = cv->waiters_count_;

    // Start a new generation.
    cv->wait_generation_count++;
  }
  LeaveCriticalSection (&cv->waiters_count_lock_);
  return 1;
}

inline int
pthread_cond_destroy (pthread_cond_t *cv) 
{
  DeleteCriticalSection(&(cv->waiters_count_lock_));
  CloseHandle(cv->event_);
  return 1;
}

#define smileCondInitVar(COND) 
#define smileCondCreate(COND) InitializeCriticalSection(&(COND.mtx)); pthread_cond_init(&(COND.cond));
#define smileCondWait(COND) {  EnterCriticalSection(&(COND.mtx)); \
                              pthread_cond_wait(&(COND.cond), &(COND.mtx)) \
							  LeaveCriticalSection(&(COND.mtx)); }

#define smileCondWaitWMtx(COND,MTX) pthread_cond_wait(&(COND.cond), &(MTX))

// this function returns !=0 upon timeout and 0 upon success (i.e. signal!)
#include <time.h>
#define smileCondTimedWait(COND,MSEC) {  EnterCriticalSection(&(COND.mtx)); \
                                        pthread_cond_timedwait(&(COND.cond), &(COND.mtx), MSEC); \
                                        LeaveCriticalSection(&(COND.mtx)); }

#define smileCondTimedWaitWMtx(COND,MSEC,MTX) { pthread_cond_timedwait(&(COND.cond), &(MTX), MSEC); }

#define smileCondSignal(COND) { EnterCriticalSection(&(COND.mtx)); \
                                pthread_cond_signal(&(COND.cond)); \
                                LeaveCriticalSection(&(COND.mtx)); }
#define smileCondSignalRaw(COND) { pthread_cond_signal(&(COND.cond)); }
#define smileCondBroadcast(COND) { EnterCriticalSection(&(COND.mtx)); \
                                   pthread_cond_broadcast(&(COND.cond)); \
                                   LeaveCriticalSection(&(COND.mtx)); }
#define smileCondBroadcastRaw(COND) pthread_cond_broadcast(&(COND.cond))
#define smileCondDestroy(COND)  DeleteCriticalSection(&(COND.mtx)); pthread_cond_destroy(&(COND.cond));
/* TODO: destroy condition */

#endif // __VISTA

//#define smileSleep(msec)  usleep((msec)*1000)
#define smileSleep(msec)  Sleep( msec )
//#define smileYield()      sched_yield()
#define smileYield()   SwitchToThread() //Sleep( 0 )



#endif // __WINDOWS
#endif // HAVE_PTHREAD


//--------------------------------------------------

#include <exceptions.hpp>
#include <smileUtil.h>

//#define XERCESC_NS  xercesc_3_0

#endif  // __SMILE_COMMON_H
