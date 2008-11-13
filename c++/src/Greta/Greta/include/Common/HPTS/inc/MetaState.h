#ifndef _MetaState_H
#define _MetaState_H

#include <Set.h>
#include <StateMachine.h>

namespace ResourceScheduler
{
  class HPTS_DLL MetaState
  {
    // ---------------
    // Donnees membres
    // ---------------

    // Informations sur l'etat ainsi que la transition a prendre
    Protected HPTS::StateMachine * m_stateMachine ;
    Protected HPTS::State        * m_state ;
    Protected HPTS::Transition   * m_transition ;
    // Poids de la proposition
    Protected double               m_weight ;
    // Ressources prises et heritees
    Protected Set m_takenResources ;
    Protected Set m_inheritedResources ;

    // --------
    // Methodes
    // --------

    // Constructeur
    Public inline MetaState(HPTS::StateMachine * stateMachine,
			    HPTS::State        * state,
			    HPTS::Transition   * transition,
			    double               weight,
			    Set const &          takenResources,
			    Set const &          inheritedResources
			    ) ;
    
    // Destructeur
    Public virtual ~MetaState() ;

    // Recuperation des informations contenues dans la classe
    Public inline HPTS::StateMachine * stateMachine() const ;
    Public inline HPTS::State        * state() const ;
    Public inline HPTS::Transition   * transition() const ;
    Public inline double               weight() const ;
    Public inline const Set &          takenResources() const ;
    Public inline const Set &          inheritedResources() const ;

    // Modification du poids
    Public inline void setWeight(double weight) ;
  };

  // -----------------------------------
  // Implementation des inlines/template
  // -----------------------------------

  MetaState::MetaState(HPTS::StateMachine * stateMachine,
		       HPTS::State        * state,
		       HPTS::Transition   * transition,
		       double               weight,
		       Set const &          takenResources,
			 Set const &          inheritedResources
		       )
    : m_stateMachine(stateMachine), m_state(state), m_transition(transition), 
    m_weight(weight), 
    m_takenResources(takenResources), m_inheritedResources(inheritedResources)
  {}

  HPTS::StateMachine * MetaState::stateMachine() const 
  { return m_stateMachine ; }

  HPTS::State        * MetaState::state() const 
  { return m_state ; }

  HPTS::Transition   * MetaState::transition() const 
  { return m_transition ; }

  double               MetaState::weight() const 
  { return m_weight ; }

  const Set &          MetaState::takenResources() const 
  { return m_takenResources ; }
  
  const Set &          MetaState::inheritedResources() const 
  { return m_inheritedResources ; }

  void MetaState::setWeight(double weight)
  { m_weight = weight ; }
}

#endif

