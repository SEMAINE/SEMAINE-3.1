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

   ConfigManager
                     -> config reader backends


*/


class cConfigManager {
  public:
    cConfigManager(cConfigReader * _reader);  // initialize object with primary config reader
    int addReader(cConfigReader * _reader, int priority);


    // fast retrieval by id, only works for first hierarchy (i.e. non-hierarchical config)
    char getParameterValue(int _id);                               // retrieval by id (fast...)
    long getParameterValue(int _id);                               // retrieval by id (fast...)
    char *getParameterValue(int _id);                              // retrieval by id (fast...)
    char **getParameterValue(int _id, int *n);                     // string array retrieval by id (fast...)
    double getParameterValue(int _id);                             // retrieval by id (fast...)

    // these functions get parameters (even array and hierarchical, based on the following syntax:
    // Name1.Name2[x].Name      : arrays are denoted by postfix [X], where X is the index to retrieve, hierarchies are separated by a "."
    char getParameterValue(char *_module, char *_parameterName);  // retrieval by name and module (slow)
    long getParameterValue(char *_module, char *_parameterName);  // retrieval by name and module (slow)
    char *getParameterValue(char *_module, char *_parameterName); // retrieval by name and module (slow)
    double getParameterValue(char *_module, char *_parameterName);// retrieval by name and module (slow)

    void printUsage();  // using description and parameter names, print a usage information
    void loadConfig();  // load and parse config from all readers, make it available to getParameterValue function
    void loadConfig(int i);  // load and parse config from reader #i, make it available to getParameterValue function

    ~cConfigManager();

    // parameter table:
    cConfigParameters *param;  // NOT an array, only pointer to ONE object

  private:
    // config reader backends:
    int nReadersAlloc;       // number of allocated config reader array elements
    int nReaders;            // number of configured config file readers
    cConfigReader * reader;  // array of config reader objects
    int * readerPriorities;  // priorities of config readers (primary config reader always has priority 0)

};



/*

  central configuration manager for all components
  provides standard interface for various plugable config reader classes (e.g. config file, commandline, database, etc.)
  
  this configManager ONLY manages the configuration of specific parameters for components (or component instances),
  it does NOT manage the components to be initialized and the component dependencies (i.e. initialisation of readers and writers of each component)

  module names in this class refer to the names of the component instances.
  i.e. if you create a component instance config be sure to use the same module (aka. component) names in the component parameter configuration file (this is the one read and managed by this module)

  (however, there is one way to put the component initialisation config in this class:
    you need to create a component "compInitializer"  (or use the componentManager)
    and then configure this component
  )
 */

#define CONFIGMANAGER_DEFAULT_NREADERS 4
#define CONFIGPARAMETERS_DEFAULT_NPARAM 50

enum eParamType {
  UNKNOWN=0,
  INT,    // long
  FLOAT,  // double
  CHAR,   // char
  STRING,  // char *
  STRING_ARRAY,   // char **
  VIRTUAL,  // virtual parameter : i.e. only description for usage information, no real parameter and no value?
           // use this for "dynamic" parameters, i.e. if another parameter in the config specifies the name of this parameter
  DYNMAIC,
  META     // parameter containing sub-parameter set , but no value
};

class cConfigParameter {
  public:

  eParamType type;
  char *module;      // actually only relevant on the first hierarchy, therefore, this might be NULL on sub levels
  char *name;
  char *description;
  
  long intValue;     // integer value
  double fpValue;    // floating point value
  char *str;    // string
  int nArrEl;
  char **strArr;   // string array
  cConfigParameters *clsValue;    // class (e.g. cConfigParameters for hierarchies)
  
  cConfigParameter(eParamType _type, char *_module, char *_name);
  virtual ~cConfigParameter();
};

#include <string.h>

