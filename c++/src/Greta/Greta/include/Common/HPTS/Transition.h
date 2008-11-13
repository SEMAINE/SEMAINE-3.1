#ifndef _HPTS_Transition
#define _HPTS_Transition

//#include <Presentation.h>
#include <HPTS/hpts_dll.h>

namespace HPTS
{

  class State ;

  /** \brief Classe décrivant une transition d'automate.

  Cette classe décrit une transition d'automate et les informations qui lui sont associées:
  \li Etat d'origine de la transition.
  \li Etat d'extrémité de la transition.
  \li Action associée à la transition
  \li Degré de préférence associe à la transition
  \li Condition associée a la transition
   */
  class HPTS_DLL Transition
  {
    /** \brief Etat d'origine de la transition */
    protected: State * m_origin ;
    
    /** \brief Etat d'extrémité de la transition */
    protected: State * m_extremity ;

    /** \brief Constructeur de transition.
	
    \param origin Etat d'origine de la transition.
    \param extremity Etat d'extrémité de la transition.
     */
    public: Transition(State * origin, State * extremity) ;

    /** \brief Destructeur */
    public: virtual ~Transition() ;

    /** \brief Référence une transition dans son état d'origine. */
    public: void referenceTransition() ;

    /** \brief Récupération de l'état d'origine de la transition */
    public: State * origin()    const { return m_origin ; }

    /** \brief Récupération de l'état d'extrémité de la transition */
    public: State * extremity() const { return m_extremity ; }
    
    /** \brief Degré de préférence associé à la transition.

    \return Valeur du degré de préférence
     */
    public: virtual double preference() = 0 ;

    /** \brief Condition associée à la transition.

    \retval true La transition est franchissable.
    \retval false La transition n'est par franchissable.
     */
    public: virtual bool condition() = 0 ;

    /** \brief Action associée à la transition. */
    public: virtual void action() = 0 ;
  } ;

} ;

#endif

