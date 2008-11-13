/**
 * Summary: interfaces for thread handling
 * Description: set of generic threading related routines
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Daniel Veillard
 */

#ifndef __XML_NTHREADS_H__
#define __XML_NTHREADS_H__

#include <libxml/xmlversion.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * xmlMutex are a simple mutual exception locks.
 */
typedef struct _xmlMutex xmlMutex;
typedef xmlMutex *xmlMutexPtr;

/*
 * xmlRMutex are reentrant mutual exception locks.
 */
typedef struct _xmlRMutex xmlRMutex;
typedef xmlRMutex *xmlRMutexPtr;

#ifdef __cplusplus
}
#endif

#include <libxml/globals.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _xmlThreadHandler
{
	xmlMutexPtr (XMLCALL *newMutex) (void);
	void (XMLCALL *lockMutex) (xmlMutexPtr);
	void (XMLCALL *unlockMutex) (xmlMutexPtr);
	void (XMLCALL *freeMutex) (xmlMutexPtr);
	
	xmlRMutexPtr (XMLCALL *newRMutex) (void);
	void (XMLCALL *lockRMutex) (xmlRMutexPtr);
	void (XMLCALL *unlockRMutex) (xmlRMutexPtr);
	void (XMLCALL *freeRMutex) (xmlRMutexPtr);
	
	xmlGlobalStatePtr (XMLCALL *getGlobalState)(void);
	int (XMLCALL *getThreadId) (void);
	int (XMLCALL *isMainThread) (void);
};
typedef struct _xmlThreadHandler xmlThreadHandler;
typedef xmlThreadHandler* xmlThreadHandlerPtr;


#ifdef __cplusplus
}
#endif


#endif /* __XML_THREADS_H__ */