cConfigParameter::cConfigParameter(eParamType _type, char *_module, char *_name) :
  type(_type),
  module(NULL),
  name(NULL),
  description(NULL),
  intValue(0),
  fpValue(0.0)
  str(NULL),
  nArrEl(0),
  strArr(NULL),
  clsValue(NULL)
{
  if (_module != NULL) module=strdup(_module);
  if (_name != NULL) _name=strdup(_name);
  if (_description != NULL) description=strdup(_description);
}

cConfigParameter::~cConfigParameter()
{
  if (module != NULL) free(module);
  if (name != NULL) free(name);
  if (description != NULL) free(description);
  if (str != NULL) free(str);
  if ((nArrEl > 0)&&(strArr!=NULL)) {
    int i;
    for (i=0; i<nArrEl; i++) { free(strArr[i]); }
    free(strArr);
  }
  if (clsValue != NULL) delete clsValue;
}

/***************************************/

class cConfigParameters {
  public:
    cConfigParameters();
    
    int registerParameter(const char *_module, const char *_parameterName, eParamType _type);   // register a parameter and return a paramter id, by which the parameter can be retrieved
    int registerParameter(const char *_module, const char *_parameterName, const char *_description, eParamType _type);   // register a parameter and return a paramter id, by which the parameter can be retrieved, additionally specify a description which will be displayed by showUsage() of the configManager

    void paramSetDefault(int _id, char _val);
    void paramSetDefault(int _id, long _val);
    void paramSetDefault(int _id, char * _val);
    void paramSetDefault(int _id, double _val);
    // special array parameters:
    void paramSetDefault(int _id, char ** _val, int n);
    // meta parameter (one that contains another parameter set)
    void paramSetDefault(int _id, cConfigParameters *_sub);

    char getParameterValue(int _id);                               // retrieval by id (fast...)
    char getParameterValue(char *_module, char *_parameterName);  // retrieval by name and module (slow)
    long getParameterValue(int _id);                               // retrieval by id (fast...)
    long getParameterValue(char *_module, char *_parameterName);  // retrieval by name and module (slow)
    char *getParameterValue(int _id);                              // retrieval by id (fast...)
    char *getParameterValue(char *_module, char *_parameterName); // retrieval by name and module (slow)
    double getParameterValue(int _id);                             // retrieval by id (fast...)
    double getParameterValue(char *_module, char *_parameterName);// retrieval by name and module (slow)
    // special array parameters:
    char **getParameterValue(int _id, int *n);                              // retrieval by id (fast...)
    char **getParameterValue(char *_module, char *_parameterName, int *n); // retrieval by name and module (slow)
    // hierarchical parameters:
    cConfigParameters *getParameterValue(int _id);                              // retrieval by id (fast...)
    cConfigParameters *getParameterValue(char *_module, char *_parameterName); // retrieval by name and module (slow)

    void loadConfig(cConfigReader *_r);
    
    virtual ~cConfigParameters();
    
  private:
    getNextParamIdx();
    int nParam, nParamAlloc;
    cConfigParameter **param;
    
};


cConfigParameters::cConfigParameters() :
  nParam(0),
  nParamAlloc(0),
  param(NULL)
{
  param = (cConfigParameter **)calloc(1,sizeof(cConfigParameter *)*CONFIGPARAMETERS_DEFAULT_NPARAM);
  if (param!= NULL) { nParamAlloc = CONFIGPARAMETERS_DEFAULT_NPARAM; }
  else {
    // throw...
  }
}

int cConfigParameters::getNextParamIdx()
{
  if (nParam >= nParamAlloc) {
    cConfigParameter **tmp;
    tmp = (cConfigParameter **)crealloc(param,
                                          nParamAlloc*sizeof(cConfigParameter *),
                                          (nParamAlloc+CONFIGPARAMETERS_DEFAULT_NPARAM)*sizeof(cConfigParameter *)));
    if (tmp != NULL) {
      nParamAlloc += CONFIGPARAMETERS_DEFAULT_NPARAM;
      param = tmp;
    }else {
      //throw...
      
      return nParam;
    }
  }
  return nParam++;
}

