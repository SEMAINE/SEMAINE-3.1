#ifndef _HPTS_TemplateState_H
#define _HPTS_TemplateState_H

#include <State.h>
#include <ClassMethodPointer.h>
#include <ClassVoidMethodPointer.h>

namespace HPTS
{

  /** \brief Classe de description d'un �tat d'automate g�n�re par le compilateur d'HPTS++.

  Ce type est utilise pour g�n�rer des �tats � partir de la description des automates
  via le compilateur de HPTS++. En effet, lorsqu'une classe d'automate est d�crite, un ensemble
  de m�thodes propres a l'automate correspondant aux diff�rents codes associ�s aux �tats est g�n�r�.
  Cette classe permet de r�cup�rer des pointeurs sur ces m�thodes pour en faire des �tats.


  \param SM Le type de l'automate auquel cet �tat est associe
  */
  template <class SM>
  class TemplateState : public State
  {
    /** \brief Type de pointeur sur m�thode 

    Ce type d�crit un pointeur sur une m�thode de la classe SM ne prenant pas
    de param�tres et renvoyant un type void.
    */
    Public typedef ClassVoidMethodPointer<SM> MethodPointer ;

    /** \brief Pointeur sur la m�thode de SM correspondant � la r�action au signal kill */
    Protected MethodPointer m_kill ;

    /** \brief Pointeur sur la m�thode de SM correspondant � la r�action au signal suspend */
    Protected MethodPointer m_suspend ;

    /** \brief Pointeur sur la m�thode de SM correspondant � la r�action au signal resume */
    Protected MethodPointer m_resume ;

    /** \brief Pointeur sur la m�thode de SM correspondant � l'entr�e dans l'�tat */
    Protected MethodPointer m_entry ;
    
    /** \brief Pointeur sur la m�thode de SM correspondant au code associ� � l'�tat */
    Protected MethodPointer m_during ;
 
    /** \brief Pointeur sur la m�thode de SM correspondant � la sortie de l'�tat */
    Protected MethodPointer m_exit ;


    /** \brief Constructeur g�n�rant un �tat associ� � la classe d'automate SM

    \param stateMachine L'instance d'automate auquel appartient cet �tat. 
    \param entry Pointeur sur la m�thode de SM correspondant � l'entr�e dans l'�tat.
    \param during Pointeur sur la m�thode de SM correspondant au code associe � l'�tat.
    \param exit Pointeur sur la m�thode de SM correspondant au code de sortie de l'�tat
    \param kill Pointeur sur la m�thode de SM correspondant � la r�action de cet etat au signal kill
    \param suspend Pointeur sur la m�thode de SM correspondant � la r�action de cet �tat au signal suspend
    \param resume Pointeur sur la m�thode de SM correspondant � la r�action de cet �tat au signal resume
     */
    Public TemplateState(SM * stateMachine, 
			 typename MethodPointer::MethodType entry, 
			 typename MethodPointer::MethodType during, 
			 typename MethodPointer::MethodType exit,
			 typename MethodPointer::MethodType kill,
			 typename MethodPointer::MethodType suspend,
			 typename MethodPointer::MethodType resume) ;

    // Destructeur
    Public virtual ~TemplateState() ;

    // Implementation des differentes methodes associees a un etat
    Public virtual void entry() ;
    Public virtual void during() ;
    Public virtual void exit() ;

    // Gestion des signaux
    Public virtual void kill() ;
    Public virtual void suspend() ;
    Public virtual void resume() ;
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

