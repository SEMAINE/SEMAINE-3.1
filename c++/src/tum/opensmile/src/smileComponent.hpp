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


#ifndef __SMILE_COMPONENT_HPP
#define __SMILE_COMPONENT_HPP

#include <smileCommon.hpp>
#include <configManager.hpp>

#define COMPONENT_DESCRIPTION_XXXX  "example description"
#define COMPONENT_NAME_XXXX         "exampleName"

#undef class
class DLLEXPORT cComponentManager;
class DLLEXPORT cSmileComponent;

#define CMSG_textLen      64
#define CMSG_typenameLen  32
#define CMSG_nUserData     8

class DLLEXPORT cComponentMessage {
public:
  char msgtype[CMSG_typenameLen];     // message type name (used by receiver to identify message), set by constructor
  char msgname[CMSG_typenameLen];     // custom message name
  const char * sender;  // name of sender component (filled in by sendComponentMessage in cSmileComponent)
  double smileTime;  // seconds (accurate up to milliseconds) since componentManager startup (filled in by componentManager, TODO!)
  double userTime1;  // user defined time
  double userTime2;  // user defined time
  double readerTime; // readerTime in seconds (derived vom vec->tmeta->vIdx and reader period in the tick the message ist sent), can also be -1.0 if not used!
  long msgid;           // custom message id
  // -- message data --
  double floatData[CMSG_nUserData];  // 8 freely usable doubles for message data, initialized with 0
  long intData[CMSG_nUserData];      // 8 freely usable ints
  char msgtext[CMSG_textLen];     // 64 characters for message text, freely usable, initialized with all 0
  int userflag1, userflag2, userflag3;
  void * custData;      // pointer to custom message data (allocated by sender, to be freed by sender after call to sendComponentMessage)
  void * custData2;     // pointer to custom message data (allocated by sender, to be freed by sender after call to sendComponentMessage)

  cComponentMessage(const char *_type=NULL, const char *_name=NULL) :  msgid(-1), sender(NULL), smileTime(0.0), readerTime(-1.0), userflag1(0), userflag2(0), userflag3(0), custData(NULL), custData2(NULL) {
    if (_type==NULL) memset(msgtype, 0, sizeof(char)*CMSG_typenameLen);
    else strncpy(msgtype, _type, CMSG_typenameLen);
    if (_name==NULL) memset(msgname, 0, sizeof(char)*CMSG_typenameLen);
    else strncpy(msgname, _name, CMSG_typenameLen);
    memset(floatData, 0, sizeof(double)*CMSG_nUserData);
    memset(intData, 0, sizeof(long)*CMSG_nUserData);
    memset(msgtext, 0, sizeof(char)*CMSG_textLen);
  }
};

class DLLEXPORT sComponentInfo { public:
  int registerAgain;
  const char *componentName;
  const char *description;
  int abstract;   // flag that indicates whether component is 'abstract' only (i.e. without practical functionality)
  int noDmem;
  int builtIn;   // 1= built in component ; 0= dynamically linked component
  cSmileComponent * (*create) (const char *_instname);
  sComponentInfo * next;
};

// create for a real class (which implements fetchConfig() )
#define SMILECOMPONENT_CREATE(TP) cSmileComponent * TP::create(const char*_instname) { \
                                                  cSmileComponent *c = new TP(_instname); \
                                                  if (c!=NULL) c->setComponentInfo(sconfman,scname,sdescription); \
                                                  return c; \
                                                  }
                                                  
// create for an abstract class
#define SMILECOMPONENT_CREATE_ABSTRACT(TP) cSmileComponent * TP::create(const char*_instname) { return NULL; }

// static declaration in Cpp file of derived class
#define SMILECOMPONENT_STATICS(TP)  cConfigManager *TP::sconfman; \
                                    const char *TP::scname; \
                                    const char *TP::sdescription;
                                    
// static declarations in derived class (public)
#define SMILECOMPONENT_STATIC_DECL  static sComponentInfo * registerComponent(cConfigManager *_confman, cComponentManager *_compman);    \
                                    static cSmileComponent * create(const char *_instname);
                                    
