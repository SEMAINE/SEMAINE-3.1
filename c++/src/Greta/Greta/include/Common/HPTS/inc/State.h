#ifndef _HPTS_State
#define _HPTS_State

#include <Presentation.h>
#include <SignalsInterface.h>
#include <list>


namespace HPTS
{

  class Transition ;
  class Resource ;

  /** \brief Classe de représentation d'un état d'automate

  Cette classe abstraite offre un interface décrivant les informations
  utiles associées à un état d'un automate. Ces informations sont
  \li La réaction aux signaux.
  \li Le code d'entrée dans l'état.
  \li Le code de sortie de l'état.
  \li Le code associe a l'état.
  \li La liste des ressources utilisées dans l'état.
   */
  class HPTS_DLL State : public SignalsInterface
  {
    /** \brief Liste de transitions sortante. */
    Protected std::list<Transition*> m_transitionList ;
    
    /** \brief Liste des ressources nécessaire à l'état. */
    Protected std::list<Resource*>   m_resourceList ;

    /** \brief Référence une transition sortante.
	
    Pour pouvoir manipuler la structure de l'automate, le choix a ete fait de 
    référencer les transitions sortantes d'un état à l'intérieur de l'état via cette méthode.
    De cette façon, à partir de la simple information sur l'état, il devient possible d'exécuter
    l'automate et de faire des calculs sur la structure de ce dernier.
    \param transition Une transition sortante à ajouter.
    */
    Public void addTransition(Transition * transition) ;

    /** \brief Destructeur */
    Public virtual ~State() ;

    /** \brief Méthode correspondant au code d'entrée dans l'état. 

    Ce code est à exécuter lors de l'entrée par transition dans l'état.
    \sa HPTS::StateMachineController::compute()
    */
    Public virtual void entry() = 0 ;
    /** \brief Méthode correspondant au code associe à l'état 

    Ce code est à exécuter tant que l'on reste dans l'état.
    \sa HPTS::StateMachineController::compute()
    */
    Public virtual void during() = 0 ;

    /** \brief Méthode correspondant au code de sortie de l'état 
     
    Ce code est à exécuter lorsqu'une transition provoque la sortie de l'état.
    \sa HPTS::StateMachineController::compute()
    */
    Public virtual void exit() = 0 ;

    /** \brief Récupération de la liste des ressources utilisées dans cet état

    \return La liste des ressources utilisées dans cet état.
     */
    Public const std::list<Resource*> & resources() const ;

    /** \brief Ajout d'une ressource utilisée par l'états
	
    \param resource Une ressource ajoutée à la liste des ressources utilisées par l'état.
     */
    Public void addResource(Resource * resource) ;

    /** \brief Ajout d'une liste de ressources utilisées par l'état

    \param resource Une liste de ressources ajoutées à la liste des ressources utilisées par l'état
     */
    Public void addResource(std::list<Resource*> const & resource) ;

    /** \brief Consultation de la liste des transitions sortantes de l'état

    \return La liste des transitions sortantes.
     */
    Public const std::list<Transition*> & transitions() const ;

    /** \brief Permet de savoir s'il s'agit d'un état final d'un automate
	
    Un état final d'automate est défini comme un état ne possédant pas de transition sortante.
     */
    Public bool final() const ;
  } ;

} ;

#endif

