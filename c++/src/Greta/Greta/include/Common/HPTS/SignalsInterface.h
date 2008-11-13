#ifndef _HPTS_SignalsInterface_H
#define _HPTS_SignalsInterface_H

//#include <Presentation.h>
#include <HPTS/hpts_dll.h>

namespace HPTS
{
  /** \brief Interface de gestion des signaux

  Cette classe abstraite fourni une interface pour les objets pouvant recevoir et
  g�rer des signaux de type kill/suspend/resume
  \sa HPTS::StateMachineController
  \sa HPTS::StateMachine
  \sa HPTS::State
   */
  class SignalsInterface
  {
    /** \brief M�thode de r�action au signal kill */
    public: virtual void kill()=0 ;
    
    /** \brief M�thode de r�action au signal suspend */
    public: virtual void suspend()=0 ;

    /** \brief M�thode de r�action au signal resume */
    public: virtual void resume()=0 ;
  } ;

} ;

#endif

