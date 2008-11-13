#ifndef _ControlledStateMachine_H
#define _ControlledStateMachine_H
#include <HPTS/StateMachine.h>

#line 3 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net.v0.5.net2003.OK/Runtime/StateMachine/ControlledStateMachine.sm"
#include <HPTS/StateMachineController.h>

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

#line 11 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net.v0.5.net2003.OK/Runtime/StateMachine/ControlledStateMachine.sm"
public: typedef HPTS::StateMachineController Controller ;

#line 14 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net.v0.5.net2003.OK/Runtime/StateMachine/ControlledStateMachine.sm"
private: Controller * m_controller ;

#line 17 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net.v0.5.net2003.OK/Runtime/StateMachine/ControlledStateMachine.sm"
public: ControlledStateMachine(Controller * controller)

#line 18 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net.v0.5.net2003.OK/Runtime/StateMachine/ControlledStateMachine.sm"
: m_controller(controller)

#line 19 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net.v0.5.net2003.OK/Runtime/StateMachine/ControlledStateMachine.sm"
{ }

#line 22 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net.v0.5.net2003.OK/Runtime/StateMachine/ControlledStateMachine.sm"
public: ControlledStateMachine()

#line 23 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net.v0.5.net2003.OK/Runtime/StateMachine/ControlledStateMachine.sm"
: m_controller(NULL)

#line 24 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net.v0.5.net2003.OK/Runtime/StateMachine/ControlledStateMachine.sm"
{ }

#line 27 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net.v0.5.net2003.OK/Runtime/StateMachine/ControlledStateMachine.sm"
public: HPTS::StateMachineController & controller() const

#line 28 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net.v0.5.net2003.OK/Runtime/StateMachine/ControlledStateMachine.sm"
{ return *m_controller ; }

#line 31 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net.v0.5.net2003.OK/Runtime/StateMachine/ControlledStateMachine.sm"
public: void SetController(Controller * controller)

#line 32 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net.v0.5.net2003.OK/Runtime/StateMachine/ControlledStateMachine.sm"
{ m_controller = controller; }

} ;
#endif