// static declarations in derived class (protected)
#define SMILECOMPONENT_STATIC_DECL_PR    static cConfigManager *sconfman;    \
                                         static const char *scname;  \
                                         static const char *sdescription;

#define SMILECOMPONENT_REGCOMP(TP)  sComponentInfo * TP::registerComponent(cConfigManager *_confman, cComponentManager *_compman)
#define SMILECOMPONENT_REGCOMP_INIT  if (_confman == NULL) return NULL; \
                                     int rA = 0; \
									 sconfman = _confman;

#define SMILECOMPONENT_CREATE_CONFIGTYPE    ConfigType *ct = new ConfigType(scname); \
	                                        if (ct == NULL) OUT_OF_MEMORY;
#define SMILECOMPONENT_INHERIT_CONFIGTYPE(configtype)  ConfigType *ct = NULL; \
	                                                   const ConfigType *r = sconfman->getTypeObj(configtype); \
													   if (r == NULL) {  \
													     SMILE_WRN(4,"%s config Type not found!","configtype"); \
														 rA=1; \
													   } else { \
													     ct = new ConfigType( *(r) , scname ); \
													   }

#define SMILECOMPONENT_REGISTER_CONFIGTYPE     if (rA==0) { \
	                                             ConfigInstance *Tdflt = new ConfigInstance( scname, ct, 1 );  \
												 sconfman->registerType(Tdflt); \
                                               }

#define SMILECOMPONENT_IFNOTREGAGAIN_BEGIN  if (rA==0) {
#define SMILECOMPONENT_IFNOTREGAGAIN_END  SMILECOMPONENT_REGISTER_CONFIGTYPE }
#define SMILECOMPONENT_IFNOTREGAGAIN(__code__) SMILECOMPONENT_IFNOTREGAGAIN_BEGIN __code__ ; SMILECOMPONENT_IFNOTREGAGAIN_END

#define SMILECOMPONENT_MAKEINFO(TP)  if ((rA!=0)&&(ct!=NULL)) delete ct; \
	                                 return makeInfo(sconfman, scname, sdescription, TP::create, rA)
#define SMILECOMPONENT_MAKEINFO_ABSTRACT(TP) if ((rA!=0)&&(ct!=NULL)) delete ct; \
	                                         return makeInfo(sconfman, scname, sdescription, TP::create, rA, 1, 1)

// make info for non-datamemory components (e.g. the cFunctionalXXXX components), e.g. components that are sub-components of other components
#define SMILECOMPONENT_MAKEINFO_NODMEM(TP) if ((rA!=0)&&(ct!=NULL)) delete ct; \
	                                       return makeInfo(sconfman, scname, sdescription, TP::create, rA, 0, 1)

// make info for non-datamemory components (e.g. the cFunctionalXXXX components), e.g. components that are sub-components of other components
#define SMILECOMPONENT_MAKEINFO_NODMEM_ABSTRACT(TP) if ((rA!=0)&&(ct!=NULL)) delete ct; \
	                                                return makeInfo(sconfman, scname, sdescription, TP::create, rA, 1, 1)

#undef class
class DLLEXPORT cSmileComponent {
  private:
    int id;
    int EOI;   // flag that indicates end of input
               // i.e. if EOI is 1, myTick should show a different behaviour
               //  esp. dataReaders should return right padded matricies, in getMatrix , getNextMatrix etc..
    
    smileMutex  messageMtx;

    cComponentManager *compman;
    cSmileComponent *parent;
    char *iname;  // name of component instance
    char *cfname;  // name of config instance associated with this component instance
    //TODO: separate config instance name and component instance name!

    // profiling variables;
    int doProfile, printProfile;
    double profileCur, profileSum; // exec. time of last tick, exec time total
    struct timeval startTime;
    struct timeval endTime;

    long lastNrun;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    cConfigManager *confman;
    const char*cname;
    const char*description;

    int _isRegistered, _isConfigured, _isFinalised, _isReady;
    int _runMe;

    int manualConfig;
    int override;
    
