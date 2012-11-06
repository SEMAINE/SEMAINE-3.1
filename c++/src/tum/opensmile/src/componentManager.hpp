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


#ifndef __COMPONENT_MANAGER_HPP
#define __COMPONENT_MANAGER_HPP

#include <smileCommon.hpp>
#include <smileComponent.hpp>

// this is the name of the configuration instance in the config file the component manager will search for:
#define CM_CONF_INST  "componentInstances"


// global component list: -----------------
#undef class
class DLLEXPORT cComponentManager;
typedef sComponentInfo * (*registerFunction)(cConfigManager *_confman, cComponentManager *_compman);
typedef void (*unRegisterFunction)();

extern DLLEXPORT const registerFunction componentlist[];

// default number of initial component variable space and plugin variable space to allocate:
#define COMPONENTMANAGER_DEFAULT_NCOMPS  50

// Max. iterations for register type, register instance, configure, and finalise process of components
#define MAX_REG_ITER  4
#define MAX_REGI_ITER 2
#define MAX_CONF_ITER 4  // <- should be nComponents!!
#define MAX_FIN_ITER  4


// openSMILE component runner thread status:
#define THREAD_ACTIVE   0
#define THREAD_WAIT_A   1
#define THREAD_WAIT_B   2
#define THREAD_END      3
#define THREAD_INACTIVE 4

typedef struct {
  cComponentManager *obj;
  long long maxtick;
  int threadId;
  int status;
} sThreadData;


class DLLEXPORT cComponentManager {
protected:
  int printPlugins;
//#ifdef DEBUG
  int execDebug;
//#endif
#ifdef __WINDOWS
  HINSTANCE *handlelist;
#else
  void **handlelist;
#endif

  registerFunction * regFnlist;
  int nPluginHandles, nPluginHandlesAlloc;

  int loadPlugin(const char * path, const char * fname);
  int registerPlugins();
  int checkPluginFilename(const char * str);

  // you may overwrite this method in a derived class to implement custom processing in every tick
  // return 1 on success and 0 on failure (a return value of 0 will terminate the processing loop)
  // threadID = -1 ==> single threaded mode   threadID >= 0  ==> multi-threaded mode with 1 or more threads
  virtual int userOnTick(long long tickNr, int EOI=0, int threadID=-1) { return 1; }

public:
  static void registerType(cConfigManager *_confman);

  cComponentManager(cConfigManager *_confman, const registerFunction _clist[] = componentlist);               // create component manager

  int compIsDm(const char *_compn);
  int ciRegisterComps(int _dm);
  int ciConfigureComps(int _dm);
  int ciFinaliseComps(int _dm, int *_n=NULL);
  int ciConfFinComps(int _dm, int *_n=NULL);

  void createInstances(int readConfig=1); // read config file and create instances as specified in config file

  // end-of-input handling:
  void setEOI() { EOI = 1; }
  int isEOI() { return EOI; }

  long long tickLoopA(long long maxtick, int threadId, sThreadData *_data);
  void controlLoopA(void);

  long tick(int threadId, long long tickNr, long lastNrun=-1);  // call all components to process one time step ('tick')

  // this function calls tick() until 0 components run successfully, returns total number of ticks
  long long runSingleThreaded(long long maxtick=-1);

  // still a bit todo, basically same as runSingleThreaded with multiple threads, total number of ticks not yet fully supported!!
  long long runMultiThreaded(long long maxtick=-1);

  int addComponent(const char *_instname, const char *_type, const char *_ci=NULL, int _threadId=-1);  // create + register
  cSmileComponent * createComponent(const char *_name, const char *_type);
  cSmileComponent * createComponent(const char *_name, int n);

  // return number of registered component types
  int getNtypes() { return nCompTs; } 

  // filter=0: no filter, filter=1 : no abstract components, filter=2 non abstract and no non-Dmem (non-standalone) components (= sub-components)
  const char * getComponentType(int i, int filter=2, int *isAbstract=NULL, int *isNoDmem=NULL); // get name of component Type "i". abstract=1 = include abstract types (if abstract = 0 , then NULL will be returned if the type is abstract), or you may use the isAbstract flag, which is set by this function

  // get description of component "i"
  const char * getComponentDescr(int i); 

  int registerComponent(sComponentInfo *c, int noFree=0); // register a component type
  int findComponentType(const char *_type);
  int registerComponentTypes(const registerFunction _clist[]); // register component types by scanning given struct

  // filter=0: no filter, filter=1 : no abstract components, filter=2 non abstract and no non-Dmem (non-standalone) components (= sub-components)
  int printComponentList(int filter=2, int details=1);

  int registerComponentInstance(cSmileComponent * _component, const char *_typename, int _threadId=-1);  // register a component, return value: component id
  void unregisterComponentInstance(int id, int noDM=0);  // unregister and free component object
  int findComponentInstance(const char *_compname) const;
  cSmileComponent * getComponentInstance(int n);
  const char * getComponentInstanceType(int n);
  cSmileComponent * getComponentInstance(const char *_compname);
  const char * getComponentInstanceType(const char *_compname);

  // send inter component messages directly to component:
  int sendComponentMessage(const char *_compname, cComponentMessage *_msg);
  // send inter component messages directly to component:
  int sendComponentMessage(int compid, cComponentMessage *_msg);

  // get time in seconds (accurate up to the millisecond or microsecond (depending on the system)) since creation if componentManager
  double getSmileTime();

  void resetInstances(void);  // delete all component instances and reset componentManger to state before createInstances
  void waitForAllThreads(int threadID);
  void waitForController(int threadID, int stage);
  void decreaseNActive();

  // request program loop termination (works for single and multithreaded)
  void requestAbort() {
    smileMutexLock(abortMtx);
    abortRequest = 1;
    if (nThreads != 1) { smileCondSignal(controlCond); }
    smileMutexUnlock(abortMtx);
  }
  int isAbort() { 
    smileMutexLock(abortMtx);
    int ret = abortRequest;
    smileMutexUnlock(abortMtx);
    return ret;
  }

  int isReady() {
    return ready;
  }

  ~cComponentManager();              // unregister and free all component objects

private:
  cConfigManager *confman;
  void fetchConfig(); // read config from confman

  int printLevelStats;

  struct timeval startTime;
  int nCompTs, nCompTsAlloc;
  sComponentInfo *compTs; // component types

  int ready;    // flag that indicates if all components are set up and ready...
  int isConfigured;
  int isFinalised;
  int EOI;

  //long long tickNr;
  int nComponents, nComponentsAlloc;
  int nActiveComponents;  // number of non-passive (i.e. non-dataMemory components)
  int nThreads;
  int threadPriority; /* default thread priority, or single thread priority */

  int lastComponent;
  cSmileComponent ** component;   // array of component instances
  char **componentInstTs;    // component instance corresponding type names
  int  * componentThreadId;

  long messageCounter;

  smileMutex    messageMtx;
  smileMutex    waitEndMtx;
  smileCond     waitEndCond;
  smileCond     waitControllerCond;
  int waitEndCnt;

  // mutli thread sync variables:
  smileMutex    syncCondMtx;
  smileMutex    abortMtx;
  smileCond     syncCond;
  smileCond     controlCond;
  int nWaiting, nProbe, nActive;
  int runStatus, compRunFlag, probeFlag, controllerPresent; // communication flags between controller thread and component runner threads
  int globalRunState;
  int abortRequest, endOfLoop;

  int getNextComponentId();
};


#endif  // __COMPONENT_MANAGER_HPP
