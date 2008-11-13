#ifndef _HierarchicalStateMachine_H
#define _HierarchicalStateMachine_H
#include <StateMachine.h>

#line 3 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net/Runtime/StateMachine/HierarchicalStateMachine.sm"
#include <ControlledStateMachine.h>

#line 4 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net/Runtime/StateMachine/HierarchicalStateMachine.sm"
#include <set>

class HPTS_DLL HierarchicalStateMachine:public ControlledStateMachine{
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

#line 11 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net/Runtime/StateMachine/HierarchicalStateMachine.sm"
Public typedef std::set<Controller::Identifier> IdentifierSet ;

#line 13 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net/Runtime/StateMachine/HierarchicalStateMachine.sm"
Protected IdentifierSet m_subStateMachines ;

#line 15 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net/Runtime/StateMachine/HierarchicalStateMachine.sm"
HierarchicalStateMachine(Controller * controller)

#line 16 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net/Runtime/StateMachine/HierarchicalStateMachine.sm"
: ControlledStateMachine(controller)

#line 17 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net/Runtime/StateMachine/HierarchicalStateMachine.sm"
{ }

#line 19 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net/Runtime/StateMachine/HierarchicalStateMachine.sm"
Controller::Identifier start(ControlledStateMachine * stateMachine) ;

#line 21 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net/Runtime/StateMachine/HierarchicalStateMachine.sm"
void removeSon(Controller::Identifier id) ;

#line 23 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net/Runtime/StateMachine/HierarchicalStateMachine.sm"
virtual ~HierarchicalStateMachine() ;

} ;
#endif
