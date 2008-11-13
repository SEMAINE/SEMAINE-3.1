#ifndef _HPTS_State
#define _HPTS_State

#include <Presentation.h>
#include <SignalsInterface.h>
#include <list>


namespace HPTS
{

  class Transition ;
  class Resource ;

  /** \brief Classe de repr�sentation d'un �tat d'automate

  Cette classe abstraite offre un interface d�crivant les informations
  utiles associ�es � un �tat d'un automate. Ces informations sont
  \li La r�action aux signaux.
  \li Le code d'entr�e dans l'�tat.
  \li Le code de sortie de l'�tat.
  \li Le code associe a l'�tat.
  \li La liste des ressources utilis�es dans l'�tat.
   */
  class HPTS_DLL State : public SignalsInterface
  {
    /** \brief Liste de transitions sortante. */
    Protected std::list<Transition*> m_transitionList ;
    
    /** \brief Liste des ressources n�cessaire � l'�tat. */
    Protected std::list<Resource*>   m_resourceList ;

    /** \brief R�f�rence une transition sortante.
	
    Pour pouvoir manipuler la structure de l'automate, le choix a ete fait de 
    r�f�rencer les transitions sortantes d'un �tat � l'int�rieur de l'�tat via cette m�thode.
    De cette fa�on, � partir de la simple information sur l'�tat, il devient possible d'ex�cuter
    l'automate et de faire des calculs sur la structure de ce dernier.
    \param transition Une transition sortante � ajouter.
    */
    Public void addTransition(Transition * transition) ;

    /** \brief Destructeur */
    Public virtual ~State() ;

    /** \brief M�thode correspondant au code d'entr�e dans l'�tat. 

    Ce code est � ex�cuter lors de l'entr�e par transition dans l'�tat.
    \sa HPTS::StateMachineController::compute()
    */
    Public virtual void entry() = 0 ;
    /** \brief M�thode correspondant au code associe � l'�tat 

    Ce code est � ex�cuter tant que l'on reste dans l'�tat.
    \sa HPTS::StateMachineController::compute()
    */
    Public virtual void during() = 0 ;

    /** \brief M�thode correspondant au code de sortie de l'�tat 
     
    Ce code est � ex�cuter lorsqu'une transition provoque la sortie de l'�tat.
    \sa HPTS::StateMachineController::compute()
    */
    Public virtual void exit() = 0 ;

    /** \brief R�cup�ration de la liste des ressources utilis�es dans cet �tat

    \return La liste des ressources utilis�es dans cet �tat.
     */
    Public const std::list<Resource*> & resources() const ;

    /** \brief Ajout d'une ressource utilis�e par l'�tats
	
    \param resource Une ressource ajout�e � la liste des ressources utilis�es par l'�tat.
     */
    Public void addResource(Resource * resource) ;

    /** \brief Ajout d'une liste de ressources utilis�es par l'�tat

    \param resource Une liste de ressources ajout�es � la liste des ressources utilis�es par l'�tat
     */
    Public void addResource(std::list<Resource*> const & resource) ;

    /** \brief Consultation de la liste des transitions sortantes de l'�tat

    \return La liste des transitions sortantes.
     */
    Public const std::list<Transition*> & transitions() const ;

    /** \brief Permet de savoir s'il s'agit d'un �tat final d'un automate
	
    Un �tat final d'automate est d�fini comme un �tat ne poss�dant pas de transition sortante.
     */
    Public bool final() const ;
  } ;

} ;

#endif

