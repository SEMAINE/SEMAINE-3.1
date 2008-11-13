#ifndef _HPTS_StateMachine
#define _HPTS_StateMachine

#include <Presentation.h>
#include <State.h>
#include <Transition.h>
#include <SignalsInterface.h>
#include <list>

namespace HPTS
{

  /** \brief La classe de base des automates gérés par HPTS++.

  Cette classe offre les fonctionnalités utiles a la gestion des automates 
  par les contrôleurs de HPTS++.
  \sa HPTS::StateMachineControlleur
  \sa HPTS::StateMachineControlleurNoResource
   */
  class HPTS_DLL StateMachine : public SignalsInterface
  {
    /** \brief Liste des états déclarés dans l'automate par allocation dynamique (utilisation de new) 
	
    A la destruction de l'automate, ces états seront automatiquement détruits.
    */
    Private std::list<State*>      m_stateList ;

    /** \brief Liste des transition déclarées dans l'automate par allocation dynamique (utilisation de new)  

    A la destruction de l'automate, ces transitions seront automatiquement détruites.
    */
    Private std::list<Transition*> m_transitionList ;

    /** \brief Référence un état à détruire lors de la destruction d'un automate.
	
    Ces états sont ajoutés dans la liste m_stateList.
     */
    Protected inline void referenceStateDelete(State * state) ;

    /** \brief Référence une transition à détruire lors de la destruction d'un automate.

    Ces états sont ajoutes dans la liste m_transitionList.
    */
    Protected inline void referenceTransitionDelete(Transition * transition) ;

    /** \brief Déclaration et initialisation des états de l'automate.  */
    Public virtual void initStates() = 0 ;

    /** \brief Déclaration et initialisation des transitions de l'automate. */
    Public virtual void initTransitions() = 0 ;

    /** \brief Référence les transitions dans leur état d'origine. */
    Public virtual void referenceTransitions() = 0 ;

    /** \brief Code d'automate à exécuter avant un pas de temps. 
	
    \sa HPTS::StateMachineController::compute()
    */
    Public virtual void beforeStep() ;

    /** \brief Code d'automate à exécuter après un pas de temps. 

    \sa HPTS::StateMachineController::compute()
    */
    Public virtual void afterStep() ;

    // Priorite de l'automate (1.0 par defaut)
    /** \brief Priorité de l'automate

    Cette fonction renvoie 1.0 comme valeur par défaut de la priorité.
    */
    Public virtual double priority() ;

    /** \brief Etat de départ de l'automate */
    Public virtual State * entry() = 0 ;

    /** \brief Destructeur

    Ce destructeur se charge de détruire les états référencés dans m_stateList
    ainsi que les transitions référencées dans m_transitionList.
    */
    Public virtual ~StateMachine() ;

    /** \brief Initialisation de l'automate

    Cette méthode initialise de manière cohérente l'automate:
    \li Initialisation des états.
    \li Initialisation des transitions.
    \li Référence des transitions dans leur etat d'origine.
    \warning Cette méthode doit être appelée avant toute manipulation des informations
    contenues dans l'automate. La méthode HPTS::StateMachineController::start l'appelle 
    automatiquement. Pour toute autre utilisation des automates, cet appel est a la charge du programmeur.
     */
    Public void initStateMachine() ;

    virtual void kill() ;
    virtual void suspend() ;
    virtual void resume() ;
  } ;

  /** \brief Référence un état comme étant à détruire lors de la destruction de l'automate. */
  void StateMachine::referenceStateDelete(State * state) 
  {
    m_stateList.push_back(state) ;
  }

  /** \brief Référence une transition comme étant à détruire lors de la destruction de l'automate */
  void StateMachine::referenceTransitionDelete(Transition * transition)
  {
    m_transitionList.push_back(transition) ;
  }

} ;

#endif

