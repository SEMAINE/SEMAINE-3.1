#ifndef _HPTS_StateMachineStructure_H
#define _HPTS_StateMachineStructure_H

//#include <Presentation.h>
#include <HPTS/hpts_dll.h>
#include <list>
#include <set>
#include <map>
#include <HPTS/StateMachine.h>
#include <assert.h>

namespace HPTS
{

  /** \brief Classe de manipulation de la structure d'un automate
      
   Classe permettant de faire des calculs sur la structure de l'automate,
   notamment de calculer la fermeture du graphe des �tats utilisant des ressources
   pour permettre le calcul des ressources h�rit�es.

   \author Fabrice LAMARCHE
  */
  class HPTS_DLL StateMachineStructure
  {
    /** \brief Type liste de transitions */
    public: typedef std::list<HPTS::Transition*>     TransitionList ;
    
    /** \brief Ensemble d'�tats d'automates */
    public: typedef std::set<HPTS::State*>           StateSet ;

    /** \brief Type de graphe

    Cette structure de donn�es repr�sente le graphe associe a un automate.
    Elle associe un �tat � l'ensemble de ses �tats successeurs.
     */
    public: typedef std::map<HPTS::State*, StateSet> Graph ;

    /** \brief L'automate repr�sent� */
    protected: HPTS::StateMachine * m_stateMachine ;            // Automate

    /** \brief L'ensemble des �tats de l'automate */
    protected: StateSet             m_smStates ;                // Ensemble des etats de l'automate

    /** \brief L'ensemble des �tats de l'automate utilisant des ressources */
    protected: StateSet             m_smResourceStates ;        // Ensemble des etats utilisant des ressources

    /** \brief La fermeture transitive du graphe des �tats utilisant des ressources */
    protected: Graph                m_smSuccessorClosureGraph ; // Fermeture du graphe des etats utilisant des ressources

    /** \brief Constructeur 
	
    Ce constructeur initialise l'ensemble des donn�es membres de cette classe.
    A partir de l'automate, il calcule l'ensemble des �tats accessibles, filtre les �tats
    utilisant des ressources et construit la fermeture transitive du graphe associe
    aux �tats utilisant des ressources. Le calcul de cette fermeture est utile pour 
    le calcul des d�pendances de ressources.
    \param stateMachine L'automate de travail.
    */
    public: StateMachineStructure(HPTS::StateMachine * stateMachine) ;

    /** \brief R�cup�ration de l'ensemble des �tats atteignables des automates */
    public: inline const StateSet & states() const ;

    /** \brief R�cup�ration des successeurs d'un �tat */
    public: inline const StateSet & successors(HPTS::State * state) const ;
  
    /** \brief Extraction des �tats atteignables d'un automate */
    protected: void extractStates() ;

    /** \brief Calcul des �tats utilisant des ressources */
    protected: void filterResourceStates() ;

    /** \brief Calcul du graphe associe � un ensemble d'�tats 
	
    Ce graphe est calcule � partir des �tats passes en param�tre. Si un successeur
    d'un �tat n'est pas dans l'ensemble des �tats passes en param�tre, la transition
    n'est pas prise en compte et n'est donc pas repr�sent�e dans le graphe r�sultat
    \param states L'ensemble des �tats de travail
    \param result Le graphe r�sultant du calcul
    */
    protected: void successorGraph(StateSet const & states, Graph & result) ;

    /** \brief Calcul des successeurs de degr� 2 d'un �tat dans un graphe
	
    Cette m�thode calcule, � partir d'un graphe, les successeurs de l'�tat pass� en param�tre.
    Les successeurs calcul�s sont ceux atteignables par le passage de deux transitions.
    \param graph Le graphe sur lequel le calcul s'effectue.
    \param state L'�tat dont on veut calculer les successeurs de degr� 2.
    \param result L'ensemble des �tats accessibles par le passage de deux transitions.
     */
    protected: bool successors2(const Graph & graph, HPTS::State * state, StateSet & result) ;

    /** \brief Calcul de la fermeture transitive du graphe.

    \param graph Le graphe d'origine, en entr�e, qui est modifi� pour repr�senter sa fermeture transitive.
    */
    protected: void closure(Graph & graph) ;
  } ;

  const StateMachineStructure::StateSet & StateMachineStructure::states() const 
  {
    return m_smResourceStates ;
  }

  const StateMachineStructure::StateSet & StateMachineStructure::successors(HPTS::State * state) const 
  {
    assert(m_smSuccessorClosureGraph.find(state)!=m_smSuccessorClosureGraph.end()) ;
    return (*m_smSuccessorClosureGraph.find(state)).second ;
  }

}

#endif

