#ifndef _HPTS_StateMachineController_H
#define _HPTS_StateMachineController_H

//#include <Presentation.h>
#include <HPTS/hpts_dll.h>
#include <HPTS/StateMachine.h>
#include <HPTS/TemplateMapId.h>
#include <HPTS/MapStringId.h>
#include <HPTS/Resource.h>
#include <HPTS/StateMachineStructure.h>
#include <HPTS/Set.h>

namespace HPTS
{
  
  /**
     \brief Classe de contrôleur gérant l'exécution de plusieurs automates.

     Son rôle est multiple:
     \li Gestion des ressources.
     \li Gestion de l'exécution des automates avec ordonnancement.
     \li Envoi de signaux aux automates.

     \author Fabrice LAMARCHE
  */
  class HPTS_DLL StateMachineController
  {

    // -----
    // Types
    // -----
    /** \brief Type énumérant les états d'exécution possibles pour un automate. */
    public: typedef enum {
      /** L'automate est en cours d'exécution. */
      running, 
      /** L'automate est suspendu dans son exécution, donc en attente d'un signal resume. */
      suspended, 
      /** L'automate est termine, son exécution est donc arretee. */
      ended,
      /** L'automate a été tué par un signal kill, son exécution est donc arrêtée */
      killed} Status ;

    /** \brief Classe de sous contrôleur gérant l'exécution d'un automate.

        Ce contrôleur est uniquement utilise en interne de la classe StateMachineController
	et de ses éventuelles classes filles. Elle constitue la représentation d'un automate manipulée
	par un contrôleur en contenant les informations sur l'état d'exécution de l'automate ainsi que
	sur les calculs qui ont étés effectués pour chercher les dépendances de ressources.

	\author Fabrice LAMARCHE
     */
    protected: class SubController
    {
      /** \brief Pointeur sur l'automate géré par ce sous-contrôleur */
      protected: StateMachine *             m_stateMachine ;
      /** \brief Booléen permettant de savoir si la destruction de l'automate est gérée par le contrôleur ou l'utilisateur */
      protected: bool                       m_autoDelete ;
      /** \brief Etat d'exécution de l'automate */
      protected: Status                     m_status ;
      /** \brief Etat courant de l'automate */
      protected: State *                    m_currentState ;

      /** \brief Structure de l'automate. 

      Cette donnée membre sert pour le calcul des dépendances de ressources
      utilisé pour l'évitement des inter blocages.
       */
      protected: StateMachineStructure *    m_stateMachineStructure ;

      /** \brief Transition choisie a ce pas de temps
  
      Si ce pointeur est a NULL, c'est qu'il n'y a pas de transition à franchir.
      */
      protected: Transition *               m_selectedTransition ;

      /**
	 \brief Constructeur du sous-contrôleur
	 \param stateMachine L'automate géré par ce sous-contrôleur.
	 \param autoDelete Permet de savoir si la destruction de l'automate est gérée par le contrôleur ou par l'utilisateur.
       */
      public: inline SubController(StateMachine * stateMachine, bool autoDelete) ;
      /**
	 \brief Destructeur du sous-contrôleur.

	 Ce dernier détruit l'automate qu'il contrôle si le système de destruction automatique d'automate est actif
	 (si m_autoDelete est vrai).
       */
      public: inline ~SubController() ;

      /** \brief Récupération de l'état de l'automate 
	  \return running/suspended/ended/killed. 
      */
      public: inline Status         status() const ;
      /** \brief Récupération de l'état courant de l'automate. 
	  \return pointeur sur l'etat courant de l'automate
      */
      public: inline State *        currentState() const ;
      /** \brief Récupération de l'automate géré par ce sous contrôleur
	  \return Pointeur sur l'automate.
      */
      public: inline StateMachine * stateMachine() const ;
      
      // Controle d'execution, selection de la transition a prendre (NULL correspond a pas de transition a faire)
      /**
	 \brief Méthode permettant d'agir sur l'exécution de l'automate.
	 \param transition Pointeur sur la transition a passer a ce pas de temps. Si NULL, pas de transition a passer.
       */
      public: inline void    selectTransition(Transition * transition)  ;

      /** \brief Méthode exécutant le code d'entrée dans l'état (extrémité) si une transition a été sélectionnée. */
      public: inline void entryState() ;

      /** \brief Méthode exécutant le code de sortie de l'état (origine) si une transition a été sélectionnée. */
      public: inline void exitState() ;

      /** \brief Méthode exécutant le code associe au fait de rester dans un état */
      public: inline void duringState() ;

      /** \brief Exécution de l'action associée à la transition si une transition a été sélectionnée */
      public: inline void actionTransition() ;

      /** \brief Envoi d'un signal kill à l'automate géré par le contrôleur 
	  
      L'envoi de ce signal fait passer l'automate à l'état killed
       */
      public: inline void kill() ;

      /** \brief Envoi d'un signal suspend a l'automate géré par le contrôleur 
      
      L'envoi de ce signal fait passer l'automate dans l'état suspended.
      */
      public: inline void suspend() ;

      /** \brief Envoi d'un signal resume à l'automate géré par le contrôleur 
      
      L'envoi de ce signal fait passer l'automate de l'état suspended à l'état resumed.
      */
      public: inline void resume() ;

      // Calcul des resources prises et heritees par un etat de cet automate
      /** \brief Calcul d'un ensemble a partir d'une liste de ressources.

      \param resList La liste des ressources a ajouter dans l'ensemble.
      \param result L'ensemble dans lequel les ressources ont été ajoutées.
       */
      protected: void addResources(std::list<Resource*> const & resList, Set & result) ; 

      /** \brief Calcul des ressources prises et héritées d'un état donné de l'automate.

      \param state L'état dont on veut le calcul des ressources.
      \param taken L'ensemble des ressources prises par l'état.
      \param inherited L'ensemble des ressources héritées par l'état (les dépendances de ressources).
       */
      public:    void computeResources(State * state, Set & taken, Set & inherited) ;
    } ;
    /** \brief Association identifiant / automate
	
    Ce type est utilise pour faire correspondre un numéro d'identifiant à un pointeur d'automate.
    */
    public: typedef TemplateMapId<StateMachine*>     MapStateMachineId ;

