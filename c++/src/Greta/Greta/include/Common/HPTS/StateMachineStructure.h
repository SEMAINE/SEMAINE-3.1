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
   notamment de calculer la fermeture du graphe des états utilisant des ressources
   pour permettre le calcul des ressources héritées.

   \author Fabrice LAMARCHE
  */
  class HPTS_DLL StateMachineStructure
  {
    /** \brief Type liste de transitions */
    public: typedef std::list<HPTS::Transition*>     TransitionList ;
    
    /** \brief Ensemble d'états d'automates */
    public: typedef std::set<HPTS::State*>           StateSet ;

    /** \brief Type de graphe

    Cette structure de données représente le graphe associe a un automate.
    Elle associe un état à l'ensemble de ses états successeurs.
     */
    public: typedef std::map<HPTS::State*, StateSet> Graph ;

    /** \brief L'automate représenté */
    protected: HPTS::StateMachine * m_stateMachine ;            // Automate

    /** \brief L'ensemble des états de l'automate */
    protected: StateSet             m_smStates ;                // Ensemble des etats de l'automate

    /** \brief L'ensemble des états de l'automate utilisant des ressources */
    protected: StateSet             m_smResourceStates ;        // Ensemble des etats utilisant des ressources

    /** \brief La fermeture transitive du graphe des états utilisant des ressources */
    protected: Graph                m_smSuccessorClosureGraph ; // Fermeture du graphe des etats utilisant des ressources

    /** \brief Constructeur 
	
    Ce constructeur initialise l'ensemble des données membres de cette classe.
    A partir de l'automate, il calcule l'ensemble des états accessibles, filtre les états
    utilisant des ressources et construit la fermeture transitive du graphe associe
    aux états utilisant des ressources. Le calcul de cette fermeture est utile pour 
    le calcul des dépendances de ressources.
    \param stateMachine L'automate de travail.
    */
    public: StateMachineStructure(HPTS::StateMachine * stateMachine) ;

    /** \brief Récupération de l'ensemble des états atteignables des automates */
    public: inline const StateSet & states() const ;

    /** \brief Récupération des successeurs d'un état */
    public: inline const StateSet & successors(HPTS::State * state) const ;
  
    /** \brief Extraction des états atteignables d'un automate */
    protected: void extractStates() ;

    /** \brief Calcul des états utilisant des ressources */
    protected: void filterResourceStates() ;

    /** \brief Calcul du graphe associe à un ensemble d'états 
	
    Ce graphe est calcule à partir des états passes en paramètre. Si un successeur
    d'un état n'est pas dans l'ensemble des états passes en paramètre, la transition
    n'est pas prise en compte et n'est donc pas représentée dans le graphe résultat
    \param states L'ensemble des états de travail
    \param result Le graphe résultant du calcul
    */
    protected: void successorGraph(StateSet const & states, Graph & result) ;

    /** \brief Calcul des successeurs de degré 2 d'un état dans un graphe
	
    Cette méthode calcule, à partir d'un graphe, les successeurs de l'état passé en paramètre.
    Les successeurs calculés sont ceux atteignables par le passage de deux transitions.
    \param graph Le graphe sur lequel le calcul s'effectue.
    \param state L'état dont on veut calculer les successeurs de degré 2.
    \param result L'ensemble des états accessibles par le passage de deux transitions.
     */
    protected: bool successors2(const Graph & graph, HPTS::State * state, StateSet & result) ;

    /** \brief Calcul de la fermeture transitive du graphe.

    \param graph Le graphe d'origine, en entrée, qui est modifié pour représenter sa fermeture transitive.
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