    static sComponentInfo * makeInfo(cConfigManager *_confman,
                                     const char *_name, const char *_description,
                                     cSmileComponent * (*create) (const char *_instname),
                                     int regAgain=0, int _abstract=0, int _nodmem=0);

    cSmileComponent * getComponentInstance(const char * name);
    const char * getComponentInstanceType(const char * name);
    cSmileComponent * createComponent(const char*_name, const char*_type);
    
    long getLastNrun() { return lastNrun; }

    // name is without the instance name...
    double getDouble(const char*name) {
      return confman->getDouble_f(myvprint("%s.%s",cfname,name));
    }
    double getDouble_f(char*name) {
      double d = getDouble(name);
      if (name!=NULL) free(name);
      return d;
    }

    int getInt(const char*name) {
      return confman->getInt_f(myvprint("%s.%s",cfname,name));
    }
    int getInt_f(char*name) {
      int d = getInt(name);
      if (name!=NULL) free(name);
      return d;
    }

    const char *getStr(const char*name){
      return confman->getStr_f(myvprint("%s.%s",cfname,name));
    }
    const char * getStr_f(char*name) {
      const char * s = getStr(name);
      if (name!=NULL) free(name);
      return s;
    }

    char getChar(const char*name) {
      return confman->getChar_f(myvprint("%s.%s",cfname,name));
    }
    const char getChar_f(char*name) {
      const char c = getChar(name);
      if (name!=NULL) free(name);
      return c;
    }

    const ConfigValue *getValue(const char*name) {
      return confman->getValue_f(myvprint("%s.%s",cfname,name));
    }
    const ConfigValue * getValue_f(char*name) {
      const ConfigValue * v = getValue(name);
      if (name!=NULL) free(name);
      return v;
    }

    const ConfigValueArr *getArray(const char*name) {
      return (ConfigValueArr *)(confman->getValue_f(myvprint("%s.%s",cfname,name)));
    }
    const ConfigValueArr * getArray_f(char*name) {
      const ConfigValueArr * a = getArray(name);
      if (name!=NULL) free(name);
      return a;
    }

    int getArraySize(const char*name) {
      return (confman->getArraySize_f(myvprint("%s.%s",cfname,name)));
    }
    int getArraySize_f(char*name) {
      int s = getArraySize(name);
      if (name!=NULL) free(name);
      return s;
    }

    int isSet(const char*name) {
      return (confman->isSet_f(myvprint("%s.%s",cfname,name)));
    }
    int isSet_f(char*name) {
      int s = isSet(name);
      if (name!=NULL) free(name);
      return s;
    }

    int isAbort();
    int isEOI() { return EOI; }

    // each derived class is responsible for fetching configuration from confman or setting it manually BEFORE configureInstance is called!
    virtual void fetchConfig()=0;

    virtual void mySetEnvironment() { }
    
    virtual int myRegisterInstance(int *runMe=NULL) { return 1; }
    virtual int myConfigureInstance() { return 1; }
    virtual int myFinaliseInstance() { return 1; }
    virtual int myTick(long long t) { return 0; }


    // signal EOI to componentManager (theoretically only useful for dataSource components, however we make it accessible to all smile components)
    // NOTE: you do not need to do this explicitely.. if all components fail, EOI is assumed, then a new tickLoop is started by the component manager
    void signalEOI();

    // check if component message is of type "_type"
    int isMessageType( cComponentMessage *_msg, const char * _type) {
      if (_msg!=NULL) {
        return (!strncmp(_msg->msgtype, _type, 32));
      } else return 0;
    }

    // this function is only called with valid pointer and locked mutex. A component must override this to receive messages.
    // Return value: 0, message was not processed; != 0, message was processed. The value will be passed on to the sender
    // NOTE: the custData pointer in cComponentMessage (as well as the whole cComponentMessage object) is valid ONLY until processComponentMessage() returns! Thus, you are advised to copy data to local memory
    virtual int processComponentMessage( cComponentMessage *_msg ) { return 0; }  
    
    // this function forwards the message to the componentMananger and sets the *sender pointer correctly
    // Return value: 0, message was not processed; != 0, message was processed.
    int sendComponentMessage( const char *recepient, cComponentMessage *_msg );