// register a parameter and return a paramter id, by which the parameter can be retrieved
int cConfigParameters::registerParameter(const char *_module, const char *_parameterName, eParamType _type)
{
  int idx = getNextParamIdx();
  param[idx] = new cConfigParameter(_type,_module,_name);
  if (param[idx] != NULL) {
    return idx;
  } else { return -1; }
}


// register a parameter and return a paramter id, by which the parameter can be retrieved, additionally specify a description which will be displayed by showUsage() of the configManager
int cConfigParameters::registerParameter(const char *_module, const char *_parameterName, const char *_description, eParamType _type)
{
  int idx = registerParameter(_module, _parameterName, _type);
  if ((idx >= 0)&&(_description!=NULL)) param[idx]->description = strdup(_description);
  return idx;
}

void cConfigParameters::paramSetDefault(int _id, char _val)
{
  paramSetDefault(_id, (long)_val);
}

void cConfigParameters::paramSetDefault(int _id, long _val)
{
  if ((_id >=0)&&(_id < nParam)) {
    param[_id]->intValue = _val;
    param[_id]->fpValue = (double)_val;
  }
}


void cConfigParameters::paramSetDefault(int _id, char * _val)
{
  if ((_id >=0)&&(_id < nParam)) {
    param[_id]->intValue = _val;
    param[_id]->fpValue = (double)_val;
  }
}

void cConfigParameters::paramSetDefault(int _id, double _val)
{
  if ((_id >=0)&&(_id < nParam)) {
    param[_id]->fpValue = _val;
    param[_id]->intValue = (long)_val;
  }
}

// special string array parameter:
void cConfigParameters::paramSetDefault(int _id, char ** _val, int n)
{
  if ((_id >=0)&&(_id < nParam)) {
    param[_id]->strArr = (char**) calloc(1,sizeof(char*)*n);
    if (param[_id]->strArr != NULL) { 
      param[_id]->nArrEl = n;
      int i;
      for (i=0; i<n; i++) {
        if (_val[i] != NULL) param[_id]->strArr[i] = strdup(_val[i]);
      }
    }
  }
}

// meta parameter (one that contains another parameter set)
void cConfigParameters::paramSetDefault(int _id, cConfigParameters *_sub)
{
  if ((_id >=0)&&(_id < nParam)) {
    param[_id]->clsValue = _sub;
  }
}

// find locally by given module and parameter name, return parameter index
// -1 is returned if the parameter was not found!
int cConfigParameters::findParameterByName(char *_module, char *_parameterName)
{
  int i;
  for (i=0; (i<nParam)&&(i<nParamAlloc); i++) {
    if (!strcmp(_module,param[i]->module)) {
      if (!strcmp(_parameterName,param[i]->name)) { return i; }
  }
  return -1;
}

// parameter retrieval by id (fast)
char cConfigParameters::getParameterValue(int _id)
{
  if ((_id >= 0)&&(_id < nParam)) {
    return (char)(param[_id]->intValue);
  }
}

long cConfigParameters::getParameterValue(int _id)
{
  if ((_id >= 0)&&(_id < nParam)) {
    return param[_id]->intValue;
  } else {
    // throw...
  }
  
}

double cConfigParameters::getParameterValue(int _id)
  if ((_id >= 0)&&(_id < nParam)) {
    return param[_id]->fpValue;
  } else {
    // throw...
  }
}

// content pointed to by returned pointers may not be modified!
// also pointers may not be freed!
char * cConfigParameters::getParameterValue(int _id)
{
  if ((_id >= 0)&&(_id < nParam)) {
    return param[_id]->str;
  } else {
    // throw...
    
  }
}

char ** cConfigParameters::getParameterValue(int _id, int *n) 
{
  if ((_id >= 0)&&(_id < nParam)) {
    if (*n != NULL) {
      *n = param[_id]->nArrEl;
      return param[_id]->strArr;
    }
    return NULL;
  } else {
    // throw...
    
  }
}

