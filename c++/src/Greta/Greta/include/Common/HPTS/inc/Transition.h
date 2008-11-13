#ifndef _HPTS_Transition
#define _HPTS_Transition

#include <Presentation.h>

namespace HPTS
{

  class State ;

  /** \brief Classe d�crivant une transition d'automate.

  Cette classe d�crit une transition d'automate et les informations qui lui sont associ�es:
  \li Etat d'origine de la transition.
  \li Etat d'extr�mit� de la transition.
  \li Action associ�e � la transition
  \li Degr� de pr�f�rence associe � la transition
  \li Condition associ�e a la transition
   */
  class HPTS_DLL Transition
  {
    /** \brief Etat d'origine de la transition */
    Protected State * m_origin ;
    
    /** \brief Etat d'extr�mit� de la transition */
    Protected State * m_extremity ;

    /** \brief Constructeur de transition.
	
    \param origin Etat d'origine de la transition.
    \param extremity Etat d'extr�mit� de la transition.
     */
    Public Transition(State * origin, State * extremity) ;

    /** \brief Destructeur */
    Public virtual ~Transition() ;

    /** \brief R�f�rence une transition dans son �tat d'origine. */
    Public void referenceTransition() ;

    /** \brief R�cup�ration de l'�tat d'origine de la transition */
    Public State * origin()    const { return m_origin ; }

    /** \brief R�cup�ration de l'�tat d'extr�mit� de la transition */
    Public State * extremity() const { return m_extremity ; }
    
    /** \brief Degr� de pr�f�rence associ� � la transition.

    \return Valeur du degr� de pr�f�rence
     */
    Public virtual double preference() = 0 ;

    /** \brief Condition associ�e � la transition.

    \retval true La transition est franchissable.
    \retval false La transition n'est par franchissable.
     */
    Public virtual bool condition() = 0 ;

    /** \brief Action associ�e � la transition. */
    Public virtual void action() = 0 ;
  } ;

} ;

#endif

