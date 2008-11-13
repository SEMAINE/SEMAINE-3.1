#ifndef _HPTS_StateMachineController_H
#define _HPTS_StateMachineController_H

#include <Presentation.h>
#include <StateMachine.h>
#include <TemplateMapId.h>
#include <MapStringId.h>
#include <Resource.h>
#include <StateMachineStructure.h>
#include <Set.h>

namespace HPTS
{
  
  /**
     \brief Classe de contr�leur g�rant l'ex�cution de plusieurs automates.

     Son r�le est multiple:
     \li Gestion des ressources.
     \li Gestion de l'ex�cution des automates avec ordonnancement.
     \li Envoi de signaux aux automates.

     \author Fabrice LAMARCHE
  */
  class HPTS_DLL StateMachineController
  {

    // -----
    // Types
    // -----
    /** \brief Type �num�rant les �tats d'ex�cution possibles pour un automate. */
    Public typedef enum {
      /** L'automate est en cours d'ex�cution. */
      running, 
      /** L'automate est suspendu dans son ex�cution, donc en attente d'un signal resume. */
      suspended, 
      /** L'automate est termine, son ex�cution est donc arretee. */
      ended,
      /** L'automate a �t� tu� par un signal kill, son ex�cution est donc arr�t�e */
      killed} Status ;

    /** \brief Classe de sous contr�leur g�rant l'ex�cution d'un automate.

        Ce contr�leur est uniquement utilise en interne de la classe StateMachineController
	et de ses �ventuelles classes filles. Elle constitue la repr�sentation d'un automate manipul�e
	par un contr�leur en contenant les informations sur l'�tat d'ex�cution de l'automate ainsi que
	sur les calculs qui ont �t�s effectu�s pour chercher les d�pendances de ressources.

	\author Fabrice LAMARCHE
     */
    Protected class SubController
    {
      /** \brief Pointeur sur l'automate g�r� par ce sous-contr�leur */
      Protected StateMachine *             m_stateMachine ;
      /** \brief Bool�en permettant de savoir si la destruction de l'automate est g�r�e par le contr�leur ou l'utilisateur */
      Protected bool                       m_autoDelete ;
      /** \brief Etat d'ex�cution de l'automate */
      Protected Status                     m_status ;
      /** \brief Etat courant de l'automate */
      Protected State *                    m_currentState ;

      /** \brief Structure de l'automate. 

      Cette donn�e membre sert pour le calcul des d�pendances de ressources
      utilis� pour l'�vitement des inter blocages.
       */
      Protected StateMachineStructure *    m_stateMachineStructure ;

      /** \brief Transition choisie a ce pas de temps
  
      Si ce pointeur est a NULL, c'est qu'il n'y a pas de transition � franchir.
      */
      Protected Transition *               m_selectedTransition ;

      /**
	 \brief Constructeur du sous-contr�leur
	 \param stateMachine L'automate g�r� par ce sous-contr�leur.
	 \param autoDelete Permet de savoir si la destruction de l'automate est g�r�e par le contr�leur ou par l'utilisateur.
       */
      Public inline SubController(StateMachine * stateMachine, bool autoDelete) ;
      /**
	 \brief Destructeur du sous-contr�leur.

	 Ce dernier d�truit l'automate qu'il contr�le si le syst�me de destruction automatique d'automate est actif
	 (si m_autoDelete est vrai).
       */
      Public inline ~SubController() ;

      /** \brief R�cup�ration de l'�tat de l'automate 
	  \return running/suspended/ended/killed. 
      */
      Public inline Status         status() const ;
      /** \brief R�cup�ration de l'�tat courant de l'automate. 
	  \return pointeur sur l'etat courant de l'automate
      */
      Public inline State *        currentState() const ;
      /** \brief R�cup�ration de l'automate g�r� par ce sous contr�leur
	  \return Pointeur sur l'automate.
      */
      Public inline StateMachine * stateMachine() const ;
      
      // Controle d'execution, selection de la transition a prendre (NULL correspond a pas de transition a faire)
      /**
	 \brief M�thode permettant d'agir sur l'ex�cution de l'automate.
	 \param transition Pointeur sur la transition a passer a ce pas de temps. Si NULL, pas de transition a passer.
       */
      Public inline void    selectTransition(Transition * transition)  ;

      /** \brief M�thode ex�cutant le code d'entr�e dans l'�tat (extr�mit�) si une transition a �t� s�lectionn�e. */
      Public inline void entryState() ;

      /** \brief M�thode ex�cutant le code de sortie de l'�tat (origine) si une transition a �t� s�lectionn�e. */
      Public inline void exitState() ;

      /** \brief M�thode ex�cutant le code associe au fait de rester dans un �tat */
      Public inline void duringState() ;

      /** \brief Ex�cution de l'action associ�e � la transition si une transition a �t� s�lectionn�e */
      Public inline void actionTransition() ;

      /** \brief Envoi d'un signal kill � l'automate g�r� par le contr�leur 
	  
      L'envoi de ce signal fait passer l'automate � l'�tat killed
       */
      Public inline void kill() ;

      /** \brief Envoi d'un signal suspend a l'automate g�r� par le contr�leur 
      
      L'envoi de ce signal fait passer l'automate dans l'�tat suspended.
      */
      Public inline void suspend() ;

      /** \brief Envoi d'un signal resume � l'automate g�r� par le contr�leur 
      
      L'envoi de ce signal fait passer l'automate de l'�tat suspended � l'�tat resumed.
      */
      Public inline void resume() ;

      // Calcul des resources prises et heritees par un etat de cet automate
      /** \brief Calcul d'un ensemble a partir d'une liste de ressources.

      \param resList La liste des ressources a ajouter dans l'ensemble.
      \param result L'ensemble dans lequel les ressources ont �t� ajout�es.
       */
      Protected void addResources(std::list<Resource*> const & resList, Set & result) ; 

      /** \brief Calcul des ressources prises et h�rit�es d'un �tat donn� de l'automate.

      \param state L'�tat dont on veut le calcul des ressources.
      \param taken L'ensemble des ressources prises par l'�tat.
      \param inherited L'ensemble des ressources h�rit�es par l'�tat (les d�pendances de ressources).
       */
      Public    void computeResources(State * state, Set & taken, Set & inherited) ;
    } ;
    /** \brief Association identifiant / automate
	
    Ce type est utilise pour faire correspondre un num�ro d'identifiant � un pointeur d'automate.
    */
    Public typedef TemplateMapId<StateMachine*>     MapStateMachineId ;

