#ifndef _HPTS_SignalsInterface_H
#define _HPTS_SignalsInterface_H

//#include <Presentation.h>
#include <HPTS/hpts_dll.h>

namespace HPTS
{
  /** \brief Interface de gestion des signaux

  Cette classe abstraite fourni une interface pour les objets pouvant recevoir et
  gérer des signaux de type kill/suspend/resume
  \sa HPTS::StateMachineController
  \sa HPTS::StateMachine
  \sa HPTS::State
   */
  class SignalsInterface
  {
    /** \brief Méthode de réaction au signal kill */
    public: virtual void kill()=0 ;
    
    /** \brief Méthode de réaction au signal suspend */
    public: virtual void suspend()=0 ;

    /** \brief Méthode de réaction au signal resume */
    public: virtual void resume()=0 ;
  } ;

} ;

#endif