    // you must call this function prior to accessing variables you have accessed in processComponentMessage(). Do not use this function in processComponentMessage() itself, though!
    void lockMessageMemory() { smileMutexLock(messageMtx); }

    // call this after accessing the variables you have accessed in processComponentMessage()
    void unlockMessageMemory() { smileMutexUnlock(messageMtx); }
    
    // get smile time from component manager
    double getSmileTime();

  public:
    // static:
    // these two must be overridden in a base class:
    //static sComponentInfo * registerComponent(cConfigManager *_confman);
    //static cSmileComponent * create(const char *_instname);
    SMILECOMPONENT_STATIC_DECL

    //method for setting config... a base class may implement it with arbirtrary parameters
    // values set will override those obtained by fetchConfig it override=1
    // if override=0, then only those which are not set are updated
    // void setConfig(..., int override=1);
    int isManualConfigSet() { return manualConfig; }
    
    // get associated component manager object, i.e. the object that created this smile component instance
    cComponentManager *getCompMan() { return compman; }

    // create component with instance name "name"
    cSmileComponent(const char *instname);  
    
    virtual void setComponentInfo(cConfigManager *cm, const char*_cname, const char*_description) {
      confman = cm; cname=_cname; description=_description;
      if (cm!=NULL) fetchConfig();
    }
    
    virtual void setComponentEnvironment(cComponentManager *_compman, int _id, cSmileComponent *parent=NULL); // set component manager reference and the component ID, used by componentManager
    
    void setConfigInstanceName(const char *_cfname) {
      if (_cfname != NULL) {
        if ((cfname != NULL)&&(cfname != iname)) { free(cfname); cfname = NULL; }
        cfname = strdup(_cfname);
      }
    }

    const char * getConfigInstanceName() { return cfname; }

    // perform component specific register operation, e.g. write/read requests with dataMemory..
    // *runMe return value for component manager : 0, don't call my tick of this component, 1, call myTick
    int registerInstance(int *runMe=NULL) {
      if (runMe != NULL) *runMe = _runMe;
      if (_isRegistered) return 1;
      _isRegistered = myRegisterInstance(runMe);
      if (runMe != NULL) _runMe = *runMe;
      return _isRegistered;
    }
    int isRegistered() { return _isRegistered; }

    int configureInstance() {
      if (_isConfigured) return 1;
      _isConfigured = myConfigureInstance();
      return _isConfigured;
    }
    int isConfigured() { return _isConfigured; }

    int finaliseInstance();
    int isFinalised() { return _isFinalised; }
    int isReady() { return _isReady; }

    int tick(long long t, int _EOI=0, long _lastNrun=-1) {
      lastNrun = _lastNrun;
      if (_EOI) {
        if (!isEOI()) {
          setEOI();
          return 1; // one "successful" tick to set up all components in EOI state
        }
      }
      if (_isReady) { 
        if (doProfile) {
          startProfile(t,_EOI);
        }
        int ret = myTick(t);
        if (doProfile) {
          endProfile(t,_EOI);
        }
        return ret;
      }
      else return 0;
    }
    
    void setProfiling(int enable=1, int print=0) {
      doProfile = enable;
      printProfile = print;
    }

    void startProfile(long long t, int _EOI);
    void endProfile(long long t, int _EOI);

    double getProfile(int sum=1) { 
      if (!sum) return profileCur;
      else return profileSum;
    }

    //TODO:: support reconf??

    const char *getInstName() const { return iname; }         // get component instance name
    const char *getTypeName() const { return cname; }        // get component type name

    virtual void setEOI() { EOI = 1; } // not.. used by component manager to signal End-of-Input to components

    // this function is called externally by the component manager if another component calls sendComponentMessage:
    int receiveComponentMessage( cComponentMessage *_msg ) {
      int ret = 0;
      if (_msg != NULL)  {
        lockMessageMemory();
        ret = processComponentMessage(_msg);
        unlockMessageMemory();
      }
      return ret;
    }

    virtual ~cSmileComponent();

};



#endif //__SMILE_COMPONENT_HPP