    /** \brief Le type d'identifiant utilis� pour les automates */
    Public typedef MapStateMachineId::Identifier    Identifier ;

    /** \brief Association entre l'identifiant d'automate et le sous-contr�leur charge de son ex�cution */
    Public typedef std::pair<Identifier, SubController*> StateMachineInfo ;

    /** \brief Type de liste d'automate */
    Public typedef std::list<StateMachineInfo>           StateMachineList ;

    /** \brief Les types de signaux

	Ce type est utilise de mani�re interne pour stocker les signaux envoy�s aux diff�rents automates.
     */
    Public typedef enum {
      /** Signal kill, force la terminaison de l'automate. */
      killSignal, 
      /** Signal suspend, suspend l'ex�cution de l'automate, la reprise se faisant avec le signal resume */
      suspendSignal, 
      /** Signal resume, reprend l'ex�cution d'un automate ayant pr�alablement re�u un signal suspend */
      resumeSignal}  SignalType ;

    /** \brief Association Identifiant d'automate / Type de signal envoy�.
     */
    Public typedef std::pair<Identifier, SignalType>  Signal ;

    // ---------------
    // Donnees membres
    // ---------------
    /** \brief Association d'un identifiant � une cha�ne de caract�res nommant une ressource */
    Protected MapStringId       m_resourceMap ;

    /** \brief Table des sous-contr�leurs actifs 
	
    Cette table est index�e suivant le num�ro d'identifiant associ� � l'automate g�r� par le sous contr�leur.
    */
    std::vector<SubController*>             m_subControllers ;

    /** \brief Table d'iterateurs sur la position d'un automate dans la liste des automates.

    Cette table est index�e sur le num�ro d'identifiant de l'automate g�r� par le sous-contr�leur
    sur lequel pointe l'iterateur.
     */
    std::vector<StateMachineList::iterator> m_location ;

    /** \brief Liste des automates g�r�s par le contr�leur
     */
    StateMachineList                   m_runningStateMachines ;    

    /** \brief Liste des signaux � g�rer.

    Cette liste contient l'ensemble des signaux stocke durant l'ex�cution d'un pas de temps.
    Elle est donc remise � jour � chaque pas de temps.
     */
    std::list<Signal>    m_queuedSignals ;

    /** \brief Table de correspondance ressource / automate
	
    Cette table est index�e par le num�ro d'identifiant associe aux ressources. 
    Par cet interm�diaire, il est possible de r�cup�rer l'automate poss�dant actuellement
    un ressource donn�e.
     */
    std::vector<SubController*> m_resourceStateMachine ;


    /** \brief Constructeur */	
    Public StateMachineController() ;

