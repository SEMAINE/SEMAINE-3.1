#ifndef _MetaState_H
#define _MetaState_H

#include <HPTS/Set.h>
#include <HPTS/StateMachine.h>

namespace ResourceScheduler
{
  class HPTS_DLL MetaState
  {
    // ---------------
    // Donnees membres
    // ---------------

    // Informations sur l'etat ainsi que la transition a prendre
    protected: HPTS::StateMachine * m_stateMachine ;
    protected: HPTS::State        * m_state ;
    protected: HPTS::Transition   * m_transition ;
    // Poids de la proposition
    protected: double               m_weight ;
    // Ressources prises et heritees
    protected: Set m_takenResources ;
    protected: Set m_inheritedResources ;

    // --------
    // Methodes
    // --------

    // Constructeur
    public: inline MetaState(HPTS::StateMachine * stateMachine,
			    HPTS::State        * state,
			    HPTS::Transition   * transition,
			    double               weight,
			    Set const &          takenResources,
			    Set const &          inheritedResources
			    ) ;
    
    // Destructeur
    public: virtual ~MetaState() ;

    // Recuperation des informations contenues dans la classe
    public: inline HPTS::StateMachine * stateMachine() const ;
    public: inline HPTS::State        * state() const ;
    public: inline HPTS::Transition   * transition() const ;
    public: inline double               weight() const ;
    public: inline const Set &          takenResources() const ;
    public: inline const Set &          inheritedResources() const ;

    // Modification du poids
    public: inline void setWeight(double weight) ;
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

