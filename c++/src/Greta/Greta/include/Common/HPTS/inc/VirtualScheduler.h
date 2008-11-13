#ifndef _VirtualScheduler_H
#define _VirtualScheduler_H

#include <Presentation.h>
#include <MetaState.h>
#include <set>
#include <map>
#include <vector>

namespace ResourceScheduler
{
  /** \brief Classe de base de l'ordonnanceur d'automates.

	Il s'agit d'une classe offrant un certain nombre de fonctionnalités 
	permettant d'ajouter des propositions de transitions des automates 
	pour un calcul d'ordonnancement.
  */
  class HPTS_DLL VirtualScheduler
  {
    /** \brief Structure utilisée pour comparer des données de type MetaState *

	Cette structure est utilisée comme comparateur pour la classe map de la 	STL.	
	*/
    Public typedef struct struct_CompareMetaState
    {
      bool operator() (MetaState * s1, MetaState * s2) const
      {
	return s1->weight()>s2->weight() ||
	  (s1->weight()==s2->weight() &&
	   (s1->state()<s2->state() ||
	    (s1->state()==s2->state() &&
	     s1->transition()<s2->transition()))) ;
      }
    } CompareMetaState ;

    /** \brief Ensemble de MetaState * triés en fonction de leur poids
	
	\sa CompareMetaState
	*/
    Public typedef std::set<MetaState*, CompareMetaState> MetaStateSet ;

    /** \brief Map associant à chaque automate son ensemble de propositions */
    Public typedef std::map<HPTS::StateMachine*, MetaStateSet > StateMachinePropositionMap ;

    /** \brief Liste des propositions des automates 
	
	\warning L'ensemble des propositions référencées dans cette liste
	est détruit (appel à delete) lors de la destruction de l'ordonnanceur. 
	*/
    Protected std::list<MetaState*>           m_metaStateList ;

    /** \brief Ensemble des propositions des automates */
    Protected StateMachinePropositionMap m_stateMachineProposition ; 

    /** \brief Le résultat de l'ordonnancement contenant la combinaison
	d'états choisie
	*/
    Protected std::vector<MetaState*>         m_bestCombination ;         

    /** \brief Le poids associé à la solution de l'ordonnancement contenue dans 
	   m_stateMachineProposition.
	*/
    Protected double                     m_bestCombinationWeight ;   // Poids de la solution de l'ordonnancement

   
    /** \brief Le constructeur */
    Public inline VirtualScheduler() ;

    /** \brief Méthode appelée pour le calcul de l'ordonnancement */
    Public virtual void schedule() = 0 ;

    /** \brief Ajout d'une proposition de transition d'un automate pour l'ordonnancement.
	\param stateMachine L'automate dont vient cette proposition.
	\param state L'état proposé par l'automate.
	\param transition La transition permettant d'aboutir à cet état.
	\param weight Le poids associé à la proposition.
	\param takenResources L'ensemble des ressources utilisées dans cet état.
	\param inheritedResources L'ensemble des ressources extraites du calcul de dépendance.
	*/
    Public  void addProposition(HPTS::StateMachine * stateMachine,
				HPTS::State        * state,
				HPTS::Transition   * transition,
				double               weight,
				Set const &          takenResources,
				Set const &          inheritedResources) ;

    /** \brief Consultation du résultat de l'ordonnancement. */
    Public inline const std::vector<MetaState*> & combination() const ;

    /** \brief Consultation du poids du résultat de l'ordonnancement. */
    Public inline double combinationWeight() const ;

    /** \brief Destructeur
	
	\warning Ce destruction détruit l'ensemble des états contenus dans la liste m_metaStateList.
	*/
    Public virtual ~VirtualScheduler() ;

    /** \brief Méthode permettant de réserver de la mémoire pour une proposition 

	\param stateMachine L'automate dont vient cette proposition.
	\param state L'état proposé par l'automate.
	\param transition La transition permettant d'aboutir à cet état.
	\param weight Le poids associé à la proposition.
	\param takenResources L'ensemble des ressources utilisées dans cet état.
	\param inheritedResources L'ensemble des ressources extraites du calcul de dépendance.		
	*/
    Protected virtual MetaState * newMetaState(HPTS::StateMachine * stateMachine,
					       HPTS::State        * state,
					       HPTS::Transition   * transition,
					       double               weight,
					       Set const &          takenResources,
					       Set const &          inheritedResources) ;

  } ;
  
  // --------------------------------
  // Implementation inlines/templates
  // --------------------------------
  VirtualScheduler::VirtualScheduler()
    : m_bestCombinationWeight(0.0)
  {}
  
  const std::vector<MetaState*> & VirtualScheduler::combination() const
  {
    return m_bestCombination ;
  }

  double VirtualScheduler::combinationWeight() const 
  {
    return m_bestCombinationWeight ;
  }
} ;

#endif