    // Recuperation/declaration d'une resource associee a ce controleur
    /** \brief D�claration d'une nouvelle ressource

    Cette m�thode permet de r�cup�rer un pointeur sur une ressource identifi�e par la cha�ne
    de caract�res pass�e en param�tre.
    \param name Le nom identifiant la ressource.
    \return Un pointeur sur la ressource.
    \remarks La ressource est allou�e dynamiquement, la destruction de la ressource point�e
    est donc a la charge du programmeur.
     */
    Public Resource * resource(std::string const & name) ;
    
    // Controle d'un automate
    // Seul l'identifiant associe a l'automate permet de le controller
    /** \brief Lancement d'un nouvel automate.
	
    Cette m�thode prend l'automate passe en param�tre, l'initialise et le r�f�rence comme g�r� par ce contr�leur.
    \param stateMachine Le nouvel automate.
    \param autoDelete Le contr�leur doit-il g�rer la destruction de l'automate (par d�faut oui).
    \return L'identifiant associe a l'automate par le contr�leur
     */
    Public Identifier start(StateMachine * stateMachine, bool autoDelete=true) ;
    
    /** \brief Envoi d'un signal kill a un automate.
	
    Tue un automate en arr�tant son ex�cution.
    Fait passer l'automate dans l'�tat killed.
    \param id L'identifiant de l'automate � tuer
     */
    Public void kill(Identifier const & id) ;

    /** \brief Envoi d'un signal suspend a un automate.
	
    Suspend temporairement l'ex�cution de l'automate.
    Fait passer l'automate dans l'�tat suspended.
    \param id L'identifiant de l'automate � suspendre.
     */
    Public void suspend(Identifier const & id) ;

    /** \brief Envoi d'un signal resume � un automate.

    Reprend l'ex�cution d'un automate pr�alablement suspendu.
    \param id L'identifiant associe a l'automate dont l'ex�cution doit reprendre.
     */
    Public void resume(Identifier const & id) ;

    /** \brief R�cup�ration de l'�tat d'un automate.

    \param id L'identifiant de l'automate dont on veut l'�tat.
    \return L'�tat d'ex�cution de l'automate running/suspended/killed/ended.
     */
    Public Status status(Identifier const & id) ;

    // Recuperation de l'identifiant d'un automate
    /** \brief R�cup�ration de l'identifiant d'un automate.

	\param stateMachine L'automate dont on veut l'identifiant.
	\return L'identifiant associ�.
     */
    Public inline Identifier identifier(StateMachine * stateMachine) ;

    // Gestion des signaux
    /** \brief M�thode g�rant l'envoi des signaux 

    Elle parcourt la liste des signaux stockes dans m_queuedSignals en les envoyant au sous-contr�leur
    charge de l'ex�cution de l'automate cible.
    */
    Protected void computeSignals() ;

    /** \brief M�thode g�rant la terminaison des automates au pas de temps courant.
     */
    Protected void computeEndedStateMachines() ;

    /** \brief Calcul d'un pas de simulation.

    Un pas de simulation est d�coup� en plusieurs phases:
    \par 
    1) Envoi des signaux stockes durant le pas de temps pr�c�dent
    \par 
    2) Ex�cution du code a ex�cuter avant chaque pas de temps pour tous les automates
    \par
    3) Collecte des transitions franchissables pour tous les automates
    \par
    4) Ordonnancement des automates
    \par
    5) Ex�cution du code de sortie d'�tat pour tous les automates franchissant une transition
    \par
    6) Ex�cution du code associe a la transition pour tous les automates franchissant une transition
    \par
    7) Ex�cution du code d'entr�e dans l'�tat pour tous les automates franchissant une transition
    \par
    8) Ex�cution du code associe a l'�tat pour tous les automates.
     */
    Public virtual void compute() ;

    /** \brief Le contr�leur g�re-t-il encore des automates ?
     */
    Public inline bool runningStateMachines() const ;

    /** \brief R�cup�ration de l'automate poss�dant une ressource
	
    \param resource La ressource sur laquelle on cherche l'information
    \return L'automate poss�dant actuellement cette ressource. NULL si la ressource n'est pas prise par un automate.
     */
    Public inline StateMachine * takenBy(HPTS::Resource * resource) ;

    /** \brief destructeur du controleur
	
    Lors de la destruction du contr�leur, tous les automates encore g�r�s par se dernier se voient
    envoyer un signal kill pour forcer leur terminaison de mani�re propre.
     */
    Public virtual ~StateMachineController() ;

    class DestroyCallBack ;
    friend class DestroyCallBack ;

