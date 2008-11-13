#ifndef _HPTS_TemplateTransition_H
#define _HPTS_TemplateTransition_H

#include <HPTS/Transition.h>
#include <HPTS/ClassMethodPointer.h>
#include <HPTS/ClassVoidMethodPointer.h>

namespace HPTS
{
  

  /** \brief Classe de description d'une transition d'automate généré par le compilateur d'HPTS++.

  Ce type est utilise pour générer des transitions à partir de la description des automates
  via le compilateur de HPTS++. En effet, lorsqu'une classe d'automate est décrite, un ensemble
  de méthodes propres à l'automate correspondant aux différents codes associés aux transitions sont générés.
  Cette classe permet de récupérer des pointeurs sur ces méthodes pour en faire des transitions.


  \param SM Le type de l'automate auquel cette transition est associée
  */
  template <class SM>
  class TemplateTransition : public Transition
  {
    /** \brief Type de pointeur sur méthode de condition 

    La méthode manipulée possède l'entête suivant:
    \par
    bool SM::identifier() ;
    */
    public: typedef ClassMethodPointer<SM, bool> ConditionPointer ;

    /** \brief Type de pointeur sur méthode d'action/réaction aux signaux 

    La méthode manipulée possède l'entête suivant:
    \par
    void SM::identifier()
    */
    public: typedef ClassVoidMethodPointer<SM> MethodPointer ;

    /** \brief Type de pointeur sur méthode de degré de préférence 

    La méthode manipulée possède l'entête suivant:
    \par
    double SM::identifier()
    */
    public: typedef ClassMethodPointer<SM, double> FunctionPointer ;

    /** \brief Pointeur sur la méthode correspondant au degré de préférence */
    protected: FunctionPointer  m_preference ;

    /** \brief Pointeur sur la méthode correspondant à la condition */
    protected: ConditionPointer m_condition ;

    /** \brief Pointeur sur la méthode correspondant à l'action */
    protected: MethodPointer    m_action ;

    /** \brief Constructeur générant une transition pour un automate de type SM.

    \param stateMachine L'automate auquel appartient la transition.
    \param preference La méthode de l'automate correspondant au degré de préférence.
    \param condition La méthode de l'automate correspondant à la condition.
    \param action La méthode de l'automate correspondant à l'action.
    \param origin L'état d'origine de la transition.
    \param extremity L'état d'extrémité de la transition.
     */
    public: TemplateTransition(SM * stateMachine, 
			      typename FunctionPointer::MethodType preference,
			      typename ConditionPointer::MethodType condition, 
			      typename MethodPointer::MethodType action,
			      State * origin,
			      State * extremity) ;

    /** \brief Destructeur */
    public: virtual ~TemplateTransition() ;

    /** \brief Récupération du degré de préférence associe à la transition */
    public: double preference() ;

    /** \brief Test de la condition associée à la transition */
    public: bool condition() ;

    /** \brief Exécution de l'action associée à la transition */
    public: void action() ;
  } ;

  template <class SM>
  TemplateTransition<SM>::TemplateTransition(SM * stateMachine, 
					     typename FunctionPointer::MethodType preference,
					     typename ConditionPointer::MethodType condition, 
					     typename MethodPointer::MethodType action,
					     State * origin,
					     State * extremity)
    : Transition(origin, extremity), 
    m_preference(stateMachine, preference),
    m_condition(stateMachine, condition), 
    m_action(stateMachine, action)
  {}
  
  template <class SM>
  TemplateTransition<SM>::~TemplateTransition()
  {}

  template <class SM>
  double TemplateTransition<SM>::preference() { return m_preference() ; }

  template <class SM>
  bool TemplateTransition<SM>::condition() { return m_condition() ; }

  template <class SM>
  void TemplateTransition<SM>::action() { m_action() ; }
} ;

#endif

