#ifndef _HPTS_Transition
#define _HPTS_Transition

//#include <Presentation.h>
#include <HPTS/hpts_dll.h>

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
    protected: State * m_origin ;
    
    /** \brief Etat d'extr�mit� de la transition */
    protected: State * m_extremity ;

    /** \brief Constructeur de transition.
	
    \param origin Etat d'origine de la transition.
    \param extremity Etat d'extr�mit� de la transition.
     */
    public: Transition(State * origin, State * extremity) ;

    /** \brief Destructeur */
    public: virtual ~Transition() ;

    /** \brief R�f�rence une transition dans son �tat d'origine. */
    public: void referenceTransition() ;

    /** \brief R�cup�ration de l'�tat d'origine de la transition */
    public: State * origin()    const { return m_origin ; }

    /** \brief R�cup�ration de l'�tat d'extr�mit� de la transition */
    public: State * extremity() const { return m_extremity ; }
    
    /** \brief Degr� de pr�f�rence associ� � la transition.

    \return Valeur du degr� de pr�f�rence
     */
    public: virtual double preference() = 0 ;

    /** \brief Condition associ�e � la transition.

    \retval true La transition est franchissable.
    \retval false La transition n'est par franchissable.
     */
    public: virtual bool condition() = 0 ;

    /** \brief Action associ�e � la transition. */
    public: virtual void action() = 0 ;
  } ;

} ;

#endif

