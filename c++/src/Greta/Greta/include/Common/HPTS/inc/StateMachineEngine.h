#ifndef _StateMachineEngine_H
#define _StateMachineEngine_H

#include <StateMachine.h>

namespace HPTS
{
  //
  // Moteur d'exécution pour un automate
  // - Pas de gestion des ressources
  //
  /** \brief Moteur d'exécution d'un seul automate.
      Cette classe existe pour des besoins de test.
   */
  class HPTS_DLL StateMachineEngine : public SignalsInterface
  {
    // -----
    // Types
    // -----
    Public typedef enum {running, suspended, ended} StateMachineStatus ;

    // ---------------
    // Donnees membres
    // ---------------

    Protected StateMachineStatus m_status ;
   
    Protected bool m_deleteStateMachine ;

    Protected StateMachine * m_stateMachine ;
    Protected State        * m_currentState ;

    // --------
    // Methodes
    // --------

    // Construction du moteur d'execution d'un automate
    // - appel de la methode init
    // - recuperation de l'etat de depart
    // - execution du premier etat (entry, during)
    Public StateMachineEngine(StateMachine * stateMachine, bool deleteStateMachine=true) ;

    // Destructeur
    Public virtual ~StateMachineEngine() ;

    // Calcul d'un pas de simulation
    Public virtual void compute() ;

    // Recuperation de l'etat de l'automate
    Public StateMachineStatus status() const 
    { return m_status; }

    // Les signaux
    virtual void kill() ;
    virtual void suspend() ;
    virtual void resume() ;
  } ;
  
} ;

#endif

