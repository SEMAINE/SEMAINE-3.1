#ifndef _StateMachineControllerNoResource_H
#define _StateMachineControllerNoResource_H

#include <StateMachineController.h>

namespace HPTS
{
    /**
     \brief Classe de contr�leur g�rant l'ex�cution de plusieurs automates.

     A la diff�rence de la classe StateMachineController, cette classe ne fait pas de calcul
     d'ordonnancement et de gestion de ressources. Ce contr�leur est donc sensiblement plus rapide
     que le contr�leur StateMachineController.

     \author Fabrice LAMARCHE
  */
  class HPTS_DLL StateMachineControllerNoResource : public StateMachineController
  {
    
    Public StateMachineControllerNoResource () {}

    /** \brief Calcul d'un pas de simulation.
	
    Un pas de simulation est d�coup� en plusieurs phases:
    \par 
    1) Envoi des signaux stockes durant le pas de temps pr�c�dent
    \par 
    2) Ex�cution du code a ex�cuter au d�but de chaque pas de temps pour tous les automates
    \par
    3) Collecte des transitions franchissables pour tous les automates
    \par
    4) Ex�cution du code de sortie d'�tat pour tous les automates franchissant une transition
    \par
    5) Ex�cution du code associe � la transition pour tous les automates franchissant une transition
    \par
    6) Ex�cution du code d'entr�e dans l'�tat pour tous les automates franchissant une transition
    \par
    7) Ex�cution du code associe � l'�tat pour tous les automates.
     */
    Public virtual void compute() ;
  } ;

}

#endif