    /** \brief Le type d'identifiant utilisé pour les automates */
    public: typedef MapStateMachineId::Identifier    Identifier ;

    /** \brief Association entre l'identifiant d'automate et le sous-contrôleur charge de son exécution */
    public: typedef std::pair<Identifier, SubController*> StateMachineInfo ;

    /** \brief Type de liste d'automate */
    public: typedef std::list<StateMachineInfo>           StateMachineList ;

    /** \brief Les types de signaux

	Ce type est utilise de manière interne pour stocker les signaux envoyés aux différents automates.
     */
    public: typedef enum {
      /** Signal kill, force la terminaison de l'automate. */
      killSignal, 
      /** Signal suspend, suspend l'exécution de l'automate, la reprise se faisant avec le signal resume */
      suspendSignal, 
      /** Signal resume, reprend l'exécution d'un automate ayant préalablement reçu un signal suspend */
      resumeSignal}  SignalType ;

    /** \brief Association Identifiant d'automate / Type de signal envoyé.
     */
    public: typedef std::pair<Identifier, SignalType>  Signal ;

    // ---------------
    // Donnees membres
    // ---------------
    /** \brief Association d'un identifiant à une chaîne de caractères nommant une ressource */
    protected: MapStringId       m_resourceMap ;

    /** \brief Table des sous-contrôleurs actifs 
	
    Cette table est indexée suivant le numéro d'identifiant associé à l'automate géré par le sous contrôleur.
    */
    std::vector<SubController*>             m_subControllers ;

