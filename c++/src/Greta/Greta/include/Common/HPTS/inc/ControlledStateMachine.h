#ifndef _ControlledStateMachine_H
#define _ControlledStateMachine_H
#include <StateMachine.h>

#line 3 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net/Runtime/StateMachine/ControlledStateMachine.sm"
#include <StateMachineController.h>

class HPTS_DLL ControlledStateMachine:public HPTS::StateMachine{
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

#line 11 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net/Runtime/StateMachine/ControlledStateMachine.sm"
Public typedef HPTS::StateMachineController Controller ;

#line 14 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net/Runtime/StateMachine/ControlledStateMachine.sm"
Private Controller * m_controller ;

#line 17 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net/Runtime/StateMachine/ControlledStateMachine.sm"
Public ControlledStateMachine(Controller * controller)

#line 18 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net/Runtime/StateMachine/ControlledStateMachine.sm"
: m_controller(controller)

#line 19 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net/Runtime/StateMachine/ControlledStateMachine.sm"
{ }

#line 22 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net/Runtime/StateMachine/ControlledStateMachine.sm"
Public HPTS::StateMachineController & controller() const

#line 23 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net/Runtime/StateMachine/ControlledStateMachine.sm"
{ return *m_controller ; }

} ;
#endif