    /** \brief Classe de callBack pour destruction synchrone lors du dereferencement.

    Cette classe est utilisee en combinaison de la classe MapStateMachineId pour detruire
    directement un automate lorsque ce dernier n'est plus reference.
     */
    class DestroyCallBack : public MapStateMachineId::CallBack
    {
      Protected StateMachineController * m_ctrl ;

      /** \brief Constructeur prenant en parametre l'adresse du StateMachineController qui lui est associe. */
      Public DestroyCallBack(StateMachineController * ctrl)
	: m_ctrl(ctrl)
      {}
      
      /** \brief action associee au call back.
	  
      Cette action detruit le sous controlleur associe a l'automate qui n'est plus reference.
       */
      Public virtual void action(int id)
      {	
	delete m_ctrl->m_subControllers[id] ; 
	m_ctrl->m_subControllers[id]=NULL ;
      }

      /** \brief destructeur virtuel. Au cas ou. */
      Public virtual ~DestroyCallBack()
      {}
    } ;

    /** \brief Le call back effectivement utilise par cette instance de controleur. */
    DestroyCallBack m_destroyCallBack ;

    /** \brief Association d'un identifiant�a un automate */
    Protected MapStateMachineId m_stateMachineMap ;
  } ;

  StateMachineController::Identifier StateMachineController::identifier(StateMachine * stateMachine)
  {
    if(m_stateMachineMap.exist(stateMachine))
      {
	return m_stateMachineMap.reference(stateMachine) ;
      }
    return Identifier() ;
  }


  bool StateMachineController::runningStateMachines() const
  { return !m_runningStateMachines.empty() ; }

  StateMachineController::SubController::SubController(StateMachine * stateMachine, bool autoDelete) 
    : m_stateMachine(stateMachine), m_autoDelete(autoDelete), 
    m_status(running)
  {
    m_stateMachine->initStateMachine() ;
    m_stateMachineStructure = new StateMachineStructure(m_stateMachine) ;
    m_selectedTransition = NULL ;
    m_currentState = m_stateMachine->entry() ;
    m_stateMachine->entry()->entry() ;
    m_stateMachine->entry()->during() ;
  }

  StateMachine * StateMachineController::takenBy(HPTS::Resource * resource) 
  {
    SubController * subController = m_resourceStateMachine[resource->identifier()()] ;
    if(subController)
      return subController->stateMachine() ;
    return NULL ;
  }


  StateMachineController::SubController::~SubController() 
  {
    if(m_autoDelete) { delete m_stateMachine ; }
    delete m_stateMachineStructure ;
  }

  // Recuperation des informations sur l'automate
  StateMachineController::Status  StateMachineController::SubController::status() const 
  {
    return m_status ;
  }

  State * StateMachineController::SubController::currentState() const 
  {
    return m_currentState ;
  }

  StateMachine * StateMachineController::SubController::stateMachine() const
  {
    return m_stateMachine ;
  }

  // Controle d'execution, selection de la transition a prendre (NULL correspond a pas de transition a faire)
  void    StateMachineController::SubController::selectTransition(Transition * transition)
  {
    m_selectedTransition = transition ;
  }

  // Execution d'un pas de temps
  // S'appuie sur select Transition...
  void StateMachineController::SubController::entryState() 
  {
    if(m_selectedTransition!=NULL && m_status==running)
      {
	m_currentState->entry() ;
      }
  }

  void StateMachineController::SubController::exitState() 
  {
    if(m_selectedTransition!=NULL && m_status==running)
      {
	m_currentState->exit() ;
      }  
  }

  void StateMachineController::SubController::duringState() 
  {
    if(m_status==running)
      {
	m_currentState->during() ;
      }
    m_selectedTransition=NULL ;
    if(m_currentState->final())
      {
	m_status=ended ;
      }
  }

  void StateMachineController::SubController::actionTransition() 
  {
    if(m_selectedTransition!=NULL && m_status==running)
      {
	m_selectedTransition->action() ;
	m_currentState = m_selectedTransition->extremity() ;
      }  
  }

  // Controle d'un automate
  void StateMachineController::SubController::kill() 
  {
    if(m_status==ended) { return ; }
    m_currentState->kill() ;
    m_stateMachine->kill() ;
    m_status=killed ;
  }

  void StateMachineController::SubController::suspend() 
  {
    if(m_status!=running) { return ; }
    m_currentState->suspend() ;
    m_stateMachine->suspend() ;
    m_status=suspended ;
  }

  void StateMachineController::SubController::resume() 
  {
    if(m_status!=suspended) { return ; }
    m_stateMachine->resume() ;
    m_currentState->resume() ;
    m_status=running ;
  }

}

#endif

