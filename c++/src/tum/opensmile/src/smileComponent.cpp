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
openSMILE component hierarchy:

cSmileComponent  (basic cmdline and config functionality)
 |
 v
cProcessorComponent  - contains: Reader, Writer, ref to data mem, one or more compute components, plus interconnection logic
cComputeComponent    - implements a specific algo, clearly defined io interface
                       I: vector, O: vector
                       I: matrix, O: vector
                       (O: matrix ?? )
cReaderComponent     - reads data from dataMemory and provides frame or matrix (also registers read request)
cWriterComponent     - writes data to dataMemory (takes vector, (or matrix??)), also registers Level (data memory at the end will check, if all dependencies are fullfilled..?)
*/


/*
   component base class,
   
   with register and config functionality
   
   
 //
   
 */

// enable this for debugging to enable profiling support for each component
#define DO_PROFILING    0
// enable this to output the profiling result after each tick via SMILE_IMSG(2,...)
#define PRINT_PROFILING 0

/* good practice for dynamic component library compatibility (will be added soon..):

  define a global register method:

  sComponentInfo * registerMe(cConfigManager *_confman) {
    sMyComponent::registerComponent(_confman);
  }

  also: add a pointer to this method to global component list...
*/

#include <smileComponent.hpp>
#include <componentManager.hpp>

#define MODULE "cSmileComponent"

SMILECOMPONENT_STATICS(cSmileComponent)

// static, must be overriden by derived component class, these are only examples...!!!!

// register component will return NULL, if the component could not be registered (fatal error..)
// it will return a valid struct, with rA=1, if some information, e.g. sub-configTypes are still missing
SMILECOMPONENT_REGCOMP(cSmileComponent)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_XXXX;
  sdescription = COMPONENT_DESCRIPTION_XXXX;

  // configure your component's configType:
  SMILECOMPONENT_CREATE_CONFIGTYPE
  
  //ConfigType *ct = new ConfigType(scname); if (ct == NULL) OUT_OF_MEMORY;

  ct->setField("f1", "this is an example int", 0);
  if (ct->setField("subconf", "this is config of sub-component",
                  sconfman->getTypeObj("nameOfSubCompType"), NO_ARRAY, DONT_FREE) == -1) {
     rA=1; // if subtype not yet found, request , re-register in the next iteration
  }

  // ...
  SMILECOMPONENT_IFNOTREGAGAIN( {} )

    // change cSmileComponent to cMyComponent !
  SMILECOMPONENT_MAKEINFO(cSmileComponent);
  //return makeInfo(sconfman, scname, sdescription, cSmileComponent::create, rA);
}


SMILECOMPONENT_CREATE_ABSTRACT(cSmileComponent)

// staic, internal....
sComponentInfo * cSmileComponent::makeInfo(cConfigManager *_confman, const char *_name, const char *_description, cSmileComponent * (*create) (const char *_instname), int regAgain, int _abstract, int _nodmem)
{
  sComponentInfo *info = new sComponentInfo;
  if (info == NULL) OUT_OF_MEMORY;
  info->componentName = _name;
  info->description = _description;
  info->create = create;
  info->registerAgain = regAgain;
  info->abstract = _abstract;
  info->noDmem = _nodmem;
  info->next = NULL;
  
  return info;
}


//--------------------- dynamic:

cSmileComponent * cSmileComponent::getComponentInstance(const char * name) {
  if (compman != NULL)
    return compman->getComponentInstance(name);
  else
    return NULL;
}

const char * cSmileComponent::getComponentInstanceType(const char * name) {
  if (compman != NULL)
    return compman->getComponentInstanceType(name);
  else
    return NULL;
}

cSmileComponent * cSmileComponent::createComponent(const char*_name, const char*_type)
{
  if (compman != NULL)
    return compman->createComponent(_name,_type);
  else
    return NULL;
}

cSmileComponent::cSmileComponent(const char *instname) :
  iname(NULL),
  id(-1),
  compman(NULL),
  parent(NULL),
  cfname(NULL),
  _isRegistered(0),
  _isConfigured(0),
  _isFinalised(0),
  _isReady(0),
  confman(NULL),
  override(0),
  manualConfig(0),
  cname(NULL),
  EOI(0),
  _runMe(1),
  doProfile(DO_PROFILING),
  printProfile(PRINT_PROFILING),
  profileCur(0.0), profileSum(0.0)
{
  smileMutexCreate(messageMtx);
  if (instname == NULL) COMP_ERR("cannot create cSmileComponent with instanceName == NULL!");
  iname = strdup(instname);
  cfname = iname;
  //fetchConfig();  // must be called by the components themselves, after the component was created!
}

void cSmileComponent::setComponentEnvironment(cComponentManager *_compman, int _id, cSmileComponent *_parent)
{
  if (_compman != NULL) {
    compman = _compman;
    id = _id;
  } else {
    SMILE_ERR(3,"setting NULL componentManager in cSmileComponent::setComponentEnvironment !");
  }
  parent=_parent;
  mySetEnvironment();
}

int cSmileComponent::sendComponentMessage( const char *recepient, cComponentMessage *_msg )
{
  int ret=0;
  if (compman != NULL) {
    if (_msg != NULL) _msg->sender = getInstName();
    ret = compman->sendComponentMessage( recepient, _msg );
  }
  return ret;
}

double cSmileComponent::getSmileTime() 
{ 
  if (compman != NULL) {
    return compman->getSmileTime();
  }
  return 0.0;
}

int cSmileComponent::isAbort() 
{ 
  if (compman != NULL) return compman->isAbort();
  else return 0; 
}

int cSmileComponent::finaliseInstance() 
{
  if (!_isConfigured) {
    SMILE_DBG(7,"finaliseInstance called on a not yet successfully configured instance '%s'",getInstName());
    return 0;
  }
  if (_isFinalised) return 1;
  _isFinalised = myFinaliseInstance();
  _isReady = _isFinalised;
  return _isFinalised;
}

void cSmileComponent::startProfile(long long t, int _EOI)
{
  gettimeofday( &startTime, NULL );
}

void cSmileComponent::endProfile(long long t, int _EOI)
{
  gettimeofday( &endTime, NULL );
  profileCur = ( (double)(endTime.tv_sec - startTime.tv_sec) + (double)(endTime.tv_usec - startTime.tv_usec)/1000000.0 );
  profileSum += profileCur;
  if (printProfile) {
    SMILE_IMSG(2,"~~~~profile~~~~ cur=%f  sum=%f  tick=%i\n",getProfile(0),getProfile(1),t);
  }

}

cSmileComponent::~cSmileComponent()
{
  if ((iname != cfname)&&(cfname!=NULL)) free (cfname);
  if (iname != NULL) free(iname);
  smileMutexDestroy(messageMtx);
}

// signal EOI to componentManager (theoretically only useful for dataSource components, however we make it accessible to all smile components)
// NOTE: you do not need to do this explicitely.. if all components fail, EOI is assumed, then a new tickLoop is started by the component manager
void cSmileComponent::signalEOI()
{
  if (compman != NULL)
    compman->setEOI();
}





