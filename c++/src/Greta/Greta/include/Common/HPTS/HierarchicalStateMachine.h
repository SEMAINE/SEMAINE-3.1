#ifndef _HierarchicalStateMachine_H
#define _HierarchicalStateMachine_H
#include <HPTS/StateMachine.h>

#line 3 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net.v0.5.net2003.OK/Runtime/StateMachine/HierarchicalStateMachine.sm"
#include <HPTS/ControlledStateMachine.h>

#line 4 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net.v0.5.net2003.OK/Runtime/StateMachine/HierarchicalStateMachine.sm"
#include <set>

class HPTS_DLL HierarchicalStateMachine:public  ControlledStateMachine{
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

#line 12 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net.v0.5.net2003.OK/Runtime/StateMachine/HierarchicalStateMachine.sm"
public: typedef std::set<Controller::Identifier> IdentifierSet ;

#line 14 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net.v0.5.net2003.OK/Runtime/StateMachine/HierarchicalStateMachine.sm"
protected: IdentifierSet m_subStateMachines ;

#line 16 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net.v0.5.net2003.OK/Runtime/StateMachine/HierarchicalStateMachine.sm"
public: HierarchicalStateMachine(Controller * controller)

#line 17 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net.v0.5.net2003.OK/Runtime/StateMachine/HierarchicalStateMachine.sm"
: ControlledStateMachine(controller)

#line 18 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net.v0.5.net2003.OK/Runtime/StateMachine/HierarchicalStateMachine.sm"
{ }

#line 20 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net.v0.5.net2003.OK/Runtime/StateMachine/HierarchicalStateMachine.sm"
public: HierarchicalStateMachine()

#line 21 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net.v0.5.net2003.OK/Runtime/StateMachine/HierarchicalStateMachine.sm"
{ }

#line 24 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net.v0.5.net2003.OK/Runtime/StateMachine/HierarchicalStateMachine.sm"
Controller::Identifier start(ControlledStateMachine * stateMachine) ;

#line 26 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net.v0.5.net2003.OK/Runtime/StateMachine/HierarchicalStateMachine.sm"
void removeSon(Controller::Identifier id) ;

#line 28 "/udd/flamarch/arch_dd/Logiciels_distrib/HPTS++.net.v0.5.net2003.OK/Runtime/StateMachine/HierarchicalStateMachine.sm"
virtual ~HierarchicalStateMachine() ;

} ;
#endif
