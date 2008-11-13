#ifndef _HPTS_TemplateState_H
#define _HPTS_TemplateState_H

#include <HPTS/State.h>
#include <HPTS/ClassMethodPointer.h>
#include <HPTS/ClassVoidMethodPointer.h>

namespace HPTS
{

  /** \brief Classe de description d'un état d'automate génère par le compilateur d'HPTS++.

  Ce type est utilise pour générer des états à partir de la description des automates
  via le compilateur de HPTS++. En effet, lorsqu'une classe d'automate est décrite, un ensemble
  de méthodes propres a l'automate correspondant aux différents codes associés aux états est généré.
  Cette classe permet de récupérer des pointeurs sur ces méthodes pour en faire des états.


  \param SM Le type de l'automate auquel cet état est associe
  */
  template <class SM>
  class TemplateState : public State
  {
    /** \brief Type de pointeur sur méthode 

    Ce type décrit un pointeur sur une méthode de la classe SM ne prenant pas
    de paramètres et renvoyant un type void.
    */
    public: typedef ClassVoidMethodPointer<SM> MethodPointer ;

    /** \brief Pointeur sur la méthode de SM correspondant à la réaction au signal kill */
    protected: MethodPointer m_kill ;

    /** \brief Pointeur sur la méthode de SM correspondant à la réaction au signal suspend */
    protected: MethodPointer m_suspend ;

    /** \brief Pointeur sur la méthode de SM correspondant à la réaction au signal resume */
    protected: MethodPointer m_resume ;

    /** \brief Pointeur sur la méthode de SM correspondant à l'entrée dans l'état */
    protected: MethodPointer m_entry ;
    
    /** \brief Pointeur sur la méthode de SM correspondant au code associé à l'état */
    protected: MethodPointer m_during ;
 
    /** \brief Pointeur sur la méthode de SM correspondant à la sortie de l'état */
    protected: MethodPointer m_exit ;


    /** \brief Constructeur générant un état associé à la classe d'automate SM

    \param stateMachine L'instance d'automate auquel appartient cet état. 
    \param entry Pointeur sur la méthode de SM correspondant à l'entrée dans l'état.
    \param during Pointeur sur la méthode de SM correspondant au code associe à l'état.
    \param exit Pointeur sur la méthode de SM correspondant au code de sortie de l'état
    \param kill Pointeur sur la méthode de SM correspondant à la réaction de cet etat au signal kill
    \param suspend Pointeur sur la méthode de SM correspondant à la réaction de cet état au signal suspend
    \param resume Pointeur sur la méthode de SM correspondant à la réaction de cet état au signal resume
     */
    public: TemplateState(SM * stateMachine, 
			 typename MethodPointer::MethodType entry, 
			 typename MethodPointer::MethodType during, 
			 typename MethodPointer::MethodType exit,
			 typename MethodPointer::MethodType kill,
			 typename MethodPointer::MethodType suspend,
			 typename MethodPointer::MethodType resume) ;

    // Destructeur
    public: virtual ~TemplateState() ;

    // Implementation des differentes methodes associees a un etat
    public: virtual void entry() ;
    public: virtual void during() ;
    public: virtual void exit() ;

    // Gestion des signaux
    public: virtual void kill() ;
    public: virtual void suspend() ;
    public: virtual void resume() ;
  } ;

  // --------------
  // Implementation
  // --------------
  template <class SM>
  TemplateState<SM>::TemplateState(SM * stateMachine, 
				   typename MethodPointer::MethodType entry, 
				   typename MethodPointer::MethodType during, 
				   typename MethodPointer::MethodType exit,
				   typename MethodPointer::MethodType kill,
				   typename MethodPointer::MethodType suspend,
				   typename MethodPointer::MethodType resume)
    : m_kill(stateMachine, kill), 
    m_suspend(stateMachine, suspend), 
    m_resume(stateMachine, resume), 
    m_entry(stateMachine, entry), 
    m_during(stateMachine, during), 
    m_exit(stateMachine, exit)
  {}

  template <class SM>
  TemplateState<SM>::~TemplateState()
  {}

  template <class SM>
  void TemplateState<SM>::entry() { m_entry() ; }

  template <class SM>
  void TemplateState<SM>::during() { m_during() ; }
  
  template <class SM>
  void TemplateState<SM>::exit() { m_exit() ; }

  template <class SM>
  void TemplateState<SM>::kill() { m_kill() ; }

  template <class SM>
  void TemplateState<SM>::suspend() { m_suspend() ; }

  template <class SM>
  void TemplateState<SM>::resume() { m_resume() ; }
} ;
#endif

