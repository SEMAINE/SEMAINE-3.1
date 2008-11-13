#ifndef _SubControllerStateMachine_H
#define _SubControllerStateMachine_H
#include <HPTS/StateMachine.h>

#line 3 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net.v0.5.net2003.OK/Runtime/StateMachine/SubControllerStateMachine.sm"
#include <HPTS/ControlledStateMachine.h>

class HPTS_DLL SubControllerStateMachine:public  ControlledStateMachine{
public: virtual 
#line 1 "Code.h"
void
 beforeStep(
#line 1 "Code.h"

);
public: virtual 
#line 1 "Code.h"
void
 kill(
#line 1 "Code.h"

);
public: virtual 
#line 1 "Code.h"
void
 suspend(
#line 1 "Code.h"

);
public: virtual 
#line 1 "Code.h"
void
 resume(
#line 1 "Code.h"

);
public: virtual 
#line 1 "Code.h"
HPTS::State *
 entry(
#line 1 "Code.h"

);
public: virtual 
#line 1 "Code.h"
void
 initStates(
#line 1 "Code.h"

);
public: virtual 
#line 1 "Code.h"
void
 initTransitions(
#line 1 "Code.h"

);
public: virtual 
#line 1 "Code.h"
void
 referenceTransitions(
#line 1 "Code.h"

);

#line 11 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net.v0.5.net2003.OK/Runtime/StateMachine/SubControllerStateMachine.sm"
private: Controller m_subController ;

#line 13 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net.v0.5.net2003.OK/Runtime/StateMachine/SubControllerStateMachine.sm"
public: SubControllerStateMachine(Controller * controller)

#line 14 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net.v0.5.net2003.OK/Runtime/StateMachine/SubControllerStateMachine.sm"
: ControlledStateMachine(controller)

#line 15 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net.v0.5.net2003.OK/Runtime/StateMachine/SubControllerStateMachine.sm"
{ }

#line 17 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net.v0.5.net2003.OK/Runtime/StateMachine/SubControllerStateMachine.sm"
public: SubControllerStateMachine()

#line 18 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net.v0.5.net2003.OK/Runtime/StateMachine/SubControllerStateMachine.sm"
{ }

#line 20 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net.v0.5.net2003.OK/Runtime/StateMachine/SubControllerStateMachine.sm"
public: Controller & subController()

#line 21 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net.v0.5.net2003.OK/Runtime/StateMachine/SubControllerStateMachine.sm"
{ return m_subController ; }

} ;
#endif
