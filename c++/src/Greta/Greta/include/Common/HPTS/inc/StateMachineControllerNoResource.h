#ifndef _StateMachineControllerNoResource_H
#define _StateMachineControllerNoResource_H

#include <StateMachineController.h>

namespace HPTS
{
    /**
     \brief Classe de contrôleur gérant l'exécution de plusieurs automates.

     A la différence de la classe StateMachineController, cette classe ne fait pas de calcul
     d'ordonnancement et de gestion de ressources. Ce contrôleur est donc sensiblement plus rapide
     que le contrôleur StateMachineController.

     \author Fabrice LAMARCHE
  */
  class HPTS_DLL StateMachineControllerNoResource : public StateMachineController
  {
    
    Public StateMachineControllerNoResource () {}

    /** \brief Calcul d'un pas de simulation.
	
    Un pas de simulation est découpé en plusieurs phases:
    \par 
    1) Envoi des signaux stockes durant le pas de temps précédent
    \par 
    2) Exécution du code a exécuter au début de chaque pas de temps pour tous les automates
    \par
    3) Collecte des transitions franchissables pour tous les automates
    \par
    4) Exécution du code de sortie d'état pour tous les automates franchissant une transition
    \par
    5) Exécution du code associe à la transition pour tous les automates franchissant une transition
    \par
    6) Exécution du code d'entrée dans l'état pour tous les automates franchissant une transition
    \par
    7) Exécution du code associe à l'état pour tous les automates.
     */
    Public virtual void compute() ;
  } ;

}

#endif