    /** \brief Table d'iterateurs sur la position d'un automate dans la liste des automates.

    Cette table est indexée sur le numéro d'identifiant de l'automate géré par le sous-contrôleur
    sur lequel pointe l'iterateur.
     */
    std::vector<StateMachineList::iterator> m_location ;

    /** \brief Liste des automates gérés par le contrôleur
     */
    StateMachineList                   m_runningStateMachines ;    

    /** \brief Liste des signaux à gérer.

    Cette liste contient l'ensemble des signaux stocke durant l'exécution d'un pas de temps.
    Elle est donc remise à jour à chaque pas de temps.
     */
    std::list<Signal>    m_queuedSignals ;

    /** \brief Table de correspondance ressource / automate
	
    Cette table est indexée par le numéro d'identifiant associe aux ressources. 
    Par cet intermédiaire, il est possible de récupérer l'automate possédant actuellement
    un ressource donnée.
     */
    std::vector<SubController*> m_resourceStateMachine ;


    /** \brief Constructeur */	
    public: StateMachineController() ;

    // Recuperation/declaration d'une resource associee a ce controleur
    /** \brief Déclaration d'une nouvelle ressource

    Cette méthode permet de récupérer un pointeur sur une ressource identifiée par la chaîne
    de caractères passée en paramètre.
    \param name Le nom identifiant la ressource.
    \return Un pointeur sur la ressource.
    \remarks La ressource est allouée dynamiquement, la destruction de la ressource pointée
    est donc a la charge du programmeur.
     */
    public: Resource * resource(std::string const & name) ;
  
    /** \brief Pre-lancement d'un automate.

    Cette methode permet d'initialiser un automate avant son lancement. Elle effectue l'ensemble des calculs relatifs
    aux dependances de ressources en construisant la structure de l'automate. Grace a cette methode, qui peut etre appelee
    bien avant le lancement de l'automate, son lancement effectif pourra etre plus rapide avec l'utilisation de la methode 
    StateMachineController::start(Identifier).
    \param stateMachine Le nouvel automate.
    \param autoDelete Le contrôleur doit-il gérer la destruction de l'automate (par défaut oui).
    \return L'identifiant associe a l'automate par le contrôleur, il sera utile pour l'appel de StateMachineController::start(Identifier).
    */
    public: StateMachineController::Identifier StateMachineController::preStart(StateMachine * stateMachine, bool autoDelete=true) ;

    /** \brief Lancement d'un automate precedemment initialise.

    Cette methode ne peut etre utilisees qu'en combinaison d'un automate precedemment initialise par l'intermediaire de la methode
    StateMachineController::preStart. Elle reference l'automate pour la prise en charge de son execution par le controleur.
    \param id L'identifiant de l'automate, obtenu par l'appel a StateMachineController::preStart (UNIQUEMENT).
    */
    public: void StateMachineController::start(StateMachineController::Identifier id) ;


    // Controle d'un automate
    // Seul l'identifiant associe a l'automate permet de le controller
    /** \brief Lancement d'un nouvel automate.
	
    Cette méthode prend l'automate passe en paramètre, l'initialise et le référence comme géré par ce contrôleur.
    \param stateMachine Le nouvel automate.
    \param autoDelete Le contrôleur doit-il gérer la destruction de l'automate (par défaut oui).
    \return L'identifiant associe a l'automate par le contrôleur
     */
    public: Identifier start(StateMachine * stateMachine, bool autoDelete=true) ;
    
    /** \brief Envoi d'un signal kill a un automate.
	
    Tue un automate en arrêtant son exécution.
    Fait passer l'automate dans l'état killed.
    \param id L'identifiant de l'automate à tuer
     */
    public: void kill(Identifier const & id) ;

    /** \brief Envoi d'un signal suspend a un automate.
	
    Suspend temporairement l'exécution de l'automate.
    Fait passer l'automate dans l'état suspended.
    \param id L'identifiant de l'automate à suspendre.
     */
    public: void suspend(Identifier const & id) ;

