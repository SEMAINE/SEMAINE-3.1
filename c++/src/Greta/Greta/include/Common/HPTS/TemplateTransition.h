#ifndef _HPTS_TemplateTransition_H
#define _HPTS_TemplateTransition_H

#include <HPTS/Transition.h>
#include <HPTS/ClassMethodPointer.h>
#include <HPTS/ClassVoidMethodPointer.h>

namespace HPTS
{
  

  /** \brief Classe de description d'une transition d'automate g�n�r� par le compilateur d'HPTS++.

  Ce type est utilise pour g�n�rer des transitions � partir de la description des automates
  via le compilateur de HPTS++. En effet, lorsqu'une classe d'automate est d�crite, un ensemble
  de m�thodes propres � l'automate correspondant aux diff�rents codes associ�s aux transitions sont g�n�r�s.
  Cette classe permet de r�cup�rer des pointeurs sur ces m�thodes pour en faire des transitions.


  \param SM Le type de l'automate auquel cette transition est associ�e
  */
  template <class SM>
  class TemplateTransition : public Transition
  {
    /** \brief Type de pointeur sur m�thode de condition 

    La m�thode manipul�e poss�de l'ent�te suivant:
    \par
    bool SM::identifier() ;
    */
    public: typedef ClassMethodPointer<SM, bool> ConditionPointer ;

    /** \brief Type de pointeur sur m�thode d'action/r�action aux signaux 

    La m�thode manipul�e poss�de l'ent�te suivant:
    \par
    void SM::identifier()
    */
    public: typedef ClassVoidMethodPointer<SM> MethodPointer ;

    /** \brief Type de pointeur sur m�thode de degr� de pr�f�rence 

    La m�thode manipul�e poss�de l'ent�te suivant:
    \par
    double SM::identifier()
    */
    public: typedef ClassMethodPointer<SM, double> FunctionPointer ;

    /** \brief Pointeur sur la m�thode correspondant au degr� de pr�f�rence */
    protected: FunctionPointer  m_preference ;

    /** \brief Pointeur sur la m�thode correspondant � la condition */
    protected: ConditionPointer m_condition ;

    /** \brief Pointeur sur la m�thode correspondant � l'action */
    protected: MethodPointer    m_action ;

    /** \brief Constructeur g�n�rant une transition pour un automate de type SM.

    \param stateMachine L'automate auquel appartient la transition.
    \param preference La m�thode de l'automate correspondant au degr� de pr�f�rence.
    \param condition La m�thode de l'automate correspondant � la condition.
    \param action La m�thode de l'automate correspondant � l'action.
    \param origin L'�tat d'origine de la transition.
    \param extremity L'�tat d'extr�mit� de la transition.
     */
    public: TemplateTransition(SM * stateMachine, 
			      typename FunctionPointer::MethodType preference,
			      typename ConditionPointer::MethodType condition, 
			      typename MethodPointer::MethodType action,
			      State * origin,
			      State * extremity) ;

    /** \brief Destructeur */
    public: virtual ~TemplateTransition() ;

    /** \brief R�cup�ration du degr� de pr�f�rence associe � la transition */
    public: double preference() ;

    /** \brief Test de la condition associ�e � la transition */
    public: bool condition() ;

    /** \brief Ex�cution de l'action associ�e � la transition */
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

