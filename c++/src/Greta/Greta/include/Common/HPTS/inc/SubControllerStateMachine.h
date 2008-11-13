#ifndef _SubControllerStateMachine_H
#define _SubControllerStateMachine_H
#include <StateMachine.h>

#line 3 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net/Runtime/StateMachine/SubControllerStateMachine.sm"
#include <ControlledStateMachine.h>

class HPTS_DLL SubControllerStateMachine:public ControlledStateMachine{
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

#line 11 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net/Runtime/StateMachine/SubControllerStateMachine.sm"
Private Controller m_subController ;

#line 13 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net/Runtime/StateMachine/SubControllerStateMachine.sm"
Public SubControllerStateMachine(Controller * controller)

#line 14 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net/Runtime/StateMachine/SubControllerStateMachine.sm"
: ControlledStateMachine(controller)

#line 15 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net/Runtime/StateMachine/SubControllerStateMachine.sm"
{ }

} ;
#endif