    /** \brief Envoi d'un signal resume à un automate.

    Reprend l'exécution d'un automate préalablement suspendu.
    \param id L'identifiant associe a l'automate dont l'exécution doit reprendre.
     */
    public: void resume(Identifier const & id) ;

    /** \brief Récupération de l'état d'un automate.

    \param id L'identifiant de l'automate dont on veut l'état.
    \return L'état d'exécution de l'automate running/suspended/killed/ended.
     */
    public: Status status(Identifier const & id) ;

    // Recuperation de l'identifiant d'un automate
    /** \brief Récupération de l'identifiant d'un automate.

	\param stateMachine L'automate dont on veut l'identifiant.
	\return L'identifiant associé.
     */
    public: inline Identifier identifier(StateMachine * stateMachine) ;

    // Gestion des signaux
    /** \brief Méthode gérant l'envoi des signaux 

    Elle parcourt la liste des signaux stockes dans m_queuedSignals en les envoyant au sous-contrôleur
    charge de l'exécution de l'automate cible.
    */
    protected: void computeSignals() ;

    /** \brief Méthode gérant la terminaison des automates au pas de temps courant.
     */
    protected: void computeEndedStateMachines() ;

    /** \brief Calcul d'un pas de simulation.

    Un pas de simulation est découpé en plusieurs phases:
    \par 
    1) Envoi des signaux stockes durant le pas de temps précédent
    \par 
    2) Exécution du code a exécuter avant chaque pas de temps pour tous les automates
    \par
    3) Collecte des transitions franchissables pour tous les automates
    \par
    4) Ordonnancement des automates
    \par
    5) Exécution du code de sortie d'état pour tous les automates franchissant une transition
    \par
    6) Exécution du code associe a la transition pour tous les automates franchissant une transition
    \par
    7) Exécution du code d'entrée dans l'état pour tous les automates franchissant une transition
    \par
    8) Exécution du code associe a l'état pour tous les automates.
     */
    public: virtual void compute() ;

    /** \brief Le contrôleur gère-t-il encore des automates ?
     */
    public: inline bool runningStateMachines() const ;

    /** \brief Récupération de l'automate possédant une ressource
	
    \param resource La ressource sur laquelle on cherche l'information
    \return L'automate possédant actuellement cette ressource. NULL si la ressource n'est pas prise par un automate.
     */
    public: inline StateMachine * takenBy(HPTS::Resource * resource) ;

    /** \brief destructeur du controleur
	
    Lors de la destruction du contrôleur, tous les automates encore gérés par se dernier se voient
    envoyer un signal kill pour forcer leur terminaison de manière propre.
     */
    public: virtual ~StateMachineController() ;

    class DestroyCallBack ;
    friend class DestroyCallBack ;

    /** \brief Classe de callBack pour destruction synchrone lors du dereferencement.

    Cette classe est utilisee en combinaison de la classe MapStateMachineId pour detruire
    directement un automate lorsque ce dernier n'est plus reference.
     */
    class DestroyCallBack : public MapStateMachineId::CallBack
    {
      protected: StateMachineController * m_ctrl ;

      /** \brief Constructeur prenant en parametre l'adresse du StateMachineController qui lui est associe. */
      public: DestroyCallBack(StateMachineController * ctrl)
	: m_ctrl(ctrl)
      {}
      
      /** \brief action associee au call back.
	  
      Cette action detruit le sous controlleur associe a l'automate qui n'est plus reference.
       */
      public: virtual void action(int id)
      {	
	delete m_ctrl->m_subControllers[id] ; 
	m_ctrl->m_subControllers[id]=NULL ;
      }

      /** \brief destructeur virtuel. Au cas ou. */
      public: virtual ~DestroyCallBack()
      {}
    } ;

    /** \brief Le call back effectivement utilise par cette instance de controleur. */
    DestroyCallBack m_destroyCallBack ;

    /** \brief Association d'un identifiantàa un automate */
    protected: MapStateMachineId m_stateMachineMap ;
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

