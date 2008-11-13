#ifndef _HPTS_StateMachine
#define _HPTS_StateMachine

#include <Presentation.h>
#include <State.h>
#include <Transition.h>
#include <SignalsInterface.h>
#include <list>

namespace HPTS
{

  /** \brief La classe de base des automates g�r�s par HPTS++.

  Cette classe offre les fonctionnalit�s utiles a la gestion des automates 
  par les contr�leurs de HPTS++.
  \sa HPTS::StateMachineControlleur
  \sa HPTS::StateMachineControlleurNoResource
   */
  class HPTS_DLL StateMachine : public SignalsInterface
  {
    /** \brief Liste des �tats d�clar�s dans l'automate par allocation dynamique (utilisation de new) 
	
    A la destruction de l'automate, ces �tats seront automatiquement d�truits.
    */
    Private std::list<State*>      m_stateList ;

    /** \brief Liste des transition d�clar�es dans l'automate par allocation dynamique (utilisation de new)  

    A la destruction de l'automate, ces transitions seront automatiquement d�truites.
    */
    Private std::list<Transition*> m_transitionList ;

    /** \brief R�f�rence un �tat � d�truire lors de la destruction d'un automate.
	
    Ces �tats sont ajout�s dans la liste m_stateList.
     */
    Protected inline void referenceStateDelete(State * state) ;

    /** \brief R�f�rence une transition � d�truire lors de la destruction d'un automate.

    Ces �tats sont ajoutes dans la liste m_transitionList.
    */
    Protected inline void referenceTransitionDelete(Transition * transition) ;

    /** \brief D�claration et initialisation des �tats de l'automate.  */
    Public virtual void initStates() = 0 ;

    /** \brief D�claration et initialisation des transitions de l'automate. */
    Public virtual void initTransitions() = 0 ;

    /** \brief R�f�rence les transitions dans leur �tat d'origine. */
    Public virtual void referenceTransitions() = 0 ;

    /** \brief Code d'automate � ex�cuter avant un pas de temps. 
	
    \sa HPTS::StateMachineController::compute()
    */
    Public virtual void beforeStep() ;

    /** \brief Code d'automate � ex�cuter apr�s un pas de temps. 

    \sa HPTS::StateMachineController::compute()
    */
    Public virtual void afterStep() ;

    // Priorite de l'automate (1.0 par defaut)
    /** \brief Priorit� de l'automate

    Cette fonction renvoie 1.0 comme valeur par d�faut de la priorit�.
    */
    Public virtual double priority() ;

    /** \brief Etat de d�part de l'automate */
    Public virtual State * entry() = 0 ;

    /** \brief Destructeur

    Ce destructeur se charge de d�truire les �tats r�f�renc�s dans m_stateList
    ainsi que les transitions r�f�renc�es dans m_transitionList.
    */
    Public virtual ~StateMachine() ;

    /** \brief Initialisation de l'automate

    Cette m�thode initialise de mani�re coh�rente l'automate:
    \li Initialisation des �tats.
    \li Initialisation des transitions.
    \li R�f�rence des transitions dans leur etat d'origine.
    \warning Cette m�thode doit �tre appel�e avant toute manipulation des informations
    contenues dans l'automate. La m�thode HPTS::StateMachineController::start l'appelle 
    automatiquement. Pour toute autre utilisation des automates, cet appel est a la charge du programmeur.
     */
    Public void initStateMachine() ;

    virtual void kill() ;
    virtual void suspend() ;
    virtual void resume() ;
  } ;

  /** \brief R�f�rence un �tat comme �tant � d�truire lors de la destruction de l'automate. */
  void StateMachine::referenceStateDelete(State * state) 
  {
    m_stateList.push_back(state) ;
  }

  /** \brief R�f�rence une transition comme �tant � d�truire lors de la destruction de l'automate */
  void StateMachine::referenceTransitionDelete(Transition * transition)
  {
    m_transitionList.push_back(transition) ;
  }

} ;

#endif