cConfigParameters * cConfigParameters::getParameterValue(int _id)
{
  if ((_id >= 0)&&(_id < nParam)) {
    return param[_id]->clsValue;
  }
}

// parameter retrieval by name and module (slow)
char cConfigParameters::getParameterValue(char *_module, char *_parameterName)
{
  return
}

char getParameterValue(char *_module, char *_parameterName);  
long getParameterValue(char *_module, char *_parameterName);  // retrieval by name and module (slow)
char *getParameterValue(char *_module, char *_parameterName); // retrieval by name and module (slow)
double getParameterValue(char *_module, char *_parameterName);// retrieval by name and module (slow)
// special array parameters:
char **getParameterValue(char *_module, char *_parameterName, int *n); // retrieval by name and module (slow)
// hierarchical parameters:
cConfigParameters *getParameterValue(char *_module, char *_parameterName); // retrieval by name and module (slow)


cConfigParameters::~cConfigParameters()
{
  if ((nParamAlloc>0)&&(param!=NULL)) {
    int i;
    for (i=0; i<nParamAlloc; i++) if (param[i] != NULL) delete param[i];
    free(param);
  }
}
                                      
/**** main class: configManager ****/


/** config readers   **/

struct sParam {
  char * module;
  int nNames;
  char ** names;
  int index;
  
  char * rawValue;
  long value;
  char * value;  // same as rawValue if type==string
  char value;
  double value;
  int type;   // 0(unknown) by default when loading from text sources
              // other sources may provide a type, so it could be set
              // if type==0 (unknown), then the only valid value field that should be read is "char *rawValue"

  int isRead; // this flag gets set to 1 (default=0) by getParameter.
              // it is used by collectParameterStatus() to find unused (i.e. unknown) parameters.
};


// virtual class that defines configReader interface
// and provides definition of parameter memory
class cConfigReader {
  public:
    cConfigReader();  // initialize reader, load, and parse config

    // getParameter functions for various parameter types
    // return value indicates whether the parameter request was found in the config
    //   0 = not found ,  1 = found(success) ,  2=found(wrong type) ... other values reserved for future use
    virtual int getParameter(char *_module, char **_parameterName, int _nNames, int _index, long *_val);
    virtual int getParameter(char *_module, char **_parameterName, int _nNames, int _index, char *_val);
    virtual int getParameter(char *_module, char **_parameterName, int _nNames, int _index, char **_val);
    virtual int getParameter(char *_module, char **_parameterName, int _nNames, int _index, double *_val);
    virtual int getParameter(char *_module, char *_parameterName, long *_val);
    virtual int getParameter(char *_module, char *_parameterName, char *_val);
    virtual int getParameter(char *_module, char *_parameterName, char **_val);
    virtual int getParameter(char *_module, char *_parameterName, double *_val);

    virtual ~cConfigReader();
    
  private:
    // internal parameter memory, where parameters loaded from file/commandline/etc. are stored
    // by default all parameters are treated as text(string) because they are loaded from text sources, the getParameter functions tries to convert and returns error or success
    int nParam, nParamAlloc;
    struct sParam * param;
    
    int addParam(sParam *_p);      // add parameter to "param" array (used by loadConfig)
    
    // load helper function:
    virtual int loadConfig();  // this is pure virtual and must be implemented by the derived class
    
    int collectParameterStatus();  // find "unused" parameters present in config
    virtual int handleParameterStatus();  // pure virtual.. to be implemented by derived class
                                          // to handle behaviour when unknown parameters are given in config
};


class cConfigReaderFile : public cConfigReader {
  public:
    cConfigReader(const char * _filename);

    virtual ~cConfigReader();
    
  private:
    char *filename;
    
    // load helper function:
    virtual int loadConfig();  // implementation, loads and parses a config file
    virtual int handleParameterStatus();   // maybe display warning: unkown option in config file... etc.
};
